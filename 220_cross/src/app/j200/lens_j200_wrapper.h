#ifndef LENS_J200_WRAPPER_H
#define LENS_J200_WRAPPER_H

#include "j200/lens_j200_controller.h"
#include <QObject>
#include <qthread.h>


enum J200_CMD_enum
{
    //Общие запросы
    J200_DISCONNECT,
    J200_REPORT,
    J200_ZOOM_OUT_CONT,
    J200_ZOOM_IN_CONT,
    J200_ZOOM_STOP,
    J200_FOCUS_FAR_CONT,
    J200_FOCUS_NEAR_CONT,
    J200_FOCUS_STOP,
    J200_SET_SPEED,
    J200_GOTO_POS,
    J200_GOTO_FOCAL_LEN,
};


typedef struct {
    qint32   focalLength;
    qint32   zoomPosition;
    qint32   focusPosition;
    int   temperature;
} J200_DEVSTATE;


class LensJ200Wrapper: public QObject
{
    Q_OBJECT

public:
    explicit  LensJ200Wrapper(QString port, quint32 baudrate, QObject *parent = nullptr);
    ~LensJ200Wrapper();
    void proc_CtrlCloseConnection();

private:
    LensJ200Controller * m_j200_ctrl = nullptr;
    QString m_port = "";
    quint32 m_baudrate = 0;
    QTimer* m_NotificationTimer = nullptr;
    J200_DEVSTATE m_ds = {0};
    QThread thread;
    bool m_IsRequestReport = true;

private slots:
    void proc_timerUpdate();



public slots:
  //  void proc_InitReady();
    void proc_SendCMD(J200_CMD_enum cmd, const quint32 par1 = 0, const quint32 par2 = 0);
    void proc_Thread_Finished();


signals:
    void sig_ReportReady(J200_DEVSTATE ds);
};

#endif // LENS_J200_WRAPPER_H
