#include "ldc20i_controller.h"
#include <qdebug.h>
#include <QThread>
//#define DEBUG_LOG

LDC20I_Controller::LDC20I_Controller(QString port, quint32 baudrate, QObject *parent):
   port(port),
   baudrate(baudrate),
   com(port,baudrate,this)
{



    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
        crc7_generate_table();

        connect(this, &LDC20I_Controller::comInitConnection, &com, &LDC20I_QSerial::initConnection, Qt::QueuedConnection );
        connect(this, &LDC20I_Controller::comCloseConnection, &com, &LDC20I_QSerial::closeConnection, Qt::QueuedConnection );

        connect(this, &LDC20I_Controller::sendData, &com, &LDC20I_QSerial::proc_sendData , Qt::QueuedConnection);

        //connect(&com, &LDC20I_QSerial::serialANS, this, &LDC20I_Controller::proc_serialANS, Qt::QueuedConnection );
        connect(&com, &LDC20I_QSerial::deviceConnected, this, &LDC20I_Controller::proc_deviceConnected, Qt::QueuedConnection );
        connect(this, &LDC20I_Controller::send_LDC_CMD, this, &LDC20I_Controller::proc_LDC_CMD, Qt::QueuedConnection);

        connect(&com,&LDC20I_QSerial::dataParse, this, &LDC20I_Controller::proc_dataParse, Qt::QueuedConnection) ;



       // connect(&this, &LDC20I_QSerial::deviceConnected, this, &LDC20I_Controller::proc_deviceConnected, Qt::QueuedConnection );



        qDebug() << Q_FUNC_INFO << "LDC-20I QSerial thread id: " << QThread::currentThreadId();

        m_NotificationTimer = new QTimer(this);
        m_NotificationTimer->setInterval(10000);

        memset((uint8_t*)&dev_state, 0x00, sizeof(LDC_STATE));

        connect(m_NotificationTimer, &QTimer::timeout, this, &LDC20I_Controller::proc_timerUpdate);
        rolling_init(&rs);

    });

    QObject::connect(&thread, &QThread::finished, this, &LDC20I_Controller::proc_Thread_Finished);

    thread.start();


    //timer.start(200);
}



LDC20I_Controller::~LDC20I_Controller()
{
    qDebug() << Q_FUNC_INFO;
    thread.exit();
    thread.wait();
}

void LDC20I_Controller::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;
    m_NotificationTimer->stop();
    delete m_NotificationTimer;

}



void LDC20I_Controller::proc_timerUpdate()
{
    //emit send_LDC_CMD(LDC_GET_MEASUREMENTS,0);
    //timer.stop();
    buf.clear();
    emit recieved_LDC_DS_ANS(LDC_ANS_NO_REPLY, dev_state);
}

void LDC20I_Controller::proc_deviceConnected()
{

   emit comDeviceConnected();
}
bool  LDC20I_Controller::bufClear(int32_t* cycle_limit ){

    (*cycle_limit)--;
     if(*cycle_limit<0)
    {
            buf.clear();
         return false;
     }
            else
         return true;
}


void LDC20I_Controller::proc_dataParse(QByteArray ba)
{
    //    qDebug() << Q_FUNC_INFO << "LDC-20I QSerial thread id: " << QThread::currentThreadId();
    buf.append(ba);
    if(buf.length() < sizeof(LDC_HEAD_STR))
        return;
    while (buf.length()>0)
    {
        auto head_byte = (uint8_t) buf.front();
        if(head_byte != CONST_HDR)
        {
            buf.remove(0,1               );
        }
        else {
            if(buf.length() > sizeof(LDC_HEAD_STR))
            {
                LDC_HEAD_STR *head=(LDC_HEAD_STR *)buf.data();
                if (head->LEN > 50)
                {
                    buf.remove(0,1);
                    continue;
                }
                else
                {
                    if(head->LEN + 1 > buf.length()){
                         if(bufClear(&cycle_limit)) break;

                    }
                    else
                    {
                        QByteArray ans(buf);
                        //buf.clear();
                        buf.remove(0,head->LEN+2);
                        proc_serialANS(ans);
                        cycle_limit= 10;
                    }
                }

            }
            else
                if(bufClear(&cycle_limit)) break;
        }


    }
#ifdef DEBUG_LOG

    qDebug()<< Q_FUNC_INFO << buf.length();
#endif
}

void LDC20I_Controller::checkTRegState()
{
    if(dev_state.treg_state.temp_reg_ld == 0)
        dev_state.emit_en = 0;
}

