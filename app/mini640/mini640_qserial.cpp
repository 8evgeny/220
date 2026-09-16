#include "mini640_qserial.h"
#include <qdebug.h>
#include <qtimer.h>

//#define DEBUG_LOG

MINI640_QSerial::MINI640_QSerial(QString port, quint32 baudrate, QObject *parent)
    : port {port}
    , baudrate {baudrate}
{



     moveToThread(&thread);

    connect(&thread, &QThread::started, this, [this]{





 });
     QObject::connect(&thread, &QThread::finished, this, &MINI640_QSerial::proc_Thread_Finished);

     thread.start();
}


void MINI640_QSerial::proc_Thread_Finished()
{
}

MINI640_QSerial::~MINI640_QSerial()
{
    // if (m_serialPort->isOpen()) {
    //     m_serialPort->close();
    //     qDebug() << "Serial port closed in destructor";
    // }
    qDebug() << Q_FUNC_INFO;

    thread.exit();
    thread.wait();
}


void MINI640_QSerial::serialBytesWritten(qint64 bytes) {
#ifdef DEBUG_LOG
        qDebug() << "Sent data: " << bytes << " bytes.";
#endif
    };

qint32 MINI640_QSerial::initConnection()
{



  //  QTimer::singleShot(100, this, [&](){

        m_serialPort = new QSerialPort(this);
        Q_ASSERT(m_serialPort != nullptr );
        connect(m_serialPort, &QSerialPort::bytesWritten, this, &MINI640_QSerial::serialBytesWritten, Qt::QueuedConnection);
        connect(m_serialPort,&QSerialPort::readyRead, this, &MINI640_QSerial::serialReceived, Qt::QueuedConnection) ;
        connect(m_serialPort,&QSerialPort::errorOccurred, this, &MINI640_QSerial::serialError, Qt::QueuedConnection) ;

        qDebug()<<"MINI640 QSerial Connect";
        // sendTimer->setInterval(1000);
        m_serialPort->setPortName(this->port);
        qDebug()<<this->port << this->baudrate;
        m_serialPort->setBaudRate(this->baudrate);
        m_serialPort->setParity(QSerialPort::Parity::NoParity);
        m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
        m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
       // m_serialPort->open(QSerialPort::OpenModeFlag::ReadWrite);

        if (!m_serialPort->open(QIODevice::ReadWrite)) {
            qDebug() << "Error opening serial port:" << m_serialPort->errorString();
            delete m_serialPort;
            m_serialPort = nullptr;
        } else {
            qDebug() << "Port opened successfully!";
        }

        Q_ASSERT(m_serialPort->isOpen() == true );

        qDebug() << Q_FUNC_INFO << "MINI640 QSerial thread id: " << QThread::currentThreadId() << "m_serialPort"<< m_serialPort->isOpen()
            <<" PTR"  << m_serialPort;

        emit sig_READY();

   // });

    qDebug() << "*** Connection init successfully" << Q_FUNC_INFO;


    //emit deviceConnected();

    return 0;// !m_serialPort->isOpen();

}

void MINI640_QSerial::serialError(QSerialPort::SerialPortError error)
{
     qDebug() << Q_FUNC_INFO << QThread::currentThreadId() << error;
}


qint32 MINI640_QSerial::closeConnection()
{
#ifdef DEBUG_LOG
   qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif

    disconnect(m_serialPort,&QSerialPort::readyRead, this, &MINI640_QSerial::serialReceived) ;
    disconnect(m_serialPort,&QSerialPort::bytesWritten, this, &MINI640_QSerial::serialBytesWritten) ;

     if (m_serialPort->isOpen()) {
         m_serialPort->close();
         qDebug() << "MINI640_QSerial port closed";
     }
    return 0;
}

void MINI640_QSerial::serialReceived()
{
#ifdef DEBUG_LOG
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
    if(m_serialPort == nullptr)
        return;

    if(!m_serialPort->isOpen())
        return;

    QByteArray ba = m_serialPort->readAll();
    //buf.append(ba);
 //   qDebug() << "RECEIVE: " << ba << " bytes.";
    emit dataParse(ba);
}

void MINI640_QSerial::proc_sendData(QByteArray ba)
{

    if(m_serialPort == nullptr)
        return;

   // qDebug() << Q_FUNC_INFO << "MINI640 QSerial proc_sendData: " << QThread::currentThreadId() << "m_serialPort"<< m_serialPort->isOpen()
  //           <<" PTR"  << m_serialPort;

    if(!m_serialPort->isOpen())
    {
      //    qDebug() << "TRY SEND but closed: " << ba;
        return;
    }
#ifdef DEBUG_LOG
    qDebug() << "TRY SEND: " << ba.length() << " bytes.";
#endif


 //  qDebug() << "SEND: " << ba.length() << " bytes.";
    m_serialPort->write(ba);
 //   serialPort->waitForBytesWritten();


}

