#include "ldc20i_qserial.h"
#include <QDebug>

LDC20I_QSerial::LDC20I_QSerial(QString port, quint32 baudrate, QObject *parent)
    : port {port}
    , baudrate {baudrate}
{
    Q_UNUSED(parent);
    moveToThread(&thread);
    connect(&thread, &QThread::started, this, [this] {
        qDebug() << Q_FUNC_INFO << "LDC-20I serial thread id:" << QThread::currentThreadId()
                 << "Port:" << this->port << "Baudrate:" << this->baudrate;
    });
    connect(&thread, &QThread::finished, this, &LDC20I_QSerial::proc_Thread_Finished);
    thread.start();
}

LDC20I_QSerial::~LDC20I_QSerial()
{
    thread.quit();
    thread.wait();
}

void LDC20I_QSerial::proc_Thread_Finished()
{
    qDebug() << Q_FUNC_INFO;
}

void LDC20I_QSerial::serialBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

qint32 LDC20I_QSerial::initConnection()
{
    if (serialPort && serialPort->isOpen())
        return 0;

    if (!serialPort)
        serialPort = new QSerialPort(this);

    connect(serialPort, &QSerialPort::bytesWritten,
            this, &LDC20I_QSerial::serialBytesWritten, Qt::QueuedConnection);
    connect(serialPort, &QSerialPort::readyRead,
            this, &LDC20I_QSerial::serialReceived, Qt::QueuedConnection);

    serialPort->setPortName(port);
    serialPort->setBaudRate(baudrate);
    serialPort->setParity(QSerialPort::Parity::NoParity);
    serialPort->setDataBits(QSerialPort::DataBits::Data8);
    serialPort->setStopBits(QSerialPort::StopBits::OneStop);

    if (!serialPort->open(QSerialPort::OpenModeFlag::ReadWrite)) {
        qWarning() << "Failed to open LDC20I serial port" << port << serialPort->errorString();
        return -1;
    }

    emit deviceConnected();
    return 0;
}

qint32 LDC20I_QSerial::closeConnection()
{
    if (!serialPort)
        return 0;

    disconnect(serialPort, &QSerialPort::readyRead, this, &LDC20I_QSerial::serialReceived);
    disconnect(serialPort, &QSerialPort::bytesWritten, this, &LDC20I_QSerial::serialBytesWritten);
    if (serialPort->isOpen())
        serialPort->close();
    return 0;
}

void LDC20I_QSerial::serialReceived()
{
    if (!serialPort || !serialPort->isOpen())
        return;

    emit dataParse(serialPort->readAll());
}

void LDC20I_QSerial::proc_sendData(QByteArray ba)
{
    if (!serialPort || !serialPort->isOpen())
        return;

    serialPort->write(ba);
}
