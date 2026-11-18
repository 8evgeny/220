#include "gpf_controller.h"
#include <qdebug.h>
#include <QThread>
//#define DEBUG_LOG
#include <byteswap.h>
#include <type_traits>
#include <cstdlib>
#include <qstring.h>
#include <stdlib.h>


/**
 * @brief Initialize CRC calculation
 * @param poly - CRC polynomial
 * @param init_value - initial value
 * @param xor_value - final XOR value
 */
static void init_crc_calculation(uint8_t poly, uint8_t init_value, uint8_t xor_value)
{
    _init_value = init_value;
    _xor_value = xor_value;

    // Fill CRC table
    const uint32_t bits_mask = (1 << _poly_width) - 1;
    const uint32_t top_bit = 1 << (_poly_width - 1);

    uint32_t index;
    for (index = 0; index < CRC_TABLE_SIZE; ++index)
    {
        uint32_t value = index << (_poly_width - 8);
        uint32_t bit_index;

        for (bit_index = 0; bit_index < 8; ++bit_index)
        {
            if (value & top_bit)
            {
                value = (value << 1) ^ poly;
            }
            else
            {
                value = value << 1;
            }
            value &= bits_mask;
        }
        _CRC8Table[index] = (uint8_t)value;
    }
    _is_inited = 1;
}

/**
 * @brief Calculate CRC8 checksum
 * @param data - input data buffer
 * @param size - size of data in bytes
 * @return CRC8 checksum
 */
static uint8_t crc_calc(uint8_t *data, uint8_t size)
{
    if (!_is_inited)
    {
        // Handle uninitialized case - either return error or use defaults
        return 0;
    }

    uint8_t crc = _init_value;

    while (size--)
    {
        crc = _CRC8Table[crc ^ *data++];
    }

    return crc ^ _xor_value;
}


GyroPlatformVCD_Controller::GyroPlatformVCD_Controller(QString port, quint32 baudrate, QObject *parent):
   m_port(port),
   m_baudrate(baudrate)

{



    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this]{
       // crc7_generate_table();
        com = new GyroPlatformVCD_QSerial(m_port,m_baudrate,this);

        init_crc_calculation(0x31, 0xFF, 0x00);

        connect(com, &GyroPlatformVCD_QSerial::sig_READY, this, &GyroPlatformVCD_Controller::proc_InitReady, Qt::QueuedConnection );

    });
    thread.start();


    //timer.start(200);
}







void GyroPlatformVCD_Controller::proc_InitReady()
{


    connect(this, &GyroPlatformVCD_Controller::comInitConnection, com, &GyroPlatformVCD_QSerial::initConnection, Qt::QueuedConnection );
    connect(this, &GyroPlatformVCD_Controller::comCloseConnection, com, &GyroPlatformVCD_QSerial::closeConnection, Qt::QueuedConnection );

    connect(this, &GyroPlatformVCD_Controller::sendData, com, &GyroPlatformVCD_QSerial::proc_sendData , Qt::QueuedConnection);

    //connect(&com, &LDC20I_QSerial::serialANS, this, &LDC20I_Controller::proc_serialANS, Qt::QueuedConnection );
    connect(com, &GyroPlatformVCD_QSerial::deviceConnected, this, &GyroPlatformVCD_Controller::proc_deviceConnected, Qt::QueuedConnection );
    connect(this, &GyroPlatformVCD_Controller::send_GyroPlatformVCD_CMD, this, &GyroPlatformVCD_Controller::proc_GyroPlatformVCD_CMD, Qt::QueuedConnection);


    connect(com,&GyroPlatformVCD_QSerial::dataParse, this, &GyroPlatformVCD_Controller::proc_dataParse, Qt::QueuedConnection) ;



    // connect(&this, &LDC20I_QSerial::deviceConnected, this, &LDC20I_Controller::proc_deviceConnected, Qt::QueuedConnection );



    qDebug() << Q_FUNC_INFO << "GyroPlatformVCD Controller thread id: " << QThread::currentThreadId();

    m_AnswerTimeoutTimer = new QTimer(this);
    m_AnswerTimeoutTimer->setInterval(10000);
    m_AnswerTimeoutTimer->setSingleShot(true);
    memset((uint8_t*)&dev_state, 0x00, sizeof(dev_state));

    connect(m_AnswerTimeoutTimer, &QTimer::timeout, this, &GyroPlatformVCD_Controller::proc_timeout_event);
    emit sig_READY();

}

void GyroPlatformVCD_Controller::proc_timeout_event()
{
    //TODO:

    //buf.clear();
    req_cmd.isRequest = false;
    req_cmd.cmd = -1;
    emit recieved_GyroPlatformVCD_ANS(MINI_ANS_NO_REPLY);
}

void GyroPlatformVCD_Controller::proc_deviceConnected()
{

   emit comDeviceConnected();
}
bool  GyroPlatformVCD_Controller::bufClear(int32_t* cycle_limit ){

    (*cycle_limit)--;
     if(*cycle_limit<0)
    {
            buf.clear();
         return false;
     }
            else
         return true;
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
    uint16_t crc16_calc = crc16(buf);
    rd_res.crc_ok = crc16 == crc16(buf);
    return rd_res;
}


