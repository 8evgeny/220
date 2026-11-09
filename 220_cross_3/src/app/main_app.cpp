#include "main_app.h"
#include "glib.h"
#include "gst_wrapper.h"
// #include <CL/opencl.hpp>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDateTime>
#include <QEventLoop>
#include <QFile>
#include <QThread>
#include <csignal>
#include <cstdio> // fflush()
#include <fstream>
#include <gst/gst.h>
#include <gst/play/gstplay.h>
#include <gst/tag/tag.h>
#include <qthreadpool.h>
#include <unistd.h> // _exit(), alarm()
#include <vector>

static Main_app *m_main_app;

// ─── Watchdog завершения ────────────────────────────────────────────────────
// Если очистка зависла (GStreamer/v4l2 close() в несъёмном состоянии), процесс
// обязан уйти сам, с кодом 0: иначе systemd/SIGKILL показывает "killed, signal=…"
// и сервис выглядит упавшим. alarm()/SIGALRM + _exit() — async-signal-safe.
static void shutdownDeadlineHandler(int)
{
    _exit(0);
} // shutdownDeadlineHandler

static void armShutdownDeadline(int seconds)
{
    static bool armed = false;
    if (armed)
        return;
    armed = true;
    std::signal(SIGALRM, shutdownDeadlineHandler);
    alarm(static_cast<unsigned>(seconds));
} // armShutdownDeadline

void disconnectAllInThread(QObject *thread)
{
    if (!thread)
        return;

    QObjectList objects = thread->findChildren<QObject *>();
    for (QObject *obj : objects)
    {
        QObject::disconnect(obj, nullptr, nullptr, nullptr);
    } // for (QObject *obj : objects)

    // Also disconnect the thread's own signals
    QObject::disconnect(thread, nullptr, nullptr, nullptr);
} // disconnectAllInThread

void disconnectAll(QObject *obj)
{
    if (!obj)
        return;

    // Отключаем все входящие и исходящие сигналы
    QObject::disconnect(obj, nullptr, nullptr, nullptr);

    // Рекурсивно для дочерних объектов
    for (auto *child : obj->children())
    {
        disconnectAll(child);
    } // for (auto *child : obj->children())
} // disconnectAll

static void handleSignal(int signal)
{
    // Signal handler должен быть async-signal-safe!
    // Используем приём: пишем в self-pipe или используем QTimer::singleShot если поток жив.
    // Для Qt-приложений допустимо через invokeMethod main thread:

    qDebug() << "Handling termination event, signal:" << signal;

    // Откладываем cleanup из контекста signal handler. Один путь и для SIGTERM/SIGINT,
    // и для служебных вызовов (signal < 0 — напр. таймаут инициализации GST): он закрывает
    // UART + lock-файлы и уходит через _exit(0), не заходя в деструкторы worker'ов.
    QTimer::singleShot(0, m_main_app, []()
                       {
                           static bool s_shutdownStarted = false;
                           if (s_shutdownStarted)
                               return; // повторный SIGTERM/SIGINT — не лезем второй раз
                           s_shutdownStarted = true;

                           // Предельный бюджет всего завершения: если какой-то close()/join зависнет,
                           // процесс уходит сам с кодом 0 вместо SIGKILL/SIGABRT от systemd.
                           armShutdownDeadline(15);

#ifdef GYRO_PLATFORM_HOST
                           // GSP: закрываем UART ПЕРВЫМ (как LensJ200/MINI640 — их порты закрылись раньше всех).
                           // Иначе долгая остановка GST/worker'ов может не успеть завершиться до SIGKILL от IDE,
                           // и /var/lock/LCK..ttyXXX останется «висеть» (port not released).
                           m_main_app->deinitGyroPlatform();
#endif

                           emit m_main_app->sig_GST_StopCapture();
                           emit m_main_app->sig_VISCA_DISCONNECT();
                           emit m_main_app->sig_MINI640_CloseConnection();
                           emit m_main_app->sig_LensJ200_CloseConnection();
                           emit m_main_app->sig_LDC_CloseConnection();

                           // Все перечисленные close-слоты подключены как QueuedConnection, а сами driver-worker'ы
                           // (в т.ч. Camera_EV9500M_Worker для TV /dev/ttyS9) живут в ГЛАВНОМ потоке: их слоты
                           // доставляются только когда главный поток возвращается в event loop. Сразу ниже идёт
                           // блокирующий shutdownGracefully() → thread->wait(), который главный event loop НЕ
                           // крутит, а затем _exit(0). Без явной прокачки proc_VISCA_DISCONNECT() не выполнялся и
                           // tty TV-камеры + /var/lock/LCK..ttyXXX оставались заняты. Прокачиваем очередь ЗДЕСЬ —
                           // до блокирующего ожидания GST, чтобы UART освободились как можно раньше.
                           // Безопасно: потенциально опасные GST restart/watchdog-таймеры привязаны к worker-потоку
                           // (this = gst_wrp), а не к главному, поэтому pump трогает только main-thread UART close'ы.
                           QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);

                           // GST: остановка конвейеров/трекера/транспорта и join worker-потока.
                           // Возвращает false, если поток не удалось дождаться — тогда объект НЕ удаляем
                           // (удаление работающего QThread = qFatal → SIGABRT) и уходим через _exit().
                           bool gstJoined = true;
                           if (m_main_app->gst_wrp)
                           {
                               gstJoined = m_main_app->gst_wrp->shutdownGracefully(7000);
                           } // if (m_main_app->gst_wrp)

                           // Дальше объекты намеренно НЕ удаляются: всё внешнее (UART-порты и их lock-файлы,
                           // /dev/video*, pipe fd, сокеты) уже освобождено явными close() выше, остальное
                           // заберёт ядро. Деструкторы worker'ов с потоками — это ровно тот путь, который
                           // раньше давал "QThread: Destroyed while thread is still running" → status=6/ABRT.
                           qDebug() << "UARTs Closed, resources freed; gst threads joined:" << gstJoined;

                           fflush(nullptr); // stderr буферизуется при redirect в journal/файл
                           _exit(0);        // детерминированный выход: без static/global destructors
                       }); // lambda [] ()
} // handleSignal

