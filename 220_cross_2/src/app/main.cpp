#include "ldc20i/ldc20i_worker.h"

#include <QCoreApplication>
#include <QDebug>
#include <QMetaType>
#include <QRegularExpression>
#include <QSettings>
#include <QSocketNotifier>
#include <QTimer>

#include <cerrno>
#include <cstring>
#include <string>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("ldc20i-control");

    qRegisterMetaType<LDC_CMD_enum>("LDC_CMD_enum");
    qRegisterMetaType<LDC_ANS_enum>("LDC_ANS_enum");
    qRegisterMetaType<LDC_STATE>("LDC_STATE");

    const QString settingsPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(settingsPath, QSettings::IniFormat);
    const QString port = settings.value("LDC20I.UART_Device", "/dev/ttyS7").toString();
    bool baudrateOk = false;
    const int baudrate = settings.value("LDC20I.UART_Baudrate", 115200).toInt(&baudrateOk);
    if (!baudrateOk || baudrate <= 0) {
        qCritical() << "Invalid LDC20I.UART_Baudrate in" << settingsPath;
        return 1;
    }

    qInfo() << "Starting LDC20I controller on" << port << "at" << baudrate << "baud";
    LDC20I_Worker ldc(port, static_cast<quint32>(baudrate), nullptr);

    QObject::connect(&ldc, &LDC20I_Worker::send_LDC_State, &app, [](LDC_STATE state) {
        qInfo() << "LDC measurements: laser temp" << state.meas.temp_ld
                << "power switch temp" << state.meas.temp_power_sw
                << "voltage" << state.meas.voltage
                << "distance" << state.dist.distance
                << "mean distance" << state.dist.distance_mean;
    }, Qt::QueuedConnection);

    QTimer::singleShot(0, &app, [&ldc] {
        ldc.proc_CtrlInitConnection();
    });

    if (::isatty(STDIN_FILENO)) {
        qInfo() << "Commands: measurements, distance, reg-state, reg <0..255>, emit-state,"
                   " emit <0|1>, mode <single|continuous>, period <usec>, frequency <Hz>, quit";
        auto *notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, &app);
        std::string inputBuffer;
        QObject::connect(notifier, &QSocketNotifier::activated, &app,
                         [&app, &ldc, notifier, &inputBuffer](QSocketDescriptor, QSocketNotifier::Type) {
            char buffer[256];
            const ssize_t count = ::read(STDIN_FILENO, buffer, sizeof(buffer));
            if (count < 0) {
                if (errno == EINTR)
                    return;
                qWarning() << "Failed reading LDC command input:" << std::strerror(errno);
                notifier->setEnabled(false);
                return;
            }
            if (count == 0) {
                notifier->setEnabled(false);
                return;
            }
            inputBuffer.append(buffer, static_cast<size_t>(count));

            size_t newline = 0;
            while ((newline = inputBuffer.find('\n')) != std::string::npos) {
                const QStringList args = QString::fromUtf8(inputBuffer.data(), static_cast<qsizetype>(newline))
                                             .trimmed()
                                             .split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
                inputBuffer.erase(0, newline + 1);
                if (args.isEmpty())
                    continue;

                const QString command = args[0].toLower();
                LDC_CMD_enum ldcCommand;
                quint32 value = 0;
                bool valueOk = false;

                if (command == "measurements" && args.size() == 1) {
                    ldcCommand = LDC_GET_MEASUREMENTS;
                } else if (command == "distance" && args.size() == 1) {
                    ldcCommand = LDC_GET_DISTANCE;
                } else if (command == "reg-state" && args.size() == 1) {
                    ldcCommand = LDC_GET_TEMP_REG_STATE;
                } else if (command == "emit-state" && args.size() == 1) {
                    ldcCommand = LDC_GET_EMIT_STATE;
                } else if ((command == "reg" || command == "emit" || command == "period"
                            || command == "frequency") && args.size() == 2) {
                    value = args[1].toUInt(&valueOk);
                    if (!valueOk || (command == "emit" && value > 1)) {
                        qWarning() << "Invalid value for" << command;
                        continue;
                    }
                    if (command == "reg")
                        ldcCommand = LDC_SET_TEMP_REG_STATE;
                    else if (command == "emit")
                        ldcCommand = LDC_SET_EMIT_STATE;
                    else if (command == "period")
                        ldcCommand = LDC_SET_EMIT_PERIOD;
                    else
                        ldcCommand = LDC_SET_EMIT_MFREQ;
                } else if (command == "mode" && args.size() == 2
                           && (args[1].compare("single", Qt::CaseInsensitive) == 0
                               || args[1].compare("continuous", Qt::CaseInsensitive) == 0)) {
                    ldcCommand = LDC_SET_EMIT_MODE;
                    value = args[1].compare("continuous", Qt::CaseInsensitive) == 0 ? 1 : 0;
                } else if (command == "quit" && args.size() == 1) {
                    app.quit();
                    continue;
                } else {
                    qWarning() << "Unknown command or invalid arguments:" << args.join(' ');
                    continue;
                }

                QMetaObject::invokeMethod(&ldc, [&ldc, ldcCommand, value] {
                    ldc.proc_LDCSendCmd(ldcCommand, value);
                }, Qt::QueuedConnection);
            }
        });
    }

    return app.exec();
}
