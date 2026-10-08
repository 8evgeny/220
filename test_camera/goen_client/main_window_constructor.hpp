#ifndef MAIN_WINDOW_CONSTRUCTOR_HPP
#define MAIN_WINDOW_CONSTRUCTOR_HPP


#include "cv_main_window.hpp"
#include "cv_widget.hpp"
#include "cv_button/cv_button.hpp"
#include "cv_label/cv_label.hpp"
#include "cv_line_edit/cv_line_edit.hpp"
#include "cv_frame_show/cv_frame_show.hpp"
#include "cv_slider/cv_slider.hpp"
#include "cv_combo_box/cv_combo_box.hpp"
#include "RS232_transieverLS.hpp"
#include "get_send.hpp"
#include "commands.hpp"
#include "modules/cv_widgets/cv_panel.hpp"

#ifdef USE_LOG_CMD
#include "tools/log_cmd/log_cmd.hpp"
#endif // USE_LOG_CMD

//#include <jsoncpp/json/json.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <functional>
#include "thread"
#include "chrono"

using namespace std;
using namespace cv;
using namespace cvw;

struct MVals
{
    double val = 0;
    double sum = 0;
    list<double> l_val;
};

class MainWindowConstructor
{
public:
    MainWindowConstructor(const std::string & config, bool & ok);
    ~MainWindowConstructor();
    void start();

    // LibSerial
    std::shared_ptr<RS232TransieverLS> rs232_ptr;
#ifdef USE_UDP
    std::shared_ptr<get_send_data> eth_udp_ptr;
#endif

    void exec_keep_rs232(); // получение в цикле телеметрии с шара
    void exec_receive_eth(); // Получение телеметрии с шара по ethernet

private:
    std::string config_path = "../config.ini";
    cv::Mat frame_mainWindow; ;
    int mw_w = 2736;
    int mw_h = 1500;
    std::string winname = "win";
    CVMainWindow mw;

    int dist = 32;
    cv::Point pt_markup = {dist,dist};
    cv::Scalar btn_clr_active = {190,250,190};
    cv::Scalar btn_clr_active_mouse_in = {220,250,220};
    cv::Scalar btn_clr_base = {220,220,220};
    cv::Scalar btn_clr_base_mouse_in = {250,250,250};

    friend void frame_show_device_slot(MainWindowConstructor *, int, int);
    friend void line_edit_url_callback(MainWindowConstructor * mwc, std::string & str);
    ///dbg::
    friend void cmbbx_edit_url_slot(MainWindowConstructor *, int, int);
    friend void cmbbx_edit_com_slot(MainWindowConstructor *, int, int);

    friend void btn_start_device_callback(MainWindowConstructor * mwc);
    friend void lbl_fps_timed(MainWindowConstructor * mwc);
    friend void exec_check_status_device(MainWindowConstructor * mwc);
    friend void btn_stop_device_callback(MainWindowConstructor * mwc);
    friend void btn_open_com_callback(MainWindowConstructor *);
    friend void btn_close_com_callback(MainWindowConstructor *);
    friend void btn_to_zero_position_callback(MainWindowConstructor *);
    friend void btn_stop_tracking_callback(MainWindowConstructor *);
    friend void btn_up_callback(MainWindowConstructor *);
    friend void btn_left_callback(MainWindowConstructor *);
    friend void btn_stop_callback(MainWindowConstructor *);
    friend void btn_right_callback(MainWindowConstructor *);
    friend void btn_down_callback(MainWindowConstructor *);
    friend void btn_up_callback(MainWindowConstructor *);

    friend void key_control_callback(MainWindowConstructor *mw, unsigned char key);
    friend void cmd_stop_after_pause(MainWindowConstructor *);
    friend void cmd_stop_after_pause_detach(MainWindowConstructor *);

    friend void btn_TPV_callback(MainWindowConstructor *);
    friend void btn_TV_callback(MainWindowConstructor *);
    friend void btn_set_angle_callback(MainWindowConstructor *);

    friend void btn_trac_size_up_callback(MainWindowConstructor *);
    friend void btn_trac_size_down_callback(MainWindowConstructor *);

