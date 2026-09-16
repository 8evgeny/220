
#ifndef COMMAND_H
#define COMMAND_H


#include <QByteArray>


class CommandExecutor;



class Command
{
public:
    Command( CommandExecutor *const commandExecutor = nullptr);

    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual QByteArray serialize() const = 0;
    virtual bool isValid() const = 0;
    virtual bool isRequiresResponse() const = 0;

    virtual uint8_t getErrorCode() = 0;

    virtual uint8_t getSrcDeviceId() const = 0;
    virtual uint8_t getSrcFunction() const = 0;

    virtual uint8_t getDstDeviceId() const = 0;
    virtual uint8_t getDstFunction() const = 0;

    virtual bool isRequest() const = 0;


protected:
    CommandExecutor *const  m_pCommandExecutor;
};



#endif

