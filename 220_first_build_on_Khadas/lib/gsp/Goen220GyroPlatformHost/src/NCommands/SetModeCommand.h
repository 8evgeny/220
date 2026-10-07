
#ifndef SET_MODE_COMMAND_H
#define SET_MODE_COMMAND_H


#include "WriteRequestCommand.h"



class SetModeCommand : public WriteRequestCommand
{
public:
    enum Mode : uint8_t
    {
         Dummy          = 0xC0
        ,Stabilization	= 0xC1
        ,RotaryPlatform	= 0xC2
        ,Parkin         = 0xC3
        ,Off            = 0xC4

        ,Unknown        = 0xFF
    };

    SetModeCommand( const Mode mode,
                    const uint8_t srcDevice, const uint8_t dstDevice,
                    const bool requiresResponse,
                    CommandExecutor *const commandExecutor = nullptr);

    SetModeCommand( const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;


private:

};

#endif

