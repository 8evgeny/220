#include "gpf_qserial.h"
#include <qdebug.h>
//#define DEBUG_LOG

GyroPlatformVCD_QSerial::GyroPlatformVCD_QSerial(QString port, quint32 baudrate, QObject *parent)
    : port {port}
    , baudrate {baudrate}
{



     moveToThread(&thread);

    connect(&thread, &QThread::started, this, [this]{
         m_serialPort = new QSerialPort();
         Q_ASSERT(m_serialPort != nullptr );

         connect(m_serialPort, &QSerialPort::bytesWritten, this, &GyroPlatformVCD_QSerial::serialBytesWritten, Qt::QueuedConnection);
         connect(m_serialPort,&QSerialPort::readyRead, this, &GyroPlatformVCD_QSerial::serialReceived, Qt::QueuedConnection) ;

         // sendTimer->setInterval(1000);
         m_serialPort->setPortName(this->port);
         m_serialPort->setBaudRate(this->baudrate);
         m_serialPort->setParity(QSerialPort::Parity::NoParity);
         m_serialPort->setDataBits(QSerialPort::DataBits::Data8);
         m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);



        qDebug() << Q_FUNC_INFO << "GyroPlatformVCD QSerial thread id: " << QThread::currentThreadId();

        emit sig_READY();

 });
     thread.start();
}





void GyroPlatformVCD_QSerial::serialBytesWritten(qint64 bytes) {
#ifdef DEBUG_LOG
        qDebug() << "Sent data: " << bytes << " bytes.";
#endif
    };

qint32 GyroPlatformVCD_QSerial::initConnection()
{


    Q_ASSERT(m_serialPort!=nullptr );


    m_serialPort->open(QSerialPort::OpenModeFlag::ReadWrite);

    qDebug() << "*** Connection init successfully" << Q_FUNC_INFO;


    emit deviceConnected();

    return !m_serialPort->isOpen();

}

qint32 GyroPlatformVCD_QSerial::closeConnection()
{
#ifdef DEBUG_LOG
   qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif

    disconnect(m_serialPort,&QSerialPort::readyRead, this, &GyroPlatformVCD_QSerial::serialReceived) ;
    disconnect(m_serialPort,&QSerialPort::bytesWritten, this, &GyroPlatformVCD_QSerial::serialBytesWritten) ;
    m_serialPort->close();
    free(m_serialPort);
    return 0;
}

void GyroPlatformVCD_QSerial::serialReceived()
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

    emit dataParse(ba);
}

void GyroPlatformVCD_QSerial::proc_sendData(QByteArray ba)
{
    if(m_serialPort == nullptr)
        return;

    if(!m_serialPort->isOpen())
        return;
#ifdef DEBUG_LOG
    qDebug() << "TRY SEND: " << ba.length() << " bytes.";
#endif


  //  qDebug() << "SEND: " << ba.length() << " bytes.";
    m_serialPort->write(ba);
 //   serialPort->waitForBytesWritten();


}

