#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QObject>
#include <QTimer>

#include <SDL2/SDL.h>

class Joystick : public QObject
{
    Q_OBJECT
public:
    explicit Joystick(QObject * parent = nullptr);
    ~Joystick();

    inline bool is_opened() {return _opened;}

    static SDL_Joystick* currentJoystick();

    void setPollingIntervalMs(int ms);

    QString joystickName() const;

signals:
    void agc_switch();
    void swir_gain_switch();

    void zoom_in();
    void zoom_out();
    void zoom_stop();
    void focus_near();
    void focus_far();
    void focus_stop();
    void iris_close();
    void iris_open();
    void iris_stop();
    void field_sync_narrow();
    void field_sync();
    void field_sync_reg_switch();
    void autofocus();

    void gsp_speed_set(const double &, const double &);
    void gsp_slow_speed_set(const double &, const double &);
    void gsp_absolute_angle_set(const double &, const double &);
    void gsp_relative_angle_set(const double &, const double &);
    void gsp_arretir_set();
    void gsp_arretir_reset();
    void gsp_stabilization_switch();
    void gsp_park();
    void gsp_null_position();

    void lrf_enable_switch();
    void lrf_mode_switch();
    void lrf_start();
    void lrf_toggle_continuous();
    void lrf_single();

    void target_enable_switch();
    void target_set_enabled(bool en);
    void target_capture();
    void target_free();
    void target_window_set(const double &);
    void target_id_switch();
    void target_choose();
    void target_fast_switch();

    void digital_zoom_in();
    void digital_zoom_out();
    void pic_in_pic();
    void inversion();
    void brightness_up();
    void brightness_down();
    void contrast_up();
    void contrast_down();

    void next_channel();
    void prev_channel();
    void change_channel(const uint32_t &);
    void switch_channel();


    void full_screen();

    void help();
    void telemetry();
    void settings();
    void gspCtrl();
    void rangeFinderCtrl();

    void zoom_pos_fix();
    void zoom_pos_fix_width();

    void videoRecordSwitch();

    void gspFixAngleSwitch();
private slots:
    void joystick_tmr_timedout();

public slots:
    void set_enable(bool flag);
    void set_inverse_x(bool flag);
    void set_inverse_z(bool flag);

protected:
    void check_hikvision_events();
private:
    void load();
    void save();

    void open(const int & device_index);
    void close();
    void check_events();
    void check_xbox360controller_events();
    void check_ps_events();
    void check_vpc_events();

    void check_thrustmaster_events();
    void check_thrustmaster_hats();

    void check_machenike();
    void check_machenike_axis();
    void check_machenike_hats();
    void check_machenike_buttons();

    void check_machenike_axisTwo();

    void check_logitech_events();
    void check_logitech_axis();
    void check_logitech_hats();
    void check_logitech_buttons();

    int exp_filter(const int & prev, const int & next, const double & a);
    double exp_filter(const double & prev, const double & next, const double & a);
private:
    SDL_Joystick * joystick;
    QString joystick_name;
    QTimer *joystick_tmr;
    int joystick_poll_interval_ms= 50;
    bool _enable;
    int prev_buttons;
    int prev_hats;
    double x_coord;
    double z_coord;
    double y_coord;

    double zoom_coord;
    double focus_coord;

    double sizeUp;
    double sizeDown;

    double x_adjust_coord;
    double z_adjust_coord;

    int    MIN_TRACKBALL_COORD          = -30704;
    int    MAX_TRACKBALL_COORD          = 30703;
    int    MIN_TRACKBALL_COORD_MACHENIKE = -32768;
    int    MAX_TRACKBALL_COORD_MACHENIKE =  32767;
    int    MIN_SENCE_TRACKBALL_COORD    = 50;
    int    MIN_SENCE_TRACKBALL_COORD_PS = 2000;
    int    MIN_SENCE_TRACKBALL_COORD_TM = 50;

    int    MIN_TRACKBALL_COORD_PC          = -32768;
    // int    MIN_TRACKBALL_COORD_PC          = -32768;
    int    MAX_TRACKBALL_COORD_PC          = 32768;
    int    HALF_TRACKBALL_COORD_PC          = 32768;

    double TRACKBALL_COORD_EXP_FILTER  = 0.9;

    bool _opened;

    double _x_inverted_koef;
    double _z_inverted_koef;
};

#endif // JOYSTICK_H
