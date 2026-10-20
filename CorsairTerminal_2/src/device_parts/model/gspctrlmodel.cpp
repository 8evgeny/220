#include "gspctrlmodel.h"

#include <QDateTime>
#include <QVariant>
#include <qmv/property/booleanvalueproperty.h>
#include <qmv/property/doublevalueproperty.h>
#include <qmv/property/intvalueproperty.h>
#include <qmv/property/cmdproperty.h>
#include "device_parts/channel/abstractchannel.h"
#include "device_parts/cmd/vdb_registers.h"

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logGspCtrl, "GspCtrl")

typedef enum {
  en_all_ax = 0,
  en_x_ax   = 1,
  en_z_ax   = 2
} en_gsp_axis;

typedef enum {
  en_mode_off = 0,
  en_mode_on  = 1
} en_gsp_mode;

GSPCtrlModel::GSPCtrlModel(DEV_TYPES dev_type, QObject * parent, uint32_t id, QString name) :
    QMVModel(parent, id, name),

    _dev_type(dev_type),
    status(new IntValueProperty(this, id_status, "status", 0, 0, 655350)),
    mode(new IntValueProperty(this, id_mode, "mode", 0, 0, 655350)),
    st_mechanical_locking(new BooleanValueProperty(this, id_st_mechanical_locking, "st_mechanical_locking", false)),
    st_electronic_locking(new BooleanValueProperty(this, id_st_electronic_locking, "st_electronic_locking", false)),
    st_free_locking(new BooleanValueProperty(this, id_st_free_locking, "st_free_locking", false)),
    st_gyrostabilization(new BooleanValueProperty(this, id_st_gyrostabilization, "st_gyrostabilization", false)),
    st_zero(new BooleanValueProperty(this, id_st_zero, "st_zero", false)),
    st_hike(new BooleanValueProperty(this, id_st_hike, "st_hike", false)),
    st_pilot(new BooleanValueProperty(this, id_st_pilot, "st_pilot", false)),
    st_angle_control(new BooleanValueProperty(this, id_st_angle_control, "st_angle_control", false)),
    st_park(new BooleanValueProperty(this, id_st_park, "st_park", false)),
    x_drive_pos(new IntValueProperty(this, id_x_drive_pos, "x_drive_pos", 0, 0, 655350)),
    x_angle(new DoubleValueProperty(this, id_x_angle, "x_angle", 0, -120, 120)),
    x_speed(new IntValueProperty(this, id_x_speed, "x_speed", 0, -655350, 655350)),
    z_drive_pos(new IntValueProperty(this, id_z_drive_pos, "z_drive_pos", 0, 0, 655350)),
    z_angle(new DoubleValueProperty(this, id_z_angle, "z_angle", 0, -180, 180)),
    z_speed(new IntValueProperty(this, id_z_speed, "z_speed", 0, -655350, 655350)),
    alignment_pos_y(new IntValueProperty(this, id_alignment_pos_y, "alignment_pos_y", 0, 0, 655350)),
    alignment_pos_z(new IntValueProperty(this, id_alignment_pos_z, "alignment_pos_z", 0, 0, 655350)),
    drift_x(new IntValueProperty(this, id_drift_x, "drift_x", 0, -200, 200)),
    drift_z(new IntValueProperty(this, id_drift_z, "drift_z", 0, -200, 200)),

    angle_enable(new BooleanValueProperty(this, id_angle_enable, "angle_enable", false)),
    x_angle_set(new DoubleValueProperty(this, id_x_angle_set, "x_angle_set", 0, -180, 180)),
    z_angle_set(new DoubleValueProperty(this, id_z_angle_set, "z_angle_set", 0, -180, 180)),
    x_speed_f_set(new DoubleValueProperty(this, id_x_speed_f_set, "x_speed_f_set", 0, -180.0, 180.0)),
    z_speed_f_set(new DoubleValueProperty(this, id_z_speed_f_set, "z_speed_f_set", 0, -180.0, 180.0)),
    angle_discrepancy_x(new DoubleValueProperty(this, id_angle_discrepancy_x, "angle_discrepancy_x", 0, 0, 10)),
    angle_discrepancy_z(new DoubleValueProperty(this, id_angle_discrepancy_z, "angle_discrepancy_z", 0, 0, 10)),

    up_start(new CmdProperty(this, id_up_start, "up_start")),
    down_start(new CmdProperty(this, id_down_start, "down_start")),
    left_start(new CmdProperty(this, id_left_start, "left_start")),
    right_start(new CmdProperty(this, id_right_start, "right_start")),
    stop(new CmdProperty(this, id_stop, "stop")),
    angle_abs(new CmdProperty(this, id_angle_abs, "angle_abs")),
    angle_rel(new CmdProperty(this, id_angle_rel, "angle_rel")),
    speed_f_set(new CmdProperty(this, id_speed_f_set, "speed_f_set")),
    speed_f_stop(new CmdProperty(this, id_speed_f_stop, "speed_f_stop")),
    arretir_reset(new CmdProperty(this, id_arretir_reset, "arretir_reset")),
    stabilization(new CmdProperty(this, id_stabilization, "stabilization")),
    park(new CmdProperty(this, id_park, "park")),
    arretir_set(new CmdProperty(this, id_arretir_set, "arretir_set")),
    null_position(new CmdProperty(this, id_null_position, "null_position")),
    pohod_position(new CmdProperty(this, id_pohod_position, "pohod_position")),
    polyot_position(new CmdProperty(this, id_polyot_position, "polyot_position")),
    alignment(new CmdProperty(this, id_alignment, "alignment")),
    x_arretir_off     (new CmdProperty(this, id_x_arretir_off,      "x_arretir_off")),
    z_arretir_off     (new CmdProperty(this, id_z_arretir_off,      "z_arretir_off")),
    all_arretir_off   (new CmdProperty(this, id_all_arretir_off,    "all_arretir_off")),
    x_arretir_on      (new CmdProperty(this, id_x_arretir_on,       "x_arretir_on")),
    z_arretir_on      (new CmdProperty(this, id_z_arretir_on,       "z_arretir_on")),
    all_arretir_on    (new CmdProperty(this, id_all_arretir_on,     "all_arretir_on")),
    fine_arretir_reset(new CmdProperty(this, id_fine_arretir_reset, "fine_arretir_reset")),
    fine_arretir_set  (new CmdProperty(this, id_fine_arretir_set,   "fine_arretir_set")),
    //---------------------------------------------АС-------------------------------------------------------------/
    x_Kp(new DoubleValueProperty(this, id_x_Kp, "x_Kp", 600, -655360, 655360)),
    x_Ki(new DoubleValueProperty(this, id_x_Ki, "x_Ki", 24, -655360, 655360)),
    x_Kd(new DoubleValueProperty(this, id_x_Kd, "x_Kd", 3750, -655360, 655360)),
    x_integral_limit(new DoubleValueProperty(this, id_x_integral_limit, "x_integral_limit", 10000, -655360, 655360)),
    z_Kp(new DoubleValueProperty(this, id_z_Kp, "z_Kp", 600, -655360, 655360)),
    z_Ki(new DoubleValueProperty(this, id_z_Ki, "z_Ki", 24, -655360, 655360)),
    z_Kd(new DoubleValueProperty(this, id_z_Kd, "z_Kd", 3750, -655360, 655360)),
    z_integral_limit(new DoubleValueProperty(this, id_z_integral_limit, "z_integral_limit", 10000, -655360, 655360)),
    pid_out_limit_z(new DoubleValueProperty(this, id_pid_out_limit_z, "pid_out_limit_z", 0, -655360, 655360)),
    pid_out_limit_x(new DoubleValueProperty(this, id_pid_out_limit_x, "pid_out_limit_x", 0, -655360, 655360)),
    target_filter_k_x(new DoubleValueProperty(this, id_target_filter_k_x, "target_filter_k_x", 0.0, -1.0, 1.0)),
    target_filter_k_y(new DoubleValueProperty(this, id_target_filter_k_y, "target_filter_k_y", 0.0, -1.0, 1.0)),
    target_filter_ctrl(new CmdProperty(this, id_target_filter_ctrl, "target_filter_ctrl")),
    angle_filter_ctrl(new CmdProperty(this, id_angle_filter_ctrl, "angle_filter_ctrl")),
    tr_type_no_filter(new BooleanValueProperty(this, id_tr_type_no_filter, "tr_type_no_filter", false)),
    tr_type_exp_filter(new BooleanValueProperty(this, id_tr_type_exp_filter, "tr_type_exp_filter", true)),
    tr_type_kalman_filter(new BooleanValueProperty(this, id_tr_type_kalman_filter, "tr_type_kalman_filter", false)),
    tr_type_lsm_filter(new BooleanValueProperty(this, id_tr_type_lsm_filter, "tr_type_lsm_filter", false)),
    tr_speed_limit_en(new BooleanValueProperty(this, id_tr_speed_limit_en, "tr_speed_limit_en", true)),

    new_angle_x(new DoubleValueProperty(this, id_new_angle_x, "new_angle_x", 0.0, -180.0, 180.0)),
    new_angle_z(new DoubleValueProperty(this, id_new_angle_z, "new_angle_z", 0.0, -180.0, 180.0)),
    set_new_angle_x(new CmdProperty(this, id_set_new_angle_x, "set_new_angle_x")),
    set_new_angle_z(new CmdProperty(this, id_set_new_angle_z, "set_new_angle_z")),
    set_new_angles (new CmdProperty(this, id_set_new_angles , "set_new_angles")),

    // Speed calculation
    measure_speed_chbox (new BooleanValueProperty(this, id_measure_speed,        "measure_speed",        false)),
    measure_speed_n     (new DoubleValueProperty (this, id_measure_speed_n,      "measure_speed_n",      0,      0, 0xFFFFFFFF)),
    measure_speed_n0    (new DoubleValueProperty (this, id_measure_speed_n0,     "measure_speed_n0",     0,      0, 0xFFFFFFFF)),
    measure_speed_nt    (new DoubleValueProperty (this, id_measure_speed_nt,     "measure_speed_nt",     0,      0, 0xFFFFFFFF)),
    measure_speed_angle0(new DoubleValueProperty (this, id_measure_speed_angle0, "measure_speed_angle0", 0, -360.0,      360.0)),
    measure_speed_anglet(new DoubleValueProperty (this, id_measure_speed_anglet, "measure_speed_anglet", 0, -360.0,      360.0)),
    x_speed_set         (new DoubleValueProperty (this, id_x_speed_set,          "x_speed_set",          0, -100.0,      100.0)),
    z_speed_set         (new DoubleValueProperty (this, id_z_speed_set,          "z_speed_set",          0, -100.0,      100.0)),
    speed_set           (new CmdProperty(this, id_speed_set,  "speed_set")),
    speed_calc          (new CmdProperty(this, id_speed_calc, "speed_calc")),

    // Raw status
    bits          (new IntValueProperty    (this, id_bits,           "bits",           0, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())),
    mech_arr_x    (new BooleanValueProperty(this, id_mech_arr_x,     "mech_arr_x",     false)),
    mech_arr_z    (new BooleanValueProperty(this, id_mech_arr_z,     "mech_arr_z",     false)),
    mech_arr_f    (new BooleanValueProperty(this, id_mech_arr_f,     "mech_arr_f",     false)),
    stab_x        (new BooleanValueProperty(this, id_stab_x,         "stab_x",         false)),
    stab_z        (new BooleanValueProperty(this, id_stab_z,         "stab_z",         false)),
    park_x        (new BooleanValueProperty(this, id_park_x,         "park_x",         false)),
    park_z        (new BooleanValueProperty(this, id_park_z,         "park_z",         false)),
    zero_x        (new BooleanValueProperty(this, id_zero_x,         "zero_x",         false)),
    zero_z        (new BooleanValueProperty(this, id_zero_z,         "zero_z",         false)),
    el_arr_x      (new BooleanValueProperty(this, id_el_arr_x,       "el_arr_x",       false)),
    el_arr_z      (new BooleanValueProperty(this, id_el_arr_z,       "el_arr_z",       false)),
    zero_pos      (new BooleanValueProperty(this, id_zero_pos,       "zero_pos",       false)),
    pilot_pos     (new BooleanValueProperty(this, id_pilot_pos,      "pilot_pos",      false)),
    pohod_pos     (new BooleanValueProperty(this, id_pohod_pos,      "pohod_pos",      false)),
    can_error     (new BooleanValueProperty(this, id_can_error,      "can_error",      false)),
    adc_bit       (new BooleanValueProperty(this, id_adc_bit,        "adc_bit",        false)),
    angle_is_set_x(new BooleanValueProperty(this, id_angle_is_set_x, "angle_is_set_x", false)),
    angle_is_set_z(new BooleanValueProperty(this, id_angle_is_set_z, "angle_is_set_z", false)),
    diagnostics   (new BooleanValueProperty(this, id_diagnostics,    "diagnostics",    false)),
    drift_en      (new BooleanValueProperty(this, id_drift_en,       "drift_en",       false)),
    drift_mode    (new BooleanValueProperty(this, id_drift_mode,     "drift_mode",     false)),
    ctrl_mode     (new BooleanValueProperty(this, id_ctrl_mode,      "ctrl_mode",      false)),
    fsm_mode_z    (new BooleanValueProperty(this, id_fsm_mode_z,     "fsm_mode_z",     false)),
    fsm_mode_x    (new BooleanValueProperty(this, id_fsm_mode_x,     "fsm_mode_x",     false)),
    park_f_x      (new BooleanValueProperty(this, id_park_f_x,       "park_f_x",       false)),
    park_f_z      (new BooleanValueProperty(this, id_park_f_z,       "park_f_z",       false)),
    diagnostic_res_z(new BooleanValueProperty(this, id_diagnostic_res_z, "diagnostic_res_z", false)),
    diagnostic_res_x(new BooleanValueProperty(this, id_diagnostic_res_x, "diagnostic_res_x", false)),

    // Diagnostic
    bits_selftest    (new IntValueProperty    (this, id_bits_selftest,     "bits_selftest",     0, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())),
    timeout_err_x    (new BooleanValueProperty(this, id_timeout_err_x,     "timeout_err_x",     false)),
    el_arr_err_x     (new BooleanValueProperty(this, id_el_arr_err_x,      "el_arr_err_x",      false)),
    stab_err_x       (new BooleanValueProperty(this, id_stab_err_x,        "stab_err_x",        false)),
    el_arr_zero_err_x(new BooleanValueProperty(this, id_el_arr_zero_err_x, "el_arr_zero_err_x", false)),
    stab_angle_err_x (new BooleanValueProperty(this, id_stab_angle_err_x,  "stab_angle_err_x",  false)),
    timeout_err_z    (new BooleanValueProperty(this, id_timeout_err_z,     "timeout_err_z",     false)),
    el_arr_err_z     (new BooleanValueProperty(this, id_el_arr_err_z,      "el_arr_err_z",      false)),
    stab_err_z       (new BooleanValueProperty(this, id_stab_err_z,        "stab_err_z",        false)),
    el_arr_zero_err_z(new BooleanValueProperty(this, id_el_arr_zero_err_z, "el_arr_zero_err_z", false)),
    stab_angle_err_z (new BooleanValueProperty(this, id_stab_angle_err_z,  "stab_angle_err_z",  false)),

    gsp_target_filter_en(false),
    gsp_angle_filter_en(false),
    _host_etld_en(false),

    _speed_measure_en(false),
    _tel_cnt(0),
    _x_inverted_koef(1.),
    _z_inverted_koef(1.),

    _write_dump(false),

    _gsp_controller(nullptr)
{
}
void GSPCtrlModel::set(const uint32_t & id)
{
    float fval{0.f};
    uint32_t uval{0u};
    bool value;
    switch(id)
    {
    case id_drift_x:
//        qInfo(logGspCtrl(), "set: Drift { X [%i] }", drift_x->get());
//        emit gsp_set_drift_compensation_x(drift_x->get());
//        break;
    case id_drift_z:
        qInfo(logGspCtrl(), "set: Drift { X [%i], Z [%i] }", drift_x->get(), drift_z->get());
        emit gsp_drift(drift_x->get(), drift_z->get());
        gsp_drift_z(drift_z->get());
        break;
//        qInfo(logGspCtrl(), "set: Drift { Z [%i] }", drift_z->get());
//        emit gsp_set_drift_compensation_z(drift_z->get());
//       break;
    case id_angle_enable:
        if(angle_enable->get()) {
            qInfo(logGspCtrl(), "set: Angle Control Enable");
            emit gsp_angle_ctrl_mode();
        } else {
            qInfo(logGspCtrl(), "set: Angle Control Disable");
            emit gsp_velocity_ctrl_mode();
        }
        break;
    case id_x_angle_set:
        emit x_angle_set->set_success();
        break;
    case id_z_angle_set:
        emit z_angle_set->set_success();
        break;
    case id_angle_discrepancy_x:
        qInfo(logGspCtrl(), "set: Angle X discrepancy = %f", angle_discrepancy_x->get());
        fval = angle_discrepancy_x->get();
        uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_angle_discrepancy_x, uval);
        break;
    case id_angle_discrepancy_z:
        qInfo(logGspCtrl(), "set: Angle Z discrepancy = %f", angle_discrepancy_z->get());
        fval = angle_discrepancy_z->get();
        uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_angle_discrepancy_z, uval);
        break;
    case id_x_Kp:
        qInfo(logGspCtrl(), "set: x_Kp = %f", x_Kp->get());
        gsp_set_x_Kp(x_Kp->get());
        break;
    case id_x_Ki:
        qInfo(logGspCtrl(), "set: x_Ki = %f", x_Ki->get());
        gsp_set_x_Ki(x_Ki->get());
        break;
    case id_x_Kd:
        qInfo(logGspCtrl(), "set: x_Kd = %f", x_Kd->get());
        gsp_set_x_Kd(x_Kd->get());
        break;
    case id_x_integral_limit:
        qInfo(logGspCtrl(), "set: x_integral_limit = %f", x_integral_limit->get());
        gsp_set_x_integral_limit(x_integral_limit->get());
        break;
    case id_z_Kp:
        qInfo(logGspCtrl(), "set: z_Kp = %f", z_Kp->get());
        gsp_set_z_Kp(z_Kp->get());
        break;
    case id_z_Ki:
        qInfo(logGspCtrl(), "set: z_Ki = %f", z_Ki->get());
        gsp_set_z_Ki(z_Ki->get());
        break;
    case id_z_Kd:
        qInfo(logGspCtrl(), "set: z_Kd = %f", z_Kd->get());
        gsp_set_z_Kd(z_Kd->get());
        break;
    case id_z_integral_limit:
        qInfo(logGspCtrl(), "set: z_integral_limit = %f", z_integral_limit->get());
        gsp_set_z_integral_limit(z_integral_limit->get());
        break;
    case id_pid_out_limit_z:
        qInfo(logGspCtrl(), "set: pid_out_limit = %f", pid_out_limit_z->get());
        gsp_set_pid_out_limit_z(pid_out_limit_z->get());
        break;
    case id_pid_out_limit_x:
        qInfo(logGspCtrl(), "set: pid_out_limit = %f", pid_out_limit_x->get());
        gsp_set_pid_out_limit_x(pid_out_limit_x->get());
        break;
    case id_target_filter_k_x:
        qInfo(logGspCtrl(), "set: target_filter_k_x = %f", target_filter_k_x->get());
        gsp_set_target_filter_k_x(target_filter_k_x->get());
        break;
    case id_target_filter_k_y:
        qInfo(logGspCtrl(), "set: target_filter_k_y = %f", target_filter_k_y->get());
        gsp_set_target_filter_k_y(target_filter_k_y->get());
        break;
    case id_tr_type_no_filter:
        if(tr_type_no_filter->get()) {
            switch_tracker_mode(0);
        }
        tr_type_exp_filter->set(false);
        tr_type_kalman_filter->set(false);
        tr_type_lsm_filter->set(false);
        break;
    case id_tr_type_exp_filter:
        if(tr_type_exp_filter->get()) {
            switch_tracker_mode(1);
        }
        tr_type_no_filter->set(false);
        tr_type_kalman_filter->set(false);
        tr_type_lsm_filter->set(false);
        break;
    case id_tr_type_kalman_filter:
        if(tr_type_kalman_filter->get()) {
            switch_tracker_mode(2);
        }
        tr_type_no_filter->set(false);
        tr_type_exp_filter->set(false);
        tr_type_lsm_filter->set(false);
        break;
    case  id_tr_type_lsm_filter:
        if(tr_type_lsm_filter->get()) {
            switch_tracker_mode(3);
        }
        tr_type_no_filter->set(false);
        tr_type_exp_filter->set(false);
        tr_type_kalman_filter->set(false);
        break;
    case id_tr_speed_limit_en:
        if(tr_speed_limit_en->get()) {
            switch_tracker_speed_limit_mode(1);
        }
        else {
            switch_tracker_speed_limit_mode(0);
        }
        break;
    case id_measure_speed:
        qInfo(logGspCtrl(), "set: GSP Speed Measure %s", measure_speed_chbox->get() ? "On" : "Off");
        _speed_measure_en = measure_speed_chbox->get();
        //emit measure_speed_changed(measure_speed_chbox->get());
        break;
    }
}
void GSPCtrlModel::get(const uint32_t & id)
{
    switch(id)
    {
    case id_x_Kp:
        gsp_get_x_Kp();
        break;
    case id_x_Ki:
        gsp_get_x_Ki();
        break;
    case id_x_Kd:
        gsp_get_x_Kd();
        break;
    case id_x_integral_limit:
        gsp_get_x_integral_limit();
        break;
    case id_z_Kp:
        gsp_get_z_Kp();
        break;
    case id_z_Ki:
        gsp_get_z_Ki();
        break;
    case id_z_Kd:
        gsp_get_z_Kd();
        break;
    case id_z_integral_limit:
        gsp_get_z_integral_limit();
        break;
    case id_target_filter_k_x:
        gsp_get_target_filter_k_x();
        break;
    case id_target_filter_k_y:
        gsp_get_target_filter_k_y();
        break;
    }
}
void GSPCtrlModel::cmd(const uint32_t & id)
{
    float val_x, val_z = 0;
    std::vector<uint8_t> data;
    uint32_t val = 0;
    switch(id)
    {
    case id_up_start:
        qInfo(logGspCtrl(), "cmd: Star Platformt Up");
        speed_measure_start(en_axis_x);
        emit gsp_speed_set(0.5 * _x_inverted_koef, 0.0);
        break;
    case id_down_start:
        qInfo(logGspCtrl(), "cmd: Start Platform Down");
        speed_measure_start(en_axis_x);
        emit gsp_speed_set(-0.5 * _x_inverted_koef, 0.0);
        break;
    case id_left_start:
        qInfo(logGspCtrl(), "cmd: Start Platform Left");
        speed_measure_start(en_axis_z);
        emit gsp_speed_set(0.0, -0.5 * _z_inverted_koef);
        break;
    case id_right_start:
        qInfo(logGspCtrl(), "cmd: Start Platform Right");
        speed_measure_start(en_axis_z);
        emit gsp_speed_set(0.0, 0.5 * _z_inverted_koef);
        break;
    case id_stop:
        qInfo(logGspCtrl(), "cmd: Stop Platform");
        speed_measure_stop();
        emit gsp_speed_set(0.0, 0.0);
        break;
    case id_angle_abs:
        qInfo(logGspCtrl(), "cmd: Platform Absolute Angle { X[ %.3f ]Z[ %.3f ] }", x_angle_set->get(), z_angle_set->get());
        val_x = x_angle_set->get();
        val_z = z_angle_set->get();
        //emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_target_angle_x_f, *((uint32_t*)(&val_x)));
        data.push_back(((*((uint32_t*)(&val_z))) >>  0) & 0xFF);
        data.push_back(((*((uint32_t*)(&val_z))) >>  8) & 0xFF);
        data.push_back(((*((uint32_t*)(&val_z))) >> 16) & 0xFF);
        data.push_back(((*((uint32_t*)(&val_z))) >> 24) & 0xFF);

        data.push_back(((*((uint32_t*)(&val_x))) >>  0) & 0xFF);
        data.push_back(((*((uint32_t*)(&val_x))) >>  8) & 0xFF);
        data.push_back(((*((uint32_t*)(&val_x))) >> 16) & 0xFF);
        data.push_back(((*((uint32_t*)(&val_x))) >> 24) & 0xFF);
        //emit vdb_write_request_array(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_target_angle_z_f, data);
        emit gsp_absolute_angle_set(x_angle_set->get(), z_angle_set->get());
        break;
    case id_angle_rel:
        qInfo(logGspCtrl(), "cmd: Platform Relative Angle { X[ %.3f ]Z[ %.3f ] }", x_angle_set->get(), z_angle_set->get());
        emit gsp_relative_angle_set(x_angle_set->get(), z_angle_set->get());
        break;
//    case id_speed_set:
//       emit gsp_speed_f_set(x_speed_f_set->get(), z_speed_f_set->get());
//        if ((x_speed_set->get() < 0.001) && (x_speed_set->get() > -0.001) && (z_speed_set->get() < 0.001) && (z_speed_set->get() > -0.001)) {
//            speed_measure_stop();
//        }
//        else {
//            if (abs(x_speed_set->get()) >= 0.01) {
//                speed_measure_start(en_axis_x);
//            }
//            else if (abs(z_speed_set->get()) >= 0.01) {
//                speed_measure_start(en_axis_z);
//            }
//        }
//        data.resize(sizeof(val_z) * 2);
//        val_z = z_speed_set->get();
//        memcpy(data.data(), &val_z, sizeof(val_z));
//        val_x = x_speed_set->get();
//        memcpy(data.data() + sizeof(val_z), &val_x, sizeof(val_x));
//        emit vdb_write_request_array(vdb_registers::en_vbd_GSP, vdb_registers::en_vbd_gsp_target_speed_z_f, data);
//        break;
//    case id_speed_calc:
//        if (_speed_measure_en) {
//            speed_measure_run();
//        }
//        break;
    case id_speed_f_set:
        emit gsp_speed_f_set(x_speed_f_set->get(), z_speed_f_set->get());
        break;
    case id_speed_f_stop:
        emit gsp_speed_f_set(0.0, 0.0);
        break;
    case id_arretir_reset:
        qInfo(logGspCtrl(), "cmd: Reset Mechanical Arretir, Set Electrical Arretir, Set to null position");
        emit gsp_arretir_reset();
        break;
    case id_stabilization:
        if(gsp_stabilization_x || gsp_stabilization_y)
        {
            qInfo(logGspCtrl(), "cmd: Stabilisation Off");
        }
        else
        {
            qInfo(logGspCtrl(), "cmd: Stabilisation On");
        }
        emit gsp_stabilization_switch();
        break;
    case id_park:
        qInfo(logGspCtrl(), "cmd: Park");
        emit gsp_park();
        break;
    case id_arretir_set:
        qInfo(logGspCtrl(), "cmd: Set Mechanical Arretir");
        emit gsp_arretir_set();
        break;
    case id_null_position:
        qInfo(logGspCtrl(), "cmd: Set to null position");
        emit gsp_null_position();
        break;
    case id_pohod_position:
        qInfo(logGspCtrl(), "cmd: Set to pohod position");
        emit gsp_pohod_position();
        break;
    case id_polyot_position:
        qInfo(logGspCtrl(), "cmd: Set to polyot position");
        emit gsp_polyot_position();
        break;
    case id_alignment:
        qInfo(logGspCtrl(), "cmd: Platform Alignment");
        emit gsp_alignment();
        break;
    case id_target_filter_ctrl:
        gsp_target_filter_switch();
        break;
    case id_angle_filter_ctrl:
        gsp_angle_filter_switch();
        break;
    case id_x_arretir_off:
        val = (en_x_ax << 16) | en_mode_off;
        emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, *reinterpret_cast<uint32_t*>(&val));
        break;
    case id_z_arretir_off:
        val = (en_z_ax << 16) | en_mode_off;
        emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, *reinterpret_cast<uint32_t*>(&val));
        break;
    case id_all_arretir_off:
        emit gsp_arretir_reset();
        val = (en_all_ax << 16) | en_mode_off;
        //emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, *reinterpret_cast<uint32_t*>(&val));
        break;
    case id_x_arretir_on:
        val = (en_x_ax << 16) | en_mode_on;
        emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, *reinterpret_cast<uint32_t*>(&val));
        break;
    case id_z_arretir_on:
        val = (en_z_ax << 16) | en_mode_on;
        //emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, *reinterpret_cast<uint32_t*>(&val));
        break;
    case id_all_arretir_on:
        emit gsp_arretir_set();
        val = (en_all_ax << 16) | en_mode_on;
        //emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, *reinterpret_cast<uint32_t*>(&val));
        break;
    case id_fine_arretir_set:
        emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, 1);
        break;
    case id_fine_arretir_reset:
        emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_arretir, 0);
        break;
    case id_set_new_angle_x:
        val_x = new_angle_x->get();
        emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_zero_pos_x, *reinterpret_cast<uint32_t*>(&val_x));
        break;
    case id_set_new_angle_z:
        val_z = new_angle_z->get();
        emit vdb_write_request(vdb_registers::en_vbd_GSP, vdb_registers::en_vdb_gsp_zero_pos_x, *reinterpret_cast<uint32_t*>(&val_z));
        break;
    case id_set_new_angles:
        data.resize(sizeof(val_z) * 2);
        val_z = new_angle_z->get();
        memcpy(data.data(), &val_z, sizeof(val_z));
        val_x = new_angle_x->get();
        memcpy(data.data() + sizeof(val_z), &val_x, sizeof(val_x));
        emit vdb_write_request_array(vdb_registers::en_vbd_GSP, vdb_registers::en_vbd_gsp_zero_pos_z, data);
        break;
    }
}
void GSPCtrlModel::update()
{

}
void GSPCtrlModel::response(Cmd)
{
}
void GSPCtrlModel::response(Telemetry t)
{
    Telemetry::t_telemetry_struct * tel = t.get();

    status->set(tel->gsp.status);
    mode->set(tel->gsp.mode.__raw_);

    st_mechanical_locking->set(tel->gsp.mode.mechanical_locking);
    st_electronic_locking->set(tel->gsp.mode.electronic_locking);
    st_free_locking->set(tel->gsp.mode.free_locking);
    st_gyrostabilization->set(tel->gsp.mode.gyrostabilization);
    st_zero->set(tel->gsp.mode.zero);
    st_hike->set(tel->gsp.mode.hike);
    st_pilot->set(tel->gsp.mode.pilot);
    st_angle_control->set(tel->gsp.mode.angle_control);
    st_park->set(tel->gsp.mode.park);

    z_drive_pos->set(tel->gsp.z_drive_pos);
    x_drive_pos->set(tel->gsp.x_drive_pos);
    z_angle->set(tel->gsp.z_angle);
    x_angle->set(tel->gsp.x_angle);
    _angle_x = tel->gsp.x_angle;
    _angle_z = tel->gsp.z_angle;
    z_speed->set(tel->gsp.speed_z);
    x_speed->set(tel->gsp.speed_x);
    alignment_pos_y->set(tel->controller.alignment_pos_x);
    alignment_pos_z->set(tel->controller.alignment_pos_z);

    gsp_mech_arretir_x = bool(tel->gsp.mode.mechanical_locking);
    gsp_mech_arretir_y = bool(tel->gsp.mode.mechanical_locking);
    gsp_el_arretir_x = bool(tel->gsp.mode.electronic_locking);
    gsp_el_arretir_y = bool(tel->gsp.mode.electronic_locking);
    gsp_stabilization_x = bool(tel->gsp.mode.gyrostabilization);
    gsp_stabilization_y = bool(tel->gsp.mode.gyrostabilization);
    gsp_zero_x = bool(tel->gsp.mode.zero);
    gsp_zero_y = bool(tel->gsp.mode.zero);
    gsp_park_x = bool(tel->gsp.mode.park);
    gsp_park_y = bool(tel->gsp.mode.park);

    angle_enable->set(bool(tel->gsp.mode.angle_control));

    if(gsp_stabilization_x || gsp_stabilization_y) emit stabilization->set_success();
    else emit stabilization->set_neutral();

    measure_speed_n->set(_tel_cnt);
    _tel_cnt++;
    if(_write_dump) {
        write_dump_in_txt(tel, _tel_cnt);
    }

    bits->set(*reinterpret_cast<int*>(&tel->dev.gsp_raw_status));
    Telemetry::t_gsp_raw_status* raw_status = reinterpret_cast<Telemetry::t_gsp_raw_status*>(&tel->dev.gsp_raw_status);
    mech_arr_x      ->set(raw_status->mech_arr_x_c);
    mech_arr_z      ->set(raw_status->mech_arr_z_c);
    mech_arr_f      ->set(raw_status->mech_arr_f);
    stab_x          ->set(raw_status->stab_x);
    stab_z          ->set(raw_status->stab_z);
    park_x          ->set(raw_status->park_x);
    park_z          ->set(raw_status->park_z);
    zero_x          ->set(raw_status->zero_x);
    zero_z          ->set(raw_status->zero_z);
    el_arr_x        ->set(raw_status->el_arr_x);
    el_arr_z        ->set(raw_status->el_arr_z);
    zero_pos        ->set(raw_status->zero_pos);
    pilot_pos       ->set(raw_status->pilot_pos);
    pohod_pos       ->set(raw_status->pohod_pos);
    can_error       ->set(raw_status->can_error);
    adc_bit         ->set(raw_status->adc_bit);
    angle_is_set_x  ->set(raw_status->angle_is_set_x);
    angle_is_set_z  ->set(raw_status->angle_is_set_z);
    diagnostics     ->set(raw_status->diagnostics);
    drift_en        ->set(raw_status->drift_en);
    drift_mode      ->set(raw_status->drift_mode);
    ctrl_mode       ->set(raw_status->ctrl_mode);
    bits_selftest   ->set(raw_status->diagnostics);
    fsm_mode_z      ->set(raw_status->fsm_mode_z);
    fsm_mode_x      ->set(raw_status->fsm_mode_x);
    park_f_x        ->set(raw_status->park_f_x);
    park_f_z        ->set(raw_status->park_f_z);
    diagnostic_res_z->set(raw_status->diagnostics_res_z);
    diagnostic_res_x->set(raw_status->diagnostics_res_x);

    uint16_t gsp_diagnostic_x = static_cast<uint16_t>(tel->dev.gsp_diagnostic);
    Telemetry::t_gsp_diagnostic* diagnostic_x = reinterpret_cast<Telemetry::t_gsp_diagnostic*>(&gsp_diagnostic_x);
    timeout_err_x    ->set(diagnostic_x->bit_field.timeout_err);
    el_arr_err_x     ->set(diagnostic_x->bit_field.el_arr_err);
    stab_err_x       ->set(diagnostic_x->bit_field.stab_err);
    el_arr_zero_err_x->set(diagnostic_x->bit_field.el_arr_zero_err);
    stab_angle_err_x ->set(diagnostic_x->bit_field.stab_angle_err);

    uint16_t gsp_diagnostic_z = static_cast<uint16_t>(tel->dev.gsp_diagnostic >> 16);
    Telemetry::t_gsp_diagnostic* diagnostic_z = reinterpret_cast<Telemetry::t_gsp_diagnostic*>(&gsp_diagnostic_z);
    timeout_err_z    ->set(diagnostic_z->bit_field.timeout_err);
    el_arr_err_z     ->set(diagnostic_z->bit_field.el_arr_err);
    stab_err_z       ->set(diagnostic_z->bit_field.stab_err);
    el_arr_zero_err_z->set(diagnostic_z->bit_field.el_arr_zero_err);
    stab_angle_err_z ->set(diagnostic_z->bit_field.stab_angle_err);
}
void GSPCtrlModel::gsp_response_x_Kp(const double &v)
{
    x_Kp->set(v);
    qInfo(logGspCtrl(), "get: x_Kp = %f", x_Kp->get());
}
void GSPCtrlModel::gsp_response_x_Ki(const double &v)
{
    x_Ki->set(v);
    qInfo(logGspCtrl(), "get: x_Ki = %f", x_Ki->get());
}
void GSPCtrlModel::gsp_response_x_Kd(const double &v)
{
    x_Kd->set(v);
    qInfo(logGspCtrl(), "get: x_Kd = %f", x_Kd->get());
}
void GSPCtrlModel::gsp_response_x_integral_limit(const double &v)
{
    x_integral_limit->set(v);
    qInfo(logGspCtrl(), "get: x_integral_limit = %f", x_integral_limit->get());
}
void GSPCtrlModel::gsp_response_z_Kp(const double &v)
{
    z_Kp->set(v);
    qInfo(logGspCtrl(), "get: z_Kp = %f", z_Kp->get());
}
void GSPCtrlModel::gsp_response_z_Ki(const double &v)
{
    z_Ki->set(v);
    qInfo(logGspCtrl(), "get: z_Ki = %f", z_Ki->get());
}
void GSPCtrlModel::gsp_response_z_Kd(const double &v)
{
    z_Kd->set(v);
    qInfo(logGspCtrl(), "get: z_Kd = %f", z_Kd->get());
}
void GSPCtrlModel::gsp_response_z_integral_limit(const double &v)
{
    z_integral_limit->set(v);
    qInfo(logGspCtrl(), "get: z_integral_limit = %f", z_integral_limit->get());
}

