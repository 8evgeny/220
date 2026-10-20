#include "terminalsettingsview.h"
#include "ui_terminalsettingsview.h"

#include <qmv/qmvmodel.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/valueproperty.h>

terminalsettingsview::terminalsettingsview(QWidget *parent) :
    QMVView(parent),
    _ui(new Ui::terminalsettingsview)
{
    _ui->setupUi(this);

    int label_width = 500;

    terminalsettingsview::setMinimumSize(label_width, 0);

    _ui->enable_click_control->set_label_text("Управление ЛВ кликами");
    _ui->enable_click_control->set_label_width(label_width);
    _ui->enable_click_control->hide_set_btn();
    _ui->enable_click_control->hide_get_btn();

    _ui->enable_tracker_deviation->set_label_text("Разрешить отклонение ЛВ кликами");
    _ui->enable_tracker_deviation->set_label_width(label_width);
    _ui->enable_tracker_deviation->hide_set_btn();
    _ui->enable_tracker_deviation->hide_get_btn();

    _ui->enable_panel->set_label_text("Использовать пульт");
    _ui->enable_panel->set_label_width(label_width);
    _ui->enable_panel->hide_set_btn();
    _ui->enable_panel->hide_get_btn();

    // Инфографика
    _ui->enable_infographics_on_screen->set_label_text("Выводить инфографику на экран");
    _ui->enable_infographics_on_screen->set_label_width(label_width);
    _ui->enable_infographics_on_screen->set_val(true);
    _ui->enable_infographics_on_screen->hide_set_btn();
    _ui->enable_infographics_on_screen->hide_get_btn();

    _ui->enable_infographics_on_record->set_label_text("Отрисовывать инфографику на видеозаписи");
    _ui->enable_infographics_on_record->set_label_width(label_width);
    _ui->enable_infographics_on_record->set_val(true);
    _ui->enable_infographics_on_record->hide_set_btn();
    _ui->enable_infographics_on_record->hide_get_btn();

    _ui->enable_infographics_on_picture->set_label_text("Отрисовывать инфографику на сохраняемых кадрах");
    _ui->enable_infographics_on_picture->set_label_width(label_width);
    _ui->enable_infographics_on_picture->set_val(true);
    _ui->enable_infographics_on_picture->hide_set_btn();
    _ui->enable_infographics_on_picture->hide_get_btn();

    _ui->infographics_angleInversionX->set_label_text("Инвертировать X");
    _ui->infographics_angleInversionX->set_label_width(label_width);
    _ui->infographics_angleInversionX->set_val(false);
    _ui->infographics_angleInversionX->hide_set_btn();
    _ui->infographics_angleInversionX->hide_get_btn();

    _ui->infographics_angleInversionZ->set_label_text("Инвертировать Z");
    _ui->infographics_angleInversionZ->set_label_width(label_width);
    _ui->infographics_angleInversionZ->set_val(false);
    _ui->infographics_angleInversionZ->hide_set_btn();
    _ui->infographics_angleInversionZ->hide_get_btn();

    // Настройки джойстика
    _ui->enable_joystick->set_label_text("Использовать джойстик");
    _ui->enable_joystick->set_label_width(label_width);
    _ui->enable_joystick->set_val(true);
    _ui->enable_joystick->hide_set_btn();
    _ui->enable_joystick->hide_get_btn();

    _ui->joystick_x_inv->set_label_text("Инвертировать X");
    _ui->joystick_x_inv->set_label_width(label_width);
    _ui->joystick_x_inv->hide_set_btn();
    _ui->joystick_x_inv->hide_get_btn();

    _ui->joystick_z_inv->set_label_text("Инвертировать Z");
    _ui->joystick_z_inv->set_label_width(label_width);
    _ui->joystick_z_inv->hide_set_btn();
    _ui->joystick_z_inv->hide_get_btn();

    // Настройки клавиатуры
    _ui->keyboard_x_inv->set_label_text("Инвертировать X");
    _ui->keyboard_x_inv->set_label_width(label_width);
    _ui->keyboard_x_inv->hide_set_btn();
    _ui->keyboard_x_inv->hide_get_btn();

    _ui->keyboard_z_inv->set_label_text("Инвертировать Z");
    _ui->keyboard_z_inv->set_label_width(label_width);
    _ui->keyboard_z_inv->hide_set_btn();
    _ui->keyboard_z_inv->hide_get_btn();

}

terminalsettingsview::~terminalsettingsview()
{
    delete _ui;
}

void terminalsettingsview::connect_model()
{
    _ui->enable_click_control    ->connect_property(model->value_property(_ui->enable_click_control    ->objectName()));
    _ui->enable_tracker_deviation->connect_property(model->value_property(_ui->enable_tracker_deviation->objectName()));
    _ui->enable_panel            ->connect_property(model->value_property(_ui->enable_panel            ->objectName()));
    // Инфографика
    _ui->enable_infographics_on_record ->connect_property(model->value_property(_ui->enable_infographics_on_record ->objectName()));
    _ui->enable_infographics_on_picture->connect_property(model->value_property(_ui->enable_infographics_on_picture->objectName()));
    _ui->enable_infographics_on_screen ->connect_property(model->value_property(_ui->enable_infographics_on_screen ->objectName()));
    _ui->infographics_angleInversionX   ->connect_property(model->value_property(_ui->infographics_angleInversionX ->objectName()));
    _ui->infographics_angleInversionZ   ->connect_property(model->value_property(_ui->infographics_angleInversionZ ->objectName()));
    // Настройки джойстика
    _ui->enable_joystick->connect_property(model->value_property(_ui->enable_joystick->objectName()));
    _ui->joystick_x_inv ->connect_property(model->value_property(_ui->joystick_x_inv ->objectName()));
    _ui->joystick_z_inv ->connect_property(model->value_property(_ui->joystick_z_inv ->objectName()));
    // Настройки клавиатуры
    _ui->keyboard_x_inv ->connect_property(model->value_property(_ui->keyboard_x_inv ->objectName()));
    _ui->keyboard_z_inv ->connect_property(model->value_property(_ui->keyboard_z_inv ->objectName()));
}

void terminalsettingsview::on_rb_pointerShip_clicked(bool checked)
{
    if(checked) {
        emit sg_infographic_setShipPointer();
    }
}
void terminalsettingsview::on_rb_pointerHelicopter_clicked(bool checked)
{
    if(checked) {
        emit sg_infographic_setHelicopterPointer();
    }
}

