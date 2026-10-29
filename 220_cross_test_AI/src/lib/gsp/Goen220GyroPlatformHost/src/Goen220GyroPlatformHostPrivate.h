#ifndef GOEN_220_GYRO_PLATFORM_HOST_PRIVATE_H
#define GOEN_220_GYRO_PLATFORM_HOST_PRIVATE_H

#include "MemsBoardStatus.h"
#include "MotorStatus.h"

#include <QObject>


//	Для тестирования через TCP модель при отсутствии двух соединенных последовательных портов
//#define USE_TCP_CLIENT



#ifdef USE_TCP_CLIENT

    namespace NNetwork
    {
        class CClientTcp;
    }

#endif


class SerialPort;
class DeviceCommunicator;
class MessageParser;
class CommandExecutor;
class CommandFactory;


class Goen220GyroPlatformHostPrivate : public QObject
{
    Q_OBJECT

public:
    explicit Goen220GyroPlatformHostPrivate( QObject *parent = nullptr);
    ~Goen220GyroPlatformHostPrivate();

    static QStringList getSerialPorts();

    void connect( const bool con);



    // bool getStateConnectToSerialPort() const;


    QString getSerialPortName() const;
    void setSerialPortName( const QString &portName);

    QString getSerialPortErrorString() const;

    bool getPrintTxRawDataSerialPort() const;
    void setPrintTxRawDataSerialPort( const bool enable);

    bool getPrintRxRawDataSerialPort() const;
    void setPrintRxRawDataSerialPort( const bool enable);


    void setHostId( const uint8_t hostId);
    void setDeviceId( const uint8_t deviceId);

    void setTimeoutWaitingResponse( const uint32_t msec);
    void setTimeBetweenStatusRequests( const uint32_t msec);


    void setMode( const uint8_t mode);
    void setControlPosition( const float yaw, const float pitch);
    void setControlSpeed( const float yaw, const float pitch);
    void setControlTracking(    const float yawMisalignment, const float pitchMisalignment,
                                const float yawAccelLimit, const float pitchAccelLimit);

signals:
    void stateConnectToSerialPortChanged( const bool con);

    void txRawDataSerialPortText( const QString &text);
    void rxRawDataSerialPortText( const QString &text);

    void versionChanged( const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                         const uint32_t versionDevice);

    void modeChanged( const uint8_t mode);

    void memsBoardStatusCommonChanged( const MemsBoardStatusCommon &memsBoardStatus);
    void memsBoardStatusSpeedAndAngleChanged( const MemsBoardStatusSpeedAndAngle &memsBoardStatus);
    void yawMotorStatusChanged(	const MotorStatus &motorStatus);
    void pitchMotorStatusChanged(	const MotorStatus &motorStatus);

    void setModeCommandCompleted();
    void setControlPositionCommandCompleted();
    void setControlSpeedCommandCompleted();
    void setControlTrackingCommandCompleted();

    void serialPortNameChanged();
    void serialPortErrorStringChanged();

    void printTxRawDataSerialPortChanged();
    void printRxRawDataSerialPortChanged();

    void restart();
    void stateConnectToRemoteDeviceChanged( const bool con);


signals:
    void connectToDevice( const bool con);

private slots:
//    void setStateConnectToSerialPort( const bool con);

private:
    SerialPort                  *const m_pSerialPort;

#ifdef USE_TCP_CLIENT

    NNetwork::CClientTcp		*m_pClient;
#endif

    bool                        m_stateConnectToSerialPort;

    QString                     m_serialPortErrorString;


    DeviceCommunicator 			*m_pDeviceCommunicator;
    MessageParser				*m_pParser;
    CommandExecutor 			*m_pHost;
    CommandFactory				*m_pFactory;

    MemsBoardStatusCommon           m_memsBoardStatusCommon;
    MemsBoardStatusSpeedAndAngle    m_memsBoardStatusSpeedAndAngle;

    MotorStatus					m_yawMotorStatus;
    MotorStatus					m_pitchMotorStatus;


    bool                        m_printTxRawDataSerialPort;
    bool                        m_printRxRawDataSerialPort;

    uint8_t                     m_hostId;
    uint8_t                     m_deviceId;
};

#endif
