
#include "Goen220GyroPlatformHostPrivate.h"

#include "SerialPort.h"
//#include "DeviceCommunicator.h"
#include "GyroPlatformDeviceCommunicator.h"
#include "MessageParser.h"
#include "CDelimiterWithLenAndCrcMessageParser.h"
#include "Calculator.h"
#include "Command.h"
#include "CommandFactory.h"
#include "DelimiterAndCrc8CommandDecorator.h"


#ifdef USE_TCP_CLIENT

    #include "CClientTcp.h"

#endif


#include <QDebug>


Goen220GyroPlatformHostPrivate::Goen220GyroPlatformHostPrivate( QObject *parent)
    : QObject{ parent}
    , m_pSerialPort( new SerialPort())

#ifdef USE_TCP_CLIENT
    , m_pClient( nullptr)
#endif

    , m_stateConnectToSerialPort( false)
    , m_serialPortErrorString( QString::fromUtf8( ""))
    , m_pDeviceCommunicator( nullptr)
    , m_pParser( nullptr)
    , m_pFactory( nullptr)
    , m_printTxRawDataSerialPort( false)
    , m_printRxRawDataSerialPort( false)
    , m_hostId( 0x13)
    , m_deviceId( 0x14)
{

    m_pDeviceCommunicator = new GyroPlatformDeviceCommunicator( m_hostId, m_deviceId);
    m_pParser = new CDelimiterWithLenAndCrcMessageParser();


#ifdef USE_TCP_CLIENT
    m_pClient =  new NNetwork::CClientTcp();

    m_pClient->setHostName( QString::fromUtf8( "127.0.0.1"));
    m_pClient->setHostPort( 1234);

    QObject::connect(   this, &Goen220GyroPlatformHostPrivate::connectToDevice,
                        m_pClient, &NNetwork::CClientTcp::connectToServer, Qt::QueuedConnection);

    QObject::connect( 	m_pClient, &NNetwork::CClientTcp::stateConnectChanged,
                        this, &Goen220GyroPlatformHostPrivate::stateConnectToSerialPortChanged, Qt::QueuedConnection);

    QObject::connect( 	m_pDeviceCommunicator, &DeviceCommunicator::sendData,
                        m_pClient, &NNetwork::CClientTcp::sendData, Qt::QueuedConnection);

    QObject::connect( 	m_pClient, &NNetwork::CClientTcp::stateConnectChanged, this,
        [this]( const bool startCommunicator)
        {
             if( m_pFactory && startCommunicator)
             {
                 Command *const cmd = m_pFactory->createGetVersionCommand( m_hostId, m_deviceId);
                 Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);
                 m_pDeviceCommunicator->setCmd( decoratedCmd);
             }

            m_stateConnectToSerialPort = startCommunicator;
            m_pDeviceCommunicator->start( startCommunicator);
        },
        Qt::QueuedConnection
    );

    QObject::connect( 	m_pClient, &NNetwork::CClientTcp::receivedData,
                        m_pParser, &MessageParser::processData, Qt::QueuedConnection);

    QObject::connect( 	m_pClient, &NNetwork::CClientTcp::receivedData, this,
        [this]( const QByteArray data)
        {
            if( m_printRxRawDataSerialPort == false)
                return;

            const QString &text = QString::fromUtf8( "RX: ") + data.toHex( ' ') + QString::fromUtf8( "\n");

            emit rxRawDataSerialPortText( text);
        },
        Qt::QueuedConnection
    );

