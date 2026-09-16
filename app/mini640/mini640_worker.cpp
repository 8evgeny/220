#include "mini640_worker.h"
#include <qdebug.h>
#include <qeventloop.h>

MINI640_Worker::MINI640_Worker(QString port, quint32 baudrate, quint32 exp_period, quint32 exp, double gpol, QObject *parent)
    :m_baudrate (baudrate), m_port(port), m_exp_period(exp_period), m_exp(exp), m_gpol(gpol)  //:
                            // ldc(port, baudrate)
{    // LDC20I_Controller ldc("/dev/ttyUSB0", 115200);
    qDebug() << Q_FUNC_INFO << "OUT THREAD" << QThread::currentThreadId();

    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
        qDebug() << Q_FUNC_INFO << "IN THREAD" << QThread::currentThreadId();

        mini_ctrl = new MINI640_Controller(this->m_port, this->m_baudrate, this);
        connect(mini_ctrl, &MINI640_Controller::sig_READY, this, &MINI640_Worker::proc_InitReady, Qt::QueuedConnection );

        connect(this, &MINI640_Worker::send_MINI640_CMD, mini_ctrl, &MINI640_Controller::proc_MINI640_CMD, Qt::QueuedConnection);
        connect(this, &MINI640_Worker::send_GOWIN_CMD, mini_ctrl, &MINI640_Controller::proc_GOWIN_CMD, Qt::QueuedConnection);

        connect(this, &MINI640_Worker::sig_CtrlInitConnection, mini_ctrl, &MINI640_Controller::proc_initConnection, Qt::QueuedConnection);
        connect(this, &MINI640_Worker::sig_CtrlCloseConnection, mini_ctrl, &MINI640_Controller::proc_closeConnection, Qt::QueuedConnection);


        connect(mini_ctrl, &MINI640_Controller::recieved_MINI640_ANS, this,  &MINI640_Worker::proc_recieved_MINI640_ANS);
        connect(mini_ctrl, &MINI640_Controller::comDeviceConnected, this,  &MINI640_Worker::proc_COMDeviceConnected, Qt::QueuedConnection);
        qDebug() << Q_FUNC_INFO << "MINI640 Worker thread id: " << QThread::currentThreadId();

        // m_NotificationTimer = new QTimer(this);
        // m_NotificationTimer->setInterval(1);
        // m_NotificationTimer->setSingleShot(true);
        m_mini640_addr_list = mini_ctrl->get_MINIAddr_List();
        // next_cmd = LDC_GET_MEASUREMENTS;
      //  connect(m_NotificationTimer, &QTimer::timeout, this, &MINI640_Worker::proc_timerUpdate);
    });

    QObject::connect(&thread, &QThread::finished, this, &MINI640_Worker::proc_Thread_Finished);

    thread.start();


}

void MINI640_Worker::proc_Thread_Finished()
{
    m_NotificationTimer->stop();
    delete(mini_ctrl);

}

MINI640_Worker::~MINI640_Worker()
{
    qDebug() << Q_FUNC_INFO;
    thread.exit();
    thread.wait();
    qDebug()<<Q_FUNC_INFO << "MINI640_Worker thread exit";

}



void MINI640_Worker::proc_InitReady()
{
      qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    m_NotificationTimer = new QTimer(this);
    m_NotificationTimer->setInterval(1);
    //m_NotificationTimer->setSingleShot(true);
   // mini640_addr_list = mini_ctrl->get_MINIAddr_List();
    // next_cmd = LDC_GET_MEASUREMENTS;
    connect(m_NotificationTimer, &QTimer::timeout, this, &MINI640_Worker::proc_timerUpdate);

    m_NotificationTimer->start();
    emit sig_READY();
}


