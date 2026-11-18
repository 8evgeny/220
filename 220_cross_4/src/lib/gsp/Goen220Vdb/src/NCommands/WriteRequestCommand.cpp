
#include "WriteRequestCommand.h"



WriteRequestCommand::WriteRequestCommand( 	const uint8_t srcDevice, const uint8_t dstDevice,
                                            const uint8_t dstFunction,
                                            const bool requiresResponse,
                                            const QByteArray &payload,
                                            CommandExecutor *const commandExecutor):
    VdbCommand( WriteRequest, requiresResponse, NoError, srcDevice, 0x00, dstDevice, dstFunction, payload,
                commandExecutor)
{

}


WriteRequestCommand::WriteRequestCommand( const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor):
    VdbCommand( headerAndPayload, commandExecutor)
{

}
