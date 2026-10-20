#ifndef GSPCTRLMODEL_H
#define GSPCTRLMODEL_H

#include <fstream>

#include <qmv/qmvmodel.h>

class BooleanValueProperty;
class IntValueProperty;
class DoubleValueProperty;

#include "device_parts/cmd/telemetry.h"
#include "device_parts/gsp/gspcontroller.h"
#include "dev_types.h"

class GSPCtrlModel : public QMVModel
{
    Q_OBJECT
public:
    explicit GSPCtrlModel(DEV_TYPES dev_type, QObject * parent = nullptr, uint32_t id = 0, QString name = "");
    enum VAL_ID {
        id_status,
        id_mode,
        id_st_mechanical_locking,
        id_st_electronic_locking,
        id_st_free_locking,
        id_st_gyrostabilization,
        id_st_zero,
        id_st_hike,
        id_st_pilot,
        id_st_angle_control,
        id_st_park,
        id_x_drive_pos,
        id_x_angle,
        id_x_speed,
        id_z_drive_pos,
        id_z_angle,
        id_z_speed,
        id_alignment_pos_y,
        id_alignment_pos_z,
        id_drift_x,
        id_drift_z,

        id_angle_enable,
        id_x_angle_set,
        id_z_angle_set,
        id_x_speed_f_set,
        id_z_speed_f_set,
        id_angle_discrepancy_x,
        id_angle_discrepancy_z,

        // АС
        id_x_Kp,
        id_x_Ki,
        id_x_Kd,
        id_x_integral_limit,
        id_z_Kp,
        id_z_Ki,
        id_z_Kd,
        id_z_integral_limit,
        id_pid_out_limit_z,
        id_pid_out_limit_x,
        // Фильтры АС
        id_target_filter_k_x,
        id_target_filter_k_y,
        id_tr_type_no_filter,
        id_tr_type_exp_filter,
        id_tr_type_kalman_filter,
        id_tr_type_lsm_filter,
        id_tr_speed_limit_en,

        id_new_angle_x,
        id_new_angle_z,

        // Speed calculation
        id_measure_speed,
        id_measure_speed_n,
        id_measure_speed_n0,
        id_measure_speed_nt,
        id_measure_speed_angle0,
        id_measure_speed_anglet,
        id_x_speed_set,
        id_z_speed_set,

        // Raw status
        id_bits,
        id_mech_arr_x,
        id_mech_arr_z,
        id_mech_arr_f,
        id_stab_x,
        id_stab_z,
        id_park_x,
        id_park_z,
        id_zero_x,
        id_zero_z,
        id_el_arr_x,
        id_el_arr_z,
        id_zero_pos,
        id_pilot_pos,
        id_pohod_pos,
        id_can_error,
        id_adc_bit,
        id_angle_is_set_x,
        id_angle_is_set_z,
        id_diagnostics,
        id_drift_en,
        id_drift_mode,
        id_ctrl_mode,
        id_fsm_mode_z,
        id_fsm_mode_x,
        id_park_f_x,
        id_park_f_z,
        id_diagnostic_res_z,
        id_diagnostic_res_x,

        // Diagnostic
        id_bits_selftest,
        id_timeout_err_x,
        id_el_arr_err_x,
        id_stab_err_x,
        id_el_arr_zero_err_x,
        id_stab_angle_err_x,
        id_timeout_err_z,
        id_el_arr_err_z,
        id_stab_err_z,
        id_el_arr_zero_err_z,
        id_stab_angle_err_z
    };
    enum CMD_ID {
        id_up_start,
        id_down_start,
        id_left_start,
        id_right_start,
        id_stop,
        id_angle_abs,
        id_angle_rel,
        id_speed_f_set,
        id_speed_f_stop,
        id_arretir_reset,
        id_stabilization,
        id_park,
        id_arretir_set,
        id_null_position,
        id_pohod_position,
        id_polyot_position,
        id_alignment,
        id_x_arretir_off,
        id_z_arretir_off,
        id_all_arretir_off,
        id_x_arretir_on,
        id_z_arretir_on,
        id_all_arretir_on,
        id_fine_arretir_reset,
        id_fine_arretir_set,
        // АС
        id_target_filter_ctrl,
        id_angle_filter_ctrl,

        id_set_new_angle_x,
        id_set_new_angle_z,
        id_set_new_angles,
        //
        id_speed_set,
        id_speed_calc
    };
    enum AXIS {
        en_axis_x = 0,
        en_axis_z = 1
    };
    void set(const uint32_t &);
    void get(const uint32_t &);
    void cmd(const uint32_t &);

    void set_gsp_controller(GSPController* gsp_controller) {_gsp_controller = gsp_controller;}