void LDC20I_Controller::proc_serialANS(QByteArray buf)
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();
#endif
    LDC_HEAD_STR* head;
    MEASUREMENTS_LDC_RAW *ms_ptr;
    DISTANCE_LDC_RAW *dist_ptr;
    LDC_TEMP_REG_STATE_ANS *reg_state_ptr;
    uint32_t *impulse_period_ptr;
    uint32_t *impulse_mfreq_ptr;
    uint8_t * emit_mode_ptr;

   const char* data_ptr = buf.data()+sizeof(LDC_HEAD_STR);

    head = (LDC_HEAD_STR*) buf.data();
  //  uint32_t crc_pos = buf.length()-1;
  //    uint8_t crc = crc7_calculate_buf( buf.slice(0,crc_pos));
   // if(crc!= buf.data()[crc_pos])
   //     return;


    switch ((LDC_ANS_enum)head->CMD)
    {
        case LDC_ANS_MEASUREMENTS:
           // qDebug() <<"LDC_ANS_MEASUREMENTS";
            ms_ptr = (MEASUREMENTS_LDC_RAW*)(data_ptr);
            measurementsDecode(ms_ptr);

            break;

        case LDC_ANS_DISTANCE:
           // qDebug() <<"LDC_ANS_DISTANCE";
            dist_ptr = (DISTANCE_LDC_RAW*)(data_ptr);
            distanceDecode(dist_ptr);

            break;

        case LDC_ANS_TEMP_REG_STATE:
            qDebug() <<"LDC_ANS_TEMP_REG_STATE";
            reg_state_ptr = (LDC_TEMP_REG_STATE_ANS*)(data_ptr);
            dev_state.treg_state=*reg_state_ptr;

            break;

        case  LDC_ANS_EMIT_STATE:
            qDebug() <<"LDC_ANS_EMIT_STATE";
            dist_ptr = (DISTANCE_LDC_RAW*)(data_ptr);
            distanceDecode(dist_ptr);
            break;

        case LDC_ANS_EMIT_PERIOD:
            qDebug() <<"LDC_ANS_EMIT_PERIOD";
            impulse_period_ptr = (uint32_t*)data_ptr;
            //dev_state.impulse_p.impulse_period = *impulse_mfreq_ptr;

            break;

        case LDC_ANS_EMIT_MFREQ:
            qDebug() <<"LDC_ANS_EMIT_MFREQ";
            impulse_mfreq_ptr = (uint32_t*)data_ptr;
            dev_state.impulse_p.impulse_mfreq = *impulse_mfreq_ptr;

            break;
        case LDC_ANS_EMIT_MODE:
            qDebug() <<"LDC_ANS_EMIT_MODE";
            emit_mode_ptr = (uint8_t*)data_ptr;
            dev_state.continous_emit = (*emit_mode_ptr == 0x02) ? true: false;

            break;
        default:
            break;
    }
        checkTRegState();
        m_NotificationTimer->setInterval(200);
#ifdef DEBUG_LOG
       qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
        emit recieved_LDC_DS_ANS((LDC_ANS_enum)head->CMD, dev_state);


}


qint32 LDC20I_Controller::proc_initConnection()
{
    qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();;
    emit comInitConnection();
    return 0;
}




qint32 LDC20I_Controller::proc_closeConnection()
{
    qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();;
    emit comCloseConnection();
    return 0;
}




void LDC20I_Controller::crc7_generate_table()
{
    uint8_t i, j;
    uint8_t CRCPoly = 0x89;
    //CRC-7 polynomial value
    // generate a table value for all 256 possible byte values
    for (i = 1; i != 0; i++)
    {
        crc7_table[i] = (i & 0x80) ? i ^ CRCPoly : i;
        for (j = 1; j < 8; ++j)
        {
            crc7_table[i] <<= 1;
            if (crc7_table[i] & 0x80)
            {
                crc7_table[i] ^= CRCPoly;
            }
        }
    }
}
uint8_t LDC20I_Controller::crc7_calc(uint8_t crc, uint8_t data)
{
    return crc7_table[(crc << 1) ^ data];

}
uint8_t LDC20I_Controller::crc7_calculate_buf(QByteArray message)
{
    uint8_t i;
    uint8_t crc7 = 0;
    for (i = 0; i < message.length(); ++i)
    {
        crc7 = crc7_calc(crc7, message[i]);
    }
    return crc7;
}

