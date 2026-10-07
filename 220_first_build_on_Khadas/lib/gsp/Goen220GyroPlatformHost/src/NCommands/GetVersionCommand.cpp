
#include "GetVersionCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"


GetVersionCommand::GetVersionCommand(   const uint8_t srcDevice, const uint8_t dstDevice,
                                        CommandExecutor *const commandExecutor):
   ReadRequestCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__VERSION, QByteArray())
    // ReadRequestCommand( srcDevice, dstDevice, 0x54, QByteArray())	//	Test
{

}


GetVersionCommand::GetVersionCommand( const QByteArray &header, CommandExecutor *const commandExecutor):
    ReadRequestCommand( header, commandExecutor)
{

}


void GetVersionCommand::execute()
{
    if( m_pCommandExecutor == nullptr)
        return;

    if( m_pCommandExecutor->getDeviceId() != getDstDeviceId())
        return;

    m_pCommandExecutor->getVersion();
}