    void update();

signals:
    void gsp_clear_speed_set(const double &, const double &);
    void gsp_speed_set(const double &, const double &);
    void gsp_set_drift_compensation_z(const int16_t& speed);
    void gsp_set_drift_compensation_x(const int16_t& speed);
    void gsp_absolute_angle_set(const double &, const double &);
    void gsp_relative_angle_set(const double &, const double &);
    void gsp_speed_f_set(const double &, const double &);
    void gsp_arretir_set();
    void gsp_arretir_reset();
    void gsp_stabilization_switch();
    void gsp_park();
    void gsp_null_position();
    void gsp_pohod_position();
    void gsp_polyot_position();
    void gsp_alignment();
    void gsp_angle_ctrl_mode();
    void gsp_velocity_ctrl_mode();

    void vdb_read_request(const uint8_t dst_board, const uint8_t dst_reg, const uint16_t length);
    void vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word);
    void vdb_write_request_array(const uint8_t dst_board, const uint8_t dst_reg, const std::vector<uint8_t>& data);

    void signal_send_msg(Packet& req);

    void gsp_drift(const int32_t & x_drift, const int32_t & z_drift);

public slots:
    void response(Cmd);
    void response(Telemetry);

    void gsp_host_etld_info(bool state);

    void set_inverse_x(bool flag);
    void set_inverse_z(bool flag);

private:

    void gsp_drift_z(const int32_t & z_drift);
    void gsp_drift_x(const int32_t & x_drift);

    void write_dump(const bool& enable);
    void open_dump_file();
    void close_dump_file();
    void write_dump_header();
    void write_dump_in_txt(Telemetry::t_telemetry_struct* tel, uint32_t &tel_cnt);

    // Speed measure
    void speed_measure_start(uint8_t x_or_z);
    void speed_measure_stop(void);
    void speed_measure_run(void);

    void gsp_response_x_Kp(const double &);
    void gsp_response_x_Ki(const double &);
    void gsp_response_x_Kd(const double &);
    void gsp_response_x_integral_limit(const double &);

    void gsp_response_z_Kp(const double &);
    void gsp_response_z_Ki(const double &);
    void gsp_response_z_Kd(const double &);
    void gsp_response_z_integral_limit(const double &);

    void gsp_set_x_Kp(const double &);
    void gsp_set_x_Ki(const double &);
    void gsp_set_x_Kd(const double &);
    void gsp_set_x_integral_limit(const double &);
    void gsp_set_pid_out_limit_x(const double &);

    void gsp_set_z_Kp(const double &);
    void gsp_set_z_Ki(const double &);
    void gsp_set_z_Kd(const double &);
    void gsp_set_z_integral_limit(const double &);
    void gsp_set_pid_out_limit_z(const double &);

    void gsp_get_x_Kp();
    void gsp_get_x_Ki();
    void gsp_get_x_Kd();
    void gsp_get_x_integral_limit();

    void gsp_get_z_Kp();
    void gsp_get_z_Ki();
    void gsp_get_z_Kd();
    void gsp_get_z_integral_limit();

    void gsp_set_target_filter_k_x(const double &);
    void gsp_set_target_filter_k_y(const double &);

    void gsp_get_target_filter_k_x();
    void gsp_get_target_filter_k_y();

    void gsp_target_filter_switch();
    void gsp_angle_filter_switch();

    void switch_tracker_mode(const uint8_t &);
    void switch_tracker_speed_limit_mode(const uint8_t &);