void handleSignal0()
{
    handleSignal(0);
} // handleSignal0

Main_app::~Main_app()
{

    // free(net_conn);gst_Start_MPEG_TS_Pipeline
    // qDebug()<<"net_conn free";

    // free(lens_j200_wrapper);
    // qDebug()<<"lens_j200_wrapper free";
    // free(cw);
    // qDebug()<<"cw free";
    // QObject::disconnect(this, nullptr, nullptr, nullptr);

#ifdef GYRO_PLATFORM_HOST
    if (m_pGyroPlatform)
    {
        m_pGyroPlatform->deleteLater();
        m_pGyroPlatform = nullptr;
    } // if (m_pGyroPlatform)

#endif
} // Main_app::~Main_app

#include <csignal>

void handleSegfault(int sig)
{
    qDebug() << "Segfault occurred. Signal: " << sig;
    // Print stack trace if possible
    exit(1);
} // handleSegfault

Main_app::Main_app(QCoreApplication *app, QObject *parent)
    : QObject{parent}
#ifdef GYRO_PLATFORM_HOST
      ,
      m_pGyroPlatform{nullptr}
#endif
{
    signal(SIGSEGV, handleSegfault);
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    m_QT_App = app;
    // tv_video_timer.setSingleShot(true);

    starttime = QDateTime::currentMSecsSinceEpoch();

    qDebug() << "Main thread id: " << QThread::currentThreadId();

    m_main_app = this;

    const QString iniPath = QCoreApplication::applicationDirPath() + "/config.ini";

#ifdef GYRO_PLATFORM_USE_DEBUG_CONFIG
    sm = new SettingsManager("/home/khadas/qt_projects/qgst-goen220/config_test_gsp.ini", this);

#else

    sm = new SettingsManager(iniPath, this);
#endif

    sm->loadSettings();

    GstPlay *player;
    qDebug() << "sm->settings_store.ip_host" << sm->settings_store.ip_host;
    // sm->settings_store.ip_dev = "192.168.1.91";
    //  sm->settings_store.ip_host = "192.168.1.18";
    // sm->settings_store.ip_host = "192.168.1.105";
    //   sm->settings_store.ip_host = "192.168.1.118";
    //  //sm->settings_store.ip_host = "192.168.1.125";
    //  sm->settings_store.udp_port_host = 52515;
    //  sm->settings_store.udp_port_dev  = 52516;
    // sm->settings_store.tv_cam_height = 1080;
    // sm->settings_store.tv_cam_width  = 1920;
    // sm->settings_store.tv_cam_timeout = 15000;
    // sm->settings_store.tv_cam_video_device = "/dev/video0";
    // sm->settings_store.tv_cam_uart_port="/dev/ttyS4";
    // sm->settings_store.tv_cam_uart_baudrate=115200;

    // sm->settings_store.ir_cam_height = 512;
    // sm->settings_store.ir_cam_width  = 640;
    // sm->settings_store.ir_cam_timeout = 15000;
    // sm->settings_store.ir_cam_video_device = "/dev/video11";
    // sm->settings_store.ir_cam_uart_port="/dev/ttyS3";

    // sm->settings_store.ir_lens_uart_port = "/dev/ttyCH9344USB6";
    // sm->settings_store.ir_lens_uart_baudrate=19200;

    // sm->settings_store.ldc_uart_port= "/dev/ttyCH9344USB1";
    // sm->settings_store.ldc_uart_baudrate = 115200;

    // sm->writeAllSettings();

    // QTimer::singleShot(18000, this, handleSignal0);

    // QObject::connect(m_QT_App, &QCoreApplication::aboutToQuit, [this]() {
    //     qDebug()<<"APP aboutToQuit";
    //     //handleSignal(0);

    // });

    //  run_opencl();

    // run_GST_Stream(sm->settings_store.ir_cam_video_device, 5001);

    gst_wrp = new GST_Wrapper(sm);

    net_conn = new Network(sm, gst_wrp);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_Switch_Channel, this, &Main_app::proc_Switch_Channel, Qt::QueuedConnection);

    connect(net_conn, &Network::sig_MPEG_TS_Telemetry, gst_wrp, &GST_Wrapper::proc_MPEG_TS_Telemetry, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_Switch_Channel, gst_wrp, &GST_Wrapper::proc_Switch_Channel, Qt::QueuedConnection);

    //   net_conn->initSocket();

    video_init_timer.start(sm->settings_store.tv_cam_timeout);

    connect(&this->video_init_timer, &QTimer::timeout,
            this, &Main_app::proc_video_init_timeout, Qt::QueuedConnection);

    connect(gst_wrp, &GST_Wrapper::sig_STREAM_RDY,
            this, &Main_app::proc_GST_RDY, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_TrackingSetTargetObject,
            gst_wrp, &GST_Wrapper::proc_setTargetObject, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_TrackingSetStopTracking,
            gst_wrp, &GST_Wrapper::proc_setStopTracking, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_TrackerChannelChanged,
            net_conn, &Network::procSetTrackerChannel, Qt::QueuedConnection);

    ldc_worker = new LDC20I_Worker(sm->settings_store.ldc_uart_port, sm->settings_store.ldc_uart_baudrate, this);

    connect(gst_wrp, &GST_Wrapper::sig_STREAM_RDY, this, &Main_app::proc_GST_InitReady, Qt::QueuedConnection);

    connect(gst_wrp, &GST_Wrapper::sig_rdyActiveLockTracking, net_conn, &Network::procUpdateActiveLockTracking, Qt::QueuedConnection);

    connect(gst_wrp, &GST_Wrapper::sig_rdyOutObject, net_conn, &Network::procUpdateTrackerObject, Qt::QueuedConnection);

    connect(this, &Main_app::sig_GST_StopCapture, gst_wrp, &GST_Wrapper::proc_stopCapture);

    // ─── EV9500M (UART-контроллер TV-камеры) ───────────────────────────────
    // Подключение стартует ТОЛЬКО когда MPEG-TS пайп поднят с реальной TV-камерой
    // (GST_Wrapper::sig_RealTvCameraUp). Регистрация — синхронно в конструкторе,
    // чтобы не пропустить сигнал, если пайп поднимется быстро. В videotestsrc-fallback
    // сигнал не приходит → UART не трогаем (реальной камеры нет).
    if (sm->settings_store.tv_cam_en)
    {
        cw = new Camera_EV9500M_Worker();

        connect(&net_conn->cmd_parser, &CMD_Parser::sig_VISCA_CMD, cw, &Camera_EV9500M_Worker::proc_VISCA_CMD, Qt::QueuedConnection);

        connect(this, &Main_app::sig_VISCA_CMD, cw, &Camera_EV9500M_Worker::proc_VISCA_CMD, Qt::QueuedConnection);

        connect(this, &Main_app::sig_VISCA_CONNECT, cw, &Camera_EV9500M_Worker::proc_VISCA_CONNECT, Qt::QueuedConnection);

        connect(this, &Main_app::sig_VISCA_DISCONNECT, cw, &Camera_EV9500M_Worker::proc_VISCA_DISCONNECT, Qt::QueuedConnection);

        connect(cw, &Camera_EV9500M_Worker::sig_sendStatus, net_conn, &Network::procVICAPstatus, Qt::QueuedConnection);

        cw->cameraInit();

        // Гейт: реальная TV-камера подняла пайп → разрешаем подключение EV9500M.

        connect(gst_wrp, &GST_Wrapper::sig_RealTvCameraUp,
                this, &Main_app::proc_ev9500mGate, Qt::QueuedConnection);

        // 10 неудач UART в режиме реальной камеры → рестарт MPEG-TS пайпа через фикс. 5 с.

        connect(cw, &Camera_EV9500M_Worker::sig_connected,
                this, &Main_app::proc_ev9500mConnected, Qt::QueuedConnection);

        m_ev9500mRecoveryTimer.setSingleShot(true);

        connect(&m_ev9500mRecoveryTimer, &QTimer::timeout,
                this, &Main_app::proc_ev9500mRecoveryTimeout, Qt::QueuedConnection);

        // Рестарт MPEG-TS пайпа: QueuedConnection → слот исполнится в worker-потоке GST_Wrapper.

        connect(this, &Main_app::sig_GST_RestartMpegTs,
                gst_wrp, &GST_Wrapper::restartMpegTsPipeline, Qt::QueuedConnection);

    } // if (sm->settings_store.tv_cam_en)

    isTvUartInit = false;

    connect(ldc_worker, &LDC20I_Worker::send_LDC_State,
            net_conn, &Network::procLDCstatus, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_LDC_CMD,
            ldc_worker, &LDC20I_Worker::proc_LDCSendCmd, Qt::QueuedConnection);

    // Закрытие UART LDC при завершении: сигнал из Main_app -> слот воркера в ЕГО потоке.
    // Раньше соединение было закомментировано и ссылалось на несуществующий slot_CloseConnection,
    // из-за чего emit sig_LDC_CloseConnection() уходил в никуда и порт/lock не освобождались.

    connect(this, &Main_app::sig_LDC_CloseConnection,
            ldc_worker, &LDC20I_Worker::proc_CtrlCloseConnection, Qt::QueuedConnection);

    mini640_worker = new MINI640_Worker(sm->settings_store.ir_cam_uart_port, 9600,
                                        sm->settings_store.ir_cam_period,
                                        sm->settings_store.ir_cam_exp,
                                        sm->settings_store.ir_cam_gpol, this);
    if (sm->settings_store.ir_cam_en)
    {

        connect(mini640_worker, &MINI640_Worker::recv_MINI640_State, net_conn, &Network::proc_MINI640_State, Qt::QueuedConnection);

        connect(this, &Main_app::sig_MINI640_InitConnection, mini640_worker, &MINI640_Worker::proc_CtrlInitConnection, Qt::QueuedConnection);

        connect(this, &Main_app::sig_MINI640_CloseConnection, mini640_worker, &MINI640_Worker::proc_CtrlCloseConnection, Qt::QueuedConnection);

        connect(mini640_worker, &MINI640_Worker::sig_READY, this, &Main_app::proc_MINI640_InitReady, Qt::QueuedConnection);

    } // if (sm->settings_store.ir_cam_en)

    lens_j200_wrapper = new LensJ200Wrapper(sm->settings_store.ir_lens_uart_port, sm->settings_store.ir_lens_uart_baudrate);

    connect(lens_j200_wrapper, &LensJ200Wrapper::sig_ReportReady, net_conn, &Network::procJ200status, Qt::QueuedConnection);

    connect(&(net_conn->cmd_parser), &CMD_Parser::sig_J200_CMD, lens_j200_wrapper, &LensJ200Wrapper::proc_SendCMD, Qt::QueuedConnection);

    connect(&(net_conn->cmd_parser), &CMD_Parser::sig_MINI640_CMD, mini640_worker, &MINI640_Worker::proc_MINI640_ExecCmd, Qt::QueuedConnection);

    connect(this, &Main_app::sig_LensJ200_CloseConnection, lens_j200_wrapper, &LensJ200Wrapper::proc_CtrlCloseConnection, Qt::QueuedConnection);

    // QTimer::singleShot(100, this, [&](){

    //  connect(mini640_worker, &MINI640_Worker::recv_MINI640_State, net_conn, &Network::proc_MINI640_State , Qt::QueuedConnection);
    //  connect(this, &Main_app::sig_MINI640_InitConnection, mini640_worker, &MINI640_Worker::proc_CtrlInitConnection , Qt::QueuedConnection);
    //  connect(this, &Main_app::sig_MINI640_CloseConnection, mini640_worker, &MINI640_Worker::proc_CtrlCloseConnection , Qt::QueuedConnection);
    //  connect(mini640_worker, &MINI640_Worker::sig_READY, this, &Main_app::proc_MINI640_InitReady, Qt::QueuedConnection);
    // emit sig_MINI640_InitConnection();

    // });

    QTimer::singleShot(1000, this, [this]()
                       {
                           qDebug() << "GST PIPE STARTED";

                           // Безопасная инициализация LDC через invokeMethod (worker может ещё не быть готов)
                           // ldc_worker->initConnection(sm->settings_store.ldc_uart_port,
                           //                         sm->settings_store.ldc_uart_baudrate);
                           ldc_worker->proc_CtrlInitConnection();

                           if (sm->settings_store.ir_cam_en)
                               emit sig_MINI640_InitConnection();
                       }); // lambda [this] ()

    // Подключение EV9500M (UART TV-камеры) запускается по GST_Wrapper::sig_RealTvCameraUp
    // — см. блок инициализации cw выше (регистрируется синхронно, чтобы не пропустить
    // сигнал, если пайп с реальной камерой поднимется быстрее отложенного singleShot).

