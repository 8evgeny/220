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
    starttime= QDateTime::currentMSecsSinceEpoch();
    qDebug() << "Main thread id: " << QThread::currentThreadId();
    m_main_app = this;
    sm = new SettingsManager( "config.ini", this);
    sm->loadSettings();
    net_conn = new Network(sm, gst_wrp);

    ldc_worker = new LDC20I_Worker(sm->settings_store.ldc_uart_port,
                                  sm->settings_store.ldc_uart_baudrate, this);

    connect(ldc_worker, &LDC20I_Worker::send_LDC_State,
           net_conn, &Network::procLDCstatus, Qt::QueuedConnection);

    connect(&(net_conn->cmd_parser), &CMD_Parser::sig_LDC_CMD,
           ldc_worker, &LDC20I_Worker::proc_LDCSendCmd, Qt::QueuedConnection);

    connect(this, &Main_app::sig_LDC_CloseConnection,
           ldc_worker, &LDC20I_Worker::proc_CtrlCloseConnection, Qt::QueuedConnection);

    QTimer::singleShot(1000, this, [&]()
        {
            qDebug()<<"ldc_worker->proc_CtrlInitConnection()";
            ldc_worker->proc_CtrlInitConnection();
        });//END QTimer::singleShot(1000, this, [&]()

}//END Main_app::Main_app(QCoreApplication *app, QObject *parent)