private:
    const DEV_TYPES _dev_type;

    IntValueProperty * status;
    IntValueProperty * mode;

    BooleanValueProperty * st_mechanical_locking;
    BooleanValueProperty * st_electronic_locking;
    BooleanValueProperty * st_free_locking;
    BooleanValueProperty * st_gyrostabilization;
    BooleanValueProperty * st_zero;
    BooleanValueProperty * st_hike;
    BooleanValueProperty * st_pilot;
    BooleanValueProperty * st_angle_control;
    BooleanValueProperty * st_park;

    IntValueProperty * x_drive_pos;
    DoubleValueProperty * x_angle;
    IntValueProperty * x_speed;
    IntValueProperty * z_drive_pos;
    DoubleValueProperty * z_angle;
    IntValueProperty * z_speed;
    IntValueProperty * alignment_pos_y;
    IntValueProperty * alignment_pos_z;
    IntValueProperty * drift_x;
    IntValueProperty * drift_z;

    BooleanValueProperty * angle_enable;
    DoubleValueProperty * x_angle_set;
    DoubleValueProperty * z_angle_set;
    DoubleValueProperty * x_speed_f_set;
    DoubleValueProperty * z_speed_f_set;
    DoubleValueProperty * angle_discrepancy_x;
    DoubleValueProperty * angle_discrepancy_z;

    CmdProperty * up_start;
    CmdProperty * down_start;
    CmdProperty * left_start;
    CmdProperty * right_start;
    CmdProperty * stop;
    CmdProperty * angle_abs;
    CmdProperty * angle_rel;
    CmdProperty * speed_f_set;
    CmdProperty * speed_f_stop;
    CmdProperty * x_arretir_off;
    CmdProperty * z_arretir_off;
    CmdProperty * arretir_reset;
    CmdProperty * stabilization;
    CmdProperty * park;
    CmdProperty * arretir_set;
    CmdProperty * null_position;
    CmdProperty * pohod_position;
    CmdProperty * polyot_position;
    CmdProperty * alignment;
    CmdProperty * all_arretir_off;
    CmdProperty * x_arretir_on;
    CmdProperty * z_arretir_on;
    CmdProperty * all_arretir_on;
    CmdProperty * fine_arretir_reset;
    CmdProperty * fine_arretir_set;

    //------------------АС---------------------/
    // ПИД
    DoubleValueProperty * x_Kp;
    DoubleValueProperty * x_Ki;
    DoubleValueProperty * x_Kd;
    DoubleValueProperty * x_integral_limit;
    DoubleValueProperty * z_Kp;
    DoubleValueProperty * z_Ki;
    DoubleValueProperty * z_Kd;
    DoubleValueProperty * z_integral_limit;
    DoubleValueProperty * pid_out_limit_z;
    DoubleValueProperty * pid_out_limit_x;
    // Фильтры
    DoubleValueProperty * target_filter_k_x;
    DoubleValueProperty * target_filter_k_y;
    CmdProperty * target_filter_ctrl;
    CmdProperty * angle_filter_ctrl;
    BooleanValueProperty * tr_type_no_filter;
    BooleanValueProperty * tr_type_exp_filter;
    BooleanValueProperty * tr_type_kalman_filter;
    BooleanValueProperty * tr_type_lsm_filter;
    BooleanValueProperty * tr_speed_limit_en;

    //
    DoubleValueProperty * new_angle_x;
    DoubleValueProperty * new_angle_z;
    CmdProperty * set_new_angle_x;
    CmdProperty * set_new_angle_z;
    CmdProperty * set_new_angles;

    // Speed calculation
    BooleanValueProperty * measure_speed_chbox;
    DoubleValueProperty  * measure_speed_n;
    DoubleValueProperty  * measure_speed_n0;
    DoubleValueProperty  * measure_speed_nt;
    DoubleValueProperty  * measure_speed_angle0;
    DoubleValueProperty  * measure_speed_anglet;
    DoubleValueProperty  * x_speed_set;
    DoubleValueProperty  * z_speed_set;
    CmdProperty * speed_set;
    CmdProperty * speed_calc;

    bool gsp_off;
    bool gsp_mech_arretir_x;
    bool gsp_mech_arretir_y;
    bool gsp_el_arretir_x;
    bool gsp_el_arretir_y;
    bool gsp_stabilization_x;
    bool gsp_stabilization_y;
    bool gsp_zero_x;
    bool gsp_zero_y;
    bool gsp_park_x;
    bool gsp_park_y;

    // АС
    bool gsp_target_filter_en;
    bool gsp_angle_filter_en;

    // ETLD (external)
    bool _host_etld_en;

    // Speed measure
    bool     _speed_measure_en;
    uint8_t  _axis;
    uint32_t _tel_cnt;
    float    _angle_x;
    float    _angle_z;
    int32_t  _speed_x;
    int32_t  _speed_z;

    double _x_inverted_koef;
    double _z_inverted_koef;

    // Write dump
    bool _write_dump;
    std::ofstream _dump_stream;

    GSPController* _gsp_controller;

    // Raw status
    IntValueProperty     * bits;
    BooleanValueProperty * mech_arr_x;
    BooleanValueProperty * mech_arr_z;
    BooleanValueProperty * mech_arr_f;
    BooleanValueProperty * stab_x;
    BooleanValueProperty * stab_z;
    BooleanValueProperty * park_x;
    BooleanValueProperty * park_z;
    BooleanValueProperty * zero_x;
    BooleanValueProperty * zero_z;
    BooleanValueProperty * el_arr_x;
    BooleanValueProperty * el_arr_z;
    BooleanValueProperty * zero_pos;
    BooleanValueProperty * pilot_pos;
    BooleanValueProperty * pohod_pos;
    BooleanValueProperty * can_error;
    BooleanValueProperty * adc_bit;
    BooleanValueProperty * angle_is_set_x;
    BooleanValueProperty * angle_is_set_z;
    BooleanValueProperty * diagnostics;
    BooleanValueProperty * drift_en;
    BooleanValueProperty * drift_mode;
    BooleanValueProperty * ctrl_mode;
    BooleanValueProperty * fsm_mode_z;
    BooleanValueProperty * fsm_mode_x;
    BooleanValueProperty * park_f_x;
    BooleanValueProperty * park_f_z;
    BooleanValueProperty * diagnostic_res_z;
    BooleanValueProperty * diagnostic_res_x;

    // Diagnostic
    IntValueProperty     * bits_selftest;
    BooleanValueProperty * timeout_err_x;
    BooleanValueProperty * el_arr_err_x;
    BooleanValueProperty * stab_err_x;
    BooleanValueProperty * el_arr_zero_err_x;
    BooleanValueProperty * stab_angle_err_x;
    BooleanValueProperty * timeout_err_z;
    BooleanValueProperty * el_arr_err_z;
    BooleanValueProperty * stab_err_z;
    BooleanValueProperty * el_arr_zero_err_z;
    BooleanValueProperty * stab_angle_err_z;
};

#endif // GSPCTRLMODEL_H
