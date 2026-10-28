#include "highlightermodel.h"

#include <QVariant>
#include <QTimer>
#include <qmv/property/booleanvalueproperty.h>
#include <qmv/property/doublevalueproperty.h>
#include <qmv/property/intvalueproperty.h>
#include <qmv/property/cmdproperty.h>
#include <qmv/property/stringvalueproperty.h>
#include <qmv/property/textvalueproperty.h>

#include "device_parts/cmd/vdb_registers.h"

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logHighlighterModel, "HighlighterModel")

HighlighterModel::HighlighterModel(QObject * parent, uint32_t id, QString name) :
    QMVModel(parent, id, name),
    status(new IntValueProperty(this, id_status, "status", 0, 0, 655360)),
    meas_status(new IntValueProperty(this, id_meas_status, "meas_status", 0, 0, 655360)),
    last_distance(new DoubleValueProperty(this, id_last_distance, "last_distance", 0, 0, 655360)),
    lrf_correction(new DoubleValueProperty(this, id_lrf_correction, "lrf_correction", 0, 0, 655360)),
    avg_distance(new DoubleValueProperty(this, id_avg_distance, "avg_distance", 0, 0, 655360)),
    rms_distance(new DoubleValueProperty(this, id_rms_distance, "rms_distance", 0, 0, 655360)),
    laser_temp(new DoubleValueProperty(this, id_laser_temp, "laser_temp", 0, -655360, 655360)),
    body_temp(new DoubleValueProperty(this, id_body_temp, "body_temp", 0, -655360, 655360)),
    lrf_RMS(new DoubleValueProperty(this, id_lrf_RMS, "lrf_RMS", 0, -655360, 655360)),
    lrf_Amp(new DoubleValueProperty(this, id_lrf_Amp, "lrf_Amp", 0, -655360, 655360)),
    enable(new BooleanValueProperty(this, id_enable, "enable", false)),
    start_mono(new CmdProperty(this, id_start_mono, "start_mono")),
    start_5Hz(new CmdProperty(this, id_start_5Hz, "start_5Hz")),

    ready(new BooleanValueProperty(this, id_ready, "ready", false)),
    power_on(new BooleanValueProperty(this, id_power_on, "power_on", false)),
    radiation(new BooleanValueProperty(this, id_radiation, "radiation", false)),
    cooling(new BooleanValueProperty(this, id_cooling, "cooling", false)),
    body_ovrht(new BooleanValueProperty(this, id_body_ovrht, "body_ovrht", false)),
    test_mode(new BooleanValueProperty(this, id_test_mode, "test_mode", false)),
