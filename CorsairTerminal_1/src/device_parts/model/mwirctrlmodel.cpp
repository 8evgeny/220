#include "mwirctrlmodel.h"

#include <QVariant>
#include <qmv/property/booleanvalueproperty.h>
#include <qmv/property/doublevalueproperty.h>
#include <qmv/property/intvalueproperty.h>
#include <qmv/property/cmdproperty.h>

#include "device_parts/channel/abstractchannel.h"
#include "device_parts/cmd/vdb_registers.h"

#include "tools/autofocusmodel.h"

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logMWIRCtrl, "MWIRCtrl")

#ifdef CORSAIR_SET_1
#define AF_MODEL_CONFIG_NAME ("table/afmodel_swir_3.txt")
#endif
#ifdef CORSAIR_SET_2
#define AF_MODEL_CONFIG_NAME ("table/afmodel_mwir_1.txt")
#endif
#ifdef CORSAIR_SET_3
#define AF_MODEL_CONFIG_NAME ("table/afmodel_swir_2.txt")
#endif

#define MIN_VAL 0x8000'0000
#define MAX_VAL 0x7FFF'FFFF

MWIRCtrlModel::MWIRCtrlModel(QObject * parent, uint32_t id, QString name) :
    QMVModel(parent, id, name),
    _sensor_status(     new IntValueProperty(    this, id_sensor_status,    "sensor_status", 0, -65536, 65536)),
    _lens_status(       new IntValueProperty(    this, id_lens_status,      "lens_status", 0, -65536, 65536)),
    _fps(               new IntValueProperty(    this, id_fps,              "fps", 0, -65536, 65536)),
    _vres(              new IntValueProperty(    this, id_vres,             "vres", 0, -65536, 65536)),
    _hres(              new IntValueProperty(    this, id_hres,             "hres", 0, -65536, 65536)),
    _exp(               new IntValueProperty(    this, id_exp,              "exp", 00, -65536, 65536)),
    _zoom_drive(        new IntValueProperty(    this, id_zoom_drive,       "zoom_drive", 0, -65536, 65536)),
    _focus_drive(       new IntValueProperty(    this, id_focus_drive,      "focus_drive", 0, -65536, 65536)),
    _lens_temp(         new IntValueProperty(    this, id_lens_temp,        "lens_temp", 0, -65536, 65536)),
    _focus_mm(          new DoubleValueProperty( this, id_focus_mm,         "focus_mm", 0, -65536, 65536)),
    _field_grad(        new DoubleValueProperty( this, id_field_grad,       "field_grad", 0, -65536, 65536)),
    _zoom(              new IntValueProperty(    this, id_zoom,             "zoom", 0, 0, 65536)),
    _focus(             new IntValueProperty(    this, id_focus,            "focus", 0, 0, 65536)),
    _autofocus_lens_en(new BooleanValueProperty(this, id_autofocus_lens_en, "autofocus_lens_en", true)),
    _autofocus_host_en(new BooleanValueProperty(this, id_autofocus_host_en, "autofocus_host_en", false)),
    _thermostat_on(    new CmdProperty(this, id_thermostat_on,     "thermostat_on")),
    _thermostat_off(   new CmdProperty(this, id_thermostat_off,    "thermostat_off")),
    _heat_on(          new CmdProperty(this, id_heat_on,           "heat_on")),
    _heat_off(         new CmdProperty(this, id_heat_off,          "heat_off")),
    _agc(               new CmdProperty(this, id_agc,              "agc")),
    _inversion(         new CmdProperty(this, id_inversion,        "inversion")),
    _autofocus(        new CmdProperty(this, id_autofocus,         "autofocus")),
    _field_sync(       new CmdProperty(this, id_field_sync,        "field_sync")),
    _field_sync_narrow(new CmdProperty(this, id_field_sync_narrow, "field_sync_narrow")),
    _field_sync_reg(    new CmdProperty(this, id_field_sync_reg,   "field_sync_reg")),
    _zoom_in_start(     new CmdProperty(this, id_zoom_in_start,    "zoom_in_start")),
    _zoom_in_stop(      new CmdProperty(this, id_zoom_in_stop,     "zoom_in_stop")),
    _zoom_out_start(    new CmdProperty(this, id_zoom_out_start,   "zoom_out_start")),
    _zoom_out_stop(     new CmdProperty(this, id_zoom_out_stop,    "zoom_out_stop")),
    _iris_in_start(     new CmdProperty(this, id_iris_in_start,    "iris_in_start")),
    _iris_in_stop(      new CmdProperty(this, id_iris_in_stop,     "iris_in_stop")),
    _iris_out_start(    new CmdProperty(this, id_iris_out_start,   "iris_out_start")),
    _iris_out_stop(     new CmdProperty(this, id_iris_out_stop,    "iris_out_stop")),
    _focus_near_start(  new CmdProperty(this, id_focus_near_start, "focus_near_start")),
    _focus_near_stop(   new CmdProperty(this, id_focus_near_stop,  "focus_near_stop")),
    _focus_far_start(   new CmdProperty(this, id_focus_far_start,  "focus_far_start")),
    _focus_far_stop(    new CmdProperty(this, id_focus_far_stop,   "focus_far_stop")),
    _agc_on(false),
    _inversion_on(false),
    _autofocus_on(false),
    _autofocus_host_pause(false),
    _autofocus_host_last_state(false),
    _field_sync_reg_on(false),
    _af_model(new AutofocusModel(AF_MODEL_CONFIG_NAME)),
    _sensor_ssettings_telemetry_enable(    new BooleanValueProperty(this, id_sensor_ssettings_telemetry_enable,  "sensor_ssettings_telemetry_enable",  true)),
    _sensor_ssettings_soft_version(        new IntValueProperty(    this, id_sensor_ssettings_soft_version,      "sensor_ssettings_soft_version",      1,     MIN_VAL, MAX_VAL)),
    _sensor_ssettings_telemetry_address(   new IntValueProperty(    this, id_sensor_ssettings_telemetry_address, "sensor_ssettings_telemetry_address", 0x70,  MIN_VAL, MAX_VAL)),
    _sensor_msettings_cce_input(           new IntValueProperty(    this, id_sensor_msettings_cce_input,         "sensor_msettings_cce_input",         0x60,  MIN_VAL, MAX_VAL)),
    _sensor_msettings_cce_min(             new IntValueProperty(    this, id_sensor_msettings_cce_min,           "sensor_msettings_cce_min",           0x578, MIN_VAL, MAX_VAL)),
    _sensor_msettings_cce_max(             new IntValueProperty(    this, id_sensor_msettings_cce_max,           "sensor_msettings_cce_max",           0xB54, MIN_VAL, MAX_VAL)),
    _sensor_msettings_pattern(             new IntValueProperty(    this, id_sensor_msettings_pattern,           "sensor_msettings_pattern",           0x4,   MIN_VAL, MAX_VAL)),
    _sensor_msettings_direction_video(     new BooleanValueProperty(this, id_sensor_msettings_direction_video,   "sensor_msettings_direction_video",   true)),
    _sensor_msettings_black_white_video(   new BooleanValueProperty(this, id_sensor_msettings_black_white_video, "sensor_msettings_black_white_video", true)),
    _sensor_msettings_gpol(                new IntValueProperty(    this, id_sensor_msettings_gpol,              "sensor_msettings_gpol",              0x514,      MIN_VAL, MAX_VAL)),
    _sensor_msettings_period_time(         new IntValueProperty(    this, id_sensor_msettings_period_time,       "sensor_msettings_period_time",       0x186A0,    MIN_VAL, MAX_VAL)),
    _sensor_msettings_hl_0(                new IntValueProperty(    this, id_sensor_msettings_hl_0,              "sensor_msettings_hl_0",              0x0000C350, MIN_VAL, MAX_VAL)),
    _sensor_msettings_hl_1(                new IntValueProperty(    this, id_sensor_msettings_hl_1,              "sensor_msettings_hl_1",              0x0000927C, MIN_VAL, MAX_VAL)),
    _sensor_msettings_hl_2(                new IntValueProperty(    this, id_sensor_msettings_hl_2,              "sensor_msettings_hl_2",              0x000061A8, MIN_VAL, MAX_VAL)),
    _sensor_msettings_hl_3(                new IntValueProperty(    this, id_sensor_msettings_hl_3,              "sensor_msettings_hl_3",              0x000030D4, MIN_VAL, MAX_VAL)),
    _sensor_msettings_hl_4(                new IntValueProperty(    this, id_sensor_msettings_hl_4,              "sensor_msettings_hl_4",              0x00001D4C, MIN_VAL, MAX_VAL)),
    _sensor_msettings_serial_number(       new IntValueProperty(    this, id_sensor_msettings_serial_number,     "sensor_msettings_serial_number",     0x00000001, MIN_VAL, MAX_VAL)),
    _sensor_dde_enable(                    new BooleanValueProperty(this, id_sensor_dde_enablem,           "sensor_dde_enable",            true)),
    _sensor_dde_base_mode(                 new IntValueProperty(    this, id_sensor_dde_base_mode,         "sensor_dde_base_mode",         0x00000001, MIN_VAL, MAX_VAL)),
    _sensor_dde_detail_gain(               new IntValueProperty(    this, id_sensor_dde_detail_gain,       "sensor_dde_detail_gain",       0x00000032, MIN_VAL, MAX_VAL)),
    _sensor_dde_detail_eps_normal(         new IntValueProperty(    this, id_sensor_dde_detail_eps_normal, "sensor_dde_detail_eps_normal", 0x04D3F640, MIN_VAL, MAX_VAL)),
    _sensor_dde_hist_background(           new IntValueProperty(    this, id_sensor_dde_hist_background,   "sensor_dde_hist_background",   0x000000FF, MIN_VAL, MAX_VAL)),
    _sensor_dde_brightness(                new IntValueProperty(    this, id_sensor_dde_brightness,        "sensor_dde_brightness",        0x00000000, MIN_VAL, MAX_VAL)),
    _sensor_dde_alpha_address(             new IntValueProperty(    this, id_sensor_dde_alpha_address,     "sensor_dde_alpha_address",     0x0000001A, MIN_VAL, MAX_VAL)),
    _sensor_dde_contrast(                  new IntValueProperty(    this, id_sensor_dde_contrast,          "sensor_dde_contrast",          0x00000080, MIN_VAL, MAX_VAL)),
    _sensor_dde_inversion(                 new IntValueProperty(    this, id_sensor_dde_inversion,         "sensor_dde_inversion",         0x00000002, MIN_VAL, MAX_VAL)),
    _sensor_snr_enable(                    new BooleanValueProperty(this, id_sensor_snr_enable,  "sensor_snr_enable",  true)),
    _sensor_snr_col_eps(                   new IntValueProperty(    this, id_sensor_snr_col_eps, "sensor_snr_col_eps", 0x1999999A, MIN_VAL, MAX_VAL)),
    _sensor_snr_row_eps(                   new IntValueProperty(    this, id_sensor_snr_row_eps, "sensor_snr_row_eps", 0x00033617, MIN_VAL, MAX_VAL)),
    _sensor_nuc_calibration_enable(        new BooleanValueProperty(this, id_sensor_nuc_calibration_enable,      "sensor_nuc_calibration_enable",       true)),
    _sensor_nuc_bad_pixel_replace_enable(  new BooleanValueProperty(this, id_sensor_nuc_bad_pixel_replace_enable,"sensor_nuc_bad_pixel_replace_enable", true)),
    _sensor_nuc_limit_level_out_data(      new IntValueProperty(    this, id_sensor_nuc_limit_level_out_data,    "sensor_nuc_limit_level_out_data",     0x0000E500, MIN_VAL, MAX_VAL)),
    _sensor_nuc_lens_position(             new IntValueProperty(    this, id_sensor_nuc_lens_position,           "sensor_nuc_lens_position",            0x00000000, MIN_VAL, MAX_VAL)),
    _sensor_bad_pixel_rep_enable(          new BooleanValueProperty(this, id_sensor_bad_pixel_rep_enable,    "sensor_bad_pixel_rep_enable",   true)),
    _sensor_bad_pixel_rep_treshold(        new IntValueProperty(    this, id_sensor_bad_pixel_rep_treshold,  "sensor_bad_pixel_rep_treshold", 0x000000C8, MIN_VAL, MAX_VAL)),
    _sensor_telemetry_enable(              new BooleanValueProperty(this, id_sensor_telemetry_enable,            "sensor_telemetry_enable",             false)),
    _sensor_telemetry_nuc_enable(          new BooleanValueProperty(this, id_sensor_telemetry_nuc_enable,        "sensor_telemetry_nuc_enable",         false)),
    _sensor_telemetry_snr_enable(          new BooleanValueProperty(this, id_sensor_telemetry_snr_enable,        "sensor_telemetry_snr_enable",         false)),
    _sensor_telemetry_dde_enable(          new BooleanValueProperty(this, id_sensor_telemetry_dde_enable,        "sensor_telemetry_dde_enable",         false)),
    _sensor_telemetry_bpr_enable(          new BooleanValueProperty(this, id_sensor_telemetry_bpr_enable,        "sensor_telemetry_bpr_enable",         false)),
    _sensor_telemetry_temperature_lens(    new IntValueProperty(    this, id_sensor_telemetry_temperature_lens,  "sensor_telemetry_temperature_lens",   0, MIN_VAL, MAX_VAL)),
    _sensor_telemetry_temperature_matrix(  new IntValueProperty(    this, id_sensor_telemetry_temperature_matrix,"sensor_telemetry_temperature_matrix", 0, MIN_VAL, MAX_VAL)),
    _sensor_ssettings_save_to_flash(       new CmdProperty(this, id_sensor_ssettings_save_to_flash,        "sensor_ssettings_save_to_flash")),
    _sensor_ssettings_load_from_flash(     new CmdProperty(this, id_sensor_ssettings_load_from_flash,      "sensor_ssettings_load_from_flash")),
    _sensor_ssettings_initialization_value(new CmdProperty(this, id_sensor_ssettings_initialization_value, "sensor_ssettings_initialization_value"))
{
}
void MWIRCtrlModel::set(const uint32_t & id)
{
    uint32_t channel_id = CHANNEL_ID_MWIR;
    uint32_t value{0u};
    switch(id)
    {
    case id_zoom:
        emit zoom_pos(uint32_t(_zoom->get()), channel_id);
        break;
    case id_focus:
        emit focus_pos(uint32_t(_focus->get()), channel_id);
        break;
    case id_autofocus_lens_en:
        if (_autofocus_lens_en->get()) {
            _autofocus_host_last_state = _autofocus_host_en->get();
            _autofocus_host_en->set(false);
            emit autofocus_en(channel_id);
        } else {
            emit autofocus_dis(channel_id);
            _autofocus_host_en->set(_autofocus_host_last_state);
        }
        break;
    case id_autofocus_host_en:
        if (_autofocus_lens_en->get()) {
            _autofocus_host_en->set(false);
        } else {
            _autofocus_host_pause = false;
        }
        break;
    case id_sensor_ssettings_telemetry_enable:
        value = _sensor_ssettings_telemetry_enable->get();
//        uint32_t * uvalue = reinterpret_cast<uint32_t*>(&dvalue);
//        uint32_t uvaluertwt = *uvalue;
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_telemetry_enable, value);
        break;
    case id_sensor_ssettings_soft_version:
        value = _sensor_ssettings_soft_version->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_firmware_version, value);
        break;
    case id_sensor_ssettings_telemetry_address:
        value = _sensor_ssettings_telemetry_address->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_telemetry_address, value);
        break;
    case id_sensor_msettings_cce_input:
        value = _sensor_msettings_cce_input->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_cce_input_ctrl, value);
        break;
    case id_sensor_msettings_cce_min:
        value = _sensor_msettings_cce_min->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_cce_min, value);
        break;
    case id_sensor_msettings_cce_max:
        value = _sensor_msettings_cce_max->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_cce_max, value);
        break;
    case id_sensor_msettings_pattern:
        value = _sensor_msettings_pattern->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_pattern_ctrl, value);
        break;
    case id_sensor_msettings_direction_video:
        value = _sensor_msettings_direction_video->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_direction_video, value);
        break;
    case id_sensor_msettings_black_white_video:
        value = _sensor_msettings_black_white_video->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_black_while_video, value);
        break;
    case id_sensor_msettings_gpol:
        value = _sensor_msettings_gpol->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_gpol, value);
        break;
    case id_sensor_msettings_period_time:
        value = _sensor_msettings_period_time->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_frame_period, value);
        break;
    case id_sensor_msettings_hl_0:
        value = _sensor_msettings_hl_0->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_hl0, value);
        break;
    case id_sensor_msettings_hl_1:
        value = _sensor_msettings_hl_1->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_hl1, value);
        break;
    case id_sensor_msettings_hl_2:
        value = _sensor_msettings_hl_2->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_hl2, value);
        break;
    case id_sensor_msettings_hl_3:
        value = _sensor_msettings_hl_3->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_hl3, value);
        break;
    case id_sensor_msettings_hl_4:
        value = _sensor_msettings_hl_4->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_hl4, value);
        break;
    case id_sensor_msettings_serial_number:
        value = _sensor_msettings_serial_number->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_sensor_serial_number, value);
        break;
    case id_sensor_dde_enablem:
        value = _sensor_dde_enable->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_enable, value);
        break;
    case id_sensor_dde_base_mode:
        value = _sensor_dde_base_mode->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_base_mode, value);
        break;
    case id_sensor_dde_detail_gain:
        value = _sensor_dde_detail_gain->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_detail_gain, value);
        break;
    case id_sensor_dde_detail_eps_normal:
        value = _sensor_dde_detail_eps_normal->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_detail_eps_normal, value);
        break;
    case id_sensor_dde_hist_background:
        value = _sensor_dde_hist_background->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_hist_background_thrld, value);
        break;
    case id_sensor_dde_brightness:
        value = _sensor_dde_brightness->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_brightness, value);
        break;
    case id_sensor_dde_alpha_address:
        value = _sensor_dde_alpha_address->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_alpha_addr, value);
        break;
    case id_sensor_dde_contrast:
        value = _sensor_dde_contrast->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_contrast, value);
        break;
    case id_sensor_dde_inversion:
        value = _sensor_dde_inversion->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_dde_video_inversion, value);
        break;
    case id_sensor_snr_enable:
        value = _sensor_snr_enable->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_snr_enable, value);
        break;
    case id_sensor_snr_col_eps:
        value = _sensor_snr_col_eps->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_snr_col_eps, value);
        break;
    case id_sensor_snr_row_eps:
        value = _sensor_snr_row_eps->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_snr_row_eps, value);
        break;
    case id_sensor_nuc_calibration_enable:
        value = _sensor_nuc_calibration_enable->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_nuc_calibration_enable, value);
        break;
    case id_sensor_nuc_bad_pixel_replace_enable:
        value = _sensor_nuc_bad_pixel_replace_enable->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_nuc_bad_pixel_replace_enable, value);
        break;
    case id_sensor_nuc_limit_level_out_data:
        value = _sensor_nuc_limit_level_out_data->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_nuc_limit_level_out_data, value);
        break;
    case id_sensor_nuc_lens_position:
        value = _sensor_nuc_lens_position->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_nuc_lens_position, value);
        break;
    case id_sensor_bad_pixel_rep_enable:
        value = _sensor_bad_pixel_rep_enable->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_bpr_enable, value);
        break;
    case id_sensor_bad_pixel_rep_treshold:
        value = _sensor_bad_pixel_rep_treshold->get();
        emit vdb_write_request(vdb_registers::en_vbd_mwir_camera, vdb_registers::en_vdb_mw_sensor_bpr_threshold, value);
        break;
    default:
        break;
    }
}
void MWIRCtrlModel::get(const uint32_t &)
{
}
void MWIRCtrlModel::cmd(const uint32_t & id)
{
    uint32_t channel_id = CHANNEL_ID_MWIR;
    switch(id)
    {
    case id_thermostat_on:
        qInfo(logMWIRCtrl(), "cmd: Objective Thermostat On");
        emit thermostat_on(channel_id);
        break;
    case id_thermostat_off:
        qInfo(logMWIRCtrl(), "cmd: Objective Thermostat Off");
        emit thermostat_off(channel_id);
        break;
    case id_heat_on:
        qInfo(logMWIRCtrl(), "cmd: Objective Heating On");
        emit heat_on(channel_id);
        break;
    case id_heat_off:
        qInfo(logMWIRCtrl(), "cmd: Objective Heating Off");
        emit heat_off(channel_id);
        break;
    case id_agc:
        qInfo(logMWIRCtrl(), "cmd: AGC %s", !_agc_on ? "On" : "Off");
        emit agc_switch(channel_id);
        break;
    case id_inversion:
        qInfo(logMWIRCtrl(), "cmd: Inversion MW %s", !_inversion_on ? "On" : "Off");
        emit inversion_mw();
        break;
    case id_autofocus:
        qInfo(logMWIRCtrl(), "cmd: Autofocus");
        emit autofocus(channel_id);
        break;
    case id_field_sync:
        qInfo(logMWIRCtrl(), "cmd: Field Synchronization");
        emit field_sync(channel_id);
        break;
    case id_field_sync_narrow:
        qInfo(logMWIRCtrl(), "cmd: Narrow Field Synchronization");
        emit field_sync_narrow(channel_id);
        break;
    case id_field_sync_reg:
        qInfo(logMWIRCtrl(), "cmd: Regular Field Synchronization %s", !_field_sync_reg_on ? "On" : "Off");
        emit field_sync_reg_switch(2);
        break;
    case id_zoom_in_start:
        qInfo(logMWIRCtrl(), "cmd: Zoom In start");
        emit zoom_in(1.0, channel_id);
        break;
    case id_zoom_in_stop:
        qInfo(logMWIRCtrl(), "cmd: Zoom In stop");
        emit zoom_stop(channel_id);
        break;
    case id_zoom_out_start:
        qInfo(logMWIRCtrl(), "cmd: Zoom Out start");
        emit zoom_out(1.0, channel_id);
        break;
    case id_zoom_out_stop:
        qInfo(logMWIRCtrl(), "cmd: Zoom Out stop");
        emit zoom_stop(channel_id);
        break;
    case id_iris_in_start:
        qInfo(logMWIRCtrl(), "cmd: Iris Close start");
        emit iris_close(1.0, channel_id);
        break;
    case id_iris_in_stop:
        qInfo(logMWIRCtrl(), "cmd: Iris Close stop");
        emit iris_stop(channel_id);
        break;
    case id_iris_out_start:
        qInfo(logMWIRCtrl(), "cmd: Iris Open start");
        emit iris_open(1.0, channel_id);
        break;
    case id_iris_out_stop:
        qInfo(logMWIRCtrl(), "cmd: Iris Open stop");
        emit iris_stop(channel_id);
        break;
    case id_focus_near_start:
        qInfo(logMWIRCtrl(), "cmd: Focus Near start");
        emit focus_near(1.0, channel_id);
        break;
    case id_focus_near_stop:
        qInfo(logMWIRCtrl(), "cmd: Focus Near stop");
        emit focus_stop(channel_id);
        break;
    case id_focus_far_start:
        qInfo(logMWIRCtrl(), "cmd: Focus Far start");
        emit focus_far(1.0, channel_id);
        break;
    case id_focus_far_stop:
        qInfo(logMWIRCtrl(), "cmd: Focus Far stop");
        emit focus_stop(channel_id);
        break;
    case id_sensor_ssettings_save_to_flash:
        qInfo(logMWIRCtrl(), "cmd: MW sensor settings save to flash");
//        emit focus_stop(channel_id);
        break;
    case id_sensor_ssettings_load_from_flash:
        qInfo(logMWIRCtrl(), "cmd: MW sensor settings load from flash");
//        emit focus_stop(channel_id);
        break;
    case id_sensor_ssettings_initialization_value:
        qInfo(logMWIRCtrl(), "cmd: MW sensor settings reset");
//        emit focus_stop(channel_id);
        break;
    }
}
void MWIRCtrlModel::response(Cmd)
{
}
void MWIRCtrlModel::response(Telemetry t)
{
    Telemetry::t_telemetry_struct * tel = t.get();

    _sensor_status->set(tel->mw.sensor_status);
    _lens_status->set(tel->mw.lens_status);
    _fps->set(tel->mw.fps);
    _vres->set(tel->mw.vres);
    _hres->set(tel->mw.hres);
    _exp->set(tel->mw.exp);
    _zoom_drive->set(tel->mw.zoom_drive);
    _focus_drive->set(tel->mw.focus_drive);
    _lens_temp->set(tel->mw.lens_temp);
    _focus_mm->set(tel->mw.focus_mm);
    _field_grad->set(tel->mw.field_grad);

    _agc_on = bool(tel->agc.mwir_en);
    _inversion_on = bool(tel->inversion_mw);
    _autofocus_on = bool(tel->controller.autofocus.state && (static_cast<Telemetry::camera_codes>(tel->controller.autofocus.channel) == Telemetry::mw_camera));
    _field_sync_reg_on = bool(tel->controller.field_sync.state && (static_cast<Telemetry::camera_codes>(tel->controller.field_sync.channel) == Telemetry::mw_camera));

    if(_agc_on) emit _agc->set_success();
    else emit _agc->set_neutral();
    if(_inversion_on) emit _inversion->set_success();
    else emit _inversion->set_neutral();
    if(_autofocus_on) emit _autofocus->set_success();
    else emit _autofocus->set_neutral();
    if(_field_sync_reg_on) emit _field_sync_reg->set_success();
    else emit _field_sync_reg->set_neutral();

    if(!_autofocus_lens_en->get() && _autofocus_host_en->get() && !_autofocus_host_pause)
    {
        int focus = _af_model->get_focus(_lens_temp->get(), _zoom_drive->get());
        if(abs(_focus_drive->get() - focus) > 3)
        {
            if(focus > 0) emit focus_pos(uint32_t(focus), CHANNEL_ID_MWIR);
        }
    }
}
void MWIRCtrlModel::set_autofocus_en_ui()
{
    _autofocus_lens_en->set(true);
    _autofocus_host_last_state = _autofocus_host_en->get();
    _autofocus_host_en->set(false);
}
void MWIRCtrlModel::set_autofocus_dis_ui()
{
    _autofocus_lens_en->set(false);
    _autofocus_host_en->set(_autofocus_host_last_state);
}
void MWIRCtrlModel::set_autofocus_host_pause(bool pause)
{
    _autofocus_host_pause = pause;
}
