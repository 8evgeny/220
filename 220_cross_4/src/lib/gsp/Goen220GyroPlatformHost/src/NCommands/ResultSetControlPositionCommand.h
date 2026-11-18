
#ifndef RESULT_SET_CONTROL_POSITION_COMMAND_H
#define RESULT_SET_CONTROL_POSITION_COMMAND_H



#include "WriteResponseCommand.h"



class ResultSetControlPositionCommand : public WriteResponseCommand
{
public:
    ResultSetControlPositionCommand(    const uint8_t srcDevice, const uint8_t dstDevice,
                                        const Error error);

    ResultSetControlPositionCommand(    const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

