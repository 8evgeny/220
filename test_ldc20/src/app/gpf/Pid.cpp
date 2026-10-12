
#include "Pid.h"




Pid::Pid( const float Kp, const float Ki, const float Kd)
    : m_Kp( Kp)
    , m_Ki( Ki)
    , m_Kd( Kd)
    , m_integral( 0.0f)
    , m_prevError( 0.0f)
{

}


void Pid::setKp( const float Kp)
{
    m_Kp = Kp;
}


void Pid::setKi( const float Ki)
{
    m_Ki = Ki;
}


void Pid::setKd( const float Kd)
{
    m_Kd = Kd;
}


float Pid::getPidOutput( const float error)
{
    const float P = m_Kp * error;

    m_integral += error;
    const float I = m_Ki * m_integral;

    const float D = m_Kd * (error - m_prevError);

    m_prevError = error;

    return (P + I + D);
}



