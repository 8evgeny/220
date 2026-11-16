#include "swirctrlview.h"
#include "ui_swirctrlview.h"

#include <qmv/qmvmodel.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/valueproperty.h>

SWIRCtrlView::SWIRCtrlView(QWidget *parent) :
    QMVView(parent),
    ui(new Ui::SWIRCtrlView),
    zoom_in_start(new InoutCmdWidget(this)),
    zoom_in_stop(new InoutCmdWidget(this)),
    zoom_out_start(new InoutCmdWidget(this)),
    zoom_out_stop(new InoutCmdWidget(this)),
    iris_in_start(new InoutCmdWidget(this)),
    iris_in_stop(new InoutCmdWidget(this)),
    iris_out_start(new InoutCmdWidget(this)),
    iris_out_stop(new InoutCmdWidget(this)),
    focus_near_start(new InoutCmdWidget(this)),
    focus_near_stop(new InoutCmdWidget(this)),
    focus_far_start(new InoutCmdWidget(this)),
    focus_far_stop(new InoutCmdWidget(this))
{
    ui->setupUi(this);
    zoom_in_start->setObjectName("zoom_in_start");
    zoom_in_stop->setObjectName("zoom_in_stop");
    zoom_out_start->setObjectName("zoom_out_start");
    zoom_out_stop->setObjectName("zoom_out_stop");
    iris_in_start->setObjectName("iris_in_start");
    iris_in_stop->setObjectName("iris_in_stop");
    iris_out_start->setObjectName("iris_out_start");
    iris_out_stop->setObjectName("iris_out_stop");
    focus_near_start->setObjectName("focus_near_start");
    focus_near_stop->setObjectName("focus_near_stop");
    focus_far_start->setObjectName("focus_far_start");
    focus_far_stop->setObjectName("focus_far_stop");
    zoom_in_start->setVisible(false);
    zoom_in_stop->setVisible(false);
    zoom_out_start->setVisible(false);
    zoom_out_stop->setVisible(false);
    iris_in_start->setVisible(false);
    iris_in_stop->setVisible(false);
    iris_out_start->setVisible(false);
    iris_out_stop->setVisible(false);
    focus_near_start->setVisible(false);
    focus_near_stop->setVisible(false);
    focus_far_start->setVisible(false);
    focus_far_stop->setVisible(false);

    ui->sensor_status->set_label_text("Статус сенсора");
    ui->sensor_status->set_label_width(200);
    ui->sensor_status->hide_set_btn();
    ui->sensor_status->hide_get_btn();
    ui->sensor_status->set_disabled();

    ui->lens_status->set_label_text("Статус объектива");
    ui->lens_status->set_label_width(200);
    ui->lens_status->hide_set_btn();
    ui->lens_status->hide_get_btn();
    ui->lens_status->set_disabled();

    ui->fps->set_label_text("Частота кадров[кадров/с]]");
    ui->fps->set_label_width(200);
    ui->fps->hide_set_btn();
    ui->fps->hide_get_btn();
    ui->fps->set_disabled();

    ui->vres->set_label_text("Вертикальное разрешение[пикс]");
    ui->vres->set_label_width(200);
    ui->vres->hide_set_btn();
    ui->vres->hide_get_btn();
    ui->vres->set_disabled();

    ui->hres->set_label_text("Горизонтальное разрешение[пикс]");
    ui->hres->set_label_width(200);
    ui->hres->hide_set_btn();
    ui->hres->hide_get_btn();
    ui->hres->set_disabled();

    ui->exp->set_label_text("Экспонента");
    ui->exp->set_label_width(200);
    ui->exp->hide_set_btn();
    ui->exp->hide_get_btn();
    ui->exp->set_disabled();

    ui->zoom_drive->set_label_text("Привод увеличения[разр.ацп]");
    ui->zoom_drive->set_label_width(200);
    ui->zoom_drive->hide_set_btn();
    ui->zoom_drive->hide_get_btn();
    ui->zoom_drive->set_disabled();

    ui->focus_drive->set_label_text("Привод фокусировки[разр.ацп]");
    ui->focus_drive->set_label_width(200);
    ui->focus_drive->hide_set_btn();
    ui->focus_drive->hide_get_btn();
    ui->focus_drive->set_disabled();

    ui->lens_temp->set_label_text("Температура объектива[град.Ц.]");
    ui->lens_temp->set_label_width(200);
    ui->lens_temp->hide_set_btn();
    ui->lens_temp->hide_get_btn();
    ui->lens_temp->set_disabled();

    ui->focus_mm->set_label_text("Фокус[мм]");
    ui->focus_mm->set_label_width(200);
    ui->focus_mm->hide_set_btn();
    ui->focus_mm->hide_get_btn();
    ui->focus_mm->set_disabled();

    ui->field_grad->set_label_text("Угловое поле[град.]");
    ui->field_grad->set_label_width(200);
    ui->field_grad->hide_set_btn();
    ui->field_grad->hide_get_btn();
    ui->field_grad->set_disabled();

    ui->zoom->set_label_text("Привод увеличения[разр.ацп]");
    ui->zoom->set_label_width(200);
    ui->zoom->hide_get_btn();

    ui->focus->set_label_text("Привод фокусировки[разр.ацп]");
    ui->focus->set_label_width(200);
    ui->focus->hide_get_btn();

    ui->autofocus_lens_en->set_label_text("Разрешение автофокуса в объективе");
    ui->autofocus_lens_en->set_label_width(200);
    ui->autofocus_lens_en->hide_set_btn();
    ui->autofocus_lens_en->hide_get_btn();

    ui->autofocus_host_en->set_label_text("Разрешение автофокуса с хоста");
    ui->autofocus_host_en->set_label_width(200);
    ui->autofocus_host_en->hide_set_btn();
    ui->autofocus_host_en->hide_get_btn();

    ui->agc->set_cmd_text("Аппаратное АРУ");

    ui->thermostat_on->set_cmd_text("Включить термостат объектива");

    ui->thermostat_off->set_cmd_text("Выключить термостат объектива");

    ui->heat_on->set_cmd_text("Включить обогрев объектива");

    ui->heat_off->set_cmd_text("Выключить обогрев объектива");

    ui->autofocus->set_cmd_text("Автофокус");

    ui->field_sync->set_cmd_text("Синхронизация полей");

    ui->field_sync_narrow->set_cmd_text("Синхронизация в узком поле");

    ui->field_sync_reg->set_cmd_text("Регулярная синхронизация");
}
void SWIRCtrlView::connect_model()
{
    ui->sensor_status->connect_property(model->value_property(ui->sensor_status->objectName()));
    ui->lens_status->connect_property(model->value_property(ui->lens_status->objectName()));
    ui->fps->connect_property(model->value_property(ui->fps->objectName()));
    ui->vres->connect_property(model->value_property(ui->vres->objectName()));
    ui->hres->connect_property(model->value_property(ui->hres->objectName()));
    ui->exp->connect_property(model->value_property(ui->exp->objectName()));
    ui->zoom_drive->connect_property(model->value_property(ui->zoom_drive->objectName()));
    ui->focus_drive->connect_property(model->value_property(ui->focus_drive->objectName()));
    ui->lens_temp->connect_property(model->value_property(ui->lens_temp->objectName()));
    ui->focus_mm->connect_property(model->value_property(ui->focus_mm->objectName()));
    ui->field_grad->connect_property(model->value_property(ui->field_grad->objectName()));
    ui->thermostat_on->connect_property(model->cmd_property(ui->thermostat_on->objectName()));
    ui->thermostat_off->connect_property(model->cmd_property(ui->thermostat_off->objectName()));
    ui->heat_on->connect_property(model->cmd_property(ui->heat_on->objectName()));
    ui->heat_off->connect_property(model->cmd_property(ui->heat_off->objectName()));
    ui->agc->connect_property(model->cmd_property(ui->agc->objectName()));
    ui->autofocus->connect_property(model->cmd_property(ui->autofocus->objectName()));
    ui->field_sync->connect_property(model->cmd_property(ui->field_sync->objectName()));
    ui->field_sync_narrow->connect_property(model->cmd_property(ui->field_sync_narrow->objectName()));
    ui->field_sync_reg->connect_property(model->cmd_property(ui->field_sync_reg->objectName()));
    ui->zoom->connect_property(model->value_property(ui->zoom->objectName()));
    ui->focus->connect_property(model->value_property(ui->focus->objectName()));
    ui->autofocus_lens_en->connect_property(model->value_property(ui->autofocus_lens_en->objectName()));
    ui->autofocus_host_en->connect_property(model->value_property(ui->autofocus_host_en->objectName()));
    zoom_in_start->connect_property(model->cmd_property(zoom_in_start->objectName()));
    zoom_in_stop->connect_property(model->cmd_property(zoom_in_stop->objectName()));
    zoom_out_start->connect_property(model->cmd_property(zoom_out_start->objectName()));
    zoom_out_stop->connect_property(model->cmd_property(zoom_out_stop->objectName()));
    iris_in_start->connect_property(model->cmd_property(iris_in_start->objectName()));
    iris_in_stop->connect_property(model->cmd_property(iris_in_stop->objectName()));
    iris_out_start->connect_property(model->cmd_property(iris_out_start->objectName()));
    iris_out_stop->connect_property(model->cmd_property(iris_out_stop->objectName()));
    focus_near_start->connect_property(model->cmd_property(focus_near_start->objectName()));
    focus_near_stop->connect_property(model->cmd_property(focus_near_stop->objectName()));
    focus_far_start->connect_property(model->cmd_property(focus_far_start->objectName()));
    focus_far_stop->connect_property(model->cmd_property(focus_far_stop->objectName()));
}
void SWIRCtrlView::on_zoom_in_pressed()
{
    emit zoom_in_start->cmd();
}
void SWIRCtrlView::on_zoom_in_released()
{
    emit zoom_in_stop->cmd();
}
void SWIRCtrlView::on_zoom_out_pressed()
{
    emit zoom_out_start->cmd();
}
void SWIRCtrlView::on_zoom_out_released()
{
    emit zoom_out_stop->cmd();
}
void SWIRCtrlView::on_iris_in_pressed()
{
    emit iris_in_start->cmd();
}
void SWIRCtrlView::on_iris_in_released()
{
    emit iris_in_stop->cmd();
}
void SWIRCtrlView::on_iris_out_pressed()
{
    emit iris_out_start->cmd();
}
void SWIRCtrlView::on_iris_out_released()
{
    emit iris_out_stop->cmd();
}
void SWIRCtrlView::on_focus_near_pressed()
{
    emit focus_near_start->cmd();
}
void SWIRCtrlView::on_focus_near_released()
{
    emit focus_near_stop->cmd();
}
void SWIRCtrlView::on_focus_far_pressed()
{
    emit focus_far_start->cmd();
}
void SWIRCtrlView::on_focus_far_released()
{
    emit focus_far_stop->cmd();
}
SWIRCtrlView::~SWIRCtrlView()
{
    delete ui;
}
