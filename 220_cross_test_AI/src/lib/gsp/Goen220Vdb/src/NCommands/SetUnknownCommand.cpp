
#include "SetUnknownCommand.h"

// #include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

SetUnknownCommand::SetUnknownCommand(const QByteArray &headerAndPayload,
                                     CommandExecutor *const commandExecutor)
    : WriteRequestCommand(headerAndPayload, commandExecutor)
{

} /* SetUnknownCommand::SetUnknownCommand */

void SetUnknownCommand::execute()
{
    m_pCommandExecutor->setRequiresResponse(isRequiresResponse());
    m_pCommandExecutor->setUnknownCommand(getDstFunction());
} /* SetUnknownCommand::execute */
