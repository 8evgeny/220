
#ifndef RESULT_SET_CONTROL_TRACKING_COMMAND_H
#define RESULT_SET_CONTROL_TRACKING_COMMAND_H



#include "WriteResponseCommand.h"



class ResultSetControlTrackingCommand : public WriteResponseCommand
{
public:
    ResultSetControlTrackingCommand(    const uint8_t srcDevice, const uint8_t dstDevice,
                                        const Error error);

    ResultSetControlTrackingCommand(    const QByteArray &header, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