#else

    QObject::connect(   this, &Goen220GyroPlatformHostPrivate::connectToDevice,
                        m_pSerialPort, &SerialPort::connect, Qt::QueuedConnection);

    QObject::connect(   m_pSerialPort, &SerialPort::stateConnectChanged, this,
        [this]( const bool con)
        {
            if( con == false)
                return;

            m_serialPortErrorString = QString::fromUtf8( "");

            emit serialPortErrorStringChanged();
        },
        Qt::QueuedConnection
    );

    QObject::connect(   m_pSerialPort, &SerialPort::stateConnectChanged,
                        this, &Goen220GyroPlatformHostPrivate::stateConnectToSerialPortChanged, Qt::QueuedConnection);

    QObject::connect(   m_pSerialPort, &SerialPort::errorOccurred, this,
        [this]( const QString error)
        {
            if( m_serialPortErrorString == error)
                return;

            m_serialPortErrorString = error;

            emit serialPortErrorStringChanged();
        },
        Qt::QueuedConnection
    );

    QObject::connect( 	m_pDeviceCommunicator, &DeviceCommunicator::sendData,
                        m_pSerialPort, &SerialPort::sendData, Qt::QueuedConnection);


    QObject::connect( 	m_pSerialPort, &SerialPort::stateConnectChanged, this,
        [this]( const bool startCommunicator)
        {
             if( m_pFactory && startCommunicator)
             {
                 Command *const cmd = m_pFactory->createGetVersionCommand( m_hostId, m_deviceId);
                 Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);
                 m_pDeviceCommunicator->setCmd( decoratedCmd);
             }

            m_stateConnectToSerialPort = startCommunicator;
            m_pDeviceCommunicator->start( startCommunicator);
        },
        Qt::QueuedConnection
    );

    QObject::connect( 	m_pSerialPort, &SerialPort::receivedData,
                        m_pParser, &MessageParser::processData, Qt::QueuedConnection);

    QObject::connect( 	m_pSerialPort, &SerialPort::receivedData, this,
        [this]( const QByteArray data)
        {
            if( m_printRxRawDataSerialPort == false)
                return;

            const QString &text = QString::fromUtf8( "RX: ") + data.toHex( ' ') + QString::fromUtf8( "\n");

            emit rxRawDataSerialPortText( text);
        },
        Qt::QueuedConnection
    );

