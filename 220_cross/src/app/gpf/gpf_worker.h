#ifndef GPF_WORKER_H
#define GPF_WORKER_H

#include "gpf_controller.h"
#include <QObject>
#include <qthread.h>

enum GPF_CMD_enum
{

    GPF_SET_CONTROL_POSITION = 0x01,
    GPF_SET_CONTROL_SPEED = 0x02,
    GPF_SET_CONTROL_TRACKING = 0x03,
    GPF_SET_MODE = 0x05

} ;


class GyroPlatformVCD_Worker: public QObject
{
    Q_OBJECT
    QMutex m_qlock;
    QThread thread;
public:
    explicit GyroPlatformVCD_Worker(QString port, quint32 baudrate, QObject *parent = nullptr);


    // void proc_LDCCtrlEmit(uint8_t state);
    // void proc_LDCSetFreq(float emit_freqHz);
    // void proc_LDCSetPeriod(float emit_period);
    // void proc_LDCSetTempReg(uint8_t state);

public slots:
    void proc_recieved_GyroPlatformVCD_ANS(MINI640_ANS_enum ans);
    void proc_COMDeviceConnected();
    void proc_timerUpdate();
    void proc_GyroPlatformVCD_ExecCmd(MINI640_CMD_enum cmd, quint32 value);
    void proc_InitReady();
    void proc_CtrlInitConnection();
    void proc_CtrlCloseConnection();
signals:
    void recv_GyroPlatformVCD_State(MINI640_DEV_STATE dev_state);
    void send_GyroPlatformVCD_CMD(MINI640_REG_MAP_enum addr, quint32 cmd, quint32 par);
    void send_GOWIN_CMD( quint32 cmd);

    void recv_MINI640_ans_notify();
    void sig_READY();
    void sig_CtrlInitConnection();
    void sig_CtrlCloseConnection();

private:
    void synq_send_GyroPlatformVCD(MINI640_REG_MAP_enum addr, quint32 cmd, quint32 par);
    void synq_send_GOWIN(quint32 cmd);

    void req_next_DS_CMD();

    GyroPlatformVCD_Controller *gpf_ctrl = nullptr;
    QTimer* m_NotificationTimer = nullptr;

   // LDC_CMD_enum next_cmd;
    GyroPlatformVCD_DEV_STATE m_dev_state;
    QList<quint32> m_gpf_addr_list;
    QList<quint32> m_proc_addr_list;
    QString m_port = "";
    quint32 m_baudrate = 0;
    bool m_isTimeout = false;
};

#endif // MINI640_WORKER_H