QByteArray LDC20I_Controller::pack_data(uint32_t data)
{
    // представить число int32_t в виде последовательности 4 байтов формата uint7_t
    char buf[5];
    buf[0] = data & 0x7F;
    data >>= 7;
    buf[1] = data & 0x7F;
    data >>= 7;
    buf[2] = data & 0x7F;
    data >>= 7;
    buf[3] = data & 0x7F;
    return QByteArray(buf);
    ;
}
int32_t LDC20I_Controller::unpack_data(uint8_t * buf)
{
    // получить число формата int32_t из последовательности байтов формата uint7_t
    int32_t data;
    data = ((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << (8-1)) | ((uint32_t)buf[2] << (16-2)) | ((uint32_t)buf[3] << (24-3));
    data = (int32_t)((uint32_t)data << 4) >> 4;
    return data;
}





void LDC20I_Controller::measurementsDecode(MEASUREMENTS_LDC_RAW* raw)
{
    auto ms = &dev_state.meas;
    //MEASUREMENTS_LDC meas;
    ms->emit_state_code = raw->emit_state_code;
    ms->rad_tc_state_code = raw->rad_tc_state_code;
    ms->ld_tc_state_code = raw->ld_tc_state_code;
    ms->temp_power_sw = unpack_data(raw->temp_power_sw_raw)/100;
    ms->temp_ld = unpack_data(raw->temp_ld_raw)/100;
    ms->rad_heater_curr = raw->rad_heater_curr/10;
    ms->voltage = raw->voltage/10;
    ms->tem_ld_curr = unpack_data(raw->tem_ld_curr_raw)/10;
    ms->tem_ld_voltage_drop = unpack_data(raw->tem_ld_voltage_drop_raw)/10;
    ms->temp_fd = unpack_data(raw->temp_fd_raw)/100;
    dev_state.treg_state.temp_reg_ld =ms->ld_tc_state_code ;
    dev_state.treg_state.temp_reg_rad =ms->rad_tc_state_code ;

    //return meas;
}






// Инициализация структуры
void LDC20I_Controller:: rolling_init(RollingStats *rs) {
    rs->index = 0;
    rs->count = 0;
    rs->sum = 0.0f;
    rs->sum_sq = 0.0f;
}

// Обновление при поступлении одного нового значения дистанции
void LDC20I_Controller:: rolling_update(RollingStats *rs, float new_value, float *mean, float *stddev) {
    // если окно заполнено — удаляем старое значение
    if (rs->count == WINDOW_SIZE) {
        float old = rs->buffer[rs->index];
        rs->sum -= old;
        rs->sum_sq -= old * old;
    } else {
        rs->count++;
    }

    // добавляем новое значение
    rs->buffer[rs->index] = new_value;
    rs->sum += new_value;
    rs->sum_sq += new_value * new_value;

    // сдвигаем индекс по кругу
    rs->index = (rs->index + 1) % WINDOW_SIZE;

    // считаем среднее и СКО
    *mean = rs->sum / rs->count;
    float variance = (rs->sum_sq / rs->count) - (*mean) * (*mean);
    *stddev = sqrtf(variance > 0 ? variance : 0);
}


void LDC20I_Controller::distanceDecode(DISTANCE_LDC_RAW* raw)
{
    auto dist = &dev_state.dist;

    dist->impulse_emit_cnt_total = raw->impulse_emit_cnt_total;
    dist->impulse_recieve_cnt_total = raw->impulse_recieve_cnt_total;
    dist->distance = raw->distance*1.79;

    rolling_update(&rs, dist->distance,  &dist->distance_mean,  &dist->distance_stddev);


}

QByteArray LDC20I_Controller::createLDCSendBuf(LDC_HEAD_STR ldc_head, QByteArray ldc_data)
{
    QByteArray ba;

    ba.append((char*)&ldc_head, sizeof(ldc_head));

    if(ldc_data != nullptr)
        ba.append(ldc_data);

    uint8_t crc7 = crc7_calculate_buf(ba);

    ba.append(crc7);
    return ba;
}



void LDC20I_Controller::proc_LDC_CMD(LDC_CMD_enum cmd, quint32 par)
{
    m_NotificationTimer->start();
    m_NotificationTimer->setInterval(200);
#ifdef DEBUG_LOG

    qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();;
#endif
    //await.lock();
    switch (cmd) {
    case LDC_GET_MEASUREMENTS:
        proc_LDC_GET_MEASUREMENTS();
        break;
    case LDC_GET_DISTANCE:
        proc_LDC_GET_DISTANCE();
        break;
    case LDC_GET_TEMP_REG_STATE:
        proc_LDC_GET_TEMP_REG_STATE();
        break;
    case LDC_SET_TEMP_REG_STATE:
        proc_LDC_SET_TEMP_REG_STATE(par);
        break;
    case LDC_GET_EMIT_STATE:
        proc_LDC_GET_EMIT_STATE();
        break;
    case LDC_SET_EMIT_STATE:
        proc_LDC_SET_EMIT_STATE(par);
        break;
    case LDC_SET_EMIT_PERIOD:
        proc_LDC_SET_IMPULSE_PERIOD(par);
        break;
    case LDC_SET_EMIT_MFREQ:
        proc_LDC_SET_IMPULSE_MFREQ(par);
        break;
    case LDC_SET_EMIT_MODE:
        proc_LDC_SET_EMIT_MODE(par);
        break;

    default:
        break;
    }
   // QThread::usleep(100);
QThread::msleep(10);
}



qint32 LDC20I_Controller::proc_LDC_GET_MEASUREMENTS()
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_GET_MEASUREMENTS;
    ldc_head.LEN = 2;
    QByteArray ba= createLDCSendBuf(ldc_head);
    emit sendData(ba);

    return 0;
}


