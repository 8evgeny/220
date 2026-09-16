#include "ldc20i_worker.h"
#include <qdebug.h>

LDC20I_Worker::LDC20I_Worker(QString port, quint32 baudrate, QObject *parent): m_port(port), m_baudrate(baudrate)
                            // ldc(port, baudrate)
{    // LDC20I_Controller ldc("/dev/ttyUSB0", 115200);

    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
        ldc = new LDC20I_Controller(m_port, m_baudrate, this);
        connect(this, &LDC20I_Worker::send_LDC_CMD, ldc, &LDC20I_Controller::proc_LDC_CMD, Qt::QueuedConnection);
        connect(ldc, &LDC20I_Controller::recieved_LDC_DS_ANS, this,  &LDC20I_Worker::proc_recieved_LDC_DS_ANS);
        connect(ldc, &LDC20I_Controller::comDeviceConnected, this,  &LDC20I_Worker::proc_COMDeviceConnected, Qt::QueuedConnection);
        qDebug() << Q_FUNC_INFO << "LDC-20I Worker thread id: " << QThread::currentThreadId();

        m_NotificationTimer = new QTimer(this);
        m_TimeoutTimer= new QTimer(this);

        m_NotificationTimer->setInterval(50);

        m_TimeoutTimer->setInterval(20);
        m_TimeoutTimer->setSingleShot(true);

        next_cmd = LDC_GET_MEASUREMENTS;
        connect(m_NotificationTimer, &QTimer::timeout, this, &LDC20I_Worker::proc_timerUpdate);
        connect(m_NotificationTimer, &QTimer::timeout, this, &LDC20I_Worker::proc_timeoutTimerUpdate);
    });
    QObject::connect(&thread, &QThread::finished, this, &LDC20I_Worker::proc_Thread_Finished);

    thread.start();


}

LDC20I_Worker::~LDC20I_Worker()
{
    qDebug() << Q_FUNC_INFO;
    thread.exit();
    thread.wait();
    qDebug()<<Q_FUNC_INFO << "LDC20I thread exit";

}


void LDC20I_Worker::proc_Thread_Finished()
{
    m_NotificationTimer->stop();
    m_TimeoutTimer->stop();
    delete m_NotificationTimer;
    delete m_TimeoutTimer;
    delete ldc;
}

void LDC20I_Worker::proc_timeoutTimerUpdate()
{
    m_lock.lock();

    m_lock.unlock();
}

void LDC20I_Worker::proc_timerUpdate()
{
//    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    //emit send_LDC_CMD(next_cmd,0);
    emit send_LDC_CMD(LDC_GET_MEASUREMENTS,0);
   // QThread::msleep(500);
    emit send_LDC_CMD(LDC_GET_DISTANCE,0);

    m_TimeoutTimer->setInterval(20);
    m_TimeoutTimer->start();

  //   emit send_LDC_CMD(LDC_GET_DISTANCE,0);
// emit send_LDC_CMD(LDC_GET_MEASUREMENTS,0);
   // emit send_LDC_CMD(LDC_GET_MEASUREMENTS,0);
    //m_NotificationTimer->stop();
}

void LDC20I_Worker::proc_CtrlInitConnection()
{
    ldc->proc_initConnection();

}

void LDC20I_Worker::proc_CtrlCloseConnection()
{
    m_NotificationTimer->stop();
    m_TimeoutTimer->stop();

    ldc->proc_closeConnection();

}

void LDC20I_Worker::proc_LDCSendCmd(LDC_CMD_enum cmd, quint32 value)
{
        qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();;
        switch (cmd) {
        case LDC_SET_TEMP_REG_STATE:
            proc_LDCSetTempReg(value);
            break;
        case LDC_SET_EMIT_STATE:
            proc_LDCCtrlEmit(value);
            break;
        case LDC_SET_EMIT_PERIOD:
            proc_LDCSetPeriod((float)value);
            break;
        case LDC_SET_EMIT_MFREQ:
            proc_LDCSetFreq((float)value);
            break;
        case LDC_SET_EMIT_MODE:
            proc_LDCSetEmitMode((bool)value);
            break;

        default:
            break;
        }
}


void LDC20I_Worker::proc_LDCCtrlEmit(uint8_t state)
{
      if((int32_t)dev_state.treg_state.temp_reg_ld==3)
        emit send_LDC_CMD(LDC_SET_EMIT_STATE,state);

}

void LDC20I_Worker::proc_LDCSetEmitMode(bool continous_emit)
{

    emit send_LDC_CMD(LDC_SET_EMIT_MODE, continous_emit? 0x02: 0x01);

}

void LDC20I_Worker::proc_LDCSetFreq(float emit_freqHz)
{

    emit send_LDC_CMD(LDC_SET_EMIT_MFREQ, emit_freqHz*1000);

}
void LDC20I_Worker::proc_LDCSetPeriod(float emit_period)
{

    emit send_LDC_CMD(LDC_SET_EMIT_PERIOD, emit_period);

}
void LDC20I_Worker::proc_LDCSetTempReg(uint8_t state)
{

    emit send_LDC_CMD(LDC_SET_TEMP_REG_STATE, state);

}



void LDC20I_Worker::proc_COMDeviceConnected()
{
    m_NotificationTimer->start();
   // emit send_LDC_CMD(LDC_SET_TEMP_REG_STATE,1);

    //emit send_LDC_CMD(LDC_GET_MEASUREMENTS,0);

}

void LDC20I_Worker::proc_recieved_LDC_DS_ANS(LDC_ANS_enum cmd,  LDC_STATE dev_state)
{

    m_TimeoutTimer->stop();

   // qDebug() << Q_FUNC_INFO << QThread::currentThreadId();

    switch (cmd){
    case LDC_ANS_MEASUREMENTS:
      //  emit send_LDC_CMD(LDC_GET_DISTANCE,0);
       // next_cmd = LDC_GET_DISTANCE;
        emit send_LDC_State(dev_state);
        this->dev_state=dev_state;

        break;
    case LDC_ANS_DISTANCE:
      //  emit send_LDC_CMD(LDC_GET_MEASUREMENTS,0);
       // next_cmd = LDC_GET_MEASUREMENTS;

        emit send_LDC_State(dev_state);
        this->dev_state=dev_state;
        break;
    // case LDC_ANS_TEMP_REG_STATE:
    //     //emit send_LDC_CMD(LDC_GET_MEASUREMENTS,0);
    //     next_cmd = LDC_GET_MEASUREMENTS;
    //     break;
    // case LDC_ANS_NO_REPLY:
    //    // emit send_LDC_CMD(LDC_GET_MEASUREMENTS,0);
    //     next_cmd = next_cmd;//LDC_GET_MEASUREMENTS;
    //     break;
    default:
        break;
    }
//m_NotificationTimer->setInterval(1000);

  //  m_NotificationTimer->start();

   // QThread::usleep(100);


}
