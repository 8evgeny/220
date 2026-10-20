#ifndef MWIRCTRLMODEL_H
#define MWIRCTRLMODEL_H

#include <qmv/qmvmodel.h>
class BooleanValueProperty;
class IntValueProperty;
class DoubleValueProperty;
class AutofocusModel;

#include "device_parts/cmd/telemetry.h"

class MWIRCtrlModel : public QMVModel
{
    Q_OBJECT
public:
    MWIRCtrlModel(QObject * parent = nullptr, uint32_t id = 0, QString name = "");

    enum VAL_ID {
        // TAB1
        id_sensor_status,
        id_lens_status,
        id_fps,
        id_vres,
        id_hres,
        id_exp,
        id_zoom_drive,
        id_focus_drive,
        id_lens_temp,
        id_focus_mm,
        id_field_grad,
        id_zoom,
        id_focus,
        id_autofocus_lens_en,
        id_autofocus_host_en,
        // TAB2
        id_sensor_ssettings_telemetry_enable,
        id_sensor_ssettings_soft_version,
        id_sensor_ssettings_telemetry_address,
        id_sensor_msettings_cce_input,
        id_sensor_msettings_cce_min,
        id_sensor_msettings_cce_max,
        id_sensor_msettings_pattern,
        id_sensor_msettings_direction_video,
        id_sensor_msettings_black_white_video,
        id_sensor_msettings_gpol,
        id_sensor_msettings_period_time,
        id_sensor_msettings_hl_0,
        id_sensor_msettings_hl_1,
        id_sensor_msettings_hl_2,
        id_sensor_msettings_hl_3,
        id_sensor_msettings_hl_4,
        id_sensor_msettings_serial_number,
        id_sensor_dde_enablem,
        id_sensor_dde_base_mode,
        id_sensor_dde_detail_gain,
        id_sensor_dde_detail_eps_normal,
        id_sensor_dde_hist_background,
        id_sensor_dde_brightness,
        id_sensor_dde_alpha_address,
        id_sensor_dde_contrast,
        id_sensor_dde_inversion,
        id_sensor_snr_enable,
        id_sensor_snr_col_eps,
        id_sensor_snr_row_eps,
        id_sensor_nuc_calibration_enable,
        id_sensor_nuc_bad_pixel_replace_enable,
        id_sensor_nuc_limit_level_out_data,
        id_sensor_nuc_lens_position,
        id_sensor_bad_pixel_rep_enable,
        id_sensor_bad_pixel_rep_treshold,
        id_sensor_telemetry_enable,
        id_sensor_telemetry_nuc_enable,
        id_sensor_telemetry_snr_enable,
        id_sensor_telemetry_dde_enable,
        id_sensor_telemetry_bpr_enable,
        id_sensor_telemetry_temperature_lens,
        id_sensor_telemetry_temperature_matrix
    };
    enum CMD_ID {
        id_thermostat_on,
        id_thermostat_off,
        id_heat_on,
        id_heat_off,
        id_agc,
        id_inversion,
        id_autofocus,
        id_field_sync,
        id_field_sync_narrow,
        id_field_sync_reg,
        id_zoom_in_start,
        id_zoom_in_stop,
        id_zoom_out_start,
        id_zoom_out_stop,
        id_iris_in_start,
        id_iris_in_stop,
        id_iris_out_start,
        id_iris_out_stop,
        id_focus_near_start,
        id_focus_near_stop,
        id_focus_far_start,
        id_focus_far_stop,
        id_sensor_ssettings_save_to_flash,
        id_sensor_ssettings_load_from_flash,
        id_sensor_ssettings_initialization_value
    };

    void set(const uint32_t &);
    void get(const uint32_t &);
    void cmd(const uint32_t &);

public slots:
    void response(Cmd);
    void response(Telemetry);
    void set_autofocus_en_ui();
    void set_autofocus_dis_ui();
    void set_autofocus_host_pause(bool pause);

signals:
    void agc_switch(const uint32_t & channel_id);
    void inversion_mw();
    void field_sync_narrow(const uint32_t & channel_id);
    void field_sync(const uint32_t & channel_id);
    void field_sync_reg_switch(const uint32_t & channel_id);
    void autofocus(const uint32_t & channel_id);
    void autofocus_en(const uint32_t & channel_id);
    void autofocus_dis(const uint32_t & channel_id);
    void zoom_in(const double & speed, const uint32_t & channel_id);
    void zoom_out(const double & speed, const uint32_t & channel_id);
    void zoom_stop(const uint32_t & channel_id);
    void zoom_pos(const uint32_t & pos, const uint32_t & channel_id);
    void focus_near(const double & speed, const uint32_t & channel_id);
    void focus_far(const double & speed, const uint32_t & channel_id);
    void focus_stop(const uint32_t & channel_id);
    void focus_pos(const uint32_t & pos, const uint32_t & channel_id);
    void iris_close(const double & speed, const uint32_t & channel_id);
    void iris_open(const double & speed, const uint32_t & channel_id);
    void iris_stop(const uint32_t & channel_id);
    void thermostat_on(const uint32_t & channel_id);
    void thermostat_off(const uint32_t & channel_id);
    void heat_on(const uint32_t & channel_id);
    void heat_off(const uint32_t & channel_id);

