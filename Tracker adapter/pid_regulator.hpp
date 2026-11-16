#ifndef PID_REGULATOR_HPP
#define PID_REGULATOR_HPP


class PIDRegulator
{
public:
    PIDRegulator();
    PIDRegulator(float kp, float ki, float kd, float output_lim);
    float calc(float err);
    void set_coefs(float kp, float ki, float kd, float output_lim, float integral_lim);
    void reset();
private:
    float m_ki = 0, m_kp = 0, m_kd = 0;
    float m_err_prev = 0;
    float m_integ = 0;
    float m_output_lim = 0.02;
    float m_integ_lim = 0.01;
}; /* PIDRegulator */

#endif /* PID_REGULATOR_HPP */