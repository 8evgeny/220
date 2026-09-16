#include "mini640_controller.h"
#include <qdebug.h>
#include <QThread>
//#define DEBUG_LOG
#include <byteswap.h>
#include <type_traits>
#include <cstdlib>
#include <qstring.h>
#include <stdlib.h>

MINI640_Controller::MINI640_Controller(QString port, quint32 baudrate, QObject *parent):
   m_port(port),
   m_baudrate(baudrate)

{



    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
       // crc7_generate_table();
        com = new MINI640_QSerial(m_port,m_baudrate,this);


        connect(this, &MINI640_Controller::comInitConnection, com, &MINI640_QSerial::initConnection, Qt::QueuedConnection );
        connect(this, &MINI640_Controller::comCloseConnection, com, &MINI640_QSerial::closeConnection, Qt::QueuedConnection);
        connect(com, &MINI640_QSerial::sig_READY, this, &MINI640_Controller::proc_InitReady, Qt::QueuedConnection );
        connect(this, &MINI640_Controller::sendData, com, &MINI640_QSerial::proc_sendData , Qt::QueuedConnection);
        connect(com, &MINI640_QSerial::deviceConnected, this, &MINI640_Controller::proc_deviceConnected, Qt::QueuedConnection );
        connect(this, &MINI640_Controller::send_MINI640_CMD, this, &MINI640_Controller::proc_MINI640_CMD, Qt::QueuedConnection);
        connect(this, &MINI640_Controller::send_GOWIN_CMD, this, &MINI640_Controller::proc_GOWIN_CMD, Qt::QueuedConnection);


        connect(com,&MINI640_QSerial::dataParse, this, &MINI640_Controller::proc_dataParse, Qt::QueuedConnection) ;

    });

    QObject::connect(&thread, &QThread::finished, this, &MINI640_Controller::proc_Thread_Finished);

    thread.start();


    //timer.start(200);
}

MINI640_Controller::~MINI640_Controller()
{
    qDebug() << Q_FUNC_INFO;
    thread.exit();
    thread.wait();
}

void MINI640_Controller::proc_Thread_Finished()
{
    m_AnswerTimeoutTimer->stop();
    delete(m_AnswerTimeoutTimer);

}

void MINI640_Controller::proc_InitReady()
{

    mini640_addr_list = getAddressArray();



    //connect(&com, &LDC20I_QSerial::serialANS, this, &LDC20I_Controller::proc_serialANS, Qt::QueuedConnection );




   //connect(&this, &LDC20I_QSerial::deviceConnected, this, &LDC20I_Controller::proc_deviceConnected, Qt::QueuedConnection );



    qDebug() << Q_FUNC_INFO << "MINI640 Controller thread id: " << QThread::currentThreadId();

    m_AnswerTimeoutTimer = new QTimer(this);
    m_AnswerTimeoutTimer->setInterval(10000);
    m_AnswerTimeoutTimer->setSingleShot(true);
    memset((uint8_t*)&dev_state, 0x00, sizeof(dev_state));

    connect(m_AnswerTimeoutTimer, &QTimer::timeout, this, &MINI640_Controller::proc_timeout_event);
    emit sig_READY();
}

void MINI640_Controller::proc_timeout_event()
{
    //TODO:

    //buf.clear();
    req_cmd.isRequest = false;
    req_cmd.cmd = -1;
    emit recieved_MINI640_ANS(MINI_ANS_NO_REPLY);
}

void MINI640_Controller::proc_deviceConnected()
{

   emit comDeviceConnected();
}
bool  MINI640_Controller::bufClear(int32_t* cycle_limit ){

    (*cycle_limit)--;
     if(*cycle_limit<0)
    {
            buf.clear();
         return false;
     }
            else
         return true;
}