    friend void slider_set_angle_callback(MainWindowConstructor *);
    friend void slider_ptz_speed_callback(MainWindowConstructor *);
    friend void btn_azimuth_up_callback(MainWindowConstructor *);
    friend void btn_azimuth_down_callback(MainWindowConstructor *);
    friend void btn_pitch_up_callback(MainWindowConstructor *);
    friend void btn_pitch_down_callback(MainWindowConstructor *);
    friend void btn_zoom_up_callback(MainWindowConstructor *);
    friend void btn_zoom_down_callback(MainWindowConstructor *);

    friend void btn_abs_stab_callback(MainWindowConstructor *);
    friend void btn_ang_stab_callback(MainWindowConstructor *);
    friend void btn_rotaty_platform_callback(MainWindowConstructor *);
    friend void btn_motor_off_callback(MainWindowConstructor *);
    friend void btn_exit_programm_callback(MainWindowConstructor *);


//    friend void btn_image_enhancement_callback(MainWindowConstructor *);
//    friend void btn_infrared_inversion_callback(MainWindowConstructor *);
//    friend void btn_off_servo_callback(MainWindowConstructor *);
//    friend void btn_on_servo_callback(MainWindowConstructor *);
    friend void btn_update_null_callback(MainWindowConstructor *);
    friend void btn_specify_attitude_angle_callback(MainWindowConstructor *);
    friend void  btn_calib_zero_pos_fc_att_callback(MainWindowConstructor *);
//    friend void btn_close_follow_callback(MainWindowConstructor *);
//    friend void btn_azimuth_follow_callback(MainWindowConstructor *);
//    friend void btn_electric_lock_mode_callback(MainWindowConstructor *);
//    friend void btn_electric_unlock_callback(MainWindowConstructor *);

    friend void btn_attack_callback(MainWindowConstructor *);
    friend void btn_stop_attack_callback(MainWindowConstructor *);
    friend void btn_nuc_handle_control_callback(MainWindowConstructor *);
    friend void btn_TPV_img_enchance_class_callback(MainWindowConstructor *);
    friend void btn_TPV_img_enchance_class_up_callback(MainWindowConstructor *);
    friend void btn_TPV_img_enchance_class_down_callback(MainWindowConstructor *);

    friend void btn_suppress_gyro_drift_callback(MainWindowConstructor *);
    friend void slider_drift_callback(MainWindowConstructor *);
    friend void btn_suppress_gyro_drift_azimuth_up_callback(MainWindowConstructor *);
    friend void btn_suppress_gyro_drift_azimuth_down_callback(MainWindowConstructor *);
    friend void btn_suppress_gyro_drift_pitch_up_callback(MainWindowConstructor *);
    friend void btn_suppress_gyro_drift_pitch_down_callback(MainWindowConstructor *);

    friend void btn_calib_i2c_zero_callback(MainWindowConstructor *);
    friend void btn_calib_handle_i2c_zero_callback(MainWindowConstructor *);
    friend void slider_i2c_callback(MainWindowConstructor *);
    friend void btn_calib_i2c_zero_roll_up_callback(MainWindowConstructor *);
    friend void btn_calib_i2c_zero_roll_down_callback(MainWindowConstructor *);
    friend void btn_calib_i2c_zero_pitch_up_callback(MainWindowConstructor *);
    friend void btn_calib_i2c_zero_pitch_down_callback(MainWindowConstructor *);
    friend void btn_calib_i2c_zero_azimuth_up_callback(MainWindowConstructor *);
    friend void btn_calib_i2c_zero_azimuth_down_callback(MainWindowConstructor *);

//    friend void btn_motor_angle_off_callback(MainWindowConstructor *);

    friend void btn_tm_xy_mode_off_callback(MainWindowConstructor *);
    friend void btn_tm_xy_mode_on_callback(MainWindowConstructor *);
    friend void btn_abs_angle_mode_off_callback(MainWindowConstructor *);
    friend void btn_abs_angle_mode_i2c_callback(MainWindowConstructor *);
    friend void btn_abs_angle_mode_complex_callback(MainWindowConstructor *);
    friend void btn_angular_velocity_mode_off_callback(MainWindowConstructor *);
    friend void btn_angular_velocity_mode_on_callback(MainWindowConstructor *);

    friend void btn_poweroff_goen_callback(MainWindowConstructor *);


