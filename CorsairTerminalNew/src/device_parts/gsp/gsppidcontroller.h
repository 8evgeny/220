#ifndef GSPPIDCONTROLLER_H
#define GSPPIDCONTROLLER_H


class GSPPIDController
{
public:
    GSPPIDController(const double & Kp, const double & Ki, const double & Kd, const double & integral_limit);

    void reset();

    double next(const double & next_signal);

    void set_Kp(const double & v);
    void set_Ki(const double & v);
    void set_Kd(const double & v);
    void set_integral_limit(const double & v);

    double get_Kp();
    double get_Ki();
    double get_Kd();
    double get_integral_limit();

private:
    double Kp;
    double Ki;
    double Kd;
    double integral_limit;
    double prev_signal;
    double signal;
};

#endif // GSPPIDCONTROLLER_H
