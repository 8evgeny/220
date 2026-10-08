#ifndef CORSAIR_CMDMANAGER_H
#define CORSAIR_CMDMANAGER_H

#include "corsair_cmd.h"
#include "corsair_telemetry.h"
#include "corsair_vdb_registers.h"
#include "cmd/cmdmanager.h"

#define CORSAIR_CHANNEL_W   (1920)
#define CORSAIR_CHANNEL_H   (1080)

#define CORSAIR_CHANNEL_NAME_TVRGB              ("TVRGB")
#define CORSAIR_CHANNEL_SHOW_NAME_TVRGB         ("ТВ")
#define CORSAIR_CHANNEL_NAME_FUSION             ("FUSION")
#define CORSAIR_CHANNEL_SHOW_NAME_FUSION        ("КОМПЛЕКСИРОВАНИЕ")
#define CORSAIR_CHANNEL_NAME_SWIR               ("SWIR")
#define CORSAIR_CHANNEL_SHOW_NAME_SWIR          ("КВИК")
#define CORSAIR_CHANNEL_NAME_MWIR               ("MWIR")
#define CORSAIR_CHANNEL_SHOW_NAME_MWIR          ("ТПВ-С")
#define CORSAIR_CHANNEL_NAME_TVGRAY             ("TVGRAY")
#define CORSAIR_CHANNEL_SHOW_NAME_TVGRAY        ("ТВЧБ")
#define CORSAIR_CHANNEL_NAME_TVGRAYMWIR         ("TVGRAYMWIR")
#define CORSAIR_CHANNEL_SHOW_NAME_TVGRAYMWIR    ("ТВЧБ+ТПВ-С")
#define CORSAIR_CHANNEL_NAME_TEST               ("TEST")
#define CORSAIR_CHANNEL_SHOW_NAME_TEST          ("ТЕСТ")

#define CORSAIR_CHANNEL_ID_TVRGB        (0x00u)
#define CORSAIR_CHANNEL_ID_FUSION       (0x01u)
#define CORSAIR_CHANNEL_ID_SWIR         (0x02u)
#define CORSAIR_CHANNEL_ID_MWIR         (0x03u)
#define CORSAIR_CHANNEL_ID_TVGRAY       (0x04u)
#define CORSAIR_CHANNEL_ID_TVGRAYMWIR   (0x06u)
#define CORSAIR_CHANNEL_ID_TEST         (0x07u)
#define CORSAIR_CHANNEL_ID_NONE         (0xFFu)

#define CORSAIR_CHANNEL_TV_PIX_SIZE     (0.00345)
#define CORSAIR_CHANNEL_SWIR_PIX_SIZE   (0.020)
#define CORSAIR_CHANNEL_MWIR_PIX_SIZE   (0.015)

#define CORSAIR_CHANNEL_TV_MIN_FOCUS_MM     (8)
#define CORSAIR_CHANNEL_TV_MAX_FOCUS_MM     (120)
#define CORSAIR_CHANNEL_SWIR_MIN_FOCUS_MM   (32)
#define CORSAIR_CHANNEL_SWIR_MAX_FOCUS_MM   (320)
#define CORSAIR_CHANNEL_MWIR_MIN_FOCUS_MM   (15)
#define CORSAIR_CHANNEL_MWIR_MAX_FOCUS_MM   (330)

class Corsair_CmdManager : public CmdManager
{
public:
    Corsair_CmdManager();
    bool rcv_cmd(Corsair_Cmd & res, int& frame_number_vid);


public:
    void request(Packet& req);

    void vdb_read_request(const uint8_t dst_board, const uint8_t dst_reg, const uint16_t length);
    void vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word);
    void vdb_write_request_array(const uint8_t dst_board, const uint8_t dst_reg, const std::vector<uint8_t>& data);

    void select_channel(const uint32_t & id);
    void agc_switch(const uint32_t & channel_id, const bool & onoff);
    void field_sync_narrow(const uint32_t & channel_id);
    void field_sync(const uint32_t & channel_id);
    void field_sync_reg(const uint32_t & channel_id, const bool & onoff);
    void autofocus(const uint32_t & channel_id);
    void autofocus_en(const uint32_t & channel_id);
    void autofocus_dis(const uint32_t & channel_id);
    void swir_auto_gain();
    void swir_day_gain();
    void swir_night_gain();
    void zoom_in(const uint32_t & channel_id, const uint8_t & speed = 0xFFu);
    void zoom_out(const uint32_t & channel_id, const uint8_t & speed = 0xFFu);
    void zoom_stop(const uint32_t & channel_id);
    void zoom_pos(const uint32_t & channel_id, const uint32_t & pos);
    void focus_near(const uint32_t & channel_id, const uint8_t & speed = 0xFFu);
    void focus_far(const uint32_t & channel_id, const uint8_t & speed = 0xFFu);
    void focus_stop(const uint32_t & channel_id);
    void focus_pos(const uint32_t & channel_id, const uint32_t & pos);
    void iris_close(const uint32_t & channel_id, const uint8_t & speed = 0xFFu);
    void iris_open(const uint32_t & channel_id, const uint8_t & speed = 0xFFu);
    void iris_stop(const uint32_t & channel_id);
    void thermostat_on(const uint32_t & channel_id);
    void thermostat_off(const uint32_t & channel_id);
    void heat_on(const uint32_t & channel_id);
    void heat_off(const uint32_t & channel_id);
    void set_speed(const int32_t & x_speed, const int32_t & z_speed);
    void set_absolute_angle(const int16_t & abs_x_angle, const int16_t & abs_z_angle);
    void set_relative_angle(const int16_t & rel_x_angle, const int16_t & rel_z_angle);
    void gsp_click_control(const uint16_t & x, const uint16_t & y);
    void arretir_set();
    void arretir_reset();
    void stabilisation_on();
    void stabilisation_off();
    void park();
    void null_position();
    void pohod_position();
    void polyot_position();
    void alignment();
    void set_platform_control_by_angle();
    void set_platform_control_by_velocity();
    void rangefinder_radiation(uint16_t code);
    void rangefinder_set_frequency(uint8_t frequency);
    void rangefinder_power_on(bool enable);
    void capture_target_single(const uint32_t & channel_id, const int & x, const int & y, const int & w, const int & h);
    void free_target_single();
    void capture_target_multi(const uint32_t & channel_id, const int & target_id, const bool & fast, const int & x, const int & y, const int & w, const int & h);
    void free_target_multi(const int & target_id);
    void choose_target_multi(const int & target_id);
    void tracker_deviation(const uint16_t & x, const uint16_t & y);
    void self_test(const bool & en);
    void ping();
    void brightness(const uint32_t & channel_id, const int & brightness);
    void contrast(const uint32_t & channel_id, const double & contrast);
    void mw_inversion(const bool enable);
};

#endif // CORSAIR_CMDMANAGER_H
