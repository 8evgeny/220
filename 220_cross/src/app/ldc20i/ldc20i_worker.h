#ifndef LDC20I_WORKER_H
#define LDC20I_WORKER_H

#include "ldc20i_controller.h"
#include <QObject>

class LDC20I_Worker: public QObject
{
    Q_OBJECT
    QMutex m_lock;
    QThread thread;
public:
    explicit LDC20I_Worker(QString port, quint32 baudrate, QObject *parent);
    ~LDC20I_Worker();
    void proc_CtrlInitConnection();
    void proc_CtrlCloseConnection();

    void proc_LDCCtrlEmit(uint8_t state);
    void proc_LDCSetEmitMode(bool continous_emit);
    void proc_LDCSetFreq(float emit_freqHz);
    void proc_LDCSetPeriod(float emit_period);
    void proc_LDCSetTempReg(uint8_t state);

public slots:
    void proc_recieved_LDC_DS_ANS(LDC_ANS_enum cmd, LDC_STATE dev_state);
    void proc_COMDeviceConnected();
    void proc_timerUpdate();
    void proc_timeoutTimerUpdate();

    void proc_LDCSendCmd(LDC_CMD_enum cmd, quint32 value);
    void proc_Thread_Finished();

signals:
    void send_LDC_State(LDC_STATE dev_state);
    void send_LDC_CMD(LDC_CMD_enum cmd, quint32 value);

private:
    LDC20I_Controller *ldc;
    QTimer* m_NotificationTimer;
    QTimer* m_TimeoutTimer;

    LDC_CMD_enum next_cmd;
    LDC_STATE dev_state;
    QString m_port="";
    quint32 m_baudrate = 0;

};

#endif // LDC20I_WORKER_H
