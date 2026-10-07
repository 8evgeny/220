
#include "ReadResponseCommand.h"


ReadResponseCommand::ReadResponseCommand( 	const uint8_t srcDevice, const uint8_t dstDevice,
                                            const uint8_t srcFunction,
                                            const Error error,
                                            const QByteArray &payload):
    VdbCommand( ReadResponse, true, NoError, srcDevice, srcFunction, dstDevice, 0x00, payload)
{

}


ReadResponseCommand::ReadResponseCommand( const QByteArray &headerAndPayload, CommandExecutor * const commandExecutor):
    VdbCommand( headerAndPayload, commandExecutor)
{

}


