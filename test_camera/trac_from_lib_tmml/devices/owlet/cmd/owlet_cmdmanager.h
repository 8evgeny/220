#ifndef Owlet_CmdManager_H
#define Owlet_CmdManager_H

#include "cmd/cmdmanager.h"
#include "owlet_cmd_v2.h"
#include "owlet_telemetry.h"
#include "owlet_global_defines.h"

//NOTE: Owlet_CmdManager - Singleton
class Owlet_CmdManager : public CmdManager
{
public:
    Owlet_CmdManager();
private:
    void rcv_cmd();

public:
    void ping();

//    void request(Cmd & req);
    void request(Cmd_v2 & req);

    void select_channel(const uint32_t & id);
    void agc_switch(const uint32_t & channel_id, const bool & onoff);

    void lwir_brightness(const int32_t & brightness);
    void lwir_contrast(const int32_t & contrast);
    void lwir_sharpness(const int32_t & sharpness);
    void lwir_temperature();
    void lwir_right_table();
    void lwir_left_table();

    void thermostat_on(const uint32_t & channel_id);
    void thermostat_off(const uint32_t & channel_id);
    void heat_on(const uint32_t & channel_id);
    void heat_off(const uint32_t & channel_id);
    void set_speed(const int32_t & x_speed, const int32_t & z_speed);
    void set_speed(const float & x_speed, const float & z_speed);
    void set_absolute_angle(const int16_t & abs_x_angle, const int16_t & abs_z_angle);
    void set_absolute_angle(const float & abs_x_angle, const float & abs_z_angle);
    void set_relative_angle(const int16_t & rel_x_angle, const int16_t & rel_z_angle);
    void arretir_reset();
    void stabilisation_on();
    void stabilisation_off();
    void el_arr_on();
    void motor_off();
    void park();
    void unpark();
    void set_platform_control_by_angle();
    void set_platform_control_by_velocity();
    void set_manual_drift_comp(const int16_t & drift_x, const int16_t & drift_z);
    void set_manual_drift_comp(const int32_t & drift);
    void set_gsp_algorithm(const uint32_t & val);

    void write_data_to_lwir_camera(const uint8_t &cmd_code, const uint8_t &status_code, const uint16_t &addr, const int32_t &data);
private:
    EthController * ethController;
};

#endif // Owlet_CmdManager_H
