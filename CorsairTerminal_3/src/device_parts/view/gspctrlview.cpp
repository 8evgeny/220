#include "gspctrlview.h"
#include "ui_gspctrlview.h"

#include <qmv/qmvmodel.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/valueproperty.h>

#include <controller.h>
#include "device_parts/cmd/cmdmanager.h"

GSPCtrlView::GSPCtrlView(QWidget *parent) :
    QMVView(parent),
    ui(new Ui::GSPCtrlView),
    up_start(new InoutCmdWidget(this)),
    down_start(new InoutCmdWidget(this)),
    left_start(new InoutCmdWidget(this)),
    right_start(new InoutCmdWidget(this)),
    stop(new InoutCmdWidget(this))
{
    ui->setupUi(this);
    up_start->setObjectName("up_start");
    down_start->setObjectName("down_start");
    left_start->setObjectName("left_start");
    right_start->setObjectName("right_start");
    stop->setObjectName("stop");
    up_start->setVisible(false);
    down_start->setVisible(false);
    left_start->setVisible(false);
    right_start->setVisible(false);
    stop->setVisible(false);

    ui->status->set_label_text("Статус");
    ui->status->set_label_width(110);
    ui->status->hide_set_btn();
    ui->status->hide_get_btn();
    ui->status->set_disabled();

    ui->st_mechanical_locking->set_label_text("Мех. рет.");
    ui->st_mechanical_locking->set_label_width(100);
    ui->st_mechanical_locking->hide_set_btn();
    ui->st_mechanical_locking->hide_get_btn();
    ui->st_mechanical_locking->setDisabled(true);

    ui->st_electronic_locking->set_label_text("Эл. рет.");
    ui->st_electronic_locking->set_label_width(100);
    ui->st_electronic_locking->hide_set_btn();
    ui->st_electronic_locking->hide_get_btn();
    ui->st_electronic_locking->setDisabled(true);

    ui->st_free_locking->set_label_text("Свобод.");
    ui->st_free_locking->set_label_width(100);
    ui->st_free_locking->hide_set_btn();
    ui->st_free_locking->hide_get_btn();
    ui->st_free_locking->setDisabled(true);

    ui->st_gyrostabilization->set_label_text("Стаб.");
    ui->st_gyrostabilization->set_label_width(100);
    ui->st_gyrostabilization->hide_set_btn();
    ui->st_gyrostabilization->hide_get_btn();
    ui->st_gyrostabilization->setDisabled(true);

    ui->st_zero->set_label_text("Ноль");
    ui->st_zero->set_label_width(100);
    ui->st_zero->hide_set_btn();
    ui->st_zero->hide_get_btn();
    ui->st_zero->setDisabled(true);

    ui->st_hike->set_label_text("Поход");
    ui->st_hike->set_label_width(100);
    ui->st_hike->hide_set_btn();
    ui->st_hike->hide_get_btn();
    ui->st_hike->setDisabled(true);

    ui->st_pilot->set_label_text("Пилот");
    ui->st_pilot->set_label_width(100);
    ui->st_pilot->hide_set_btn();
    ui->st_pilot->hide_get_btn();
    ui->st_pilot->setDisabled(true);

    ui->st_angle_control->set_label_text("Упр. по углу");
    ui->st_angle_control->set_label_width(100);
    ui->st_angle_control->hide_set_btn();
    ui->st_angle_control->hide_get_btn();
    ui->st_angle_control->setDisabled(true);

    ui->st_park->set_label_text("Парковка");
    ui->st_park->set_label_width(100);
    ui->st_park->hide_set_btn();
    ui->st_park->hide_get_btn();
    ui->st_park->setDisabled(true);

    ui->mode->set_label_text("Режим");
    ui->mode->set_label_width(110);
    ui->mode->hide_set_btn();
    ui->mode->hide_get_btn();
    ui->mode->set_disabled();

    ui->x_drive_pos->set_label_text("Привод X");
    ui->x_drive_pos->set_label_width(110);
    ui->x_drive_pos->hide_set_btn();
    ui->x_drive_pos->hide_get_btn();
    ui->x_drive_pos->set_disabled();

    ui->x_angle->set_label_text("Угол X");
    ui->x_angle->set_label_width(110);
    ui->x_angle->hide_set_btn();
    ui->x_angle->hide_get_btn();
    ui->x_angle->set_disabled();

    ui->x_speed->set_label_text("Скорость X");
    ui->x_speed->set_label_width(110);
    ui->x_speed->hide_set_btn();
    ui->x_speed->hide_get_btn();
    ui->x_speed->set_disabled();

    ui->z_drive_pos->set_label_text("Привод Z");
    ui->z_drive_pos->set_label_width(110);
    ui->z_drive_pos->hide_set_btn();
    ui->z_drive_pos->hide_get_btn();
    ui->z_drive_pos->set_disabled();

    ui->z_angle->set_label_text("Угол Z");
    ui->z_angle->set_label_width(110);
    ui->z_angle->hide_set_btn();
    ui->z_angle->hide_get_btn();
    ui->z_angle->set_disabled();

    ui->z_speed->set_label_text("Скорость Z");
    ui->z_speed->set_label_width(110);
    ui->z_speed->hide_set_btn();
    ui->z_speed->hide_get_btn();
    ui->z_speed->set_disabled();

    ui->alignment_pos_y->set_label_text("Ноль по оси X");
    ui->alignment_pos_y->set_label_width(110);
    ui->alignment_pos_y->hide_set_btn();
    ui->alignment_pos_y->hide_get_btn();
    ui->alignment_pos_y->set_disabled();

    ui->alignment_pos_z->set_label_text("Ноль по оси Z");
    ui->alignment_pos_z->set_label_width(110);
    ui->alignment_pos_z->hide_set_btn();
    ui->alignment_pos_z->hide_get_btn();
    ui->alignment_pos_z->set_disabled();

    // ui->drift_x->set_label_text("Компенсация дрейфа оси X");
    // ui->drift_x->set_label_width(250);
    // ui->drift_x->hide_get_btn();
    // ui->drift_x->set_slider(-1000, 1000, 1, 10);

    // ui->drift_z->set_label_text("Компенсация дрейфа оси Z");
    // ui->drift_z->set_label_width(250);
    // ui->drift_z->hide_get_btn();
    // ui->drift_z->set_slider(-1000, 1000, 1, 10);

    ui->angle_enable->set_label_text("Управление по углу");
    ui->angle_enable->set_label_width(80);
    ui->angle_enable->hide_set_btn();
    ui->angle_enable->hide_get_btn();

    ui->x_angle_set->set_label_text("Угол X");
    ui->x_angle_set->set_label_width(60);
    ui->x_angle_set->hide_get_btn();

    ui->z_angle_set->set_label_text("Угол Z");
    ui->z_angle_set->set_label_width(60);
    ui->z_angle_set->hide_get_btn();

    ui->x_speed_f_set->set_label_text("Скорость X");
    ui->x_speed_f_set->set_label_width(60);
    ui->x_speed_f_set->hide_get_btn();

    ui->z_speed_f_set->set_label_text("Скорость Z");
    ui->z_speed_f_set->set_label_width(60);
    ui->z_speed_f_set->hide_get_btn();

    ui->angle_discrepancy_x->set_label_text("Невязка X");
    ui->angle_discrepancy_x->set_label_width(60);
    ui->angle_discrepancy_x->hide_get_btn();

    ui->angle_discrepancy_z->set_label_text("Невязка Z");
    ui->angle_discrepancy_z->set_label_width(60);
    ui->angle_discrepancy_z->hide_get_btn();

    ui->angle_abs->set_cmd_text("Абсолютный угол");
    ui->angle_rel->set_cmd_text("Относительный угол");

    ui->speed_f_set ->set_cmd_text("Задать скорость");
    ui->speed_f_stop->set_cmd_text("Стоп");


    ui->arretir_reset->set_cmd_text("Разарретировать");
    ui->stabilization->set_cmd_text("Стабилизация");
    ui->park->set_cmd_text("Парковка");
    ui->arretir_set->set_cmd_text("Уст-ть арретиры");

    ui->null_position->set_cmd_text("НОЛЬ");
    ui->pohod_position->set_cmd_text("ПОХОД");
    ui->polyot_position->set_cmd_text("ПОЛЕТ");

    ui->alignment->set_cmd_text("Установить ноль платформы");

    ui->x_arretir_off     ->set_cmd_text("Убрать X uart");
    ui->z_arretir_off     ->set_cmd_text("Убрать Z uart");
    ui->all_arretir_off   ->set_cmd_text("Убрать все uart");
    ui->x_arretir_on      ->set_cmd_text("Ус-ть X uart");
    ui->z_arretir_on      ->set_cmd_text("Ус-ть Z uart");
    ui->all_arretir_on    ->set_cmd_text("Ус-ть все uart");
    ui->fine_arretir_reset->set_cmd_text("Убрать точный арретир");
    ui->fine_arretir_set  ->set_cmd_text("Уст-ть точный арретир");

    //-------------Настройка ПИД регулятора АС---------------------/
    ui->x_Kp->set_label_text("Kp");
    ui->x_Kp->set_label_width(100);

    ui->x_Ki->set_label_text("Ki");
    ui->x_Ki->set_label_width(100);

    ui->x_Kd->set_label_text("Kd");
    ui->x_Kd->set_label_width(100);

    ui->x_integral_limit->set_label_text("integral_limit");
    ui->x_integral_limit->set_label_width(100);

    ui->z_Kp->set_label_text("Kp");
    ui->z_Kp->set_label_width(100);

    ui->z_Ki->set_label_text("Ki");
    ui->z_Ki->set_label_width(100);

    ui->z_Kd->set_label_text("Kd");
    ui->z_Kd->set_label_width(100);

    ui->z_integral_limit->set_label_text("integral_limit");
    ui->z_integral_limit->set_label_width(100);

    ui->pid_out_limit_z->set_label_text("pid_out_limit_z");
    ui->pid_out_limit_z->set_label_width(110);

    ui->pid_out_limit_x->set_label_text("pid_out_limit_x");
    ui->pid_out_limit_x->set_label_width(110);

    ui->target_filter_k_x->set_label_text("Фильтр X");
    ui->target_filter_k_x->set_label_width(110);

    ui->target_filter_k_y->set_label_text("Фильтр Y");
    ui->target_filter_k_y->set_label_width(110);

    // Управление фильтром координат цели
    ui->target_filter_ctrl->set_cmd_text("Фильтр цели");
    ui->target_filter_ctrl->set_cmd_width(100);
    ui->target_filter_ctrl->set_disabled();

    // Управление фильтром угла с ДУ
    ui->angle_filter_ctrl->set_cmd_text("Фильтр угла");
    ui->angle_filter_ctrl->set_cmd_width(100);
    ui->angle_filter_ctrl->set_disabled();

    ui->tr_type_no_filter->set_label_text("Без фильтра");
    ui->tr_type_no_filter->set_label_width(100);
    ui->tr_type_no_filter->hide_get_btn();
    ui->tr_type_no_filter->hide_set_btn();

    ui->tr_type_exp_filter->set_label_text("Эксп-ный фильтр");
    ui->tr_type_exp_filter->set_label_width(100);
    ui->tr_type_exp_filter->hide_get_btn();
    ui->tr_type_exp_filter->hide_set_btn();

    ui->tr_type_kalman_filter->set_label_text("Фильтр Калмана");
    ui->tr_type_kalman_filter->set_label_width(100);
    ui->tr_type_kalman_filter->hide_get_btn();
    ui->tr_type_kalman_filter->hide_set_btn();

    ui->tr_speed_limit_en->set_label_text("Ограничение скорости");
    ui->tr_speed_limit_en->set_label_width(100);
    ui->tr_speed_limit_en->hide_get_btn();
    ui->tr_speed_limit_en->hide_set_btn();

    ui->tr_type_lsm_filter->set_label_text("МНК");
    ui->tr_type_lsm_filter->set_label_width(100);
    ui->tr_type_lsm_filter->hide_get_btn();
    ui->tr_type_lsm_filter->hide_set_btn();

    // Сохранение нового углового положения
    ui->new_angle_x->set_label_text("angle_x");
    ui->new_angle_x->set_label_width(100);
    ui->new_angle_x->hide_get_btn();

    ui->new_angle_z->set_label_text("angle_z");
    ui->new_angle_z->set_label_width(100);
    ui->new_angle_z->hide_get_btn();

    ui->set_new_angle_x->set_cmd_text("set angle X");
    ui->set_new_angle_x->set_cmd_width(50);

    ui->set_new_angle_z->set_cmd_text("set angle Z");
    ui->set_new_angle_z->set_cmd_width(50);

    ui->set_new_angles->set_cmd_text("set angle X and Z");
    ui->set_new_angles->set_cmd_width(100);

    // Измерение скорости
    ui->measure_speed->set_label_text("Измерение скорости");
    ui->measure_speed->set_label_width(100);
    ui->measure_speed->hide_get_btn();
    ui->measure_speed->hide_set_btn();

    ui->measure_speed_n->set_label_text("N");
    ui->measure_speed_n->set_label_width(100);
    ui->measure_speed_n->hide_get_btn();
    ui->measure_speed_n->hide_set_btn();
    ui->measure_speed_n->setDisabled(true);

    ui->measure_speed_n0->set_label_text("N0");
    ui->measure_speed_n0->set_label_width(100);
    ui->measure_speed_n0->hide_get_btn();
    ui->measure_speed_n0->hide_set_btn();
    ui->measure_speed_n0->setDisabled(true);

    ui->measure_speed_nt->set_label_text("N1");
    ui->measure_speed_nt->set_label_width(100);
    ui->measure_speed_nt->hide_get_btn();
    ui->measure_speed_nt->hide_set_btn();
    ui->measure_speed_nt->setDisabled(true);

    ui->measure_speed_angle0->set_label_text("Угол 0, град");
    ui->measure_speed_angle0->set_label_width(100);
    ui->measure_speed_angle0->hide_get_btn();
    ui->measure_speed_angle0->hide_set_btn();
    ui->measure_speed_angle0->setDisabled(true);

    ui->measure_speed_anglet->set_label_text("Angle 1, град");
    ui->measure_speed_anglet->set_label_width(100);
    ui->measure_speed_anglet->hide_get_btn();
    ui->measure_speed_anglet->hide_set_btn();
    ui->measure_speed_anglet->setDisabled(true);

    ui->x_speed_set->set_label_text("Скорость X, град/с");
    ui->x_speed_set->set_label_width(135);
    ui->x_speed_set->hide_get_btn();

    ui->z_speed_set->set_label_text("Скорость Z, град/с");
    ui->z_speed_set->set_label_width(135);
    ui->z_speed_set->hide_get_btn();

    ui->speed_calc->set_cmd_text("Рассчитать");
    ui->speed_set->set_cmd_text("Задать скорость");

    // Сырой статус
    ui->bits->hide_set_btn();
    ui->bits->hide_get_btn();
    ui->bits->set_disabled();

    ui->mech_arr_x->set_label_text("mech_arr_x");
    ui->mech_arr_x->set_label_width(100);
    ui->mech_arr_x->hide_get_btn();
    ui->mech_arr_x->hide_set_btn();
    ui->mech_arr_x->setDisabled(true);

    ui->mech_arr_z->set_label_text("mech_arr_z");
    ui->mech_arr_z->set_label_width(100);
    ui->mech_arr_z->hide_get_btn();
    ui->mech_arr_z->hide_set_btn();
    ui->mech_arr_z->setDisabled(true);

    ui->mech_arr_f->set_label_text("mech_arr_f");
    ui->mech_arr_f->set_label_width(100);
    ui->mech_arr_f->hide_get_btn();
    ui->mech_arr_f->hide_set_btn();
    ui->mech_arr_f->setDisabled(true);

    ui->stab_x->set_label_text("stab_x");
    ui->stab_x->set_label_width(100);
    ui->stab_x->hide_get_btn();
    ui->stab_x->hide_set_btn();
    ui->stab_x->setDisabled(true);

    ui->stab_z->set_label_text("stab_z");
    ui->stab_z->set_label_width(100);
    ui->stab_z->hide_get_btn();
    ui->stab_z->hide_set_btn();
    ui->stab_z->setDisabled(true);

    ui->park_x->set_label_text("park_x");
    ui->park_x->set_label_width(100);
    ui->park_x->hide_get_btn();
    ui->park_x->hide_set_btn();
    ui->park_x->setDisabled(true);

    ui->park_z->set_label_text("park_z");
    ui->park_z->set_label_width(100);
    ui->park_z->hide_get_btn();
    ui->park_z->hide_set_btn();
    ui->park_z->setDisabled(true);

    ui->zero_x->set_label_text("zero_x");
    ui->zero_x->set_label_width(100);
    ui->zero_x->hide_get_btn();
    ui->zero_x->hide_set_btn();
    ui->zero_x->setDisabled(true);

    ui->zero_z->set_label_text("zero_z");
    ui->zero_z->set_label_width(100);
    ui->zero_z->hide_get_btn();
    ui->zero_z->hide_set_btn();
    ui->zero_z->setDisabled(true);

    ui->el_arr_x->set_label_text("el_arr_x");
    ui->el_arr_x->set_label_width(100);
    ui->el_arr_x->hide_get_btn();
    ui->el_arr_x->hide_set_btn();
    ui->el_arr_x->setDisabled(true);

    ui->el_arr_z->set_label_text("el_arr_z");
    ui->el_arr_z->set_label_width(100);
    ui->el_arr_z->hide_get_btn();
    ui->el_arr_z->hide_set_btn();
    ui->el_arr_z->setDisabled(true);

    ui->zero_pos->set_label_text("zero_pos");
    ui->zero_pos->set_label_width(100);
    ui->zero_pos->hide_get_btn();
    ui->zero_pos->hide_set_btn();
    ui->zero_pos->setDisabled(true);

    ui->pilot_pos->set_label_text("pilot_pos");
    ui->pilot_pos->set_label_width(100);
    ui->pilot_pos->hide_get_btn();
    ui->pilot_pos->hide_set_btn();
    ui->pilot_pos->setDisabled(true);

    ui->pohod_pos->set_label_text("pohod_pos");
    ui->pohod_pos->set_label_width(100);
    ui->pohod_pos->hide_get_btn();
    ui->pohod_pos->hide_set_btn();
    ui->pohod_pos->setDisabled(true);

    ui->can_error->set_label_text("can_error");
    ui->can_error->set_label_width(100);
    ui->can_error->hide_get_btn();
    ui->can_error->hide_set_btn();
    ui->can_error->setDisabled(true);

    ui->adc_bit->set_label_text("adc_bit");
    ui->adc_bit->set_label_width(100);
    ui->adc_bit->hide_get_btn();
    ui->adc_bit->hide_set_btn();
    ui->adc_bit->setDisabled(true);

    ui->angle_is_set_x->set_label_text("angle_is_set_x");
    ui->angle_is_set_x->set_label_width(100);
    ui->angle_is_set_x->hide_get_btn();
    ui->angle_is_set_x->hide_set_btn();
    ui->angle_is_set_x->setDisabled(true);

    ui->angle_is_set_z->set_label_text("angle_is_set_z");
    ui->angle_is_set_z->set_label_width(100);
    ui->angle_is_set_z->hide_get_btn();
    ui->angle_is_set_z->hide_set_btn();
    ui->angle_is_set_z->setDisabled(true);

    ui->diagnostics->set_label_text("diagnostics");
    ui->diagnostics->set_label_width(100);
    ui->diagnostics->hide_get_btn();
    ui->diagnostics->hide_set_btn();
    ui->diagnostics->setDisabled(true);

    ui->drift_en->set_label_text("drift_en");
    ui->drift_en->set_label_width(100);
    ui->drift_en->hide_get_btn();
    ui->drift_en->hide_set_btn();
    ui->drift_en->setDisabled(true);

    ui->drift_mode->set_label_text("drift_mode");
    ui->drift_mode->set_label_width(100);
    ui->drift_mode->hide_get_btn();
    ui->drift_mode->hide_set_btn();
    ui->drift_mode->setDisabled(true);

    ui->ctrl_mode->set_label_text("ctrl_mode");
    ui->ctrl_mode->set_label_width(100);
    ui->ctrl_mode->hide_get_btn();
    ui->ctrl_mode->hide_set_btn();
    ui->ctrl_mode->setDisabled(true);

    ui->diagnostic_res_z->set_label_text("diag_res_z");
    ui->diagnostic_res_z->set_label_width(100);
    ui->diagnostic_res_z->hide_get_btn();
    ui->diagnostic_res_z->hide_set_btn();
    ui->diagnostic_res_z->setDisabled(true);

    ui->diagnostic_res_x->set_label_text("diag_res_x");
    ui->diagnostic_res_x->set_label_width(100);
    ui->diagnostic_res_x->hide_get_btn();
    ui->diagnostic_res_x->hide_set_btn();
    ui->diagnostic_res_x->setDisabled(true);

    ui->fsm_mode_z->set_label_text("fsm_mode_z");
    ui->fsm_mode_z->set_label_width(100);
    ui->fsm_mode_z->hide_get_btn();
    ui->fsm_mode_z->hide_set_btn();
    ui->fsm_mode_z->setDisabled(true);

    ui->fsm_mode_x->set_label_text("fsm_mode_x");
    ui->fsm_mode_x->set_label_width(100);
    ui->fsm_mode_x->hide_get_btn();
    ui->fsm_mode_x->hide_set_btn();
    ui->fsm_mode_x->setDisabled(true);

    ui->park_f_x->set_label_text("park_f_x");
    ui->park_f_x->set_label_width(100);
    ui->park_f_x->hide_get_btn();
    ui->park_f_x->hide_set_btn();
    ui->park_f_x->setDisabled(true);

    ui->park_f_z->set_label_text("park_f_z");
    ui->park_f_z->set_label_width(100);
    ui->park_f_z->hide_get_btn();
    ui->park_f_z->hide_set_btn();
    ui->park_f_z->setDisabled(true);

    // Самодиагностика
    ui->bits_selftest->hide_set_btn();
    ui->bits_selftest->hide_get_btn();
    ui->bits_selftest->set_disabled();

    ui->timeout_err_x->set_label_text("timeout_err");
    ui->timeout_err_x->set_label_width(100);
    ui->timeout_err_x->hide_get_btn();
    ui->timeout_err_x->hide_set_btn();
    ui->timeout_err_x->setDisabled(true);

    ui->el_arr_err_x->set_label_text("el_arr_err");
    ui->el_arr_err_x->set_label_width(100);
    ui->el_arr_err_x->hide_get_btn();
    ui->el_arr_err_x->hide_set_btn();
    ui->el_arr_err_x->setDisabled(true);

    ui->stab_err_x->set_label_text("stab_err");
    ui->stab_err_x->set_label_width(100);
    ui->stab_err_x->hide_get_btn();
    ui->stab_err_x->hide_set_btn();
    ui->stab_err_x->setDisabled(true);

    ui->el_arr_zero_err_x->set_label_text("el_arr_zero_err");
    ui->el_arr_zero_err_x->set_label_width(100);
    ui->el_arr_zero_err_x->hide_get_btn();
    ui->el_arr_zero_err_x->hide_set_btn();
    ui->el_arr_zero_err_x->setDisabled(true);

    ui->stab_angle_err_x->set_label_text("stab_angle_err");
    ui->stab_angle_err_x->set_label_width(100);
    ui->stab_angle_err_x->hide_get_btn();
    ui->stab_angle_err_x->hide_set_btn();
    ui->stab_angle_err_x->setDisabled(true);

    ui->timeout_err_z->set_label_text("timeout_err");
    ui->timeout_err_z->set_label_width(100);
    ui->timeout_err_z->hide_get_btn();
    ui->timeout_err_z->hide_set_btn();
    ui->timeout_err_z->setDisabled(true);

    ui->el_arr_err_z->set_label_text("el_arr_err");
    ui->el_arr_err_z->set_label_width(100);
    ui->el_arr_err_z->hide_get_btn();
    ui->el_arr_err_z->hide_set_btn();
    ui->el_arr_err_z->setDisabled(true);

    ui->stab_err_z->set_label_text("stab_err");
    ui->stab_err_z->set_label_width(100);
    ui->stab_err_z->hide_get_btn();
    ui->stab_err_z->hide_set_btn();
    ui->stab_err_z->setDisabled(true);

    ui->el_arr_zero_err_z->set_label_text("el_arr_zero_err");
    ui->el_arr_zero_err_z->set_label_width(100);
    ui->el_arr_zero_err_z->hide_get_btn();
    ui->el_arr_zero_err_z->hide_set_btn();
    ui->el_arr_zero_err_z->setDisabled(true);

    ui->stab_angle_err_z->set_label_text("stab_angle_err");
    ui->stab_angle_err_z->set_label_width(100);
    ui->stab_angle_err_z->hide_get_btn();
    ui->stab_angle_err_z->hide_set_btn();
    ui->stab_angle_err_z->setDisabled(true);


    connect(ui->pbGyCal_calibrate, &QPushButton::clicked, this, &GSPCtrlView::gyCal_calibrate);
    connect(ui->pbGyCal_auto, &QPushButton::clicked, this, &GSPCtrlView::gyCal_auto);

}
void GSPCtrlView::connect_model()
{
    ui->status->connect_property(model->value_property(ui->status->objectName()));
    ui->mode->connect_property(model->value_property(ui->mode->objectName()));

    ui->st_mechanical_locking->connect_property(model->value_property(ui->st_mechanical_locking->objectName()));
    ui->st_electronic_locking->connect_property(model->value_property(ui->st_electronic_locking->objectName()));
    ui->st_free_locking->connect_property(model->value_property(ui->st_free_locking->objectName()));
    ui->st_gyrostabilization->connect_property(model->value_property(ui->st_gyrostabilization->objectName()));
    ui->st_zero->connect_property(model->value_property(ui->st_zero->objectName()));
    ui->st_hike->connect_property(model->value_property(ui->st_hike->objectName()));
    ui->st_pilot->connect_property(model->value_property(ui->st_pilot->objectName()));
    ui->st_angle_control->connect_property(model->value_property(ui->st_angle_control->objectName()));
    ui->st_park->connect_property(model->value_property(ui->st_park->objectName()));

    ui->x_drive_pos->connect_property(model->value_property(ui->x_drive_pos->objectName()));
    ui->x_angle->connect_property(model->value_property(ui->x_angle->objectName()));
    ui->x_speed->connect_property(model->value_property(ui->x_speed->objectName()));
    ui->z_drive_pos->connect_property(model->value_property(ui->z_drive_pos->objectName()));
    ui->z_angle->connect_property(model->value_property(ui->z_angle->objectName()));
    ui->z_speed->connect_property(model->value_property(ui->z_speed->objectName()));
    ui->alignment_pos_y->connect_property(model->value_property(ui->alignment_pos_y->objectName()));
    ui->alignment_pos_z->connect_property(model->value_property(ui->alignment_pos_z->objectName()));
    // ui->drift_x->connect_property(model->value_property(ui->drift_x->objectName()));
    // ui->drift_z->connect_property(model->value_property(ui->drift_z->objectName()));

    ui->angle_enable->connect_property(model->value_property(ui->angle_enable->objectName()));
    ui->x_angle_set->connect_property(model->value_property(ui->x_angle_set->objectName()));
    ui->z_angle_set->connect_property(model->value_property(ui->z_angle_set->objectName()));

    ui->x_speed_f_set->connect_property(model->value_property(ui->x_speed_f_set->objectName()));
    ui->z_speed_f_set->connect_property(model->value_property(ui->z_speed_f_set->objectName()));

    ui->angle_discrepancy_x->connect_property(model->value_property(ui->angle_discrepancy_x->objectName()));
    ui->angle_discrepancy_z->connect_property(model->value_property(ui->angle_discrepancy_z->objectName()));

    up_start->connect_property(model->cmd_property(up_start->objectName()));
    down_start->connect_property(model->cmd_property(down_start->objectName()));
    left_start->connect_property(model->cmd_property(left_start->objectName()));
    right_start->connect_property(model->cmd_property(right_start->objectName()));
    stop->connect_property(model->cmd_property(stop->objectName()));

    ui->angle_abs->connect_property(model->cmd_property(ui->angle_abs->objectName()));
    ui->angle_rel->connect_property(model->cmd_property(ui->angle_rel->objectName()));

    ui->speed_f_set ->connect_property(model->cmd_property(ui->speed_f_set ->objectName()));
    ui->speed_f_stop->connect_property(model->cmd_property(ui->speed_f_stop->objectName()));

    ui->arretir_reset->connect_property(model->cmd_property(ui->arretir_reset->objectName()));
    ui->stabilization->connect_property(model->cmd_property(ui->stabilization->objectName()));
    ui->park->connect_property(model->cmd_property(ui->park->objectName()));
    ui->arretir_set->connect_property(model->cmd_property(ui->arretir_set->objectName()));

    ui->null_position->connect_property(model->cmd_property(ui->null_position->objectName()));
    ui->pohod_position->connect_property(model->cmd_property(ui->pohod_position->objectName()));
    ui->polyot_position->connect_property(model->cmd_property(ui->polyot_position->objectName()));

    ui->alignment->connect_property(model->cmd_property(ui->alignment->objectName()));

    ui->x_arretir_off     ->connect_property(model->cmd_property(ui->x_arretir_off     ->objectName()));
    ui->z_arretir_off     ->connect_property(model->cmd_property(ui->z_arretir_off     ->objectName()));
    ui->all_arretir_off   ->connect_property(model->cmd_property(ui->all_arretir_off   ->objectName()));
    ui->x_arretir_on      ->connect_property(model->cmd_property(ui->x_arretir_on      ->objectName()));
    ui->z_arretir_on      ->connect_property(model->cmd_property(ui->z_arretir_on      ->objectName()));
    ui->all_arretir_on    ->connect_property(model->cmd_property(ui->all_arretir_on    ->objectName()));
    ui->fine_arretir_reset->connect_property(model->cmd_property(ui->fine_arretir_reset->objectName()));
    ui->fine_arretir_set  ->connect_property(model->cmd_property(ui->fine_arretir_set  ->objectName()));

    // ------------------------------------------------ АС --------------------------------------------------------- //
    // ПИД
    ui->x_Kp->connect_property(model->value_property(ui->x_Kp->objectName()));
    ui->x_Ki->connect_property(model->value_property(ui->x_Ki->objectName()));
    ui->x_Kd->connect_property(model->value_property(ui->x_Kd->objectName()));
    ui->x_integral_limit->connect_property(model->value_property(ui->x_integral_limit->objectName()));
    ui->z_Kp->connect_property(model->value_property(ui->z_Kp->objectName()));
    ui->z_Ki->connect_property(model->value_property(ui->z_Ki->objectName()));
    ui->z_Kd->connect_property(model->value_property(ui->z_Kd->objectName()));
    ui->z_integral_limit->connect_property(model->value_property(ui->z_integral_limit->objectName()));
    ui->pid_out_limit_z->connect_property(model->value_property(ui->pid_out_limit_z->objectName()));
    ui->pid_out_limit_x->connect_property(model->value_property(ui->pid_out_limit_x->objectName()));
    // Фильтры
    ui->target_filter_k_x->connect_property(model->value_property(ui->target_filter_k_x->objectName()));
    ui->target_filter_k_y->connect_property(model->value_property(ui->target_filter_k_y->objectName()));
    ui->target_filter_ctrl->connect_property(model->cmd_property(ui->target_filter_ctrl->objectName()));
    ui->angle_filter_ctrl->connect_property(model->cmd_property(ui->angle_filter_ctrl->objectName()));
    ui->tr_type_no_filter->connect_property(model->value_property(ui->tr_type_no_filter->objectName()));
    ui->tr_type_exp_filter->connect_property(model->value_property(ui->tr_type_exp_filter->objectName()));
    ui->tr_type_kalman_filter->connect_property(model->value_property(ui->tr_type_kalman_filter->objectName()));
    ui->tr_type_lsm_filter->connect_property(model->value_property(ui->tr_type_lsm_filter->objectName()));
    ui->tr_speed_limit_en->connect_property(model->value_property(ui->tr_speed_limit_en->objectName()));

    // Сохранение нового углового положения
    ui->new_angle_x    ->connect_property(model->value_property(ui->new_angle_x    ->objectName()));
    ui->new_angle_z    ->connect_property(model->value_property(ui->new_angle_z    ->objectName()));
    ui->set_new_angle_x->connect_property(model->cmd_property  (ui->set_new_angle_x->objectName()));
    ui->set_new_angle_z->connect_property(model->cmd_property  (ui->set_new_angle_z->objectName()));
    ui->set_new_angles ->connect_property(model->cmd_property  (ui->set_new_angles ->objectName()));

    // --------------------------------------- Измерение скорости --------------------------------------------------//
    ui->measure_speed->connect_property(model->value_property(ui->measure_speed->objectName()));

    ui->measure_speed_n ->connect_property(model->value_property(ui->measure_speed_n ->objectName()));
    ui->measure_speed_n0->connect_property(model->value_property(ui->measure_speed_n0->objectName()));
    ui->measure_speed_nt->connect_property(model->value_property(ui->measure_speed_nt->objectName()));

    ui->measure_speed_angle0->connect_property(model->value_property(ui->measure_speed_angle0->objectName()));
    ui->measure_speed_anglet->connect_property(model->value_property(ui->measure_speed_anglet->objectName()));

    ui->x_speed_set->connect_property(model->value_property(ui->x_speed_set->objectName()));
    ui->z_speed_set->connect_property(model->value_property(ui->z_speed_set->objectName()));
    ui->speed_set  ->connect_property(model->cmd_property  (ui->speed_set  ->objectName()));
    ui->speed_calc ->connect_property(model->cmd_property  (ui->speed_calc ->objectName()));

    // --------------------------------------- Сырой статус --------------------------------------------------------//
    ui->bits          ->connect_property(model->value_property(ui->bits          ->objectName()));
    ui->mech_arr_x    ->connect_property(model->value_property(ui->mech_arr_x    ->objectName()));
    ui->mech_arr_z    ->connect_property(model->value_property(ui->mech_arr_z    ->objectName()));
    ui->mech_arr_f    ->connect_property(model->value_property(ui->mech_arr_f    ->objectName()));
    ui->stab_x        ->connect_property(model->value_property(ui->stab_x        ->objectName()));
    ui->stab_z        ->connect_property(model->value_property(ui->stab_z        ->objectName()));
    ui->park_x        ->connect_property(model->value_property(ui->park_x        ->objectName()));
    ui->park_z        ->connect_property(model->value_property(ui->park_z        ->objectName()));
    ui->zero_x        ->connect_property(model->value_property(ui->zero_x        ->objectName()));
    ui->zero_z        ->connect_property(model->value_property(ui->zero_z        ->objectName()));
    ui->el_arr_x      ->connect_property(model->value_property(ui->el_arr_x      ->objectName()));
    ui->el_arr_z      ->connect_property(model->value_property(ui->el_arr_z      ->objectName()));
    ui->zero_pos      ->connect_property(model->value_property(ui->zero_pos      ->objectName()));
    ui->pilot_pos     ->connect_property(model->value_property(ui->pilot_pos     ->objectName()));
    ui->pohod_pos     ->connect_property(model->value_property(ui->pohod_pos     ->objectName()));
    ui->can_error     ->connect_property(model->value_property(ui->can_error     ->objectName()));
    ui->adc_bit       ->connect_property(model->value_property(ui->adc_bit       ->objectName()));
    ui->angle_is_set_x->connect_property(model->value_property(ui->angle_is_set_x->objectName()));
    ui->angle_is_set_z->connect_property(model->value_property(ui->angle_is_set_z->objectName()));
    ui->diagnostics   ->connect_property(model->value_property(ui->diagnostics   ->objectName()));
    ui->drift_en      ->connect_property(model->value_property(ui->drift_en      ->objectName()));
    ui->drift_mode    ->connect_property(model->value_property(ui->drift_mode    ->objectName()));
    ui->ctrl_mode     ->connect_property(model->value_property(ui->ctrl_mode     ->objectName()));
    ui->fsm_mode_z    ->connect_property(model->value_property(ui->fsm_mode_z    ->objectName()));
    ui->fsm_mode_x    ->connect_property(model->value_property(ui->fsm_mode_x    ->objectName()));
    ui->park_f_x      ->connect_property(model->value_property(ui->park_f_x      ->objectName()));
    ui->park_f_z      ->connect_property(model->value_property(ui->park_f_z      ->objectName()));
    ui->diagnostic_res_z->connect_property(model->value_property(ui->diagnostic_res_z->objectName()));
    ui->diagnostic_res_x->connect_property(model->value_property(ui->diagnostic_res_x->objectName()));

    // Самодиагностика
    ui->bits_selftest    ->connect_property(model->value_property(ui->bits_selftest    ->objectName()));
    ui->timeout_err_x    ->connect_property(model->value_property(ui->timeout_err_x    ->objectName()));
    ui->el_arr_err_x     ->connect_property(model->value_property(ui->el_arr_err_x     ->objectName()));
    ui->stab_err_x       ->connect_property(model->value_property(ui->stab_err_x       ->objectName()));
    ui->el_arr_zero_err_x->connect_property(model->value_property(ui->el_arr_zero_err_x->objectName()));
    ui->stab_angle_err_x ->connect_property(model->value_property(ui->stab_angle_err_x ->objectName()));
    ui->timeout_err_z    ->connect_property(model->value_property(ui->timeout_err_z    ->objectName()));
    ui->el_arr_err_z     ->connect_property(model->value_property(ui->el_arr_err_z     ->objectName()));
    ui->stab_err_z       ->connect_property(model->value_property(ui->stab_err_z       ->objectName()));
    ui->el_arr_zero_err_z->connect_property(model->value_property(ui->el_arr_zero_err_z->objectName()));
    ui->stab_angle_err_z ->connect_property(model->value_property(ui->stab_angle_err_z ->objectName()));
}
void GSPCtrlView::on_up_pressed()
{
    emit up_start->cmd();
}
void GSPCtrlView::on_up_released()
{
    emit stop->cmd();
}
void GSPCtrlView::on_down_pressed()
{
    emit down_start->cmd();
}
void GSPCtrlView::on_down_released()
{
    emit stop->cmd();
}
void GSPCtrlView::on_left_pressed()
{
    emit left_start->cmd();
}
void GSPCtrlView::on_left_released()
{
    emit stop->cmd();
}
void GSPCtrlView::on_right_pressed()
{
    emit right_start->cmd();
}
void GSPCtrlView::on_right_released()
{
    emit stop->cmd();
}
void GSPCtrlView::on_stop_clicked()
{
    emit stop->cmd();
}

void GSPCtrlView::gyCal_calibrate()
{
    m_controller->cmdManger()->gyro_calibration(
        (float)ui->dsbGyCal_yaw->value(),
        (float)ui->dsbGyCal_pitch->value()
        );
}

void GSPCtrlView::gyCal_auto()
{
    m_controller->cmdManger()->gyro_calibration_auto();
}

GSPCtrlView::~GSPCtrlView()
{
    delete ui;
}

void GSPCtrlView::setController(Controller *ctrl)
{
    m_controller = ctrl;
}
