#ifndef CAMERA_EV9500M_WORKER_H
#define CAMERA_EV9500M_WORKER_H

#include "camera_ev9500m.h"
#include <QObject>
#include <qthread.h>

class Camera_EV9500M_Worker: public QObject
{
    Q_OBJECT
    QThread thread;

public:
    explicit Camera_EV9500M_Worker();
    ~Camera_EV9500M_Worker();
    void cameraInit();

private:
    TVCamera m_cam;
    QTimer *m_timer;
    bool m_isConnected = false;

public slots:
    void proc_VISCA_CMD(TVCamera::Command cmd, quint32 value);
    void proc_VISCA_CONNECT(QString portName, quint32 baudrate);
    void proc_VISCA_DISCONNECT();

    void timeout();
    void proc_Thread_Finished();

signals:
    void connected(bool isConnected);
    void sendStatus(CameraStatus status);


};

#endif // CAMERA_EV9500M_WORKER_H