void MINI640_Worker::synq_send_MINI640(MINI640_REG_MAP_enum addr, quint32 cmd, quint32 par)
{
  //  qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    QEventLoop loop;
    connect(this, &MINI640_Worker::recv_MINI640_ans_notify, &loop, &QEventLoop::quit);
    emit send_MINI640_CMD(addr,cmd, par);
    loop.exec();
    if(m_isTimeout)
    {
        if(m_RptCnt < 3)
        {
            m_RptCnt++;
            synq_send_MINI640(addr, cmd, par);
        }
        else
        {
            m_RptCnt=0;
        }

    } else
        m_RptCnt=0;


}

void MINI640_Worker::synq_send_GOWIN(quint32 cmd)
{
     // qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    QEventLoop loop;
    connect(this, &MINI640_Worker::recv_MINI640_ans_notify, &loop, &QEventLoop::quit);
    emit send_GOWIN_CMD(cmd);
    loop.exec();
    if(m_isTimeout)
    {
        if(m_RptCnt < 3)
        {
            m_RptCnt++;
            synq_send_GOWIN(cmd);
        }
        else
        {
            m_RptCnt=0;
        }

    } else
        m_RptCnt=0;





}

int count =0;

void MINI640_Worker::req_next_DS_CMD()
{

   // m_qlock.lock();
  //  qDebug() << Q_FUNC_INFO << QThread::currentThreadId() << ++count;
    //qDebug() << "proc_addr_list: " <<proc_addr_list.size();

    {
     //   qDebug()<<"Gowin send start";
        synq_send_GOWIN(GOWIN_MINI640_GET_TS);
     //   qDebug()<<"Gowin send end";
        m_proc_addr_list = m_mini640_addr_list;
        MINI640_DEV_STATE ds= mini_ctrl->get_MINI_DS();
        ds.isTimeout = m_isTimeout;
        m_dev_state = ds;
        //proc_MINI640_ExecCmd(MINI640_GET_TS, 0);
        // proc_MINI640_ExecCmd(MINI640_SET_TEST_PATTERN, 5);
        proc_MINI640_ExecCmd(MINI640_SET_FRAME_PERIOD, m_exp_period);
        //    QThread::msleep(1);
        //proc_MINI640_ExecCmd(MINI640_SET_FRAME_PERIOD, 90000);
        proc_MINI640_ExecCmd(MINI640_SET_EXP_DURATION, m_exp);
        proc_MINI640_ExecCmd(MINI640_SET_VIDEO_INV, 1);


        //     proc_MINI640_ExecCmd(MINI640_SET_GPOL_BIAS, 0, 701.445);

        if((m_gpol <100)|| (m_gpol > 1500))
            m_gpol= 701.445;
        proc_MINI640_ExecCmd(MINI640_SET_GPOL_BIAS, 0, m_gpol);

        // proc_MINI640_ExecCmd(MINI640_SET_GPOL_BIAS, 0, 870.000);
        //   proc_MINI640_ExecCmd(MINI640_SET_EXP_DURATION, 5);
        // proc_MINI640_ExecCmd(MINI640_SET_TEST_PATTERN, 6);
   //     qDebug()<<"recv_MINI640_State start";

        emit recv_MINI640_State(ds);
//        qDebug()<<"recv_MINI640_State stop";

    }

    while(!m_proc_addr_list.empty())
    {
        MINI640_REG_MAP_enum addr= (MINI640_REG_MAP_enum)m_proc_addr_list.takeFirst();
       // proc_addr_list.removeFirst();
        synq_send_MINI640(addr, MINI640_RD, 0);

    }
   // else

   // m_qlock.unlock();
  //  m_NotificationTimer->start();
}

void MINI640_Worker::proc_timerUpdate()
{
   // qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    req_next_DS_CMD();
}

void MINI640_Worker::proc_CtrlInitConnection()
{
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    //mini_ctrl->proc_initConnection();
    emit sig_CtrlInitConnection();
}

