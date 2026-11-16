#include "panel.h"

#include <QTimer>
#include <QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include "device_parts/channel/abstractchannel.h"

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logPanel, "Panel")

#define COM_RX_BUFFER_SIZE (256)
#define START_SIGN (char(0xAA))
//#define MIN_MSG_SIZE (4)
#define BUT_CNT (25)
#define MAX_SPEED (1600)
#define MIN_SENCE_SPEED (100)

typedef enum
{
    BUTN_AF_EN        = 0,
    BUTN_AF_DIS       = 24,
    BUTN_TRACKER_EN   = 1,
    BUTN_TV_CH_COLOUR = 23,
    BUTN_TV_CH_BW     = 22,
    BUTN_SWIR_CH      = 2,
    BUTN_MWIR_CH      = 3,
    BUTN_LEVEL_PLUS   = 9,
    BUTN_LEVEL_MINUS  = 18,
    BUTN_GAIN_PLUS    = 5,
    BUTN_GAIN_MINUS   = 12,
    BUTN_DEFAULT_AGC  = 14,
    BUTN_DIG_ZOOM     = 15,
    BUTN_INVER        = 11,
    BUTN_RF_MEAS      = 4
} TButtonCodes;

typedef enum
{
    SET_GSP_SPEED            = 0x19,
    SET_ZOOM_SPEED           = 0x1A,
    SET_FOCUS_SPEED          = 0x1B,
    SET_TRACKING_STROBE_SIZE = 0x1C,
} TCmdCodes;

