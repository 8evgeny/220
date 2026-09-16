
#include "GetStatusCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"


GetStatusCommand::GetStatusCommand(     const uint8_t srcDevice, const uint8_t dstDevice,
                                        CommandExecutor *const commandExecutor):
    ReadRequestCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__STATUS, QByteArray())
{

}


GetStatusCommand::GetStatusCommand( const QByteArray &header, CommandExecutor *const commandExecutor):
    ReadRequestCommand( header, commandExecutor)
{

}


void GetStatusCommand::execute()
{
    if( m_pCommandExecutor == nullptr)
        return;

    if( m_pCommandExecutor->getDeviceId() != getDstDeviceId())
        return;

    m_pCommandExecutor->getStatus();
}
