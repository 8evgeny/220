#include "ldc20i_qserial.h"
#include <qdebug.h>
// #define DEBUG_LOG

LDC20I_QSerial::LDC20I_QSerial(QString port, quint32 baudrate, QObject *parent)
    : port{port}, baudrate{baudrate}
{
    moveToThread(&thread);

    connect(&thread, &QThread::started, this, [this]
            {
                qDebug() << Q_FUNC_INFO << "LDC-20I QSerial thread id: " << QThread::currentThreadId()
                         << "Port:" << this->port << "Baudrate" << this->baudrate;
            } /* lambda */);

    QObject::connect(&thread, &QThread::finished, this, &LDC20I_QSerial::proc_Thread_Finished);

    thread.start();
} /* LDC20I_QSerial::LDC20I_QSerial */

LDC20I_QSerial::~LDC20I_QSerial()
{
    if (serialPort->isOpen())
    {
        closeConnection();
    } /* if */
    qDebug() << Q_FUNC_INFO;
    thread.exit();
    thread.wait();

} /* LDC20I_QSerial::~LDC20I_QSerial */

void LDC20I_QSerial::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;
    // closeConnection();
    //  delete(serialPort);
} /* LDC20I_QSerial::proc_Thread_Finished */

void LDC20I_QSerial::serialBytesWritten(qint64 bytes)
{
#ifdef DEBUG_LOG
    qDebug() << "Sent data: " << bytes << " bytes.";
#endif
} /* LDC20I_QSerial::serialBytesWritten */;

qint32 LDC20I_QSerial::initConnection()
{

    serialPort = new QSerialPort();

    connect(serialPort, &QSerialPort::bytesWritten, this, &LDC20I_QSerial::serialBytesWritten,
            Qt::QueuedConnection);

    connect(serialPort, &QSerialPort::readyRead, this, &LDC20I_QSerial::serialReceived,
            Qt::QueuedConnection);

    // sendTimer->setInterval(1000);
    serialPort->setPortName(port);
    serialPort->setBaudRate(baudrate);
    serialPort->setParity(QSerialPort::Parity::NoParity);
    serialPort->setDataBits(QSerialPort::DataBits::Data8);
    serialPort->setStopBits(QSerialPort::StopBits::OneStop);

    serialPort->open(QSerialPort::OpenModeFlag::ReadWrite);
    emit deviceConnected();

    return !serialPort->isOpen();

} /* LDC20I_QSerial::initConnection */

qint32 LDC20I_QSerial::closeConnection()
{
#ifdef DEBUG_LOG
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif

    disconnect(serialPort, &QSerialPort::readyRead, this, &LDC20I_QSerial::serialReceived);
    disconnect(serialPort, &QSerialPort::bytesWritten, this, &LDC20I_QSerial::serialBytesWritten);
    serialPort->close();
    // free(serialPort);
    return 0;
} /* LDC20I_QSerial::closeConnection */

void LDC20I_QSerial::serialReceived()
{
#ifdef DEBUG_LOG
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
#endif
    if (serialPort == nullptr)
        return;

    if (!serialPort->isOpen())
        return;

    QByteArray ba = serialPort->readAll();
    // buf.append(ba);

    emit dataParse(ba);
} /* LDC20I_QSerial::serialReceived */

void LDC20I_QSerial::proc_sendData(QByteArray ba)
{
    if (serialPort == nullptr)
        return;

    if (!serialPort->isOpen())
        return;
    // qDebug() << "TRY SEND: " << ba.length() << " bytes.";

    // qDebug() << "SEND: " << ba.length() << " bytes.";
    serialPort->write(ba);
    //   serialPort->waitForBytesWritten();

} /* LDC20I_QSerial::proc_sendData */