#ifdef GYRO_PLATFORM_HOST
    qDebug() << "GSP: Init";
    m_pGyroPlatform = new Goen220GyroPlatformHost();
    m_pGyroPlatformHandler = new GyroPlatformHandler(m_pGyroPlatform);
    initGyroPlatform();

#endif

} // Main_app::Main_app

void Main_app::proc_GST_InitReady()
{
    //   qDebug()<<"GST PIPE STARTED";

    //  dbus_wrapper.dbus_CommInit();

} // Main_app::proc_GST_InitReady

void Main_app::proc_GST_RDY()
{
    video_init_timer.stop();
} // Main_app::proc_GST_RDY

void Main_app::proc_MINI640_InitReady()
{
    Q_ASSERT(mini640_worker != nullptr);
    Q_ASSERT(net_conn != nullptr);

    qDebug() << " 640 init ok";
    //  emit sig_MINI640_InitConnection();
    // mini640_worker->proc_CtrlInitConnection();
} // Main_app::proc_MINI640_InitReady

void Main_app::proc_Switch_Channel(quint32 channel, quint8 en)
{

    if (channel == 0x00) // TV
    {
        //        tv_v4l2.enableFrameRequest(en);

        //        ir_v4l2.enableFrameRequest(!en);
    } // if (channel == 0x00)
    if (channel == 0x03) // IR
    {
        //        tv_v4l2.enableFrameRequest(!en);
        //        ir_v4l2.enableFrameRequest(en);
    } // if (channel == 0x03)
} // Main_app::proc_Switch_Channel