    friend void default_slot(MainWindowConstructor *);
    shared_ptr<CVFrameShow> frame_show_device = nullptr;
    shared_ptr<CVWidget> lbl_video_data = nullptr;
    shared_ptr<CVLabel> lbl_fps = nullptr;
    shared_ptr<CVLabel> lbl_frame_data = nullptr;

    shared_ptr<CVWidget> lbl_record_on = nullptr;
    cv::Mat img_record_on;
    cv::Mat img_record_off;
    cv::Rect rct_record;

    shared_ptr<CVWidget> lbl_edit_url = nullptr;
    shared_ptr<CVLineEdit> line_edit_url = nullptr;
    shared_ptr<CVComboBox> cmbbx_edit_com = nullptr;
    shared_ptr<CVWidget> lbl_edit_com = nullptr;

    shared_ptr<CVButton> btn_start_device = nullptr;
    shared_ptr<CVButton> btn_stop_device = nullptr;
    shared_ptr<CVButton> btn_open_com = nullptr;
    shared_ptr<CVButton> btn_close_com = nullptr;

    shared_ptr<CVButton> btn_set_null = nullptr;
    shared_ptr<CVButton> btn_up = nullptr;
    shared_ptr<CVButton> btn_stop_tracking = nullptr;
    shared_ptr<CVButton> btn_left = nullptr;
    shared_ptr<CVButton> btn_stop = nullptr;
    shared_ptr<CVButton> btn_right = nullptr;
    shared_ptr<CVButton> btn_TV_TPV = nullptr;
    shared_ptr<CVButton> btn_down = nullptr;
    shared_ptr<CVButton> btn_TV = nullptr;
    shared_ptr<CVWidget> lbl_set_azimuth = nullptr;
    shared_ptr<CVLabel> lbl_set_azimuth_now = nullptr;
    shared_ptr<CVSlider> slider_edit_azimuth = nullptr;
    shared_ptr<CVWidget> lbl_set_pitch = nullptr;
    shared_ptr<CVLabel> lbl_set_pitch_now = nullptr;
    shared_ptr<CVSlider> slider_edit_pitch = nullptr;
    shared_ptr<CVButton> btn_set_angle = nullptr;

    shared_ptr<CVButton> btn_pitch_up = nullptr;
    shared_ptr<CVButton> btn_pitch_down = nullptr;
    shared_ptr<CVButton> btn_azimuth_up = nullptr;
    shared_ptr<CVButton> btn_azimuth_down = nullptr;

    shared_ptr<CVWidget> lbl_trac_size = nullptr;
    shared_ptr<CVLabel> lbl_trac_size_value = nullptr;
    shared_ptr<CVButton> btn_trac_size_up = nullptr;
    shared_ptr<CVButton> btn_trac_size_down = nullptr;

    shared_ptr<CVWidget> lbl_ptz_speed = nullptr;
    shared_ptr<CVSlider> slider_ptz_speed = nullptr;
    shared_ptr<CVLabel> lbl_ptz_speed_value = nullptr;
    shared_ptr<CVWidget> lbl_zoom = nullptr;
    shared_ptr<CVButton> btn_zoom_up = nullptr;
    shared_ptr<CVButton> btn_zoom_down = nullptr;
    shared_ptr<CVLabel> lbl_zoom_ratio = nullptr;

    shared_ptr<CVWidget> lbl_target_miss = nullptr;
    shared_ptr<CVWidget> lbl_tmx = nullptr;
    shared_ptr<CVWidget> lbl_tmy = nullptr;
    shared_ptr<CVLabel> lbl_show_tmx = nullptr;
    shared_ptr<CVLabel> lbl_show_tmy = nullptr;
    shared_ptr<CVWidget> lbl_deg0 = nullptr;
    shared_ptr<CVWidget> lbl_deg1 = nullptr;

