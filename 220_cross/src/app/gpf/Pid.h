#ifndef PID_H
#define PID_H



class Pid
{
public:
    Pid( const float Kp = 0.01f, const float Ki = 0.0f, const float Kd = 0.0f);

    void setKp( const float Kp);
    void setKi( const float Ki);
    void setKd( const float Kd);

    float getPidOutput( const float error);

private:
    float 	m_Kp;
    float 	m_Ki;
    float 	m_Kd;

    float 	m_integral;
    float 	m_prevError;
};



#endif
