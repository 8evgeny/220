#include "lens_j200.h"
#include "qdebug.h"


LensJ200::LensJ200(QObject *parent)
    : QObject{parent},
    m_currentCommandID(0),
    m_isContinuous(false),
    m_pendingRequest(false),
    minPosition(0),
    maxPosition(32000)
{
    QObject::connect(&serialPort, &QSerialPort::readyRead, this, &LensJ200::read);
    m_status.zoomPosition = 0;
    m_status.focusPosition = 0;
}



LensJ200::~LensJ200()
{
    qDebug() << Q_FUNC_INFO;
  //  serialPort.close();
}


bool LensJ200::open(QString portName)
{
    qDebug()<<Q_FUNC_INFO<<"Open serial port:"<<portName;
    serialPort.setPortName(portName);
    serialPort.setBaudRate(QSerialPort::Baud19200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);

    m_status.isConnected = serialPort.open(QIODevice::ReadWrite);

    return m_status.isConnected; //serialPort.open(QIODevice::ReadWrite);
}

void LensJ200::close()
{
    if(serialPort.isOpen())
        serialPort.close();
}

void LensJ200::send(QByteArray req)
{

    if(!serialPort.isOpen())
        return;
    if (req.length() < 1) {
        return;
    }

    char initPkt[] = {0x24, 0, 0, 0, 0};
    QByteArray ba(initPkt, sizeof(initPkt));
    ba[1] = m_currentCommandID = req.at(0);

    req = req.mid(1, req.length());
    if ( req.length() ) {
        ba[2] = req.length();
        ba[3] = (req.length()) >> 8;
        ba.insert(4, req);
    }

    ba[ba.length()-1] = getChecksum(ba, false);
    m_request = ba;
    //qDebug()<<Q_FUNC_INFO<<"Write data:"<<ba.toHex(' ');
    serialPort.write(ba);
}

QByteArray m_serialbuf;
void LensJ200::read()
{
    if(!serialPort.isOpen())
        return;
    const QByteArray data = serialPort.readAll();
   // qDebug()<<Q_FUNC_INFO<<data.toHex(' ');

    m_serialbuf.append(data);
    bool ok =false ;
    int iCnt = 0;
    while(m_serialbuf.length() >5)
    {

        if(m_serialbuf.data()[0]!= 0x24)
        {
//            m_serialbuf.removeFirst();
            m_serialbuf.remove(0,1);
            continue;
        }

        Lens::J200_HEAD* head = (Lens::J200_HEAD*)m_serialbuf.data();
        int len_msg =  (head->len+sizeof(Lens::J200_HEAD)+sizeof(Lens::J200_TAIL));


        if(len_msg>20)
        {
            m_serialbuf.remove(0,1);
            continue;
        }

        if( len_msg > m_serialbuf.length())
        {
            if(iCnt >10)
            {

                m_serialbuf.remove(0,1);
                iCnt=0;
                continue;
            }
            else
            {
                iCnt ++;
                return;
            }
        }
        iCnt=0;

        QByteArray ans = m_serialbuf.mid(0, len_msg);

        bool ok = parseResponse(ans);

        if(ok)
        {
            emit sendResponse(ok);
            m_serialbuf.remove(0, len_msg);

        }
        else
            m_serialbuf.remove(0,1);
        // if(!ok && m_serialbuf.length()>0)
        //     m_serialbuf.removeFirst();
        // else
        //    m_serialbuf.remove(0,6);
    }

    // emit sendResponse(ok);
   // qDebug()<<"";
}

uint8_t LensJ200::getChecksumOld(QByteArray data, bool isResponse)
{
    //qDebug()<<Q_FUNC_INFO<<data.toHex(' ');
    uint8_t cs = data.at(0);

    for (int i = 1; i < data.length()-1-(quint8)isResponse; i++) {
   // for (int i = 1; i < data.length()-1; i++) {
        cs ^= data.at(i);
        //    qDebug()<<Q_FUNC_INFO<<"data ="<<QString("%1").arg((quint8)data[i], 0, 16)<<"cs="<<QString("%1").arg((quint8)cs, 0, 16);
    }

    //qDebug()<<Q_FUNC_INFO<<"CS:"<<QString("%1").arg((quint8)cs, 0, 16);;
    return cs;
}

