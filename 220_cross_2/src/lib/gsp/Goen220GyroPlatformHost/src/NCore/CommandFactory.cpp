
#include "CommandFactory.h"


#include "VdbProtocol.h"
#include "GyroPlatformProtocol.h"
#include "VdbCommand.h"
#include "Goen220Crc.h"

#include "SetModeCommand.h"
#include "GetVersionCommand.h"
#include "GetStatusCommand.h"
#include "SetControlPositionCommand.h"
#include "SetControlSpeedCommand.h"
#include "SetControlTrackingCommand.h"

#include "ResultGetVersionCommand.h"
#include "ResultGetStatusCommand.h"
#include "ResultSetModeCommand.h"
#include "ResultSetControlPositionCommand.h"
#include "ResultSetControlSpeedCommand.h"
#include "ResultSetControlTrackingCommand.h"
#include "ResultSetUnknownCommand.h"
#include "ResultStatusWithErrorCommand.h"


#include "SetUnknownCommand.h"


CommandFactory::CommandFactory( CommandExecutor *const commandExecutor, QObject *parent):
    m_pCommandExecutor( commandExecutor)
  , m_invalidMessages( 0)
{

}


bool CommandFactory::validateMessage( const Message &message)
{
    char a = 0xAA;

    const QByteArray &payload = QByteArray( &a, 1) + message.getPayload();
    const uint8_t *const data = (uint8_t *)payload.constData();
    const uint8_t payloadCrc = goen220__crc8_calc( data, (uint8_t)payload.size());
    const QByteArray &dataCrc = message.getCrc();
    const uint8_t crc = *(uint8_t *)dataCrc.constData();

    return (payloadCrc == crc);
}


static Command *createRequestCommand( const VdbHeader *const header, const Message &message, CommandExecutor *const commandExecutor)
{
    if( message.getPayload().size() < sizeof(VdbHeader))
        return nullptr;

    switch( header->dst.function)
    {
    case GOEN_220__GYRO_PLATFORM__CMD__MODE:
        if( message.getPayload().size() < (sizeof(VdbHeader) + sizeof(ModeCmd)))
            return nullptr;

        return new SetModeCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__VERSION:
        return new GetVersionCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__STATUS:
        return new GetStatusCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__CONTROL_POSITION:
        if( message.getPayload().size() < (sizeof(VdbHeader) + sizeof(ControlPosition)))
            return nullptr;

        return new SetControlPositionCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__CONTROL_SPEED:
        if( message.getPayload().size() < (sizeof(VdbHeader) + sizeof(ControlSpeed)))
            return nullptr;

        return new SetControlSpeedCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__CONTROL_TRACKING:
        if( message.getPayload().size() < (sizeof(VdbHeader) + sizeof(ControlTracking)))
            return nullptr;

        return new SetControlTrackingCommand( message.getPayload(), commandExecutor);

    default:
        return new SetUnknownCommand( message.getPayload(), commandExecutor);
    }

    return nullptr;
}


static Command *createResponseCommand( const VdbHeader *const header, const Message &message,
                                       CommandExecutor *const commandExecutor)
{
    if( message.getPayload().size() < sizeof(VdbHeader))
        return nullptr;

    if( header->status > VdbCommand::RequestWithResponse)
    {
        if( (header->status - VdbCommand::RequestWithResponse) == VdbCommand::FunctionNotExist)
        {
            return new ResultSetUnknownCommand( message.getPayload(), commandExecutor);
        }
        else
        {
            return new ResultStatusWithErrorCommand( message.getPayload(), commandExecutor);
        }
    }

    switch( header->src.function)
    {
    case GOEN_220__GYRO_PLATFORM__CMD__MODE:
        return new ResultSetModeCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__VERSION:
        if( message.getPayload().size() < (sizeof(VdbHeader) + sizeof(VersionResultCmd)))
            return nullptr;

        return new ResultGetVersionCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__STATUS:
        if( message.getPayload().size() < (sizeof(VdbHeader) + sizeof(TelemetryUART)))
            return nullptr;

        return new ResultGetStatusCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__CONTROL_POSITION:
        return new ResultSetControlPositionCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__CONTROL_SPEED:
        return new ResultSetControlSpeedCommand( message.getPayload(), commandExecutor);

    case GOEN_220__GYRO_PLATFORM__CMD__CONTROL_TRACKING:
        return new ResultSetControlTrackingCommand( message.getPayload(), commandExecutor);

    default:
        return new ResultSetUnknownCommand( message.getPayload(), commandExecutor);
    }

    return nullptr;
}


Command *CommandFactory::createCommand( const Message message)
{
    if( validateMessage( message) == false)
    {
        m_invalidMessages ++;
        if( m_invalidMessages < 0)
            m_invalidMessages = 0;

        emit invalidMessageCounterChanged( m_invalidMessages);
        return nullptr;
    }

    if( message.getPayload().size() < sizeof(VdbHeader))
        return nullptr;

    const VdbHeader *const header = (VdbHeader *)message.getPayload().constData();

    if( header->cmd == VdbCommand::WriteRequest || header->cmd == VdbCommand::ReadRequest)
    {
        return createRequestCommand( header, message, m_pCommandExecutor);
    }
    else
    {
        return createResponseCommand( header, message, m_pCommandExecutor);
    }

    return nullptr;
}


Command *CommandFactory::createGetVersionCommand( const uint8_t srcDevice, const uint8_t dstDevice) const
{
    return new GetVersionCommand( srcDevice, dstDevice, m_pCommandExecutor);
}


void CommandFactory::executeMessage( const Message message)
{
    Command *const command = createCommand( message);
    if( command == nullptr)
        return;

    command->execute();

    delete command;
}


void CommandFactory::setMessage( const Message message)
{
    Command *const command = createCommand( message);
    if( command == nullptr)
        return;

    emit commandCreated( command);
}

