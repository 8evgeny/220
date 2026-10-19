
#ifndef SET_UNKNOWN_COMMAND_H
#define SET_UNKNOWN_COMMAND_H



#include "WriteRequestCommand.h"



class SetUnknownCommand : public WriteRequestCommand
{
public:
    SetUnknownCommand(     const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

