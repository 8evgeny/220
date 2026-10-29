#include "ev9500m/camera_ev9500m.h"
#include "settingsmanager.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);
    QCoreApplication::setApplicationName("test_EV9500");
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
    const QString portName = settings.settings_store.tv_cam_uart_port.trimmed();
    const qint32 baudrate = settings.settings_store.tv_cam_uart_baudrate;
    if (portName.isEmpty() || baudrate <= 0)
    {
        error << "Error: TV_UART_Device or TV_UART_Baudrate is invalid in " << configPath << '\n';
        return 2;
    }

    TVCamera camera;
    output << "Connecting to Sony camera on " << portName << " at " << baudrate << " baud...\n";
    output.flush();

    if (camera.connect(portName, baudrate) != VISCA_SUCCESS)
    {
        error << "[FAIL] Camera unavailable or not a Sony VISCA camera.\n";
        return 1;
    }

    output << "[PASS] Camera is reachable; vendor=0x" << Qt::hex << camera.vendor()
           << " model=0x" << camera.model() << Qt::dec << '\n';

    bool basicChecksPassed = true;
    const auto reportBasic = [&output, &basicChecksPassed](const QString &name, bool success,
                                                           qint64 value)
    {
        output << (success ? "[PASS] " : "[FAIL] ") << name << ": " << value << '\n';
        basicChecksPassed &= success;
    } /* lambda */;

    bool queryPassed = camera.getTemperature();
    reportBasic("Temperature (C)", queryPassed, camera.status()->temperature);
    queryPassed = camera.getIris();
    reportBasic("Iris", queryPassed, camera.status()->iris);
    queryPassed = camera.getShutter();
    reportBasic("Shutter", queryPassed, camera.status()->shutter);
    queryPassed = camera.getGain();
    reportBasic("Gain", queryPassed, camera.status()->gain);
    queryPassed = camera.getContrast();
    reportBasic("Contrast", queryPassed, camera.status()->contrast);
    queryPassed = camera.getFocusValue();
    reportBasic("Focus position", queryPassed, camera.status()->focus_val);
    queryPassed = camera.getZoomValue();
    reportBasic("Zoom position", queryPassed, camera.status()->zoom_val);
    queryPassed = camera.getAWBDisplayMode();
    reportBasic("Spot AWB display mode", queryPassed, camera.status()->awb_display_mode);

    output << "Extended parameters:\n";
    for (const CameraDiagnosticParameter &parameter : camera.queryAdvancedParameters())
    {
        if (parameter.supported)
            output << "  [PASS] " << parameter.name << ": " << parameter.value << '\n';
        else
            output << "  [N/A]  " << parameter.name << " (unsupported or query failed)\n";
    }

    camera.disconnect();

    if (!basicChecksPassed)
    {
        error << "[FAIL] One or more basic camera parameter queries failed.\n";
        return 1;
    }

    output << "[PASS] Sony camera diagnostics completed.\n";
    return 0;
} /* main */
