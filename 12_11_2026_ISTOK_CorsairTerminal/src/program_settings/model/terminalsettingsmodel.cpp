#include "terminalsettingsmodel.h"
#include <qmv/property/stringvalueproperty.h>
#include <qmv/property/cmdproperty.h>
#include <QTimer>

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logTerminalSettings, "terminalsettings")

terminalsettingsmodel::terminalsettingsmodel(QObject* parent, uint32_t id, QString name)
    : QMVModel(parent, id, name)
    , enable_click_control    (new BooleanValueProperty(this, id_click_control,     "enable_click_control",     false))
    , enable_tracker_deviation(new BooleanValueProperty(this, id_tracker_deviation, "enable_tracker_deviation", false))
    , using_panel             (new BooleanValueProperty(this, id_panel,             "enable_panel",             false))
    // Инфографика
    , enable_infographics_on_screen (new BooleanValueProperty(this, id_infographics_screen,  "enable_infographics_on_screen",  true))
    , enable_infographics_on_record (new BooleanValueProperty(this, id_infographics_record,  "enable_infographics_on_record",  true))
    , enable_infographics_on_picture(new BooleanValueProperty(this, id_infographics_picture, "enable_infographics_on_picture", true))
    , infographics_angleInversionX   (new BooleanValueProperty(this, id_infographics_angleInversionX, "infographics_angleInversionX", false))
    , infographics_angleInversionZ   (new BooleanValueProperty(this, id_infographics_angleInversionZ, "infographics_angleInversionZ", false))
    // Джойстик
    , using_joystick(new BooleanValueProperty(this, id_joystick,       "enable_joystick", true))
    , joystick_x_inv(new BooleanValueProperty(this, id_joystick_x_inv, "joystick_x_inv",  false))
    , joystick_z_inv(new BooleanValueProperty(this, id_joystick_z_inv, "joystick_z_inv",  false))
    // Клавиатура
    , keyboard_x_inv(new BooleanValueProperty(this, id_keyboard_x_inv, "keyboard_x_inv", false))
    , keyboard_z_inv(new BooleanValueProperty(this, id_keyboard_z_inv, "keyboard_z_inv", false))
{
    QTimer::singleShot(500, this, [this]()
                       {
                            emit infographics_angleInversion_X_changed(infographics_angleInversionX->get());
                            emit infographics_angleInversion_Z_changed(infographics_angleInversionZ->get());
                        });

}

void terminalsettingsmodel::set(const uint32_t& id)
{
    switch(id) {
    case id_click_control: {
        const bool flag{enable_click_control->get()};
        qInfo(logTerminalSettings(), "set: Enable click control %s", flag ? "On" : "Off");
        emit click_control_changed(flag);
    } break;
    case id_tracker_deviation: {
        const bool flag{enable_click_control->get()};
        qInfo(logTerminalSettings(), "set: Enable tracker diviation %s", flag ? "On" : "Off");
        emit tracker_deviation_changed(flag);
    } break;
    case id_panel: {
        const bool flag{using_panel->get()};
        qInfo(logTerminalSettings(), "set: Enable Panel %s", flag ? "On" : "Off");
        emit panel_changed(flag);
    } break;
    // Инфографика
    case id_infographics_screen: {
        const bool flag{enable_infographics_on_screen->get()};
        qInfo(logTerminalSettings(), "set: Enable Infographics on screen %s", flag ? "On" : "Off");
        emit infographics_on_screen_changed(flag);
    } break;
    case id_infographics_record: {
        const bool flag{enable_infographics_on_record->get()};
        qInfo(logTerminalSettings(), "set: Enable Infographics on record %s", flag ? "On" : "Off");
        emit infographics_on_record_changed(flag);
    } break;
    case id_infographics_picture: {
        const bool flag{enable_infographics_on_picture->get()};
        qInfo(logTerminalSettings(), "set: Enable Infographics on picture %s", flag ? "On" : "Off");
        emit infographics_on_picture_changed(flag);
    } break;
    case id_infographics_angleInversionX: {
        const bool flag{infographics_angleInversionX->get()};
        qInfo(logTerminalSettings(), "set: Infographics Angle X Inversion %s", flag ? "On" : "Off");
        emit infographics_angleInversion_X_changed(flag);
    } break;
    case id_infographics_angleInversionZ: {
        const bool flag{infographics_angleInversionZ->get()};
        qInfo(logTerminalSettings(), "set: Infographics Angle Z Inversion %s", flag ? "On" : "Off");
        emit infographics_angleInversion_Z_changed(flag);
    } break;
    // Джойстик
    case id_joystick: {
        const bool flag{using_joystick->get()};
        qInfo(logTerminalSettings(), "set: Enable Joystick %s", flag ? "On" : "Off");
        emit joystick_changed(flag);
    } break;
    case id_joystick_x_inv: {
        emit joystick_inv_x(joystick_x_inv->get());
    } break;
    case id_joystick_z_inv: {
        emit joystick_inv_z(joystick_z_inv->get());
    } break;
    // Клавиатура
    case id_keyboard_x_inv: {
        emit keyboard_inv_x(keyboard_x_inv->get());
    } break;
    case id_keyboard_z_inv: {
        emit keyboard_inv_z(keyboard_z_inv->get());
    } break;
    }
}

void terminalsettingsmodel::get(const uint32_t&id)
{
    Q_UNUSED(id)
}

void terminalsettingsmodel::cmd(const uint32_t&id)
{
    Q_UNUSED(id)
}

void terminalsettingsmodel::update_settings()
{
    emit joystick_inv_x(joystick_x_inv->get());
    emit joystick_inv_z(joystick_z_inv->get());

    emit keyboard_inv_x(keyboard_x_inv->get());
    emit keyboard_inv_z(keyboard_z_inv->get());
}

