
#include "CommandDecorator.h"



CommandDecorator::CommandDecorator(  Command *const command):
    m_pCommand( command)
{

}


CommandDecorator::~CommandDecorator()
{
    delete m_pCommand;
}


Command *CommandDecorator::getCommand() const
{
    return m_pCommand;
}


bool CommandDecorator::isValid() const
{
    return m_pCommand->isValid();
}


bool CommandDecorator::isRequiresResponse() const
{
    return m_pCommand->isRequiresResponse();
}


uint8_t CommandDecorator::getErrorCode()
{
    return m_pCommand->getErrorCode();
}


uint8_t CommandDecorator::getSrcDeviceId() const
{
    return m_pCommand->getSrcDeviceId();
}


uint8_t CommandDecorator::getSrcFunction() const
{
    return m_pCommand->getSrcFunction();
}


uint8_t CommandDecorator::getDstDeviceId() const
{
    return m_pCommand->getDstDeviceId();
}


uint8_t CommandDecorator::getDstFunction() const
{
    return m_pCommand->getDstFunction();
}


bool CommandDecorator::isRequest() const
{
    return m_pCommand->isRequest();
}
