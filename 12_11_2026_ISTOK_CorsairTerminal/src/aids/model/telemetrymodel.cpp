#include "telemetrymodel.h"
#include "telemetrymodel.h"
#include "telemetrymodel.h"

#include <QDateTime>
#include <QVariant>

#include <qmv/property/booleanvalueproperty.h>
#include <qmv/property/doublevalueproperty.h>
#include <qmv/property/intvalueproperty.h>
#include <qmv/property/stringvalueproperty.h>

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logTelemetry, "Telemetry")

TelemetryModel::TelemetryModel(DEV_TYPES dev_type, QObject * parent, uint32_t id, QString name) :
    QMVModel(parent, id, name),
    _dev_type(dev_type),
    _write_dump(false),
    rangefinder_status(new IntValueProperty(this, id_rangefinder_status, "rangefinder_status", 0, -655360, 655360)),
    meas_status(new IntValueProperty(this, id_meas_status, "meas_status", 0, -655360, 655360)),
    last_distance(new DoubleValueProperty(this, id_last_distance, "last_distance", 0, -655360, 655360)),
    avg_distance(new DoubleValueProperty(this, id_avg_distance, "avg_distance", 0, -655360, 655360)),
    rms_distance(new DoubleValueProperty(this, id_rms_distance, "rms_distance", 0, -655360, 655360)),
    laser_temp(new DoubleValueProperty(this, id_laser_temp, "laser_temp", 0, -655360, 655360)),
    body_temp(new DoubleValueProperty(this, id_body_temp, "body_temp", 0, -655360, 655360)),

    tv_sensor_status(new IntValueProperty(this, id_tv_sensor_status, "tv_sensor_status", 0, -655360, 655360)),
    tv_lens_status(new IntValueProperty(this, id_tv_lens_status, "tv_lens_status", 0, -655360, 655360)),
    tv_fps(new IntValueProperty(this, id_tv_fps, "tv_fps", 0, -655360, 655360)),
    tv_vres(new IntValueProperty(this, id_tv_vres, "tv_vres", 0, -655360, 655360)),
    tv_hres(new IntValueProperty(this, id_tv_hres, "tv_hres", 0, -655360, 655360)),
    tv_exp(new IntValueProperty(this, id_tv_exp, "tv_exp", 0, -655360, 655360)),
    tv_zoom_drive(new IntValueProperty(this, id_tv_zoom_drive, "tv_zoom_drive", 0, -655360, 655360)),
    tv_focus_drive(new IntValueProperty(this, id_tv_focus_drive, "tv_focus_drive", 0, -655360, 655360)),
    tv_lens_temp(new IntValueProperty(this, id_tv_lens_temp, "tv_lens_temp", 0, -655360, 655360)),
    tv_focus_mm(new DoubleValueProperty(this, id_tv_focus_mm, "tv_focus_mm", 0)),
    tv_field_grad(new DoubleValueProperty(this, id_tv_field_grad, "tv_field_grad", 0, -655360, 655360)),

    swir_sensor_status(new IntValueProperty(this, id_swir_sensor_status, "swir_sensor_status", 0, -655360, 655360)),
    swir_lens_status(new IntValueProperty(this, id_swir_lens_status, "swir_lens_status", 0, -655360, 655360)),
    swir_fps(new IntValueProperty(this, id_swir_fps, "swir_fps", 0, -655360, 655360)),
    swir_vres(new IntValueProperty(this, id_swir_vres, "swir_vres", 0, -655360, 655360)),
    swir_hres(new IntValueProperty(this, id_swir_hres, "swir_hres", 0, -655360, 655360)),
    swir_exp(new IntValueProperty(this, id_swir_exp, "swir_exp", 0, -655360, 655360)),
    swir_zoom_drive(new IntValueProperty(this, id_swir_zoom_drive, "swir_zoom_drive", 0, -655360, 655360)),
    swir_focus_drive(new IntValueProperty(this, id_swir_focus_drive, "swir_focus_drive", 0, -655360, 655360)),
    swir_lens_temp(new IntValueProperty(this, id_swir_lens_temp, "swir_lens_temp", 0, -655360, 655360)),
    swir_focus_mm(new DoubleValueProperty(this, id_swir_focus_mm, "swir_focus_mm", 0, -655360, 655360)),
    swir_field_grad(new DoubleValueProperty(this, id_swir_field_grad, "swir_field_grad", 0, -655360, 655360)),

    mwir_sensor_status(new IntValueProperty(this, id_mwir_sensor_status, "mwir_sensor_status", 0, -655360, 655360)),
    mwir_lens_status(new IntValueProperty(this, id_mwir_lens_status, "mwir_lens_status", 0, -655360, 655360)),
    mwir_fps(new IntValueProperty(this, id_mwir_fps, "mwir_fps", 0, -655360, 655360)),
    mwir_vres(new IntValueProperty(this, id_mwir_vres, "mwir_vres", 0, -655360, 655360)),
    mwir_hres(new IntValueProperty(this, id_mwir_hres, "mwir_hres", 0, -655360, 655360)),
    mwir_exp(new IntValueProperty(this, id_mwir_exp, "mwir_exp", 0, -655360, 655360)),
    mwir_zoom_drive(new IntValueProperty(this, id_mwir_zoom_drive, "mwir_zoom_drive", 0, -655360, 655360)),
    mwir_focus_drive(new IntValueProperty(this, id_mwir_focus_drive, "mwir_focus_drive", 0, -655360, 655360)),
    mwir_lens_temp(new IntValueProperty(this, id_mwir_lens_temp, "mwir_lens_temp", 0, -655360, 655360)),
    mwir_focus_mm(new DoubleValueProperty(this, id_mwir_focus_mm, "mwir_focus_mm", 0, -655360, 655360)),
    mwir_field_grad(new DoubleValueProperty(this, id_mwir_field_grad, "mwir_field_grad", 0, -655360, 655360)),

    gsp_status(new IntValueProperty(this, id_gsp_status, "gsp_status", 0, -655360, 655360)),
    mode(new IntValueProperty(this, id_mode, "mode", 0, -655360, 655360)),
    z_drive_pos(new IntValueProperty(this, id_z_drive_pos, "z_drive_pos", 0, -655360, 655360)),
    x_drive_pos(new IntValueProperty(this, id_x_drive_pos, "x_drive_pos", 0, -655360, 655360)),
    z_angle(new DoubleValueProperty(this, id_z_angle, "z_angle", 0, -655360, 655360)),
    x_angle(new DoubleValueProperty(this, id_x_angle, "x_angle", 0, -655360, 655360)),
    speed_z(new IntValueProperty(this, id_speed_z, "speed_z", 0, -655360, 655360)),
    speed_x(new IntValueProperty(this, id_speed_x, "speed_x", 0, -655360, 655360)),

    tv_agc_colour(new IntValueProperty(this, id_tv_agc_colour, "tv_agc_colour", 0, -655360, 655360)),
    tv_agc_grey(new IntValueProperty(this, id_tv_agc_grey, "tv_agc_grey", 0, -655360, 655360)),
    sw_agc(new IntValueProperty(this, id_sw_agc, "sw_agc", 0, -655360, 655360)),
    mw_agc(new IntValueProperty(this, id_mw_agc, "mw_agc", 0, -655360, 655360)),
    inversion_mw(new IntValueProperty(this, id_inversion_mw, "inversion_mw", 0, -655360, 655360)),
    autofocus(new IntValueProperty(this, id_autofocus, "autofocus", 0, -655360, 655360)),
    autofocus_channel(new StringValueProperty(this, id_autofocus_channel, "autofocus_channel", QString("unknown"))),
    field_sync(new IntValueProperty(this, id_field_sync, "field_sync", 0, -655360, 655360)),
    field_sync_channel(new StringValueProperty(this, id_field_sync_channel, "field_sync_channel", QString("unknown"))),
    tracker_channel(new IntValueProperty(this, id_tracker_channel, "tracker_channel", 0, -655360, 655360)),
    tracker_state(new IntValueProperty(this, id_tracker_state, "tracker_state", 0, -655360, 655360)),
    target(new IntValueProperty(this, id_target, "target", 0, -655360, 655360)),
    target_x_center(new IntValueProperty(this, id_target_x_center, "target_x_center", 0, -655360, 655360)),
    target_y_center(new IntValueProperty(this, id_target_y_center, "target_y_center", 0, -655360, 655360)),
    target_x_size(new IntValueProperty(this, id_target_x_size, "target_x_size", 0, -655360, 655360)),
    target_y_size(new IntValueProperty(this, id_target_y_size, "target_y_size", 0, -655360, 655360)),
    target_z_angle_center(new DoubleValueProperty(this, id_target_z_angle_center, "target_z_angle_center", 0, -655360, 655360)),
    target_x_angle_center(new DoubleValueProperty(this, id_target_x_angle_center, "target_x_angle_center", 0, -655360, 655360)),
    target_z_angle_size(new DoubleValueProperty(this, id_target_z_angle_size, "target_z_angle_size", 0, -655360, 655360)),
    target_x_angle_size(new DoubleValueProperty(this, id_target_x_angle_size, "target_x_angle_size", 0, -655360, 655360)),
    fc0_status(new IntValueProperty(this, id_fc0_status, "fc0_status", 0, -655360, 655360)),
    fc1_status(new IntValueProperty(this, id_fc1_status, "fc1_status", 0, -655360, 655360)),
    eth_status(new IntValueProperty(this, id_eth_status, "eth_status", 0, -655360, 655360)),
    com_status(new IntValueProperty(this, id_com_status, "com_status", 0, -655360, 655360)),
    year(new IntValueProperty(this, id_year, "year", 0, -655360, 655360)),
    month(new IntValueProperty(this, id_month, "month", 0, -655360, 655360)),
    controller_id(new IntValueProperty(this, id_controller_id, "controller_id", 0, -655360, 655360)),
    hard_ver(new IntValueProperty(this, id_hard_ver, "hard_ver", 0, -655360, 655360)),
    soft_ver(new IntValueProperty(this, id_soft_ver, "soft_ver", 0, -655360, 655360)),
    hours(new IntValueProperty(this, id_hours, "hours", 0, -655360, 655360)),
    minutes(new IntValueProperty(this, id_minutes, "minutes", 0, -655360, 655360)),
    sw_gain(new IntValueProperty(this, id_sw_gain, "sw_gain", 0, -655360, 655360)),
    st_suc_perc(new DoubleValueProperty(this, id_st_suc_perc, "st_suc_perc", 0, -655360, 655360)),
    st_count(new IntValueProperty(this, id_st_count, "st_count", 0, -655360, 655360)),
    alignment_pos_y(new IntValueProperty(this, id_alignment_pos_y, "alignment_pos_y", 0, -655360, 655360)),
    alignment_pos_z(new IntValueProperty(this, id_alignment_pos_z, "alignment_pos_z", 0, -655360, 655360)),

    lrf_RMS(new IntValueProperty(this, id_lrf_RMS, "lrf_RMS", 0, -655360, 655360)),
    lrf_Amp(new IntValueProperty(this, id_lrf_Amp, "lrf_Amp", 0, -655360, 655360)),

    st_in_progress(new BooleanValueProperty(this,            id_st_in_progress,               "st_in_progress", false)),
    st_conn_pc_eth(new BooleanValueProperty(this,            id_st_st_conn_pc_eth,            "st_conn_pc_eth", false)),
    st_conn_server_fc(new BooleanValueProperty(this,         id_st_st_conn_server_fc,         "st_conn_server_fc", false)),
    st_conn_gsp_rs422(new BooleanValueProperty(this,         id_st_st_conn_gsp_rs422,         "st_conn_gsp_rs422", false)),
    st_ready_gsp(new BooleanValueProperty(this,              id_st_st_ready_gsp,              "st_ready_gsp", false)),
    st_conn_cpu_eth(new BooleanValueProperty(this,           id_st_st_conn_cpu_eth,           "st_conn_cpu_eth", false)),
    st_conn_mobile_seriallite(new BooleanValueProperty(this, id_st_st_conn_mobile_seriallite, "st_conn_mobile_seriallite", false)),
    st_conn_lrf_rs485(new BooleanValueProperty(this,         id_st_st_conn_lrf_rs485,         "st_conn_lrf_rs485", false)),
    st_ready_lrf(new BooleanValueProperty(this,              id_st_st_ready_lrf,              "st_ready_lrf", false)),
    st_conn_sensor_tv_uart(new BooleanValueProperty(this,    id_st_st_conn_sensor_tv_uart,    "st_conn_sensor_tv_uart", false)),
    st_conn_sensor_tv_camlink(new BooleanValueProperty(this, id_st_st_conn_sensor_tv_camlink, "st_conn_sensor_tv_camlink", false)),
    st_conn_lens_tv_rs232(new BooleanValueProperty(this,     id_st_st_conn_lens_tv_rs232,     "st_conn_lens_tv_rs232", false)),
    st_ready_tv_image(new BooleanValueProperty(this,         id_st_st_ready_tv_image,         "st_ready_tv_image", false)),
    st_conn_sensor_sw_uart(new BooleanValueProperty(this,    id_st_st_conn_sensor_sw_uart,    "st_conn_sensor_sw_uart", false)),
    st_conn_sensor_sw_camlink(new BooleanValueProperty(this, id_st_st_conn_sensor_sw_camlink, "st_conn_sensor_sw_camlink", false)),
    st_conn_lens_sw_rs232(new BooleanValueProperty(this,     id_st_st_conn_lens_sw_rs232,     "st_conn_lens_sw_rs232", false)),
    st_ready_sw_image(new BooleanValueProperty(this,         id_st_st_ready_sw_image,         "st_ready_sw_image", false)),
    st_conn_sensor_mw_uart(new BooleanValueProperty(this,    id_st_st_conn_sensor_mw_uart,    "st_conn_sensor_mw_uart", false)),
    st_conn_sensor_mw_camlink(new BooleanValueProperty(this, id_st_st_conn_sensor_mw_camlink, "st_conn_sensor_mw_camlink", false)),
    st_conn_lens_mw_rs232(new BooleanValueProperty(this,     id_st_st_conn_lens_mw_rs232,     "st_conn_lens_mw_rs232", false)),
    st_ready_mw_image(new BooleanValueProperty(this,         id_st_st_ready_mw_image,         "st_ready_mw_image", false)),
    st_video_tv_bayer(new BooleanValueProperty(this,         id_st_st_video_tv_bayer,         "st_video_tv_bayer", false)),
    st_video_tv_mono(new BooleanValueProperty(this,          id_st_st_video_tv_mono,          "st_video_tv_mono", false)),
    st_video_sw(new BooleanValueProperty(this,               id_st_st_video_sw,               "st_video_sw", false)),
    st_video_mw(new BooleanValueProperty(this,               id_st_st_video_mw,               "st_video_mw", false)),
    st_video_fusion(new BooleanValueProperty(this,           id_st_st_video_fusion,           "st_video_fusion", false)),
    st_drive_tv_zoom(new BooleanValueProperty(this,          id_st_st_drive_tv_zoom,          "st_drive_tv_zoom", false)),
    st_drive_tv_focus(new BooleanValueProperty(this,         id_st_st_drive_tv_focus,         "st_drive_tv_focus", false)),
    st_drive_sw_zoom(new BooleanValueProperty(this,          id_st_st_drive_sw_zoom,          "st_drive_sw_zoom", false)),
    st_drive_sw_focus(new BooleanValueProperty(this,         id_st_st_drive_sw_focus,         "st_drive_sw_focus", false)),
    st_drive_mw_zoom(new BooleanValueProperty(this,          id_st_st_drive_mw_zoom,          "st_drive_mw_zoom", false)),
    st_drive_mw_focus(new BooleanValueProperty(this,         id_st_st_drive_mw_focus,         "st_drive_mw_focus", false)),
    st_drive_gsp_azimuth(new BooleanValueProperty(this,      id_st_st_drive_gsp_azimuth,      "st_drive_gsp_azimuth", false)),
    st_drive_gsp_elevation(new BooleanValueProperty(this,    id_st_st_drive_gsp_elevation,    "st_drive_gsp_elevation", false)),
    // ---------------------------------------------------------- Для отладки фильтра Калмана --------------------------------------------------------------------- //
    kalman_coord_z(new DoubleValueProperty(this, id_kalman_coord_z, "kalman_coord_z", 0, -655360, 655360)),
    kalman_coord_x(new DoubleValueProperty(this, id_kalman_coord_x, "kalman_coord_x", 0, -655360, 655360)),
    kalman_speed_z(new DoubleValueProperty(this, id_kalman_speed_z, "kalman_speed_z", 0, -655360, 655360)),
    kalman_speed_x(new DoubleValueProperty(this, id_kalman_speed_x, "kalman_speed_x", 0, -655360, 655360)),
    kalman_accel_z(new DoubleValueProperty(this, id_kalman_accel_z, "kalman_accel_z", 0, -655360, 655360)),
    kalman_accel_x(new DoubleValueProperty(this, id_kalman_accel_x, "kalman_accel_x", 0, -655360, 655360)),
    kalman_delta_tracker_z(new DoubleValueProperty(this, id_kalman_delta_tracker_z, "kalman_delta_tracker_z", 0, -655360, 655360)),
    kalman_delta_tracker_x(new DoubleValueProperty(this, id_kalman_delta_tracker_x, "kalman_delta_tracker_x", 0, -655360, 655360)),
    // -------------------------------------------------------------- Стабильность системы ----------------------------------------------------------------------- //
    cycle_execution_time_mb(       new IntValueProperty(this, id_cycle_execution_time_mb, "cycle_execution_time_mb",        0, -655360, 655360)),
    time_between_cycles_mb(        new IntValueProperty(this, id_time_between_cycles_mb, "time_between_cycles_mb",         0, -655360, 655360)),
    cycle_execution_time_fb(       new IntValueProperty(this, id_cycle_execution_time_fb, "cycle_execution_time_fb",        0, -655360, 655360)),
    time_between_cycles_fb(        new IntValueProperty(this, id_time_between_cycles_fb, "time_between_cycles_fb",         0, -655360, 655360)),
    fixed_board_counter_tel(       new IntValueProperty(this, id_fixed_board_counter_tel, "fixed_board_counter_tel",        0, -655360, 655360)),
    mobile_board_counter_tel(      new IntValueProperty(this, id_mobile_board_counter_tel, "mobile_board_counter_tel",       0, -655360, 655360)),
    mobile_board_telemetry_per_sec(new IntValueProperty(this, id_mobile_board_telemetry_per_sec, "mobile_board_telemetry_per_sec", 0, -655360, 655360)),
    gsp_per_sec(                   new IntValueProperty(this, id_gsp_per_sec, "gsp_per_sec",                    0, -655360, 655360)),
    cpu_per_sec(                   new IntValueProperty(this, id_cpu_per_sec, "cpu_per_sec",                    0, -655360, 655360)),
    adapter_per_sec(               new IntValueProperty(this, id_adapter_per_sec, "adapter_per_sec",                0, -655360, 655360)),
    // --------------------------------------------------------------- Additional struct ------------------------------------------------------------------------- //
    add_gsp_volt             (new IntValueProperty    (this, id_add_gsp_volt             , "add_gsp_volt",              0, -655360, 655360)),
    add_rf_meas_delta_time   (new IntValueProperty    (this, id_add_rf_meas_delta_time   , "add_rf_meas_delta_time",    0, -655360, 655360)),
    add_rf_cooling_time_cnt  (new IntValueProperty    (this, id_add_rf_cooling_time_cnt  , "add_rf_cooling_time_cnt",   0, -655360, 655360)),
    add_rf_radiation_time_cnt(new IntValueProperty    (this, id_add_rf_radiation_time_cnt, "add_rf_radiation_time_cnt", 0, -655360, 655360)),
    add_mw_coarse_af_en      (new BooleanValueProperty(this, id_add_mw_coarse_af_en      , "add_mw_coarse_af_en",       false)),
    add_fc_tv_fps            (new IntValueProperty    (this, id_add_fc_tv_fps            , "add_fc_tv_fps",             0, -655360, 655360)),
    add_fc_mw_fps            (new IntValueProperty    (this, id_add_fc_mw_fps            , "add_fc_mw_fps",             0, -655360, 655360)),
    add_fc_fusion_fps        (new IntValueProperty    (this, id_add_fc_fusion_fps        , "add_fc_fusion_fps",         0, -655360, 655360))
{
}
TelemetryModel::~TelemetryModel()
{
    close_dump_file();
}
void TelemetryModel::set(const uint32_t &)
{
}
void TelemetryModel::get(const uint32_t &)
{
}
void TelemetryModel::cmd(const uint32_t &)
{
}
void TelemetryModel::write_dump(const bool& enable)
{
    _write_dump = enable;
    if(_write_dump) {
        open_dump_file();
    } else {
        close_dump_file();
    }
}
void TelemetryModel::response(Cmd)
{
}
void TelemetryModel::response(Telemetry t)
{
    Telemetry::t_telemetry_struct * tel = t.get();

    rangefinder_status->set_val(tel->rf.status);
    meas_status->set(tel->rf.meas_status);
    last_distance->set(tel->rf.last_distance);
    avg_distance->set(tel->rf.avg_distance);
    rms_distance->set(tel->rf.rms_distance);
    body_temp->set(tel->rf.body_temp);

    tv_sensor_status->set(tel->tv.sensor_status);
    tv_lens_status->set(tel->tv.lens_status);
    tv_fps->set(tel->tv.fps);
    tv_vres->set(tel->tv.vres);
    tv_hres->set(tel->tv.hres);
    tv_exp->set(tel->tv.exp);
    tv_zoom_drive->set(tel->tv.zoom_drive);
    tv_focus_drive->set(tel->tv.focus_drive);
    tv_lens_temp->set(tel->tv.lens_temp);
    tv_focus_mm->set(tel->tv.focus_mm);
    tv_field_grad->set(tel->tv.field_grad);

    if(_dev_type == DEV_TYPES::en_dev_type_350 || _dev_type == DEV_TYPES::en_dev_type_obzor) {
        swir_sensor_status->set(tel->sw.sensor_status);
        swir_lens_status->set(tel->sw.lens_status);
        swir_fps->set(tel->sw.fps);
        swir_vres->set(tel->sw.vres);
        swir_hres->set(tel->sw.hres);
        swir_exp->set(tel->sw.exp);
        swir_zoom_drive->set(tel->sw.zoom_drive);
        swir_focus_drive->set(tel->sw.focus_drive);
        swir_lens_temp->set(tel->sw.lens_temp);
        swir_focus_mm->set(tel->sw.focus_mm);
        swir_field_grad->set(tel->sw.field_grad);
    } else if (_dev_type == DEV_TYPES::en_dev_type_350p) {

    }

    mwir_sensor_status->set(tel->mw.sensor_status);
    mwir_lens_status->set(tel->mw.lens_status);
    mwir_fps->set(tel->mw.fps);
    mwir_vres->set(tel->mw.vres);
    mwir_hres->set(tel->mw.hres);
    mwir_exp->set(tel->mw.exp);
    mwir_zoom_drive->set(tel->mw.zoom_drive);
    mwir_focus_drive->set(tel->mw.focus_drive);
    mwir_lens_temp->set(tel->mw.lens_temp);
    mwir_focus_mm->set(tel->mw.focus_mm);
    mwir_field_grad->set(tel->mw.field_grad);

    gsp_status->set(tel->gsp.status);
    mode->set(tel->gsp.mode.__raw_);
    z_drive_pos->set(tel->gsp.z_drive_pos);
    x_drive_pos->set(tel->gsp.x_drive_pos);
    z_angle->set(tel->gsp.z_angle);
    x_angle->set(tel->gsp.x_angle);
    speed_z->set(tel->gsp.speed_z);
    speed_x->set(tel->gsp.speed_x);

    tv_agc_colour->set(tel->agc.tv_colour_en);
    tv_agc_grey->set(tel->agc.tv_grey_en);
    sw_agc->set(tel->agc.swir_en);
    mw_agc->set(tel->agc.mwir_en);
    inversion_mw->set(tel->inversion_mw);

    autofocus->set(tel->controller.autofocus.state);
    autofocus_channel->set(camera_code_map.at(tel->controller.autofocus.state ? static_cast<Telemetry::camera_codes>(tel->controller.autofocus.channel) : Telemetry::disable));

    field_sync->set(tel->controller.field_sync.state);
    field_sync_channel->set(camera_code_map.at(tel->controller.field_sync.state ? static_cast<Telemetry::camera_codes>(tel->controller.field_sync.channel) : Telemetry::disable));

    tracker_state->set(tel->tracker.tracker_channel != 0xFF);
    tracker_channel->set(tel->tracker.tracker_channel);
    target->set(bool(tel->tracker.targets[0].target == 0));
    target_x_center->set(tel->tracker.targets[0].x_center);
    target_y_center->set(tel->tracker.targets[0].y_center);
    target_x_size->set(tel->tracker.targets[0].x_size);
    target_y_size->set(tel->tracker.targets[0].y_size);
    target_z_angle_center->set(tel->tracker.targets[0].z_angle_center);
    target_x_angle_center->set(tel->tracker.targets[0].x_angle_center);
    target_z_angle_size->set(tel->tracker.targets[0].z_angle_size);
    target_x_angle_size->set(tel->tracker.targets[0].x_angle_size);
    fc0_status->set(tel->controller.fc0_status);
    fc1_status->set(tel->controller.fc1_status);
    eth_status->set(tel->controller.eth_status);
    com_status->set(tel->controller.com_status);
    year->set(tel->controller.year);
    month->set(tel->controller.month);
    controller_id->set(tel->controller.id);
    hard_ver->set(tel->controller.hard_ver);
    soft_ver->set(tel->controller.soft_ver);
    hours->set(tel->controller.hours);
    minutes->set(tel->controller.minutes);
    if(_dev_type == DEV_TYPES::en_dev_type_350) sw_gain->set(tel->controller.sw_gain);
    st_suc_perc->set(tel->controller.self_test_suc_perc);
    st_count->set(tel->controller.self_test_count);
    alignment_pos_y->set(tel->controller.alignment_pos_x);
    alignment_pos_z->set(tel->controller.alignment_pos_z);

    lrf_RMS->set(tel->dev.lrf_stat.lrf_RMS);
    lrf_Amp->set(tel->dev.lrf_stat.lrf_Amp);

    st_in_progress->set(      bool(tel->controller.self_test_in_progress));
    st_conn_pc_eth->set(           tel->controller.self_test.flags.conn_pc_eth);
    st_conn_server_fc->set(        tel->controller.self_test.flags.conn_server_fc);
    st_conn_gsp_rs422->set(        tel->controller.self_test.flags.conn_gsp_rs422);
    st_ready_gsp->set(             tel->controller.self_test.flags.ready_gsp);
    st_conn_cpu_eth->set(          tel->controller.self_test.flags.conn_cpu_eth);
    st_conn_mobile_seriallite->set(tel->controller.self_test.flags.conn_mobile_seriallite);
    st_conn_lrf_rs485->set(        tel->controller.self_test.flags.conn_lrf_rs485);
    st_ready_lrf->set(             tel->controller.self_test.flags.ready_lrf);
    st_conn_sensor_tv_uart->set(   tel->controller.self_test.flags.conn_sensor_tv_uart);
    st_conn_sensor_tv_camlink->set(tel->controller.self_test.flags.conn_sensor_tv_camlink);
    st_conn_lens_tv_rs232->set(    tel->controller.self_test.flags.conn_lens_tv_rs232);
    st_ready_tv_image->set(        tel->controller.self_test.flags.ready_tv_image);
    st_conn_sensor_sw_uart->set(   tel->controller.self_test.flags.conn_sensor_sw_uart);
    st_conn_sensor_sw_camlink->set(tel->controller.self_test.flags.conn_sensor_sw_camlink);
    st_conn_lens_sw_rs232->set(    tel->controller.self_test.flags.conn_lens_sw_rs232);
    st_ready_sw_image->set(        tel->controller.self_test.flags.ready_sw_image);
    st_conn_sensor_mw_uart->set(   tel->controller.self_test.flags.conn_sensor_mw_uart);
    st_conn_sensor_mw_camlink->set(tel->controller.self_test.flags.conn_sensor_mw_camlink);
    st_conn_lens_mw_rs232->set(    tel->controller.self_test.flags.conn_lens_mw_rs232);
    st_ready_mw_image->set(        tel->controller.self_test.flags.ready_mw_image);
    st_video_tv_bayer->set(        tel->controller.self_test.flags.video_tv_bayer);
    st_video_tv_mono->set(         tel->controller.self_test.flags.video_tv_mono);
    st_video_sw->set(              tel->controller.self_test.flags.video_sw);
    st_video_mw->set(              tel->controller.self_test.flags.video_mw);
    st_video_fusion->set(          tel->controller.self_test.flags.video_fusion);
    st_drive_tv_zoom->set(         tel->controller.self_test.flags.drive_tv_zoom);
    st_drive_tv_focus->set(        tel->controller.self_test.flags.drive_tv_focus);
    st_drive_sw_zoom->set(         tel->controller.self_test.flags.drive_sw_zoom);
    st_drive_sw_focus->set(        tel->controller.self_test.flags.drive_sw_focus);
    st_drive_mw_zoom->set(         tel->controller.self_test.flags.drive_mw_zoom);
    st_drive_mw_focus->set(        tel->controller.self_test.flags.drive_mw_focus);
    st_drive_gsp_azimuth->set(     tel->controller.self_test.flags.drive_gsp_azimuth);
    st_drive_gsp_elevation->set(   tel->controller.self_test.flags.drive_gsp_elevation);

    // Статистика
    cycle_execution_time_mb ->set(tel->dev.cycle_execution_time_mb);
    cycle_execution_time_fb ->set(tel->dev.cycle_execution_time_fb);

    time_between_cycles_mb  ->set(tel->dev.time_between_cycles_mb);
    time_between_cycles_fb  ->set(tel->dev.time_between_cycles_fb);

    mobile_board_counter_tel->set(tel->dev.mb_tel_counter);
    fixed_board_counter_tel ->set(tel->dev.fb_tel_counter);

    gsp_per_sec                   ->set(tel->dev.gsp_per_sec);
    cpu_per_sec                   ->set(tel->dev.cpu_per_sec);
    adapter_per_sec               ->set(tel->dev.adapter_per_sec);
    mobile_board_telemetry_per_sec->set(tel->dev.mb_tel_per_sec);

    // Additional struct
    add_gsp_volt             ->set(tel->add.gsp_volt);
    add_rf_meas_delta_time   ->set(tel->add.rf_meas_delta_time);
    add_rf_cooling_time_cnt  ->set(tel->add.rf_cooling_time_cnt);
    add_rf_radiation_time_cnt->set(tel->add.rf_radiation_time_cnt);
    add_mw_coarse_af_en      ->set(tel->add.mw_coarse_af_en);
    add_fc_tv_fps            ->set(tel->add.fc_tv_fps);
    add_fc_mw_fps            ->set(tel->add.fc_mw_fps);
    add_fc_fusion_fps        ->set(tel->add.fc_fusion_fps);

    if(_write_dump) {
        write_dump_in_txt(tel);
    }
}

