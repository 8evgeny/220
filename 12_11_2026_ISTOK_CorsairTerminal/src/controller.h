#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <atomic>
#include <thread>
#include "device_parts/cmd/cmd.h"
#include "device_parts/cmd/telemetry.h"
#include "device_parts/channel/abstractchannel.h"


#include "dev_types.h"

class AbstractChannel;
class ETLD;
class CmdManager;
class GSPController;
class EthController;
class CorsairPainter;

class Controller : public QObject
{
    Q_OBJECT
public:
    typedef enum
    {
        en_lrf_rc_stop_radiation = 0x0000, // Остановить излучение
        en_lrf_rc_single_measure = 0xAA55, // Единичное измерение
        en_lrf_rc_illumination   = 0xB5AD, // Режим "подсвет"
        en_lrf_rc_single_shot    = 0x9EED, // Единичный импульс основным излучателем без измерения
        en_lrf_rc_test_shot      = 0xB99D, // Тестовый излучатель
    } t_lrf_radiation_ctrl;
    struct t_rangefinder {
        struct {
            uint16_t rangefinder_mode;
            bool rangefinder_power_on;
            bool rangefinder_rdy;
            bool rangefinder_pause;
            bool rangefinder_cont_emit;
        } flags;
        double rangefinder_range_mono;
    };

    struct t_gsp {
        struct t_flags {
            bool mech_arretir;
            bool el_arretir;
            bool off;
            bool stabilization;
        } flags;
        double x_angle;
        double z_angle;
    };
#pragma pack(push, 1)
    struct TrackerObject {
        bool valid;
        bool trusted;
        int x;
        int y;
        int w;
        int h;
    };
#pragma pack(pop)

public:
    explicit Controller(QObject *parent = nullptr, DEV_TYPES dev_type = DEV_TYPES::en_dev_type_350);
    ~Controller();
    QImage *image(QSize &frm_size, QTransform &disp_to_frm_tr);
    inline uint32_t channelId() {return _channel ? _channel->id() : uint32_t(-1);}
    AbstractChannel *channel();
    const AbstractChannel *channel() const;
    AbstractChannel *channel(uint32_t id);
    const AbstractChannel *channel(uint32_t id) const;
    GSPController* get_gsp_controller() {return _gspController;}
    void openMirror(const QString src, const QString dst);
    double gsp_get_z_delta_shift();
    int8_t gsp_get_z_inv();

    CmdManager *cmdManger();

signals:
    void response(Cmd);
    void response(Telemetry);
    void autofocus_data();
    void response(Packet);
    void set_autofocus_host_pause(bool);
    void image_ready();
    void host_etld_state_changed(bool);

    void channelChanged(uint32_t chan_id);
    void sg_trackerEn(const bool);

    void sg_paint();

public slots:
    void start();
    void stop();

    void vdb_read_request(const uint8_t dst_board, const uint8_t dst_reg, const uint16_t length);
    void vdb_write_request(const uint8_t dst_board, const uint8_t dst_reg, const uint32_t word);
    void vdb_write_request_array(const uint8_t dst_board, const uint8_t dst_reg, const std::vector<uint8_t>& data);
    void send_msg(Packet& req);

    void new_object(bool is_single, const int & x, const int & y, const int & w, const int & h);
    void stop_object(bool is_single);
    void video_click(const int & x0, const int & y0);

    void host_etld_en_set(const bool & v);
    void host_x_inv_set(const bool & v);
    void host_y_inv_set(const bool & v);

