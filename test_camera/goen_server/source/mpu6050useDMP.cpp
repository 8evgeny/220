#include "headers/mpu6050useDMP.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

MPU6050UseDMP::MPU6050UseDMP(const string &config_path, bool &ok)
{
    kalman_yaw_vel_ptr = std::make_shared<Kalman>(config_path, section_velocity, ok); // DBG::
    if(!ok) { std::cout << "ERROR INIT Kalman yaw velocity" << std::endl; return;}
    kalman_pitch_vel_ptr = std::make_shared<Kalman>(config_path, section_velocity, ok); // DBG::
    if(!ok) { std::cout << "ERROR INIT Kalman pitch velocity" << std::endl; return;}
    kalman_roll_vel_ptr = std::make_shared<Kalman>(config_path, section_velocity, ok); // DBG::
    if(!ok) { std::cout << "ERROR INIT Kalman roll velocity" << std::endl; return;}

    i2c_ptr = make_shared<I2Cdev>();
    i2c_ptr->initialize(i2c_bus);
    ok = get_ini_params(config_path);
    if(!ok) {cout << "ERROR MPU6050UseDMP::get_ini_params!" << endl;}
    else {cout << "OK MPU6050UseDMP::get_ini_params" << endl;}
    mpu_ptr = make_shared<MPU6050>();
    mpu_ptr->initialize();
    cout << "BEGIN dmpInitialize()" << endl;
    mpu_ptr->dmpInitialize();
    k_accel = getAccelRangeKoef();
    k_gyro = getGyroRangeKoef();
    mpu_ptr->setDMPEnabled(true);
} // -- END MPU6050UseDMP

MPU6050UseDMP::~MPU6050UseDMP()
{
    cout << "Sensor Destructor" << endl;
} // -- END ~MPU6050UseDMP


void MPU6050UseDMP::start()
{
    thread thrd(&MPU6050UseDMP::work, this);
    thrd.detach();
} // -- END start

void MPU6050UseDMP::stop()
{
    cout << "MPU6050UseDMP::BEGIN stop MPU6050UseDMP" << endl;
    f_exec.store(false);
    this_thread::sleep_for(100ms);
    cout << "MPU6050UseDMP:: Execution was stop!" << endl;
} // -- END stop

