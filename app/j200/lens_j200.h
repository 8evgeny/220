#ifndef LENSJ200_H
#define LENSJ200_H

#include <QObject>
#include <QSerialPort>

namespace Lens {

enum class Command : char {
    Report = 0x02,

    ZoomIn = 0x08,
    ZoomOut = 0x09,
    ZoomInContinuous = 0x23,
    ZoomOutContinuous = 0x24,
    ZoomStop = 0x14,

    FocusFarContinuous = 0x22,
    FocusNearContinuous = 0x21,
    FocusStop = 0x1B,

    GotoPosition = 0x0D,
    GetFocalLength = 0x42,
    GotoFocalLength = 0x43,
    SpeedSet = 0x1F,

    Temperature = 0x45
};

enum class Error : char {
    NoError,
    Timeout,
    BadResponse
};

typedef struct
{
    uint8_t synq;
    uint8_t cmd;
    uint16_t len;

} J200_HEAD;

typedef struct
{
    uint8_t ACK;
    uint8_t CS;

} J200_TAIL;


static int maxEncoderValue = 31550;
static int minEncoderValue = 0; //450;

}

struct LensStatus {
    bool isConnected;
    int zoomPosition;
    int zoomMinPosition;
    int zoomMaxPosition;
    int focusPosition;
    int focalLength;
    int temperature;
};

class LensJ200 : public QObject
{
    Q_OBJECT
public:
    explicit LensJ200(QObject *parent = nullptr);
    ~LensJ200();

    bool open(QString portName);
    void close();
    void send(QByteArray req);
    void read();

    LensStatus status() {
        return m_status;
    };

protected:
    enum Packet {
        syncChar = 0,
        command = 1,
        dataLength = 2,
        data = 4,
    };

    QSerialPort serialPort;

    uint8_t getChecksum(QByteArray data, bool isResponse);
    uint8_t getChecksumOld(QByteArray data, bool isResponse);

    bool parseResponse(QByteArray resp);

    LensStatus m_status;
    quint8 m_currentCommandID;
    bool m_isContinuous;
    bool m_pendingRequest;
    QByteArray m_request;

private:
    static const int serialPortTimeout = 2000; // in ms
    int maxPosition;
    int minPosition;
    static const uint8_t minResponsePacketSizeBytes = 6;
    static const uint8_t respTemperaturePacketSizeBytes = 9;
    static const uint8_t successOperation = 0xFE;

    static const int errRead = -1;

signals:
    void sendResponse(bool isOk);
};

#endif // LENSJ200_H
