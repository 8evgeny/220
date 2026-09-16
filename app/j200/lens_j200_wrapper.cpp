#include "lens_j200_wrapper.h"
#include <qdebug.h>

LensJ200Wrapper::LensJ200Wrapper(QString port, quint32 baudrate, QObject *parent)
    :m_baudrate (baudrate), m_port(port)
{
    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
        m_j200_ctrl = new LensJ200Controller();

        qDebug() << Q_FUNC_INFO <<"Lens J200 Wrapper thread id: " << QThread::currentThreadId();

        m_NotificationTimer = new QTimer(this);
        m_NotificationTimer->setInterval(100);

        connect(m_NotificationTimer, &QTimer::timeout, this, &LensJ200Wrapper::proc_timerUpdate);

        QTimer::singleShot(100, this, [&](){
            m_j200_ctrl->connect(m_port);
            m_NotificationTimer->start();
        });




    });
    QObject::connect(&thread, &QThread::finished, this, &LensJ200Wrapper::proc_Thread_Finished);

    thread.start();


}


LensJ200Wrapper::~LensJ200Wrapper()
{
    qDebug() << Q_FUNC_INFO;

    thread.exit();
    thread.wait();
    qDebug()<<Q_FUNC_INFO << "LensJ200Wrapper thread exit";

}

void LensJ200Wrapper::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;
    m_NotificationTimer->stop();
    delete m_NotificationTimer;
    delete(m_j200_ctrl);
}



void LensJ200Wrapper::proc_CtrlCloseConnection()
{
    m_NotificationTimer->stop();
    m_j200_ctrl->proc_disconnect();
    qDebug()<<"LensJ200 UART Closed";

}

void LensJ200Wrapper::proc_timerUpdate()
{
    if(m_j200_ctrl == nullptr)
        return;
    if(!m_j200_ctrl->isConnected())
        return;

    if(m_IsRequestReport)
    {
        m_j200_ctrl->proc_report();
       // m_j200_ctrl->proc_getFocalLength();
    }

    m_ds.focalLength = m_j200_ctrl->getFocalLength();
    m_ds.zoomPosition = m_j200_ctrl->getZoomPosition();
    m_ds.focusPosition = m_j200_ctrl->getFocusPosition();
    m_ds.temperature = m_j200_ctrl->getTemperature();


    emit sig_ReportReady(m_ds);
}

void LensJ200Wrapper::proc_SendCMD(J200_CMD_enum cmd, const quint32 par1, const quint32 par2)
{

    if(m_j200_ctrl == nullptr)
        return;
    if(!m_j200_ctrl->isConnected())
        return;
  //  qDebug() << Q_FUNC_INFO << cmd<< "  "<< par1 << "  " << par2;

    switch(cmd)
    {
    case  J200_DISCONNECT:
        m_j200_ctrl->proc_disconnect();
        break;
    case  J200_REPORT:
        m_j200_ctrl->proc_report();
        break;
    case J200_ZOOM_OUT_CONT:
        m_j200_ctrl->proc_zoomOutContinuous();
        m_IsRequestReport=false;
        break;
    case J200_ZOOM_IN_CONT:
        m_j200_ctrl->proc_zoomInContinuous();
        m_IsRequestReport=false;
        break;
    case J200_ZOOM_STOP:
        m_j200_ctrl->proc_zoomStop();
        m_IsRequestReport=true;
        break;
    case J200_FOCUS_FAR_CONT:
        m_j200_ctrl->proc_focusFarContinuous();
        m_IsRequestReport=false;
        break;
    case J200_FOCUS_NEAR_CONT:
        m_j200_ctrl->proc_focusNearContinuous();
        m_IsRequestReport=false;
        break;
    case J200_FOCUS_STOP:
        m_j200_ctrl->proc_focusStop();
        m_IsRequestReport=true;
        break;
    case J200_SET_SPEED:
        m_j200_ctrl->proc_setSpeed(par1,par2);
        break;
    case J200_GOTO_POS:
        m_j200_ctrl->proc_gotoPosition(par1, par2);
        break;
    case J200_GOTO_FOCAL_LEN:
        m_j200_ctrl->proc_gotoFocalLength(par1);
        break;

    }
}