#ifdef GYRO_PLATFORM_HOST

void Main_app::initGyroPlatform()
{
    if (m_pGyroPlatform == nullptr || m_pGyroPlatformHandler == nullptr || net_conn == nullptr || sm == nullptr)
        return;

    m_pGyroPlatform->setHostId(0x00);
    m_pGyroPlatform->setDeviceId(sm->settings_store.m_gyroPlatform_deviceId); //  0x01

    qDebug() << m_pGyroPlatform->getSerialPorts();

    // m_pGyroPlatform->setSerialPortName( QString::fromUtf8( "ttyCH9344USB2"));   //2
    m_pGyroPlatform->setSerialPortName(sm->settings_store.m_gyroPlatform_uartPort);

    qDebug() << Q_FUNC_INFO << "****************************************************************" << "uartPort=" << sm->settings_store.m_gyroPlatform_uartPort << "deviceId=" << sm->settings_store.m_gyroPlatform_deviceId << "uartDebug=" << sm->settings_store.m_gyroPlatform_uartDebug;

    QTimer::singleShot(7000, this, [&]() //  4000
                       {
                           m_pGyroPlatform->connect(true);
                       }); // lambda [&] ()

    m_pGyroPlatformHandler->enableCompensateGyroDrift(sm->settings_store.m_gyroPlatform_compensateGyroDrift);
    m_pGyroPlatformHandler->setGyroDrift(sm->settings_store.m_gyroPlatform_yawGyroDrift,
                                         sm->settings_store.m_gyroPlatform_pitchGyroDrift);
    // Калибровка (saveCalibrationToFile) должна писаться в ТОТ ЖЕ конфиг, что читает
    // SettingsManager. Иначе после перезапуска значения «скачут» (чтение из одного
    // файла, запись в applicationDirPath()/config.ini).
    m_pGyroPlatformHandler->setSettingsFilePath(sm->settingsFilePath());

    connect(m_pGyroPlatform, &Goen220GyroPlatformHost::stateConnectToRemoteDeviceChanged, net_conn, &Network::procGspStatus, Qt::QueuedConnection);

    connect(m_pGyroPlatformHandler, &GyroPlatformHandler::motorAngleChanged, net_conn, &Network::procGspMotorAngle, Qt::QueuedConnection);

    connect(m_pGyroPlatformHandler, &GyroPlatformHandler::motorSpeedChanged, net_conn, &Network::procGspMotorSpeed, Qt::QueuedConnection);

    connect(m_pGyroPlatformHandler, &GyroPlatformHandler::positionModeChanged, net_conn, &Network::procGspPositionMode, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setDrivePositionMode, m_pGyroPlatformHandler, &GyroPlatformHandler::setDrivePositionMode, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setControlSpeed, m_pGyroPlatformHandler, &GyroPlatformHandler::setControlSpeed, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setControlPositionRel, m_pGyroPlatformHandler, &GyroPlatformHandler::setControlPositionRel, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setControlPositionAbs, m_pGyroPlatformHandler, &GyroPlatformHandler::setControlPositionAbs, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setStabilization, m_pGyroPlatformHandler, &GyroPlatformHandler::setStabilization, Qt::QueuedConnection);

#ifdef GYRO_PLATFORM_HOST

    connect(this, &Main_app::sig_GSP_CloseConnection, this, &Main_app::proc_deinitGyroPlatform, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_turnOffMotors, m_pGyroPlatformHandler, &GyroPlatformHandler::turnOffMotors, Qt::QueuedConnection);

    //  Tracking:

    connect(gst_wrp, &GST_Wrapper::sig_rdyActiveLockTracking, m_pGyroPlatformHandler, &GyroPlatformHandler::setActiveLockTracking, Qt::QueuedConnection);

    connect(gst_wrp, &GST_Wrapper::sig_rdyOutObject, m_pGyroPlatformHandler, &GyroPlatformHandler::updateTrackerObject, Qt::QueuedConnection);

    connect(net_conn, &Network::tvFieldChanged, m_pGyroPlatformHandler, &GyroPlatformHandler::setTvField, Qt::QueuedConnection);

    connect(net_conn, &Network::sig_irFieldChanged,
            m_pGyroPlatformHandler, &GyroPlatformHandler::setIrField, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_TrackerChannelChanged,
            m_pGyroPlatformHandler, &GyroPlatformHandler::procSetTrackerChannel, Qt::QueuedConnection);

    // Калибровка дрейфа гироплатформы

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setGyroDriftCalibration,
            m_pGyroPlatformHandler, &GyroPlatformHandler::startGyroDriftCalibration,
            Qt::QueuedConnection);

    // Сигнал завершения калибровки → обновляем телеметрию / логируем

    connect(m_pGyroPlatformHandler, &GyroPlatformHandler::calibrationCompleted, this, [this](const float yawDrift, const float pitchDrift, const bool success)
            {
                if (success)
                {
                    qDebug() << "GYRO CALIBRATION OK: yaw=" << yawDrift << "pitch=" << pitchDrift;
                    // Сохраняем ДРЕЙФ + ВКЛЮЧАЕМ компенсацию в тот же конфиг, что читаем,
                    // и синхронизируем settings_store. Handler пишет Yaw/Pitch, но НЕ флаг
                    // Compensate — без этого после рестарта компенсация была бы выключена.
                    sm->saveGyroDrift(yawDrift, pitchDrift, true);
                } // if (success)
                else
                {
                    qWarning() << "GYRO CALIBRATION FAILED";
                } // else
            }, // lambda [this] (const float yawDrift, const float pitchDrift, const bool success)
            Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setGyroDrift, m_pGyroPlatformHandler, [this](const float yawDrift, const float pitchDrift)
            {
                qDebug() << "GYRO DRIFT MANUAL SET: yaw=" << yawDrift
                         << "pitch=" << pitchDrift;

                // Применяем к хендлеру
                m_pGyroPlatformHandler->setGyroDrift(yawDrift, pitchDrift);

                // Включаем компенсацию если ещё не включена
                m_pGyroPlatformHandler->enableCompensateGyroDrift(true);

                // Сохраняем в ТОТ ЖЕ конфиг, что читает SettingsManager (+ память).
                // Раньше путь хардкодился (applicationDirPath()/config.ini) и мог
                // расходиться с конфигом загрузки → значения «скакали» после рестарта.
                sm->saveGyroDrift(yawDrift, pitchDrift, true);
            }, // lambda [this] (const float yawDrift, const float pitchDrift)
            Qt::QueuedConnection);

    // Установка нового нуля ГСП (сохранение offset в конфиг)

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setNewZeroAngleX,
            net_conn, &Network::procSetNewZeroAngleX, Qt::QueuedConnection);

    connect(&net_conn->cmd_parser, &CMD_Parser::sig_setNewZeroAngleZ,
            net_conn, &Network::procSetNewZeroAngleZ, Qt::QueuedConnection);

    m_pGyroPlatform->setTimeoutWaitingResponse(100);
    m_pGyroPlatform->setTimeBetweenStatusRequests(100);

    //  Для отладки:

    if (sm->settings_store.m_gyroPlatform_uartDebug)
    {
        m_pGyroPlatform->setTimeoutWaitingResponse(500);
        m_pGyroPlatform->setTimeBetweenStatusRequests(1000);

        m_pGyroPlatform->setPrintTxRawDataSerialPort(true);
        m_pGyroPlatform->setPrintRxRawDataSerialPort(true);

        connect(m_pGyroPlatform, &Goen220GyroPlatformHost::txRawDataSerialPortText, this,
                [this](const QString &text)
                {
                    qDebug() << "GSP " << text;
                }); // lambda [this] (const QString &text)

        connect(m_pGyroPlatform, &Goen220GyroPlatformHost::rxRawDataSerialPortText, this,
                [this](const QString &text)
                {
                    qDebug() << "GSP " << text;
                }); // lambda [this] (const QString &text)

    } // if (sm->settings_store.m_gyroPlatform_uartDebug)
