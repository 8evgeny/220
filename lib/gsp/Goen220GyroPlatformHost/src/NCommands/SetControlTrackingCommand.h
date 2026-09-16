
#ifndef SET_CONTROL_TRACKING_COMMAND_H
#define SET_CONTROL_TRACKING_COMMAND_H



#include "WriteRequestCommand.h"



class SetControlTrackingCommand : public WriteRequestCommand
{
public:
    SetControlTrackingCommand(  const float yawMisalignment, const float pitchMisalignment,
                                const float yawAccelLimit, const float pitchAccelLimit,
                                const uint8_t srcDevice, const uint8_t dstDevice,
                                const bool requiresResponse,
                                CommandExecutor *const commandExecutor = nullptr);

    SetControlTrackingCommand(     const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

