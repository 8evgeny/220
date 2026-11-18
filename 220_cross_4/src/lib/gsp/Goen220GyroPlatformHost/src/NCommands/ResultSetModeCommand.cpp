
#include "ResultSetModeCommand.h"


#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"


#include <QDebug>


ResultSetModeCommand::ResultSetModeCommand( const uint8_t srcDevice, const uint8_t dstDevice, const Error error):
    WriteResponseCommand( srcDevice, dstDevice, GOEN_220__GYRO_PLATFORM__CMD__MODE, error, QByteArray())
{

}


ResultSetModeCommand::ResultSetModeCommand( const QByteArray &header, CommandExecutor *const commandExecutor):
    WriteResponseCommand( header, commandExecutor)
{

}


void ResultSetModeCommand::execute()
{
//    qDebug() << Q_FUNC_INFO;

    emit m_pCommandExecutor->setModeCommandCompleted();
}