    // Вывод телеметрии углов
    shared_ptr<CVWidget> wid_motor_angles = nullptr;
//    shared_ptr<CVWidget> lbl_frame_angle = nullptr;
//    shared_ptr<CVWidget> lbl_roll0 = nullptr;
    shared_ptr<CVWidget> lbl_pitch0 = nullptr;
    shared_ptr<CVWidget> lbl_azimuth0 = nullptr;
//    shared_ptr<CVLabel> lbl_roll_show = nullptr;
    shared_ptr<CVLabel> lbl_pitch_show = nullptr;
    shared_ptr<CVLabel> lbl_azimuth_show = nullptr;
//    shared_ptr<CVWidget> lbl_deg2 = nullptr;
    shared_ptr<CVWidget> lbl_deg3 = nullptr;
    shared_ptr<CVWidget> lbl_deg4 = nullptr;

//    shared_ptr<CVWidget> wid_las_ranging = nullptr;
//    shared_ptr<CVLabel> lbl_las_ranging = nullptr;
//    shared_ptr<CVWidget> wid_01m = nullptr;

    // Вывод телеметрии скоростей
    shared_ptr<CVWidget> lbl_angular_velocity = nullptr;
    shared_ptr<CVWidget> lbl_av_roll0 = nullptr;
    shared_ptr<CVWidget> lbl_av_pitch0 = nullptr;
    shared_ptr<CVWidget> lbl_av_azimuth0 = nullptr;
    shared_ptr<CVLabel> lbl_av_roll_show = nullptr;
    shared_ptr<CVLabel> lbl_av_pitch_show = nullptr;
    shared_ptr<CVLabel> lbl_av_azimuth_show = nullptr;
    shared_ptr<CVWidget> lbl_deg5 = nullptr;
    shared_ptr<CVWidget> lbl_deg6 = nullptr;
    shared_ptr<CVWidget> lbl_deg7 = nullptr;

    // Вывод неименованных полей // res dbg::
    shared_ptr<CVWidget> wid_abs_angles = nullptr;
    shared_ptr<CVWidget> wid_16_17_abs_roll = nullptr;
    shared_ptr<CVLabel> lbl_16_17_abs_roll = nullptr;
    shared_ptr<CVWidget> lbl_deg_8 = nullptr;
    shared_ptr<CVWidget> wid_18_19_abs_pitch = nullptr;
    shared_ptr<CVLabel> lbl_18_19_abs_pitch = nullptr;
    shared_ptr<CVWidget> lbl_deg_9 = nullptr;
    shared_ptr<CVWidget> wid_29_30_abs_azimuth = nullptr;
    shared_ptr<CVLabel> lbl_29_30_abs_azimuth = nullptr;
    shared_ptr<CVWidget> lbl_deg_10 = nullptr;

    shared_ptr<CVWidget> wid_zoom_bot_panel = nullptr;
    shared_ptr<CVLabel> lbl_zoom_bot_panel = nullptr;

    /// Panel other cmd
    shared_ptr<CVButton> btn_abs_stab = nullptr;
    shared_ptr<CVButton> btn_ang_stab = nullptr;
    shared_ptr<CVButton> btn_rotaty_platform = nullptr;
    shared_ptr<CVButton> btn_motor_off = nullptr;
    shared_ptr<CVButton> btn_exit_programm = nullptr;

//    shared_ptr<CVButton> btn_image_enhancement = nullptr;
//    shared_ptr<CVButton> btn_infrared_inversion = nullptr;
    shared_ptr<CVButton> btn_update_null = nullptr;
//    shared_ptr<CVButton> btn_on_off_servo = nullptr;
//    shared_ptr<CVButton> btn_close_follow = nullptr;
//    shared_ptr<CVButton> btn_azimuth_follow = nullptr;
//    shared_ptr<CVButton> btn_electric_lock_mode = nullptr;

//    shared_ptr<CVWidget> lbl_motor_switch = nullptr;
//    shared_ptr<CVButton> btn_motor_on = nullptr;
//    shared_ptr<CVButton> btn_motor_off = nullptr;
//    shared_ptr<CVWidget> lbl_azimuth_follow_switch = nullptr;
//    shared_ptr<CVButton> btn_follow_start = nullptr;
//    shared_ptr<CVButton> btn_follow_close = nullptr;
//    shared_ptr<CVWidget> lbl_lock_mode_switch = nullptr;
//    shared_ptr<CVButton> btn_lock_mode_on = nullptr;
//    shared_ptr<CVButton> btn_lock_mode_off = nullptr;

