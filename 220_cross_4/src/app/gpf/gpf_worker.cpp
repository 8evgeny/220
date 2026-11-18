#include "gpf_worker.h"
#include <qdebug.h>
#include <qeventloop.h>

GyroPlatformVCD::GyroPlatformVCD(QString port, quint32 baudrate, QObject *parent)
    :m_baudrate (baudrate), m_port(port)  //:
                            // ldc(port, baudrate)
{    // LDC20I_Controller ldc("/dev/ttyUSB0", 115200);
    qDebug() << Q_FUNC_INFO << "OUT THREAD" << QThread::currentThreadId();

    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
        qDebug() << Q_FUNC_INFO << "IN THREAD" << QThread::currentThreadId();

        mini_ctrl = new VCD_Controller(this->m_port, this->m_baudrate, this);
        connect(mini_ctrl, &VCD_Controller::sig_READY, this, &GyroPlatformVCD::proc_InitReady, Qt::QueuedConnection );

        connect(this, &GyroPlatformVCD::send_VCD_CMD, mini_ctrl, &VCD_Controller::proc_VCD_CMD, Qt::QueuedConnection);
        connect(this, &GyroPlatformVCD::send_GOWIN_CMD, mini_ctrl, &VCD_Controller::proc_GOWIN_CMD, Qt::QueuedConnection);

        connect(this, &GyroPlatformVCD::sig_CtrlInitConnection, mini_ctrl, &VCD_Controller::proc_initConnection, Qt::QueuedConnection);
        connect(this, &GyroPlatformVCD::sig_CtrlCloseConnection, mini_ctrl, &VCD_Controller::proc_closeConnection, Qt::QueuedConnection);


        connect(mini_ctrl, &VCD_Controller::recieved_VCD_ANS, this,  &GyroPlatformVCD::proc_recieved_VCD_ANS);
        connect(mini_ctrl, &VCD_Controller::comDeviceConnected, this,  &GyroPlatformVCD::proc_COMDeviceConnected, Qt::QueuedConnection);
        qDebug() << Q_FUNC_INFO << "VCD Worker thread id: " << QThread::currentThreadId();

        // m_NotificationTimer = new QTimer(this);
        // m_NotificationTimer->setInterval(1);
        // m_NotificationTimer->setSingleShot(true);
        m_vcd_addr_list = mini_ctrl->get_MINIAddr_List();
        // next_cmd = LDC_GET_MEASUREMENTS;
      //  connect(m_NotificationTimer, &QTimer::timeout, this, &GyroPlatformVCD::proc_timerUpdate);
    });
    thread.start();


}


void GyroPlatformVCD::proc_InitReady()
{
      qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    m_NotificationTimer = new QTimer(this);
    m_NotificationTimer->setInterval(50);
    //m_NotificationTimer->setSingleShot(true);
   // vcd_addr_list = mini_ctrl->get_MINIAddr_List();
    // next_cmd = LDC_GET_MEASUREMENTS;
    connect(m_NotificationTimer, &QTimer::timeout, this, &GyroPlatformVCD::proc_timerUpdate);

    emit sig_READY();
}


void GyroPlatformVCD::synq_send_VCD(VCD_REG_MAP_enum addr, quint32 cmd, quint32 par)
{
  //  qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    QEventLoop loop;
    connect(this, &GyroPlatformVCD::recv_VCD_ans_notify, &loop, &QEventLoop::quit);
    emit send_VCD_CMD(addr,cmd, par);
    loop.exec();
}

void GyroPlatformVCD::synq_send_GOWIN(quint32 cmd)
{
     // qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    QEventLoop loop;
    connect(this, &GyroPlatformVCD::recv_VCD_ans_notify, &loop, &QEventLoop::quit);
    emit send_GOWIN_CMD(cmd);
    loop.exec();
}



