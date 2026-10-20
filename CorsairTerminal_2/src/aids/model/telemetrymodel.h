#ifndef TELEMETRYMODEL_H
#define TELEMETRYMODEL_H

#include <qmv/qmvmodel.h>
#include <map>

#include "dev_types.h"

class BooleanValueProperty;
class IntValueProperty;
class DoubleValueProperty;
class StringValueProperty;

#include "device_parts/cmd/telemetry.h"

#include <iostream>
#include <fstream>

class TelemetryModel : public QMVModel
{
    Q_OBJECT
public:
    explicit TelemetryModel(DEV_TYPES dev_type, QObject * parent = nullptr, uint32_t id = 0, QString name = "");
    virtual ~TelemetryModel();

    enum VAL_ID {
        id_rangefinder_status,
        id_meas_status,
        id_last_distance,
        id_avg_distance,
        id_rms_distance,
        id_laser_temp,
        id_body_temp,

        id_tv_sensor_status,
        id_tv_lens_status,
        id_tv_fps,
        id_tv_vres,
        id_tv_hres,
        id_tv_exp,
        id_tv_zoom_drive,
        id_tv_focus_drive,
        id_tv_lens_temp,
        id_tv_focus_mm,
        id_tv_field_grad,

        id_swir_sensor_status,
        id_swir_lens_status,
        id_swir_fps,
        id_swir_vres,
        id_swir_hres,
        id_swir_exp,
        id_swir_zoom_drive,
        id_swir_focus_drive,
        id_swir_lens_temp,
        id_swir_focus_mm,
        id_swir_field_grad,

        id_mwir_sensor_status,
        id_mwir_lens_status,
        id_mwir_fps,
        id_mwir_vres,
        id_mwir_hres,
        id_mwir_exp,
        id_mwir_zoom_drive,
        id_mwir_focus_drive,
        id_mwir_lens_temp,
        id_mwir_focus_mm,
        id_mwir_field_grad,

        id_gsp_status,
        id_mode,
        id_z_drive_pos,
        id_x_drive_pos,
        id_z_angle,
        id_x_angle,
        id_speed_z,
        id_speed_x,

        id_tv_agc_colour,
        id_tv_agc_grey,
        id_sw_agc,
        id_mw_agc,
        id_inversion_mw,
        id_autofocus,
        id_autofocus_channel,
        id_field_sync,
        id_field_sync_channel,
        id_tracker_channel,
        id_tracker_state,
        id_target,
        id_target_x_center,
        id_target_y_center,
        id_target_x_size,
        id_target_y_size,
        id_target_z_angle_center,
        id_target_x_angle_center,
        id_target_z_angle_size,
        id_target_x_angle_size,
        id_fc0_status,
        id_fc1_status,
        id_eth_status,
        id_com_status,
        id_year,
        id_month,
        id_controller_id,
        id_hard_ver,
        id_soft_ver,
        id_hours,
        id_minutes,
        id_sw_gain,
        id_st_suc_perc,
        id_st_count,
        id_alignment_pos_y,
        id_alignment_pos_z,

        id_lrf_RMS,
        id_lrf_Amp,
        id_tv_bayer_fps,
        id_tv_bayer_hsize,
        id_tv_bayer_vsize,
        id_sw_fps,
        id_sw_hsize,
        id_sw_vsize,
        id_mw_fps,
        id_mw_hsize,
        id_mw_vsize,
        id_tv_mono_fps,
        id_tv_mono_hsize,
        id_tv_mono_vsize,
        id_complex_fps,
        id_complex_hsize,
        id_complex_vsize,

        id_st_in_progress,
        id_st_st_conn_pc_eth,
        id_st_st_conn_server_fc,
        id_st_st_conn_gsp_rs422,
        id_st_st_ready_gsp,
        id_st_st_conn_cpu_eth,
        id_st_st_conn_mobile_seriallite,
        id_st_st_conn_lrf_rs485,
        id_st_st_ready_lrf,
        id_st_st_conn_sensor_tv_uart,
        id_st_st_conn_sensor_tv_camlink,
        id_st_st_conn_lens_tv_rs232,
        id_st_st_ready_tv_image,
        id_st_st_conn_sensor_sw_uart,
        id_st_st_conn_sensor_sw_camlink,
        id_st_st_ready_sw_image,
        id_st_st_conn_lens_sw_rs232,
        id_st_st_conn_sensor_mw_uart,
        id_st_st_conn_sensor_mw_camlink,
        id_st_st_conn_lens_mw_rs232,
        id_st_st_ready_mw_image,
        id_st_st_video_tv_bayer,
        id_st_st_video_tv_mono,
        id_st_st_video_sw,
        id_st_st_video_mw,
        id_st_st_video_fusion,
        id_st_st_drive_tv_zoom,
        id_st_st_drive_tv_focus,
        id_st_st_drive_sw_zoom,
        id_st_st_drive_sw_focus,
        id_st_st_drive_mw_zoom,
        id_st_st_drive_mw_focus,
        id_st_st_drive_gsp_azimuth,
        id_st_st_drive_gsp_elevation,

