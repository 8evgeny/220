
#include "CommandDecorator.h"

CommandDecorator::CommandDecorator(Command *const command)
    : m_pCommand(command)
{

} /* CommandDecorator::CommandDecorator */

CommandDecorator::~CommandDecorator()
{
    delete m_pCommand;
} /* CommandDecorator::~CommandDecorator */

Command *CommandDecorator::getCommand() const
{
    return m_pCommand;
} /* CommandDecorator::getCommand */

bool CommandDecorator::isValid() const
{
    return m_pCommand->isValid();
} /* CommandDecorator::isValid */

bool CommandDecorator::isRequiresResponse() const
{
    return m_pCommand->isRequiresResponse();
} /* CommandDecorator::isRequiresResponse */

uint8_t CommandDecorator::getErrorCode()
{
    return m_pCommand->getErrorCode();
} /* CommandDecorator::getErrorCode */

uint8_t CommandDecorator::getSrcDeviceId() const
{
    return m_pCommand->getSrcDeviceId();
} /* CommandDecorator::getSrcDeviceId */

uint8_t CommandDecorator::getSrcFunction() const
{
    return m_pCommand->getSrcFunction();
} /* CommandDecorator::getSrcFunction */

uint8_t CommandDecorator::getDstDeviceId() const
{
    return m_pCommand->getDstDeviceId();
} /* CommandDecorator::getDstDeviceId */

uint8_t CommandDecorator::getDstFunction() const
{
    return m_pCommand->getDstFunction();
} /* CommandDecorator::getDstFunction */

bool CommandDecorator::isRequest() const
{
    return m_pCommand->isRequest();
} /* CommandDecorator::isRequest */
