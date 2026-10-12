#include "main_app.h"
#include "glib.h"
#include "gst_wrapper.h"
//#include <CL/opencl.hpp>
#include <QCoreApplication>
#include <QFile>
#include <vector>
#include <fstream>
#include <QDateTime>
#include <QThread>
#include <gst/gst.h>
#include <gst/tag/tag.h>
#include <gst/play/gstplay.h>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <csignal>
#include  <qthreadpool.h>

static Main_app *m_main_app;

void disconnectAllInThread(QObject* thread) {
    if (!thread) return;

    QObjectList objects = thread->findChildren<QObject*>();
    for (QObject* obj : objects) {
        QObject::disconnect(obj, nullptr, nullptr, nullptr);
    }

    // Also disconnect the thread's own signals
    QObject::disconnect(thread, nullptr, nullptr, nullptr);
}


void disconnectAll(QObject* obj) {
    if (!obj) return;

    // Отключаем все входящие и исходящие сигналы
    QObject::disconnect(obj, nullptr, nullptr, nullptr);

    // Рекурсивно для дочерних объектов
    for (auto* child : obj->children()) {
        disconnectAll(child);
    }
}

void handleSignal(int signal)
{
    qDebug()<<"Handling termination event";
    if(signal <0) goto exit;

    emit m_main_app->sig_GST_StopCapture();
    emit m_main_app->sig_VISCA_DISCONNECT();

    emit m_main_app->sig_MINI640_CloseConnection();
    emit m_main_app->sig_LensJ200_CloseConnection();
    emit m_main_app->sig_LDC_CloseConnection();
#ifdef GYRO_PLATFORM_HOST
    emit m_main_app->sig_GSP_CloseConnection();
#endif
    delete(m_main_app->gst_wrp);
    qDebug()<<"gst free";

   // QThread::sleep(2);
    delete(m_main_app->cw);

     qDebug()<<"cw free";

    delete(m_main_app->mini640_worker);

    qDebug()<<"mini640_worker free";


    delete(m_main_app->lens_j200_wrapper);
    qDebug()<<"j200 free";

     delete(m_main_app->ldc_worker);
    qDebug()<<"ldc free";


     delete(m_main_app->net_conn);
    qDebug()<<"netcon free";
  //   sleep(5);



    qDebug()<<"UARTs Closed";
exit:
    QCoreApplication::quit();
}

void handleSignal0()
{
    handleSignal(0);
}

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
    if( m_pGyroPlatform)
    {
        m_pGyroPlatform->deleteLater();
        m_pGyroPlatform = nullptr;
    }

#endif
}

#include <csignal>

void handleSegfault(int sig) {
    qDebug() << "Segfault occurred. Signal: " << sig;
    // Print stack trace if possible
    exit(1);
}

Main_app::Main_app(QCoreApplication *app, QObject *parent)
    : QObject{parent}
#ifdef GYRO_PLATFORM_HOST
    , m_pGyroPlatform{ nullptr}
#endif
{
    signal(SIGSEGV, handleSegfault);
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    m_QT_App = app;
   // tv_video_timer.setSingleShot(true);

    starttime= QDateTime::currentMSecsSinceEpoch();

    qDebug() << "Main thread id: " << QThread::currentThreadId();

    m_main_app = this;

#ifdef GYRO_PLATFORM_USE_DEBUG_CONFIG
    sm = new SettingsManager( "/home/khadas/qt_projects/qgst-goen220/config_test_gsp.ini", this);

#else

    sm = new SettingsManager( "config.ini", this);
#endif

    sm->loadSettings();

    net_conn = new Network(sm, gst_wrp);

   ldc_worker = new LDC20I_Worker(sm->settings_store.ldc_uart_port,
                                  sm->settings_store.ldc_uart_baudrate, this);

   isTvUartInit=false;

    connect(ldc_worker, &LDC20I_Worker::send_LDC_State,
           net_conn, &Network::procLDCstatus, Qt::QueuedConnection);

    connect(&(net_conn->cmd_parser), &CMD_Parser::sig_LDC_CMD,
           ldc_worker, &LDC20I_Worker::proc_LDCSendCmd, Qt::QueuedConnection);

    connect(this, &Main_app::sig_LDC_CloseConnection,
           ldc_worker, &LDC20I_Worker::proc_CtrlCloseConnection, Qt::QueuedConnection);

    QTimer::singleShot(1000, this, [&]()
        {
            qDebug()<<"GST PIPE STARTED";
            ldc_worker->proc_CtrlInitConnection();
        });//END QTimer::singleShot(1000, this, [&]()

}//END Main_app::Main_app(QCoreApplication *app, QObject *parent)

void Main_app::proc_GST_InitReady()
{
 //   qDebug()<<"GST PIPE STARTED";

  //  dbus_wrapper.dbus_CommInit();

}

void Main_app::proc_GST_RDY()
{
    video_init_timer.stop();
}


void Main_app::proc_MINI640_InitReady()
{
    Q_ASSERT(mini640_worker!=nullptr);
    Q_ASSERT(net_conn!=nullptr);

    qDebug()<<" 640 init ok";
  //  emit sig_MINI640_InitConnection();
    //mini640_worker->proc_CtrlInitConnection();
}




void Main_app::proc_Switch_Channel(quint32 channel, quint8 en)
{

    if(channel == 0x00) // TV
    {
//        tv_v4l2.enableFrameRequest(en);

//        ir_v4l2.enableFrameRequest(!en);
    }
    if(channel == 0x03) // IR
    {
//        tv_v4l2.enableFrameRequest(!en);
//        ir_v4l2.enableFrameRequest(en);
    }
}