void MPU6050UseDMP::work()
{
    cout << "start work()" << endl;
    time_point tp_startapp = std::chrono::system_clock::now();
    time_now = (std::chrono::system_clock::now() - tp_startapp).count() * 1e-9;
    f_exec.store(true);
    while(f_exec.load())
    {
        if(mpu_ptr->dmpGetCurrentFIFOPacket(fifoBuffer))
        {
            // cout << "Read ok" << endl;
            time_now = (std::chrono::system_clock::now() - tp_startapp).count() * 1e-9;
            /// DMP (Digital Motion Processor) angle and velocity calculation
            mpu_ptr->dmpGetQuaternion(&q, fifoBuffer);
            mpu_ptr->dmpGetGravity(&gravity, &q);
            mpu_ptr->dmpGetYawPitchRoll(ypr, &q, &gravity); //  нормированные значения крена/тангажа/азимута [- pi; pi]
            yaw_angle_deg = ypr[0] * rad2deg;   // приводим значения из радиан в градусы
            pitch_angle_deg = ypr[2] * rad2deg;
            roll_angle_deg = ypr[1] * rad2deg;

            switch(velocity_type)
            {
            case 0: {get_gyro_velocity(); break;}
            case 1: {get_dmp_velocity(); break;}
            case 2: {get_dimension_velocity(); break;}
            default: {cout << "MPU6050UseDMP::work::UNKNOWN VELOCITY TYPE!" << endl; f_exec.store(false); break;}
            } // END switch(velocity_type)

            if(use_kalman == 1)
            {
                float vel_filtered, stash;
                kalman_yaw_vel_ptr->work(yaw_vel_deg_sec, vel_filtered, stash);
                yaw_vel_deg_sec = vel_filtered;
                kalman_pitch_vel_ptr->work(pitch_vel_deg_sec, vel_filtered, stash);
                pitch_vel_deg_sec = vel_filtered;
                kalman_roll_vel_ptr->work(roll_vel_deg_sec, vel_filtered, stash);
                roll_vel_deg_sec = vel_filtered;
            } // END if(use_kalman == 1)

            if(f_calib.load())
            {
                i_err_calib = 0;
                y0_new -= yaw_angle_deg;
                p0_new -= pitch_angle_deg;
                r0_new -= roll_angle_deg;
                vy0_new -= yaw_vel_deg_sec;
                vp0_new -= pitch_vel_deg_sec;
                vr0_new -= roll_vel_deg_sec;
                i_calib++;
                if(i_calib == i_calib_max)
                {
                    float i_calib_max_1 = 1.f / i_calib_max;
                    cout << "Summ angle at " << i_calib_max << " measurments is YPR = " << Point3f(y0_new, p0_new, r0_new) << endl;
                    y0 = y0_new * i_calib_max_1;
                    p0 = p0_new * i_calib_max_1;
                    r0 = r0_new * i_calib_max_1;
                    cout << "Summ velocity at " << i_calib_max << " measurments is vYvPvR = " << Point3f(vy0_new, vp0_new, vr0_new) << endl;
                    vy0 = vy0_new * i_calib_max_1;
                    vp0 = vp0_new * i_calib_max_1;
                    vr0 = vr0_new * i_calib_max_1;
                    cout << "New calib use YPR: " << Point3f(y0, p0, r0) << "; vYvPvR" << Point3f(vy0, vp0, vr0) << endl;
                    f_calib.store(false);
                    if(time_now >= 20 && !f_start_calib.load())
                    {
                        f_start_calib.store(true);
                    } // END if(time_now >= 20 && !f_start_calib.load())
                    i_err_calib = 0;
                    i_calib = 0;
                    y0_new = 0;
                    p0_new = 0;
                    r0_new = 0;
                    vy0_new = 0;
                    vp0_new = 0;
                    vr0_new = 0;
                } // END if(i_calib > i_calib_max)
            } // END if(f_calib.load())
            if(time_now >= 20 && !f_start_calib.load() && !f_calib.load())
            {
                calib_zero_auto();
            } // END if(time_now >= 20 && !f_start_calib.load())
            f_ready.store(true);
        } // END if(mpu_ptr->dmpGetCurrentFIFOPacket(mpu_dmp_ptr->fifoBuffer))
        else
        {
            if(f_calib.load())
            {
                i_err_calib++;
                if(i_err_calib >= i_err_calib_max)
                {
                    cout << "Cant calib! Error::FIFO buffer acces denied\n";
                    cout << "Try again for " << i_calib << "; " <<  i_err_calib << "/" << i_err_calib_max <<  endl;
                    cout << "Old calib use YPR: " << Point3f(y0, p0, r0) <<
                        "; vYvPvR" << Point3f(vy0, vp0, vr0)<< "; vYvPvR" << Point3f(vy0, vp0, vr0) << endl;
                    f_calib.store(false);
                    i_err_calib = 0;
                    i_calib = 0;
                    y0_new = 0;
                    p0_new = 0;
                    r0_new = 0;
                    vy0_new = 0;
                    vp0_new = 0;
                    vr0_new = 0;
                } // END if (i_err_calib > i_err_calib_max)
            } // END if(f_calib.load())
            // cout << "FIFO buffer not read in work! sleep 10ms" << endl;
            this_thread::sleep_for(10ms);
        } // END if(!mpu_ptr->dmpGetCurrentFIFOPacket(mpu_dmp_ptr->fifoBuffer))
    } // END while(f_exec.load())
    cout << "MPU6050UseDMP::work::END EXECUTION!" << endl;
} // -- END work