#endif
} // Main_app::initGyroPlatform

#endif

void Main_app::proc_deinitGyroPlatform()
{
#ifdef GYRO_PLATFORM_HOST
    if (m_pGyroPlatform == nullptr)
        return;
    m_pGyroPlatform->connect(false);
    qDebug() << "GSP DeInit success";
#endif
} // Main_app::proc_deinitGyroPlatform

// Единая точка завершения драйвера гироплатформы: закрывает UART и детерминированно
// освобождает GSP-объекты. Вся логика — здесь, main_app взаимодействует с драйвером
// через один публичный метод (принцип «единого окна», как в initGyroPlatform).
// Порядок важен:
//   1) Синхронно инициируем закрытие порта через Goen220GyroPlatformHost::connect(false).
//   2) Прокручиваем очередь событий, чтобы queued цепочка closeToDevice →
//      SerialPort::connect(false) → QSerialPort::close() реально выполнилась, пока
//      host/port ещё живы.
//   3) Удаляем handler (держит сырой указатель на host — раньше хоста).
//   4) Удаляем host; его деструктор и Goen220GyroPlatformHostPrivate используют
//      deleteLater(), поэтому ещё раз дожимаем очередь.
void Main_app::deinitGyroPlatform()
{
#ifdef GYRO_PLATFORM_HOST
    if (m_pGyroPlatform == nullptr && m_pGyroPlatformHandler == nullptr)
        return;

    proc_deinitGyroPlatform(); // close UART (queued → SerialPort::connect(false))
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    if (m_pGyroPlatformHandler)
    { // handler→host, удаляем handler раньше
        delete m_pGyroPlatformHandler;
        m_pGyroPlatformHandler = nullptr;
    } // if (m_pGyroPlatformHandler)
    if (m_pGyroPlatform)
    { // хост: внутри deleteLater() для Private/SerialPort
        delete m_pGyroPlatform;
        m_pGyroPlatform = nullptr;
    } // if (m_pGyroPlatform)

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif
} // Main_app::deinitGyroPlatform