uint8_t LensJ200::getChecksum(QByteArray data, bool isResponse)
{
    //qDebug()<<Q_FUNC_INFO<<data.toHex(' ');
    uint8_t cs = data.at(0);

    //for (int i = 1; i < data.length()-1-(quint8)isResponse; i++) {
     for (int i = 1; i < data.length()-1; i++) {
        cs ^= data.at(i);
        //    qDebug()<<Q_FUNC_INFO<<"data ="<<QString("%1").arg((quint8)data[i], 0, 16)<<"cs="<<QString("%1").arg((quint8)cs, 0, 16);
    }

    //qDebug()<<Q_FUNC_INFO<<"CS:"<<QString("%1").arg((quint8)cs, 0, 16);;
    return cs;
}


/*
 * Return the code of the executed command or 0 in case of error
 */
bool LensJ200::parseResponse(QByteArray resp)
{
    // check packet length
    if (resp.length() < minResponsePacketSizeBytes) {
        qDebug()<<">>>1";
        return false;
    }

    Lens::J200_HEAD* head = (Lens::J200_HEAD*)resp.data();

    if(head->synq != 0x24) return false;


    //
    // 24 29 24 29 06 00 b8 59 3d 46 21 00 fe b0
    // if ( resp.contains(QByteArray::fromRawData("\x24\x29\x06\x00", 4)) ) {
    //     int idx = resp.indexOf(QByteArray::fromRawData("\x24\x29\x06\x00", 4));
    //     resp = resp.mid(idx, resp.length());
    // //    qDebug()<<"111"<<resp.toHex(' ');
    // }

    // read and check crc

    int cs = getChecksum(resp, true) ;
    int oldcs = getChecksumOld(resp, true) ;

    if ( (cs != resp.at(resp.length()-1)) && (oldcs != resp.at(resp.length()-1 ))) {
        qDebug()<<"BAD Checksum";
        return false;
    }
    // check operation status
    if (resp.at(resp.length()-2) != successOperation) {
        return false;
    }
    // // check executed command ID
    // if ((resp.at(Packet::command) != m_currentCommandID) && (resp.at(Packet::command) != 0x29)) {
    //     qDebug()<<">>>>>>>>>>>>>>>>>"<<m_currentCommandID<<(quint8)resp.at(Packet::command);
    //     return false;
    // }

    // parse commands
    switch (resp.at(Packet::command)) {
    case 0x08:
    case 0x09:
        //    qDebug()<<"ddddd";
        //    m_status.focusPosition = (resp.at(9) << 8) | resp.at(8);
        //    m_status.zoomPosition = (resp.at(7) << 8) | resp.at(6) ;
        //    m_status.temperature = resp.at(10);
        break;
    case 0x29:
    case 0x23:
    case 0x24:
    case 0x21:
    case 0x22:

      //  qDebug()<<"Update Lens State";
        m_status.focusPosition = (resp.at(7) << 8) | resp.at(6);
        m_status.zoomPosition = (resp.at(5) << 8) | resp.at(4) ;
        m_status.temperature = resp.at(8);


        // switch (m_currentCommandID) {
        // case (quint8)Lens::Command::ZoomOut:
        // //    m_status.zoomPosition = m_status.zoomMinPosition = (resp.at(5) << 8) | resp.at(4);
        // //    qDebug()<<"min"<<m_status.zoomMinPosition;
        //     break;
        // case (quint8)Lens::Command::ZoomIn:
        // //    m_status.zoomPosition = m_status.zoomMaxPosition = (resp.at(5) << 8) | resp.at(4);
        // //    qDebug()<<"max"<<m_status.zoomMaxPosition;
        //     break;
        // case (quint8)Lens::Command::ZoomInContinuous:
        // //    m_status.zoomPosition = (resp.at(5) << 8) | resp.at(4);
        //  //   m_status.zoomMaxPosition = 32000;
        //     break;
        // case (quint8)Lens::Command::ZoomOutContinuous:
        // //    m_status.zoomPosition = (resp.at(5) << 8) | resp.at(4);
        // //    m_status.zoomMaxPosition = 0;
        //     break;
        // case (quint8)Lens::Command::Report:
        //     m_status.focusPosition = (resp.at(7) << 8) | resp.at(6);
        //     m_status.zoomPosition = (resp.at(5) << 8) | resp.at(4) ;
        //     m_status.temperature = resp.at(8);
        //     break;
        // default:
        //     break;
        // }

        break;
    case 0x45:
        m_status.temperature = resp.at(6);
        break;
    case 0x2A:
        m_status.focalLength = (resp.at(5) << 8) | resp.at(4);
    default:
        break;
    }

    return true;
}