    /// Panel status information feedback
    // status information feedback 1
    shared_ptr<CVWidget> wid_tracked_video_source = nullptr;
    shared_ptr<CVWidget> wid_tracking_algorithm_type = nullptr;
    shared_ptr<CVWidget> wid_target_automatic_prompt = nullptr;
    shared_ptr<CVWidget> wid_target_tracking_status = nullptr;
    shared_ptr<CVLabel> lbl_tracked_video_source = nullptr;
    shared_ptr<CVLabel> lbl_tracking_algorithm_type = nullptr;
    shared_ptr<CVLabel> lbl_target_automatic_prompt = nullptr;
    shared_ptr<CVLabel> lbl_target_tracking_status = nullptr;
    // status information feedback 2
    shared_ptr<CVWidget> wid_image_enchancement = nullptr;
    shared_ptr<CVWidget> wid_storage = nullptr;
    shared_ptr<CVWidget> wid_motor_status = nullptr;
    shared_ptr<CVWidget> wid_follow_mode = nullptr;
    shared_ptr<CVWidget> wid_electric_lock_mode = nullptr;
    shared_ptr<CVWidget> wid_laser_status = nullptr;
    shared_ptr<CVLabel> lbl_image_enchancement = nullptr;
    shared_ptr<CVLabel> lbl_storage = nullptr;
    shared_ptr<CVLabel> lbl_motor_status = nullptr;
    shared_ptr<CVLabel> lbl_follow_mode = nullptr;
    shared_ptr<CVLabel> lbl_electric_lock_mode = nullptr;
    shared_ptr<CVLabel> lbl_laser_status = nullptr;
    // status information feedback 3
    shared_ptr<CVWidget> wid_large_screen_displayed = nullptr;
    shared_ptr<CVWidget> wid_small_screen_displayed = nullptr;
    shared_ptr<CVLabel> lbl_large_screen_displayed = nullptr;
    shared_ptr<CVLabel> lbl_small_screen_displayed = nullptr;
    // self inspection result
    shared_ptr<CVWidget> wid_imaging_plate = nullptr;
    shared_ptr<CVWidget> wid_encoder_and_servo_drive = nullptr;
    shared_ptr<CVWidget> wid_gyroscope_calibration = nullptr;
    shared_ptr<CVWidget> wid_self_inspection_complete = nullptr;
    shared_ptr<CVLabel> lbl_imaging_plate = nullptr;
    shared_ptr<CVLabel> lbl_encoder_and_servo_drive = nullptr;
    shared_ptr<CVLabel> lbl_gyroscope_calibration = nullptr;
    shared_ptr<CVLabel> lbl_self_inspection_completed = nullptr;

    shared_ptr<CVButton> btn_attack = nullptr;
    shared_ptr<CVButton> btn_stop_attack = nullptr;
    shared_ptr<CVButton> btn_nuc_handle_control = nullptr;
    shared_ptr<CVButton> btn_specify_attitude_angle = nullptr;
    shared_ptr<CVButton> btn_calib_zero_pos_fc_att = nullptr;

    // Кнопки подачи команды установки компенсации дрейфа. В случае auto X=Y=0x7FFF
    shared_ptr<CVButton> btn_suppress_gyro_drift = nullptr;

    // Блок регулировки дрейфа по X/Y [-2000,2000]
    shared_ptr<CVWidget> wid_azimuth_drift_val = nullptr;
    shared_ptr<CVLabel> lbl_azimuth_drift_val = nullptr;
    shared_ptr<CVSlider> slider_azimuth_drift = nullptr;
    shared_ptr<CVButton> btn_suppress_gyro_drift_azimuth_up = nullptr;
    shared_ptr<CVButton> btn_suppress_gyro_drift_azimuth_down = nullptr;
    shared_ptr<CVWidget> wid_pitch_drift_val = nullptr;
    shared_ptr<CVLabel> lbl_pitch_drift_val = nullptr;
    shared_ptr<CVSlider> slider_pitch_drift = nullptr;
    shared_ptr<CVButton> btn_suppress_gyro_drift_pitch_up = nullptr;
    shared_ptr<CVButton> btn_suppress_gyro_drift_pitch_down = nullptr;

