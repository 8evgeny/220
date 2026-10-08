#pragma once
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "MPU6050.h"
#include "INIReader.h"
#include <thread>
#include <iostream>
#include <kalman.hpp>
#include <chrono>
#include <thread>
#include <atomic>

/// YPR=YXZ(?)

class MPU6050UseDMP
{


public:
    MPU6050UseDMP(const string &pathToConfig, bool &ok);
    ~MPU6050UseDMP();
    void start();
    void stop();
    void work();
    bool get_motion(float &yaw, float &pitch, float &roll, float &vyaw, float &vpitch, float &vroll);
    bool calib_zero_auto();
    bool calib_zero_velocity();
    bool calib_zero_handle(float yaw0, float pitch0, float roll0, float vyaw0 = -11111, float vpitch0 = -11111, float vroll0 = -11111);

    std::shared_ptr<I2Cdev> i2c_ptr = nullptr;
    const char * i2c_bus = "/dev/i2c-6";
    std::shared_ptr<MPU6050> mpu_ptr = nullptr;
    uint8_t fifoBuffer[45]; // буфер для чтения регистров DMP MPU6050

    /// Synchronization
    std::atomic<bool> f_start_calib = {false};
    std::atomic<bool> f_ready = {false};
    std::atomic<bool> f_exec = {false};
    std::atomic<bool> f_calib = {false};// флаг синхронизации, 0 - калибровки нет, 1 - идёт калибровка нулевого положения и скоростей углов/угловых скоростей
    std::mutex mtx;
private:
    // Переменные для вывода результата работы
    int velocity_type = 0;
    double yaw_angle_deg, pitch_angle_deg, roll_angle_deg;         // переменные для хранения текущего угла [deg]
    double yaw_vel_deg_sec, pitch_vel_deg_sec, roll_vel_deg_sec;   //  переменные для хранения текущих соростей по осям [deg/s]
    double k_accel = 1.0; // LSB/[deg/s^2]
    double k_gyro = 1.0;                // LSB/[deg/s]
    std::vector<double> v_gyro_LSBg = {1.f / 131.0, 1.f / 65.5, 1.f / 32.8, 1.f / 16.4};
    std::vector<double> v_accel_LSBg = {1.f / 16384.f, 1.f / 8192.f, 1.f / 4096.f, 1.f / 2048.f};
    int16_t ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0; // переменные для хранения "сырых" данных с акселероетров и гироскопов
    int16_t gyro_data16[3] = {0,0,0};
    Quaternion q;
    VectorFloat gravity;
    float ypr[3] = {0,0,0}; // массив абсолютных углов (YawPitchRoll) в промежутке -pi..pi [rad]
    int ini_err_value = -1111111111;
    /// Calibration
    float i_calib = 0;
    float i_calib_max = 100;
    float i_calib_max_1 = 1.f / i_calib_max;
    float i_err_calib = 0;
    float i_err_calib_max = 20;
    double y0 = 0, p0 = 0, r0 = 0;      // [deg] смещения углов для осей азимута, тангажа и крена в результате автокалибровки ( YawPitchRoll)
    double vy0 = 0, vp0 = 0, vr0 = 0;   // [deg/s] смещение значения нулевой скорости по осям азимута, тангажа и крена в результате автокалибровки ( YawPitchRoll)
    double y0_new = 0, p0_new = 0, r0_new = 0;
    double vy0_new = 0, vp0_new = 0, vr0_new = 0;
    double yh0 = 0, ph0 = 0, rh0 = 0; // [deg] смещения углов для осей азимута, тангажа и крена добавленные ручной калибровкой
    double vyh0 = 0, vph0 = 0, vrh0 = 0;   // [deg/s] смещение значения нулевой скорости по осям азимута, тангажа и крена добавленные ручной калибровкой

    const double rad2deg = 180.f / M_PI;
    std::chrono::system_clock::time_point tp_startapp;
    float time_now = 0;     // время считывания FIFObuffer
    float time_prev = 0;    // время предыдущего успешного считываеня FIFObuffer
    float delta_t = 0;      // время между текущим и предыдущий считываниями FIFObuffer
    float delta_t_1 = 0;    // мгновенная частота между считывания данных из  FIFObuffer
    float yaw_prev = 0, roll_prev = 0, pitch_prev = 0;  //

    int use_kalman = 0;                                         // флаг, указывающий необходимо ли выполнять фильтрацию Калмана для скоростей
    std::string section_velocity = "Kalman_velocity_settings";  // раздел параметров фильтра Калмана в штш-файле
    float yaw_vel_k = 0, pitch_vel_k = 0, roll_vel_k = 0;       //  переменные для хранения отфильтрованных соростей по осям [deg/s]
    std::shared_ptr<Kalman> kalman_yaw_vel_ptr = nullptr;
    std::shared_ptr<Kalman> kalman_pitch_vel_ptr = nullptr;
    std::shared_ptr<Kalman> kalman_roll_vel_ptr = nullptr;

    std::string section_name = "MPU6050";

    void get_gyro_velocity();
    void get_dimension_velocity();
    void get_dmp_velocity();

    double getAccelRangeKoef();
    double getGyroRangeKoef();
    bool get_ini_params(const string &config);

}; // END class MPU6050UseDMP
