
#ifndef READ_RESPONSE_COMMAND_H
#define READ_RESPONSE_COMMAND_H


#include "VdbCommand.h"

#include <QByteArray>


class ReadResponseCommand : public VdbCommand
{
public:
    ReadResponseCommand( 	const uint8_t srcDevice, const uint8_t dstDevice,
                            const uint8_t srcFunction,
                            const Error error,
                            const QByteArray &payload);

    ReadResponseCommand(    const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);
};



#endif

