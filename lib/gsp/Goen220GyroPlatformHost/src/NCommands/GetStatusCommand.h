
#ifndef GET_STATUS_COMMAND_H
#define GET_STATUS_COMMAND_H


#include "ReadRequestCommand.h"



class GetStatusCommand : public ReadRequestCommand
{
public:
    GetStatusCommand(   const uint8_t srcDevice, const uint8_t dstDevice,
                        CommandExecutor *const commandExecutor = nullptr);

    GetStatusCommand(   const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;

};





#endif