#ifdef GYRO_PLATFORM_HOST

    void Main_app::initGyroPlatform()
    {
        if( m_pGyroPlatform == nullptr || m_pGyroPlatformHandler == nullptr || net_conn == nullptr || sm == nullptr)
            return;

        m_pGyroPlatform->setHostId( 0x00);
        m_pGyroPlatform->setDeviceId( sm->settings_store.m_gyroPlatform_deviceId);    //  0x01

        qDebug() << m_pGyroPlatform->getSerialPorts();

        // m_pGyroPlatform->setSerialPortName( QString::fromUtf8( "ttyCH9344USB2"));   //2
        m_pGyroPlatform->setSerialPortName( sm->settings_store.m_gyroPlatform_uartPort);

        qDebug() << Q_FUNC_INFO <<
            "****************************************************************" <<
            "uartPort=" << sm->settings_store.m_gyroPlatform_uartPort <<
            "deviceId=" << sm->settings_store.m_gyroPlatform_deviceId <<
            "uartDebug=" << sm->settings_store.m_gyroPlatform_uartDebug;

        QTimer::singleShot( 7000, this, [&]()    //  4000
            {
                m_pGyroPlatform->connect( true);
            }
        );

        m_pGyroPlatformHandler->enableCompensateGyroDrift( sm->settings_store.m_gyroPlatform_compensateGyroDrift);
        m_pGyroPlatformHandler->setGyroDrift(   sm->settings_store.m_gyroPlatform_yawGyroDrift,
                                                sm->settings_store.m_gyroPlatform_pitchGyroDrift);


        connect( m_pGyroPlatform, &Goen220GyroPlatformHost::stateConnectToRemoteDeviceChanged, net_conn, &Network::procGspStatus, Qt::QueuedConnection);
        connect( m_pGyroPlatformHandler, &GyroPlatformHandler::motorAngleChanged, net_conn, &Network::procGspMotorAngle, Qt::QueuedConnection);
        connect( m_pGyroPlatformHandler, &GyroPlatformHandler::motorSpeedChanged, net_conn, &Network::procGspMotorSpeed, Qt::QueuedConnection);

        connect( m_pGyroPlatformHandler, &GyroPlatformHandler::positionModeChanged, net_conn, &Network::procGspPositionMode, Qt::QueuedConnection);
        connect( &net_conn->cmd_parser, &CMD_Parser::sig_setDrivePositionMode, m_pGyroPlatformHandler, &GyroPlatformHandler::setDrivePositionMode, Qt::QueuedConnection);

        connect( &net_conn->cmd_parser, &CMD_Parser::sig_setControlSpeed, m_pGyroPlatformHandler, &GyroPlatformHandler::setControlSpeed, Qt::QueuedConnection);
        connect( &net_conn->cmd_parser, &CMD_Parser::sig_setControlPositionRel, m_pGyroPlatformHandler, &GyroPlatformHandler::setControlPositionRel, Qt::QueuedConnection);
        connect( &net_conn->cmd_parser, &CMD_Parser::sig_setControlPositionAbs, m_pGyroPlatformHandler, &GyroPlatformHandler::setControlPositionAbs, Qt::QueuedConnection);

        connect( &net_conn->cmd_parser, &CMD_Parser::sig_setStabilization, m_pGyroPlatformHandler, &GyroPlatformHandler::setStabilization, Qt::QueuedConnection);

       #ifdef GYRO_PLATFORM_HOST
        connect(this, &Main_app::sig_GSP_CloseConnection, this,   &Main_app::proc_deinitGyroPlatform, Qt::QueuedConnection);
        connect( &net_conn->cmd_parser, &CMD_Parser::sig_turnOffMotors, m_pGyroPlatformHandler, &GyroPlatformHandler::turnOffMotors, Qt::QueuedConnection);

        //  Tracking:
        connect( gst_wrp, &GST_Wrapper::sig_rdyActiveLockTracking, m_pGyroPlatformHandler, &GyroPlatformHandler::setActiveLockTracking, Qt::QueuedConnection);
        connect( gst_wrp, &GST_Wrapper::sig_rdyOutObject, m_pGyroPlatformHandler, &GyroPlatformHandler::updateTrackerObject, Qt::QueuedConnection);

        connect( net_conn, &Network::tvFieldChanged, m_pGyroPlatformHandler, &GyroPlatformHandler::setTvField, Qt::QueuedConnection);

        m_pGyroPlatform->setTimeoutWaitingResponse( 100);
        m_pGyroPlatform->setTimeBetweenStatusRequests( 100);

        //  Для отладки:

        if( sm->settings_store.m_gyroPlatform_uartDebug)
        {
            m_pGyroPlatform->setTimeoutWaitingResponse( 500);
            m_pGyroPlatform->setTimeBetweenStatusRequests( 1000);

            m_pGyroPlatform->setPrintTxRawDataSerialPort( true);
            m_pGyroPlatform->setPrintRxRawDataSerialPort( true);

            connect( m_pGyroPlatform, &Goen220GyroPlatformHost::txRawDataSerialPortText, this,
                [this]( const QString &text)
                {
                    qDebug() << "GSP " << text;
                }
            );

            connect( m_pGyroPlatform, &Goen220GyroPlatformHost::rxRawDataSerialPortText, this,
                    [this]( const QString &text)
                    {
                        qDebug() << "GSP " << text;
                    }
            );
        }
        #endif
    }



#endif
    void Main_app::proc_deinitGyroPlatform()
    {
#ifdef GYRO_PLATFORM_HOST
        m_pGyroPlatform->connect( false);
        qDebug() << "GSP DeInit success";
#endif
    }

void Main_app::proc_video_init_timeout()
{
    video_init_timer.stop();

    qDebug() << "GST subsystem timeout:" << QDateTime::currentMSecsSinceEpoch()-starttime;

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


}

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