void TelemetryModel::open_dump_file()
{
    close_dump_file();
    const auto data = QDate::currentDate().toString().toStdString(); // возвращаем текущую дату
    const auto time = QTime::currentTime().toString().toStdString(); // возвращаем текущее время
    std::string str(data);
    str.append("_").append(time).append("telemetry_dump");
    _dump_stream.open(str);
    write_dump_header();
}
void TelemetryModel::close_dump_file()
{
    if(_dump_stream.is_open()) {
        _dump_stream.close();
    }
}
void TelemetryModel::write_dump_header()
{
    if(_dump_stream.is_open()) {
        _dump_stream << "Time\t"
                     << "tv_sensor_status\t"
                     << "tv_lens_status\t"
                     << "tv_fps\t"
                     << "tv_vres\t"
                     << "tv_hres\t"
                     << "tv_exp\t"
                     << "tv_zoom_drive\t"
                     << "tv_focus_drive\t"
                     << "tv_lens_temp\t"
                     << "tv_focus_mm\t"
                     << "tv_field_grad\t"
                     << "mw_sensor_status\t"
                     << "mw_lens_status\t"
                     << "mw_fps\t"
                     << "mw_vres\t"
                     << "mw_hres\t"
                     << "mw_exp\t"
                     << "mw_zoom_drive\t"
                     << "mw_focus_drive\t"
                     << "mw_lens_temp\t"
                     << "mw_focus_mm\t"
                     << "mw_field_grad\t"
                     << "rf_status\t"
                     << "rf_meas_status\t"
                     << "rf_last_distance\t"
                     << "rf_avg_distance\t"
                     << "rf_rms_distance\t"
                     << "rf_state\t"
                     << "rf_body_temp\t"
                     << "rf_dev_lrf_RMS\t"
                     << "rf_dev_lrf_Amp\t"
                     << "rf_dev_lrf_correction\t"
                     << "rf_dev_frequency\t"
                     << "rf_dev_vavd\t"
                     << "gsp_status\t"
                     << "gsp_mode\t"
                     << "gsp_z_drive_pos\t"
                     << "gsp_x_drive_pos\t"
                     << "gsp_z_angle\t"
                     << "gsp_x_angle\t"
                     << "gsp_speed_z\t"
                     << "gsp_speed_x\t"
                     //<< "autofocus\t"
                     //<< "field_sync\t"
                     << "tracker_state\t"
                     << "tracker_target\t"
                     << "tracker_x_center\t"
                     << "tracker_y_center\t"
                     << "tracker_x_size\t"
                     << "tracker_y_size\t"
                     << "tracker_z_angle_center\t"
                     << "tracker_x_angle_center\t"
                     << "tracker_z_angle_size\t"
                     << "tracker_x_angle_size\t"
                     << "alignment_pos_x\t"
                     << "alignment_pos_z\t"
                     << "agc_tv_colour_en\t"
                     << "agc_swir_en\t"
                     << "agc_mwir_en\t"
                     << "agc_tv_grey_en\t"
                     << "telemetry_frame_number\t"
                     << std::endl;
    }
}
void TelemetryModel::write_dump_in_txt(Telemetry::t_telemetry_struct * tel)
{
    const auto time = QTime::currentTime().toString().toStdString(); // возвращаем текущее время
    _dump_stream << time
                 << tel->tv.sensor_status << '\t'
                 << tel->tv.lens_status << '\t'
                 << tel->tv.fps << '\t'
                 << tel->tv.vres << '\t'
                 << tel->tv.hres << '\t'
                 << tel->tv.exp << '\t'
                 << tel->tv.zoom_drive << '\t'
                 << tel->tv.focus_drive << '\t'
                 << tel->tv.lens_temp << '\t'
                 << tel->tv.focus_mm << '\t'
                 << tel->tv.field_grad << '\t'
                 << tel->mw.sensor_status << '\t'
                 << tel->mw.lens_status << '\t'
                 << tel->mw.fps << '\t'
                 << tel->mw.vres << '\t'
                 << tel->mw.hres << '\t'
                 << tel->mw.exp << '\t'
                 << tel->mw.zoom_drive << '\t'
                 << tel->mw.focus_drive << '\t'
                 << tel->mw.lens_temp << '\t'
                 << tel->mw.focus_mm << '\t'
                 << tel->mw.field_grad << '\t'
                 << tel->rf.status << '\t'
                 << tel->rf.meas_status << '\t'
                 << tel->rf.last_distance << '\t'
                 << tel->rf.avg_distance << '\t'
                 << tel->rf.rms_distance << '\t'
                 << tel->rf.state << '\t'
                 << tel->rf.body_temp << '\t'
                 << tel->dev.lrf_stat.lrf_RMS <<  '\t'
                 << tel->dev.lrf_stat.lrf_Amp <<  '\t'
                 << tel->dev.lrf_stat.lrf_correction <<  '\t'
                 << tel->dev.lrf_stat.frequency <<  '\t'
                 << tel->dev.lrf_stat.vavd <<  '\t'
                 << tel->gsp.status <<  '\t'
                 << tel->gsp.mode.__raw_ <<  '\t'
                 << tel->gsp.z_drive_pos <<  '\t'
                 << tel->gsp.x_drive_pos <<  '\t'
                 << tel->gsp.z_angle <<  '\t'
                 << tel->gsp.x_angle <<  '\t'
                 << tel->gsp.speed_z <<  '\t'
                 << tel->gsp.speed_x <<  '\t'
                 //<< (static_cast<uint8_t>(tel->controller.autofocus.state ) | static_cast<uint8_t>((tel->controller.autofocus.channel  << 1))) <<  '\t'
                 //<< static_cast<uint8_t>(tel->controller.field_sync.state) | static_cast<uint8_t>((tel->controller.field_sync.channel << 1)) <<  '\t'
                 << tel->controller.tracker_state <<  '\t'
                 << tel->controller.target.x_center <<  '\t'
                 << tel->controller.target.y_center <<  '\t'
                 << tel->controller.target.x_size <<  '\t'
                 << tel->controller.target.y_size <<  '\t'
                 << tel->controller.target.z_angle_center <<  '\t'
                 << tel->controller.target.x_angle_center <<  '\t'
                 << tel->controller.target.z_angle_size <<  '\t'
                 << tel->controller.target.x_angle_size <<  '\t'
                 << tel->controller.alignment_pos_x <<  '\t'
                 << tel->controller.alignment_pos_z <<  '\t'
                 << tel->agc.tv_colour_en <<  '\t'
                 << tel->agc.swir_en <<  '\t'
                 << tel->agc.mwir_en <<  '\t'
                 << tel->agc.tv_grey_en <<  '\t'
                 << tel->dev.frame_number <<  '\t'
                 << std::endl;
}
