
#ifndef MESSAGE_H
#define MESSAGE_H

#include <QByteArray>

class Message
{
  public:

    Message()
    {

    } // Message

    Message(const QByteArray startDelimiter,
            const QByteArray payload,
            const QByteArray crc) : m_startDelimiter(startDelimiter), m_payload(payload), m_crc(crc)
    {

    } // Message

    const QByteArray &getStartDelimiter() const
    {
        return m_startDelimiter;
    } // &getStartDelimiter() const

    const QByteArray &getPayload() const
    {
        return m_payload;
    } // &getPayload() const

    const QByteArray &getCrc() const
    {
        return m_crc;
    } // &getCrc() const

    uint32_t getSize() const
    {
        return (uint32_t)m_startDelimiter.size() + (uint32_t)m_payload.size() + (uint32_t)m_crc.size();
    } // getSize

  private:
    QByteArray m_startDelimiter;
    QByteArray m_payload;
    QByteArray m_crc;
}; // class Message

#endif
