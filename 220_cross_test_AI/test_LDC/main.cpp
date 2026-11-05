#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QSerialPort>
#include <QSettings>
#include <QTimer>
#include <QtEndian>

#include <array>

namespace {

constexpr quint8 FrameHeader = 0xff;
constexpr quint8 GetDistanceCommand = 0x0a;
constexpr quint8 DistanceAnswerCommand = 0x4a;
constexpr quint8 ErrorAnswerCommand = 0x40;
constexpr int DistancePayloadSize = 12;

QByteArray makeDistanceRequest()
{
    static const std::array<quint8, 256> crcTable = [] {
        std::array<quint8, 256> table{};
        constexpr quint8 polynomial = 0x89;

        for (int i = 0; i < 256; ++i) {
            quint8 value = static_cast<quint8>(i);
            table[i] = (value & 0x80) ? value ^ polynomial : value;
            for (int bit = 1; bit < 8; ++bit) {
                value = static_cast<quint8>(table[i] << 1);
                if (value & 0x80)
                    value ^= polynomial;
                table[i] = value;
            }
        }
        return table;
    }();

    QByteArray request;
    request.append(static_cast<char>(FrameHeader));
    request.append(static_cast<char>(2));
    request.append(static_cast<char>(GetDistanceCommand));

    quint8 crc = 0;
    for (const char byte : request)
        crc = crcTable[static_cast<quint8>((crc << 1) ^ static_cast<quint8>(byte))];
    request.append(static_cast<char>(crc));
    return request;
}

} // namespace

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("test_LDC"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Read one distance measurement from an LDC20 rangefinder."));
    parser.addHelpOption();

    const QCommandLineOption configOption(
        {QStringLiteral("c"), QStringLiteral("config")},
        QStringLiteral("Path to config.ini (default: config.ini next to the executable)."),
        QStringLiteral("file"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/config.ini"));
    const QCommandLineOption timeoutOption(
        {QStringLiteral("t"), QStringLiteral("timeout")},
        QStringLiteral("Response timeout in milliseconds (default: 3000)."),
        QStringLiteral("ms"),
        QStringLiteral("3000"));
    parser.addOptions({configOption, timeoutOption});
    parser.process(app);

    const QString configPath = parser.value(configOption);
    if (!QFileInfo::exists(configPath)) {
        qCritical().noquote() << "Configuration file not found:" << configPath;
        return 2;
    }

    QSettings settings(configPath, QSettings::IniFormat);
    const QString portName = settings.value(QStringLiteral("LDC20I.UART_Device")).toString().trimmed();
    bool baudOk = false;
    const int baudRate = settings.value(QStringLiteral("LDC20I.UART_Baudrate")).toInt(&baudOk);
    if (settings.status() != QSettings::NoError) {
        qCritical().noquote() << "Cannot read configuration file:" << configPath;
        return 2;
    }
    bool timeoutOk = false;
    const int timeoutMs = parser.value(timeoutOption).toInt(&timeoutOk);
    if (portName.isEmpty() || !baudOk || baudRate <= 0 || !timeoutOk || timeoutMs <= 0) {
        qCritical() << "Config must contain a serial port and positive baud rate; timeout must be positive.";
        return 2;
    }

    QSerialPort serial;
    serial.setPortName(portName);
    serial.setBaudRate(baudRate);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!serial.open(QIODevice::ReadWrite)) {
        qCritical().noquote() << "Cannot open" << serial.portName() << ":" << serial.errorString();
        return 2;
    }

    QByteArray receiveBuffer;
    int exitCode = 1;
    QTimer timeout;
    timeout.setSingleShot(true);

    QObject::connect(&serial, &QSerialPort::readyRead, &app, [&] {
        receiveBuffer.append(serial.readAll());
        while (!receiveBuffer.isEmpty()) {
            if (static_cast<quint8>(receiveBuffer.front()) != FrameHeader) {
                receiveBuffer.remove(0, 1);
                continue;
            }
            if (receiveBuffer.size() < 3)
                return;

            const auto length = static_cast<quint8>(receiveBuffer.at(1));
            if (length < 2 || length > 50) {
                receiveBuffer.remove(0, 1);
                continue;
            }

            const int frameSize = static_cast<int>(length) + 2;
            if (receiveBuffer.size() < frameSize)
                return;

            const QByteArray frame = receiveBuffer.left(frameSize);
            receiveBuffer.remove(0, frameSize);
            const quint8 command = static_cast<quint8>(frame.at(2));

            if (command == ErrorAnswerCommand) {
                qCritical() << "LDC20 rejected the distance request.";
                serial.close();
                timeout.stop();
                app.exit(1);
                return;
            }
            if (command != DistanceAnswerCommand)
                continue;
            if (frameSize < 3 + DistancePayloadSize + 1) {
                qCritical() << "LDC20 returned an incomplete distance response.";
                serial.close();
                timeout.stop();
                app.exit(1);
                return;
            }

            const auto *payload = reinterpret_cast<const uchar *>(frame.constData() + 3);
            const quint32 emitted = qFromLittleEndian<quint32>(payload);
            const quint32 received = qFromLittleEndian<quint32>(payload + 4);
            const quint32 rawDistance = qFromLittleEndian<quint32>(payload + 8);
            const double distance = rawDistance * 1.79;

            qInfo().noquote() << QStringLiteral("Distance: %1 (raw: %2; emitted: %3; received: %4)")
                                     .arg(distance, 0, 'f', 2)
                                     .arg(rawDistance)
                                     .arg(emitted)
                                     .arg(received);
            exitCode = 0;
            serial.close();
            timeout.stop();
            app.quit();
            return;
        }
    });

    QObject::connect(&timeout, &QTimer::timeout, &app, [&] {
        qCritical().noquote() << "Timed out waiting for an LDC20 distance response on"
                              << serial.portName();
        serial.close();
        app.exit(1);
    });

    QObject::connect(&serial, &QSerialPort::errorOccurred, &app,
                     [&](QSerialPort::SerialPortError error) {
        if (error == QSerialPort::NoError || error == QSerialPort::TimeoutError)
            return;
        qCritical().noquote() << "Serial port error:" << serial.errorString();
        serial.close();
        timeout.stop();
        app.exit(1);
    });

    const QByteArray request = makeDistanceRequest();
    if (serial.write(request) != request.size()) {
        qCritical().noquote() << "Failed to send the LDC20 distance request:"
                              << serial.errorString();
        serial.close();
        return 1;
    }
    timeout.start(timeoutMs);
    return app.exec() == 0 ? exitCode : 1;
}
