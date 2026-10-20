#include "telemetryview.h"
#include "ui_telemetryview.h"

#include <qmv/qmvmodel.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/valueproperty.h>

TelemetryView::TelemetryView(QWidget * parent) :
    QMVView(parent),
    ui(new Ui::TelemetryView)
{
    ui->setupUi(this);

    ui->rangefinder_status->set_label_text("rangefinder_status");
    ui->rangefinder_status->set_label_width(100);
    ui->rangefinder_status->hide_set_btn();
    ui->rangefinder_status->hide_set_btn();
    ui->rangefinder_status->hide_get_btn();
    ui->rangefinder_status->setDisabled(true);

    ui->meas_status->set_label_text("meas_status");
    ui->meas_status->set_label_width(100);
    ui->meas_status->hide_set_btn();
    ui->meas_status->hide_get_btn();
    ui->meas_status->setDisabled(true);

    ui->last_distance->set_label_text("last_distance");
    ui->last_distance->set_label_width(100);
    ui->last_distance->hide_set_btn();
    ui->last_distance->hide_get_btn();
    ui->last_distance->setDisabled(true);

    ui->avg_distance->set_label_text("avg_distance");
    ui->avg_distance->set_label_width(100);
    ui->avg_distance->hide_set_btn();
    ui->avg_distance->hide_get_btn();
    ui->avg_distance->setDisabled(true);

    ui->rms_distance->set_label_text("rms_distance");
    ui->rms_distance->set_label_width(100);
    ui->rms_distance->hide_set_btn();
    ui->rms_distance->hide_get_btn();
    ui->rms_distance->setDisabled(true);

    ui->laser_temp->set_label_text("laser_temp");
    ui->laser_temp->set_label_width(100);
    ui->laser_temp->hide_set_btn();
    ui->laser_temp->hide_get_btn();
    ui->laser_temp->setDisabled(true);

    ui->body_temp->set_label_text("body_temp");
    ui->body_temp->set_label_width(100);
    ui->body_temp->hide_set_btn();
    ui->body_temp->hide_get_btn();
    ui->body_temp->setDisabled(true);

    ui->lrf_RMS->set_label_text("lrf_RMS");
    ui->lrf_RMS->set_label_width(100);
    ui->lrf_RMS->hide_set_btn();
    ui->lrf_RMS->hide_get_btn();
    ui->lrf_RMS->setDisabled(true);

    ui->lrf_Amp->set_label_text("lrf_Amp");
    ui->lrf_Amp->set_label_width(100);
    ui->lrf_Amp->hide_set_btn();
    ui->lrf_Amp->hide_get_btn();
    ui->lrf_Amp->setDisabled(true);

    ui->tv_sensor_status->set_label_text("tv_sensor_status");
    ui->tv_sensor_status->set_label_width(100);
    ui->tv_sensor_status->hide_set_btn();
    ui->tv_sensor_status->hide_get_btn();
    ui->tv_sensor_status->setDisabled(true);

    ui->tv_lens_status->set_label_text("tv_lens_status");
    ui->tv_lens_status->set_label_width(100);
    ui->tv_lens_status->hide_set_btn();
    ui->tv_lens_status->hide_get_btn();
    ui->tv_lens_status->setDisabled(true);

    ui->tv_fps->set_label_text("tv_fps");
    ui->tv_fps->set_label_width(100);
    ui->tv_fps->hide_set_btn();
    ui->tv_fps->hide_get_btn();
    ui->tv_fps->setDisabled(true);

    ui->tv_vres->set_label_text("tv_vres");
    ui->tv_vres->set_label_width(100);
    ui->tv_vres->hide_set_btn();
    ui->tv_vres->hide_get_btn();
    ui->tv_vres->setDisabled(true);

    ui->tv_hres->set_label_text("tv_hres");
    ui->tv_hres->set_label_width(100);
    ui->tv_hres->hide_set_btn();
    ui->tv_hres->hide_get_btn();
    ui->tv_hres->setDisabled(true);

    ui->tv_exp->set_label_text("tv_exp");
    ui->tv_exp->set_label_width(100);
    ui->tv_exp->hide_set_btn();
    ui->tv_exp->hide_get_btn();
    ui->tv_exp->setDisabled(true);

    ui->tv_zoom_drive->set_label_text("tv_zoom_drive");
    ui->tv_zoom_drive->set_label_width(100);
    ui->tv_zoom_drive->hide_set_btn();
    ui->tv_zoom_drive->hide_get_btn();
    ui->tv_zoom_drive->setDisabled(true);

    ui->tv_focus_drive->set_label_text("tv_focus_drive");
    ui->tv_focus_drive->set_label_width(100);
    ui->tv_focus_drive->hide_set_btn();
    ui->tv_focus_drive->hide_get_btn();
    ui->tv_focus_drive->setDisabled(true);

    ui->tv_lens_temp->set_label_text("tv_lens_temp");
    ui->tv_lens_temp->set_label_width(100);
    ui->tv_lens_temp->hide_set_btn();
    ui->tv_lens_temp->hide_get_btn();
    ui->tv_lens_temp->setDisabled(true);

    ui->tv_focus_mm->set_label_text("tv_focus_mm");
    ui->tv_focus_mm->set_label_width(100);
    ui->tv_focus_mm->hide_set_btn();
    ui->tv_focus_mm->hide_get_btn();
    ui->tv_focus_mm->setDisabled(true);

    ui->tv_field_grad->set_label_text("tv_field_grad");
    ui->tv_field_grad->set_label_width(100);
    ui->tv_field_grad->hide_set_btn();
    ui->tv_field_grad->hide_get_btn();
    ui->tv_field_grad->setDisabled(true);

    // SW
    ui->swir_sensor_status->set_label_text("swir_sensor_status");
    ui->swir_sensor_status->set_label_width(100);
    ui->swir_sensor_status->hide_set_btn();
    ui->swir_sensor_status->hide_get_btn();
    ui->swir_sensor_status->setDisabled(true);

    ui->swir_lens_status->set_label_text("swir_lens_status");
    ui->swir_lens_status->set_label_width(100);
    ui->swir_lens_status->hide_set_btn();
    ui->swir_lens_status->hide_get_btn();
    ui->swir_lens_status->setDisabled(true);

    ui->swir_fps->set_label_text("swir_fps");
    ui->swir_fps->set_label_width(100);
    ui->swir_fps->hide_set_btn();
    ui->swir_fps->hide_get_btn();
    ui->swir_fps->setDisabled(true);

    ui->swir_vres->set_label_text("swir_vres");
    ui->swir_vres->set_label_width(100);
    ui->swir_vres->hide_set_btn();
    ui->swir_vres->hide_get_btn();
    ui->swir_vres->setDisabled(true);

    ui->swir_hres->set_label_text("swir_hres");
    ui->swir_hres->set_label_width(100);
    ui->swir_hres->hide_set_btn();
    ui->swir_hres->hide_get_btn();
    ui->swir_hres->setDisabled(true);

    ui->swir_exp->set_label_text("swir_exp");
    ui->swir_exp->set_label_width(100);
    ui->swir_exp->hide_set_btn();
    ui->swir_exp->hide_get_btn();
    ui->swir_exp->setDisabled(true);

    ui->swir_zoom_drive->set_label_text("swir_zoom_drive");
    ui->swir_zoom_drive->set_label_width(100);
    ui->swir_zoom_drive->hide_set_btn();
    ui->swir_zoom_drive->hide_get_btn();
    ui->swir_zoom_drive->setDisabled(true);

    ui->swir_focus_drive->set_label_text("swir_focus_drive");
    ui->swir_focus_drive->set_label_width(100);
    ui->swir_focus_drive->hide_set_btn();
    ui->swir_focus_drive->hide_get_btn();
    ui->swir_focus_drive->setDisabled(true);

    ui->swir_lens_temp->set_label_text("swir_lens_temp");
    ui->swir_lens_temp->set_label_width(100);
    ui->swir_lens_temp->hide_set_btn();
    ui->swir_lens_temp->hide_get_btn();
    ui->swir_lens_temp->setDisabled(true);

    ui->swir_focus_mm->set_label_text("swir_focus_mm");
    ui->swir_focus_mm->set_label_width(100);
    ui->swir_focus_mm->hide_set_btn();
    ui->swir_focus_mm->hide_get_btn();
    ui->swir_focus_mm->setDisabled(true);

    ui->swir_field_grad->set_label_text("swir_field_grad");
    ui->swir_field_grad->set_label_width(100);
    ui->swir_field_grad->hide_set_btn();
    ui->swir_field_grad->hide_get_btn();
    ui->swir_field_grad->setDisabled(true);

    ui->sw_gain->set_label_text("sw_gain");
    ui->sw_gain->set_label_width(100);
    ui->sw_gain->hide_set_btn();
    ui->sw_gain->hide_get_btn();
    ui->sw_gain->setDisabled(true);

    ui->mwir_sensor_status->set_label_text("mwir_sensor_status");
    ui->mwir_sensor_status->set_label_width(100);
    ui->mwir_sensor_status->hide_set_btn();
    ui->mwir_sensor_status->hide_get_btn();
    ui->mwir_sensor_status->setDisabled(true);

    ui->mwir_lens_status->set_label_text("mwir_lens_status");
    ui->mwir_lens_status->set_label_width(100);
    ui->mwir_lens_status->hide_set_btn();
    ui->mwir_lens_status->hide_get_btn();
    ui->mwir_lens_status->setDisabled(true);

    ui->mwir_fps->set_label_text("mwir_fps");
    ui->mwir_fps->set_label_width(100);
    ui->mwir_fps->hide_set_btn();
    ui->mwir_fps->hide_get_btn();
    ui->mwir_fps->setDisabled(true);

    ui->mwir_vres->set_label_text("mwir_vres");
    ui->mwir_vres->set_label_width(100);
    ui->mwir_vres->hide_set_btn();
    ui->mwir_vres->hide_get_btn();
    ui->mwir_vres->setDisabled(true);

    ui->mwir_hres->set_label_text("mwir_hres");
    ui->mwir_hres->set_label_width(100);
    ui->mwir_hres->hide_set_btn();
    ui->mwir_hres->hide_get_btn();
    ui->mwir_hres->setDisabled(true);

    ui->mwir_exp->set_label_text("mwir_exp");
    ui->mwir_exp->set_label_width(100);
    ui->mwir_exp->hide_set_btn();
    ui->mwir_exp->hide_get_btn();
    ui->mwir_exp->setDisabled(true);

    ui->mwir_zoom_drive->set_label_text("mwir_zoom_drive");
    ui->mwir_zoom_drive->set_label_width(100);
    ui->mwir_zoom_drive->hide_set_btn();
    ui->mwir_zoom_drive->hide_get_btn();
    ui->mwir_zoom_drive->setDisabled(true);

    ui->mwir_focus_drive->set_label_text("mwir_focus_drive");
    ui->mwir_focus_drive->set_label_width(100);
    ui->mwir_focus_drive->hide_set_btn();
    ui->mwir_focus_drive->hide_get_btn();
    ui->mwir_focus_drive->setDisabled(true);

    ui->mwir_lens_temp->set_label_text("mwir_lens_temp");
    ui->mwir_lens_temp->set_label_width(100);
    ui->mwir_lens_temp->hide_set_btn();
    ui->mwir_lens_temp->hide_get_btn();
    ui->mwir_lens_temp->setDisabled(true);

    ui->mwir_focus_mm->set_label_text("mwir_focus_mm");
    ui->mwir_focus_mm->set_label_width(100);
    ui->mwir_focus_mm->hide_set_btn();
    ui->mwir_focus_mm->hide_get_btn();
    ui->mwir_focus_mm->setDisabled(true);

    ui->mwir_field_grad->set_label_text("mwir_field_grad");
    ui->mwir_field_grad->set_label_width(100);
    ui->mwir_field_grad->hide_set_btn();
    ui->mwir_field_grad->hide_get_btn();
    ui->mwir_field_grad->setDisabled(true);

    ui->gsp_status->set_label_text("gsp_status");
    ui->gsp_status->set_label_width(100);
    ui->gsp_status->hide_set_btn();
    ui->gsp_status->hide_get_btn();
    ui->gsp_status->setDisabled(true);

    ui->mode->set_label_text("mode");
    ui->mode->set_label_width(100);
    ui->mode->hide_set_btn();
    ui->mode->hide_get_btn();
    ui->mode->setDisabled(true);

    ui->z_drive_pos->set_label_text("z_drive_pos");
    ui->z_drive_pos->set_label_width(100);
    ui->z_drive_pos->hide_set_btn();
    ui->z_drive_pos->hide_get_btn();
    ui->z_drive_pos->setDisabled(true);

    ui->x_drive_pos->set_label_text("x_drive_pos");
    ui->x_drive_pos->set_label_width(100);
    ui->x_drive_pos->hide_set_btn();
    ui->x_drive_pos->hide_get_btn();
    ui->x_drive_pos->setDisabled(true);

    ui->z_angle->set_label_text("z_angle");
    ui->z_angle->set_label_width(100);
    ui->z_angle->hide_set_btn();
    ui->z_angle->hide_get_btn();
    ui->z_angle->setDisabled(true);

    ui->x_angle->set_label_text("x_angle");
    ui->x_angle->set_label_width(100);
    ui->x_angle->hide_set_btn();
    ui->x_angle->hide_get_btn();
    ui->x_angle->setDisabled(true);

    ui->speed_z->set_label_text("speed_z");
    ui->speed_z->set_label_width(100);
    ui->speed_z->hide_set_btn();
    ui->speed_z->hide_get_btn();
    ui->speed_z->setDisabled(true);

    ui->speed_x->set_label_text("speed_x");
    ui->speed_x->set_label_width(100);
    ui->speed_x->hide_set_btn();
    ui->speed_x->hide_get_btn();
    ui->speed_x->setDisabled(true);

    ui->alignment_pos_y->set_label_text("alignment_pos_y");
    ui->alignment_pos_y->set_label_width(100);
    ui->alignment_pos_y->hide_set_btn();
    ui->alignment_pos_y->hide_get_btn();
    ui->alignment_pos_y->setDisabled(true);

    ui->alignment_pos_z->set_label_text("alignment_pos_z");
    ui->alignment_pos_z->set_label_width(100);
    ui->alignment_pos_z->hide_set_btn();
    ui->alignment_pos_z->hide_get_btn();
    ui->alignment_pos_z->setDisabled(true);

    ui->tv_agc_colour->set_label_text("tv_agc_colour");
    ui->tv_agc_colour->set_label_width(100);
    ui->tv_agc_colour->hide_set_btn();
    ui->tv_agc_colour->hide_get_btn();
    ui->tv_agc_colour->setDisabled(true);

    ui->tv_agc_grey->set_label_text("tv_agc_grey");
    ui->tv_agc_grey->set_label_width(100);
    ui->tv_agc_grey->hide_set_btn();
    ui->tv_agc_grey->hide_get_btn();
    ui->tv_agc_grey->setDisabled(true);

    ui->sw_agc->set_label_text("sw_agc");
    ui->sw_agc->set_label_width(100);
    ui->sw_agc->hide_set_btn();
    ui->sw_agc->hide_get_btn();
    ui->sw_agc->setDisabled(true);

    ui->mw_agc->set_label_text("mw_agc");
    ui->mw_agc->set_label_width(100);
    ui->mw_agc->hide_set_btn();
    ui->mw_agc->hide_get_btn();
    ui->mw_agc->setDisabled(true);

    ui->inversion_mw->set_label_text("inversion_mw");
    ui->inversion_mw->set_label_width(100);
    ui->inversion_mw->hide_set_btn();
    ui->inversion_mw->hide_get_btn();
    ui->inversion_mw->setDisabled(true);

    ui->autofocus->set_label_text("autofocus");
    ui->autofocus->set_label_width(100);
    ui->autofocus->hide_set_btn();
    ui->autofocus->hide_get_btn();
    ui->autofocus->setDisabled(true);

    ui->autofocus_channel->set_label_text("autofocus ch");
    ui->autofocus_channel->set_label_width(100);
    ui->autofocus_channel->hide_get_btn();
    ui->autofocus_channel->hide_set_btn();
    ui->autofocus_channel->set_disabled();

    ui->field_sync->set_label_text("field_sync");
    ui->field_sync->set_label_width(100);
    ui->field_sync->hide_set_btn();
    ui->field_sync->hide_get_btn();
    ui->field_sync->setDisabled(true);

    ui->field_sync_channel->set_label_text("field_sinc ch");
    ui->field_sync_channel->set_label_width(100);
    ui->field_sync_channel->hide_get_btn();
    ui->field_sync_channel->hide_set_btn();
    ui->field_sync_channel->set_disabled();

    ui->tracker_channel->set_label_text("tracker_channel");
    ui->tracker_channel->set_label_width(100);
    ui->tracker_channel->hide_set_btn();
    ui->tracker_channel->hide_get_btn();
    ui->tracker_channel->setDisabled(true);

    ui->tracker_state->set_label_text("tracker_state");
    ui->tracker_state->set_label_width(100);
    ui->tracker_state->hide_set_btn();
    ui->tracker_state->hide_get_btn();
    ui->tracker_state->setDisabled(true);

    ui->target->set_label_text("target");
    ui->target->set_label_width(100);
    ui->target->hide_set_btn();
    ui->target->hide_get_btn();
    ui->target->setDisabled(true);

    ui->target_x_center->set_label_text("target_x_center");
    ui->target_x_center->set_label_width(100);
    ui->target_x_center->hide_set_btn();
    ui->target_x_center->hide_get_btn();
    ui->target_x_center->setDisabled(true);

    ui->target_y_center->set_label_text("target_y_center");
    ui->target_y_center->set_label_width(100);
    ui->target_y_center->hide_set_btn();
    ui->target_y_center->hide_get_btn();
    ui->target_y_center->setDisabled(true);

    ui->target_x_size->set_label_text("target_x_size");
    ui->target_x_size->set_label_width(100);
    ui->target_x_size->hide_set_btn();
    ui->target_x_size->hide_get_btn();
    ui->target_x_size->setDisabled(true);

    ui->target_y_size->set_label_text("target_y_size");
    ui->target_y_size->set_label_width(100);
    ui->target_y_size->hide_set_btn();
    ui->target_y_size->hide_get_btn();
    ui->target_y_size->setDisabled(true);

    ui->target_z_angle_center->set_label_text("target_z_angle_center");
    ui->target_z_angle_center->set_label_width(100);
    ui->target_z_angle_center->hide_set_btn();
    ui->target_z_angle_center->hide_get_btn();
    ui->target_z_angle_center->setDisabled(true);

    ui->target_x_angle_center->set_label_text("target_x_angle_center");
    ui->target_x_angle_center->set_label_width(100);
    ui->target_x_angle_center->hide_set_btn();
    ui->target_x_angle_center->hide_get_btn();
    ui->target_x_angle_center->setDisabled(true);

    ui->target_z_angle_size->set_label_text("target_z_angle_size");
    ui->target_z_angle_size->set_label_width(100);
    ui->target_z_angle_size->hide_set_btn();
    ui->target_z_angle_size->hide_get_btn();
    ui->target_z_angle_size->setDisabled(true);

    ui->target_x_angle_size->set_label_text("target_x_angle_size");
    ui->target_x_angle_size->set_label_width(100);
    ui->target_x_angle_size->hide_set_btn();
    ui->target_x_angle_size->hide_get_btn();
    ui->target_x_angle_size->setDisabled(true);

    ui->fc0_status->set_label_text("fc0_status");
    ui->fc0_status->set_label_width(100);
    ui->fc0_status->hide_set_btn();
    ui->fc0_status->hide_get_btn();
    ui->fc0_status->setDisabled(true);

    ui->fc1_status->set_label_text("fc1_status");
    ui->fc1_status->set_label_width(100);
    ui->fc1_status->hide_set_btn();
    ui->fc1_status->hide_get_btn();
    ui->fc1_status->setDisabled(true);

    ui->eth_status->set_label_text("eth_status");
    ui->eth_status->set_label_width(100);
    ui->eth_status->hide_set_btn();
    ui->eth_status->hide_get_btn();
    ui->eth_status->setDisabled(true);

    ui->com_status->set_label_text("com_status");
    ui->com_status->set_label_width(100);
    ui->com_status->hide_set_btn();
    ui->com_status->hide_get_btn();
    ui->com_status->setDisabled(true);

    ui->year->set_label_text("year");
    ui->year->set_label_width(100);
    ui->year->hide_set_btn();
    ui->year->hide_get_btn();
    ui->year->setDisabled(true);

    ui->month->set_label_text("month");
    ui->month->set_label_width(100);
    ui->month->hide_set_btn();
    ui->month->hide_get_btn();
    ui->month->setDisabled(true);

    ui->controller_id->set_label_text("controller_id");
    ui->controller_id->set_label_width(100);
    ui->controller_id->hide_set_btn();
    ui->controller_id->hide_get_btn();
    ui->controller_id->setDisabled(true);

    ui->hard_ver->set_label_text("hard_ver");
    ui->hard_ver->set_label_width(100);
    ui->hard_ver->hide_set_btn();
    ui->hard_ver->hide_get_btn();
    ui->hard_ver->setDisabled(true);

    ui->soft_ver->set_label_text("soft_ver");
    ui->soft_ver->set_label_width(100);
    ui->soft_ver->hide_set_btn();
    ui->soft_ver->hide_get_btn();
    ui->soft_ver->setDisabled(true);

    ui->hours->set_label_text("hours");
    ui->hours->set_label_width(100);
    ui->hours->hide_set_btn();
    ui->hours->hide_get_btn();
    ui->hours->setDisabled(true);

    ui->minutes->set_label_text("minutes");
    ui->minutes->set_label_width(100);
    ui->minutes->hide_set_btn();
    ui->minutes->hide_get_btn();
    ui->minutes->setDisabled(true);

    ui->st_count->set_label_text("st_count");
    ui->st_count->set_label_width(100);
    ui->st_count->hide_set_btn();
    ui->st_count->hide_get_btn();
    ui->st_count->setDisabled(true);

    ui->st_suc_perc->set_label_text("st_suc_perc");
    ui->st_suc_perc->set_label_width(100);
    ui->st_suc_perc->hide_set_btn();
    ui->st_suc_perc->hide_get_btn();
    ui->st_suc_perc->setDisabled(true);

    ui->st_in_progress->set_label_text("Progress");
    ui->st_in_progress->set_label_width(60);
    ui->st_in_progress->hide_set_btn();
    ui->st_in_progress->hide_get_btn();
    ui->st_in_progress->setDisabled(true);

    ui->st_conn_pc_eth->set_label_text("Client GigEthernet");
    ui->st_conn_pc_eth->set_label_width(100);
    ui->st_conn_pc_eth->hide_set_btn();
    ui->st_conn_pc_eth->hide_get_btn();
    ui->st_conn_pc_eth->setDisabled(true);
    ui->st_conn_server_fc->set_label_text("Client FibreChannel");
    ui->st_conn_server_fc->set_label_width(100);
    ui->st_conn_server_fc->hide_set_btn();
    ui->st_conn_server_fc->hide_get_btn();
    ui->st_conn_server_fc->setDisabled(true);
    ui->st_conn_gsp_rs422->set_label_text("GSP RS-422");
    ui->st_conn_gsp_rs422->set_label_width(100);
    ui->st_conn_gsp_rs422->hide_set_btn();
    ui->st_conn_gsp_rs422->hide_get_btn();
    ui->st_conn_gsp_rs422->setDisabled(true);
    ui->st_ready_gsp->set_label_text("GSP Ready");
    ui->st_ready_gsp->set_label_width(100);
    ui->st_ready_gsp->hide_set_btn();
    ui->st_ready_gsp->hide_get_btn();
    ui->st_ready_gsp->setDisabled(true);
    ui->st_conn_cpu_eth->set_label_text("CPU GigEthernet");
    ui->st_conn_cpu_eth->set_label_width(100);
    ui->st_conn_cpu_eth->hide_set_btn();
    ui->st_conn_cpu_eth->hide_get_btn();
    ui->st_conn_cpu_eth->setDisabled(true);
    ui->st_conn_mobile_seriallite->set_label_text("Seriallite");
    ui->st_conn_mobile_seriallite->set_label_width(100);
    ui->st_conn_mobile_seriallite->hide_set_btn();
    ui->st_conn_mobile_seriallite->hide_get_btn();
    ui->st_conn_mobile_seriallite->setDisabled(true);
    ui->st_conn_lrf_rs485->set_label_text("LRF RS-485");
    ui->st_conn_lrf_rs485->set_label_width(100);
    ui->st_conn_lrf_rs485->hide_set_btn();
    ui->st_conn_lrf_rs485->hide_get_btn();
    ui->st_conn_lrf_rs485->setDisabled(true);
    ui->st_ready_lrf->set_label_text("LRF Ready");
    ui->st_ready_lrf->set_label_width(100);
    ui->st_ready_lrf->hide_set_btn();
    ui->st_ready_lrf->hide_get_btn();
    ui->st_ready_lrf->setDisabled(true);
    ui->st_conn_sensor_tv_uart->set_label_text("TV Sensor UART");
    ui->st_conn_sensor_tv_uart->set_label_width(100);
    ui->st_conn_sensor_tv_uart->hide_set_btn();
    ui->st_conn_sensor_tv_uart->hide_get_btn();
    ui->st_conn_sensor_tv_uart->setDisabled(true);
    ui->st_conn_sensor_tv_camlink->set_label_text("TV Video CamLink");
    ui->st_conn_sensor_tv_camlink->set_label_width(100);
    ui->st_conn_sensor_tv_camlink->hide_set_btn();
    ui->st_conn_sensor_tv_camlink->hide_get_btn();
    ui->st_conn_sensor_tv_camlink->setDisabled(true);
    ui->st_conn_lens_tv_rs232->set_label_text("TV Lens RS-232");
    ui->st_conn_lens_tv_rs232->set_label_width(100);
    ui->st_conn_lens_tv_rs232->hide_set_btn();
    ui->st_conn_lens_tv_rs232->hide_get_btn();
    ui->st_conn_lens_tv_rs232->setDisabled(true);
    ui->st_ready_tv_image->set_label_text("TV Ready");
    ui->st_ready_tv_image->set_label_width(100);
    ui->st_ready_tv_image->hide_set_btn();
    ui->st_ready_tv_image->hide_get_btn();
    ui->st_ready_tv_image->setDisabled(true);
    ui->st_conn_sensor_sw_uart->set_label_text("SWIR Sensor UART");
    ui->st_conn_sensor_sw_uart->set_label_width(100);
    ui->st_conn_sensor_sw_uart->hide_set_btn();
    ui->st_conn_sensor_sw_uart->hide_get_btn();
    ui->st_conn_sensor_sw_uart->setDisabled(true);
    ui->st_conn_sensor_sw_camlink->set_label_text("SWIR Video CamLink");
    ui->st_conn_sensor_sw_camlink->set_label_width(100);
    ui->st_conn_sensor_sw_camlink->hide_set_btn();
    ui->st_conn_sensor_sw_camlink->hide_get_btn();
    ui->st_conn_sensor_sw_camlink->setDisabled(true);
    ui->st_conn_lens_sw_rs232->set_label_text("SWIR Lens RS-232");
    ui->st_conn_lens_sw_rs232->set_label_width(100);
    ui->st_conn_lens_sw_rs232->hide_set_btn();
    ui->st_conn_lens_sw_rs232->hide_get_btn();
    ui->st_conn_lens_sw_rs232->setDisabled(true);
    ui->st_ready_sw_image->set_label_text("SWIR Ready");
    ui->st_ready_sw_image->set_label_width(100);
    ui->st_ready_sw_image->hide_set_btn();
    ui->st_ready_sw_image->hide_get_btn();
    ui->st_ready_sw_image->setDisabled(true);
    ui->st_conn_sensor_mw_uart->set_label_text("MWIR Sensor UART");
    ui->st_conn_sensor_mw_uart->set_label_width(100);
    ui->st_conn_sensor_mw_uart->hide_set_btn();
    ui->st_conn_sensor_mw_uart->hide_get_btn();
    ui->st_conn_sensor_mw_uart->setDisabled(true);
    ui->st_conn_sensor_mw_camlink->set_label_text("MWIR Video CamLink");
    ui->st_conn_sensor_mw_camlink->set_label_width(100);
    ui->st_conn_sensor_mw_camlink->hide_set_btn();
    ui->st_conn_sensor_mw_camlink->hide_get_btn();
    ui->st_conn_sensor_mw_camlink->setDisabled(true);
    ui->st_conn_lens_mw_rs232->set_label_text("MWIR Lens RS-232");
    ui->st_conn_lens_mw_rs232->set_label_width(100);
    ui->st_conn_lens_mw_rs232->hide_set_btn();
    ui->st_conn_lens_mw_rs232->hide_get_btn();
    ui->st_conn_lens_mw_rs232->setDisabled(true);
    ui->st_ready_mw_image->set_label_text("MWIR Ready");
    ui->st_ready_mw_image->set_label_width(100);
    ui->st_ready_mw_image->hide_set_btn();
    ui->st_ready_mw_image->hide_get_btn();
    ui->st_ready_mw_image->setDisabled(true);
    ui->st_video_tv_bayer->set_label_text("Stream TV-Bayer");
    ui->st_video_tv_bayer->set_label_width(100);
    ui->st_video_tv_bayer->hide_set_btn();
    ui->st_video_tv_bayer->hide_get_btn();
    ui->st_video_tv_bayer->setDisabled(true);
    ui->st_video_tv_mono->set_label_text("Stream TV-Grey");
    ui->st_video_tv_mono->set_label_width(100);
    ui->st_video_tv_mono->hide_set_btn();
    ui->st_video_tv_mono->hide_get_btn();
    ui->st_video_tv_mono->setDisabled(true);
    ui->st_video_sw->set_label_text("Stream SWIR");
    ui->st_video_sw->set_label_width(100);
    ui->st_video_sw->hide_set_btn();
    ui->st_video_sw->hide_get_btn();
    ui->st_video_sw->setDisabled(true);
    ui->st_video_mw->set_label_text("Stream MWIR");
    ui->st_video_mw->set_label_width(100);
    ui->st_video_mw->hide_set_btn();
    ui->st_video_mw->hide_get_btn();
    ui->st_video_mw->setDisabled(true);
    ui->st_video_fusion->set_label_text("Stream Fusion");
    ui->st_video_fusion->set_label_width(100);
    ui->st_video_fusion->hide_set_btn();
    ui->st_video_fusion->hide_get_btn();
    ui->st_video_fusion->setDisabled(true);

    ui->st_drive_tv_zoom->set_label_text("TV Zoom");
    ui->st_drive_tv_zoom->set_label_width(100);
    ui->st_drive_tv_zoom->hide_set_btn();
    ui->st_drive_tv_zoom->hide_get_btn();
    ui->st_drive_tv_zoom->setDisabled(true);
    ui->st_drive_tv_focus->set_label_text("TV Focus");
    ui->st_drive_tv_focus->set_label_width(100);
    ui->st_drive_tv_focus->hide_set_btn();
    ui->st_drive_tv_focus->hide_get_btn();
    ui->st_drive_tv_focus->setDisabled(true);
    ui->st_drive_tv_focus->setDisabled(true);
    ui->st_drive_sw_zoom->set_label_text("SWIR Zoom");
    ui->st_drive_sw_zoom->set_label_width(100);
    ui->st_drive_sw_zoom->hide_set_btn();
    ui->st_drive_sw_zoom->hide_get_btn();
    ui->st_drive_sw_zoom->setDisabled(true);
    ui->st_drive_sw_focus->set_label_text("SWIR Focus");
    ui->st_drive_sw_focus->set_label_width(100);
    ui->st_drive_sw_focus->hide_set_btn();
    ui->st_drive_sw_focus->hide_get_btn();
    ui->st_drive_sw_focus->setDisabled(true);
    ui->st_drive_mw_zoom->set_label_text("MWIR Zoom");
    ui->st_drive_mw_zoom->set_label_width(100);
    ui->st_drive_mw_zoom->hide_set_btn();
    ui->st_drive_mw_zoom->hide_get_btn();
    ui->st_drive_mw_zoom->setDisabled(true);
    ui->st_drive_mw_focus->set_label_text("MWIR Focus");
    ui->st_drive_mw_focus->set_label_width(100);
    ui->st_drive_mw_focus->hide_set_btn();
    ui->st_drive_mw_focus->hide_get_btn();
    ui->st_drive_mw_focus->setDisabled(true);
    ui->st_drive_gsp_azimuth->set_label_text("GSP Z");
    ui->st_drive_gsp_azimuth->set_label_width(100);
    ui->st_drive_gsp_azimuth->hide_set_btn();
    ui->st_drive_gsp_azimuth->hide_get_btn();
    ui->st_drive_gsp_azimuth->setDisabled(true);
    ui->st_drive_gsp_elevation->set_label_text("GSP X");
    ui->st_drive_gsp_elevation->set_label_width(100);
    ui->st_drive_gsp_elevation->hide_set_btn();
    ui->st_drive_gsp_elevation->hide_get_btn();
    ui->st_drive_gsp_elevation->setDisabled(true);

    //--------------------Для отладки фильтра Калмана---------------------//
    ui->kalman_coord_z->set_label_text("kalman_coord_z");
    ui->kalman_coord_z->set_label_width(100);
    ui->kalman_coord_z->hide_get_btn();
    ui->kalman_coord_z->hide_set_btn();
    ui->kalman_coord_z->setDisabled(true);

    ui->kalman_coord_x->set_label_text("kalman_coord_x");
    ui->kalman_coord_x->set_label_width(100);
    ui->kalman_coord_x->hide_get_btn();
    ui->kalman_coord_x->hide_set_btn();
    ui->kalman_coord_x->setDisabled(true);

    ui->kalman_speed_z->set_label_text("kalman_speed_z");
    ui->kalman_speed_z->set_label_width(100);
    ui->kalman_speed_z->hide_get_btn();
    ui->kalman_speed_z->hide_set_btn();
    ui->kalman_speed_z->setDisabled(true);

    ui->kalman_speed_x->set_label_text("kalman_speed_x");
    ui->kalman_speed_x->set_label_width(100);
    ui->kalman_speed_x->hide_get_btn();
    ui->kalman_speed_x->hide_set_btn();
    ui->kalman_speed_x->setDisabled(true);

    ui->kalman_accel_z->set_label_text("kalman_accel_z");
    ui->kalman_accel_z->set_label_width(100);
    ui->kalman_accel_z->hide_get_btn();
    ui->kalman_accel_z->hide_set_btn();
    ui->kalman_accel_z->setDisabled(true);

    ui->kalman_accel_x->set_label_text("kalman_accel_x");
    ui->kalman_accel_x->set_label_width(100);
    ui->kalman_accel_x->hide_get_btn();
    ui->kalman_accel_x->hide_set_btn();
    ui->kalman_accel_x->setDisabled(true);

    ui->kalman_delta_tracker_z->set_label_text("delta_angle_z");
    ui->kalman_delta_tracker_z->set_label_width(100);
    ui->kalman_delta_tracker_z->hide_get_btn();
    ui->kalman_delta_tracker_z->hide_set_btn();
    ui->kalman_delta_tracker_z->setDisabled(true);

    ui->kalman_delta_tracker_x->set_label_text("delta_angle_x");
    ui->kalman_delta_tracker_x->set_label_width(100);
    ui->kalman_delta_tracker_x->hide_get_btn();
    ui->kalman_delta_tracker_x->hide_set_btn();
    ui->kalman_delta_tracker_x->setDisabled(true);
   //--------------------------------------------------------------Стабильность системы-----------------------------------------------------------------------//
    ui->cycle_execution_time_mb->set_label_text("Время выполнения МБ [мкс]");
    ui->cycle_execution_time_mb->set_label_width(200);
    ui->cycle_execution_time_mb->hide_set_btn();
    ui->cycle_execution_time_mb->hide_get_btn();
    ui->cycle_execution_time_mb->setDisabled(true);

    ui->time_between_cycles_mb->set_label_text("Время между выполнениями МБ [мкс]");
    ui->time_between_cycles_mb->set_label_width(200);
    ui->time_between_cycles_mb->hide_set_btn();
    ui->time_between_cycles_mb->hide_get_btn();
    ui->time_between_cycles_mb->setDisabled(true);

    ui->cycle_execution_time_fb->set_label_text("Время выполнения ФБ [мкс]");
    ui->cycle_execution_time_fb->set_label_width(200);
    ui->cycle_execution_time_fb->hide_set_btn();
    ui->cycle_execution_time_fb->hide_get_btn();
    ui->cycle_execution_time_fb->setDisabled(true);

    ui->time_between_cycles_fb->set_label_text("Время между выполнениями ФБ [мкс]");
    ui->time_between_cycles_fb->set_label_width(200);
    ui->time_between_cycles_fb->hide_set_btn();
    ui->time_between_cycles_fb->hide_get_btn();
    ui->time_between_cycles_fb->setDisabled(true);

    ui->mobile_board_counter_tel->set_label_text("Счётчик телеметрии МБ");
    ui->mobile_board_counter_tel->set_label_width(200);
    ui->mobile_board_counter_tel->hide_set_btn();
    ui->mobile_board_counter_tel->hide_get_btn();
    ui->mobile_board_counter_tel->setDisabled(true);

    ui->fixed_board_counter_tel->set_label_text("Счётчик телеметрии ФБ");
    ui->fixed_board_counter_tel->set_label_width(200);
    ui->fixed_board_counter_tel->hide_set_btn();
    ui->fixed_board_counter_tel->hide_get_btn();
    ui->fixed_board_counter_tel->setDisabled(true);

    ui->mobile_board_telemetry_per_sec->set_label_text("Телеметрия с МБ на ФБ [n/сек]");
    ui->mobile_board_telemetry_per_sec->set_label_width(200);
    ui->mobile_board_telemetry_per_sec->hide_set_btn();
    ui->mobile_board_telemetry_per_sec->hide_get_btn();
    ui->mobile_board_telemetry_per_sec->setDisabled(true);

    ui->gsp_per_sec->set_label_text("Телеметрия ГСП [n/сек]");
    ui->gsp_per_sec->set_label_width(200);
    ui->gsp_per_sec->hide_set_btn();
    ui->gsp_per_sec->hide_get_btn();
    ui->gsp_per_sec->setDisabled(true);

    ui->cpu_per_sec->set_label_text("Телеметрия CPU [n/сек]");
    ui->cpu_per_sec->set_label_width(200);
    ui->cpu_per_sec->hide_set_btn();
    ui->cpu_per_sec->hide_get_btn();
    ui->cpu_per_sec->setDisabled(true);

    ui->adapter_per_sec->set_label_text("Телеметрия адаптера [n/сек]");
    ui->adapter_per_sec->set_label_width(200);
    ui->adapter_per_sec->hide_set_btn();
    ui->adapter_per_sec->hide_get_btn();
    ui->adapter_per_sec->setDisabled(true);

    // ---------------------- additional struct -------------------------------------------- //
    ui->add_gsp_volt->set_label_text("input_voltage, V");
    ui->add_gsp_volt->set_label_width(100);
    ui->add_gsp_volt->hide_set_btn();
    ui->add_gsp_volt->hide_get_btn();
    ui->add_gsp_volt->setDisabled(true);

    ui->add_rf_meas_delta_time->set_label_text("rf_meas_delta_time");
    ui->add_rf_meas_delta_time->set_label_width(100);
    ui->add_rf_meas_delta_time->hide_set_btn();
    ui->add_rf_meas_delta_time->hide_get_btn();
    ui->add_rf_meas_delta_time->setDisabled(true);

    ui->add_rf_cooling_time_cnt->set_label_text("rf_cooling_time_cnt");
    ui->add_rf_cooling_time_cnt->set_label_width(100);
    ui->add_rf_cooling_time_cnt->hide_set_btn();
    ui->add_rf_cooling_time_cnt->hide_get_btn();
    ui->add_rf_cooling_time_cnt->setDisabled(true);

    ui->add_rf_radiation_time_cnt->set_label_text("rf_radiation_time_cnt");
    ui->add_rf_radiation_time_cnt->set_label_width(100);
    ui->add_rf_radiation_time_cnt->hide_set_btn();
    ui->add_rf_radiation_time_cnt->hide_get_btn();
    ui->add_rf_radiation_time_cnt->setDisabled(true);

    ui->add_mw_coarse_af_en->set_label_text("mw_coarse_af_en");
    ui->add_mw_coarse_af_en->set_label_width(100);
    ui->add_mw_coarse_af_en->hide_set_btn();
    ui->add_mw_coarse_af_en->hide_get_btn();
    ui->add_mw_coarse_af_en->setDisabled(true);

    ui->add_fc_tv_fps->set_label_text("fc_tv_fps");
    ui->add_fc_tv_fps->set_label_width(100);
    ui->add_fc_tv_fps->hide_set_btn();
    ui->add_fc_tv_fps->hide_get_btn();
    ui->add_fc_tv_fps->setDisabled(true);

    ui->add_fc_mw_fps->set_label_text("fc_mw_fps");
    ui->add_fc_mw_fps->set_label_width(100);
    ui->add_fc_mw_fps->hide_set_btn();
    ui->add_fc_mw_fps->hide_get_btn();
    ui->add_fc_mw_fps->setDisabled(true);

    ui->add_fc_fusion_fps->set_label_text("fc_fusion_fps");
    ui->add_fc_fusion_fps->set_label_width(100);
    ui->add_fc_fusion_fps->hide_set_btn();
    ui->add_fc_fusion_fps->hide_get_btn();
    ui->add_fc_fusion_fps->setDisabled(true);
}
void TelemetryView::connect_model()
{
    ui->rangefinder_status->connect_property(model->value_property(ui->rangefinder_status->objectName()));
    ui->meas_status->connect_property(model->value_property(ui->meas_status->objectName()));
    ui->last_distance->connect_property(model->value_property(ui->last_distance->objectName()));
    ui->avg_distance->connect_property(model->value_property(ui->avg_distance->objectName()));
    ui->rms_distance->connect_property(model->value_property(ui->rms_distance->objectName()));
    ui->laser_temp->connect_property(model->value_property(ui->laser_temp->objectName()));
    ui->body_temp->connect_property(model->value_property(ui->body_temp->objectName()));
    ui->lrf_RMS->connect_property(model->value_property(ui->lrf_RMS->objectName()));
    ui->lrf_Amp->connect_property(model->value_property(ui->lrf_Amp->objectName()));

    ui->tv_sensor_status->connect_property(model->value_property(ui->tv_sensor_status->objectName()));
    ui->tv_lens_status->connect_property(model->value_property(ui->tv_lens_status->objectName()));
    ui->tv_fps->connect_property(model->value_property(ui->tv_fps->objectName()));
    ui->tv_vres->connect_property(model->value_property(ui->tv_vres->objectName()));
    ui->tv_hres->connect_property(model->value_property(ui->tv_hres->objectName()));
    ui->tv_exp->connect_property(model->value_property(ui->tv_exp->objectName()));
    ui->tv_zoom_drive->connect_property(model->value_property(ui->tv_zoom_drive->objectName()));
    ui->tv_focus_drive->connect_property(model->value_property(ui->tv_focus_drive->objectName()));
    ui->tv_lens_temp->connect_property(model->value_property(ui->tv_lens_temp->objectName()));
    ui->tv_focus_mm->connect_property(model->value_property(ui->tv_focus_mm->objectName()));
    ui->tv_field_grad->connect_property(model->value_property(ui->tv_field_grad->objectName()));

    ui->swir_sensor_status->connect_property(model->value_property(ui->swir_sensor_status->objectName()));
    ui->swir_lens_status->connect_property(model->value_property(ui->swir_lens_status->objectName()));
    ui->swir_fps->connect_property(model->value_property(ui->swir_fps->objectName()));
    ui->swir_vres->connect_property(model->value_property(ui->swir_vres->objectName()));
    ui->swir_hres->connect_property(model->value_property(ui->swir_hres->objectName()));
    ui->swir_exp->connect_property(model->value_property(ui->swir_exp->objectName()));
    ui->swir_zoom_drive->connect_property(model->value_property(ui->swir_zoom_drive->objectName()));
    ui->swir_focus_drive->connect_property(model->value_property(ui->swir_focus_drive->objectName()));
    ui->swir_lens_temp->connect_property(model->value_property(ui->swir_lens_temp->objectName()));
    ui->swir_focus_mm->connect_property(model->value_property(ui->swir_focus_mm->objectName()));
    ui->swir_field_grad->connect_property(model->value_property(ui->swir_field_grad->objectName()));
    ui->sw_gain->connect_property(model->value_property(ui->sw_gain->objectName()));

    ui->mwir_sensor_status->connect_property(model->value_property(ui->mwir_sensor_status->objectName()));
    ui->mwir_lens_status->connect_property(model->value_property(ui->mwir_lens_status->objectName()));
    ui->mwir_fps->connect_property(model->value_property(ui->mwir_fps->objectName()));
    ui->mwir_vres->connect_property(model->value_property(ui->mwir_vres->objectName()));
    ui->mwir_hres->connect_property(model->value_property(ui->mwir_hres->objectName()));
    ui->mwir_exp->connect_property(model->value_property(ui->mwir_exp->objectName()));
    ui->mwir_zoom_drive->connect_property(model->value_property(ui->mwir_zoom_drive->objectName()));
    ui->mwir_focus_drive->connect_property(model->value_property(ui->mwir_focus_drive->objectName()));
    ui->mwir_lens_temp->connect_property(model->value_property(ui->mwir_lens_temp->objectName()));
    ui->mwir_focus_mm->connect_property(model->value_property(ui->mwir_focus_mm->objectName()));
    ui->mwir_field_grad->connect_property(model->value_property(ui->mwir_field_grad->objectName()));

    ui->gsp_status->connect_property(model->value_property(ui->gsp_status->objectName()));
    ui->mode->connect_property(model->value_property(ui->mode->objectName()));
    ui->z_drive_pos->connect_property(model->value_property(ui->z_drive_pos->objectName()));
    ui->x_drive_pos->connect_property(model->value_property(ui->x_drive_pos->objectName()));
    ui->z_angle->connect_property(model->value_property(ui->z_angle->objectName()));
    ui->x_angle->connect_property(model->value_property(ui->x_angle->objectName()));
    ui->speed_z->connect_property(model->value_property(ui->speed_z->objectName()));
    ui->speed_x->connect_property(model->value_property(ui->speed_x->objectName()));
    ui->alignment_pos_y->connect_property(model->value_property(ui->alignment_pos_y->objectName()));
    ui->alignment_pos_z->connect_property(model->value_property(ui->alignment_pos_z->objectName()));

    ui->tv_agc_colour->connect_property(model->value_property(ui->tv_agc_colour->objectName()));
    ui->tv_agc_grey->connect_property(model->value_property(ui->tv_agc_grey->objectName()));
    ui->sw_agc->connect_property(model->value_property(ui->sw_agc->objectName()));
    ui->mw_agc->connect_property(model->value_property(ui->mw_agc->objectName()));
    ui->inversion_mw->connect_property(model->value_property(ui->inversion_mw->objectName()));
    ui->autofocus->connect_property(model->value_property(ui->autofocus->objectName()));
    ui->autofocus_channel->connect_property(model->value_property(ui->autofocus_channel->objectName()));
    ui->field_sync->connect_property(model->value_property(ui->field_sync->objectName()));
    ui->field_sync_channel->connect_property(model->value_property(ui->field_sync_channel->objectName()));
    ui->tracker_channel->connect_property(model->value_property(ui->tracker_channel->objectName()));
    ui->tracker_state->connect_property(model->value_property(ui->tracker_state->objectName()));
    ui->target->connect_property(model->value_property(ui->target->objectName()));
    ui->target_x_center->connect_property(model->value_property(ui->target_x_center->objectName()));
    ui->target_y_center->connect_property(model->value_property(ui->target_y_center->objectName()));
    ui->target_x_size->connect_property(model->value_property(ui->target_x_size->objectName()));
    ui->target_y_size->connect_property(model->value_property(ui->target_y_size->objectName()));
    ui->target_z_angle_center->connect_property(model->value_property(ui->target_z_angle_center->objectName()));
    ui->target_x_angle_center->connect_property(model->value_property(ui->target_x_angle_center->objectName()));
    ui->target_z_angle_size->connect_property(model->value_property(ui->target_z_angle_size->objectName()));
    ui->target_x_angle_size->connect_property(model->value_property(ui->target_x_angle_size->objectName()));
    ui->fc0_status->connect_property(model->value_property(ui->fc0_status->objectName()));
    ui->fc1_status->connect_property(model->value_property(ui->fc1_status->objectName()));
    ui->eth_status->connect_property(model->value_property(ui->eth_status->objectName()));
    ui->com_status->connect_property(model->value_property(ui->com_status->objectName()));
    ui->year->connect_property(model->value_property(ui->year->objectName()));
    ui->month->connect_property(model->value_property(ui->month->objectName()));
    ui->controller_id->connect_property(model->value_property(ui->controller_id->objectName()));
    ui->hard_ver->connect_property(model->value_property(ui->hard_ver->objectName()));
    ui->soft_ver->connect_property(model->value_property(ui->soft_ver->objectName()));
    ui->hours->connect_property(model->value_property(ui->hours->objectName()));
    ui->minutes->connect_property(model->value_property(ui->minutes->objectName()));
    ui->st_count->connect_property(model->value_property(ui->st_count->objectName()));
    ui->st_suc_perc->connect_property(model->value_property(ui->st_suc_perc->objectName()));

    ui->st_in_progress->connect_property(model->value_property(ui->st_in_progress->objectName()));
    ui->st_conn_pc_eth->connect_property(model->value_property(ui->st_conn_pc_eth->objectName()));
    ui->st_conn_server_fc->connect_property(model->value_property(ui->st_conn_server_fc->objectName()));
    ui->st_conn_gsp_rs422->connect_property(model->value_property(ui->st_conn_gsp_rs422->objectName()));
    ui->st_ready_gsp->connect_property(model->value_property(ui->st_ready_gsp->objectName()));
    ui->st_conn_cpu_eth->connect_property(model->value_property(ui->st_conn_cpu_eth->objectName()));
    ui->st_conn_mobile_seriallite->connect_property(model->value_property(ui->st_conn_mobile_seriallite->objectName()));
    ui->st_conn_lrf_rs485->connect_property(model->value_property(ui->st_conn_lrf_rs485->objectName()));
    ui->st_ready_lrf->connect_property(model->value_property(ui->st_ready_lrf->objectName()));
    ui->st_conn_sensor_tv_uart->connect_property(model->value_property(ui->st_conn_sensor_tv_uart->objectName()));
    ui->st_conn_sensor_tv_camlink->connect_property(model->value_property(ui->st_conn_sensor_tv_camlink->objectName()));
    ui->st_conn_lens_tv_rs232->connect_property(model->value_property(ui->st_conn_lens_tv_rs232->objectName()));
    ui->st_ready_tv_image->connect_property(model->value_property(ui->st_ready_tv_image->objectName()));
    ui->st_conn_sensor_sw_uart->connect_property(model->value_property(ui->st_conn_sensor_sw_uart->objectName()));
    ui->st_conn_sensor_sw_camlink->connect_property(model->value_property(ui->st_conn_sensor_sw_camlink->objectName()));
    ui->st_conn_lens_sw_rs232->connect_property(model->value_property(ui->st_conn_lens_sw_rs232->objectName()));
    ui->st_ready_sw_image->connect_property(model->value_property(ui->st_ready_sw_image->objectName()));
    ui->st_conn_sensor_mw_uart->connect_property(model->value_property(ui->st_conn_sensor_mw_uart->objectName()));
    ui->st_conn_sensor_mw_camlink->connect_property(model->value_property(ui->st_conn_sensor_mw_camlink->objectName()));
    ui->st_conn_lens_mw_rs232->connect_property(model->value_property(ui->st_conn_lens_mw_rs232->objectName()));
    ui->st_ready_mw_image->connect_property(model->value_property(ui->st_ready_mw_image->objectName()));
    ui->st_video_tv_bayer->connect_property(model->value_property(ui->st_video_tv_bayer->objectName()));
    ui->st_video_tv_mono->connect_property(model->value_property(ui->st_video_tv_mono->objectName()));
    ui->st_video_sw->connect_property(model->value_property(ui->st_video_sw->objectName()));
    ui->st_video_mw->connect_property(model->value_property(ui->st_video_mw->objectName()));
    ui->st_video_fusion->connect_property(model->value_property(ui->st_video_fusion->objectName()));
    ui->st_drive_tv_zoom->connect_property(model->value_property(ui->st_drive_tv_zoom->objectName()));
    ui->st_drive_tv_focus->connect_property(model->value_property(ui->st_drive_tv_focus->objectName()));
    ui->st_drive_sw_zoom->connect_property(model->value_property(ui->st_drive_sw_zoom->objectName()));
    ui->st_drive_sw_focus->connect_property(model->value_property(ui->st_drive_sw_focus->objectName()));
    ui->st_drive_mw_zoom->connect_property(model->value_property(ui->st_drive_mw_zoom->objectName()));
    ui->st_drive_mw_focus->connect_property(model->value_property(ui->st_drive_mw_focus->objectName()));
    ui->st_drive_gsp_azimuth->connect_property(model->value_property(ui->st_drive_gsp_azimuth->objectName()));
    ui->st_drive_gsp_elevation->connect_property(model->value_property(ui->st_drive_gsp_elevation->objectName()));

    ui->kalman_coord_z->connect_property(model->value_property(ui->kalman_coord_z->objectName()));
    ui->kalman_coord_x->connect_property(model->value_property(ui->kalman_coord_x->objectName()));
    ui->kalman_speed_z->connect_property(model->value_property(ui->kalman_speed_z->objectName()));
    ui->kalman_speed_x->connect_property(model->value_property(ui->kalman_speed_x->objectName()));
    ui->kalman_accel_z->connect_property(model->value_property(ui->kalman_accel_z->objectName()));
    ui->kalman_accel_x->connect_property(model->value_property(ui->kalman_accel_x->objectName()));
    ui->kalman_delta_tracker_z->connect_property(model->value_property(ui->kalman_delta_tracker_z->objectName()));
    ui->kalman_delta_tracker_x->connect_property(model->value_property(ui->kalman_delta_tracker_x->objectName()));
    //--------------------------------------------------------------Стабильность системы-----------------------------------------------------------------------//
    ui->cycle_execution_time_mb->connect_property(model->value_property(ui->cycle_execution_time_mb->objectName()));
    ui->time_between_cycles_mb->connect_property(model->value_property(ui->time_between_cycles_mb->objectName()));
    ui->cycle_execution_time_fb->connect_property(model->value_property(ui->cycle_execution_time_fb->objectName()));
    ui->time_between_cycles_fb->connect_property(model->value_property(ui->time_between_cycles_fb->objectName()));
    ui->mobile_board_counter_tel->connect_property(model->value_property(ui->mobile_board_counter_tel->objectName()));
    ui->fixed_board_counter_tel->connect_property(model->value_property(ui->fixed_board_counter_tel->objectName()));
    ui->mobile_board_telemetry_per_sec->connect_property(model->value_property(ui->mobile_board_telemetry_per_sec->objectName()));
    ui->gsp_per_sec->connect_property(model->value_property(ui->gsp_per_sec->objectName()));
    ui->cpu_per_sec->connect_property(model->value_property(ui->cpu_per_sec->objectName()));
    ui->adapter_per_sec->connect_property(model->value_property(ui->adapter_per_sec->objectName()));
    // ------------------------------------------------------------- additional struct ---------------------------------------------------------------------- //
    ui->add_gsp_volt             ->connect_property(model->value_property(ui->add_gsp_volt             ->objectName()));
    ui->add_rf_meas_delta_time   ->connect_property(model->value_property(ui->add_rf_meas_delta_time   ->objectName()));
    ui->add_rf_cooling_time_cnt  ->connect_property(model->value_property(ui->add_rf_cooling_time_cnt  ->objectName()));
    ui->add_rf_radiation_time_cnt->connect_property(model->value_property(ui->add_rf_radiation_time_cnt->objectName()));
    ui->add_mw_coarse_af_en      ->connect_property(model->value_property(ui->add_mw_coarse_af_en      ->objectName()));
    ui->add_fc_tv_fps            ->connect_property(model->value_property(ui->add_fc_tv_fps            ->objectName()));
    ui->add_fc_mw_fps            ->connect_property(model->value_property(ui->add_fc_mw_fps            ->objectName()));
    ui->add_fc_fusion_fps        ->connect_property(model->value_property(ui->add_fc_fusion_fps        ->objectName()));
}
TelemetryView::~TelemetryView()
{
    delete ui;
}
