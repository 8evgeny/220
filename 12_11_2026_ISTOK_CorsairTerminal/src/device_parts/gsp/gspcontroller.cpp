#include "gspcontroller.h"
#include <cmath>
#include <QVariant>
#include "device_parts/gsp/gsppidcontroller.h"

#include "tools/resource.h"

#include "tools/logger.h"
Q_LOGGING_CATEGORY(logGSPController, "GSPController")

#define RESOURCE_SECTION    ("gspcontroller")
#define RESOURCE_FILENAME   ("rsc/gspcontroller.xml")

#define MIN_SPEED (5)

GSPController::GSPController(QObject * parent) :
    QObject(parent)
{
    load();
    x_pid_controller = new GSPPIDController(600, 24, 3750, 10000);
    z_pid_controller = new GSPPIDController(600, 24, 3750, 10000);
}
GSPController::~GSPController()
{
    save();
    delete x_pid_controller;
    delete z_pid_controller;
}
void GSPController::set_drift(const int32_t & _x_drift, const int32_t & _z_drift)
{
    x_drift = _x_drift;
    z_drift = _z_drift;
}
void GSPController::set_x_Kp(const double & v)
{
    x_pid_controller->set_Kp(v);
}
void GSPController::set_x_Ki(const double & v)
{
    x_pid_controller->set_Ki(v);
}
void GSPController::set_x_Kd(const double & v)
{
    x_pid_controller->set_Kd(v);
}
void GSPController::set_x_integral_limit(const double & v)
{
    x_pid_controller->set_integral_limit(v);
}
void GSPController::set_z_Kp(const double & v)
{
    z_pid_controller->set_Kp(v);
}
void GSPController::set_z_Ki(const double & v)
{
    z_pid_controller->set_Ki(v);
}
void GSPController::set_z_Kd(const double & v)
{
    z_pid_controller->set_Kd(v);
}
void GSPController::set_z_integral_limit(const double & v)
{
    z_pid_controller->set_integral_limit(v);
}
double GSPController::get_x_Kp()
{
    return x_pid_controller->get_Kp();
}
double GSPController::get_x_Ki()
{
    return x_pid_controller->get_Ki();
}
double GSPController::get_x_Kd()
{
    return x_pid_controller->get_Kd();
}
double GSPController::get_x_integral_limit()
{
    return x_pid_controller->get_integral_limit();
}
double GSPController::get_z_Kp()
{
    return z_pid_controller->get_Kp();
}
double GSPController::get_z_Ki()
{
    return z_pid_controller->get_Ki();
}
double GSPController::get_z_Kd()
{
    return z_pid_controller->get_Kd();
}
double GSPController::get_z_integral_limit()
{
    return z_pid_controller->get_integral_limit();
}

void GSPController::set_z_shift_angle(double angle)
{
    z_shift_angle = angle;
}