//    lrf_on(new BooleanValueProperty(this, id_lrf_on, "lrf_on", false)),
//    pause_on(new BooleanValueProperty(this, id_pause_on, "pause_on", false)),
//    bd_ovrht(new BooleanValueProperty(this, id_bd_ovrht, "bd_ovrht", false)),
//    _tec_on(new BooleanValueProperty(this, id__tec_on, "_tec_on", false)),
//    mode0(new BooleanValueProperty(this, id_mode0, "mode0", false)),
//    mode1(new BooleanValueProperty(this, id_mode1, "mode1", false)),
//    no_izm(new BooleanValueProperty(this, id_no_izm, "no_izm", false)),
//    t_check(new BooleanValueProperty(this, id_t_check, "t_check", false)),
    tlld_ckeck(new BooleanValueProperty(this, id_tlld_ckeck, "tlld_ckeck", false)),
    izm_test_on(new BooleanValueProperty(this, id_izm_test_on, "izm_test_on", false)),
    tec_on(new BooleanValueProperty(this, id_tec_on, "tec_on", false)),

    ild_set(new DoubleValueProperty(this, id_ild_set, "ild_set", 0, -655360, 655360)),
    ild_get(new DoubleValueProperty(this, id_ild_get, "ild_get", 0, -655360, 655360)),
    tstab_set(new DoubleValueProperty(this, id_tstab_set, "tstab_set", 0, -655360, 655360)),
    tstab_get(new DoubleValueProperty(this, id_tstab_get, "tstab_get", 0, -655360, 655360)),
    tbdmax_set(new DoubleValueProperty(this, id_tbdmax_set, "tbdmax_set", 0, -655360, 655360)),
    tbdmax_get(new DoubleValueProperty(this, id_tbdmax_get, "tbdmax_get", 0, -655360, 655360)),
    vavd_set(new DoubleValueProperty(this, id_vavd_set, "vavd_set", 0, 0, 55)),
    vavd_get(new DoubleValueProperty(this, id_vavd_get, "vavd_get", 0, -655360, 655360)),
    frequency_get(new IntValueProperty(this, id_frequency_get, "frequency_get", 0, -655360, 655360)),
    frequency_set(new IntValueProperty(this, id_frequency_set, "frequency_set", 1, 1, 30)),
    tavd(new DoubleValueProperty(this, id_tavd, "tavd", 0, -655360, 655360)),
    tpls(new DoubleValueProperty(this, id_tpls, "tpls", 0, -655360, 655360)),
    msg_data(new DoubleValueProperty(this, id_msg_data, "msg_data", 0, -655360, 655360)),

    msg_code(new IntValueProperty(this, id_msg_code, "msg_code", 0, -655360, 655360)),
    moffset_set(new IntValueProperty(this, id_moffset_set, "moffset_set", 0, -655360, 655360)),
    moffset_get(new IntValueProperty(this, id_moffset_get, "moffset_get", 0, -655360, 655360)),

    last_msg(new StringValueProperty(this, id_last_msg, "last_msg", "")),
    arr_dist(new TextValueProperty(this, id_arr_dist, "arr_dist", "")),

    set_mode(new CmdProperty(this, id_set_mode, "set_mode")),
    save_coef(new CmdProperty(this, id_save_coef, "save_coef")),
    msg_send(new CmdProperty(this, id_msg_send, "msg_send")),

    rangefinder_on(false),

    // Work boundings
    dangerZone_L1_begin     (new DoubleValueProperty(this, id_dangerZone_L1_begin   , "dangerZone_L1_begin"     , 0, -655360, 655360)),
    dangerZone_L1_end       (new DoubleValueProperty(this, id_dangerZone_L1_end     , "dangerZone_L1_end"       , 0, -655360, 655360)),
    dangerZone_L2_begin     (new DoubleValueProperty(this, id_dangerZone_L2_begin   , "dangerZone_L2_begin"     , 0, -655360, 655360)),
    dangerZone_L2_end       (new DoubleValueProperty(this, id_dangerZone_L2_end     , "dangerZone_L2_end"       , 0, -655360, 655360)),
    dangerZone_L3_begin     (new DoubleValueProperty(this, id_dangerZone_L3_begin   , "dangerZone_L3_begin"     , 0, -655360, 655360)),
    dangerZone_L3_end       (new DoubleValueProperty(this, id_dangerZone_L3_end     , "dangerZone_L3_end"       , 0, -655360, 655360)),
    dangerZone_L4_begin     (new DoubleValueProperty(this, id_dangerZone_L4_begin   , "dangerZone_L4_begin"     , 0, -655360, 655360)),
    dangerZone_L4_end       (new DoubleValueProperty(this, id_dangerZone_L4_end     , "dangerZone_L4_end"       , 0, -655360, 655360)),
    dangerZone_x_base       (new DoubleValueProperty(this, id_dangerZone_x_base     , "dangerZone_x_base"       , 0, -655360, 655360)),
    dangerZone_x_legs       (new DoubleValueProperty(this, id_dangerZone_x_legs     , "dangerZone_x_legs"       , 0, -655360, 655360)),
    dangerZone_deltaTvLrf   (new DoubleValueProperty(this, id_dangerZone_deltaTvLrf , "dangerZone_deltaTvLrf"   , 0, -655360, 655360)),
    dangerZone_lrfD         (new DoubleValueProperty(this, id_dangerZone_lrfD       , "dangerZone_lrfD"         , 0, -655360, 655360)),
    dangerZone_reserve      (new DoubleValueProperty(this, id_dangerZone_reserve    , "dangerZone_reserve"      , 0, -655360, 655360)),
    dangerZone_onoff        (new CmdProperty        (this, id_dangerZone_onoff      , "dangerZone_onoff")),
    dangerZone_test_x       (new DoubleValueProperty(this, id_dangerZone_test_x     , "dangerZone_test_x"       , 0, -655360, 655360)),
    dangerZone_test_z       (new DoubleValueProperty(this, id_dangerZone_test_z     , "dangerZone_test_z"       , 0, -655360, 655360))
{

    QTimer::singleShot(500, this, &HighlighterModel::slot_sentSavedData);
}
void HighlighterModel::update_view()
{
    QMVModel::update_view();

    arr_dist->clear();
    last_msg->set("");
}
void HighlighterModel::set(const uint32_t & id)
{
    switch(id)
    {
    case id_enable: break;
    case id_ready:break;
    case id_power_on:break;
    case id_radiation:break;
    case id_cooling:break;
    case id_body_ovrht:break;
    case id_test_mode:break;
    case id_tlld_ckeck: break;
    case id_izm_test_on: break;
    case id_tec_on:  break;
    case id_ild_set: break;
    case id_ild_get: break;
    case id_tstab_set: break;
    case id_tstab_get: break;
    case id_tbdmax_set: break;
    case id_tbdmax_get: break;
    case id_vavd_set: {
        float dvalue = float(vavd_set->get());
        uint32_t * uvalue = reinterpret_cast<uint32_t*>(&dvalue);
        uint32_t uvaluertwt = *uvalue;
        emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_vavd, uvaluertwt);
    } break;
    case id_vavd_get:  break;
    case id_frequency_set: {
        uint32_t uvalue = static_cast<uint32_t>(frequency_set->get());
        emit signal_set_frequency(uvalue);
    } break;
    case id_frequency_get: break;
    case id_tavd:break;
    case id_tpls:break;
    case id_msg_data:break;
    case id_msg_code:break;
    case id_moffset_set:break;
    case id_moffset_get:break;
    case id_last_msg:break;
    case id_arr_dist:break;
    // Work boundings
    case id_dangerZone_L1_begin: break;
    case id_dangerZone_L1_end: break;
    case id_dangerZone_L2_begin: break;
    case id_dangerZone_L2_end: break;
    case id_dangerZone_L3_begin: break;
    case id_dangerZone_L3_end: break;
    case id_dangerZone_L4_begin: break;
    case id_dangerZone_L4_end: break;
    case id_dangerZone_x_base: break;
    case id_dangerZone_x_legs: break;
    }
}
void HighlighterModel::get(const uint32_t & id)
{
    switch(id)
    {
    case id_ready:break;
    case id_power_on:break;
    case id_radiation:break;
    case id_cooling:break;
    case id_body_ovrht:break;
    case id_test_mode:break;
    case id_tlld_ckeck:break;
    case id_izm_test_on:break;
    case id_tec_on:break;
    case id_ild_set:break;
    case id_ild_get: break;
    case id_tstab_set:break;
    case id_tstab_get: break;
    case id_tbdmax_set:break;
    case id_tbdmax_get: break;
    case id_vavd_set:break;
    case id_vavd_get: {
        emit vdb_write_request(vdb_registers::en_vbd_LRF, vdb_registers::en_vdb_lrf_telemetry_period, true);
    } break;
    case id_frequency_set:break;
    case id_frequency_get:break;
    case id_tavd:break;
    case id_tpls:break;
    case id_msg_data:break;
    case id_msg_code:break;
    case id_moffset_set:break;
    case id_moffset_get:break;
    case id_last_msg:break;
    case id_arr_dist:break;
    // Work boundings
    case id_dangerZone_L1_begin: break;
    case id_dangerZone_L1_end: break;
    case id_dangerZone_L2_begin: break;
    case id_dangerZone_L2_end: break;
    case id_dangerZone_L3_begin: break;
    case id_dangerZone_L3_end: break;
    case id_dangerZone_L4_begin: break;
    case id_dangerZone_L4_end: break;
    case id_dangerZone_x_base: break;
    case id_dangerZone_x_legs: break;
    }
}
void HighlighterModel::cmd(const uint32_t & id)
{
    switch(id)
    {
    case id_start_mono: break;
    case id_start_5Hz: break;
    case id_set_mode: break;
    case id_save_coef: break;
    case id_msg_send: break;
    case id_dangerZone_onoff:
    {
        if(dangerZone_on) {
            dangerZone_clearParams();
        } else {
            dangerZone_fillStruct();
            dangerZone_setEnable(true);
        }
        emit signal_dangerZone_en(dangerZone_on, dangerZoneParams);
    } break;
    }
}
void HighlighterModel::response(Cmd)
{
}
void HighlighterModel::response(Telemetry t)
{
    Telemetry::t_telemetry_struct * tel = t.get();

    status->set_val(tel->rf.status);
    meas_status->set(tel->rf.meas_status);
    last_distance->set(tel->rf.last_distance);
    lrf_correction->set(tel->dev.lrf_stat.lrf_correction);
    avg_distance->set(tel->rf.avg_distance);
    rms_distance->set(tel->rf.rms_distance);
//    laser_temp->set(tel->rf.laser_temp);
    lrf_RMS->set(tel->dev.lrf_stat.lrf_RMS);
    lrf_Amp->set(tel->dev.lrf_stat.lrf_Amp);
    body_temp->set(tel->rf.body_temp);

    Telemetry::t_union_rf_state rf_state;
    rf_state.word = tel->rf.state;
    ready->set(rf_state.flags.ready);
    power_on->set(rf_state.flags.power_on);
    radiation->set(rf_state.flags.radiation);
    cooling->set(rf_state.flags.cooling);
    body_ovrht->set(rf_state.flags.body_ovrht);
    test_mode->set(rf_state.flags.test_mode);


    // VAVD
    float fval = tel->dev.lrf_stat.vavd;
    vavd_get->set(fval);
    // freq
    int32_t freq = tel->dev.lrf_stat.frequency;
    frequency_get->set(freq);
    // U0
}

