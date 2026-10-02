
#ifndef RESULT_SET_CONTROL_SPEED_COMMAND_H
#define RESULT_SET_CONTROL_SPEED_COMMAND_H



#include "WriteResponseCommand.h"



class ResultSetControlSpeedCommand : public WriteResponseCommand
{
public:
    ResultSetControlSpeedCommand(   const uint8_t srcDevice, const uint8_t dstDevice,
                                    const Error error);

    ResultSetControlSpeedCommand(   const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

