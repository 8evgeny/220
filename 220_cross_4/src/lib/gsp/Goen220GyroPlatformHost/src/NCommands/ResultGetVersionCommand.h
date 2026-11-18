
#ifndef RESULT_GET_VERSION_COMMAND_H
#define RESULT_GET_VERSION_COMMAND_H



#include "ReadResponseCommand.h"



class ResultGetVersionCommand : public ReadResponseCommand
{
public:
    ResultGetVersionCommand(    const uint8_t srcDevice, const uint8_t dstDevice,
                                const uint32_t versionProtocolVdb, const uint32_t versionProtocol,
                                const uint32_t versionDevice,
                                CommandExecutor *const commandExecutor = nullptr);

    ResultGetVersionCommand( const QByteArray &headerAndPayload, CommandExecutor *const commandExecutor = nullptr);

    void execute() override;

};


#endif

