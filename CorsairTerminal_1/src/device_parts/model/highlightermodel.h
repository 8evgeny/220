#ifndef HighlighterModel_H
#define HighlighterModel_H

#include <qmv/qmvmodel.h>
class BooleanValueProperty;
class IntValueProperty;
class DoubleValueProperty;
class StringValueProperty;
class TextValueProperty;

#include "device_parts/cmd/telemetry.h"

class HighlighterModel : public QMVModel
{
    Q_OBJECT
public:
    HighlighterModel(QObject * parent = nullptr, uint32_t id = 0, QString name = "");

    enum VAL_ID {
        id_status,
        id_meas_status,
        id_last_distance,
        id_lrf_correction,
        id_avg_distance,
        id_rms_distance,
        id_laser_temp,
        id_body_temp,
        id_lrf_RMS,
        id_lrf_Amp,
        id_enable,

        id_ready,
        id_power_on,
        id_radiation,
        id_cooling,
        id_body_ovrht,
        id_test_mode,
        id_tlld_ckeck,
        id_izm_test_on,
        id_tec_on,
        id_ild_set,
        id_ild_get,
        id_tstab_set,
        id_tstab_get,
        id_tbdmax_set,
        id_tbdmax_get,
        id_vavd_set,
        id_vavd_get,
        id_frequency_set,
        id_frequency_get,
        id_tavd,
        id_tpls,
        id_msg_data,
        id_msg_code,
        id_moffset_set,
        id_moffset_get,
        id_last_msg,
        id_arr_dist,

        // Work boundings
        id_dangerZone_L1_begin,
        id_dangerZone_L1_end,
        id_dangerZone_L2_begin,
        id_dangerZone_L2_end,
        id_dangerZone_L3_begin,
        id_dangerZone_L3_end,
        id_dangerZone_L4_begin,
        id_dangerZone_L4_end,
        id_dangerZone_x_base,
        id_dangerZone_x_legs,
        id_dangerZone_deltaTvLrf,
        id_dangerZone_lrfD,
        id_dangerZone_reserve,
        id_dangerZone_test_x,
        id_dangerZone_test_z
    };
    enum CMD_ID {
        id_start_mono,
        id_start_5Hz,

        id_set_mode,
        id_save_coef,
        id_msg_send,
        // Danger Zone
        id_dangerZone_onoff
    };

    void update_view();

    void set(const uint32_t &);
    void get(const uint32_t &);
    void cmd(const uint32_t &);

signals:
    void signal_send_msg(Packet& req);
    void signal_set_frequency(uint8_t frequency);
    void vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word);

    void signal_dangerZone_en(const bool en, const t_lrf_danger_zone_params params);

public slots:
    void response(Cmd);
    void response(Telemetry);

private slots:
    void slot_sentSavedData();

private:
    void send_cmd_to_lrf(const uint8_t & code, const uint32_t & value);
    uint8_t crc(const std::vector<uint8_t> & cmd);

    void dangerZone_fillStruct();
    void dangerZone_clearParams();
    void dangerZone_setEnable(const bool en);

private:
    IntValueProperty * status;
    IntValueProperty * meas_status;
    DoubleValueProperty * last_distance;
    DoubleValueProperty * lrf_correction;
    DoubleValueProperty * avg_distance;
    DoubleValueProperty * rms_distance;
    DoubleValueProperty * laser_temp;
    DoubleValueProperty * body_temp;
    DoubleValueProperty * lrf_RMS;
    DoubleValueProperty * lrf_Amp;
    BooleanValueProperty * enable;

    CmdProperty * start_mono;
    CmdProperty * start_5Hz;

    BooleanValueProperty * ready;
    BooleanValueProperty * power_on;
    BooleanValueProperty * radiation;
    BooleanValueProperty * cooling;
    BooleanValueProperty * body_ovrht;
    BooleanValueProperty * test_mode;

    BooleanValueProperty * tlld_ckeck;
    BooleanValueProperty * izm_test_on;
    BooleanValueProperty * tec_on;
    DoubleValueProperty * ild_set;
    DoubleValueProperty * ild_get;
    DoubleValueProperty * tstab_set;
    DoubleValueProperty * tstab_get;
    DoubleValueProperty * tbdmax_set;
    DoubleValueProperty * tbdmax_get;
    DoubleValueProperty * vavd_set;
    DoubleValueProperty * vavd_get;
    IntValueProperty * frequency_get;
    IntValueProperty * frequency_set;
    DoubleValueProperty * u0;
    DoubleValueProperty * tavd;
    DoubleValueProperty * tpls;
    DoubleValueProperty * msg_data;
    IntValueProperty * msg_code;
    IntValueProperty * moffset_set;
    IntValueProperty * moffset_get;
    StringValueProperty * last_msg;
    TextValueProperty * arr_dist;

    CmdProperty * set_mode;
    CmdProperty * save_coef;
    CmdProperty * msg_send;

    bool rangefinder_on;

    // Work boundings
    DoubleValueProperty * dangerZone_L1_begin;
    DoubleValueProperty * dangerZone_L1_end;
    DoubleValueProperty * dangerZone_L2_begin;
    DoubleValueProperty * dangerZone_L2_end;
    DoubleValueProperty * dangerZone_L3_begin;
    DoubleValueProperty * dangerZone_L3_end;
    DoubleValueProperty * dangerZone_L4_begin;
    DoubleValueProperty * dangerZone_L4_end;
    DoubleValueProperty * dangerZone_x_base;
    DoubleValueProperty * dangerZone_x_legs;
    DoubleValueProperty * dangerZone_deltaTvLrf;
    DoubleValueProperty * dangerZone_lrfD;
    DoubleValueProperty * dangerZone_reserve;
    CmdProperty         * dangerZone_clear;
    CmdProperty         * dangerZone_onoff;
    DoubleValueProperty * dangerZone_test_x;
    DoubleValueProperty * dangerZone_test_z;

    bool dangerZone_on { false };
    t_lrf_danger_zone_params dangerZoneParams;

};

#endif // HighlighterModel_H