void MPU6050UseDMP::get_gyro_velocity()
{
    mpu_ptr->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    pitch_vel_deg_sec = (float)gx * k_gyro;
    roll_vel_deg_sec = (float)gy * k_gyro;
    yaw_vel_deg_sec = (float)gz * k_gyro;
} // END get_gyro_velocity

void MPU6050UseDMP::get_dimension_velocity()
{
    delta_t_1 = 1.f / (time_now - time_prev);
    yaw_vel_deg_sec = (yaw_angle_deg - yaw_prev) * delta_t_1;
    pitch_vel_deg_sec = (pitch_angle_deg - pitch_prev) * delta_t_1;
    roll_vel_deg_sec = (roll_angle_deg - roll_prev) * delta_t_1;
    cout << "YPR[dt] = " << yaw_vel_deg_sec << ", " << pitch_vel_deg_sec << ", " << roll_vel_deg_sec << ", " << endl;
    yaw_prev = yaw_angle_deg;
    pitch_prev = pitch_angle_deg;
    roll_prev = roll_angle_deg;
    time_prev = time_now;
} // END get_dimension_velocity

void MPU6050UseDMP::get_dmp_velocity()
{
    // получение скоростей zyx-yxz
    mpu_ptr->dmpGetGyro(gyro_data16, fifoBuffer);
    pitch_vel_deg_sec = gyro_data16[0] * k_gyro;
    roll_vel_deg_sec = gyro_data16[1] * k_gyro;
    yaw_vel_deg_sec = gyro_data16[2] * k_gyro;
} // END get_dmp_velocity

bool MPU6050UseDMP::get_motion(float &yaw, float &pitch, float &roll, float &vyaw, float &vpitch, float &vroll)
{
    if(f_ready.load())
    {
        lock_guard<mutex> lock(mtx);
        yaw = -(yaw_angle_deg + y0 + yh0);
        pitch = -(pitch_angle_deg + p0 + ph0);
        roll = -(roll_angle_deg + r0 + rh0);
        vyaw = (yaw_vel_deg_sec + vy0 + vyh0);
        vpitch = -(pitch_vel_deg_sec + vp0 + vph0);
        vroll = (roll_vel_deg_sec + vr0 + vrh0);
        if(yaw >= 180) {yaw -= 360;}
        if(yaw < -180) {yaw += 360;}
        if(pitch >= 180) {pitch -= 360;}
        if(pitch < -180) {pitch += 360;}
        if(roll >= 180) {roll -= 360;}
        if(roll < -180) {roll += 360;}
        f_ready.store(false);
        return true;
    } // END if(f_ready)
    else
    {
        return false;
    } // END else
} // -- END get_motion

bool MPU6050UseDMP::calib_zero_auto()
{
    cout << "\nMPU6050UseDMP::START AUTO CALIBRATION\n" << endl;
    i_err_calib = 0;
    i_calib = 0;
    vy0_new = 0;
    vp0_new = 0;
    r0_new = 0;
    vy0_new = 0;
    vp0_new = 0;
    vr0_new = 0;
    f_calib.store(true);
    return true;
} // -- END calib_zero_auto

bool MPU6050UseDMP::calib_zero_handle(float yaw0, float pitch0, float roll0, float vyaw0, float vpitch0, float vroll0)
{
    cout << "\nMPU6050UseDMP::START AUTO CALIBRATION\n" << endl;
    yh0 = -yaw0;
    ph0 = -pitch0;
    rh0 = -roll0;
    if(vyaw0 == -11111) {return true;}
    vyh0 = -vyaw0;
    if(vpitch0 == -11111) {return true;}
    vph0 = -vpitch0;
    if(vroll0 == -11111) {return true;}
    vrh0 = -vroll0;
    return true;
} // -- END calib_zero_handle