void Main_app::proc_video_init_timeout()
{
    video_init_timer.stop();

    qDebug() << "GST subsystem timeout:" << QDateTime::currentMSecsSinceEpoch() - starttime;

    // Синтетический режим: реальные камеры недоступны, обе ветки encode подняты
    // на videotestsrc — видеотракт полностью рабочий, завершать процесс по watchdog
    // нельзя (в этом режиме сигнал готовности может не дойти вовремя, т.к. главный
    // поток занят инициализацией отсутствующего железа: EV9500M/serial/gyro).
    if (gst_wrp && gst_wrp->isSyntheticVideoMode())
    {
        qWarning() << "[GST] video_init timeout IGNORED: synthetic videotestsrc mode (no cameras)";
        return;
    } // if (gst_wrp && gst_wrp->isSyntheticVideoMode())

    handleSignal(-100);
    // tv_v4l2.stopCapture();
    // V4L2::sleep(1);
    // tv_v4l2.freeFrameBuffers();

    // tv_v4l2.close();
    // V4L2::sleep(1);

    // tv_v4l2.open(sm->settings_store.tv_cam_video_device, 0);
    // tv_v4l2.startCapture(tv_imageFormat);
    // video_timer.start(sm->settings_store.tv_cam_timeout);
    // isTvUartInit=false;

} // Main_app::proc_video_init_timeout

