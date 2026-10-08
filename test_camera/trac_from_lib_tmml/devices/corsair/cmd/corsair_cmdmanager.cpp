#include "corsair_cmdmanager.h"
#include "../eth/corsair_ethcontroller.h"


#define CORSAIR_BUFFER_LEN (512)
static uint8_t buffer[CORSAIR_BUFFER_LEN];

Corsair_CmdManager::Corsair_CmdManager()
{
    ethController = &Corsair_EthController::inst();
}

bool Corsair_CmdManager::rcv_cmd(Corsair_Cmd & res, int& frame_number_vid)
{
    int _cmd_res_len;
    uint8_t * _cmd_res = ethController->rcv_cmd(_cmd_res_len);
    if(_cmd_res == nullptr) return false;
    frame_number_vid = ethController->get_frame_num();
    return res.from_buffer(_cmd_res, _cmd_res_len);
}

void Corsair_CmdManager::request(Packet & req)
{
    static uint32_t cnt = 0;
    uint32_t buffer_len = req.to_buffer(buffer);
    ethController->send_cmd(buffer, CORSAIR_BUFFER_LEN);
    ++cnt;
}
void Corsair_CmdManager::vdb_read_request(const uint8_t dst_board, const uint8_t dst_reg, const uint16_t length)
{
    const uint8_t cmd = 0x00;
    const uint8_t status = 0x80;
    const uint16_t dst_address = (dst_board << 8) | dst_reg;
    Packet packet(cmd, status, dst_address, length);
    request(packet);

}
void Corsair_CmdManager::vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word)
{
    const uint8_t cmd = 0x00;
    const uint8_t status = 0x00;
    const uint16_t dst_address = (dst_board << 8) | dst_reg;
    Packet packet(cmd, status, dst_address);
    packet.append_data(word);
    request(packet);
}
void Corsair_CmdManager::vdb_write_request_array(const uint8_t dst_board, const uint8_t dst_reg, const std::vector<uint8_t>& data)
{
    const uint8_t cmd = 0x00;
    const uint8_t status = 0x00;
    const uint16_t dst_address = (dst_board << 8) | dst_reg;
    Packet packet(cmd, status, dst_address);
    packet.append_data(data);
    request(packet);
}
void Corsair_CmdManager::select_channel(const uint32_t & id)
{
    Corsair_Cmd cmd(CH_ENABLE, 0);
    cmd.append_data(uint8_t(id));
    cmd.append_data(uint8_t(1));
    request(cmd);
}