void GSPCtrlModel::gsp_host_etld_info(bool state)
{
    _host_etld_en = state;
}

void GSPCtrlModel::set_inverse_x(bool flag)
{
    _x_inverted_koef = flag ? -1. : 1.;
}

void GSPCtrlModel::set_inverse_z(bool flag)
{
    _z_inverted_koef = flag ? -1. : 1.;
}

void GSPCtrlModel::gsp_drift_x(const int32_t& x_drift)
{
//    if(_gsp_controller) _gsp_controller->set_drift_x(x_drift);
//    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_drift_compensation_x, x_drift);
}

void GSPCtrlModel::gsp_drift_z(const int32_t& z_drift)
{
//    if(_gsp_controller) _gsp_controller->set_drift_z(z_drift);
//    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_drift_compensation_z, z_drift);
}

void GSPCtrlModel::gsp_set_x_Kp(const double & v)
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) _gsp_controller->set_x_Kp(v);
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        float fval = v;
        uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_kp_x, uval);
    }
}
void GSPCtrlModel::gsp_set_x_Ki(const double & v)
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) _gsp_controller->set_x_Ki(v);
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        float fval = v;
        uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_ki_x, uval);
    }
}
void GSPCtrlModel::gsp_set_x_Kd(const double & v)
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) _gsp_controller->set_x_Kd(v);
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        float fval = v;
        uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_kd_x, uval);
    }
}
void GSPCtrlModel::gsp_set_x_integral_limit(const double & v)
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) _gsp_controller->set_x_integral_limit(v);
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        float fval = v;
        uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_integral_limit_x, uval);
    }
}
void GSPCtrlModel::gsp_set_z_Kp(const double & v)
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) _gsp_controller->set_z_Kp(v);
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        float fval = v;
        uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_kp_z, uval);
    }
}
void GSPCtrlModel::gsp_set_z_Ki(const double & v)
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) _gsp_controller->set_z_Ki(v);
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        float fval = v;
        uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_ki_z, uval);
    }
}
void GSPCtrlModel::gsp_set_z_Kd(const double & v)
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) _gsp_controller->set_z_Kd(v);
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        float fval = v;
        uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_kd_z, uval);
    }
}
void GSPCtrlModel::gsp_set_z_integral_limit(const double & v)
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) _gsp_controller->set_z_integral_limit(v);
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        float fval = v;
        uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
        emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_integral_limit_z, uval);
    }
}
void GSPCtrlModel::gsp_set_pid_out_limit_z(const double & v)
{
    float fval = v;
    uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_max_speed_z, uval);
}
void GSPCtrlModel::gsp_set_pid_out_limit_x(const double & v)
{
    float fval = v;
    uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_max_speed_x, uval);
}
void GSPCtrlModel::gsp_get_x_Kp()
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
       if(_gsp_controller) gsp_response_x_Kp(_gsp_controller->get_x_Kp());
       else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_kp_x, 1);
    }
}
void GSPCtrlModel::gsp_get_x_Ki()
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) gsp_response_x_Ki(_gsp_controller->get_x_Ki());
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_ki_x, 1);
    }
}
void GSPCtrlModel::gsp_get_x_Kd()
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) gsp_response_x_Kd(_gsp_controller->get_x_Kd());
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_kd_x, 1);
    }
}
void GSPCtrlModel::gsp_get_x_integral_limit()
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) gsp_response_x_integral_limit(_gsp_controller->get_x_integral_limit());
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_integral_limit_x, 1);
    }
}
void GSPCtrlModel::gsp_get_z_Kp()
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) gsp_response_z_Kp(_gsp_controller->get_z_Kp());
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_kp_z, 1);
    }
}
void GSPCtrlModel::gsp_get_z_Ki()
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) gsp_response_z_Ki(_gsp_controller->get_z_Ki());
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_ki_z, 1);
    }
}
void GSPCtrlModel::gsp_get_z_Kd()
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) gsp_response_z_Kd(_gsp_controller->get_z_Kd());
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_kd_z, 1);
    }
}
void GSPCtrlModel::gsp_get_z_integral_limit()
{
    if(_host_etld_en) { // Если настраиваем АС для ПК
        if(_gsp_controller) gsp_response_z_integral_limit(_gsp_controller->get_z_integral_limit());
        else qWarning(logGspCtrl(), "warning: _gsp_controller == nulptr");
    }
    else { // Если настраиваем АС в шаре
        emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_gc_integral_limit_z, 1);
    }
}
void GSPCtrlModel::gsp_set_target_filter_k_x(const double & v)
{
    float fval = v;
    uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
//    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_tr_target_filter_k_x, uval);
}
void GSPCtrlModel::gsp_set_target_filter_k_y(const double & v)
{
    float fval = v;
    uint32_t uval = *reinterpret_cast<uint32_t*>(&fval);
//    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_tr_target_filter_k_y, uval);
}
void GSPCtrlModel::gsp_get_target_filter_k_x()
{
//    emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_tr_target_filter_k_x, 1);
}
void GSPCtrlModel::gsp_get_target_filter_k_y()
{
//    emit vdb_read_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_tr_target_filter_k_y, 1);
}
void GSPCtrlModel::gsp_target_filter_switch()
{
    gsp_target_filter_en = !gsp_target_filter_en;

//    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_tr_target_filter_state, gsp_target_filter_en ? (0) : ((1 << 16) | 1));
    if(gsp_target_filter_en) {
        emit target_filter_ctrl->set_success();
    }
    else {
        emit target_filter_ctrl->set_neutral();
    }
}
void GSPCtrlModel::gsp_angle_filter_switch()
{
    gsp_angle_filter_en = !gsp_angle_filter_en;

//    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_tr_target_filter_state, gsp_target_filter_en ? (0) : ((1 << 16) | 1));
    if(gsp_angle_filter_en) {
        emit angle_filter_ctrl->set_success();
    }
    else {
        emit angle_filter_ctrl->set_neutral();
    }
}