#endif

    QObject::connect( 	m_pDeviceCommunicator, &DeviceCommunicator::sendData, this,
        [this]( const QByteArray data)
        {
            if( m_printTxRawDataSerialPort == false)
                return;

            const QString &text = QString::fromUtf8( "TX: ") + data.toHex( ' ') + QString::fromUtf8( "\n");

            emit txRawDataSerialPortText( text);
        }
    );


    m_pHost = new Calculator( m_hostId);

    m_pFactory = new CommandFactory( m_pHost);
    QObject::connect( 	m_pParser, &MessageParser::messageParsed,
                        m_pFactory, &CommandFactory::setMessage);

    QObject::connect( 	m_pFactory, &CommandFactory::commandCreated,
                        m_pDeviceCommunicator, &DeviceCommunicator::setResponseCmd);

    QObject::connect(   m_pHost, &CommandExecutor::sendCmd, m_pDeviceCommunicator, &DeviceCommunicator::setCmd);

    QObject::connect(   m_pHost, &CommandExecutor::versionChanged,
                        this, &Goen220GyroPlatformHostPrivate::versionChanged);

    QObject::connect(   m_pHost, &CommandExecutor::modeChanged, this,
        [this]( const CommandExecutor::Mode mode)
        {
            emit modeChanged( (uint8_t)mode);
        }
    );


    QObject::connect(   m_pHost, &CommandExecutor::memsBoardStatusCommonChanged, this,
        [this]( const uint8_t mode, const bool errorAllMems,
                const uint8_t memsNotConfiguredFlags,
                const uint8_t memsIllegalDataFlags,
                const uint8_t memsNoResponseOnSpiFlags,
                const uint8_t reserveFlags,
                const uint8_t memsAxisSwitchMode,
                const uint8_t memsProcessMode)
        {
            m_memsBoardStatusCommon.mode            = mode;
            m_memsBoardStatusCommon.errorAllMems    = errorAllMems;

            m_memsBoardStatusCommon.notConfiguredFlags      = memsNotConfiguredFlags;
            m_memsBoardStatusCommon.illegalDataFlags        = memsIllegalDataFlags;
            m_memsBoardStatusCommon.noResponseOnSpiFlags    = memsNoResponseOnSpiFlags;
            m_memsBoardStatusCommon.reserveFlags            = reserveFlags;

            m_memsBoardStatusCommon.axisSwitchMode  = memsAxisSwitchMode;
            m_memsBoardStatusCommon.memsProcessMode = memsProcessMode;

            emit memsBoardStatusCommonChanged( m_memsBoardStatusCommon);
        }
    );


    QObject::connect(   m_pHost, &CommandExecutor::memsBoardStatusSpeedAndAngleChanged, this,
        [this]( const float speedYaw, const float speedPitch,
                const float angleYaw, const float anglePitch)
        {
            m_memsBoardStatusSpeedAndAngle.speedYaw     = speedYaw;
            m_memsBoardStatusSpeedAndAngle.speedPitch   = speedPitch;
            m_memsBoardStatusSpeedAndAngle.angleYaw     = angleYaw;
            m_memsBoardStatusSpeedAndAngle.anglePitch   = anglePitch;

            emit memsBoardStatusSpeedAndAngleChanged( m_memsBoardStatusSpeedAndAngle);
        }
    );


    QObject::connect( 	m_pHost, &CommandExecutor::yawMotorStatusChanged, this,
        [this]( const uint8_t mode, const uint8_t errors, const uint8_t flags,
                const float angle, const float speed)
        {
            m_yawMotorStatus.mode 	= mode;

            m_yawMotorStatus.errors	= errors;
            m_yawMotorStatus.flags	= flags;

            m_yawMotorStatus.angle	= angle;
            m_yawMotorStatus.speed	= speed;

            emit yawMotorStatusChanged( m_yawMotorStatus);
        }
    );

    QObject::connect( 	m_pHost, &CommandExecutor::pitchMotorStatusChanged, this,
        [this]( const uint8_t mode, const uint8_t errors, const uint8_t flags,
                const float angle, const float speed)
        {
            m_pitchMotorStatus.mode 	= mode;

            m_pitchMotorStatus.errors	= errors;
            m_pitchMotorStatus.flags	= flags;

            m_pitchMotorStatus.angle	= angle;
            m_pitchMotorStatus.speed	= speed;

            emit pitchMotorStatusChanged( m_pitchMotorStatus);
        }
    );


    QObject::connect(  	m_pHost, &CommandExecutor::setModeCommandCompleted,
                        this, &Goen220GyroPlatformHostPrivate::setModeCommandCompleted);

    QObject::connect(  	m_pHost, &CommandExecutor::setControlPositionCommandCompleted,
                        this, &Goen220GyroPlatformHostPrivate::setControlPositionCommandCompleted);

    QObject::connect(  	m_pHost, &CommandExecutor::setControlSpeedCommandCompleted,
                        this, &Goen220GyroPlatformHostPrivate::setControlSpeedCommandCompleted);

    QObject::connect(  	m_pHost, &CommandExecutor::setControlTrackingCommandCompleted,
                        this, &Goen220GyroPlatformHostPrivate::setControlTrackingCommandCompleted);


    QObject::connect( 	m_pDeviceCommunicator, &DeviceCommunicator::deviceConnectChanged,
                        this, &Goen220GyroPlatformHostPrivate::stateConnectToRemoteDeviceChanged);

    QObject::connect( 	m_pDeviceCommunicator, &DeviceCommunicator::restart,
                        this, &Goen220GyroPlatformHostPrivate::restart);
}


Goen220GyroPlatformHostPrivate::~Goen220GyroPlatformHostPrivate()
{
    if( m_pFactory)
        m_pFactory->deleteLater();

    if( m_pHost)
        m_pHost->deleteLater();

    if( m_pParser)
        m_pParser->deleteLater();

    if( m_pDeviceCommunicator)
        m_pDeviceCommunicator->deleteLater();

#ifdef USE_TCP_CLIENT
    if( m_pClient)
        m_pClient->deleteLater();
#endif

    m_pSerialPort->deleteLater();

}


QStringList Goen220GyroPlatformHostPrivate::getSerialPorts()
{
    return SerialPort::getAvailablePorts();
}


