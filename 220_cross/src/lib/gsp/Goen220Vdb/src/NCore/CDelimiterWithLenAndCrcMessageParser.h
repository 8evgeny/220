
#ifndef C_DELIMITER_WITH_LEN_AND_CRC_MESSAGE_PARSER_H
#define C_DELIMITER_WITH_LEN_AND_CRC_MESSAGE_PARSER_H

#include <QObject>

#include "MessageParser.h"



class CDelimiterWithLenAndCrcMessageParser : public MessageParser
{
    Q_OBJECT

public:
    explicit CDelimiterWithLenAndCrcMessageParser(  const QByteArray &startDelimiter = "\xAA",
                                                    const uint32_t headerSize = 8,
                                                    const uint32_t lengthFieldOffset = 6,
                                                    const uint32_t lengthFieldSize = 2,
                                                    const uint32_t crcFieldSize = 1,
                                                    const uint32_t maxPayloadSize = 128,
                                                    QObject *parent = nullptr);

public slots:
    void processData( const QByteArray data)   override;

signals:

protected:

private:
    void removeUnnecessaryData();
    uint32_t getPayloadSize() const;
    const Message createMessage( const uint32_t payloadSize)    const;

private:
    const QByteArray    m_startDelimiter;

    const uint32_t      m_headerSize;
    const uint32_t      m_lengthFieldOffset;
    const uint32_t      m_lengthFieldSize;

    const uint32_t      m_crcFieldSize;

    const uint32_t      m_maxPayloadSize;

    QByteArray          m_data;
};



#endif

