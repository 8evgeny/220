
#include "WriteResponseCommand.h"


WriteResponseCommand::WriteResponseCommand( const uint8_t srcDevice, const uint8_t dstDevice,
                                            const uint8_t srcFunction, const Error error, const QByteArray &payload):
    VdbCommand( WriteResponse, true, error, srcDevice, srcFunction, dstDevice, 0x00, payload)
{

}


WriteResponseCommand::WriteResponseCommand( const QByteArray &header,
                                            CommandExecutor *const commandExecutor):
    VdbCommand( header, commandExecutor)
{

}