void GyroPlatformVCD_Controller::parse_serialANS(const VCD_HEAD_STR *head_ans, QByteArray &buf)
{
    
    char* payload_ptr = buf.data()+sizeof(VCD_HEAD_STR);
    
    VPF_MODE_STR *mode_ptr = nullptr;
    VPF_CONTROL_POS_STR *ctrlpos_ptr=  nullptr;
    VPF_CONTROL_SPEED_STR *ctrlspeed_ptr = nullptr;
    VPF_CONTROL_TRACK_STR  *ctrltrack_ptr = nullptr;
    VPF_MEMS_BOARD_STATUS_STR *status_ptr = nullptr;
    
    if((head_ans->dst.device== VCD_HOST_ADDR) && (head_ans->src.device== VCD_GYRO_ADDR))
    {
        switch (head_ans->dst.function) {
        case GyroPlatformVCD_MODE:

            break;
        case GyroPlatformVCD_CONTROL_POSITION:

            break;
        case GyroPlatformVCD_CONTROL_SPEED:

            break;
        case GyroPlatformVCD_CONTROL_TRACKING:

            break;
        case GyroPlatformVCD_STATUS:

            break;
        case GyroPlatformVCD_VERSION:

            break;
        default:
            break;
        }
    }

}

void GyroPlatformVCD_Controller::proc_dataParse(QByteArray ba)
{
    buf.append(ba);
    int isWaitRd = -1;
    bool isSuccess = false;


#ifdef DEBUG_LOG
        qDebug() << Q_FUNC_INFO << "Mini640 Parse thread id: " << QThread::currentThreadId();
#endif

    if(buf.length() < sizeof(VCD_HEAD_STR))
        return;
    while (buf.length()>0)
    {
        auto head_byte = (uint8_t) buf.front();
        if((head_byte != GyroPlatformVCD_SOF))
        {
            buf.remove(0,1 );
            qDebug() << "Remove start";
        }
        else {
            if(buf.length() >= sizeof(VCD_HEAD_STR))
            {
            //    qDebug() << "Try Parse";

                QByteArray ans(buf);

                const VCD_HEAD_STR *head=(VCD_HEAD_STR *)buf.data();

                if(buf.length() >=  head->byteCount + sizeof(VCD_HEAD_STR) )
                {
                    parse_serialANS(head, buf );
                    buf.remove(0,await_ans_len)

                    cycle_limit= 10;
                }
                else
                    return;


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




void GyroPlatformVCD_Controller::proc_GyroPlatformVCD_CMD(GyroPlatformVCD_FUNC_MAP_enum func, quint32 cmd, quint32 par)
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
    auto ba =   proc_GyroPlatformVCD_SendCMD(req);

   // qDebug() << "GyroPlatformVCD BA cmd:"<< ba;
    emit sendData(ba);

    m_AnswerTimeoutTimer->start();
    m_AnswerTimeoutTimer->setInterval(200);
}


template <class CMD_TYPE_RAW> static QByteArray create_BA_CMD_Raw(const REQ_CMD &req)
{
    CMD_TYPE_RAW cmd = {0};
    cmd.sof = GyroPlatformVCD_SOF;
    cmd.cmd = req.cmd;
    if constexpr(!std::is_same_v<CMD_TYPE_RAW, GyroPlatformVCD_FLASH_CMD_RAW>)
    {
        // U12_CL *addr_ptr= (U12_CL *)&(req.addr);
        // cmd.addr[0] = addr_ptr->msb_val;
        // cmd.addr[1] = addr_ptr->mid_val;
        // cmd.addr[2] = addr_ptr->lsb_val;
        char addr[4];
        sprintf(addr,"%03X",req.addr);
        memcpy(cmd.addr, addr, sizeof(cmd.addr));

    }

    if constexpr(std::is_same_v<CMD_TYPE_RAW, GyroPlatformVCD_WRITE_CMD_RAW>)
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


QByteArray GyroPlatformVCD_Controller::proc_GyroPlatformVCD_SendCMD(const REQ_CMD &req )
{
    req_cmd = req;
    req_cmd.isRequest = true;
    QByteArray ba;
    switch (req.cmd) {
    case GyroPlatformVCD_WR:
        ba = create_BA_CMD_Raw<GyroPlatformVCD_WRITE_CMD_RAW>(req);
        break;
    case GyroPlatformVCD_RD:
        ba = create_BA_CMD_Raw<GyroPlatformVCD_READ_CMD_RAW>(req);
        break;
    case GyroPlatformVCD_FLUSH:
        ba = create_BA_CMD_Raw<GyroPlatformVCD_FLASH_CMD_RAW>(req);
        break;
    case GyroPlatformVCD_RESTORE:
        ba = create_BA_CMD_Raw<GyroPlatformVCD_FLASH_CMD_RAW>(req);
        break;
    default:
        break;
    }

    return ba;
}



