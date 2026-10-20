#include "cmdmanager.h"
#include "eth/ethcontroller.h"

#include "tools/logger.h"

#include "device_parts/channel/abstractchannel.h"
#include "vdb_registers.h"

#include <QDateTime>

Q_LOGGING_CATEGORY(logCmdManager, "CmdManager")

#define BUFFER_LEN (512)
static uint8_t buffer[BUFFER_LEN];

CmdManager::CmdManager(QObject * parent) :
    QObject(parent)
{
    ethController = &EthController::inst();
}
bool CmdManager::rcv_cmd(Packet & res)
{
    int _cmd_res_len;
    uint8_t * _cmd_res = ethController->rcv_cmd(_cmd_res_len);
    if(_cmd_res == nullptr) return false;
    return res.from_buffer(_cmd_res, _cmd_res_len);
}
void CmdManager::request(Packet & req)
{
    static uint32_t cnt = 0;
//    QString s;
//    s.sprintf("request[ %u ]: { ID[ 0x%x ]DATA_LEN[ %u ]DATA[ ", cnt, req.get_id(), req.get_data_len());
//    uint32_t data_len;
//    uint8_t * data = req.get_data(data_len);
//    for(uint32_t i = 0; i < data_len; ++i)
//    {
//        s += QString("0x") + QString::number(data[i], 16) + QString(", ");
//    }
//    s += QString("] }");
//    qInfo(logCmdManager(), s.toStdString().c_str());
    uint32_t buffer_len = req.to_buffer(buffer);
    ethController->send_cmd(buffer, buffer_len);
    ++cnt;
}
void CmdManager::vdb_read_request(const uint8_t dst_board, const uint8_t dst_reg, const uint16_t length)
{
    const uint8_t cmd = 0x00;
    const uint8_t status = 0x80;
    const uint16_t dst_address = (dst_board << 8) | dst_reg;
    Packet packet(cmd, status, dst_address, length);
    request(packet);

}
void CmdManager::vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word)
{
    const uint8_t cmd = 0x00;
    const uint8_t status = 0x00;
    const uint16_t dst_address = (dst_board << 8) | dst_reg;
    Packet packet(cmd, status, dst_address);
    packet.append_data(word);
    request(packet);
}
void CmdManager::vdb_write_request_array(const uint8_t dst_board, const uint8_t dst_reg, const std::vector<uint8_t>& data)
{
    const uint8_t cmd = 0x00;
    const uint8_t status = 0x00;
    const uint16_t dst_address = (dst_board << 8) | dst_reg;
    Packet packet(cmd, status, dst_address);
    packet.append_data(data);
    request(packet);
}
void CmdManager::select_channel(const uint32_t & id)
{
    Cmd cmd(CH_ENABLE, 0);
    cmd.append_data(uint8_t(id));
    cmd.append_data(uint8_t(1));
    request(cmd);
}
void CmdManager::agc_switch(const uint32_t & channel_id, const bool & onoff)
{
    Cmd cmd(CONTR_ENHANCEMENT);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(uint8_t(onoff));
    request(cmd);
}
// FIXME
void CmdManager::field_sync_narrow(const uint32_t & channel_id)
{
    Cmd cmd(FIELD_SYNC_NARROW);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void CmdManager::field_sync(const uint32_t & channel_id)
{
    Cmd cmd(FIELD_SYNC);
    cmd.append_data(uint8_t(channel_id));
    request(cmd);
}
void CmdManager::field_sync_reg(const uint32_t & channel_id, const bool & onoff)
{
    Cmd cmd(FIELD_SYNC_REG);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(uint8_t(onoff));
    request(cmd);
}
void CmdManager::autofocus(const uint32_t & channel_id)
{
    Cmd cmd(AUTOFOCUS);
    cmd.append_data(uint8_t(channel_id));
    request(cmd);
}
void CmdManager::autofocus_en(const uint32_t & channel_id)
{
//    enum PCCOM lens = PCCOM_NOP;
    switch (channel_id)
    {
    case CHANNEL_ID_TVRGB:
    case CHANNEL_ID_TVGRAY:
        vdb_write_request(vdb_registers::en_vbd_tv_objective, vdb_registers::en_vdb_tv_lens_autofocus_state, 1); break;
    case CHANNEL_ID_SWIR:
        vdb_write_request(vdb_registers::en_vbd_swir_objective, vdb_registers::en_vdb_tv_lens_autofocus_state, 1); break;
    case CHANNEL_ID_MWIR:
        vdb_write_request(vdb_registers::en_vbd_mwir_objective, vdb_registers::en_vdb_mw_lens_autofocus_state, 1); break;
    default: return;
    }
}
void CmdManager::autofocus_dis(const uint32_t & channel_id)
{
//    enum PCCOM lens = PCCOM_NOP;
    switch (channel_id)
    {
    case CHANNEL_ID_TVRGB:
    case CHANNEL_ID_TVGRAY:
        vdb_write_request(vdb_registers::en_vbd_tv_objective, vdb_registers::en_vdb_tv_lens_autofocus_state, 0); break;
    case CHANNEL_ID_SWIR:
        vdb_write_request(vdb_registers::en_vbd_swir_objective, vdb_registers::en_vdb_tv_lens_autofocus_state, 1); break;
    case CHANNEL_ID_MWIR:
        vdb_write_request(vdb_registers::en_vbd_mwir_objective, vdb_registers::en_vdb_mw_lens_autofocus_state, 0); break;
    default: return;
    }
}
void CmdManager::swir_auto_gain()
{
    Cmd cmd(SWIR_GAIN_MODE);
    cmd.append_data(uint8_t(0x02u));
    request(cmd);
}
void CmdManager::swir_day_gain()
{
    Cmd cmd(SWIR_GAIN_MODE);
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void CmdManager::swir_night_gain()
{
    Cmd cmd(SWIR_GAIN_MODE);
    cmd.append_data(uint8_t(0x01u));
    request(cmd);
}
void CmdManager::zoom_in(const uint32_t & channel_id, const uint8_t & speed)
{
    emit notifyCmd(MAN_ZOOM_IN, {{"speed", speed}});

    Cmd cmd(MAN_ZOOM_IN);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(speed);
    request(cmd);
}
void CmdManager::zoom_out(const uint32_t & channel_id, const uint8_t & speed)
{
    emit notifyCmd(MAN_ZOOM_OUT, {{"speed", speed}});
    Cmd cmd(MAN_ZOOM_OUT);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(speed);
    request(cmd);
}
void CmdManager::zoom_stop(const uint32_t & channel_id)
{
    emit notifyCmd(MAN_ZOOM_STOP, {});
    Cmd cmd(MAN_ZOOM_STOP);
    cmd.append_data(uint8_t(channel_id));
    request(cmd);
}
void CmdManager::zoom_pos(const uint32_t & channel_id, const uint32_t & pos)
{
    Cmd cmd(MAN_ZOOM_POSITION);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(uint16_t(pos));
    request(cmd);
}
void CmdManager::focus_near(const uint32_t & channel_id, const uint8_t & speed)
{
    emit notifyCmd(MAN_FOCUS_NEAR, {{"speed", speed}});
    Cmd cmd(MAN_FOCUS_NEAR);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(speed);
    request(cmd);
}
void CmdManager::focus_far(const uint32_t & channel_id, const uint8_t & speed)
{
    emit notifyCmd(MAN_FOCUS_FAR, {{"speed", speed}});
    Cmd cmd(MAN_FOCUS_FAR);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(speed);
    request(cmd);
}
void CmdManager::focus_stop(const uint32_t & channel_id)
{
    emit notifyCmd(MAN_FOCUS_STOP, {});
    Cmd cmd(MAN_FOCUS_STOP);
    cmd.append_data(uint8_t(channel_id));
    request(cmd);
}
void CmdManager::focus_pos(const uint32_t & channel_id, const uint32_t & pos)
{
    Cmd cmd(MAN_FOCUS_POSITION);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(uint16_t(pos));
    request(cmd);
}
void CmdManager::iris_close(const uint32_t & channel_id, const uint8_t & speed)
{
    Cmd cmd(MAN_IRIS_CLOSE);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(speed);
    request(cmd);
}
void CmdManager::iris_open(const uint32_t & channel_id, const uint8_t & speed)
{
    Cmd cmd(MAN_IRIS_OPEN);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(speed);
    request(cmd);
}
void CmdManager::iris_stop(const uint32_t & channel_id)
{
    Cmd cmd(MAN_IRIS_STOP);
    cmd.append_data(uint8_t(channel_id));
    request(cmd);
}
void CmdManager::thermostat_on(const uint32_t & channel_id)
{
    const bool state{true};
    switch (channel_id) {
    case CHANNEL_ID_TVRGB:
    case CHANNEL_ID_TVGRAY:
        vdb_write_request(vdb_registers::en_vbd_tv_objective,   vdb_registers::en_vdb_tv_lens_auto_temp_control_state, state); break;
    case CHANNEL_ID_SWIR:
        vdb_write_request(vdb_registers::en_vbd_swir_objective, vdb_registers::en_vdb_sw_lens_auto_temp_control_state, state); break;
    case CHANNEL_ID_MWIR:
        vdb_write_request(vdb_registers::en_vbd_mwir_objective, vdb_registers::en_vdb_mw_lens_auto_temp_control_state, state); break;
    default: return;
    }
}
void CmdManager::thermostat_off(const uint32_t & channel_id)
{
    const bool state{false};
    switch (channel_id) {
    case CHANNEL_ID_TVRGB:
    case CHANNEL_ID_TVGRAY:
        vdb_write_request(vdb_registers::en_vbd_tv_objective,   vdb_registers::en_vdb_tv_lens_auto_temp_control_state, state); break;
    case CHANNEL_ID_SWIR:
        vdb_write_request(vdb_registers::en_vbd_swir_objective, vdb_registers::en_vdb_sw_lens_auto_temp_control_state, state); break;
    case CHANNEL_ID_MWIR:
        vdb_write_request(vdb_registers::en_vbd_mwir_objective, vdb_registers::en_vdb_mw_lens_auto_temp_control_state, state); break;
    default: return;
    }
}
void CmdManager::heat_on(const uint32_t & channel_id)
{
    const bool state{true};
    switch (channel_id) {
    case CHANNEL_ID_TVRGB:
    case CHANNEL_ID_TVGRAY:
        vdb_write_request(vdb_registers::en_vbd_tv_objective,   vdb_registers::en_vdb_tv_lens_heater_state, state); break;
    case CHANNEL_ID_SWIR:
        vdb_write_request(vdb_registers::en_vbd_swir_objective, vdb_registers::en_vdb_sw_lens_heater_state, state); break;
    case CHANNEL_ID_MWIR:
        vdb_write_request(vdb_registers::en_vbd_mwir_objective, vdb_registers::en_vdb_mw_lens_heater_state, state); break;
    default: return;
    }
}
void CmdManager::heat_off(const uint32_t & channel_id)
{
    const bool state{false};
    switch (channel_id) {
    case CHANNEL_ID_TVRGB:
    case CHANNEL_ID_TVGRAY:
        vdb_write_request(vdb_registers::en_vbd_tv_objective,   vdb_registers::en_vdb_tv_lens_heater_state, state); break;
    case CHANNEL_ID_SWIR:
        vdb_write_request(vdb_registers::en_vbd_swir_objective, vdb_registers::en_vdb_sw_lens_heater_state, state); break;
    case CHANNEL_ID_MWIR:
        vdb_write_request(vdb_registers::en_vbd_mwir_objective, vdb_registers::en_vdb_mw_lens_heater_state, state); break;
    default: return;
    }
}
void CmdManager::set_speed(const int32_t & _x_speed, const int32_t & _z_speed)
{
    static int32_t prev_x_speed = std::numeric_limits<int32_t>::max();
    static int32_t prev_z_speed = std::numeric_limits<int32_t>::max();

    // anti DDOS ))
    auto dx = _x_speed - prev_x_speed;
    auto dz = _z_speed - prev_z_speed;
    if (dx == 0 && dz == 0)
        return;

    prev_x_speed = _x_speed;
    prev_z_speed = _z_speed;

    emit notifyCmd(SET_SPEED, {{"x", _x_speed}, {"z", _z_speed}});

    auto x_speed = uint32_t(_x_speed);
    auto z_speed = uint32_t(_z_speed);

    Cmd cmd(SET_SPEED, 0x1234);
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

void CmdManager::set_speed_deg_per_sec(const float &x_speed, const float &z_speed)
{
    Cmd cmd(SET_SPEED_F, 0x1234);
    cmd.append_data(z_speed);
    cmd.append_data(x_speed);
    request(cmd);
}
void CmdManager::set_absolute_angle(const int16_t & _abs_x_angle, const int16_t & _abs_z_angle)
{
    auto abs_x_angle = uint16_t(_abs_x_angle);
    auto abs_z_angle = uint16_t(_abs_z_angle);
    Cmd cmd(SET_ABS_ANGLE, 0x1234);
    cmd.append_data(uint8_t((abs_z_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_z_angle >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_x_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((abs_x_angle >> 8 ) & 0xFFu));
    request(cmd);
}
void CmdManager::set_absolute_angle(const float & abs_x_angle, const float & abs_z_angle)
{
    Cmd cmd(SET_ABS_ANGLE_F, 0x1234);
    cmd.append_data(abs_z_angle);
    cmd.append_data(abs_x_angle);
    request(cmd);
}
void CmdManager::set_relative_angle(const int16_t & _rel_x_angle, const int16_t & _rel_z_angle)
{
    auto rel_x_angle = uint16_t(_rel_x_angle);
    auto rel_z_angle = uint16_t(_rel_z_angle);
    Cmd cmd(SET_ANGLE, 0x1234);
    cmd.append_data(uint8_t((rel_z_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((rel_z_angle >> 8 ) & 0xFFu));
    cmd.append_data(uint8_t((rel_x_angle >> 0 ) & 0xFFu));
    cmd.append_data(uint8_t((rel_x_angle >> 8 ) & 0xFFu));
    request(cmd);
}

void CmdManager::gsp_click_control(const uint16_t& x, const uint16_t& y)
{
    Cmd cmd(GSP_CLICK_CONTROL);
    cmd.append_data(x);
    cmd.append_data(y);
    request(cmd);
}

void CmdManager::gyro_calibration_auto()
{
    Cmd cmd(GYRO_CALIBRATION_AUTO);
    request(cmd);
}

void CmdManager::gyro_calibration(float yaw, float pitch)
{
    Cmd cmd(GYRO_CALIBRATION);
    cmd.append_data(yaw);
    cmd.append_data(pitch);
    request(cmd);
}

void CmdManager::arretir_set()
{
    Cmd cmd(ARR_SET, 0x1234);
    cmd.append_data(uint8_t(0x01u));
    request(cmd);
}
void CmdManager::arretir_reset()
{
    Cmd cmd(ARR_SET, 0x1234);
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void CmdManager::stabilisation_on()
{
    Cmd cmd(STAB_EN, 0x1234);
    cmd.append_data(uint8_t(0x01u));
    request(cmd);
}
void CmdManager::stabilisation_off()
{
    Cmd cmd(STAB_EN, 0x1234);
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void CmdManager::park()
{
    Cmd cmd(SET_POS);
    cmd.append_data(uint8_t(en_gdp_parking));
    request(cmd);
}
void CmdManager::null_position()
{
    Cmd cmd(SET_POS);
    cmd.append_data(uint8_t(en_gdp_zero));
    request(cmd);
}
void CmdManager::pohod_position()
{
    Cmd cmd(SET_POS);
    cmd.append_data(uint8_t(en_gdp_hike));
    request(cmd);
}
void CmdManager::polyot_position()
{
    Cmd cmd(SET_POS);
    cmd.append_data(uint8_t(en_gdp_piloting));
    request(cmd);
}
void CmdManager::alignment()
{
    Cmd cmd(ALIGNMENT);
    cmd.append_data(uint16_t(CONFIRM_CODE_ALIGNMENT));
    request(cmd);
}
void CmdManager::set_platform_control_by_angle()
{
    Cmd cmd(CONTROL_MODE, 0x1234);
    cmd.append_data(uint8_t(0x01u));
    request(cmd);
}
void CmdManager::set_platform_control_by_velocity()
{
    Cmd cmd(CONTROL_MODE, 0x1234);
    cmd.append_data(uint8_t(0x00u));
    request(cmd);
}
void CmdManager::rangefinder_radiation(uint16_t code)
{
    Cmd cmd(LRF_MAKE_RADIATION, 0x1234);
    cmd.append_data(code);
    request(cmd);
}
void CmdManager::rangefinder_set_frequency(uint8_t frequency)
{
    Cmd cmd(LRF_SET_FREQ, 0x1234);
    cmd.append_data(frequency);
    request(cmd);
}
void CmdManager::rangefinder_power_on(bool enable)
{
    Cmd cmd(LRF_POWER_ON, 0x1234);
    cmd.append_data(uint8_t(enable));
    request(cmd);
}

void CmdManager::capture_target_single(const uint32_t &channel_id, const int &x, const int &y, const int &w, const int &h)
{
    Cmd cmd(TRACKING_EN_SINGLE);
    cmd.append_data(uint16_t(channel_id));
    cmd.append_data(uint16_t(x));
    cmd.append_data(uint16_t(y));
    cmd.append_data(uint16_t(w));
    cmd.append_data(uint16_t(h));
    request(cmd);
}
void CmdManager::free_target_single()
{
    Cmd cmd(TRACKING_DIS_SINGLE);
    request(cmd);
}
//void CmdManager::rangefinder_start_mono()
//{
//    Cmd cmd(LRF_MAKE_MEAS, 0x1234);
//    cmd.append_data(uint8_t(0x01u));
//    cmd.append_data(uint8_t(0x00u));
//    request(cmd);
//}
//void CmdManager::rangefinder_start_5Hz()
//{
//    Cmd cmd(LRF_MAKE_MEAS, 0x1234);
//    cmd.append_data(uint8_t(0x01u));
//    cmd.append_data(uint8_t(0x05u));
//    request(cmd);
//}
//void CmdManager::rangefinder_stop()
//{
//    Cmd cmd(LRF_MAKE_MEAS, 0x1234);
//    cmd.append_data(uint8_t(0x02u));
//    request(cmd);
//}
void CmdManager::capture_target_multi(const uint32_t & channel_id, const int & target_id, const bool & fast, const int & x, const int & y, const int & w, const int & h)
{
    Cmd cmd(TRACKING_EN_MULTI);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(uint8_t(target_id));
    cmd.append_data(uint8_t(fast));
    cmd.append_data(uint16_t(x));
    cmd.append_data(uint16_t(y));
    cmd.append_data(uint16_t(w));
    cmd.append_data(uint16_t(h));
    request(cmd);
    // qDebug() << " ============== ch id" << channel_id << "tgt id" << target_id << "fast:" << fast << "obj" << x << y << "/" << w << h;
}
void CmdManager::free_target_multi(const int & target_id)
{
    Cmd cmd(TRACKING_DIS_MULTI);
    cmd.append_data(uint8_t(target_id));
    request(cmd);
}
void CmdManager::choose_target_multi(const int & target_id)
{
    Cmd cmd(TRACKING_CHS_MULTI);
    cmd.append_data(uint8_t(target_id));
    request(cmd);
}
void CmdManager::tracker_deviation(const uint16_t& x, const uint16_t& y)
{
    Cmd cmd(TRACKING_DEVIATION);
    cmd.append_data(x);
    cmd.append_data(y);
    request(cmd);
}
void CmdManager::self_test(const bool & en)
{
    Cmd cmd(AUTO_INSPECTION);
    cmd.append_data(uint16_t(CONFIRM_CODE_AUTO_INSPECTION));
    cmd.append_data(uint8_t(en));
    request(cmd);
}
void CmdManager::ping()
{
    Cmd cmd(PING);
    request(cmd);
}

void CmdManager::brightness(const uint32_t& channel_id, const int& brightness)
{
    int8_t val = (brightness > 120) ? 120 : ((brightness < -120) ? -120 : brightness);

    Cmd cmd(CONTR_PROCESSING_BRIGHTNESS);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(int8_t(val));
    request(cmd);
}

void CmdManager::contrast(const uint32_t& channel_id, const double& contrast)
{
    uint32_t val = 0;
    if(contrast > 0) {
        val = static_cast<uint32_t>(contrast / 2. * 0xFFFF);
    }
    val = (val > 0xFFFF) ? 0xFFFF : val;
    Cmd cmd(CONTR_PROCESSING_CONTRAST);
    cmd.append_data(uint8_t(channel_id));
    cmd.append_data(uint16_t(val));
    request(cmd);
}

void CmdManager::mw_inversion(const bool enable)
{
//     vdb_write_request(vdb_registers::en_vbd_fixed_board, vdb_registers::en_vdb_vc_mw_inversion, enable);
Cmd cmd(CONTR_INVERSION_MW);
cmd.append_data(uint8_t(enable));
request(cmd);
}
