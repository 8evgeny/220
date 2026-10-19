#ifndef MINI640_WORKER_H
#define MINI640_WORKER_H

#include "mini640_controller.h"
#include <QObject>

enum MINI640_CMD_enum
{

    MINI640_SET_TEST_PATTERN = 0x01,
    MINI640_SET_FRAME_PERIOD = 0x02,
    MINI640_SET_EXP_DURATION = 0x03,
    MINI640_SET_VIDEO_INV    = 0x04,
    MINI640_SET_GPOL_BIAS    = 0x05,

    MINI640_GET_TS           = 0xFF,
    MINI640_COOLER_ON        = 0xF0,
    MINI640_COOLER_OFF       = 0xF1,

} ;


class MINI640_Worker: public QObject
{
    Q_OBJECT
    QMutex m_qlock;
    QThread thread;
public:
    explicit MINI640_Worker(QString port, quint32 baudrate, quint32 exp_period, quint32 exp, double gpol, QObject *parent);
    ~MINI640_Worker();

    // void proc_LDCCtrlEmit(uint8_t state);
    // void proc_LDCSetFreq(float emit_freqHz);
    // void proc_LDCSetPeriod(float emit_period);
    // void proc_LDCSetTempReg(uint8_t state);

public slots:
    void proc_recieved_MINI640_ANS(MINI640_ANS_enum ans);
    void proc_COMDeviceConnected();
    void proc_timerUpdate();
    void proc_MINI640_ExecCmd(MINI640_CMD_enum cmd, quint32 uvalue=0, float fvalue=0);
    void proc_InitReady();
    void proc_CtrlInitConnection();
    void proc_CtrlCloseConnection();
    void proc_Thread_Finished();

signals:
    void recv_MINI640_State(MINI640_DEV_STATE dev_state);
    void send_MINI640_CMD(MINI640_REG_MAP_enum addr, quint32 cmd, quint32 par);
    void send_GOWIN_CMD( quint32 cmd);

    void recv_MINI640_ans_notify();
    void sig_READY();
    void sig_CtrlInitConnection();
    void sig_CtrlCloseConnection();

private:
    void synq_send_MINI640(MINI640_REG_MAP_enum addr, quint32 cmd, quint32 par);
    void synq_send_GOWIN(quint32 cmd);

    void req_next_DS_CMD();

    MINI640_Controller *mini_ctrl = nullptr;
    QTimer* m_NotificationTimer = nullptr;

   // LDC_CMD_enum next_cmd;
    MINI640_DEV_STATE m_dev_state;
    QList<quint32> m_mini640_addr_list;
    QList<quint32> m_proc_addr_list;
    QString m_port = "";
    quint32 m_baudrate = 0;
    quint32 m_exp_period = 92500;
    quint32 m_exp = 62000;
    double m_gpol = 701;

    bool m_isTimeout = false;
    quint32 m_RptCnt = 0;

};

#endif // MINI640_WORKER_H
