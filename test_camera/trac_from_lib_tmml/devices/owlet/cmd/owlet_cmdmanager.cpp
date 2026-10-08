#include "owlet_cmdmanager.h"
#include "../eth/owlet_ethcontroller.h"
#include "owlet_global_defines.h"

#define BUFFER_LEN (512)
static uint8_t buffer[BUFFER_LEN];

//Owlet_CmdManager & Owlet_CmdManager::inst()
//{
//    static Owlet_CmdManager inst;
//    return inst;
//}
Owlet_CmdManager::Owlet_CmdManager()
{
    ethController = &Owlet_EthController::inst();
}
void Owlet_CmdManager::rcv_cmd()
{

}

void Owlet_CmdManager::ping()
{
    return void();
}

//void Owlet_CmdManager::request(Cmd & req)
//{
//    static uint32_t cnt = 0;
//    uint32_t buffer_len = req.to_buffer(buffer);
//    ethController->send_cmd(buffer, buffer_len);
//    ++cnt;
//}

void Owlet_CmdManager::request(Cmd_v2 &req)
{
    static uint32_t cnt = 0;
    uint32_t buffer_len = req.to_buffer(buffer);
    ethController->send_cmd(buffer, buffer_len);
    ++cnt;
}
void Owlet_CmdManager::select_channel(const uint32_t & id)
{
    /*Cmd cmd(CH_ENABLE, 0);
    cmd.append_data(uint8_t(id));
    cmd.append_data(uint8_t(1));
    request(cmd);*/

    Cmd_v2 cmd(g_dest_stream);
    cmd.append_data(id);
    request(cmd);
}
void Owlet_CmdManager::agc_switch(const uint32_t & channel_id, const bool & onoff)
{
//    Cmd cmd(CONTR_ENHANCEMENT);
//    cmd.append_data(uint8_t(channel_id));
//    cmd.append_data(uint8_t(onoff));
//    request(cmd);
    Cmd_v2 cmd(g_agc);
    uint32_t data = onoff ? 1 : 0;
    cmd.append_data(data);
    request(cmd);
}
void Owlet_CmdManager::lwir_brightness(const int32_t & brightness)
{
    auto bright = uint32_t(brightness);
    Cmd_v2 cmd(lwir_addr_dde_brightness, owlet_vdb_cmd_request_write, owlet_vdb_st_request_reply);
    cmd.append_data(uint8_t((bright >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 16) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::lwir_contrast(const int32_t &contrast)
{
    auto bright = uint32_t(contrast);
    Cmd_v2 cmd(lwir_addr_dde_contrast, owlet_vdb_cmd_request_write, owlet_vdb_st_request_reply);
    cmd.append_data(uint8_t((bright >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 16) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::lwir_sharpness(const int32_t &sharpness)
{
    auto bright = uint32_t(sharpness);
    Cmd_v2 cmd(lwir_addr_dde_gain_max, owlet_vdb_cmd_request_write, owlet_vdb_st_request_reply);
    cmd.append_data(uint8_t((bright >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 16) & 0xFFu));
    cmd.append_data(uint8_t((bright >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::lwir_temperature()
{
    uint32_t buf = 0;
    Cmd_v2 cmd(lwir_addr_temperature, owlet_vdb_cmd_request_read, owlet_vdb_st_request_reply);
    cmd.append_data(uint8_t((buf >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 16) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::lwir_right_table()
{
    uint32_t buf = 0;
    Cmd_v2 cmd(lwir_addr_left_table_code, owlet_vdb_cmd_request_read, owlet_vdb_st_request_reply);
    cmd.append_data(uint8_t((buf >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 16) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::lwir_left_table()
{
    uint32_t buf = 0;
    Cmd_v2 cmd(lwir_addr_right_table_code, owlet_vdb_cmd_request_read, owlet_vdb_st_request_reply);
    cmd.append_data(uint8_t((buf >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 16) & 0xFFu));
    cmd.append_data(uint8_t((buf >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::thermostat_on(const uint32_t & channel_id)
{
//    enum PCCOM lens = PCCOM_NOP;
//    switch (channel_id)
//    {
//    case CHANNEL_ID_TVRGB: lens = PCCOM_TV_LENS; break;
//    //case CHANNEL_ID_SWIR: lens = PCCOM_SWIR_LENS; break;
//    case CHANNEL_ID_MWIR: lens = PCCOM_LWIR_LENS; break;
//    default: return;
//    }
//    Cmd cmd_old(lens);
//    cmd_old.append_data(uint8_t(0x30));
//    uint32_t buffer_len = cmd_old.to_buffer(buffer);
//    Cmd cmd(DESIGNER_CMD);
//    cmd.append_data(buffer, buffer_len);
//    request(cmd);
}
void Owlet_CmdManager::thermostat_off(const uint32_t & channel_id)
{
//    enum PCCOM lens = PCCOM_NOP;
//    switch (channel_id)
//    {
//    case CHANNEL_ID_TVRGB: lens = PCCOM_TV_LENS; break;
//    //case CHANNEL_ID_SWIR: lens = PCCOM_SWIR_LENS; break;
//    case CHANNEL_ID_MWIR: lens = PCCOM_LWIR_LENS; break;
//    default: return;
//    }
//    Cmd cmd_old(lens);
//    cmd_old.append_data(uint8_t(0x31));
//    uint32_t buffer_len = cmd_old.to_buffer(buffer);
//    Cmd cmd(DESIGNER_CMD);
//    cmd.append_data(buffer, buffer_len);
//    request(cmd);
}
void Owlet_CmdManager::heat_on(const uint32_t & channel_id)
{
//    enum PCCOM lens = PCCOM_NOP;
//    switch (channel_id)
//    {
//    case CHANNEL_ID_TVRGB: lens = PCCOM_TV_LENS; break;
//    //case CHANNEL_ID_SWIR: lens = PCCOM_SWIR_LENS; break;
//    case CHANNEL_ID_MWIR: lens = PCCOM_LWIR_LENS; break;
//    default: return;
//    }
//    Cmd cmd_old(lens);
//    cmd_old.append_data(uint8_t(0x10));
//    uint32_t buffer_len = cmd_old.to_buffer(buffer);
//    Cmd cmd(DESIGNER_CMD);
//    cmd.append_data(buffer, buffer_len);
//    request(cmd);
}
void Owlet_CmdManager::heat_off(const uint32_t & channel_id)
{
//    enum PCCOM lens = PCCOM_NOP;
//    switch (channel_id)
//    {
//    case CHANNEL_ID_TVRGB: lens = PCCOM_TV_LENS; break;
//    //case CHANNEL_ID_SWIR: lens = PCCOM_SWIR_LENS; break;
//    case CHANNEL_ID_MWIR: lens = PCCOM_LWIR_LENS; break;
//    default: return;
//    }
//    Cmd cmd_old(lens);
//    cmd_old.append_data(uint8_t(0x11));
//    uint32_t buffer_len = cmd_old.to_buffer(buffer);
//    Cmd cmd(DESIGNER_CMD);
//    cmd.append_data(buffer, buffer_len);
//    request(cmd);
}
void Owlet_CmdManager::set_speed(const int32_t & _x_speed, const int32_t & _z_speed)
{
//    auto x_speed = uint32_t(_x_speed);
//    auto z_speed = uint32_t(_z_speed);
//    Cmd cmd(SET_SPEED, 0x1234);
//    cmd.append_data(uint8_t((z_speed >> 0 ) & 0xFFu));
//    cmd.append_data(uint8_t((z_speed >> 8 ) & 0xFFu));
//    cmd.append_data(uint8_t((z_speed >> 16) & 0xFFu));
//    cmd.append_data(uint8_t((x_speed >> 0 ) & 0xFFu));
//    cmd.append_data(uint8_t((x_speed >> 8 ) & 0xFFu));
//    cmd.append_data(uint8_t((x_speed >> 16) & 0xFFu));
//    request(cmd);

    auto x_speed = uint32_t(_x_speed);
    auto z_speed = uint32_t(_z_speed);
    Cmd_v2 cmd(gsp_addr_target_speed_z);
    cmd.append_data(uint8_t((z_speed >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((z_speed >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((z_speed >> 16) & 0xFFu));
    cmd.append_data(uint8_t((z_speed >> 24) & 0xFFu));
    cmd.append_data(uint8_t((x_speed >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((x_speed >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((x_speed >> 16) & 0xFFu));
    cmd.append_data(uint8_t((x_speed >> 24) & 0xFFu));
    request(cmd);
}

void Owlet_CmdManager::set_speed(const float &_x_speed, const float &_z_speed)
{
    auto p_x_speed = reinterpret_cast<const uint32_t*>(&_x_speed);
    auto p_z_speed = reinterpret_cast<const uint32_t*>(&_z_speed);
    Cmd_v2 cmd(gsp_addr_target_speed_z);
    cmd.append_data(uint8_t((p_z_speed[0] >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((p_z_speed[0] >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((p_z_speed[0] >> 16) & 0xFFu));
    cmd.append_data(uint8_t((p_z_speed[0] >> 24) & 0xFFu));
    cmd.append_data(uint8_t((p_x_speed[0] >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((p_x_speed[0] >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((p_x_speed[0] >> 16) & 0xFFu));
    cmd.append_data(uint8_t((p_x_speed[0] >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::set_absolute_angle(const int16_t & _abs_x_angle, const int16_t & _abs_z_angle)
{
    auto abs_x_angle = uint16_t(_abs_x_angle);
    auto abs_z_angle = uint16_t(_abs_z_angle);
//    Cmd cmd(SET_ABS_ANGLE, 0x1234);
//    cmd.append_data(uint8_t((abs_z_angle >> 0 ) & 0xFFu));
//    cmd.append_data(uint8_t((abs_z_angle >> 8 ) & 0xFFu));
//    cmd.append_data(uint8_t((abs_x_angle >> 0 ) & 0xFFu));
//    cmd.append_data(uint8_t((abs_x_angle >> 8 ) & 0xFFu));
//    request(cmd);
    Cmd_v2 cmd(gsp_addr_target_angle_z);
    cmd.append_data(uint8_t((abs_z_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_z_angle >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_z_angle >> 16) & 0xFFu));
    cmd.append_data(uint8_t((abs_z_angle >> 24) & 0xFFu));
    cmd.append_data(uint8_t((abs_x_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_x_angle >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_x_angle >> 16) & 0xFFu));
    cmd.append_data(uint8_t((abs_x_angle >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::set_absolute_angle(const float &abs_x_angle, const float &abs_z_angle)
{
    auto p_x_angle = reinterpret_cast<const uint32_t*>(&abs_x_angle);
    auto p_z_angle = reinterpret_cast<const uint32_t*>(&abs_z_angle);
    Cmd_v2 cmd(gsp_addr_target_angle_z);
    cmd.append_data(uint8_t((p_z_angle[0] >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((p_z_angle[0] >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((p_z_angle[0] >> 16) & 0xFFu));
    cmd.append_data(uint8_t((p_z_angle[0] >> 24) & 0xFFu));
    cmd.append_data(uint8_t((p_x_angle[0] >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((p_x_angle[0] >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((p_x_angle[0] >> 16) & 0xFFu));
    cmd.append_data(uint8_t((p_x_angle[0] >> 24) & 0xFFu));
    request(cmd);
}
void Owlet_CmdManager::set_relative_angle(const int16_t & _rel_x_angle, const int16_t & _rel_z_angle)
{
//    auto rel_x_angle = uint16_t(_rel_x_angle);
//    auto rel_z_angle = uint16_t(_rel_z_angle);
//    Cmd cmd(SET_ANGLE, 0x1234);
//    cmd.append_data(uint8_t((rel_z_angle >> 0 ) & 0xFFu));
//    cmd.append_data(uint8_t((rel_z_angle >> 8 ) & 0xFFu));
//    cmd.append_data(uint8_t((rel_x_angle >> 0 ) & 0xFFu));
//    cmd.append_data(uint8_t((rel_x_angle >> 8 ) & 0xFFu));
//    request(cmd);
}
void Owlet_CmdManager::arretir_reset()
{
//    Cmd cmd(ARR_SET, 0x1234);
//    cmd.append_data(uint8_t(0x00u));
//    request(cmd);
    Cmd_v2 cmd(gsp_addr_target_position);
    cmd.append_data(uint32_t((all_ax << 16) | null_position));
    request(cmd);
}
void Owlet_CmdManager::stabilisation_on()
{
    //Cmd cmd(STAB_EN, 0x1234);
    //cmd.append_data(uint8_t(0x01u));
    //request(cmd);
    Cmd_v2 cmd(gsp_addr_stab);
    cmd.append_data(uint32_t((all_ax << 16) | on));
    request(cmd);
}
void Owlet_CmdManager::stabilisation_off()
{
    //Cmd cmd(STAB_EN, 0x1234);
    //cmd.append_data(uint8_t(0x00u));
    //request(cmd);
    Cmd_v2 cmd(gsp_addr_stab);
    cmd.append_data(uint32_t((all_ax << 16) | off));
    request(cmd);
}
void Owlet_CmdManager::el_arr_on()
{
    Cmd_v2 cmd(gsp_addr_el_arr);
    cmd.append_data(uint32_t((all_ax << 16) | on));
    request(cmd);
}
void Owlet_CmdManager::motor_off()
{
    Cmd_v2 cmd(gsp_addr_el_arr);
    cmd.append_data(uint32_t((all_ax << 16) | off));
    request(cmd);
}
void Owlet_CmdManager::park()
{
//    Cmd cmd(PARK, 0x1234);
//    cmd.append_data(uint8_t(0x01u));
//    request(cmd);
    Cmd_v2 cmd(gsp_addr_target_position);
    cmd.append_data(uint32_t((all_ax << 16) | park_position));
    request(cmd);
}
void Owlet_CmdManager::unpark()
{
//    Cmd cmd(PARK, 0x1234);
//    cmd.append_data(uint8_t(0x00u));
//    request(cmd);
    //NOTE: Разпарковка осуществляется переводом платформы в абсолютное нулевое положение
//    set_absolute_angle(0.0f, 0.0f);
    Cmd_v2 cmd(gsp_addr_target_position);
    cmd.append_data(uint32_t((all_ax << 16) | null_position));
    request(cmd);
}
void Owlet_CmdManager::set_platform_control_by_angle()
{
//    Cmd cmd(CONTROL_MODE, 0x1234);
//    cmd.append_data(uint8_t(0x01u));
//    request(cmd);
}
void Owlet_CmdManager::set_platform_control_by_velocity()
{
//    Cmd cmd(CONTROL_MODE, 0x1234);
//    cmd.append_data(uint8_t(0x00u));
//    request(cmd);
}

void Owlet_CmdManager::set_manual_drift_comp(const int16_t & drift_x, const int16_t & drift_z)
{
    Cmd_v2 cmd(gsp_addr_drift_comp);
    cmd.append_data(uint32_t((drift_z << 16) | drift_x));
    request(cmd);
}

void Owlet_CmdManager::set_manual_drift_comp(const int32_t &drift)
{
    Cmd_v2 cmd(gsp_addr_drift_comp);
    cmd.append_data(uint32_t(drift));
    request(cmd);
}

void Owlet_CmdManager::set_gsp_algorithm(const uint32_t & val)
{
    Cmd_v2 cmd(gsp_addr_state);
    cmd.append_data(val);
    request(cmd);
}
void Owlet_CmdManager::write_data_to_lwir_camera(const uint8_t &cmd_code, const uint8_t &status_code, const uint16_t &addr, const int32_t &data)
{
    auto p_data = reinterpret_cast<const uint32_t*>(&data);
    Cmd_v2 cmd(addr, cmd_code, status_code);
    cmd.append_data(uint8_t((p_data[0] >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((p_data[0] >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((p_data[0] >> 16) & 0xFFu));
    cmd.append_data(uint8_t((p_data[0] >> 24) & 0xFFu));
    request(cmd);
}
