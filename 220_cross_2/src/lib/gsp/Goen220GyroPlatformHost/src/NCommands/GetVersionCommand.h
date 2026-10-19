
#ifndef GET_VERSION_COMMAND_H
#define GET_VERSION_COMMAND_H


#include "ReadRequestCommand.h"



class GetVersionCommand : public ReadRequestCommand
{
public:
    GetVersionCommand(  const uint8_t srcDevice, const uint8_t dstDevice,
                        CommandExecutor *const commandExecutor = nullptr);

    GetVersionCommand( const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;

};





#endif

