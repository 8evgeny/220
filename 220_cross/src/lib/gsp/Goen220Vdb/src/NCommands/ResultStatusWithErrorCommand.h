
#ifndef RESULT_STATUS_WITH_ERROR_COMMAND_H
#define RESULT_STATUS_WITH_ERROR_COMMAND_H



#include "WriteResponseCommand.h"



class ResultStatusWithErrorCommand : public WriteResponseCommand
{
public:

    ResultStatusWithErrorCommand( const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