static uint16_t MODBUS_CRC16(QByteArray buf)
{
    static const uint16_t table[256] = {
                                        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
                                        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
                                        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
                                        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
                                        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
                                        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
                                        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
                                        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
                                        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
                                        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
                                        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
                                        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
                                        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
                                        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
                                        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
                                        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
                                        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
                                        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
                                        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
                                        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
                                        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
                                        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
                                        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
                                        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
                                        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
                                        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
                                        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
                                        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
                                        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
                                        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
                                        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
                                        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

    uint8_t x_or = 0;
    uint16_t crc = 0xFFFF;
    char* buf_ptr = buf.data();
    int32_t len = buf.length();
    while(len--)
    {
        x_or = (*buf_ptr++) ^ crc;
        crc >>= 8;
        crc ^= table[x_or];
    }

    return crc;
}



static uint16_t MODBUS_CRC16_RAW(QByteArray &buf)
{
    auto buf_raw = buf.sliced(0, buf.length() - 4);
    return MODBUS_CRC16(buf_raw);
}



//Преобразование uint4[] в uint
template <class dt> static dt baToDt(const uint8_t* buf, uint32_t size=0)
{

    char *str = new char(sizeof(dt)*2+1);
    if(size == 0)
        memcpy(str, buf, sizeof(dt)*2);
    else
        memcpy(str, buf, size);
    dt ans = (int)strtol((const char*) str, NULL, 16);

    free(str);
    return ans;
}

static RESP_RD_CMD parseRawAnsGowin (const REQ_CMD &req_cmd, const GOWIN_RESP_RD_CMD_RAW *resp, QByteArray &buf )
{
    RESP_RD_CMD rd_res = {0};
    rd_res.addr = req_cmd.addr;
    rd_res.cmd = req_cmd.cmd;
    rd_res.ack = resp->ack;

     rd_res.rd_data = baToDt<uint32_t>(resp->rd_data, 6);

    uint16_t crc16 = (baToDt<uint16_t>(resp->crc16));
    uint16_t crc16_calc = MODBUS_CRC16_RAW(buf);
    rd_res.crc_ok = crc16 == MODBUS_CRC16_RAW(buf);
    return rd_res;
}


template <class  RESP_TYPE_RAW> static RESP_RD_CMD  parseRawAns(const REQ_CMD &req_cmd, const RESP_TYPE_RAW *resp, QByteArray &buf )
{
    RESP_RD_CMD rd_res = {0};
    rd_res.addr = req_cmd.addr;
    rd_res.cmd = req_cmd.cmd;
    rd_res.ack = resp->ack;

    if constexpr (std::is_same_v<RESP_RD_CMD_RAW, RESP_TYPE_RAW>)
    {

        rd_res.rd_data = baToDt<uint8_t>(resp->rd_data);
    }
    uint16_t crc16 = (baToDt<uint16_t>(resp->crc16));
    uint16_t crc16_calc = MODBUS_CRC16_RAW(buf);
    rd_res.crc_ok = crc16 == MODBUS_CRC16_RAW(buf);
    return rd_res;
}


void MINI640_Controller::proc_dataParse(QByteArray ba)
{
    buf.append(ba);
    int isWaitRd = -1;
    bool isSuccess = false;
    RESP_RD_CMD resp = {0};
    switch (req_cmd.cmd) {
    case MINI640_RD:
        await_ans_len = sizeof(RESP_RD_CMD_RAW);
        isWaitRd = 1;
        break;
    case MINI640_WR:
    case MINI640_FLUSH:
    case MINI640_RESTORE:
        await_ans_len = sizeof(RESP_CMD_RAW);
        isWaitRd=0;
        break;

    case GOWIN_CMD:

        await_ans_len = sizeof(GOWIN_RESP_RD_CMD_RAW);
        isWaitRd=2;
        break;

    default:
        isWaitRd = -1;
        break;
    }

#ifdef DEBUG_LOG
        qDebug() << Q_FUNC_INFO << "Mini640 Parse thread id: " << QThread::currentThreadId();
#endif

    if(buf.length() < await_ans_len)
        return;
    while (buf.length()>0)
    {
        auto head_byte = (uint8_t) buf.front();
        if((head_byte != MINI640_SOF) && (head_byte != GOWIN_SOF))
        {
            buf.remove(0,1 );
            qDebug() << "Remove start";
        }
        else {
            if(buf.length() >= await_ans_len)
            {
            //    qDebug() << "Try Parse";

                QByteArray ans(buf);

                if(isWaitRd == 1)
                {
                   // qDebug() << "Enter rd";
                    const RESP_RD_CMD_RAW *raw=(RESP_RD_CMD_RAW *)buf.data();
                    resp = parseRawAns<RESP_RD_CMD_RAW>(req_cmd, raw, buf);
                    proc_serialANS(resp);
                }
                else if(isWaitRd == 0)
                {
                    const RESP_CMD_RAW *raw=(RESP_CMD_RAW *)buf.data();
                    resp = parseRawAns<RESP_CMD_RAW>(req_cmd, raw, buf);
                    proc_serialANS(resp);
                }
                else if(isWaitRd == 2)
                {
                    const GOWIN_RESP_RD_CMD_RAW *raw=(GOWIN_RESP_RD_CMD_RAW *)buf.data();
                    resp = parseRawAnsGowin(req_cmd, raw, buf);
                    proc_serialANSGowin(resp);
                }

                else {
                    buf.remove(0,1 );
                    continue;
                }

                buf.remove(0,await_ans_len);
                cycle_limit= 10;



            }
            else
                if(bufClear(&cycle_limit)) break;
        }


    }
#ifdef DEBUG_LOG

    qDebug()<< Q_FUNC_INFO << buf.length();
#endif
}



template <class  WT>  void setSB(WT *word , const uint8_t byte , const uint8_t pos)
{
    char * ptr_b = (char*)word;
    ptr_b[pos] = byte;
}


void MINI640_Controller::proc_serialANSGowin(const RESP_RD_CMD &resp)
{

    m_AnswerTimeoutTimer->stop();
    mDsUpdateLock.lock();
    if((resp.crc_ok))
    {

        dev_state.isCoolerEn = (resp.ack == 'Y');
        dev_state.cooler_timer = resp.rd_data;
    }
   // qDebug()<<dev_state.cooler_timer;
    mDsUpdateLock.unlock();
   // if((resp.crc_ok) && (resp.ack == 'Y'))
    if((resp.crc_ok))
        emit recieved_MINI640_ANS(MINI_ANS_OK);
    else
        emit recieved_MINI640_ANS(MINI_ANS_ERROR);
}


void MINI640_Controller::proc_serialANS(const RESP_RD_CMD &resp)
{
#ifdef DEBUG_LOG
    qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();
#endif

    bool isUpdateDS=false;

    mDsUpdateLock.lock();
#ifdef DEBUG
    qDebug() << "ack " << resp.ack << "rd_data " << resp.rd_data << "crc_ok " << resp.crc_ok;
#endif
    if(resp.cmd == MINI640_RD)
        switch ((MINI640_REG_MAP_enum)resp.addr)
        {
            case CCE_CONF:
    #ifdef DEBUG_LOG
                qDebug() <<"CCE_СONF";
    #endif

                CCE_СONF_Decode(resp.rd_data);
                isUpdateDS=true;
                break;

            case CCE_Input_CTRL:
    #ifdef DEBUG_LOG
                qDebug() <<"CCE_Input_CTRL";
    #endif

                CCE_Input_CTRL_Decode(resp.rd_data);
                isUpdateDS=true;

                break;

            case CCE_PBIT:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_PBIT";
    #endif

                CCE_PBIT_Decode(resp.rd_data);
                isUpdateDS=true;

                break;
            case CCE_CBIT:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_CBIT";
    #endif

                CCE_CBIT_Decode(resp.rd_data);
                isUpdateDS=true;

                break;
            case CCE_Temp_status:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_Temp_status";
    #endif

                CCE_TEMP_STAT_Decode(resp.rd_data);
                isUpdateDS=true;

                break;
            case CCE_Power_status:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_Power_status";
    #endif

                CCE_POWER_STAT_Decode(resp.rd_data);
                isUpdateDS=true;

                break;
            case MONITOR_REF2_5V_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "MONITOR_REF2_5V_LSB";
    #endif
                {
                    uint16_t ref2_5V = dev_state.mon_raw.ref2_5V;
                    setSB<uint16_t>(&ref2_5V, resp.rd_data, U16_LSB);
                    dev_state.mon_raw.ref2_5V = ref2_5V;
                }
                break;
            case MONITOR_REF2_5V_MSB:
    #ifdef DEBUG_LOG
               qDebug() << resp.cmd << "MONITOR_REF2_5V_MSB";
    #endif

               {
                    uint16_t ref2_5V = dev_state.mon_raw.ref2_5V;
                    setSB<uint16_t>(&ref2_5V, resp.rd_data, U16_MSB);
                    dev_state.mon_raw.ref2_5V = ref2_5V;
                }
                dev_state.mon.ref2_5V = dev_state.mon_raw.ref2_5V/1000.0;
                isUpdateDS=true;

                break;
            case MONITOR_3_3V_ADC_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "MONITOR_3_3V_ADC_LSB";
    #endif
                {
                    uint16_t ref3_3V = dev_state.mon_raw.ref3_3V;
                    setSB<uint16_t>(&ref3_3V, resp.rd_data, U16_LSB);
                    dev_state.mon_raw.ref3_3V = ref3_3V;

                }
                break;
            case MONITOR_3_3V_ADC_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "MONITOR_3_3V_ADC_MSB";
    #endif
                {
                    uint16_t ref3_3V = dev_state.mon_raw.ref3_3V;
                    setSB<uint16_t>(&ref3_3V, resp.rd_data, U16_MSB);
                    dev_state.mon_raw.ref3_3V = ref3_3V;
                    dev_state.mon.ref3_3V = ref3_3V/1000.0;
                }
                isUpdateDS=true;

                break;
            case MONITOR_5V_N4V_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "MONITOR_5V_N4V_LSB";
    #endif
                {
                    uint16_t ref5_N4V = dev_state.mon_raw.ref5_N4V;
                    setSB<uint16_t>(&ref5_N4V, resp.rd_data, U16_LSB);
                    dev_state.mon_raw.ref5_N4V = ref5_N4V;
                }
                break;
            case MONITOR_5V_N4V_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "MONITOR_5V_N4V_MSB";
    #endif

                {
                    uint16_t ref5_N4V = dev_state.mon_raw.ref5_N4V;
                    setSB<uint16_t>(&ref5_N4V, resp.rd_data, U16_MSB);
                    dev_state.mon.ref5_N4V = ref5_N4V/1000.0;
                    dev_state.mon_raw.ref5_N4V = ref5_N4V;
                    isUpdateDS=true;
                }
                break;
            case PATTERN_CTRL:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "PATTERN_CTRL";
    #endif

                FPA_PATTERN_CTRL_Decode(resp.rd_data);
                isUpdateDS=true;

                break;
            case GPOL_DAC_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "GPOL_DAC_MSB";
    #endif
                {
                    uint16_t gpol_DAC=  dev_state.mon_raw.gpol_DAC;
                    setSB<uint16_t>(&gpol_DAC, resp.rd_data, U16_MSB);
                    dev_state.mon_raw.gpol_DAC = gpol_DAC;
                }


                break;
            case GPOL_DAC_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "GPOL_DAC_LSB";
    #endif

                {

                    uint16_t gpol_DAC=  dev_state.mon_raw.gpol_DAC;
                    setSB<uint16_t>(&gpol_DAC, resp.rd_data, U16_LSB);
                    dev_state.mon_raw.gpol_DAC = gpol_DAC;
                    dev_state.mon.gpol_DAC = gpol_DAC / 43.6;
                    isUpdateDS=true;
                }

                break;

            case CCE_Ref_1:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_Ref_1";
    #endif

                dev_state.hsv.CCE_Ref_1 = resp.rd_data;
                break;
            case CCE_VersionSft_1:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_VersionSft_1";
    #endif

                dev_state.hsv.CCE_VersionSft_1 = resp.rd_data;
                break;
            case CCE_VersionSft_2:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_VersionSft_2";
    #endif

                dev_state.hsv.CCE_VersionSft_2 = resp.rd_data;

                break;
            case CCE_VersionFPGA_1:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_VersionFPGA_1";
    #endif

                dev_state.hsv.CCE_VersionFPGA_1 = resp.rd_data;

                break;
            case CCE_VersionFPGA_2:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_VersionFPGA_2";
    #endif

                dev_state.hsv.CCE_VersionFPGA_2 = resp.rd_data;
                isUpdateDS=true;

                break;


            case CCE_Temp_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_Temp_LSB";
    #endif

                {
                    uint16_t cce_temp= dev_state.mon_raw.cce_temp;
                    setSB<uint16_t>(&cce_temp, resp.rd_data, U16_LSB);
                    dev_state.mon_raw.cce_temp = cce_temp;
                }
                break;
            case CCE_Temp_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_Temp_MSB";
    #endif
                {
                    uint16_t cce_temp= dev_state.mon_raw.cce_temp;
                    setSB<uint16_t>(&cce_temp, resp.rd_data, U16_MSB);
                    dev_state.mon_raw.cce_temp = cce_temp;
                    dev_state.mon.cce_temp = cce_temp * 0.01;
                }
                isUpdateDS=true;

                break;
            case CCE_Temp_Thres_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_Temp_Thres_LSB";
    #endif
                {
                    uint16_t cce_temp_th = dev_state.th_raw.cce_temp_th;
                    setSB<uint16_t>(&cce_temp_th, resp.rd_data, U16_LSB);
                    dev_state.th_raw.cce_temp_th =cce_temp_th;

                }
                break;
            case CCE_Temp_Thres_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "CCE_Temp_Thres_MSB";
    #endif
                {
                    uint16_t cce_temp_th = dev_state.th_raw.cce_temp_th;
                    setSB<uint16_t>(&cce_temp_th, resp.rd_data, U16_MSB);
                    dev_state.th_raw.cce_temp_th =cce_temp_th;
                    dev_state.th.cce_temp_th = cce_temp_th * 0.01;
                }
                break;
            case A1_Temp_Poly_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A1_Temp_Poly_LSB";
    #endif

                dev_state.temp_poly_raw.a1_temp_poly[I24_LSB] = resp.rd_data;
                break;
            case A1_Temp_Poly_Mid:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A1_Temp_Poly_Mid";
    #endif

                dev_state.temp_poly_raw.a1_temp_poly[I24_MID] = resp.rd_data;
                break;
            case A1_Temp_Poly_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A1_Temp_Poly_MSB";
    #endif

                dev_state.temp_poly_raw.a1_temp_poly[I24_MSB] = resp.rd_data;
                dev_state.temp_poly.a1_temp_poly = ((I24*)dev_state.temp_poly_raw.a1_temp_poly)->ival / 1000.0f;

                break;
            case A2_Temp_Poly_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A2_Temp_Poly_LSB";
    #endif

                dev_state.temp_poly_raw.a2_temp_poly[I24_LSB] = resp.rd_data;

                break;
            case A2_Temp_Poly_Mid:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A2_Temp_Poly_Mid";
    #endif

                dev_state.temp_poly_raw.a2_temp_poly[I24_MID] = resp.rd_data;

                break;
            case A2_Temp_Poly_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A2_Temp_Poly_MSB";
    #endif

                dev_state.temp_poly_raw.a2_temp_poly[I24_MSB] = resp.rd_data;
                dev_state.temp_poly.a2_temp_poly = ((I24*)dev_state.temp_poly_raw.a2_temp_poly)->ival / 1000.0f;
                break;
            case A3_Temp_Poly_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A3_Temp_Poly_LSB";
    #endif

                dev_state.temp_poly_raw.a3_temp_poly[I24_LSB] = resp.rd_data;

                break;
            case A3_Temp_Poly_Mid:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A3_Temp_Poly_Mid";
    #endif

                dev_state.temp_poly_raw.a2_temp_poly[I24_MID] = resp.rd_data;

                break;
            case A3_Temp_Poly_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A3_Temp_Poly_MSB";
    #endif

                dev_state.temp_poly_raw.a3_temp_poly[I24_MSB] = resp.rd_data;
                dev_state.temp_poly.a3_temp_poly = ((I24*)dev_state.temp_poly_raw.a3_temp_poly)->ival / 1000.0f;
                break;
            case A4_Temp_Poly_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A4_Temp_Poly_LSB";
    #endif

                dev_state.temp_poly_raw.a4_temp_poly[I24_LSB] = resp.rd_data;

                break;
            case A4_Temp_Poly_Mid:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A4_Temp_Poly_Mid";
    #endif

                dev_state.temp_poly_raw.a4_temp_poly[I24_MID] = resp.rd_data;

                break;
            case A4_Temp_Poly_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A4_Temp_Poly_MSB";
    #endif

                dev_state.temp_poly_raw.a4_temp_poly[I24_MSB] = resp.rd_data;
                dev_state.temp_poly.a4_temp_poly = ((I24*)dev_state.temp_poly_raw.a4_temp_poly)->ival / 1000.0f;
                break;
            case A5_Temp_Poly_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A5_Temp_Poly_LSB";
    #endif

                dev_state.temp_poly_raw.a5_temp_poly[I24_LSB] = resp.rd_data;
                break;
            case A5_Temp_Poly_Mid:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A5_Temp_Poly_Mid";
    #endif

                dev_state.temp_poly_raw.a5_temp_poly[I24_MID] = resp.rd_data;

                break;
            case A5_Temp_Poly_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "A5_Temp_Poly_MSB";
    #endif

                dev_state.temp_poly_raw.a5_temp_poly[I24_MSB] = resp.rd_data;
                dev_state.temp_poly.a5_temp_poly = ((I24*)dev_state.temp_poly_raw.a5_temp_poly)->ival / 1000.0f;
                isUpdateDS=true;

                break;
            case FPA_CONF:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "FPA_CONF";
    #endif

                FPA_CONF_Decode(resp.rd_data);
                isUpdateDS=true;

                break;
            case INT_PERIOD_TIME_1:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "INT_PERIOD_TIME_1";
    #endif

                ((char*)&dev_state.exp.INT_PERIOD_TIME)[U24_LSB] = resp.rd_data;
                break;
            case INT_PERIOD_TIME_2:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "INT_PERIOD_TIME_2";
    #endif

                ((char*)&dev_state.exp.INT_PERIOD_TIME)[U24_MID] = resp.rd_data;
                break;
            case INT_PERIOD_TIME_3:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "INT_PERIOD_TIME_3";
    #endif

                ((char*)&dev_state.exp.INT_PERIOD_TIME)[U24_MSB] = resp.rd_data;
                break;
            case INT_HIGH_LEVEL_1:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "INT_HIGH_LEVEL_1";
    #endif

                ((char*)&dev_state.exp.INT_HIGH_LEVEL)[U24_LSB] = resp.rd_data;
                break;
            case INT_HIGH_LEVEL_2:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "INT_HIGH_LEVEL_2";
    #endif

                ((char*)&dev_state.exp.INT_HIGH_LEVEL)[U24_MID] = resp.rd_data;
                break;
            case INT_HIGH_LEVEL_3:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "INT_HIGH_LEVEL_3";
    #endif

                ((char*)&dev_state.exp.INT_HIGH_LEVEL)[U24_MSB] = resp.rd_data;
                isUpdateDS=true;

                break;

            case X1MIN_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "X1MIN_LSB";
    #endif

                ((char*)&dev_state.roi.X1MIN)[U16_LSB] = resp.rd_data;
                break;
            case X1MIN_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "X1MIN_MSB";
    #endif

                ((char*)&dev_state.roi.X1MIN)[U16_MSB] = resp.rd_data;

                break;
            case Y1MIN_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "Y1MIN_LSB";
    #endif

                ((char*)&dev_state.roi.Y1MIN)[U16_LSB] = resp.rd_data;

                break;
            case Y1MIN_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "Y1MIN_MSB";
    #endif

                ((char*)&dev_state.roi.Y1MIN)[U16_MSB] = resp.rd_data;
                break;
            case X1MAX_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "X1MAX_LSB";
    #endif

                ((char*)&dev_state.roi.X1MAX)[U16_LSB] = resp.rd_data;
                break;
            case X1MAX_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "X1MAX_MSB";
    #endif

                ((char*)&dev_state.roi.X1MAX)[U16_MSB] = resp.rd_data;

                break;
            case Y1MAX_LSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "Y1MAX_LSB";
    #endif

                ((char*)&dev_state.roi.Y1MAX)[U16_LSB] = resp.rd_data;

                break;
            case Y1MAX_MSB:
    #ifdef DEBUG_LOG
                qDebug() << resp.cmd << "Y1MAX_MSB";
    #endif

                ((char*)&dev_state.roi.Y1MAX)[U16_MSB] = resp.rd_data;
                isUpdateDS=true;

                break;


            default:
                break;
        }

   // if((resp.cmd == MINI640_RD) && (resp.crc_ok) && (resp.ack=='Y'))
   //     dev_state.raw_reg[resp.addr] = resp.rd_data;
    #ifdef DEBUG_LOG
           qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
    #endif

    m_AnswerTimeoutTimer->stop();
    mDsUpdateLock.unlock();
    if((resp.crc_ok) && (resp.ack == 'Y'))
        emit recieved_MINI640_ANS(MINI_ANS_OK);
    else
        emit recieved_MINI640_ANS(MINI_ANS_ERROR);
}


qint32 MINI640_Controller::proc_initConnection()
{
    qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();;
    emit comInitConnection();
    return 0;
}




qint32 MINI640_Controller::proc_closeConnection()
{
    qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();;
    m_AnswerTimeoutTimer->stop();
    disconnect(m_AnswerTimeoutTimer, &QTimer::timeout, this, &MINI640_Controller::proc_timeout_event);

    emit comCloseConnection();
    return 0;
}



void MINI640_Controller::FPA_CONF_Decode(uint8_t data)
{
    FPA_CONF_STR* fpa_ctrl = (FPA_CONF_STR*)&data;
    dev_state.readout_mod_sel = fpa_ctrl->readout_mod_sel;
    dev_state.video_size_sel = fpa_ctrl->video_size_sel;
}

    void MINI640_Controller::FPA_PATTERN_CTRL_Decode(uint8_t data)
{
    FPA_PATTERN_CTRL_STR* fpa_ctrl = (FPA_PATTERN_CTRL_STR*)&data;
    dev_state.test_pattern_act = fpa_ctrl->test_pattern_act;
    dev_state.power_on_fpa = fpa_ctrl->power_on_fpa;
}


void MINI640_Controller::CCE_POWER_STAT_Decode(uint8_t data)
{
    CCE_POWER_STAT_STR* cce_ctrl = (CCE_POWER_STAT_STR*)&data;
    dev_state.ref_2V5 = cce_ctrl->ref_2V5;
    dev_state.ref_3V3_ADC = cce_ctrl->ref_3V3_ADC;
    dev_state.ref_5V = cce_ctrl->ref_5V;
    dev_state.ref_5V_M4V = cce_ctrl->ref_5V_M4V;
}

void MINI640_Controller::CCE_TEMP_STAT_Decode(uint8_t data)
{
    CCE_TEMP_STAT_STR* cce_ctrl = (CCE_TEMP_STAT_STR*)&data;
    dev_state.temp_low = cce_ctrl->temp_low;
}

void MINI640_Controller::CCE_CBIT_Decode(uint8_t data)
{
    CCE_CBIT_STR* cce_ctrl = (CCE_CBIT_STR*)&data;
    dev_state.power_supply_stat = cce_ctrl->power_supply_stat;
    dev_state.serdat_err = cce_ctrl->serdat_err;

}

void MINI640_Controller::CCE_PBIT_Decode(uint8_t data)
{
    CCE_PBIT_STR* cce_ctrl = (CCE_PBIT_STR*)&data;
    dev_state.memory_checksum_state = cce_ctrl->memory_checksum_state;
}
void MINI640_Controller::CCE_Input_CTRL_Decode(uint8_t data)
{
    CCE_Input_CTRL_STR* cce_ctrl = (CCE_Input_CTRL_STR*)&data;
    dev_state.frame_source = cce_ctrl->frame_source;
    dev_state.frame_source_mod = cce_ctrl->frame_source_mod;
    dev_state.master_clk_freq = cce_ctrl->master_clk_freq;
    dev_state.master_clk_src = cce_ctrl->master_clk_src;
    dev_state.pixel_position = cce_ctrl->pixel_position;

}


void MINI640_Controller::CCE_СONF_Decode(uint8_t data)
{
    CCE_CONF_STR* cce_conf = (CCE_CONF_STR*)&data;
    dev_state.video_inv = cce_conf->video_inversion;


}

QList<quint32> MINI640_Controller::get_MINIAddr_List()
{
    return getAddressArray();
}


MINI640_DEV_STATE MINI640_Controller::get_MINI_DS()
{
    MINI640_DEV_STATE ds;
    mDsUpdateLock.lock();
    ds = dev_state;
    mDsUpdateLock.unlock();
    return ds;
}


QList<quint32> getAddressArray()
{
    QList<quint32> addr_list;
    addr_list.append(	CCE_CONF	);
    addr_list.append(	CCE_Input_CTRL	);
    addr_list.append(	CCE_PBIT	);
    addr_list.append(	CCE_CBIT	);
    addr_list.append(	CCE_Temp_status	);
    addr_list.append(	CCE_Power_status	);
    addr_list.append(	MONITOR_REF2_5V_LSB	);
    addr_list.append(	MONITOR_REF2_5V_MSB	);
    addr_list.append(	MONITOR_3_3V_ADC_LSB	);
    addr_list.append(	MONITOR_3_3V_ADC_MSB	);
    addr_list.append(	MONITOR_5V_N4V_LSB	);
    addr_list.append(	MONITOR_5V_N4V_MSB	);
    addr_list.append(	PATTERN_CTRL	);
    addr_list.append(	GPOL_DAC_MSB	);
    addr_list.append(	GPOL_DAC_LSB	);
    addr_list.append(	CCE_Ref_1	);
    addr_list.append(	CCE_VersionSft_1	);
    addr_list.append(	CCE_VersionSft_2	);
    addr_list.append(	CCE_VersionFPGA_1	);
    addr_list.append(	CCE_VersionFPGA_2	);
    addr_list.append(	CCE_Temp_LSB	);
    addr_list.append(	CCE_Temp_MSB	);
    addr_list.append(	CCE_Temp_Thres_LSB	);
    addr_list.append(	CCE_Temp_Thres_MSB	);
    addr_list.append(	A1_Temp_Poly_LSB	);
    addr_list.append(	A1_Temp_Poly_Mid	);
    addr_list.append(	A1_Temp_Poly_MSB	);
    addr_list.append(	A2_Temp_Poly_LSB	);
    addr_list.append(	A2_Temp_Poly_Mid	);
    addr_list.append(	A2_Temp_Poly_MSB	);
    addr_list.append(	A3_Temp_Poly_LSB	);
    addr_list.append(	A3_Temp_Poly_Mid	);
    addr_list.append(	A3_Temp_Poly_MSB	);
    addr_list.append(	A4_Temp_Poly_LSB	);
    addr_list.append(	A4_Temp_Poly_Mid	);
    addr_list.append(	A4_Temp_Poly_MSB	);
    addr_list.append(	A5_Temp_Poly_LSB	);
    addr_list.append(	A5_Temp_Poly_Mid	);
    addr_list.append(	A5_Temp_Poly_MSB	);
    addr_list.append(	FPA_CONF	);
    addr_list.append(	INT_PERIOD_TIME_1	);
    addr_list.append(	INT_PERIOD_TIME_2	);
    addr_list.append(	INT_PERIOD_TIME_3	);
    addr_list.append(	INT_HIGH_LEVEL_1	);
    addr_list.append(	INT_HIGH_LEVEL_2	);
    addr_list.append(	INT_HIGH_LEVEL_3	);
    addr_list.append(	X1MIN_LSB	);
    addr_list.append(	X1MIN_MSB	);
    addr_list.append(	Y1MIN_LSB	);
    addr_list.append(	Y1MIN_MSB	);
    addr_list.append(	X1MAX_LSB	);
    addr_list.append(	X1MAX_MSB	);
    addr_list.append(	Y1MAX_LSB	);
    addr_list.append(	Y1MAX_MSB	);
    
    return addr_list;
}

void MINI640_Controller::proc_GOWIN_CMD( quint32 cmd)
{

    REQ_CMD req;
    req.addr = 0xFF;
    req.cmd = GOWIN_CMD;
    req.isRequest = true;
    req.wr_data = cmd;

    req_cmd = req;
    req_cmd.isRequest = true;



   // qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();;
    QByteArray ba = QByteArray(1,(char)cmd);
    emit sendData(ba);

    m_AnswerTimeoutTimer->setInterval(200);
    m_AnswerTimeoutTimer->start();
}

void MINI640_Controller::proc_MINI640_CMD(MINI640_REG_MAP_enum addr, quint32 cmd, quint32 par)
{

#ifdef DEBUG_LOG

    qDebug() <<Q_FUNC_INFO << "Thread id:" << QThread::currentThreadId();;
#endif
    //await.lock();


    REQ_CMD req;
    req.addr = addr;
    req.cmd = cmd;
    req.isRequest = true;
    req.wr_data = par;
    auto ba =   proc_MINI640_SendCMD(req);

   // qDebug() << "MINI640 BA cmd:"<< ba;
    emit sendData(ba);

    m_AnswerTimeoutTimer->setInterval(200);
    m_AnswerTimeoutTimer->start();
}


template <class CMD_TYPE_RAW> static QByteArray create_BA_CMD_Raw(const REQ_CMD &req)
{
    CMD_TYPE_RAW cmd = {0};
    cmd.sof = MINI640_SOF;
    cmd.cmd = req.cmd;
    if constexpr(!std::is_same_v<CMD_TYPE_RAW, MINI640_FLASH_CMD_RAW>)
    {
        // U12_CL *addr_ptr= (U12_CL *)&(req.addr);
        // cmd.addr[0] = addr_ptr->msb_val;
        // cmd.addr[1] = addr_ptr->mid_val;
        // cmd.addr[2] = addr_ptr->lsb_val;
        char addr[4];
        sprintf(addr,"%03X",req.addr);
        memcpy(cmd.addr, addr, sizeof(cmd.addr));

    }

    if constexpr(std::is_same_v<CMD_TYPE_RAW, MINI640_WRITE_CMD_RAW>)
    {
        char data_str[3];
        sprintf(data_str, "%02X", req.wr_data);
        memcpy(cmd.data, data_str, sizeof(cmd.crc16));

        // U8_CL *data_ptr= (U8_CL *)&req.wr_data;
        // cmd.data[0]= data_ptr->msb_val;
        // cmd.data[1]= data_ptr->lsb_val;
    }
    QByteArray ba_wr= QByteArray((char*)&cmd,sizeof(CMD_TYPE_RAW));

    auto cmd_ptr= (CMD_TYPE_RAW*) ba_wr.data();
    uint16_t crc16= MODBUS_CRC16_RAW(ba_wr);


    char crc_str[5];
    sprintf(crc_str, "%04X", crc16);
    memcpy(cmd_ptr->crc16, crc_str, sizeof(cmd.crc16));

    return ba_wr;
}


QByteArray MINI640_Controller::proc_MINI640_SendCMD(const REQ_CMD &req )
{
    req_cmd = req;
    req_cmd.isRequest = true;
    QByteArray ba;
    switch (req.cmd) {
    case MINI640_WR:
        ba = create_BA_CMD_Raw<MINI640_WRITE_CMD_RAW>(req);
        break;
    case MINI640_RD:
        ba = create_BA_CMD_Raw<MINI640_READ_CMD_RAW>(req);
        break;
    case MINI640_FLUSH:
        ba = create_BA_CMD_Raw<MINI640_FLASH_CMD_RAW>(req);
        break;
    case MINI640_RESTORE:
        ba = create_BA_CMD_Raw<MINI640_FLASH_CMD_RAW>(req);
        break;
    default:
        break;
    }

    return ba;
}



