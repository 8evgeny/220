
#ifndef DELIMITER_AND_CRC_8_COMMAND_DECORATOR_H
#define DELIMITER_AND_CRC_8_COMMAND_DECORATOR_H


#include "CommandDecorator.h"

#include <QByteArray>


class DelimiterAndCrc8CommandDecorator : public CommandDecorator
{
public:
    DelimiterAndCrc8CommandDecorator( Command *const command, const QByteArray &startDelimiter = "\xAA");

    QByteArray serialize() const override;

    void execute() override;


private:
    const QByteArray    m_startDelimiter;

};



#endif