Panel::Panel(QObject * parent) :
    QObject(parent),
    tmr(new QTimer(this)),
    com_port(new QSerialPort(this)),
    _port_name("COM1"),
    _auto_connect(false),
    _panel_connected(false),
    _enabled(false)
{
    com_port->setBaudRate(QSerialPort::Baud115200);
    com_port->setDataBits(QSerialPort::Data8);
    com_port->setParity(QSerialPort::NoParity);
    com_port->setStopBits(QSerialPort::OneStop);
    com_port->setFlowControl(QSerialPort::NoFlowControl);
    com_port->setReadBufferSize(COM_RX_BUFFER_SIZE);

    connect(tmr, SIGNAL(timeout()), this, SLOT(com_tmr_task()));
    connect(com_port, SIGNAL(readyRead()), this, SLOT(com_port_data_ready()));

    tmr->start(40);
}
bool Panel::is_opened()
{
    if(tmr) {
        tmr->stop();
        tmr->deleteLater();
        tmr = nullptr;
    }
    return com_port->isOpen();
}
bool Panel::open(const QString & name)
{
    close();
    com_port->setPortName(name);
    bool res = com_port->open(QIODevice::ReadWrite);
    if(res)
    {
        qInfo(logPanel(), "open: Open Success");
    }
    else
    {
//        qInfo(logPanel(), "open: Open Error");
    }
    return res;
}
void Panel::close()
{
    if(com_port->isOpen())
    {
        com_port->close();
        qInfo(logPanel(), "close: Close Success");
    }
}
void Panel::set_enable(const bool& enabled)
{
    if(_enabled) close();
    _enabled = enabled;
}
void Panel::com_tmr_task()
{
    static int auto_connect_cnt = 0;
    static int auto_disconnect_cnt = 0;
    if(_enabled) {
        if(com_port->isOpen())
        {
            _panel_connected = false;
            handle_msg();
            if(_panel_connected)
            {
                auto_disconnect_cnt = 0;
            }
            else if(++auto_disconnect_cnt > 1000 / 20)
            {
                auto_disconnect_cnt = 0;
                close();
            }
            auto_connect_cnt = 0;
        }
        else if(_auto_connect)
        {
            if(++auto_connect_cnt > 1000 / 20)
            {
                auto_connect_cnt = 0;
                open(_port_name);
            }
        }
    }
}
void Panel::com_port_data_ready()
{
    reports += com_port->readAll();
}
void Panel::handle_msg()
{
    int lastStart = -1;
    int reportsNum = reports.size();
    for(int i = 0; i < reportsNum; i++)
    {
        if(reports.at(i) == START_SIGN)
        {
            lastStart = i;
            if(i + 2 < reportsNum)
            {
                char msgCode = reports.at(i + 1);
                char msgDataLen = reports.at(i + 2);
                if(i + 3 + msgDataLen < reportsNum)
                {
                    char msgCRC = reports.at(i + 3 + msgDataLen);
                    if(msgCRC == crc(i, 3 + msgDataLen))
                    {
                        handle_cmd(static_cast<uint8_t>(msgCode), i + 3);
                        lastStart = -1;
                        i += static_cast<int>(3 + msgDataLen);
                    }
                }
            }
        }
    }

    if(lastStart != -1)
    {
        reports.remove(0, lastStart);
    }
    else
    {
        reports.clear();
    }
}
void Panel::handle_cmd(const uint8_t & code, const int & data_ind)
{
    if(code == PANEL_CONNECTION_NOTICE)
    {
        _panel_connected = true;
    }

    if(!_enabled) return;

    switch(code)
    {
        case BUTN_TV_CH_COLOUR:
        {
            emit select_channel(CHANNEL_ID_TVRGB);
        } break;
        case BUTN_TV_CH_BW:
        {
            emit select_channel(CHANNEL_ID_TVGRAY);
        } break;
        case BUTN_SWIR_CH:
        {
            emit select_channel(CHANNEL_ID_SWIR);
        }break;
        case BUTN_MWIR_CH:
        {
            emit select_channel(CHANNEL_ID_MWIR);
        }break;
        case BUTN_LEVEL_PLUS:
        {
            emit brightness_up();
        }break;
        case BUTN_LEVEL_MINUS:
        {
            emit brightness_down();
        }break;
        case BUTN_GAIN_PLUS:
        {
            emit contrast_up();
        }break;
        case BUTN_GAIN_MINUS:
        {
            emit contrast_down();
        }break;
        case BUTN_DEFAULT_AGC:
        {
            emit bc_reset();
        }break;
        case BUTN_DIG_ZOOM:
        {
            emit digital_zoom_switch();
        }break;
        case BUTN_INVER:
        {
            emit inversion();
        }break;
        case BUTN_AF_EN:
        {
            emit autofocus_en();
        }break;
        case BUTN_AF_DIS:
        {
            emit autofocus_dis();
        }break;
        case BUTN_TRACKER_EN:
        {
            emit target_switch();
        }break;
        case BUTN_RF_MEAS:
        {
            emit lrf_start();
        }break;
        case SET_GSP_SPEED:
        {
            short zSpeed = static_cast<uint8_t>(reports.at(data_ind))     | (static_cast<uint16_t>(reports.at(data_ind + 1)) << 8);
            short xSpeed = static_cast<uint8_t>(reports.at(data_ind + 2)) | (static_cast<uint16_t>(reports.at(data_ind + 3)) << 8);
            gspSpeedSet(xSpeed, zSpeed);
        } break;
        case SET_ZOOM_SPEED:
        {
            short zoomSpeed = static_cast<uint8_t>(reports.at(data_ind)) | (static_cast<unsigned short>(reports.at(data_ind + 1)) << 8);
            zoomSpeedSet(zoomSpeed);
        } break;
        case SET_FOCUS_SPEED:
        {
            short focusSpeed = static_cast<uint8_t>(reports.at(data_ind)) | (static_cast<unsigned short>(reports.at(data_ind + 1)) << 8);
            focusSpeedSet(focusSpeed);
        } break;
        case SET_TRACKING_STROBE_SIZE:
        {
            short strobeSize = static_cast<uint8_t>(reports.at(data_ind)) | (static_cast<unsigned short>(reports.at(data_ind + 1)) << 8);
            strobeSizeSet(strobeSize);
        } break;
    }
}
char Panel::crc(const int & start, const int & len)
{
    char res = 0;
    for(int i = start; i < start + len; i++)
    {
        res ^= reports.at(i);
    }
    return res;
}
void Panel::gspSpeedSet(const int & _x_speed, const int & _z_speed)
{
    static bool send_zero = true;
    int x_speed = _x_speed;
    int z_speed = _z_speed;
    if(abs(x_speed) < MIN_SENCE_SPEED) x_speed = 0;
    if(abs(z_speed) < MIN_SENCE_SPEED) z_speed = 0;
    if((x_speed != 0) || (z_speed != 0))
    {
        send_zero = true;
        emit gsp_speed_set(-double(x_speed) / (MAX_SPEED+0.2*MAX_SPEED), double(z_speed) / (MAX_SPEED+0.2*MAX_SPEED));
    }
    else if(send_zero)
    {
        send_zero = false;
        emit gsp_speed_set(0, 0);
    }
}
void Panel::zoomSpeedSet(const int & zoomSpeed)
{
    static bool released = false;

    if(zoomSpeed == 0)
    {
        if(!released)
        {
            emit zoom_stop();
            released = true;
        }
        return;
    }

    unsigned char data;
    unsigned int speed = static_cast<unsigned int>(fabs(zoomSpeed / 8) + 32);

    if(speed > 255) data = 255;
    else data = static_cast<unsigned char>(speed);
    if(zoomSpeed > 0)
    {
        emit zoom_in(double(data) / 255.0);
    }
    else
    {
        emit zoom_out(double(data) / 255.0);
    }
    released = false;
}
void Panel::focusSpeedSet(const int & focusSpeed)
{
    static bool released = false;

    if(focusSpeed == 0)
    {
        if(!released)
        {
            emit focus_stop();
            released = true;
        }
        return;
    }

    unsigned char data;
    unsigned int speed = static_cast<unsigned int>(fabs(focusSpeed / 48) + 32);

    if(speed > 255) data = 255;
    else data = static_cast<unsigned char>(speed);
    if(focusSpeed > 0) emit focus_near(double(data) / 255.0);
    else emit focus_far(double(data) / 255.0);
    released = false;
}
void Panel::strobeSizeSet(const short & strobeSize)
{
    static int prev_strobe_size = -1;
    if(prev_strobe_size != strobeSize)
    {
        prev_strobe_size = strobeSize;
        emit target_window_set(strobeSize * 4);
    }
}
Panel::~Panel()
{
    if(tmr->isActive())
    {
        tmr->stop();
    }
    close();
}
