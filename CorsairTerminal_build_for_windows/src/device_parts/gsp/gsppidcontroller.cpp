#include "gsppidcontroller.h"

#include <cmath>

GSPPIDController::GSPPIDController(const double & Kp, const double & Ki, const double & Kd, const double & integral_limit):
    Kp(Kp),
    Ki(Ki),
    Kd(Kd),
    integral_limit(integral_limit),
    prev_signal(0),
    signal(0)
{
}

void GSPPIDController::reset()
{
    prev_signal = 0;
    signal = 0;
}

double GSPPIDController::next(const double & next_signal)
{
    double signal_p = next_signal * Kp;
    double signal_i = signal + next_signal * Ki;
    double signal_d = (next_signal - prev_signal) * Kd;
    prev_signal = next_signal;
    if(fabs(signal_i) > integral_limit)
    {
        if(signal_i < 0)
        {
            signal_i = -integral_limit;
        }
        else
        {
            signal_i = integral_limit;
        }
    }
    signal = signal_p + signal_i + signal_d;
    return signal;
}

void GSPPIDController::set_Kp(const double & v)
{
    Kp = v;
}
void GSPPIDController::set_Ki(const double & v)
{
    Ki = v;
}
void GSPPIDController::set_Kd(const double & v)
{
    Kd = v;
}
void GSPPIDController::set_integral_limit(const double & v)
{
    integral_limit = v;
}

double GSPPIDController::get_Kp()
{
    return Kp;
}
double GSPPIDController::get_Ki()
{
    return Ki;
}
double GSPPIDController::get_Kd()
{
    return Kd;
}
double GSPPIDController::get_integral_limit()
{
    return integral_limit;
}