// ─── EV9500M (UART TV-камеры): гейт запуска + восстановление ──────────────
// Реальная TV-камера подняла MPEG-TS пайп → разрешаем подключение EV9500M.
void Main_app::proc_ev9500mGate()
{
    if (!sm->settings_store.tv_cam_en)
        return;
    if (m_ev9500mConnectInFlight)
        return;
    m_ev9500mConnectInFlight = true;
    qInfo() << "[EV9500M] real TV camera pipeline up — start VISCA connect";
    emit sig_VISCA_CONNECT(sm->settings_store.tv_cam_uart_port,
                           sm->settings_store.tv_cam_uart_baudrate);
} // Main_app::proc_ev9500mGate

// Результат подключения EV9500M. При 10 неудачах в режиме реальной камеры заводим
// recovery (рестарт пайпа через фикс. 5 с). В videotestsrc-fallback рестарт не нужен.
void Main_app::proc_ev9500mConnected(bool ok)
{
    m_ev9500mConnectInFlight = false;
    if (ok)
        return;
    if (gst_wrp && gst_wrp->isSyntheticVideoMode())
    {
        qWarning() << "[EV9500M] VISCA connect failed but synthetic mode — no pipeline restart";
        return;
    } // if (gst_wrp && gst_wrp->isSyntheticVideoMode())
    qWarning() << "[EV9500M] VISCA connect failed (real camera) — restart MPEG-TS pipeline in 5s";
    m_ev9500mRecoveryTimer.start(5000);
} // Main_app::proc_ev9500mConnected

