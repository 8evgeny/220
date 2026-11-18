
#include "CDelimiterWithLenAndCrcMessageParser.h"

#include <QDebug>


CDelimiterWithLenAndCrcMessageParser::CDelimiterWithLenAndCrcMessageParser( const QByteArray &startDelimiter,
                                                                            const uint32_t headerSize,
                                                                            const uint32_t lengthFieldOffset,
                                                                            const uint32_t lengthFieldSize,
                                                                            const uint32_t crcFieldSize,
                                                                            const uint32_t maxPayloadSize,
                                                                            QObject *parent):
    MessageParser( parent)
  , m_startDelimiter( startDelimiter)
  , m_headerSize( headerSize)
  , m_lengthFieldOffset( lengthFieldOffset)
  , m_lengthFieldSize( lengthFieldSize)
  , m_crcFieldSize( crcFieldSize)
  , m_maxPayloadSize( maxPayloadSize)
{
    if( m_startDelimiter.isEmpty() || m_startDelimiter.isNull() || m_startDelimiter.size() != 1)
    {
        qFatal( "Invalid startDelimiter size");
    }

    if( (m_lengthFieldOffset + m_lengthFieldSize) > m_headerSize)
    {
        qFatal( "Invalid header size OR LengthField");
    }
}


void CDelimiterWithLenAndCrcMessageParser::processData( const QByteArray data)
{
    m_data.append( data);

    do
    {
        removeUnnecessaryData();

        if( m_data.size() < (m_startDelimiter.size() + m_headerSize + m_crcFieldSize))
            return;

        const uint32_t payloadSize = getPayloadSize();
        if( payloadSize > m_maxPayloadSize)
        {
            m_data.clear();
            return;
        }

        if( m_data.size() < (m_startDelimiter.size() + m_headerSize + payloadSize + m_crcFieldSize))
            return;

        const Message msg = createMessage( payloadSize);
        m_data.remove( 0, msg.getSize());

        emit messageParsed( msg);
    }
    while( m_data.size() >= (m_startDelimiter.size() + m_headerSize + m_crcFieldSize));
}


void CDelimiterWithLenAndCrcMessageParser::removeUnnecessaryData()
{
    if( m_data.size() < 1)
        return;

    const int index = m_data.indexOf( m_startDelimiter);
    if( index == -1)
    {
        m_data.clear();
        return;
    }

    if( index > 0)
    {
        m_data.remove( 0, index);
    }
}


uint32_t CDelimiterWithLenAndCrcMessageParser::getPayloadSize() const
{
    if( m_data.size() < (m_startDelimiter.size() + m_headerSize))
        return 0;

    const uint8_t *const header = (uint8_t *)(m_data.constData() + m_startDelimiter.size());

    uint32_t size = 0;

    switch( m_lengthFieldSize)
    {
    case sizeof( uint8_t):  size = *(uint8_t  *)(header + m_lengthFieldOffset);     break;
    case sizeof(uint16_t):  size = *(uint16_t *)(header + m_lengthFieldOffset);     break;
    case sizeof(uint32_t):  size = *(uint32_t *)(header + m_lengthFieldOffset);     break;
    default:
        qFatal( "Invalid lengthFieldSize");
        break;
    }

    return size;
}


const Message CDelimiterWithLenAndCrcMessageParser::createMessage( const uint32_t payloadSize) const
{
    if( m_data.size() < (m_startDelimiter.size() + m_headerSize + payloadSize + m_crcFieldSize))
        return Message();

    const char *const delimiterOffset = m_data.constData();
    const char *const headerOffset = delimiterOffset + m_startDelimiter.size();
    const char *const payloadOffset = headerOffset + m_headerSize;
    const char *const crcOffset = payloadOffset + payloadSize;

    return Message(
                    QByteArray( delimiterOffset, m_startDelimiter.size()),
                    QByteArray( headerOffset, (m_headerSize + payloadSize)),
                    QByteArray( crcOffset, m_crcFieldSize)
                   );
}