        // ------------ Для отладки фильтра Калмана -------- //
        id_kalman_coord_z,
        id_kalman_coord_x,
        id_kalman_speed_z,
        id_kalman_speed_x,
        id_kalman_accel_z,
        id_kalman_accel_x,
        id_kalman_delta_tracker_z,
        id_kalman_delta_tracker_x,
        // --------------- Стабильность системы ------------ //
        id_cycle_execution_time_mb,
        id_time_between_cycles_mb,
        id_cycle_execution_time_fb,
        id_time_between_cycles_fb,
        id_fixed_board_counter_tel,
        id_mobile_board_counter_tel,
        id_mobile_board_telemetry_per_sec,
        id_gsp_per_sec,
        id_cpu_per_sec,
        id_adapter_per_sec,
        // ---------------- Additional struct -------------- //
        id_add_gsp_volt,
        id_add_rf_meas_delta_time,
        id_add_rf_cooling_time_cnt,
        id_add_rf_radiation_time_cnt,
        id_add_mw_coarse_af_en,
        id_add_fc_tv_fps,
        id_add_fc_mw_fps,
        id_add_fc_fusion_fps
    };
    enum CMD_ID {
    };
    void set(const uint32_t &);
    void get(const uint32_t &);
    void cmd(const uint32_t &);

public slots:
    void write_dump(const bool& enable);
    void response(Cmd);
    void response(Telemetry);

private:
    void open_dump_file();
    void close_dump_file();
    void write_dump_header();
    void write_dump_in_txt(Telemetry::t_telemetry_struct* tel);
private:
    std::ofstream _dump_stream; // поток для записи
    const DEV_TYPES _dev_type;
    bool _write_dump;
    IntValueProperty * rangefinder_status;
    IntValueProperty * meas_status;
    DoubleValueProperty * last_distance;
    DoubleValueProperty * avg_distance;
    DoubleValueProperty * rms_distance;
    DoubleValueProperty * laser_temp;
    DoubleValueProperty * body_temp;

    IntValueProperty * tv_sensor_status;
    IntValueProperty * tv_lens_status;
    IntValueProperty * tv_fps;
    IntValueProperty * tv_vres;
    IntValueProperty * tv_hres;
    IntValueProperty * tv_exp;
    IntValueProperty * tv_zoom_drive;
    IntValueProperty * tv_focus_drive;
    IntValueProperty * tv_lens_temp;
    DoubleValueProperty * tv_focus_mm;
    DoubleValueProperty * tv_field_grad;

    IntValueProperty * swir_sensor_status;
    IntValueProperty * swir_lens_status;
    IntValueProperty * swir_fps;
    IntValueProperty * swir_vres;
    IntValueProperty * swir_hres;
    IntValueProperty * swir_exp;
    IntValueProperty * swir_zoom_drive;
    IntValueProperty * swir_focus_drive;
    IntValueProperty * swir_lens_temp;
    DoubleValueProperty * swir_focus_mm;
    DoubleValueProperty * swir_field_grad;

    IntValueProperty * mwir_sensor_status;
    IntValueProperty * mwir_lens_status;
    IntValueProperty * mwir_fps;
    IntValueProperty * mwir_vres;
    IntValueProperty * mwir_hres;
    IntValueProperty * mwir_exp;
    IntValueProperty * mwir_zoom_drive;
    IntValueProperty * mwir_focus_drive;
    IntValueProperty * mwir_lens_temp;
    DoubleValueProperty * mwir_focus_mm;
    DoubleValueProperty * mwir_field_grad;

    IntValueProperty * gsp_status;
    IntValueProperty * mode;
    IntValueProperty * z_drive_pos;
    IntValueProperty * x_drive_pos;
    DoubleValueProperty * z_angle;
    DoubleValueProperty * x_angle;
    IntValueProperty * speed_z;
    IntValueProperty * speed_x;

    IntValueProperty * tv_agc_colour;
    IntValueProperty * tv_agc_grey;
    IntValueProperty * sw_agc;
    IntValueProperty * mw_agc;
    IntValueProperty * inversion_mw;
    IntValueProperty * autofocus;
    StringValueProperty * autofocus_channel;
    IntValueProperty * field_sync;
    StringValueProperty * field_sync_channel;
    IntValueProperty * tracker_channel;
    IntValueProperty * tracker_state;
    IntValueProperty * target;
    IntValueProperty * target_x_center;
    IntValueProperty * target_y_center;
    IntValueProperty * target_x_size;
    IntValueProperty * target_y_size;
    DoubleValueProperty * target_z_angle_center;
    DoubleValueProperty * target_x_angle_center;
    DoubleValueProperty * target_z_angle_size;
    DoubleValueProperty * target_x_angle_size;
    IntValueProperty * fc0_status;
    IntValueProperty * fc1_status;
    IntValueProperty * eth_status;
    IntValueProperty * com_status;
    IntValueProperty * year;
    IntValueProperty * month;
    IntValueProperty * controller_id;
    IntValueProperty * hard_ver;
    IntValueProperty * soft_ver;
    IntValueProperty * hours;
    IntValueProperty * minutes;
    IntValueProperty * sw_gain;
    DoubleValueProperty * st_suc_perc;
    IntValueProperty * st_count;
    IntValueProperty * alignment_pos_y;
    IntValueProperty * alignment_pos_z;

