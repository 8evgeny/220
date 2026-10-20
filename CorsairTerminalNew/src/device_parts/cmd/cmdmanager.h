#ifndef CMDMANAGER_H
#define CMDMANAGER_H

#include <QObject>
#include <QVariantMap>
#include "device_parts/cmd/cmd.h"
#include "device_parts/cmd/telemetry.h"

#include "cmd.h"

class EthController;

class CmdManager : public QObject
{
    Q_OBJECT
public:
    CmdManager(QObject * parent = nullptr);
    bool rcv_cmd(Packet &res);

private slots:

signals:

    void notifyCmd(COMNEW cmd, QVariantMap data);

public slots:
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
    void set_speed_deg_per_sec(const float & x_speed,   const float & z_speed);
    void set_absolute_angle(const int16_t & abs_x_angle, const int16_t & abs_z_angle);
    void set_absolute_angle(const float & abs_x_angle,   const float & abs_z_angle);
    void set_relative_angle(const int16_t & rel_x_angle, const int16_t & rel_z_angle);
    void gsp_click_control(const uint16_t & x, const uint16_t & y);
    void gyro_calibration_auto();
    void gyro_calibration(float yaw, float pitch);
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

private:
    EthController * ethController;
};

#endif // CMDMANAGER_H
