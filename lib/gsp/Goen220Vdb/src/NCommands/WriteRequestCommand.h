
#ifndef WRITE_REQUEST_COMMAND_H
#define WRITE_REQUEST_COMMAND_H


#include "VdbCommand.h"


#include <QByteArray>


class WriteRequestCommand : public VdbCommand
{
public:
    WriteRequestCommand( 	const uint8_t srcDevice, const uint8_t dstDevice,
                            const uint8_t dstFunction,
                            const bool requiresResponse,
                            const QByteArray &payload,
                            CommandExecutor *const commandExecutor = nullptr);

    WriteRequestCommand( const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);

private:

};



#endif