    IntValueProperty * lrf_RMS;
    IntValueProperty * lrf_Amp;
    IntValueProperty * tv_bayer_fps;
    IntValueProperty * tv_bayer_hsize;
    IntValueProperty * tv_bayer_vsize;
    IntValueProperty * sw_fps;
    IntValueProperty * sw_hsize;
    IntValueProperty * sw_vsize;
    IntValueProperty * mw_fps;
    IntValueProperty * mw_hsize;
    IntValueProperty * mw_vsize;
    IntValueProperty * tv_mono_fps;
    IntValueProperty * tv_mono_hsize;
    IntValueProperty * tv_mono_vsize;
    IntValueProperty * complex_fps;
    IntValueProperty * complex_hsize;
    IntValueProperty * complex_vsize;

    BooleanValueProperty * st_in_progress;
    BooleanValueProperty * st_conn_pc_eth;
    BooleanValueProperty * st_conn_server_fc;
    BooleanValueProperty * st_conn_gsp_rs422;
    BooleanValueProperty * st_ready_gsp;
    BooleanValueProperty * st_conn_cpu_eth;
    BooleanValueProperty * st_conn_mobile_seriallite;
    BooleanValueProperty * st_conn_lrf_rs485;
    BooleanValueProperty * st_ready_lrf;
    BooleanValueProperty * st_conn_sensor_tv_uart;
    BooleanValueProperty * st_conn_sensor_tv_camlink;
    BooleanValueProperty * st_conn_lens_tv_rs232;
    BooleanValueProperty * st_ready_tv_image;
    BooleanValueProperty * st_conn_sensor_sw_uart;
    BooleanValueProperty * st_conn_sensor_sw_camlink;
    BooleanValueProperty * st_conn_lens_sw_rs232;
    BooleanValueProperty * st_ready_sw_image;
    BooleanValueProperty * st_conn_sensor_mw_uart;
    BooleanValueProperty * st_conn_sensor_mw_camlink;
    BooleanValueProperty * st_conn_lens_mw_rs232;
    BooleanValueProperty * st_ready_mw_image;
    BooleanValueProperty * st_video_tv_bayer;
    BooleanValueProperty * st_video_tv_mono;
    BooleanValueProperty * st_video_sw;
    BooleanValueProperty * st_video_mw;
    BooleanValueProperty * st_video_fusion;
    BooleanValueProperty * st_drive_tv_zoom;
    BooleanValueProperty * st_drive_tv_focus;
    BooleanValueProperty * st_drive_sw_zoom;
    BooleanValueProperty * st_drive_sw_focus;
    BooleanValueProperty * st_drive_mw_zoom;
    BooleanValueProperty * st_drive_mw_focus;
    BooleanValueProperty * st_drive_gsp_azimuth;
    BooleanValueProperty * st_drive_gsp_elevation;

    // ----------- Для отладки фильтра Калмана -------------//
    DoubleValueProperty * kalman_coord_z;
    DoubleValueProperty * kalman_coord_x;
    DoubleValueProperty * kalman_speed_z;
    DoubleValueProperty * kalman_speed_x;
    DoubleValueProperty * kalman_accel_z;
    DoubleValueProperty * kalman_accel_x;
    DoubleValueProperty * kalman_delta_tracker_z;
    DoubleValueProperty * kalman_delta_tracker_x;

    // --------------- Стабильность системы --------------- //
    IntValueProperty * cycle_execution_time_mb;
    IntValueProperty * time_between_cycles_mb;
    IntValueProperty * cycle_execution_time_fb;
    IntValueProperty * time_between_cycles_fb;
    IntValueProperty * fixed_board_counter_tel;
    IntValueProperty * mobile_board_counter_tel;
    IntValueProperty * mobile_board_telemetry_per_sec;
    IntValueProperty * gsp_per_sec;
    IntValueProperty * cpu_per_sec;
    IntValueProperty * adapter_per_sec;

    // ---------------- Additional struct ---------------- //
    IntValueProperty     * add_gsp_volt;
    IntValueProperty     * add_rf_meas_delta_time;
    IntValueProperty     * add_rf_cooling_time_cnt;
    IntValueProperty     * add_rf_radiation_time_cnt;
    BooleanValueProperty * add_mw_coarse_af_en;
    IntValueProperty     * add_fc_tv_fps;
    IntValueProperty     * add_fc_mw_fps;
    IntValueProperty     * add_fc_fusion_fps;

    std::map<Telemetry::camera_codes, QString> camera_code_map = {{Telemetry::tv_camera, QString("TB")}, {Telemetry::sw_camera, QString("КВИК")}, {Telemetry::mw_camera, QString("TПB")}, {Telemetry::disable, QString("Выкл.")}};
};

#endif // TELEMETRYMODEL_H
