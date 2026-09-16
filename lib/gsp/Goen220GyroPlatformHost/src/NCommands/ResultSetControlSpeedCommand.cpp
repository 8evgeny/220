
#include "ResultSetControlSpeedCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>




ResultSetControlSpeedCommand::ResultSetControlSpeedCommand( const uint8_t srcDevice, const uint8_t dstDevice,
                                                            const Error error):
    WriteResponseCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__CONTROL_SPEED, error, QByteArray())
{

}


ResultSetControlSpeedCommand::ResultSetControlSpeedCommand( const QByteArray &header,
                                                            CommandExecutor *const commandExecutor):
    WriteResponseCommand( header, commandExecutor)
{

}


void ResultSetControlSpeedCommand::execute()
{
    emit m_pCommandExecutor->setControlSpeedCommandCompleted();
}
