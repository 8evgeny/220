#ifndef TERMINALSETTINGSMODEL_H
#define TERMINALSETTINGSMODEL_H

#include <qmv/qmvmodel.h>

#include <qmv/property/booleanvalueproperty.h>
class StringValueProperty;
class CmdProperty;

class terminalsettingsmodel : public QMVModel
{
    Q_OBJECT
public:

    explicit terminalsettingsmodel(QObject * parent, uint32_t id, QString name);

    enum VAL_ID {
        id_click_control = 0,
        id_tracker_deviation,
        id_panel,
        // Инфографика
        id_infographics_screen,
        id_infographics_record,
        id_infographics_picture,
        id_infographics_angleInversionX,
        id_infographics_angleInversionZ,
        // Джойстик
        id_joystick,
        id_joystick_x_inv,
        id_joystick_z_inv,
        // Клавиатура
        id_keyboard_x_inv,
        id_keyboard_z_inv
    };

    enum CMD_ID {

    };

    void set(const uint32_t &id);
    void get(const uint32_t &id);
    void cmd(const uint32_t &id);
    void update_settings();

signals:
    void click_control_changed(bool);
    void tracker_deviation_changed(bool);
    void panel_changed(bool);

    void infographics_on_record_changed(bool);
    void infographics_on_screen_changed(bool);
    void infographics_on_picture_changed(bool);
    void infographics_angleInversion_X_changed(bool);
    void infographics_angleInversion_Z_changed(bool);

    void joystick_changed(bool);
    void joystick_inv_x(bool);
    void joystick_inv_z(bool);

    void keyboard_inv_x(bool);
    void keyboard_inv_z(bool);

private:
    BooleanValueProperty * enable_click_control;
    BooleanValueProperty * enable_tracker_deviation;
    BooleanValueProperty * using_panel;
    // Инфографика
    BooleanValueProperty * enable_infographics_on_screen;
    BooleanValueProperty * enable_infographics_on_record;
    BooleanValueProperty * enable_infographics_on_picture;
    BooleanValueProperty * infographics_angleInversionX;
    BooleanValueProperty * infographics_angleInversionZ;
    // Джойстик
    BooleanValueProperty * using_joystick;
    BooleanValueProperty * joystick_x_inv;
    BooleanValueProperty * joystick_z_inv;
    // Клавиатура
    BooleanValueProperty * keyboard_x_inv;
    BooleanValueProperty * keyboard_z_inv;
};

#endif // TERMINALSETTINGSMODEL_H
