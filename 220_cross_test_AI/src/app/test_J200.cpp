#include "j200/lens_j200.h"
#include "settingsmanager.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QFileInfo>
#include <QTextStream>
#include <QTimer>

namespace
{
    bool queryLens(LensJ200 &lens, Lens::Command command, int timeoutMs)
    {
        QEventLoop eventLoop;
        QTimer timeout;
        timeout.setSingleShot(true);

        bool responseReceived = false;
        bool responseOk = false;

        QObject::connect(&lens, &LensJ200::sendResponse, &eventLoop,
                         [&eventLoop, &responseReceived, &responseOk](bool isOk)
                         {
                             responseReceived = true;
                             responseOk = isOk;
                             eventLoop.quit();
                         });

        QObject::connect(&timeout, &QTimer::timeout, &eventLoop, &QEventLoop::quit);

        QByteArray request(1, static_cast<char>(command));
        timeout.start(timeoutMs);
        lens.send(request);
        eventLoop.exec();

        return responseReceived && responseOk;
    } /* queryLens */
} /* namespace */

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    QCoreApplication::setApplicationName("test_J200");
    QCoreApplication::setApplicationVersion("1.0");

    QTextStream output(stdout);
    QTextStream error(stderr);

    const QString configPath = application.applicationDirPath() + "/config.ini";
    if (!QFileInfo::exists(configPath))
    {
        error << "Error: config.ini was not found at " << configPath << '\n';
        return 2;
    }

    SettingsManager settings(configPath);
    settings.loadSettings();
    const QString portName = settings.settings_store.ir_lens_uart_port.trimmed();
    const qint32 baudrate = settings.settings_store.ir_lens_uart_baudrate;
    if (portName.isEmpty() || baudrate <= 0)
    {
        error << "Error: IR_Lens_UART_Device or IR_Lens_UART_Baudrate is invalid in "
              << configPath << '\n';
        return 2;
    }

    LensJ200 lens;
    output << "Connecting to LensJ200 on " << portName << " at " << baudrate << " baud...\n";
    output.flush();

    if (!lens.open(portName, static_cast<quint32>(baudrate)))
    {
        error << "[FAIL] Could not open LensJ200 serial port " << portName << ".\n";
        return 1;
    }

    if (!queryLens(lens, Lens::Command::Report, 3000))
    {
        lens.close();
        error << "[FAIL] LensJ200 did not respond to the status request.\n";
        return 1;
    }

    const LensStatus status = lens.status();
    output << "[PASS] LensJ200 is reachable.\n"
           << "[PASS] Zoom position: " << status.zoomPosition << '\n'
           << "[PASS] Focus position: " << status.focusPosition << '\n'
           << "[PASS] Temperature: " << status.temperature << '\n';

    bool extendedChecksPassed = true;
    if (queryLens(lens, Lens::Command::GetFocalLength, 3000))
        output << "[PASS] Focal length: " << lens.status().focalLength << '\n';
    else
    {
        output << "[FAIL] Focal length request failed.\n";
        extendedChecksPassed = false;
    }

    if (queryLens(lens, Lens::Command::Temperature, 3000))
        output << "[PASS] Temperature inquiry: " << lens.status().temperature << '\n';
    else
    {
        output << "[FAIL] Temperature inquiry failed.\n";
        extendedChecksPassed = false;
    }

    lens.close();
    if (!extendedChecksPassed)
    {
        error << "[FAIL] One or more LensJ200 parameter queries failed.\n";
        return 1;
    }

    output << "[PASS] LensJ200 diagnostics completed.\n";
    return 0;
} /* main */
