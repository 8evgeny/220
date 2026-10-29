
#include "ResultStatusWithErrorCommand.h"

// #include "GyroPlatformProtocol.h"
#include "CommandExecutor.h"

#include <QDebug>

ResultStatusWithErrorCommand::ResultStatusWithErrorCommand(const QByteArray &header,
                                                           CommandExecutor *const commandExecutor)
    : WriteResponseCommand(header, commandExecutor)
{

} /* ResultStatusWithErrorCommand::ResultStatusWithErrorCommand */

void ResultStatusWithErrorCommand::execute()
{
    m_pCommandExecutor->setStatusWithErrorCommand(getErrorCode());
} /* ResultStatusWithErrorCommand::execute */
