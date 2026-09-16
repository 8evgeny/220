#include "camera_ev9500m_worker.h"
#include "QThread"

Camera_EV9500M_Worker::Camera_EV9500M_Worker() {}

Camera_EV9500M_Worker::~Camera_EV9500M_Worker()
{
    qDebug() << Q_FUNC_INFO;
    thread.exit();
    thread.wait();

    qDebug() << Q_FUNC_INFO << "exit" ;

}

void Camera_EV9500M_Worker::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;

    m_timer->stop();
    delete m_timer;
}


void Camera_EV9500M_Worker::cameraInit()
{
    connect(&thread, &QThread::started, this, [this]{
        m_timer = new QTimer(this);
        qDebug() << Q_FUNC_INFO << "Camera VISCA thread id: " << QThread::currentThreadId();
        connect(m_timer, &QTimer::timeout, this, &Camera_EV9500M_Worker::timeout);
    });
    QObject::connect(&thread, &QThread::finished, this, &Camera_EV9500M_Worker::proc_Thread_Finished);

    thread.start();


}


void Camera_EV9500M_Worker::proc_VISCA_CONNECT(QString portName, quint32 baudrate)
{

    qDebug() << Q_FUNC_INFO << portName << baudrate;

    if(m_isConnected)
        m_cam.disconnect();

    while( m_cam.connect(portName, baudrate) != VISCA_SUCCESS) {
        m_cam.disconnect();
        thread.msleep(1000);
    }


    m_isConnected = true;

    if (m_cam.isConnected()) {
        m_timer->start(500);
    } else {
        m_timer->stop();
        qDebug()<<Q_FUNC_INFO<<"Connection error";
    }

    emit connected(m_isConnected);
}

void Camera_EV9500M_Worker::proc_VISCA_DISCONNECT()
{




    m_isConnected= false;
     m_timer->stop();
    emit connected(false);
     thread.usleep(1000);

    m_cam.disconnect();
    qDebug()<<"EV9500M UART closed";

}


void Camera_EV9500M_Worker::proc_VISCA_CMD(TVCamera::Command cmd, quint32 value)
{ 
    if (m_cam.executeCommand(cmd, value) != VISCA_SUCCESS) {
        qDebug()<<"Visca command error";
    }
}

void Camera_EV9500M_Worker::timeout()
{
  //  qDebug()<<Q_FUNC_INFO<<QThread::currentThreadId();
    if (!m_cam.isBusy())
    {
        bool res = VISCA_SUCCESS;
        res |= m_cam.getTemperature();
        res |= m_cam.getIris();
        res |= m_cam.getShutter();
        res |= m_cam.getGain();
        res |= m_cam.getContrast();
        res |= m_cam.getFocusValue();
        res |= m_cam.getZoomValue();
        res |= m_cam.getAWBDisplayMode();
         //m_cam.getFocusZoomValue();

        if(res != VISCA_SUCCESS)
            qDebug()<<"EV9500M error status request";
        CameraStatus status = *m_cam.status();


        // qDebug()<<"Status:";
     //    qDebug()<<"Temperature:"<<status.temperature;
        // qDebug()<<"Iris:"<<status.iris;
        // qDebug()<<"Gain:"<<status.gain;
        //qDebug()<<"Contrast:"<<status.contrast;

        emit sendStatus(status);
    }
}
