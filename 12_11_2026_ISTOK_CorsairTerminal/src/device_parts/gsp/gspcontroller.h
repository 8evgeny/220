#ifndef GSPCONTROLLER_H
#define GSPCONTROLLER_H

#include <QObject>

class GSPPIDController;
class GSPController : public QObject
{
    Q_OBJECT
public:
    explicit GSPController(QObject * parent = nullptr);
    ~GSPController();

    void set_drift(const int32_t & x_drift, const int32_t & z_drift);
//    void set_drift_x(const int32_t & x_drift);
//    void set_drift_z(const int32_t & z_drift);

    double get_x_Kp();
    double get_x_Ki();
    double get_x_Kd();
    double get_x_integral_limit();

    double get_z_Kp();
    double get_z_Ki();
    double get_z_Kd();
    double get_z_integral_limit();

    void set_z_shift_angle(double angle);
    double get_z_shift_angle();

private:
    void load();
    void save();

signals:

public slots:

    void set_x_Kp(const double & v);
    void set_x_Ki(const double & v);
    void set_x_Kd(const double & v);
    void set_x_integral_limit(const double & v);

    void set_z_Kp(const double & v);
    void set_z_Ki(const double & v);
    void set_z_Kd(const double & v);
    void set_z_integral_limit(const double & v);

    void start_host_etld_object_error();
    void set_host_etld_object_error(const double & x_angle_error, const double & z_angle_error, int32_t & x_speed, int32_t & z_speed);
    void set_abs_speed(const double & _x_speed, const double & _z_speed, int32_t & x_speed, int32_t & z_speed);

    void set_speed(const double & norm_x_speed, const double & norm_z_speed, int32_t & x_speed, int32_t & z_speed);
    void set_slow_speed(const double & norm_x_speed, const double & norm_z_speed, int32_t & x_speed, int32_t & z_speed);
    void set_absolute_angle(const double & _abs_x_angle, const double & _abs_z_angle, int16_t & abs_x_angle, int16_t & abs_z_angle);
    void set_relative_angle(const double & _rel_x_angle, const double & _rel_z_angle, int16_t & rel_x_angle, int16_t & rel_z_angle);

private:
    GSPPIDController * x_pid_controller;
    GSPPIDController * z_pid_controller;

    int32_t x_drift = 0;
    int32_t z_drift = 0;

    double z_shift_angle = 0.;

    // int X_SPEED_MAX = 60000;// * 10;
    // int Z_SPEED_MAX = 60000;// * 10;
    int X_SPEED_MAX = 45000;// * 10;
    int Z_SPEED_MAX = 45000;// * 10;
    int X_SPEED_SLOW_MAX = 10000;
    int Z_SPEED_SLOW_MAX = 10000;
    int X_ANGLE_MIN = -179;
    int X_ANGLE_MAX = 179;
    int Z_ANGLE_MIN = -179;
    int Z_ANGLE_MAX = 179;
};

#endif // GSPCONTROLLER_H