    void select_channel(const uint32_t & channel_id);
    void agc_switch(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void agc_on(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void agc_off(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void field_sync_narrow(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void field_sync(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void field_sync_reg_switch(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void field_sync_reg_on(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void field_sync_reg_off(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void autofocus(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void autofocus_en(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void autofocus_dis(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void swir_gain_switch();
    void swir_gain_auto();
    void swir_gain_day();
    void swir_gain_night();

    void zoom_in(const double & speed = 1.0, const uint32_t & channel_id = CHANNEL_ID_NONE);
    void zoom_out(const double & speed = 1.0, const uint32_t & channel_id = CHANNEL_ID_NONE);
    void zoom_stop(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void zoom_pos(const uint32_t & pos, const uint32_t & channel_id = CHANNEL_ID_NONE);
    void zoom_pos_fix();
    void zoom_pos_fix_width();
    void focus_near(const double & speed = 1.0, const uint32_t & channel_id = CHANNEL_ID_NONE);
    void focus_far(const double & speed = 1.0, const uint32_t & channel_id = CHANNEL_ID_NONE);
    void focus_stop(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void focus_pos(const uint32_t & pos, const uint32_t & channel_id = CHANNEL_ID_NONE);
    void iris_close(const double & speed = 1.0, const uint32_t & channel_id = CHANNEL_ID_NONE);
    void iris_open(const double & speed = 1.0, const uint32_t & channel_id = CHANNEL_ID_NONE);
    void iris_stop(const uint32_t & channel_id = CHANNEL_ID_NONE);

    void thermostat_on(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void thermostat_off(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void heat_on(const uint32_t & channel_id = CHANNEL_ID_NONE);
    void heat_off(const uint32_t & channel_id = CHANNEL_ID_NONE);

    void gsp_speed_set(const double x_speed_in, const double z_speed_in);
    void gsp_speed_tracker_set(const double x_speed, const double z_speed);
    void gsp_speed_set_deg(const double &, const double &);
    void gsp_slow_speed_set(const double &, const double &);
    void gsp_absolute_angle_set(const double &, const double &);
    void gsp_relative_angle_set(const double &, const double &);
    void gsp_arretir_set();
    void gsp_arretir_reset();
    void gsp_el_arr_coarse();
    void gsp_el_arr_fine();
    void gsp_stabilization_switch();
    void gsp_stabilization_on();
    void gsp_stabilization_off();
    void gsp_park();
    void gsp_null_position();
    void gsp_pohod_position();
    void gsp_polyot_position();
    void gsp_alignment();
    void gsp_angle_ctrl_mode();
    void gsp_velocity_ctrl_mode();
    void gsp_drift(const int32_t & x_drift, const int32_t & z_drift);
    void gsp_set_z_delta_shift(const double& delta);

    void lrf_enable_switch();
    void lrf_set_enabled(bool state);
    void lrf_mode_switch();
    void lrf_start();
    void lrf_start_mono();
    void lrf_start_5Hz();
    void lrf_stop();
    void lrf_toggle_continuos();
    void lrf_set_frequency(uint8_t frequency);
    void lrf_dangerZone_en(const bool en, const t_lrf_danger_zone_params params);


    void target_enable_switch_single();
    void target_set_single_enabled(bool en);
    void target_capture_single();
    void target_free_single();
    void target_enable_switch_multi();
    void target_set_multi_enabled(bool en);
    void target_capture_multi();
    void target_free_multi();
    void target_switch_multi();
    void target_window_set(const double &);
    void target_window_set(const int &);
    void target_id_switch();
    void target_id_set(const int &);
    void target_choose();
    void target_fast_switch();
    void target_fast_set(const bool &);

    void digital_zoom_in();
    void digital_zoom_out();
    void digital_zoom_switch();
    void pic_in_pic();
    void inversion_mw();
    void brightness_up();
    void brightness_down();
    void contrast_up();
    void contrast_down();
    void bc_reset();

    void tvgraymwir_enable_changed(bool);
    void tvgraymwir_method_changed(int);
    void tvgraymwir_offset_x_changed(int);
    void tvgraymwir_offset_y_changed(int);
    void tvgraymwir_method_agc_en_changed(bool);
    void tvgraymwir_method_c_changed(double);

    void self_test_on();
    void self_test_off();

    void video_record_switch();
    void video_record_on();
    void video_record_off();

    void image_save_switch();
    void image_save_on();
    void image_save_off();

    void click_control(bool flag) {_settings.flags.click_control = flag;}
    void tracker_deviation(bool flag) {_settings.flags.tracker_deviation = flag;}

    void infographics_on_screen(bool  flag);
    void infographics_on_record(bool  flag);
    void infographics_on_picture(bool flag);
    void infographics_angleInversion_X(bool flag);
    void infographics_angleInversion_Z(bool flag);
    void sl_blockGSPControl(const bool & block) { _GSPControlEnable = !block; };
    void sl_setZAngleRange180(bool val);
    void sl_mwirDumpEnable(const bool en);

    void sl_infographic_setShipPointer();
    void sl_infographic_setHelicopterPointer();

private slots:
    void sl_mwirWaitingStatusOff();
    void sl_updateFrame();

private:
    void run();
    void rcv_frame();
    void rcv_cmd();
    void telemetry_handler(Telemetry& cmd);
    // void image_draw_save_record();

    void host_etld_procedure();

    bool lrfCalcDangerZone();
    bool lrfCalcOneZone(const double z,
                        const double legBegin, const double legEnd,
                        const double boardShift);

private:
    const DEV_TYPES _dev_type;
    // Channel
    std::unordered_map<uint32_t, AbstractChannel *> _channels;
    AbstractChannel *_channel;
    // CmdManager
    CmdManager * _cmdManager;
    // GSPController
    GSPController * _gspController;
    // EthController
    EthController * _ethController;
    // ETLD
    bool _host_etld_en;
    ETLD * _etld;
    std::mutex _host_etld_mutex;
    // Target
    bool _target_en;
    int _target_size;
    int _target_id;
    bool _target_fast;
    // Object
    uint8_t _tracker_state;
    uint8_t _tracker_channel;
    TrackerObject _objects[4];
    // Rangefinder
    t_rangefinder _rangefinder;
    bool _lrf_IsDangerZone{true};
    bool _lrf_DangerZoneOn{true};
    t_lrf_danger_zone_params _lrf_dangerZoneParams;
    //GSP
    t_gsp _gsp;
    // Image
    cv::Size _currentFrameSize;
    QImage *m_rdy_img{nullptr};
    QTransform m_disp_to_frm_tr;
    QSize m_frm_size;
    std::mutex _img_mutex;
    bool _img_ready;
    // Thread
    std::atomic<bool> _execute;
    std::thread _thd;
    // Settings
    struct t_settings {
        struct t_flags {
            bool click_control;
            bool tracker_deviation;
            bool is_tracker_enable;
        } flags;
    } _settings;


    bool _GSPControlEnable{true};

    int32_t _mwirSensorStatus{1};
    // bool m_mwirDumpEn{false};

    CorsairPainter *_painter{nullptr};
    // QThread *m_painterThread{nullptr};
};

#endif // CONTROLLER_H
