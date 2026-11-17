#include "pid_regulator.hpp"
#include <math.h>


PIDRegulator::PIDRegulator(float kp, float ki, float kd, float output_lim) : m_ki(ki), m_kp(kp), m_kd(kd), m_output_lim(output_lim)
{

} /* PIDRegulator(...) */

PIDRegulator::PIDRegulator()
{

} /* PIDRegulator */

void PIDRegulator::set_coefs(float kp, float ki, float kd, float output_lim, float integral_lim)
{
    m_ki = ki;
    m_kp = kp;
    m_kd = kd;
    m_output_lim = output_lim;
    m_integ_lim = integral_lim;
} /* set_coefs */

void PIDRegulator::reset()
{
    m_integ = 0;
    m_err_prev = 0;
} /* reset */

float PIDRegulator::calc(float err)
{
    float p_gain = err * m_kp;
    float d_gain = (err - m_err_prev) * m_kd;
    m_integ = err * m_ki + m_integ;
    if(std::fabs(m_integ_lim) < std::fabs(m_integ))
    {
        m_integ = m_integ_lim * std::fabs(m_integ) / m_integ;
    }

    m_err_prev = err;
    float output = p_gain + d_gain + m_integ;
    // std::cout << "DBG: [AEGIC]: p_gain=" << p_gain << "; d_gain=" << d_gain << "; integ=" << integ << std::endl;
    if(std::fabs(output) > m_output_lim)
    {
        if(output > 0)
        {
            return m_output_lim;
        }
        return -1.0f * m_output_lim;
    }
    return output;
} /* calc */