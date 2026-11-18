
#ifndef SET_CONTROL_POSITION_COMMAND_H
#define SET_CONTROL_POSITION_COMMAND_H



#include "WriteRequestCommand.h"



class SetControlPositionCommand : public WriteRequestCommand
{
public:
    SetControlPositionCommand(  const float yaw, const float pitch,
                                const uint8_t srcDevice, const uint8_t dstDevice,
                                const bool requiresResponse,
                                CommandExecutor *const commandExecutor = nullptr);

    SetControlPositionCommand( const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};



#endif