void MINI640_Worker::proc_CtrlCloseConnection()
{
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    m_NotificationTimer->stop();
   // mini_ctrl->proc_closeConnection();
    emit sig_CtrlCloseConnection();
    qDebug()<<"MINI640 UART Closed";



}

void MINI640_Worker::proc_MINI640_ExecCmd(MINI640_CMD_enum cmd, quint32 uvalue, float fvalue)
{
    //m_NotificationTimer->stop();

   // m_qlock.lock();
    //qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    FPA_PATTERN_CTRL_STR pcs;
    uint8_t *ptr= (uint8_t*)&uvalue;
   // if(dev_state.hsv.CCE_Ref_1==0 || dev_state.hsv.CCE_VersionFPGA_1==0 || dev_state.hsv.CCE_VersionSft_1 == 0 )
   //     return; // no devstate
    if(m_dev_state.mon.cce_temp ==0)
        return;
     switch (cmd) {
        case MINI640_SET_TEST_PATTERN:
            //pcs = *(FPA_PATTERN_CTRL_STR*)&dev_state.raw_reg[PATTERN_CTRL];
            pcs.test_pattern_act=uvalue;
            synq_send_MINI640(PATTERN_CTRL, MINI640_WR, *(quint32*)&pcs);
            break;
        case MINI640_SET_FRAME_PERIOD:
            synq_send_MINI640(INT_PERIOD_TIME_1, MINI640_WR, ptr[0]);
            synq_send_MINI640(INT_PERIOD_TIME_2, MINI640_WR, ptr[1]);
            synq_send_MINI640(INT_PERIOD_TIME_3, MINI640_WR, ptr[2]);
            break;
        case MINI640_SET_EXP_DURATION:
            synq_send_MINI640(INT_HIGH_LEVEL_1, MINI640_WR, ptr[0]);
            synq_send_MINI640(INT_HIGH_LEVEL_2, MINI640_WR, ptr[1]);
            synq_send_MINI640(INT_HIGH_LEVEL_3, MINI640_WR, ptr[2]);
            break;

        case  MINI640_SET_VIDEO_INV:
            CCE_CONF_STR cce;
            cce.video_inversion = uvalue;
            ptr= (uint8_t*)&cce;
            synq_send_MINI640(CCE_CONF, MINI640_WR, ptr[0]);
            break;

        case  MINI640_SET_GPOL_BIAS:

            {
                float f_gpol_bias = fvalue;
                m_gpol = fvalue;

                uint16_t u_gpol_bias = f_gpol_bias*43.6;
                uint8_t *ptr= (uint8_t*)&u_gpol_bias;

                synq_send_MINI640(GPOL_DAC_LSB, MINI640_WR, ptr[0]);
                synq_send_MINI640(GPOL_DAC_MSB, MINI640_WR, ptr[1]);

            }
            break;

        case MINI640_GET_TS:
            synq_send_GOWIN(GOWIN_MINI640_GET_TS);
            break;
        case MINI640_COOLER_ON:
            synq_send_GOWIN(GOWIN_MINI640_COOLER_ON);
            break;
        case MINI640_COOLER_OFF:
            synq_send_GOWIN(GOWIN_MINI640_COOLER_OFF);
            break;
        break;
     }
       // m_NotificationTimer->setInterval(100);
     //   m_NotificationTimer->start();

     //   m_qlock.unlock();


}



void MINI640_Worker::proc_COMDeviceConnected()
{


}

void MINI640_Worker::proc_recieved_MINI640_ANS(MINI640_ANS_enum ans)
{

   // qDebug() << Q_FUNC_INFO << QThread::currentThreadId() << "ANS: " << ans;

    if(ans == MINI_ANS_NO_REPLY)
    {
        m_isTimeout = true;
        // qDebug() << Q_FUNC_INFO << QThread::currentThreadId() << "ANS: " << ans;
    }
    else
        m_isTimeout = false;
    emit recv_MINI640_ans_notify();


}
