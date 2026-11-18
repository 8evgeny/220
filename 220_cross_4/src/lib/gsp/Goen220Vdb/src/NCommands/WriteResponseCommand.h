
#ifndef WRITE_RESPONSE_COMMAND_H
#define WRITE_RESPONSE_COMMAND_H

#include "VdbCommand.h"

#include <QByteArray>



class WriteResponseCommand : public VdbCommand
{
public:

    WriteResponseCommand( 	const uint8_t srcDevice, const uint8_t dstDevice,
                            const uint8_t srcFunction,
                            const Error error,
                            const QByteArray &payload);

    WriteResponseCommand(   const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

};





#endif

