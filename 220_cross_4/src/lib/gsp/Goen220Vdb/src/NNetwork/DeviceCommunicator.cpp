
#include "DeviceCommunicator.h"


//#include "GetVersionCommand.h"
//#include "GetStatusCommand.h"
//#include "DelimiterAndCrc8CommandDecorator.h"

#include "Command.h"

#include <QDebug>
#include <QState>
#include <QDateTime>



class DeviceCommunicatorPrivate : public QObject
{
    Q_OBJECT

public:
    explicit DeviceCommunicatorPrivate( DeviceCommunicator *const deviceCommunicator):
        m_pDeviceCommunicator( deviceCommunicator)
    {

    }

signals:
    void userCommandWasAdded();
    void commandReceivedWithoutErrors();

private:
    DeviceCommunicator 		*const m_pDeviceCommunicator;

};





DeviceCommunicator::DeviceCommunicator( const uint8_t hostId, const uint8_t deviceId, QObject *parent):
    QObject( parent)
  , m_hostId( hostId)
  , m_deviceId( deviceId)
  , m_attemptsBeforeRestart( 3)
  , m_attempts( 0)
  , m_commandsSent( 0)
  , m_commandsReceived( 0)
  , m_commandsLost( 0)
  , m_sendingTime( 0)
  , m_isDeviceConnected( false)
  , m_debugStateMachine( false)
  , d_ptr( new DeviceCommunicatorPrivate( this))
{
    const uint32_t speedMode = 1;

    m_commandExecutionTimer.setSingleShot( true);

    if( speedMode)
        m_commandExecutionTimer.setInterval( 40);
    else
        m_commandExecutionTimer.setInterval( 1000);

    m_attemptTimer.setSingleShot( true);

    if( speedMode)
        m_attemptTimer.setInterval( 30);
    else
        m_attemptTimer.setInterval( 1000);

    m_statusTimer.setSingleShot( true);

    if( speedMode)
        m_statusTimer.setInterval( 10);
    else
        m_statusTimer.setInterval( 1000);

    m_immediateSwitchTimer.setSingleShot( true);
    m_immediateSwitchTimer.setInterval( 1);

    initStateMachine();
}


DeviceCommunicator::~DeviceCommunicator()
{
    d_ptr->deleteLater();
    d_ptr = nullptr;
}


void DeviceCommunicator::setHostId( const uint8_t hostId)
{
    m_hostId = hostId;
}


void DeviceCommunicator::setDeviceId( const uint8_t deviceId)
{
    m_deviceId = deviceId;
}


void DeviceCommunicator::setTimeoutWaitingResponse( const uint32_t msec)
{
    m_commandExecutionTimer.setInterval( (int)msec);
}


void DeviceCommunicator::setTimeBetweenStatusRequests( const uint32_t msec)
{
    m_statusTimer.setInterval( msec);
}


void DeviceCommunicator::start( const bool startCommunicator)
{
    if( startCommunicator)
        m_stateMachine.start();
    else
    {
        m_stateMachine.stop();
        {
            QMutexLocker mtx( &m_enqueueDequeueCommandsMutex);

            while( m_commandsToSend.isEmpty() == false)
            {
                delete m_commandsToSend.dequeue();
            }
        }

        setDeviceConnected( false);
    }
}


void DeviceCommunicator::setCmd( Command *const cmd)
{
    if( cmd)
    {
        QMutexLocker mtx( &m_enqueueDequeueCommandsMutex);
        m_commandsToSend.enqueue( cmd);
    }

    Q_D(DeviceCommunicator);
    emit d->userCommandWasAdded();
}


void DeviceCommunicator::setResponseCmd( Command *const cmd)
{
    if( cmd->isValid() == false)
    {
        delete cmd;
        return;
    }

    if( cmd->getDstDeviceId() != m_hostId || cmd->isRequest() == true)
    {
        if( m_debugStateMachine)
            qDebug() << "Cmd Deleted";

        delete cmd;
        return;
    }

    Q_D(DeviceCommunicator);
    emit d->commandReceivedWithoutErrors();
    incCommndsReceived();

    cmd->execute();
    delete cmd;
}