void Corsair_CmdManager::agc_switch(const uint32_t & CORSAIR_CHANNEL_id, const bool & onoff)
{
    Corsair_Cmd cmd(CONTR_ENHANCEMENT);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(uint8_t(onoff));
    request(cmd);
}
// FIXME
void Corsair_CmdManager::field_sync_narrow(const uint32_t & CORSAIR_CHANNEL_id)
{
    Corsair_Cmd cmd(FIELD_SYNC_NARROW);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void Corsair_CmdManager::field_sync(const uint32_t & CORSAIR_CHANNEL_id)
{
    Corsair_Cmd cmd(FIELD_SYNC);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    request(cmd);
}
void Corsair_CmdManager::field_sync_reg(const uint32_t & CORSAIR_CHANNEL_id, const bool & onoff)
{
    Corsair_Cmd cmd(FIELD_SYNC_REG);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(uint8_t(onoff));
    request(cmd);
}
void Corsair_CmdManager::autofocus(const uint32_t & CORSAIR_CHANNEL_id)
{
    Corsair_Cmd cmd(AUTOFOCUS);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    request(cmd);
}
void Corsair_CmdManager::autofocus_en(const uint32_t & CORSAIR_CHANNEL_id)
{
    //    enum PCCOM lens = PCCOM_NOP;
    switch (CORSAIR_CHANNEL_id)
    {
    case CORSAIR_CHANNEL_ID_TVRGB:
    case CORSAIR_CHANNEL_ID_TVGRAY:
        vdb_write_request(corsair_vdb_registers::en_vbd_tv_objective, corsair_vdb_registers::en_vdb_tv_lens_autofocus_state, 1); break;
    case CORSAIR_CHANNEL_ID_SWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_swir_objective, corsair_vdb_registers::en_vdb_tv_lens_autofocus_state, 1); break;
    case CORSAIR_CHANNEL_ID_MWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_mwir_objective, corsair_vdb_registers::en_vdb_mw_lens_autofocus_state, 1); break;
    default: return;
    }
}
void Corsair_CmdManager::autofocus_dis(const uint32_t & CORSAIR_CHANNEL_id)
{
    //    enum PCCOM lens = PCCOM_NOP;
    switch (CORSAIR_CHANNEL_id)
    {
    case CORSAIR_CHANNEL_ID_TVRGB:
    case CORSAIR_CHANNEL_ID_TVGRAY:
        vdb_write_request(corsair_vdb_registers::en_vbd_tv_objective, corsair_vdb_registers::en_vdb_tv_lens_autofocus_state, 0); break;
    case CORSAIR_CHANNEL_ID_SWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_swir_objective, corsair_vdb_registers::en_vdb_tv_lens_autofocus_state, 1); break;
    case CORSAIR_CHANNEL_ID_MWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_mwir_objective, corsair_vdb_registers::en_vdb_mw_lens_autofocus_state, 0); break;
    default: return;
    }
}
void Corsair_CmdManager::swir_auto_gain()
{
    Corsair_Cmd cmd(SWIR_GAIN_MODE);
    cmd.append_data(uint8_t(0x02u));
    request(cmd);
}
void Corsair_CmdManager::swir_day_gain()
{
    Corsair_Cmd cmd(SWIR_GAIN_MODE);
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void Corsair_CmdManager::swir_night_gain()
{
    Corsair_Cmd cmd(SWIR_GAIN_MODE);
    cmd.append_data(uint8_t(0x01u));
    request(cmd);
}
void Corsair_CmdManager::zoom_in(const uint32_t & CORSAIR_CHANNEL_id, const uint8_t & speed)
{
    Corsair_Cmd cmd(MAN_ZOOM_IN);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(speed);
    request(cmd);
}
void Corsair_CmdManager::zoom_out(const uint32_t & CORSAIR_CHANNEL_id, const uint8_t & speed)
{
    Corsair_Cmd cmd(MAN_ZOOM_OUT);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(speed);
    request(cmd);
}
void Corsair_CmdManager::zoom_stop(const uint32_t & CORSAIR_CHANNEL_id)
{
    Corsair_Cmd cmd(MAN_ZOOM_STOP);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    request(cmd);
}
void Corsair_CmdManager::zoom_pos(const uint32_t & CORSAIR_CHANNEL_id, const uint32_t & pos)
{
    Corsair_Cmd cmd(MAN_ZOOM_POSITION);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(uint16_t(pos));
    request(cmd);

}
void Corsair_CmdManager::focus_near(const uint32_t & CORSAIR_CHANNEL_id, const uint8_t & speed)
{
    Corsair_Cmd cmd(MAN_FOCUS_NEAR);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(speed);
    request(cmd);
}
void Corsair_CmdManager::focus_far(const uint32_t & CORSAIR_CHANNEL_id, const uint8_t & speed)
{
    Corsair_Cmd cmd(MAN_FOCUS_FAR);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(speed);
    request(cmd);
}
void Corsair_CmdManager::focus_stop(const uint32_t & CORSAIR_CHANNEL_id)
{
    Corsair_Cmd cmd(MAN_FOCUS_STOP);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    request(cmd);
}
void Corsair_CmdManager::focus_pos(const uint32_t & CORSAIR_CHANNEL_id, const uint32_t & pos)
{
    Corsair_Cmd cmd(MAN_FOCUS_POSITION);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(uint16_t(pos));
    request(cmd);
}
void Corsair_CmdManager::iris_close(const uint32_t & CORSAIR_CHANNEL_id, const uint8_t & speed)
{
    Corsair_Cmd cmd(MAN_IRIS_CLOSE);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(speed);
    request(cmd);
}
void Corsair_CmdManager::iris_open(const uint32_t & CORSAIR_CHANNEL_id, const uint8_t & speed)
{
    Corsair_Cmd cmd(MAN_IRIS_OPEN);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(speed);
    request(cmd);
}
void Corsair_CmdManager::iris_stop(const uint32_t & CORSAIR_CHANNEL_id)
{
    Corsair_Cmd cmd(MAN_IRIS_STOP);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    request(cmd);
}
void Corsair_CmdManager::thermostat_on(const uint32_t & CORSAIR_CHANNEL_id)
{
    const bool state{true};
    switch (CORSAIR_CHANNEL_id) {
    case CORSAIR_CHANNEL_ID_TVRGB:
    case CORSAIR_CHANNEL_ID_TVGRAY:
        vdb_write_request(corsair_vdb_registers::en_vbd_tv_objective,   corsair_vdb_registers::en_vdb_tv_lens_auto_temp_control_state, state); break;
    case CORSAIR_CHANNEL_ID_SWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_swir_objective, corsair_vdb_registers::en_vdb_sw_lens_auto_temp_control_state, state); break;
    case CORSAIR_CHANNEL_ID_MWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_mwir_objective, corsair_vdb_registers::en_vdb_mw_lens_auto_temp_control_state, state); break;
    default: return;
    }
}
void Corsair_CmdManager::thermostat_off(const uint32_t & CORSAIR_CHANNEL_id)
{
    const bool state{false};
    switch (CORSAIR_CHANNEL_id) {
    case CORSAIR_CHANNEL_ID_TVRGB:
    case CORSAIR_CHANNEL_ID_TVGRAY:
        vdb_write_request(corsair_vdb_registers::en_vbd_tv_objective,   corsair_vdb_registers::en_vdb_tv_lens_auto_temp_control_state, state); break;
    case CORSAIR_CHANNEL_ID_SWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_swir_objective, corsair_vdb_registers::en_vdb_sw_lens_auto_temp_control_state, state); break;
    case CORSAIR_CHANNEL_ID_MWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_mwir_objective, corsair_vdb_registers::en_vdb_mw_lens_auto_temp_control_state, state); break;
    default: return;
    }
}
void Corsair_CmdManager::heat_on(const uint32_t & CORSAIR_CHANNEL_id)
{
    const bool state{true};
    switch (CORSAIR_CHANNEL_id) {
    case CORSAIR_CHANNEL_ID_TVRGB:
    case CORSAIR_CHANNEL_ID_TVGRAY:
        vdb_write_request(corsair_vdb_registers::en_vbd_tv_objective,   corsair_vdb_registers::en_vdb_tv_lens_heater_state, state); break;
    case CORSAIR_CHANNEL_ID_SWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_swir_objective, corsair_vdb_registers::en_vdb_sw_lens_heater_state, state); break;
    case CORSAIR_CHANNEL_ID_MWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_mwir_objective, corsair_vdb_registers::en_vdb_mw_lens_heater_state, state); break;
    default: return;
    }
}
void Corsair_CmdManager::heat_off(const uint32_t & CORSAIR_CHANNEL_id)
{
    const bool state{false};
    switch (CORSAIR_CHANNEL_id) {
    case CORSAIR_CHANNEL_ID_TVRGB:
    case CORSAIR_CHANNEL_ID_TVGRAY:
        vdb_write_request(corsair_vdb_registers::en_vbd_tv_objective,   corsair_vdb_registers::en_vdb_tv_lens_heater_state, state); break;
    case CORSAIR_CHANNEL_ID_SWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_swir_objective, corsair_vdb_registers::en_vdb_sw_lens_heater_state, state); break;
    case CORSAIR_CHANNEL_ID_MWIR:
        vdb_write_request(corsair_vdb_registers::en_vbd_mwir_objective, corsair_vdb_registers::en_vdb_mw_lens_heater_state, state); break;
    default: return;
    }
}
void Corsair_CmdManager::set_speed(const int32_t & _x_speed, const int32_t & _z_speed)
{
    auto x_speed = uint32_t(_x_speed);
    auto z_speed = uint32_t(_z_speed);
    Corsair_Cmd cmd(SET_SPEED, 0x1234);
    cmd.append_data(uint8_t((z_speed >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((z_speed >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((z_speed >> 16) & 0xFFu));
    //    cmd.append_data(int32_t(_z_speed));

    cmd.append_data(uint8_t((x_speed >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((x_speed >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((x_speed >> 16) & 0xFFu));
    //    cmd.append_data(int32_t(_x_speed));
    request(cmd);
}
void Corsair_CmdManager::set_absolute_angle(const int16_t & _abs_x_angle, const int16_t & _abs_z_angle)
{
    auto abs_x_angle = uint16_t(_abs_x_angle);
    auto abs_z_angle = uint16_t(_abs_z_angle);
    Corsair_Cmd cmd(SET_ABS_ANGLE, 0x1234);
    cmd.append_data(uint8_t((abs_z_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_z_angle >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_x_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_x_angle >> 8 ) & 0xFFu));
    request(cmd);
}
void Corsair_CmdManager::set_relative_angle(const int16_t & _rel_x_angle, const int16_t & _rel_z_angle)
{
    auto rel_x_angle = uint16_t(_rel_x_angle);
    auto rel_z_angle = uint16_t(_rel_z_angle);
    Corsair_Cmd cmd(SET_ANGLE, 0x1234);
    cmd.append_data(uint8_t((rel_z_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((rel_z_angle >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((rel_x_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((rel_x_angle >> 8 ) & 0xFFu));
    request(cmd);
}

void Corsair_CmdManager::gsp_click_control(const uint16_t& x, const uint16_t& y)
{
    Corsair_Cmd cmd(GSP_CLICK_CONTROL);
    cmd.append_data(x);
    cmd.append_data(y);
    request(cmd);
}
void Corsair_CmdManager::arretir_set()
{
    Corsair_Cmd cmd(ARR_SET, 0x1234);
    cmd.append_data(uint8_t(0x01u));
    request(cmd);
}
void Corsair_CmdManager::arretir_reset()
{
    Corsair_Cmd cmd(ARR_SET, 0x1234);
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void Corsair_CmdManager::stabilisation_on()
{
    Corsair_Cmd cmd(STAB_EN, 0x1234);
    cmd.append_data(uint8_t(0x01u));
    request(cmd);
}
void Corsair_CmdManager::stabilisation_off()
{
    Corsair_Cmd cmd(STAB_EN, 0x1234);
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void Corsair_CmdManager::park()
{
    Corsair_Cmd cmd(SET_POS);
    cmd.append_data(uint8_t(en_gdp_parking));
    request(cmd);
}
void Corsair_CmdManager::null_position()
{
    Corsair_Cmd cmd(SET_POS);
    cmd.append_data(uint8_t(en_gdp_zero));
    request(cmd);
}
void Corsair_CmdManager::pohod_position()
{
    Corsair_Cmd cmd(SET_POS);
    cmd.append_data(uint8_t(en_gdp_hike));
    request(cmd);
}
void Corsair_CmdManager::polyot_position()
{
    Corsair_Cmd cmd(SET_POS);
    cmd.append_data(uint8_t(en_gdp_piloting));
    request(cmd);
}
void Corsair_CmdManager::alignment()
{
    Corsair_Cmd cmd(ALIGNMENT);
    cmd.append_data(uint16_t(CONFIRM_CODE_ALIGNMENT));
    request(cmd);
}
void Corsair_CmdManager::set_platform_control_by_angle()
{
    Corsair_Cmd cmd(CONTROL_MODE, 0x1234);
    cmd.append_data(uint8_t(0x01u));
    request(cmd);
}
void Corsair_CmdManager::set_platform_control_by_velocity()
{
    Corsair_Cmd cmd(CONTROL_MODE, 0x1234);
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void Corsair_CmdManager::rangefinder_radiation(uint16_t code)
{
    Corsair_Cmd cmd(LRF_MAKE_RADIATION, 0x1234);
    cmd.append_data(code);
    request(cmd);
}
void Corsair_CmdManager::rangefinder_set_frequency(uint8_t frequency)
{
    Corsair_Cmd cmd(LRF_SET_FREQ, 0x1234);
    cmd.append_data(frequency);
    request(cmd);
}
void Corsair_CmdManager::rangefinder_power_on(bool enable)
{
    Corsair_Cmd cmd(LRF_POWER_ON, 0x1234);
    cmd.append_data(uint8_t(enable));
    request(cmd);
}

void Corsair_CmdManager::capture_target_single(const uint32_t &CORSAIR_CHANNEL_id, const int &x, const int &y, const int &w, const int &h)
{
    Corsair_Cmd cmd(TRACKING_EN_SINGLE);
    cmd.append_data(uint16_t(CORSAIR_CHANNEL_id));
    cmd.append_data(uint16_t(x));
    cmd.append_data(uint16_t(y));
    cmd.append_data(uint16_t(w));
    cmd.append_data(uint16_t(h));
    request(cmd);
}
void Corsair_CmdManager::free_target_single()
{
    Corsair_Cmd cmd(TRACKING_DIS_SINGLE);
    request(cmd);
}
//void Corsair_CmdManager::rangefinder_start_mono()
//{
//    Cmd cmd(LRF_MAKE_MEAS, 0x1234);
//    cmd.append_data(uint8_t(0x01u));
//    cmd.append_data(uint8_t(0x00u));
//    request(cmd);
//}
//void Corsair_CmdManager::rangefinder_start_5Hz()
//{
//    Cmd cmd(LRF_MAKE_MEAS, 0x1234);
//    cmd.append_data(uint8_t(0x01u));
//    cmd.append_data(uint8_t(0x05u));
//    request(cmd);
//}
//void Corsair_CmdManager::rangefinder_stop()
//{
//    Cmd cmd(LRF_MAKE_MEAS, 0x1234);
//    cmd.append_data(uint8_t(0x02u));
//    request(cmd);
//}
void Corsair_CmdManager::capture_target_multi(const uint32_t & CORSAIR_CHANNEL_id, const int & target_id, const bool & fast, const int & x, const int & y, const int & w, const int & h)
{
    Corsair_Cmd cmd(TRACKING_EN_MULTI);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(uint8_t(target_id));
    cmd.append_data(uint8_t(fast));
    cmd.append_data(uint16_t(x));
    cmd.append_data(uint16_t(y));
    cmd.append_data(uint16_t(w));
    cmd.append_data(uint16_t(h));
    request(cmd);
}
void Corsair_CmdManager::free_target_multi(const int & target_id)
{
    Corsair_Cmd cmd(TRACKING_DIS_MULTI);
    cmd.append_data(uint8_t(target_id));
    request(cmd);
}
void Corsair_CmdManager::choose_target_multi(const int & target_id)
{
    Corsair_Cmd cmd(TRACKING_CHS_MULTI);
    cmd.append_data(uint8_t(target_id));
    request(cmd);
}
void Corsair_CmdManager::tracker_deviation(const uint16_t& x, const uint16_t& y)
{
    Corsair_Cmd cmd(TRACKING_DEVIATION);
    cmd.append_data(x);
    cmd.append_data(y);
    request(cmd);
}
void Corsair_CmdManager::self_test(const bool & en)
{
    Corsair_Cmd cmd(AUTO_INSPECTION);
    cmd.append_data(uint16_t(CONFIRM_CODE_AUTO_INSPECTION));
    cmd.append_data(uint8_t(en));
    request(cmd);
}
void Corsair_CmdManager::ping()
{
    Corsair_Cmd cmd(PING);
    request(cmd);
}

void Corsair_CmdManager::brightness(const uint32_t& CORSAIR_CHANNEL_id, const int& brightness)
{
    int8_t val = (brightness > 120) ? 120 : ((brightness < -120) ? -120 : brightness);

    Corsair_Cmd cmd(CONTR_PROCESSING_BRIGHTNESS);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(int8_t(val));
    request(cmd);
}

void Corsair_CmdManager::contrast(const uint32_t& CORSAIR_CHANNEL_id, const double& contrast)
{
    uint32_t val = 0;
    if(contrast > 0) {
        val = static_cast<uint32_t>(contrast / 2. * 0xFFFF);
    }
    val = (val > 0xFFFF) ? 0xFFFF : val;
    Corsair_Cmd cmd(CONTR_PROCESSING_CONTRAST);
    cmd.append_data(uint8_t(CORSAIR_CHANNEL_id));
    cmd.append_data(uint16_t(val));
    request(cmd);
}

void Corsair_CmdManager::mw_inversion(const bool enable)
{
    //     vdb_write_request(corsair_vdb_registers::en_vbd_fixed_board, corsair_vdb_registers::en_vdb_vc_mw_inversion, enable);
    Corsair_Cmd cmd(CONTR_INVERSION_MW);
    cmd.append_data(uint8_t(enable));
    request(cmd);
}