void GSPCtrlModel::switch_tracker_mode(const uint8_t & v)
{
//    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_tr_target_filter_state, v);
}

void GSPCtrlModel::switch_tracker_speed_limit_mode(const uint8_t & v)
{
//    emit vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_tr_speed_limit, v);
}

void GSPCtrlModel::write_dump(const bool& enable)
{
    _write_dump = enable;
    if(_write_dump) {
        open_dump_file();
    } else {
        close_dump_file();
    }
}
void GSPCtrlModel::open_dump_file()
{
    close_dump_file();
    //std::string data = QDate::currentDate().toString().toStdString(); // возвращаем текущую дату
    //std::string time = QTime::currentTime().toString().toStdString(); // возвращаем текущее время
    //std::string name = data + "_" + time + "gsp_dump.txt";
    std::string name = "gsp_dump.txt";
    //str.append("_").append(time).append("gsp_dump");
    _dump_stream.open("gsp_dump.txt");
    write_dump_header();
}
void GSPCtrlModel::close_dump_file()
{
    if(_dump_stream.is_open()) {
        _dump_stream.close();
    }
}
void GSPCtrlModel::write_dump_header()
{
    if(_dump_stream.is_open()) {
        _dump_stream << "packet_num\t"
                     << "z_pos\t"
                     << "x_pos\t"
                     << "z_angle\t"
                     << "x_angle\t"
                     << "z_speed\t"
                     << "x_speed\t"
                     << std::endl;
    }
}
void GSPCtrlModel::write_dump_in_txt(Telemetry::t_telemetry_struct * tel, uint32_t& tel_cnt)
{
    const auto time = QTime::currentTime().toString().toStdString(); // возвращаем текущее время
    _dump_stream << tel_cnt << '\t'
                 << tel->gsp.z_drive_pos << '\t'
                 << tel->gsp.x_drive_pos << '\t'
                 << tel->gsp.z_angle << '\t'
                 << tel->gsp.x_angle << '\t'
                 << tel->gsp.speed_z << '\t'
                 << tel->gsp.speed_x << '\t'
                 << std::endl;
}

