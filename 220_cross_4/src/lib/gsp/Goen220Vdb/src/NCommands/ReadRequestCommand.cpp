
#include "ReadRequestCommand.h"


ReadRequestCommand::ReadRequestCommand( const uint8_t srcDevice, const uint8_t dstDevice,
                                        const uint8_t dstFunction,
                                        const QByteArray &payload):
    VdbCommand( ReadRequest, true, NoError, srcDevice, 0x00, dstDevice, dstFunction, QByteArray())
{

}


ReadRequestCommand::ReadRequestCommand( const QByteArray &header, CommandExecutor *const commandExecutor):
    VdbCommand( header, commandExecutor)
{

}


