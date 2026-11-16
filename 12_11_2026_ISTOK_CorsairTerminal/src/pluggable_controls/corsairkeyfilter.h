#ifndef CORSAIRKEYFILTER_H
#define CORSAIRKEYFILTER_H

#include <QObject>

class CorsairKeyFilter : public QObject
{
    Q_OBJECT
public:
    explicit CorsairKeyFilter(QObject * parent = nullptr);

signals:
    void tvrgb_on();
    void tvgray_on();
    void swir_on();
    void mwir_on();
    void fusion_on();
    void tvgraymwir_on();
    void dsp_on();
    void test_on();

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

    void target_enable_switch();
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

    void full_screen();

    void help();
    void telemetry();
    void settings();
    void gspCtrl();
    void rangeFinderCtrl();
    void connectionCtrl();

    void image_save();
    void video_record_switch();

    void write_dump(const bool&);

    void turnOnImitateJoystick();

public slots:
    void set_inverse_x(bool flag);
    void set_inverse_z(bool flag);

protected:
    bool eventFilter(QObject * watched, QEvent * event) override;
private:
    bool _write_dump;

    double _x_inverted_koef;
    double _z_inverted_koef;
};

#endif // CORSAIRKEYFILTER_H