    // Блок регулировки крена тангажа MPU6050 по roll pitch
    shared_ptr<CVButton> btn_calib_handle_i2c_zero= nullptr;
    shared_ptr<CVButton> btn_calib_i2c_zero= nullptr;

    shared_ptr<CVWidget> wid_roll_i2c_val = nullptr;
    shared_ptr<CVLabel> lbl_roll_i2c_val = nullptr;
    shared_ptr<CVSlider> slider_roll_i2c = nullptr;
    shared_ptr<CVButton> btn_calib_i2c_zero_roll_up = nullptr;
    shared_ptr<CVButton> btn_calib_i2c_zero_roll_down = nullptr;

    shared_ptr<CVWidget> wid_pitch_i2c_val = nullptr;
    shared_ptr<CVLabel> lbl_pitch_i2c_val = nullptr;
    shared_ptr<CVSlider> slider_pitch_i2c = nullptr;
    shared_ptr<CVButton> btn_calib_i2c_zero_pitch_up = nullptr;
    shared_ptr<CVButton> btn_calib_i2c_zero_pitch_down = nullptr;

    shared_ptr<CVWidget> wid_azimuth_i2c_val = nullptr;
    shared_ptr<CVLabel> lbl_azimuth_i2c_val = nullptr;
    shared_ptr<CVSlider> slider_azimuth_i2c = nullptr;
    shared_ptr<CVButton> btn_calib_i2c_zero_azimuth_up = nullptr;
    shared_ptr<CVButton> btn_calib_i2c_zero_azimuth_down = nullptr;

    // Переключение режимов передачи телеметрии
    shared_ptr<CVWidget>  wid_tm_xy_mode = nullptr;
    shared_ptr<CVButton> btn_tm_xy_mode_off = nullptr;
    shared_ptr<CVButton> btn_tm_xy_mode_on = nullptr;

    shared_ptr<CVWidget> wid_abs_angle_mode_change = nullptr;
    shared_ptr<CVButton> btn_abs_angle_mode_off = nullptr;
    shared_ptr<CVButton> btn_abs_angle_mode_complex = nullptr;

    shared_ptr<CVWidget> wid_angular_velocity_mode_change = nullptr;
    shared_ptr<CVButton> btn_angular_velocity_mode_off = nullptr;
    shared_ptr<CVButton> btn_angular_velocity_mode_on = nullptr;

    shared_ptr<CVButton> btn_motor_angle_off = nullptr;
    int tm_xy_mode = 0; // включить передачу углового отклонения рамки трека от оптической оси
    int abs_angle_mode = 1; // 0 - выкл,  2 - обработанные углы
    int angular_velocity_mode = 1; // включить передачу угловых скоростей

    shared_ptr<CVButton> btn_poweroff_goen = nullptr;
    shared_ptr<CVComboBox> cmbbx_edit_url = nullptr;

    // Блок управления передачей данных


    int16_t x_drift = 0, y_drift = 0;
    int16_t roll_i2c = 0, pitch_i2c = 0, azimuth_i2c;

    // Other cmd
    bool f_attack = 0;
    bool f_attack_prev = 0;
    bool f_image_enchance = 0;
    bool f_infrared_inversion = 0;
    int channel_tpv = -1;

    ToGoenCommand to_goen_cmd_str;
    FromGoenTelemetry from_goen_telemetry_str; // Телеметрия по таблице 2
    FromGoenTelemetry from_goen_telemetry_medium; // Телеметрия по таблице 2
    StatusInformationFeedback1 sif1_str; // Битовые поля from_goen_telemetry_str
    StatusInformationFeedback2 sif2_str;
    StatusInformationFeedback3 sif3_str;
    SelfInspectionResult sir_str;
    uint16_t zoom_ratio;
    BoolUintTransformer transform_struct2bit;
    SingleStatusReturn ssr_str;

    float k_rel2field_x_tv_1 = 1920.f / (6.f * 20.f);
    float k_rel2field_y_tv_1 = 1080.f / (3.375 * 20.f);
    float k_rel2field_x_tpv_1 = 1350.f / (12.f * 20.f);
    float k_rel2field_y_tpv_1 = 1080.f / (9.6 * 20.f);
    float zoom_now = 1.f;
    float zoom_prev = 1.f;
    // Кнопки режимов
    bool f_servo_status = 0;
    bool f_azimuth_follow_status = 0;
    bool f_lock_servo = 0;

