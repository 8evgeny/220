
#ifndef COMMAND_FACTORY_H
#define COMMAND_FACTORY_H


#include "Message.h"

#include <QObject>


class CommandExecutor;
class Command;


class CommandFactory: public QObject
{
    Q_OBJECT

public:
    explicit CommandFactory( CommandExecutor *const commandExecutor, QObject *parent = nullptr);

    static bool validateMessage( const Message &message);

    Command *createCommand( const Message message);
    Command *createGetVersionCommand( const uint8_t srcDevice, const uint8_t dstDevice) const;

signals:
    void invalidMessageCounterChanged( const int32_t invalidMessages);

public slots:
    void executeMessage( const Message message);
    void setMessage( const Message message);

signals:
    void commandCreated( Command *const cmd);

protected:

private:

private:
    CommandExecutor *const      m_pCommandExecutor;

    int32_t                     m_invalidMessages;

};



#endif