double GSPController::get_z_shift_angle()
{
    return z_shift_angle;
}
void GSPController::load()
{
    Resource resource(RESOURCE_SECTION);
    if(resource.load(RESOURCE_FILENAME))
    {
        bool ok;
        QVariant tmp;
        tmp                = resource.get("X_SPEED_MAX", ok);
        if(ok) X_SPEED_MAX = tmp.toInt();
        tmp                = resource.get("Z_SPEED_MAX", ok);
        if(ok) Z_SPEED_MAX = tmp.toInt();
        tmp                = resource.get("X_ANGLE_MIN", ok);
        if(ok) X_ANGLE_MIN = tmp.toInt();
        tmp                = resource.get("X_ANGLE_MAX", ok);
        if(ok) X_ANGLE_MAX = tmp.toInt();
        tmp                = resource.get("Z_ANGLE_MIN", ok);
        if(ok) Z_ANGLE_MIN = tmp.toInt();
        tmp                = resource.get("Z_ANGLE_MAX", ok);
        if(ok) Z_ANGLE_MAX = tmp.toInt();
        tmp                = resource.get("Z_ANGLE_SHIFT", ok);
        if(ok) z_shift_angle = tmp.toDouble();
    }
    else
    {
        save();
    }
}
void GSPController::save()
{
    Resource resource(RESOURCE_SECTION);
    resource.set("X_SPEED_MAX", X_SPEED_MAX);
    resource.set("Z_SPEED_MAX", Z_SPEED_MAX);
    resource.set("X_ANGLE_MIN", X_ANGLE_MIN);
    resource.set("X_ANGLE_MAX", X_ANGLE_MAX);
    resource.set("Z_ANGLE_MIN", Z_ANGLE_MIN);
    resource.set("Z_ANGLE_MAX", Z_ANGLE_MAX);
    resource.set("Z_ANGLE_SHIFT", z_shift_angle);
    resource.save(RESOURCE_FILENAME);
}
void GSPController::start_host_etld_object_error()
{
    x_pid_controller->reset();
    z_pid_controller->reset();
}
void GSPController::set_host_etld_object_error(const double & x_angle_error, const double & z_angle_error, int32_t & x_speed, int32_t & z_speed)
{
    x_speed = int32_t(round(x_pid_controller->next(x_angle_error))) - x_drift;
    z_speed = int32_t(round(z_pid_controller->next(z_angle_error))) - z_drift;
//    qInfo(logGSPController(), "set_host_etld_object_error: { x_angle_error[ %.3f ]z_angle_error[ %.3f ] } { x_speed[ %.3f ]z_speed[ %.3f ] }", x_angle_error, z_angle_error, x_speed, z_speed);
}
void GSPController::set_abs_speed(const double & _x_speed, const double & _z_speed, int32_t & x_speed, int32_t & z_speed)
{
    x_speed = int32_t(round(_x_speed)) - x_drift;
    z_speed = int32_t(round(_z_speed)) - z_drift;
    if(x_speed < -X_SPEED_MAX) x_speed = -X_SPEED_MAX;
    else if(x_speed > X_SPEED_MAX) x_speed = X_SPEED_MAX;
    if(z_speed < -Z_SPEED_MAX) z_speed = -Z_SPEED_MAX;
    else if(z_speed > Z_SPEED_MAX) z_speed = Z_SPEED_MAX;
//    qInfo(logGSPController(), "set_abs_speed: Set Platform Speed { X[ %i ]Z[ %i ] }", int32_t(x_speed), int32_t(z_speed));
}

void GSPController::set_speed(const double & norm_x_speed, const double & norm_z_speed, int32_t & x_speed, int32_t & z_speed)
{
    x_speed = int32_t(round(norm_x_speed * X_SPEED_MAX)) - x_drift;
    z_speed = int32_t(round(norm_z_speed * Z_SPEED_MAX)) - z_drift;
    //    qInfo(logGSPController(), "set_speed: Set Platform Speed { X[ %i ]Z[ %i ] }", int32_t(x_speed), int32_t(z_speed));
}
void GSPController::set_slow_speed(const double &norm_x_speed, const double &norm_z_speed, int32_t &x_speed, int32_t &z_speed)
{
    x_speed = int32_t(round(norm_x_speed * X_SPEED_SLOW_MAX)) - x_drift;
    z_speed = int32_t(round(norm_z_speed * Z_SPEED_SLOW_MAX)) - z_drift;
}
void GSPController::set_absolute_angle(const double & _abs_x_angle, const double & _abs_z_angle, int16_t & abs_x_angle, int16_t & abs_z_angle)
{
    abs_x_angle = int16_t(round(_abs_x_angle < X_ANGLE_MIN ? X_ANGLE_MIN : _abs_x_angle > X_ANGLE_MAX ? X_ANGLE_MAX : _abs_x_angle));
    abs_z_angle = int16_t(round(_abs_z_angle < Z_ANGLE_MIN ? Z_ANGLE_MIN : _abs_z_angle > Z_ANGLE_MAX ? Z_ANGLE_MAX : _abs_z_angle));
//    qInfo(logGSPController(), "set_absolute_angle: Set Absolute Angle { X[ %i ]Z[ %i ] }", int16_t(abs_x_angle), int16_t(abs_z_angle));
}
void GSPController::set_relative_angle(const double & _rel_x_angle, const double & _rel_z_angle, int16_t & rel_x_angle, int16_t & rel_z_angle)
{
    rel_x_angle = int16_t(round(_rel_x_angle < X_ANGLE_MIN ? X_ANGLE_MIN : _rel_x_angle > X_ANGLE_MAX ? X_ANGLE_MAX : _rel_x_angle));
    rel_z_angle = int16_t(round(_rel_z_angle < Z_ANGLE_MIN ? Z_ANGLE_MIN : _rel_z_angle > Z_ANGLE_MAX ? Z_ANGLE_MAX : _rel_z_angle));
//    qInfo(logGSPController(), "set_relative_angle: Set Relative Angle { X[ %i ]Z[ %i ] }", int16_t(rel_x_angle), int16_t(rel_z_angle));
}
