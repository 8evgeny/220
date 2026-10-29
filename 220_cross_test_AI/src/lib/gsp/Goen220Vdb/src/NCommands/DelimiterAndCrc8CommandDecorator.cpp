
#include "DelimiterAndCrc8CommandDecorator.h"

#include "Goen220Crc.h"

DelimiterAndCrc8CommandDecorator::DelimiterAndCrc8CommandDecorator(Command *const command,
                                                                   const QByteArray &startDelimiter)
    : CommandDecorator(command), m_startDelimiter(startDelimiter)
{

} /* DelimiterAndCrc8CommandDecorator::DelimiterAndCrc8CommandDecorator */

QByteArray DelimiterAndCrc8CommandDecorator::serialize() const
{
    // const QByteArray cmd = getCommand()->serialize();

    // const uint8_t crc = goen220__crc8_calc( (const uint8_t *const)cmd.constData(), (const
    // uint8_t)cmd.size());

    //    return m_startDelimiter + cmd + QByteArray( "\xDD");
    // return m_startDelimiter + cmd + QByteArray( (const char *)&crc, sizeof(crc));

    const QByteArray cmd = m_startDelimiter + getCommand()->serialize();
    const uint8_t crc =
        goen220__crc8_calc((const uint8_t *const)cmd.constData(), (const uint8_t)cmd.size());
    return cmd + QByteArray((const char *)&crc, sizeof(crc));
} /* DelimiterAndCrc8CommandDecorator::serialize */

void DelimiterAndCrc8CommandDecorator::execute()
{

} /* DelimiterAndCrc8CommandDecorator::execute */
