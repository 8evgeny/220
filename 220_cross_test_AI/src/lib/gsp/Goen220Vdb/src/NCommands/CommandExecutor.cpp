
#include "CommandExecutor.h"

CommandExecutor::CommandExecutor(QObject *parent)
    : QObject(parent), m_requiresResponse(false)
{

} /* CommandExecutor::CommandExecutor */

void CommandExecutor::setRequiresResponse(const bool requiresResponse)
{
    m_requiresResponse = requiresResponse;
} /* CommandExecutor::setRequiresResponse */