    std::atomic<bool> f_new_speed_command = {false};
    std::atomic<bool> f_need_stop_command = {false};
    std::atomic<bool> f_exec_command_stop_control = {false};
    int ptz_speed = 50;
    int zoom_factor = 100;
    int com_port_num = 0;
    int zahvat_size = 36;
    int min_angle_pitch = -110;
    int max_angle_pitch = 110;
    int min_angle_azimuth = -180;
    int max_angle_azimuth = 180;

    int max_list_size = 1 ;
    MVals m_tm_azimuth, m_tm_pitch,
    m_motor_roll, m_motor_pitch, m_motor_azimuth,
    m_abs_roll, m_abs_pitch, m_abs_azimutch,
    m_av_roll, m_av_pitch, m_av_azimuth;
    int16_t calib_i2c_zero_parameter_auto = -30000;

    void set_medium_val_for_tlm();
    double calc_m_value(const double & new_val, double & sum, list<double> & l_val, int max_l_size); // calc_m_value

    friend void key_confirm_handler(MainWindowConstructor *, unsigned char );
    friend void wid_draw_ruler_callback(MainWindowConstructor *);
    shared_ptr<CVWidget> wid_draw_ruler = nullptr;
    cv::Rect rct_ruler;
    cv::Mat draw_ruler_mat;

    std::vector <cv::Point2f> points_ruler_up;
    std::vector <cv::Point2f> points_ruler_down;
    std::vector <cv::Point2f> points_plain;
    std::vector <cv::Point2f> points_ruler_roll;

    int circleRadius = 0;
    int circleCenterX = 0;
    int circleCenterY = 0;
    float spacing = 0;

    int thickness_ruler = 2;
    int thickness_text = 2;
    int thickness_ruler_roll = 2;
    int thickness_circle = 2;
    float fontScale_text = 0.5;
    cv::Scalar red =  cv::Scalar(0, 0, 255);
    cv::Scalar green = cv::Scalar(0, 180, 0);
    cv::Scalar black  = cv::Scalar(0, 0, 0);
    cv::Scalar white  = cv::Scalar(255, 255, 255);
    cv::Scalar yellow = cv::Scalar(0, 190, 255);void fill_vector(int rows, int cols);

    cv::Scalar clr_text = white;
    cv::Scalar clr_line = white;
    cv::Scalar clr_crosshair = green;

    float k_deg2rad = CV_PI / 180.0;
    void drawRuler(cv::Mat& img, float roll, float pitch);
    float draw_roll = 0, draw_pitch = 0;
    //.hpp

    void draw_receive_telemetry();
    bool set_cmd(uint8_t control_param, uint16_t parameter_x, uint16_t parameter_y, uint8_t zoom_rate, uint8_t parameter3 = 0);

    friend void btn_confirm_yes_callback(MainWindowConstructor * mwc);
    friend void btn_confirm_no_callback(MainWindowConstructor * mwc);
    bool f_confirm = false;

    std::string confirm_str = "confirm";
    CVMainWindow window_confirm;
    cv::Mat frame_confirm;
    shared_ptr<CVLabel> lbl_confirm = nullptr;
    shared_ptr<CVButton> btn_confirm_yes = nullptr;
    shared_ptr<CVButton> btn_confirm_no = nullptr;

#ifdef USE_LOG_CMD
    std::shared_ptr<LogCmd> log_cmd_ptr = nullptr;
    std::vector<std::string> v_tlm_str;
    uint8_t buf_tlm[sizeof(from_goen_telemetry_str)];
    std::string tlm_header_str = "TIME_ms,FULL_TLM_PACK,TM_Y_deg,TM_P_deg,MOTOR_Y_deg,MOTOR_P_deg,ABS_Y_deg,ABS_P_deg,ABS_R_deg,ANG_VEL_Y_deg_s,ANG_VEL_P_deg_s,ANG_VEL_R_deg_s,ZOOM";
#endif // USE_LOG_CMD

    std::string open_close = "";
    bool get_ini_params(const std::string &config); // -- END get_ini_params

}; // -- END class MainWindowConstructor

#endif //  MAIN_WINDOW_CONSTRUCTOR_HPP
