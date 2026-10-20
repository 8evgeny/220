#include "vlx_sdl_dump_dialog.h"
#include "ui_vlx_sdl_dump_dialog.h"

#include <SDL2/SDL.h>

#include <QTextStream>
#include <QFileDialog>

#include "pluggable_controls/joystick.h"

VlxSdlDumpDialog::VlxSdlDumpDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VlxSdlDumpDialog)
{
    ui->setupUi(this);
    m_timUpdate.setInterval(200);

    connect(ui->pbOk, &QPushButton::clicked, this, &VlxSdlDumpDialog::close);
    connect(ui->pbUpdate, &QPushButton::clicked, this, &VlxSdlDumpDialog::updateDump);
    connect(ui->chbLiveUpdate, &QPushButton::clicked, this, [this](bool state){
        if (state)
            m_timUpdate.start();
        else
            m_timUpdate.stop();
    });
    connect(&m_timUpdate, &QTimer::timeout, this, &VlxSdlDumpDialog::updateDump);

    connect(ui->pbSave, &QPushButton::clicked, this, [this]{
        auto fn = QFileDialog::getSaveFileName(nullptr, "Сохранить файл", {}, "Txt (*.txt);;Все(*.*)");
        if (fn.isEmpty())
            return;

        QFile file(fn);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << ui->pteOutput->toPlainText();
            file.close();
        }
    });

}

VlxSdlDumpDialog::~VlxSdlDumpDialog()
{
    delete ui;
}

void VlxSdlDumpDialog::openDump()
{
    m_joystick = Joystick::currentJoystick();
    QDialog::show();
    QDialog::raise();
    QDialog::activateWindow();
    updateDump();
}

static void updateJoystick(SDL_Joystick *js, QTextStream &s)
{
    s << "Name      : " << SDL_JoystickName(js) << "\n";
    s << "InstanceID: " << SDL_JoystickInstanceID(js) << "\n";

#if SDL_VERSION_ATLEAST(2,0,6)
    s << "Vendor    : 0x"
      << QString("%1").arg(SDL_JoystickGetVendor(js),4,16,QChar('0'))
      << "\n";

    s << "Product   : 0x"
      << QString("%1").arg(SDL_JoystickGetProduct(js),4,16,QChar('0'))
      << "\n";

    s << "Version   : "
      << SDL_JoystickGetProductVersion(js)
      << "\n";
#endif

    SDL_JoystickGUID guid = SDL_JoystickGetGUID(js);
    char guidStr[64];
    SDL_JoystickGetGUIDString(guid, guidStr, sizeof(guidStr));

    s << "GUID      : " << guidStr << "\n";

    int axes    = SDL_JoystickNumAxes(js);
    int buttons = SDL_JoystickNumButtons(js);
    int hats    = SDL_JoystickNumHats(js);
    int balls   = SDL_JoystickNumBalls(js);

    s << "\nCounts\n";
    s << "Axes    : " << axes << "\n";
    s << "Buttons : " << buttons << "\n";
    s << "Hats    : " << hats << "\n";
    s << "Balls   : " << balls << "\n";

    SDL_JoystickUpdate();

    s << "\n========== AXES ==========\n";

    for (int i = 0; i < axes; ++i)
    {
        s << QString("Axis %1 = %2\n")
        .arg(i,2)
            .arg(SDL_JoystickGetAxis(js, i));
    }

    s << "\n========== BUTTONS ==========\n";

    for (int i = 0; i < buttons; ++i)
    {
        s << QString("Button %1 = %2\n")
        .arg(i,2)
            .arg(SDL_JoystickGetButton(js, i));
    }

    s << "\n========== HATS ==========\n";

    auto hatToString = [](Uint8 h)->QString
    {
        if (h == SDL_HAT_CENTERED)
            return "CENTER";

        QStringList l;

        if (h & SDL_HAT_UP)    l << "UP";
        if (h & SDL_HAT_DOWN)  l << "DOWN";
        if (h & SDL_HAT_LEFT)  l << "LEFT";
        if (h & SDL_HAT_RIGHT) l << "RIGHT";

        return l.join("|");
    };

    for (int i = 0; i < hats; ++i)
    {
        Uint8 h = SDL_JoystickGetHat(js, i);

        s << QString("Hat %1 = 0x%2 (%3)\n")
                 .arg(i)
                 .arg(h,2,16,QChar('0'))
                 .arg(hatToString(h));
    }
}

void VlxSdlDumpDialog::updateDump()
{
    m_dump.clear();

    QTextStream s(&m_dump);

    SDL_version ver;
    SDL_GetVersion(&ver);

    s << "========== SDL ==========\n";
    s << "Version : "
      << ver.major << "."
      << ver.minor << "."
      << ver.patch << "\n";

    s << "Platform: " << SDL_GetPlatform() << "\n";

    const char *driver = SDL_GetCurrentVideoDriver();
    s << "Video driver: " << (driver ? driver : "<none>") << "\n";

    s << "\n========== JOYSTICK ==========\n";

    if (!m_joystick) {
        s << "Joystick not opened.\n";
    } else {
        updateJoystick(m_joystick, s);
    }

    ui->pteOutput->setPlainText(m_dump);
}