void Goen220GyroPlatformHostPrivate::connect( const bool con)
{
    emit connectToDevice( con);
}


// bool Goen220GyroPlatformHostPrivate::getStateConnectToSerialPort() const
// {
//     return m_stateConnectToSerialPort;
// }


QString Goen220GyroPlatformHostPrivate::getSerialPortName() const
{
    return m_pSerialPort->portName();
}


void Goen220GyroPlatformHostPrivate::setSerialPortName( const QString &portName)
{
    if( m_pSerialPort->portName() == portName)
        return;

    m_pSerialPort->setPortName( portName);

    emit serialPortNameChanged();
}


QString Goen220GyroPlatformHostPrivate::getSerialPortErrorString() const
{
    return m_serialPortErrorString;
}


bool Goen220GyroPlatformHostPrivate::getPrintTxRawDataSerialPort() const
{
    return m_printTxRawDataSerialPort;
}


void Goen220GyroPlatformHostPrivate::setPrintTxRawDataSerialPort( const bool enable)
{
//    if( m_printTxRawDataSerialPort == enable)
//        return;

//    qDebug() << Q_FUNC_INFO << enable;
    m_printTxRawDataSerialPort = enable;

    emit printTxRawDataSerialPortChanged();
}


bool Goen220GyroPlatformHostPrivate::getPrintRxRawDataSerialPort() const
{
    return m_printRxRawDataSerialPort;
}


void Goen220GyroPlatformHostPrivate::setPrintRxRawDataSerialPort( const bool enable)
{
//    if( m_printRxRawDataSerialPort == enable)
//        return;

//    qDebug() << Q_FUNC_INFO << enable;
    m_printRxRawDataSerialPort = enable;

    emit printRxRawDataSerialPortChanged();
}


void Goen220GyroPlatformHostPrivate::setHostId( const uint8_t hostId)
{
    m_pDeviceCommunicator->setHostId( hostId);
    m_pHost->setHostId( hostId);
    m_hostId = hostId;
}


void Goen220GyroPlatformHostPrivate::setDeviceId( const uint8_t deviceId)
{
    m_pDeviceCommunicator->setDeviceId( deviceId);
    m_pHost->setDeviceId( deviceId);
    m_deviceId = deviceId;
}


void Goen220GyroPlatformHostPrivate::setTimeoutWaitingResponse( const uint32_t msec)
{
    m_pDeviceCommunicator->setTimeoutWaitingResponse( msec);
}


void Goen220GyroPlatformHostPrivate::setTimeBetweenStatusRequests( const uint32_t msec)
{
    m_pDeviceCommunicator->setTimeBetweenStatusRequests( msec);
}


void Goen220GyroPlatformHostPrivate::setMode( const uint8_t mode)
{
    if( m_stateConnectToSerialPort == false)
        return;

    m_pHost->setMode( (CommandExecutor::Mode)mode);
}


void Goen220GyroPlatformHostPrivate::setControlPosition( const float yaw, const float pitch)
{
//    qDebug() << Q_FUNC_INFO << yaw << pitch;

    m_pHost->setControlPosition( yaw, pitch);
}


void Goen220GyroPlatformHostPrivate::setControlSpeed( const float yaw, const float pitch)
{
    m_pHost->setControlSpeed( yaw, pitch);
}


void Goen220GyroPlatformHostPrivate::setControlTracking(    const float yawMisalignment, const float pitchMisalignment,
                                                            const float yawAccelLimit, const float pitchAccelLimit)
{
    m_pHost->setControlTracking( yawMisalignment, pitchMisalignment, yawAccelLimit, pitchAccelLimit);
}


//void Goen220GyroPlatformHostPrivate::setStateConnectToSerialPort( const bool con)
//{
//    if( m_stateConnectToSerialPort == con)
//        return;

//    m_stateConnectToSerialPort = con;

//    qDebug() << Q_FUNC_INFO << con;
//    emit stateConnectToSerialPortChanged();
//}