double MPU6050UseDMP::getGyroRangeKoef()
{
    // std::cout << "/n"
    //              "#MPU6050_GYRO_FS_250         0x00\n"
    //              "#MPU6050_GYRO_FS_500         0x01\n"
    //              "#MPU6050_GYRO_FS_1000        0x02\n"
    //              "#MPU6050_GYRO_FS_2000        0x03\n";
    int res = (int)mpu_ptr->getFullScaleGyroRange();
    switch(res)
    {
    case 0:
    {
        std::cout << "Work in range MPU6050_GYRO_FS_250 (+-250 [deg/s]). LSB/(deg/s) = 131.\n";
        return v_gyro_LSBg[res];
        break;
    } // END case 0:
    case 1:
    {
        std::cout << "Work in range MPU6050_GYRO_FS_500 (+-500 [deg/s]). LSB/(deg/s) = 65.5.\n";
        return v_gyro_LSBg[res];
        break;
    } // END case 1:
    case 2:
    {
        std::cout << "Work in range MPU6050_GYRO_FS_1000 (+-1000 [deg/s]). LSB/(deg/s) = 32.8.\n";
        return v_gyro_LSBg[res];
        break;
    } // END case 2:
    case 3:
    {
        std::cout << "Work in range MPU6050_GYRO_FS_2000 (+-2000 [deg/s]). LSB/(deg/s) = 16.4.\n";
        return v_gyro_LSBg[res];
        break;
    } // END case 3:
    default:
    {
        std::cout << "Incorret mean of FullScaleGyroRange!\n";
        break;
    } // END default:
    } // END switch(res)
    return -1;
} // -- END getGyroRangeKoef

double MPU6050UseDMP::getAccelRangeKoef()
{
    // std::cout << "/n"
    //              "#define MPU6050_ACCEL_FS_2          0x00\n"
    //              "#define MPU6050_ACCEL_FS_4          0x01\n"
    //              "#define MPU6050_ACCEL_FS_8          0x02\n"
    //              "#define MPU6050_ACCEL_FS_16         0x03\n";
    int res = (int)mpu_ptr->getFullScaleAccelRange();
    switch(res)
    {
    case 0:
    {
        std::cout << "Work in range MPU6050_ACCEL_FS_2 (+-2g). LSB/g = 16384.\n";
        return v_accel_LSBg[res];
        break;
    } // END case 0:
    case 1:
    {
        std::cout << "Work in range MPU6050_ACCEL_FS_4 (+-4g). LSB/g = 8192.\n";
        return v_accel_LSBg[res];
        break;
    } // END case 1:
    case 2:
    {
        std::cout << "Work in range MPU6050_ACCEL_FS_8 (+-8g). LSB/g = 4096.\n";
        return v_accel_LSBg[res];
        break;
    } // END case 2:
    case 3:
    {
        std::cout << "Work in range MPU6050_ACCEL_FS_16 (+-16g). LSB/g = 2048.\n";
        return v_accel_LSBg[res];
        break;
    } // END case 3:
    default:
    {
        std::cout << "Incorret mean of FullScaleAccelRange!\n";
        return -1;
        break;
    } // END default:
    } // END switch(res)
    return -1;
} // -- END getAccelRangeKoef

bool MPU6050UseDMP::get_ini_params(const string &config)
{
    cout << "BMPU6050UseDMP::EGIN get_ini_params" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");
    INIReader reader(config);
    cout << "OK create reader. ParseError = " << reader.ParseError() << endl;
    if(reader.ParseError() < 0)
    {
        cout << "Can't load '" << config << "'\n";
        return 0;
    } // END if(reader.ParseError() < 0)

    cout << "\n[" << section_name << "]:" << endl;
    velocity_type = reader.GetInteger(section_name, "velocity_type", ini_err_value);
    if(velocity_type == ini_err_value){std::cout << "\t[" << section_name << "]velocity_type not declared\n"; return 0;}
    std::cout << "\tvelocity_type = " << velocity_type << ";\n";

    use_kalman = reader.GetInteger(section_name, "use_kalman", ini_err_value);
    if(use_kalman == ini_err_value){std::cout << "\t[" << section_name << "]use_kalman not declared\n"; return 0;}
    std::cout << "\tuse_kalman = " << use_kalman << ";\n";
    return 1;
} // -- END get_ini_params

