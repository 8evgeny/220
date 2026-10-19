
#ifndef SET_CONTROL_SPEED_COMMAND_H
#define SET_CONTROL_SPEED_COMMAND_H



#include "WriteRequestCommand.h"



class SetControlSpeedCommand : public WriteRequestCommand
{
public:
    SetControlSpeedCommand(     const float yaw, const float pitch,
                                const uint8_t srcDevice, const uint8_t dstDevice,
                                const bool requiresResponse,
                                CommandExecutor *const commandExecutor = nullptr);

    SetControlSpeedCommand(     const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