// Таймаут recovery → рестарт MPEG-TS пайпа (перезахват ISP/v4l2 может «оживить»
// камеру). После рестарта снова придёт sig_RealTvCameraUp → повтор подключения.
void Main_app::proc_ev9500mRecoveryTimeout()
{
    if (!sm->settings_store.tv_cam_en)
        return;
    if (!gst_wrp)
        return;
    qWarning() << "[EV9500M] restarting MPEG-TS pipeline after UART failure";
    emit sig_GST_RestartMpegTs();
} // Main_app::proc_ev9500mRecoveryTimeout

// void Main_app::ir_video_timeout()
// {
//     ir_video_timer.stop();

//     qDebug() << "TV Camera frame timeout:" << QDateTime::currentMSecsSinceEpoch()-starttime;
//  /*   ir_v4l2.stopCapture();
//     V4L2::sleep(1);
//     ir_v4l2.freeFrameBuffers();

//     ir_v4l2.close();
//     V4L2::sleep(1);

//     ir_v4l2.open(sm->settings_store.ir_cam_video_device, 0);
//     ir_v4l2.startCapture(ir_imageFormat);*/
//     ir_video_timer.start(sm->settings_store.ir_cam_timeout);
//     isIRUartInit=false;

// }

// void Main_app::tv_video_reset_wd()
// {
//     tv_video_timer.setInterval(sm->settings_store.tv_cam_timeout);

//     if(!isTvUartInit){
//         sig_VISCA_CONNECT( sm->settings_store.tv_cam_uart_port, 115200);
//     }
//     isTvUartInit=true;

// }

// void Main_app::ir_video_reset_wd()
// {
//     ir_video_timer.setInterval(sm->settings_store.ir_cam_timeout);

//     if(!isIRUartInit){

//        // sig_VISCA_CONNECT("/dev/ttyS3", 115200);
//     }
//     isIRUartInit=true;

// }

// static cl::Program* program;
// void Main_app::run_opencl()
// {
//     std::vector<cl::Platform> platforms;
//     cl::Platform::get(&platforms);

//     if (platforms.empty()) {
//         qDebug() << "No OpenCL platforms found!";
//         return; //-1;
//     }

//     cl_int err;

//     // Вывод информации о платформах
//     for (const auto& platform : platforms) {
//         std::string platformName;
//         platform.getInfo(CL_PLATFORM_NAME, &platformName);
//         qDebug() << "Platform: " << platformName;
//     }

//     // Выбор первой платформы
//     cl::Platform platform = platforms[0]; // Выбор первой платформы

//     // Получение всех устройств на платформе
//     std::vector<cl::Device> devices;
//     platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

//     // Проверка наличия устройств
//     if (devices.empty()) {
//         qDebug() << "No devices found on platform";
//         return; // 1;
//     }

//     cl::Device device = devices[0]; // Выбор первого устройства
//     std::string deviceName;
//     err = device.getInfo(CL_DEVICE_NAME, &deviceName);
//     if (err != CL_SUCCESS) {
//         qDebug() << "Error getting device info: ";
//         return; // -1;
//     }
//     qDebug() << "Device: " << deviceName;

//     // Создание контекста и командной очереди
//     cl::Context context(device);
//     cl::CommandQueue queue(context, device);

//     QString appDirPath = QCoreApplication::applicationDirPath() + "/kernel.cl";
//     qDebug() << "Путь к директории приложения:" << appDirPath;

//     std::ifstream kernelFile(appDirPath.toStdString());
//     if (!kernelFile.is_open()) {
//         qDebug() << "Не удалось открыть файл kernel.cl";
//         return;
//     }

//     std::string kernelCode((std::istreambuf_iterator<char>(kernelFile)), std::istreambuf_iterator<char>());
//     //qDebug()<<kernelCode;

//     cl::Program program(context, kernelCode);
//     program.build({device});

// }