void GyroPlatformVCD::req_next_DS_CMD()
{

    m_qlock.lock();
   // qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    //qDebug() << "proc_addr_list: " <<proc_addr_list.size();
    if(!m_proc_addr_list.empty())
    {
        VCD_REG_MAP_enum addr= (VCD_REG_MAP_enum)m_proc_addr_list.takeFirst();
       // proc_addr_list.removeFirst();
        synq_send_VCD(addr, VCD_RD, 0);

    }
    else
    {
        qDebug()<<"Gowin send start";
        synq_send_GOWIN(GOWIN_VCD_GET_TS);
        qDebug()<<"Gowin send end";
        m_proc_addr_list = m_vcd_addr_list;
        VCD_DEV_STATE ds= mini_ctrl->get_MINI_DS();
        ds.isTimeout = m_isTimeout;
        m_dev_state = ds;
        //proc_VCD_ExecCmd(VCD_GET_TS, 0);
       // proc_VCD_ExecCmd(VCD_SET_TEST_PATTERN, 5);
        //proc_VCD_ExecCmd(VCD_SET_FRAME_PERIOD, 90000);
       // proc_VCD_ExecCmd(VCD_SET_EXP_DURATION, 1000);

        emit recv_VCD_State(ds);

    }
    m_qlock.unlock();
  //  m_NotificationTimer->start();
}

void GyroPlatformVCD::proc_timerUpdate()
{
   // qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    req_next_DS_CMD();
}

void GyroPlatformVCD::proc_CtrlInitConnection()
{
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    //mini_ctrl->proc_initConnection();
    emit sig_CtrlInitConnection();
}

void GyroPlatformVCD::proc_CtrlCloseConnection()
{
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    m_NotificationTimer->stop();
   // mini_ctrl->proc_closeConnection();
    emit sig_CtrlCloseConnection();


}

void GyroPlatformVCD::proc_VCD_ExecCmd(VCD_CMD_enum cmd, quint32 value)
{
    m_NotificationTimer->stop();

   // m_qlock.lock();
    //qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    FPA_PATTERN_CTRL_STR pcs;
    uint8_t *ptr= (uint8_t*)&value;
   // if(dev_state.hsv.CCE_Ref_1==0 || dev_state.hsv.CCE_VersionFPGA_1==0 || dev_state.hsv.CCE_VersionSft_1 == 0 )
   //     return; // no devstate
    if(m_dev_state.mon.cce_temp ==0)
        return;
     switch (cmd) {
        case VCD_SET_TEST_PATTERN:
            //pcs = *(FPA_PATTERN_CTRL_STR*)&dev_state.raw_reg[PATTERN_CTRL];
            pcs.test_pattern_act=value;
            synq_send_VCD(PATTERN_CTRL, VCD_WR, *(quint32*)&pcs);
            break;
        case VCD_SET_FRAME_PERIOD:
            synq_send_VCD(INT_PERIOD_TIME_1, VCD_WR, ptr[0]);
            synq_send_VCD(INT_PERIOD_TIME_2, VCD_WR, ptr[1]);
            synq_send_VCD(INT_PERIOD_TIME_3, VCD_WR, ptr[2]);
            break;
        case VCD_SET_EXP_DURATION:
            synq_send_VCD(INT_HIGH_LEVEL_1, VCD_WR, ptr[0]);
            synq_send_VCD(INT_HIGH_LEVEL_2, VCD_WR, ptr[1]);
            synq_send_VCD(INT_HIGH_LEVEL_3, VCD_WR, ptr[2]);
            break;
        case VCD_GET_TS:
            synq_send_GOWIN(GOWIN_VCD_GET_TS);
            break;
        case VCD_COOLER_ON:
            synq_send_GOWIN(GOWIN_VCD_COOLER_ON);
            break;
        case VCD_COOLER_OFF:
            synq_send_GOWIN(GOWIN_VCD_COOLER_OFF);
            break;
        break;
     }
        m_NotificationTimer->setInterval(50);
        m_NotificationTimer->start();

     //   m_qlock.unlock();


}



void GyroPlatformVCD::proc_COMDeviceConnected()
{
    m_NotificationTimer->start();

}

void GyroPlatformVCD::proc_recieved_VCD_ANS(VCD_ANS_enum ans)
{

   // qDebug() << Q_FUNC_INFO << QThread::currentThreadId() << "ANS: " << ans;

    if(ans == MINI_ANS_NO_REPLY)
        m_isTimeout = true;
    else
        m_isTimeout = false;
    emit recv_VCD_ans_notify();


}