//void DeviceCommunicator::sendGetVersionCommand()
//{
//    Command *const cmd = new GetVersionCommand( m_hostId, m_deviceId);
//    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);

//    qDebug() << "GetVersionCommand" << decoratedCmd->serialize().toHex( ' ');

//    emit sendData( decoratedCmd->serialize());
//    incCommandsSent();

//    delete decoratedCmd;
//}


//Command *DeviceCommunicator::createGetStatusCommand( const uint8_t hostId, const uint8_t deviceId)
//{
//    Command *const cmd = new GetStatusCommand( hostId, deviceId);
//    Command *const decoratedCmd = new DelimiterAndCrc8CommandDecorator( cmd);
//    return nullptr;
//    return decoratedCmd;
//}


//  TODO:   Отрефакторить
void DeviceCommunicator::initStateMachine()
{
    QState *const waitRequest = new QState();
    connect( waitRequest, &QState::entered, this,
        [this]()
        {

            bool empty = true;
            int commands = 0;

            {
                m_enqueueDequeueCommandsMutex.lock();
                empty = m_commandsToSend.isEmpty();
                commands = m_commandsToSend.count();
                m_enqueueDequeueCommandsMutex.unlock();
            }

            if( m_debugStateMachine)
                qDebug() << "waitRequest" << commands;

            if( empty)
                m_statusTimer.start();
            else
                m_immediateSwitchTimer.start();
        }
    );


    QState *const addGetStatusCmd = new QState();
    connect( addGetStatusCmd, &QState::entered, this,
        [this]()
        {
            if( m_debugStateMachine)
                qDebug() << "addGetStatusCmd";

            Command *const cmd = createGetStatusCommand( m_hostId, m_deviceId);
            if( cmd)
            {
                QMutexLocker mtx( &m_enqueueDequeueCommandsMutex);
                m_commandsToSend.enqueue( cmd);
            }

            m_immediateSwitchTimer.start();
        }
    );

    QState *const sendCmdAndWaitAnswer = new QState();
    connect( sendCmdAndWaitAnswer, &QState::entered, this,
        [this]()
        {
            if( m_debugStateMachine)
                qDebug() << "sendCmdAndWaitAnswer" << (QDateTime::currentMSecsSinceEpoch() - m_sendingTime);

            m_statusTimer.stop();
            m_sendingTime = QDateTime::currentMSecsSinceEpoch();

            Command *cmd = nullptr;
            {
                m_enqueueDequeueCommandsMutex.lock();
                cmd = m_commandsToSend.head();
                m_enqueueDequeueCommandsMutex.unlock();
            }

            if( cmd)
            {
                emit sendData( cmd->serialize());
                incCommandsSent();

                if( cmd->isRequiresResponse())
                {
                    m_commandExecutionTimer.start();
                }
                else
                {
                    {
                        QMutexLocker mtx( &m_enqueueDequeueCommandsMutex);
                        m_commandsToSend.dequeue();
                    }
                    delete cmd;
                    m_immediateSwitchTimer.start();
                }
            }
        }
    );


    QState *const startAttemptTimer = new QState();
    connect( startAttemptTimer, &QState::entered, this,
        [this]()
        {
            if( m_debugStateMachine)
                qDebug() << "startAttemptTimer (Timeout CMD)" << m_attempts;

            m_attempts ++;
            incCommndsLost();

            if( m_attempts >= m_attemptsBeforeRestart)
            {
                if( m_debugStateMachine)
                    qDebug() << "RESTART!!!";

                m_attempts = 0;

                setDeviceConnected( false);
                emit restart();
            }

            m_attemptTimer.start();
        }
    );


    QState *const removeRequestCmd = new QState();
    connect( removeRequestCmd, &QState::entered, this,
        [this]()
        {
            if( m_debugStateMachine)
                qDebug() << "removeRequestCmd";

            setDeviceConnected( true);
            m_attempts = 0;

            {
                QMutexLocker mtx( &m_enqueueDequeueCommandsMutex);
                if( m_commandsToSend.isEmpty() == false)
                    delete m_commandsToSend.dequeue();
            }

            m_immediateSwitchTimer.start();
        }
    );

    ///<    Если отсутствуют команды на отправку больше положенного времени, то сформировать команду запроса статуса
    ///<    и добавить в очередь команд:
    waitRequest->addTransition( &m_statusTimer, &QTimer::timeout, addGetStatusCmd);

    ///<    Если есть команда на отправку, то сразу переходим в состояние отправки:
    waitRequest->addTransition( &m_immediateSwitchTimer, &QTimer::timeout, sendCmdAndWaitAnswer);
    waitRequest->addTransition( d_ptr, &DeviceCommunicatorPrivate::userCommandWasAdded, sendCmdAndWaitAnswer);

    ///<    После добавления команды запроса статуса в очередь, переходим в состояние проверки очереди/команды от
    ///<    пользователя:
    addGetStatusCmd->addTransition( &m_immediateSwitchTimer, &QTimer::timeout, waitRequest);

    ///<    Если отправляемая команда не требует подтверждения, то сразу переходим к ожиданию новых команд:
    sendCmdAndWaitAnswer->addTransition( &m_immediateSwitchTimer, &QTimer::timeout, waitRequest);

    ///<    Если нет ответа от удаленного устройства, то переходим в состояние ожидания окончания таймера
    ///<    повторной отправки:
    sendCmdAndWaitAnswer->addTransition( &m_commandExecutionTimer, &QTimer::timeout, startAttemptTimer);

    ///<    Если получен верный ответ от устройства, то переходим к состоянию удаления команды из очереди команд:
    sendCmdAndWaitAnswer->addTransition( d_ptr, &DeviceCommunicatorPrivate::commandReceivedWithoutErrors,
                                         removeRequestCmd);

    ///<    После удаления команды из очереди переходим в состояние проверки очереди/команды от пользователя:
    removeRequestCmd->addTransition( &m_immediateSwitchTimer, &QTimer::timeout, waitRequest);

    ///<    После отсутствия ответа от устройства и срабатывания таймера повторной передачи команды переходим
    ///<    в состояние проверки очереди/команды от пользователя:
    startAttemptTimer->addTransition( &m_attemptTimer, &QTimer::timeout, waitRequest);

    m_stateMachine.addState( waitRequest);
    m_stateMachine.addState( addGetStatusCmd);
    m_stateMachine.addState( sendCmdAndWaitAnswer);
    m_stateMachine.addState( startAttemptTimer);
    m_stateMachine.addState( removeRequestCmd);

    m_stateMachine.setInitialState( waitRequest);
}


void DeviceCommunicator::incCommandsSent()
{
    m_commandsSent ++;
    if( m_commandsSent < 0)
        m_commandsSent = 0;

    emit commandsSentChanged( m_commandsSent);
}


void DeviceCommunicator::incCommndsReceived()
{
    m_commandsReceived ++;
    if( m_commandsReceived < 0)
        m_commandsReceived = 0;

    emit commandsReceivedChanged( m_commandsReceived);
}


void DeviceCommunicator::incCommndsLost()
{
    m_commandsLost ++;
    if( m_commandsLost < 0)
        m_commandsLost = 0;

    emit commandsLostChanged( m_commandsLost);
}


void DeviceCommunicator::setDeviceConnected( const bool con)
{
    if( m_isDeviceConnected == con)
        return;

    m_isDeviceConnected = con;

    emit deviceConnectChanged( con);
}





#include "DeviceCommunicator.moc"
