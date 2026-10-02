
#ifndef READ_REQUEST_COMMAND_H
#define READ_REQUEST_COMMAND_H

#include "VdbCommand.h"

#include <QByteArray>


class ReadRequestCommand : public VdbCommand
{
public:
    ReadRequestCommand( const uint8_t srcDevice, const uint8_t dstDevice,
                        const uint8_t dstFunction,
                        const QByteArray &payload);

    ReadRequestCommand( const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

};








#endif

