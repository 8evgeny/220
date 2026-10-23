
#ifndef COMMAND_DECORATOR_H
#define COMMAND_DECORATOR_H


#include "Command.h"


class CommandDecorator : public Command
{
public:
    CommandDecorator(  Command *const command);

    ~CommandDecorator();

    Command *getCommand() const;

    bool isValid() const override;

    bool isRequiresResponse() const override;

    uint8_t getErrorCode() override;

    uint8_t getSrcDeviceId() const override;
    uint8_t getSrcFunction() const override;

    uint8_t getDstDeviceId() const override;
    uint8_t getDstFunction() const override;

    bool isRequest() const override;

private:
    Command *const  m_pCommand;
};


#endif

