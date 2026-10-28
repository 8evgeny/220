#include "mwirctrlview.h"
#include "ui_mwirctrlview.h"

#include <qmv/qmvmodel.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/valueproperty.h>

MWIRCtrlView::MWIRCtrlView(QWidget *parent) :
    QMVView(parent),
    ui(new Ui::MWIRCtrlView),
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

    // TAB1
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
    ui->sensor_status->set_label_width(170);
    ui->sensor_status->hide_set_btn();
    ui->sensor_status->hide_get_btn();
    ui->sensor_status->set_disabled();

    ui->lens_status->set_label_text("Статус объектива");
    ui->lens_status->set_label_width(180);
    ui->lens_status->hide_set_btn();
    ui->lens_status->hide_get_btn();
    ui->lens_status->set_disabled();

    ui->fps->set_label_text("Частота кадров[кадров/с]]");
    ui->fps->set_label_width(170);
    ui->fps->hide_set_btn();
    ui->fps->hide_get_btn();
    ui->fps->set_disabled();

    ui->vres->set_label_text("Вертикальное разрешение[пикс]");
    ui->vres->set_label_width(170);
    ui->vres->hide_set_btn();
    ui->vres->hide_get_btn();
    ui->vres->set_disabled();

    ui->hres->set_label_text("Горизонтальное разрешение[пикс]");
    ui->hres->set_label_width(180);
    ui->hres->hide_set_btn();
    ui->hres->hide_get_btn();
    ui->hres->set_disabled();

    ui->exp->set_label_text("Время экспонирования[мс]");
    ui->exp->set_label_width(170);
    ui->exp->hide_set_btn();
    ui->exp->hide_get_btn();
    ui->exp->set_disabled();

    ui->zoom_drive->set_label_text("Привод увеличения[разр.ацп]");
    ui->zoom_drive->set_label_width(170);
    ui->zoom_drive->hide_set_btn();
    ui->zoom_drive->hide_get_btn();
    ui->zoom_drive->set_disabled();

    ui->focus_drive->set_label_text("Привод фокусировки[разр.ацп]");
    ui->focus_drive->set_label_width(180);
    ui->focus_drive->hide_set_btn();
    ui->focus_drive->hide_get_btn();
    ui->focus_drive->set_disabled();

    ui->lens_temp->set_label_text("Температура объектива[град.Ц.]");
    ui->lens_temp->set_label_width(180);
    ui->lens_temp->hide_set_btn();
    ui->lens_temp->hide_get_btn();
    ui->lens_temp->set_disabled();

    ui->focus_mm->set_label_text("Температура сенсора[°С]");
    ui->focus_mm->set_label_width(170);
    ui->focus_mm->hide_set_btn();
    ui->focus_mm->hide_get_btn();
    ui->focus_mm->set_disabled();

    ui->field_grad->set_label_text("Угловое поле[град.]");
    ui->field_grad->set_label_width(180);
    ui->field_grad->hide_set_btn();
    ui->field_grad->hide_get_btn();
    ui->field_grad->set_disabled();

    ui->zoom->set_label_text("Привод увеличения[разр.ацп]");
    ui->zoom->set_label_width(170);
    ui->zoom->hide_get_btn();

    ui->focus->set_label_text("Привод фокусировки[разр.ацп]");
    ui->focus->set_label_width(170);
    ui->focus->hide_get_btn();

    ui->autofocus_lens_en->set_label_text("Разрешение автофокуса в объективе");
    ui->autofocus_lens_en->set_label_width(200);
    ui->autofocus_lens_en->hide_set_btn();
    ui->autofocus_lens_en->hide_get_btn();

    ui->autofocus_host_en->set_label_text("Разрешение автофокуса с хоста");
    ui->autofocus_host_en->set_label_width(200);
    ui->autofocus_host_en->hide_get_btn();
    ui->autofocus_host_en->hide_set_btn();
    ui->autofocus_host_en->set_disabled();

    ui->agc->set_cmd_text("Аппаратное АРУ");
    ui->inversion->set_cmd_text("Инверсия");
    ui->thermostat_on->set_cmd_text("Включить термостат объектива");
    ui->thermostat_off->set_cmd_text("Выключить термостат объектива");
    ui->heat_on->set_cmd_text("Включить обогрев объектива");
    ui->heat_off->set_cmd_text("Выключить обогрев объектива");
    ui->autofocus->set_cmd_text("Автофокус");
    // ui->field_sync->set_cmd_text("Синхронизация полей");
    // ui->field_sync_narrow->set_cmd_text("Синхронизация в узком поле");
    // ui->field_sync_reg->set_cmd_text("Регулярная синхронизация");

    // TAB2
    // System settings
    ui->sensor_ssettings_telemetry_enable->set_label_text("Разрешение телеметрии");
    ui->sensor_ssettings_telemetry_enable->set_label_width(100);
    ui->sensor_ssettings_telemetry_enable->hide_set_btn();
    ui->sensor_ssettings_telemetry_enable->hide_get_btn();

    ui->sensor_ssettings_soft_version->set_label_text("Версия");
    ui->sensor_ssettings_soft_version->set_label_width(50);
    ui->sensor_ssettings_soft_version->hide_get_btn();

    ui->sensor_ssettings_telemetry_address->set_label_text("Адрес телеметрии");
    ui->sensor_ssettings_telemetry_address->set_label_width(100);
    ui->sensor_ssettings_telemetry_address->hide_get_btn();

    // Matrix settings
    ui->sensor_msettings_cce_input->set_label_text("CCE input ctrl");
    ui->sensor_msettings_cce_input->set_label_width(70);
    ui->sensor_msettings_cce_input->hide_get_btn();

    ui->sensor_msettings_cce_min->set_label_text("CCE min (мин ур. сигнала с матрицы)");
    ui->sensor_msettings_cce_min->set_label_width(200);
    ui->sensor_msettings_cce_min->hide_get_btn();

    ui->sensor_msettings_cce_max->set_label_text("CCE max (макс ур. сигнала с матрицы)");
    ui->sensor_msettings_cce_max->set_label_width(200);
    ui->sensor_msettings_cce_max->hide_get_btn();

    ui->sensor_msettings_pattern->set_label_text("Pattern ctrl");
    ui->sensor_msettings_pattern->set_label_width(60);
    ui->sensor_msettings_pattern->hide_get_btn();

    ui->sensor_msettings_direction_video->set_label_text("Direction video");
    ui->sensor_msettings_direction_video->set_label_width(70);
    ui->sensor_msettings_direction_video->hide_get_btn();
    ui->sensor_msettings_direction_video->hide_set_btn();

    ui->sensor_msettings_black_white_video->set_label_text("Black white video");
    ui->sensor_msettings_black_white_video->set_label_width(70);
    ui->sensor_msettings_black_white_video->hide_get_btn();
    ui->sensor_msettings_black_white_video->hide_set_btn();

    ui->sensor_msettings_gpol->set_label_text("Gpol (напряжение смещения матрицы)");
    ui->sensor_msettings_gpol->set_label_width(195);
    ui->sensor_msettings_gpol->hide_get_btn();

    ui->sensor_msettings_period_time->set_label_text("Period time (период кадра)");
    ui->sensor_msettings_period_time->set_label_width(140);
    ui->sensor_msettings_period_time->hide_get_btn();

    ui->sensor_msettings_hl_0->set_label_text("hl 0");
    ui->sensor_msettings_hl_0->set_label_width(25);
    ui->sensor_msettings_hl_0->hide_get_btn();

    ui->sensor_msettings_hl_1->set_label_text("hl 1");
    ui->sensor_msettings_hl_1->set_label_width(25);
    ui->sensor_msettings_hl_1->hide_get_btn();

    ui->sensor_msettings_hl_2->set_label_text("hl 2");
    ui->sensor_msettings_hl_2->set_label_width(25);
    ui->sensor_msettings_hl_2->hide_get_btn();

    ui->sensor_msettings_hl_3->set_label_text("hl 3");
    ui->sensor_msettings_hl_3->set_label_width(25);
    ui->sensor_msettings_hl_3->hide_get_btn();

    ui->sensor_msettings_hl_4->set_label_text("hl 4");
    ui->sensor_msettings_hl_4->set_label_width(25);
    ui->sensor_msettings_hl_4->hide_get_btn();

    ui->sensor_msettings_serial_number->set_label_text("Sensor serial number");
    ui->sensor_msettings_serial_number->set_label_width(105);
    ui->sensor_msettings_serial_number->hide_get_btn();

    // DDE
    ui->sensor_dde_enable->set_label_text("DDE enable");
    ui->sensor_dde_enable->set_label_width(200);
    ui->sensor_dde_enable->hide_get_btn();
    ui->sensor_dde_enable->hide_set_btn();

    ui->sensor_dde_base_mode->set_label_text("Base mode");
    ui->sensor_dde_base_mode->set_label_width(55);
    ui->sensor_dde_base_mode->hide_get_btn();

    ui->sensor_dde_detail_gain->set_label_text("Detail gain");
    ui->sensor_dde_detail_gain->set_label_width(110);
    ui->sensor_dde_detail_gain->hide_get_btn();

    ui->sensor_dde_detail_eps_normal->set_label_text("Detail eps normal");
    ui->sensor_dde_detail_eps_normal->set_label_width(110);
    ui->sensor_dde_detail_eps_normal->hide_get_btn();

    ui->sensor_dde_hist_background->set_label_text("Hist background thrld");
    ui->sensor_dde_hist_background->set_label_width(110);
    ui->sensor_dde_hist_background->hide_get_btn();

    ui->sensor_dde_brightness->set_label_text("Brightness");
    ui->sensor_dde_brightness->set_label_width(55);
    ui->sensor_dde_brightness->hide_get_btn();

    ui->sensor_dde_alpha_address->set_label_text("Alpha addr");
    ui->sensor_dde_alpha_address->set_label_width(55);
    ui->sensor_dde_alpha_address->hide_get_btn();

    ui->sensor_dde_contrast->set_label_text("Contrast");
    ui->sensor_dde_contrast->set_label_width(55);
    ui->sensor_dde_contrast->hide_get_btn();

    ui->sensor_dde_inversion->set_label_text("Video inversion");
    ui->sensor_dde_inversion->set_label_width(110);
    ui->sensor_dde_inversion->hide_get_btn();

    // SNR
     ui->sensor_snr_enable->set_label_text("SNR enable");
     ui->sensor_snr_enable->set_label_width(200);
     ui->sensor_snr_enable->hide_get_btn();
     ui->sensor_snr_enable->hide_set_btn();

     ui->sensor_snr_col_eps->set_label_text("SNR row eps");
     ui->sensor_snr_col_eps->set_label_width(65);
     ui->sensor_snr_col_eps->hide_get_btn();

     ui->sensor_snr_row_eps->set_label_text("SNR col eps");
     ui->sensor_snr_row_eps->set_label_width(65);
     ui->sensor_snr_row_eps->hide_get_btn();

     // NUC
     ui->sensor_nuc_calibration_enable->set_label_text("Calibration enable");
     ui->sensor_nuc_calibration_enable->set_label_width(200);
     ui->sensor_nuc_calibration_enable->hide_get_btn();
     ui->sensor_nuc_calibration_enable->hide_set_btn();

     ui->sensor_nuc_bad_pixel_replace_enable->set_label_text("Bad pixel replace enable");
     ui->sensor_nuc_bad_pixel_replace_enable->set_label_width(200);
     ui->sensor_nuc_bad_pixel_replace_enable->hide_get_btn();
     ui->sensor_nuc_bad_pixel_replace_enable->hide_set_btn();

     ui->sensor_nuc_limit_level_out_data->set_label_text("Limit level out data");
     ui->sensor_nuc_limit_level_out_data->set_label_width(90);
     ui->sensor_nuc_limit_level_out_data->hide_get_btn();

     ui->sensor_nuc_lens_position->set_label_text("Lens Position");
     ui->sensor_nuc_lens_position->set_label_width(90);
     ui->sensor_nuc_lens_position->hide_get_btn();

     // Bad pixel
     ui->sensor_bad_pixel_rep_enable->set_label_text("BPR enable");
     ui->sensor_bad_pixel_rep_enable->set_label_width(200);
     ui->sensor_bad_pixel_rep_enable->hide_get_btn();
     ui->sensor_bad_pixel_rep_enable->hide_set_btn();

     ui->sensor_bad_pixel_rep_treshold->set_label_text("BPR threshold");
     ui->sensor_bad_pixel_rep_treshold->set_label_width(75);
     ui->sensor_bad_pixel_rep_treshold->hide_get_btn();

     // Telemetry
     ui->sensor_telemetry_enable->set_label_text("Telemetry enable");
     ui->sensor_telemetry_enable->set_label_width(200);
     ui->sensor_telemetry_enable->hide_get_btn();
     ui->sensor_telemetry_enable->hide_set_btn();
     ui->sensor_telemetry_enable->set_disabled();

     ui->sensor_telemetry_nuc_enable->set_label_text("NUC enable");
     ui->sensor_telemetry_nuc_enable->set_label_width(200);
     ui->sensor_telemetry_nuc_enable->hide_get_btn();
     ui->sensor_telemetry_nuc_enable->hide_set_btn();
     ui->sensor_telemetry_nuc_enable->set_disabled();

     ui->sensor_telemetry_snr_enable->set_label_text("SNR enable");
     ui->sensor_telemetry_snr_enable->set_label_width(200);
     ui->sensor_telemetry_snr_enable->hide_get_btn();
     ui->sensor_telemetry_snr_enable->hide_set_btn();
     ui->sensor_telemetry_snr_enable->set_disabled();

     ui->sensor_telemetry_dde_enable->set_label_text("DDE enable");
     ui->sensor_telemetry_dde_enable->set_label_width(200);
     ui->sensor_telemetry_dde_enable->hide_get_btn();
     ui->sensor_telemetry_dde_enable->hide_set_btn();
     ui->sensor_telemetry_dde_enable->set_disabled();

     ui->sensor_telemetry_bpr_enable->set_label_text("BPR enable");
     ui->sensor_telemetry_bpr_enable->set_label_width(200);
     ui->sensor_telemetry_bpr_enable->hide_get_btn();
     ui->sensor_telemetry_bpr_enable->hide_set_btn();
     ui->sensor_telemetry_bpr_enable->set_disabled();

     ui->sensor_telemetry_temperature_lens->set_label_text("Temperature lens");
     ui->sensor_telemetry_temperature_lens->set_label_width(100);
     ui->sensor_telemetry_temperature_lens->hide_get_btn();
     ui->sensor_telemetry_temperature_lens->hide_set_btn();
     ui->sensor_telemetry_temperature_lens->set_disabled();

     ui->sensor_telemetry_temperature_matrix->set_label_text("Temperature matrix");
     ui->sensor_telemetry_temperature_matrix->set_label_width(100);
     ui->sensor_telemetry_temperature_matrix->hide_get_btn();
     ui->sensor_telemetry_temperature_matrix->hide_set_btn();
     ui->sensor_telemetry_temperature_matrix->set_disabled();

     ui->sensor_ssettings_save_to_flash->set_cmd_text(QString::fromUtf8("Save all options to flash"));
     ui->sensor_ssettings_save_to_flash->set_cmd_icon(QString::fromUtf8(":/img/save.png"), 20, 20);

     ui->sensor_ssettings_load_from_flash->set_cmd_text(QString::fromUtf8("Load all options from flash"));
     ui->sensor_ssettings_load_from_flash->set_cmd_icon(QString::fromUtf8(":/img/load.png"), 20, 20);

     ui->sensor_ssettings_initialization_value->set_cmd_text(QString::fromUtf8("Initializations default value"));
     ui->sensor_ssettings_initialization_value->set_cmd_icon(QString::fromUtf8(":/img/undo.png"), 20, 20);
}
void MWIRCtrlView::connect_model()
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
    ui->inversion->connect_property(model->cmd_property(ui->inversion->objectName()));
    ui->autofocus->connect_property(model->cmd_property(ui->autofocus->objectName()));
    // ui->field_sync->connect_property(model->cmd_property(ui->field_sync->objectName()));
    // ui->field_sync_narrow->connect_property(model->cmd_property(ui->field_sync_narrow->objectName()));
    // ui->field_sync_reg->connect_property(model->cmd_property(ui->field_sync_reg->objectName()));
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

    ui->sensor_ssettings_telemetry_enable->connect_property(model->value_property(ui->sensor_ssettings_telemetry_enable->objectName()));
    ui->sensor_ssettings_soft_version->connect_property(model->value_property(ui->sensor_ssettings_soft_version->objectName()));
    ui->sensor_ssettings_telemetry_address->connect_property(model->value_property(ui->sensor_ssettings_telemetry_address->objectName()));
    ui->sensor_msettings_cce_input->connect_property(model->value_property(ui->sensor_msettings_cce_input->objectName()));
    ui->sensor_msettings_cce_min->connect_property(model->value_property(ui->sensor_msettings_cce_min->objectName()));
    ui->sensor_msettings_cce_max->connect_property(model->value_property(ui->sensor_msettings_cce_max->objectName()));
    ui->sensor_msettings_pattern->connect_property(model->value_property(ui->sensor_msettings_pattern->objectName()));
    ui->sensor_msettings_direction_video->connect_property(model->value_property(ui->sensor_msettings_direction_video->objectName()));
    ui->sensor_msettings_black_white_video->connect_property(model->value_property(ui->sensor_msettings_black_white_video->objectName()));
    ui->sensor_msettings_gpol->connect_property(model->value_property(ui->sensor_msettings_gpol->objectName()));
    ui->sensor_msettings_period_time->connect_property(model->value_property(ui->sensor_msettings_period_time->objectName()));
    ui->sensor_msettings_hl_0->connect_property(model->value_property(ui->sensor_msettings_hl_0->objectName()));
    ui->sensor_msettings_hl_1->connect_property(model->value_property(ui->sensor_msettings_hl_1->objectName()));
    ui->sensor_msettings_hl_2->connect_property(model->value_property(ui->sensor_msettings_hl_2->objectName()));
    ui->sensor_msettings_hl_3->connect_property(model->value_property(ui->sensor_msettings_hl_3->objectName()));
    ui->sensor_msettings_hl_4->connect_property(model->value_property(ui->sensor_msettings_hl_4->objectName()));
    ui->sensor_msettings_serial_number->connect_property(model->value_property(ui->sensor_msettings_serial_number->objectName()));
    ui->sensor_dde_enable->connect_property(model->value_property(ui->sensor_dde_enable->objectName()));
    ui->sensor_dde_base_mode->connect_property(model->value_property(ui->sensor_dde_base_mode->objectName()));
    ui->sensor_dde_detail_gain->connect_property(model->value_property(ui->sensor_dde_detail_gain->objectName()));
    ui->sensor_dde_detail_eps_normal->connect_property(model->value_property(ui->sensor_dde_detail_eps_normal->objectName()));
    ui->sensor_dde_hist_background->connect_property(model->value_property(ui->sensor_dde_hist_background->objectName()));
    ui->sensor_dde_brightness->connect_property(model->value_property(ui->sensor_dde_brightness->objectName()));
    ui->sensor_dde_alpha_address->connect_property(model->value_property(ui->sensor_dde_alpha_address->objectName()));
    ui->sensor_dde_contrast->connect_property(model->value_property(ui->sensor_dde_contrast->objectName()));
    ui->sensor_dde_inversion->connect_property(model->value_property(ui->sensor_dde_inversion->objectName()));
    ui->sensor_snr_enable->connect_property(model->value_property(ui->sensor_snr_enable->objectName()));
    ui->sensor_snr_col_eps->connect_property(model->value_property(ui->sensor_snr_col_eps->objectName()));
    ui->sensor_snr_row_eps->connect_property(model->value_property(ui->sensor_snr_row_eps->objectName()));
    ui->sensor_nuc_calibration_enable->connect_property(model->value_property(ui->sensor_nuc_calibration_enable->objectName()));
    ui->sensor_nuc_bad_pixel_replace_enable->connect_property(model->value_property(ui->sensor_nuc_bad_pixel_replace_enable->objectName()));
    ui->sensor_nuc_limit_level_out_data->connect_property(model->value_property(ui->sensor_nuc_limit_level_out_data->objectName()));
    ui->sensor_nuc_lens_position->connect_property(model->value_property(ui->sensor_nuc_lens_position->objectName()));
    ui->sensor_bad_pixel_rep_enable->connect_property(model->value_property(ui->sensor_bad_pixel_rep_enable->objectName()));
    ui->sensor_bad_pixel_rep_treshold->connect_property(model->value_property(ui->sensor_bad_pixel_rep_treshold->objectName()));
    ui->sensor_telemetry_enable->connect_property(model->value_property(ui->sensor_telemetry_enable->objectName()));
    ui->sensor_telemetry_nuc_enable->connect_property(model->value_property(ui->sensor_telemetry_nuc_enable->objectName()));
    ui->sensor_telemetry_snr_enable->connect_property(model->value_property(ui->sensor_telemetry_snr_enable->objectName()));
    ui->sensor_telemetry_dde_enable->connect_property(model->value_property(ui->sensor_telemetry_dde_enable->objectName()));
    ui->sensor_telemetry_bpr_enable->connect_property(model->value_property(ui->sensor_telemetry_bpr_enable->objectName()));
    ui->sensor_telemetry_temperature_lens->connect_property(model->value_property(ui->sensor_telemetry_temperature_lens->objectName()));
    ui->sensor_telemetry_temperature_matrix->connect_property(model->value_property(ui->sensor_telemetry_temperature_matrix->objectName()));

    ui->sensor_ssettings_save_to_flash->connect_property(model->cmd_property(ui->sensor_ssettings_save_to_flash->objectName()));
    ui->sensor_ssettings_load_from_flash->connect_property(model->cmd_property(ui->sensor_ssettings_load_from_flash->objectName()));
    ui->sensor_ssettings_initialization_value->connect_property(model->cmd_property(ui->sensor_ssettings_initialization_value->objectName()));

    model->update_view();
}
void MWIRCtrlView::on_zoom_in_pressed()
{
    emit zoom_in_start->cmd();
}
void MWIRCtrlView::on_zoom_in_released()
{
    emit zoom_in_stop->cmd();
}
void MWIRCtrlView::on_zoom_out_pressed()
{
    emit zoom_out_start->cmd();
}
void MWIRCtrlView::on_zoom_out_released()
{
    emit zoom_out_stop->cmd();
}
void MWIRCtrlView::on_iris_in_pressed()
{
    emit iris_in_start->cmd();
}
void MWIRCtrlView::on_iris_in_released()
{
    emit iris_in_stop->cmd();
}
void MWIRCtrlView::on_iris_out_pressed()
{
    emit iris_out_start->cmd();
}
void MWIRCtrlView::on_iris_out_released()
{
    emit iris_out_stop->cmd();
}
void MWIRCtrlView::on_focus_near_pressed()
{
    emit focus_near_start->cmd();
}
void MWIRCtrlView::on_focus_near_released()
{
    emit focus_near_stop->cmd();
}
void MWIRCtrlView::on_focus_far_pressed()
{
    emit focus_far_start->cmd();
}
void MWIRCtrlView::on_focus_far_released()
{
    emit focus_far_stop->cmd();
}
MWIRCtrlView::~MWIRCtrlView()
{
    delete ui;
}

void MWIRCtrlView::on_sensor_ssettings_save_to_flash_clicked()
{

}


void MWIRCtrlView::on_sensor_ssettings_load_from_flash_clicked()
{

}


void MWIRCtrlView::on_sensor_ssettings_initialization_value_clicked()
{

}

