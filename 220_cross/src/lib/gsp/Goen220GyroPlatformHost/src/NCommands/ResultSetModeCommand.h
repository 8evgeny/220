
#ifndef RESULT_SET_MODE_COMMAND_H
#define RESULT_SET_MODE_COMMAND_H


#include "WriteResponseCommand.h"



class ResultSetModeCommand : public WriteResponseCommand
{
public:
    ResultSetModeCommand(	const uint8_t srcDevice, const uint8_t dstDevice,
                            const Error error);

    ResultSetModeCommand( const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;
};


#endif