void HighlighterModel::slot_sentSavedData()
{
    dangerZone_fillStruct();
    dangerZone_setEnable(false);
    emit signal_dangerZone_en(dangerZone_on, dangerZoneParams);
}

void HighlighterModel::dangerZone_fillStruct()
{
    dangerZoneParams.leg1_begin = dangerZone_L1_begin->get();
    dangerZoneParams.leg1_end   = dangerZone_L1_end->get();

    dangerZoneParams.leg2_begin = dangerZone_L2_begin->get();
    dangerZoneParams.leg2_end   = dangerZone_L2_end->get();

    dangerZoneParams.leg3_begin = dangerZone_L3_begin->get();
    dangerZoneParams.leg3_end   = dangerZone_L3_end->get();

    dangerZoneParams.leg4_begin = dangerZone_L4_begin->get();
    dangerZoneParams.leg4_end   = dangerZone_L4_end->get();

    dangerZoneParams.x_base     = dangerZone_x_base->get();
    dangerZoneParams.x_legs     = dangerZone_x_legs->get();

    dangerZoneParams.deltaTvLrf = dangerZone_deltaTvLrf->get();
    dangerZoneParams.lrfDiameter= dangerZone_lrfD->get();
    dangerZoneParams.zoneReserve= dangerZone_reserve->get();

    dangerZoneParams.test_x     = dangerZone_test_x->get();
    dangerZoneParams.test_z     = dangerZone_test_z->get();
}

void HighlighterModel::dangerZone_setEnable(const bool en)
{
    if(en) {
        // Danger Zone now is enable
        dangerZone_onoff->set_text("Выключить ограничение");
    } else {
        // Danger Zone now is disable
        dangerZone_onoff->set_text("Включить ограничение");
    }
    dangerZone_on = en;
}

void HighlighterModel::dangerZone_clearParams()
{
    dangerZoneParams.leg1_begin = 0.0;
    dangerZoneParams.leg1_end   = 0.0;

    dangerZoneParams.leg2_begin = 0.0;
    dangerZoneParams.leg2_end   = 0.0;

    dangerZoneParams.leg3_begin = 0.0;
    dangerZoneParams.leg3_end   = 0.0;

    dangerZoneParams.leg4_begin = 0.0;
    dangerZoneParams.leg4_end   = 0.0;

    dangerZoneParams.x_base     = 0.0;
    dangerZoneParams.x_legs     = 0.0;

    dangerZoneParams.deltaTvLrf = 0.0;
    dangerZoneParams.lrfDiameter= 0.0;
    dangerZoneParams.zoneReserve= 0.0;

    dangerZoneParams.test_x = 0.0;
    dangerZoneParams.test_z = 0.0;

    dangerZone_setEnable(false);
}

