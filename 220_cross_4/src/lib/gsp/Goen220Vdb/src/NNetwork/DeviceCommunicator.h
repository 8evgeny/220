
#ifndef DEVICE_COMMUNICATOR_H
#define DEVICE_COMMUNICATOR_H


#include <QObject>
#include <QTimer>
#include <QStateMachine>
#include <QQueue>
#include <QMutex>


class Command;
class DeviceCommunicatorPrivate;



class DeviceCommunicator : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DeviceCommunicator)

public:
    explicit DeviceCommunicator( const uint8_t hostId, const uint8_t deviceId, QObject *parent = nullptr);
    virtual ~DeviceCommunicator();

    void setHostId( const uint8_t hostId);
    void setDeviceId( const uint8_t deviceId);

    void setTimeoutWaitingResponse( const uint32_t msec);
    void setTimeBetweenStatusRequests( const uint32_t msec);

signals:
    void commandsSentChanged( const int32_t commandsSent);
    void commandsReceivedChanged( const int32_t commandsReceived);
    void commandsLostChanged( const int32_t commandsLost);


public slots:
    void start( const bool startCommunicator);
    void setCmd( Command *const cmd);
    void setResponseCmd( Command *const cmd);

signals:
    void restart();
    void sendData( const QByteArray &data);

    void deviceConnectChanged( const bool con);

protected slots:
//    virtual void sendGetVersionCommand();

protected:
    virtual Command *createGetStatusCommand( const uint8_t hostId, const uint8_t deviceId) = 0;

private:
    void initStateMachine();
    void incCommandsSent();
    void incCommndsReceived();
    void incCommndsLost();

    void setDeviceConnected( const bool con);

private:
    uint8_t         m_hostId;
    uint8_t         m_deviceId;

    QStateMachine   m_stateMachine;

    QTimer          m_commandExecutionTimer;
    QTimer          m_attemptTimer;

    QTimer          m_statusTimer;
    QTimer          m_immediateSwitchTimer;

    uint32_t        m_attemptsBeforeRestart;
    uint32_t        m_attempts;

    QQueue<Command *>   m_commandsToSend;
    QMutex              m_enqueueDequeueCommandsMutex;

    int32_t         m_commandsSent;
    int32_t         m_commandsReceived;
    int32_t         m_commandsLost;

    qint64          m_sendingTime;

    bool			m_isDeviceConnected;
    bool            m_debugStateMachine;

private:
    DeviceCommunicatorPrivate 	*d_ptr;

};



#endif