qint32 LDC20I_Controller::proc_LDC_GET_DISTANCE()
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_GET_DISTANCE;
    ldc_head.LEN = 2;
    QByteArray ba= createLDCSendBuf(ldc_head);
    emit sendData(ba);
    //qDebug() <<"GET_DISTANCE BA: "<<" " << ba.toHex();
    return 0;
}

qint32 LDC20I_Controller::proc_LDC_GET_TEMP_REG_STATE()
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_TEMP_REG_STATE;
    ldc_head.LEN = 2;
    QByteArray ba= createLDCSendBuf(ldc_head);
    emit sendData(ba);
    return 0;
}

qint32 LDC20I_Controller::proc_LDC_SET_TEMP_REG_STATE(uint8_t state)
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_TEMP_REG_STATE;
    ldc_head.LEN = 3;
    QByteArray ldc_data;
    ldc_data.append(state);
    QByteArray ba= createLDCSendBuf(ldc_head,ldc_data);
    emit sendData(ba);

    return 0;
}


qint32 LDC20I_Controller::proc_LDC_GET_EMIT_STATE()
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_EMIT_STATE;
    ldc_head.LEN = 2;
    QByteArray ba= createLDCSendBuf(ldc_head);
    emit sendData(ba);
    return 0;
}

qint32 LDC20I_Controller::proc_LDC_SET_EMIT_MODE(uint8_t mode)
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif

    if(mode == 0)
        return 0;
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_SET_EMIT_MODE;
    ldc_head.LEN = 3;
    QByteArray ldc_data;
    ldc_data.append(mode);
    QByteArray ba= createLDCSendBuf(ldc_head,ldc_data);

    dev_state.continous_emit = (mode==2)?true:false;;

    emit sendData(ba);


    return 0;
}


qint32 LDC20I_Controller::proc_LDC_SET_EMIT_STATE(uint8_t state)
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_EMIT_STATE;
    ldc_head.LEN = 3;
    QByteArray ldc_data;
    ldc_data.append(state);
    QByteArray ba= createLDCSendBuf(ldc_head,ldc_data);

    dev_state.emit_en = (state==1)?true:false;
    emit sendData(ba);


    return 0;
}

qint32 LDC20I_Controller::proc_LDC_SET_IMPULSE_PERIOD(uint32_t val)
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_SET_EMIT_PERIOD;
    ldc_head.LEN = 6;
    QByteArray ldc_data;
    ldc_data.append((char*)&val, sizeof(uint32_t));
    QByteArray ba= createLDCSendBuf(ldc_head,ldc_data);
    emit sendData(ba);
    return 0;
}


qint32 LDC20I_Controller::proc_LDC_SET_IMPULSE_MFREQ(uint32_t val)
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO;
#endif
    LDC_HEAD_STR ldc_head;
    ldc_head.HDR = CONST_HDR;
    ldc_head.CMD= LDC_SET_EMIT_MFREQ;
    ldc_head.LEN = 6;
    QByteArray ldc_data;
    ldc_data.append((char*)&val, sizeof(uint32_t));
    QByteArray ba= createLDCSendBuf(ldc_head,ldc_data);
    emit sendData(ba);
    return 0;
}


