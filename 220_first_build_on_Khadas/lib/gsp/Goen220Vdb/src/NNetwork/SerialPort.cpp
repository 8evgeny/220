
#include "SerialPort.h"


#include <QSerialPort>
#include <QSerialPortInfo>


SerialPort::SerialPort( QObject *parent):
    NCore::CNewThread( parent)
  , m_pSerialPort( nullptr)
  , m_connected( false)
  , m_portName( QString::fromUtf8( "COM0"))
  , m_portBaudRate( QSerialPort::Baud115200)
{
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if( ports.size() > 0)
        m_portName = ports.at( 0).portName();
}


SerialPort::~SerialPort()
{
    m_pSerialPort->deleteLater();
    m_pSerialPort = nullptr;
}


void SerialPort::setPortName( const QString &name)
{
    m_portName = name;
}


QString SerialPort::portName() const
{
    return m_portName;
}


const QList<QString> SerialPort::getAvailablePorts()
{
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    QList<QString> portNames;
    for( int port = 0;port < ports.size(); port ++)
        portNames.append( ports.at( port).portName());

    return portNames;
}


void SerialPort::connect( const bool con)
{
    if( con)
    {
        m_pSerialPort->setPortName( m_portName);
        m_pSerialPort->setBaudRate( m_portBaudRate);
        if( m_pSerialPort->open( QIODevice::ReadWrite) == false)
        {
            const QString error = QString::fromUtf8( "Не удалось открыть последовательный порт.\n%1").
                                    arg( m_pSerialPort->errorString());
            emit errorOccurred( error);
            return;
        }

        m_connected = true;
        emit stateConnectChanged( m_connected);
        return;
    }

    m_pSerialPort->close();
    m_connected = false;
    emit stateConnectChanged( m_connected);
}


void SerialPort::sendData( const QByteArray data)
{
    if( m_connected == false)
        return;

    m_pSerialPort->write( data);
    m_pSerialPort->waitForBytesWritten( 10);
}


void SerialPort::initializationInNewThread()
{
    m_pSerialPort = new QSerialPort();

    m_pSerialPort->setBaudRate( QSerialPort::Baud115200);
    m_pSerialPort->setDataBits( QSerialPort::Data8);
    m_pSerialPort->setFlowControl( QSerialPort::NoFlowControl);
    m_pSerialPort->setParity( QSerialPort::NoParity);
    m_pSerialPort->setStopBits( QSerialPort::OneStop);

    QObject::connect( m_pSerialPort, &QSerialPort::readyRead, this, &SerialPort::readyRead);
}


void SerialPort::readyRead()
{
    const QByteArray data = m_pSerialPort->readAll();
    if( data.size() < 1)
        return;

    emit receivedData( data);
}
