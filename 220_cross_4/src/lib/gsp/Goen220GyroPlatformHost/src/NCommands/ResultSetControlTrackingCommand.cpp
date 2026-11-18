
#include "ResultSetControlTrackingCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>




ResultSetControlTrackingCommand::ResultSetControlTrackingCommand(   const uint8_t srcDevice, const uint8_t dstDevice,
                                                                    const Error error):
    WriteResponseCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__CONTROL_TRACKING, error, QByteArray())
{

}


ResultSetControlTrackingCommand::ResultSetControlTrackingCommand(   const QByteArray &header,
                                                                    CommandExecutor *const commandExecutor):
    WriteResponseCommand( header, commandExecutor)
{

}


void ResultSetControlTrackingCommand::execute()
{
    emit m_pCommandExecutor->setControlTrackingCommandCompleted();
}
