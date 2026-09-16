
#include "ResultSetControlPositionCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>




ResultSetControlPositionCommand::ResultSetControlPositionCommand( const uint8_t srcDevice, const uint8_t dstDevice,
                                                                  const Error error):
    WriteResponseCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__CONTROL_POSITION, error, QByteArray())
{

}


ResultSetControlPositionCommand::ResultSetControlPositionCommand( const QByteArray &header,
                                                                  CommandExecutor *const commandExecutor):
    WriteResponseCommand( header, commandExecutor)
{

}


void ResultSetControlPositionCommand::execute()
{
    emit m_pCommandExecutor->setControlPositionCommandCompleted();
}
