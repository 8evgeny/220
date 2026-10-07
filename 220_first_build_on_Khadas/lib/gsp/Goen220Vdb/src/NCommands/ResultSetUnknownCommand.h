
#ifndef RESULT_SET_UNKNOWN_COMMAND_H
#define RESULT_SET_UNKNOWN_COMMAND_H



#include "WriteResponseCommand.h"



class ResultSetUnknownCommand : public WriteResponseCommand
{
public:
    ResultSetUnknownCommand( const uint8_t srcDevice, const uint8_t dstDevice, const uint8_t srcFunction);

    ResultSetUnknownCommand( const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