    void vdb_read_request(const uint8_t dst_board, const uint8_t dst_reg, const uint16_t length);
    void vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word);
    void vdb_write_request_array(const uint8_t dst_board, const uint8_t dst_reg, const std::vector<uint8_t>& data);

private:
    // TAB1
    IntValueProperty * _sensor_status;
    IntValueProperty * _lens_status;
    IntValueProperty * _fps;
    IntValueProperty * _vres;
    IntValueProperty * _hres;
    IntValueProperty * _exp;
    IntValueProperty * _zoom_drive;
    IntValueProperty * _focus_drive;
    IntValueProperty * _lens_temp;
    DoubleValueProperty * _focus_mm;
    DoubleValueProperty * _field_grad;
    IntValueProperty * _zoom;
    IntValueProperty * _focus;
    BooleanValueProperty * _autofocus_lens_en;
    BooleanValueProperty * _autofocus_host_en;
    CmdProperty * _thermostat_on;
    CmdProperty * _thermostat_off;
    CmdProperty * _heat_on;
    CmdProperty * _heat_off;
    CmdProperty * _agc;
    CmdProperty * _inversion;
    CmdProperty * _autofocus;
    CmdProperty * _field_sync;
    CmdProperty * _field_sync_narrow;
    CmdProperty * _field_sync_reg;
    CmdProperty * _zoom_in_start;
    CmdProperty * _zoom_in_stop;
    CmdProperty * _zoom_out_start;
    CmdProperty * _zoom_out_stop;
    CmdProperty * _iris_in_start;
    CmdProperty * _iris_in_stop;
    CmdProperty * _iris_out_start;
    CmdProperty * _iris_out_stop;
    CmdProperty * _focus_near_start;
    CmdProperty * _focus_near_stop;
    CmdProperty * _focus_far_start;
    CmdProperty * _focus_far_stop;
    bool _agc_on;
    bool _inversion_on;
    bool _autofocus_on;
    bool _autofocus_host_pause;
    bool _autofocus_host_last_state;
    bool _field_sync_reg_on;
    AutofocusModel * _af_model;

    // TAB2
    // System settings
    BooleanValueProperty* _sensor_ssettings_telemetry_enable;
    IntValueProperty*     _sensor_ssettings_soft_version;
    IntValueProperty*     _sensor_ssettings_telemetry_address;
    // Matrix settings
    IntValueProperty*     _sensor_msettings_cce_input;
    IntValueProperty*     _sensor_msettings_cce_min;
    IntValueProperty*     _sensor_msettings_cce_max;
    IntValueProperty*     _sensor_msettings_pattern;
    BooleanValueProperty* _sensor_msettings_direction_video;
    BooleanValueProperty* _sensor_msettings_black_white_video;
    IntValueProperty*     _sensor_msettings_gpol;
    IntValueProperty*     _sensor_msettings_period_time;
    IntValueProperty*     _sensor_msettings_hl_0;
    IntValueProperty*     _sensor_msettings_hl_1;
    IntValueProperty*     _sensor_msettings_hl_2;
    IntValueProperty*     _sensor_msettings_hl_3;
    IntValueProperty*     _sensor_msettings_hl_4;
    IntValueProperty*     _sensor_msettings_serial_number;
    // DDE
    BooleanValueProperty* _sensor_dde_enable;
    IntValueProperty*     _sensor_dde_base_mode;
    IntValueProperty*     _sensor_dde_detail_gain;
    IntValueProperty*     _sensor_dde_detail_eps_normal;
    IntValueProperty*     _sensor_dde_hist_background;
    IntValueProperty*     _sensor_dde_brightness;
    IntValueProperty*     _sensor_dde_alpha_address;
    IntValueProperty*     _sensor_dde_contrast;
    IntValueProperty*     _sensor_dde_inversion;
    // SNR
    BooleanValueProperty* _sensor_snr_enable;
    IntValueProperty*     _sensor_snr_col_eps;
    IntValueProperty*     _sensor_snr_row_eps;
    // NUC
    BooleanValueProperty* _sensor_nuc_calibration_enable;
    BooleanValueProperty* _sensor_nuc_bad_pixel_replace_enable;
    IntValueProperty*     _sensor_nuc_limit_level_out_data;
    IntValueProperty*     _sensor_nuc_lens_position;
         // Bad pixel
    BooleanValueProperty* _sensor_bad_pixel_rep_enable;
    IntValueProperty*     _sensor_bad_pixel_rep_treshold;

    BooleanValueProperty* _sensor_telemetry_enable;
    BooleanValueProperty* _sensor_telemetry_nuc_enable;
    BooleanValueProperty* _sensor_telemetry_snr_enable;
    BooleanValueProperty* _sensor_telemetry_dde_enable;
    BooleanValueProperty* _sensor_telemetry_bpr_enable;
    IntValueProperty*     _sensor_telemetry_temperature_lens;
    IntValueProperty*     _sensor_telemetry_temperature_matrix;

    CmdProperty* _sensor_ssettings_save_to_flash;
    CmdProperty* _sensor_ssettings_load_from_flash;
    CmdProperty* _sensor_ssettings_initialization_value;
};

#endif // MWIRCTRLMODEL_H
