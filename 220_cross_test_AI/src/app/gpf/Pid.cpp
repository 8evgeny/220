#include "Pid.h"
#include <algorithm>

Pid::Pid(const float Kp, const float Ki, const float Kd)
    : m_Kp(Kp), m_Ki(Ki), m_Kd(Kd), m_integral(0.0f), m_prevError(0.0f)
{
} /* Pid::Pid */

void Pid::setKp(const float Kp)
{
    m_Kp = Kp;
} /* Pid::setKp */
void Pid::setKi(const float Ki)
{
    m_Ki = Ki;
} /* Pid::setKi */
void Pid::setKd(const float Kd)
{
    m_Kd = Kd;
} /* Pid::setKd */

float Pid::getPidOutput(const float error)
{
    const float P = m_Kp * error;

    m_integral += error;
    if (m_integral > 50.0f)
        m_integral = 50.0f; // анти-windup
    if (m_integral < -50.0f)
        m_integral = -50.0f;

    const float I = m_Ki * m_integral;

    const float D = m_Kd * (error - m_prevError);
    m_prevError = error;

    // ★ Ограничение выхода ★
    return std::clamp(P + I + D, -m_limit, m_limit);
} /* Pid::getPidOutput */
