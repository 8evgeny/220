
#ifndef MESSAGE_H
#define MESSAGE_H


#include <QByteArray>


class Message
{
public:
    Message()
    {

    }

    Message(    const QByteArray startDelimiter,
                const QByteArray payload,
                const QByteArray crc):
        m_startDelimiter( startDelimiter)
      , m_payload( payload)
      , m_crc( crc)
    {

    }

    const QByteArray &getStartDelimiter() const
    {
        return m_startDelimiter;
    }

    const QByteArray &getPayload() const
    {
        return m_payload;
    }

    const QByteArray &getCrc() const
    {
        return m_crc;
    }

    uint32_t getSize() const
    {
        return (uint32_t)m_startDelimiter.size() + (uint32_t)m_payload.size() + (uint32_t)m_crc.size();
    }

private:
    QByteArray  m_startDelimiter;
    QByteArray  m_payload;
    QByteArray  m_crc;
};


#endif

