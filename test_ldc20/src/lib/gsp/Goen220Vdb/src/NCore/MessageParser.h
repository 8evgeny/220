
#ifndef MESSAGE_PARSER_H
#define MESSAGE_PARSER_H


#include "Message.h"

#include <QObject>


class MessageParser : public QObject
{
    Q_OBJECT

public:
    explicit MessageParser( QObject *parent = nullptr):
        QObject( parent)
    {

    }


public slots:
    virtual void processData( const QByteArray data) = 0;

signals:
    void messageParsed( const Message message);


protected:

private:

};



#endif

