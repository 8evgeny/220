
#include "ResultSetUnknownCommand.h"


//#include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>




ResultSetUnknownCommand::ResultSetUnknownCommand(   const uint8_t srcDevice, const uint8_t dstDevice, const uint8_t srcFunction):
    WriteResponseCommand( srcDevice, dstDevice, srcFunction, FunctionNotExist, QByteArray())
{

}


ResultSetUnknownCommand::ResultSetUnknownCommand( const QByteArray &header, CommandExecutor *const commandExecutor):
    WriteResponseCommand( header, commandExecutor)
{

}


void ResultSetUnknownCommand::execute()
{
    m_pCommandExecutor->setUnknownCommand( getSrcFunction());
}