void GSPCtrlModel::speed_measure_start(uint8_t x_or_z)
{
    if (_speed_measure_en) {
        write_dump(true);
        //std::cout << "// *************************************************************************** //" << std::endl;
        //std::cout << "N0 = " << _tel_cnt << "; Angle Z = " << _angle_z << "; Angle X = " << _angle_x << std::endl;
        _axis = x_or_z;
        measure_speed_n0->set(_tel_cnt);
        if (_axis == en_axis_z) { // Z
            measure_speed_angle0->set(_angle_z);
        }
        else if (_axis == en_axis_x) {
            measure_speed_angle0->set(_angle_x);
        }
    }
}
void GSPCtrlModel::speed_measure_stop(void)
{
    if (_speed_measure_en) {
        write_dump(false);
        //std::cout << "Nt = " << _tel_cnt << "; Angle Z = " << _angle_z << "; Angle X = " << _angle_x << std::endl;
        //std::cout << "// *************************************************************************** //" << std::endl;
        measure_speed_nt->set(_tel_cnt);
        if (_axis == en_axis_z) { // Z
            measure_speed_anglet->set(_angle_z);
        }
        else if (_axis == en_axis_x) {
            measure_speed_anglet->set(_angle_x);
        }
        //speed_measure_run();
    }
}
void GSPCtrlModel::speed_measure_run(void)
{
    // Call the Python script passing a filename argument.
    std::string command = "python ";
    std::string filename = "D:/Projects/PyProjects/speed_measure/speed_mwasure.py";
    command  += filename;
    //std::string _str = command  + filename;
    system(command.c_str());
}
