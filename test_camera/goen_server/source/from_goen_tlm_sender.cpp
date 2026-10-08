#include "application.hpp"

using namespace std;
using namespace cv; 
using namespace chrono;

void Application::set_from_goen_telemetry()
{
    set_status_fields();
    to_bort_tlm_rs232_str.azimuth_angular_velocity = (int16_t)round(-rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.speed * 100.f);
    to_bort_tlm_rs232_str.pitch_angular_velocity = (int16_t)round(rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.speed * 100.f);
    to_bort_tlm_rs232_str.azimuth_frame_angle = (int16_t)round(-(rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle  - rs485_worker_ptr->ang0_x) * 100.f);
    to_bort_tlm_rs232_str.pitch_frame_angle = (int16_t)round((rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.angle - rs485_worker_ptr->ang0_y) * 100.f );
    set_absolute_navigation();
#ifdef USE_GUI
    gimbal_pitch_deg = rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.angle;
    gimbal_azimuth_deg = -rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle;
#endif // USE_GUI
    rs485_worker_ptr->mut_telemetry_str.unlock();
    set_target_miss();
    // cout << "f_azimuth_follow = " << rs485_worker_ptr->f_azimuth_follow << endl;;
#ifdef USE_UDP_TLM
    set_extension_udp_tlm();
#endif // USE_UDP_TLM

    sir.imaging_plate = 0;              // 0 - normal; 1 - error
    // sir.encoder_and_servo_drive = 0 ;     // 0 - normal; 1 - error
    sir.encoder_and_servo_drive = (rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.errors |                   // Есть флаг ошибки по полю ошибок двигателей (MotorStatus)
                                   rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.errors) |
                                  (rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.flags & 0b0000'0001) |        // Есть ошибка калибровки двигателя
                                  (rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.flags & 0b0000'0001);

// Если производится калибровка одного из двигателей, устанавливаем значение 1:  // 0 - success ; 1 - in process;
#ifdef USE_I2C_DMP
    sir.gyroscope_calibration = (mpu6050_dmp_ptr->f_calib.load() || !mpu6050_dmp_ptr->f_start_calib.load() ||
                                 (rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.flags & 0b0000'0100) ||
                                 (rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.flags & 0b0000'0100));
#else // USE_I2C_DMP
    sir.gyroscope_calibration = (rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.flags & 0b0000'0100) ||
                                (rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.flags & 0b0000'0100);
#endif // !USE_I2C_DMP
    sir.self_inspection_completed = !sir.imaging_plate && !sir.encoder_and_servo_drive && !sir.gyroscope_calibration; // 1 - completed; 0 - in rogress

} // -- END set_from_goen_telemetry

#ifdef USE_UDP_TLM
void Application::set_extension_udp_tlm()
{
    // <0 - 3>
    if(tracShats->isInited() && flag_zahvat)
    {
        f_valid = 1;
    } // END if(tracShats->isInited())
    else if(tracShats->isInited() && !flag_zahvat)
    {
        f_valid -= valid_ap_step_decrease;
        if(f_valid < 0) {f_valid = 0; f_attack = 0;}
    } // END elif(tracShats->isInited() && !flag_zahvat)
    else
    {
        f_valid = 0;
        f_attack = 0;
    } // END if(tracShats->isInited())

    tlm_ap_str.valid = f_valid;

    // <4 - 11>
    tlm_ap_str.target_miss_x = to_bort_tlm_rs232_str.target_miss_x * 5 * k_deg2rad_100;
    tlm_ap_str.target_miss_y = to_bort_tlm_rs232_str.target_miss_y * 5 * k_deg2rad_100;
    // <12 - 19>
    tlm_ap_str.motor_azimuth = to_bort_tlm_rs232_str.azimuth_frame_angle * k_deg2rad_100;
    tlm_ap_str.motor_pitch = to_bort_tlm_rs232_str.pitch_frame_angle * k_deg2rad_100;
#ifdef USE_I2C_DMP
    // <20 - 27>
    tlm_ap_str.frame_pitch = to_bort_tlm_rs232_str.abs_pitch * k_deg2rad_100;
    tlm_ap_str.frame_roll = to_bort_tlm_rs232_str.abs_roll * k_deg2rad_100;
#endif // USE_I2C_DMP
    // <28 - 35>
    tlm_ap_str.pitch_angular_velocity = to_bort_tlm_rs232_str.pitch_angular_velocity * k_deg2rad_100;
    tlm_ap_str.azimuth_angular_velocity = to_bort_tlm_rs232_str.azimuth_angular_velocity * k_deg2rad_100;;
    // <36 - 39>
    tlm_ap_str.attack = f_attack;
    if(f_attack) {sif2.reserved1 = true;}
    else {sif2.reserved1 = false;}
} // -- END set_extension_udp_tlm();

#endif // USE_UDP_TLM


void Application::set_absolute_navigation()
{
#ifdef USE_I2C_DMP
    if(mpu6050_dmp_ptr->get_motion(abs_yaw, abs_pitch, abs_roll, abs_vel_yaw, abs_vel_pitch, abs_vel_roll))
    {
        float yaw_res = 0, pitch_res = 0, roll_res;
        // cout << "Motor angles: " << Point2f(-rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle, rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.angle) << endl;
#ifndef USE_UDP_TLM
        rotate_matrix(0, abs_pitch, abs_yaw, abs_roll,
                      rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.angle - rs485_worker_ptr->ang0_y, rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle - rs485_worker_ptr->ang0_x,
                      pitch_res, yaw_res, roll_res);
        get_vel(pitch_res, yaw_res, roll_res, abs_vel_pitch, abs_vel_yaw, abs_vel_roll);
#else // USE_UDP_TLM
            pitch_res = abs_pitch;
            roll_res = abs_roll;
            yaw_res = abs_yaw;
#endif // USE_UDP_TLM
#ifdef USE_TLM_MODE_CHANGER
        if(abs_angle_enable)
        {
#endif // #ifdef USE_TLM_MODE_CHANGER
            to_bort_tlm_rs232_str.abs_roll = (int16_t)round(roll_res * 100.f);
            to_bort_tlm_rs232_str.abs_pitch = (int16_t)round(pitch_res * 100.f);
            to_bort_tlm_rs232_str.abs_azimutch = (int16_t)round(yaw_res * 100.f);
#ifdef USE_TLM_MODE_CHANGER
        } // END if(abs_angle_enable)
        else
        {
            to_bort_tlm_rs232_str.abs_roll = 0;
            to_bort_tlm_rs232_str.abs_pitch = 0;
            to_bort_tlm_rs232_str.abs_azimutch = 0;
        } // END if(!abs_angle_enable)
#endif // END #ifdef USE_TLM_MODE_CHANGER

#ifdef USE_TLM_MODE_CHANGER
        if(angular_velocity_enable)
        {
#endif // END #ifdef USE_TLM_MODE_CHANGER
        to_bort_tlm_rs232_str.azimuth_angular_velocity = (int16_t)round(abs_vel_yaw * 100.f);
        to_bort_tlm_rs232_str.pitch_angular_velocity = (int16_t)round(abs_vel_pitch * 100.f);
#ifdef USE_UDP_TLM
        to_bort_tlm_rs232_str.roll_angular_velocity = 0;
#else // !USE_UDP_TLM
        to_bort_tlm_rs232_str.roll_angular_velocity = (int16_t)round(abs_vel_roll * 100.f);
#endif // USE_UDP_TLM

#ifdef USE_TLM_MODE_CHANGER
        } // END if(angular_velocity_enable)
        else
        {
            to_bort_tlm_rs232_str.azimuth_angular_velocity = 0;
            to_bort_tlm_rs232_str.pitch_angular_velocity = 0;
            to_bort_tlm_rs232_str.roll_angular_velocity = 0;
        } // END if(!angular_velocity_enable)
#endif // END #ifdef USE_TLM_MODE_CHANGER
#ifdef USE_DBG_PLOT
        tp_now = std::chrono::system_clock::now();
        time = (tp_now - tp_startapp).count() * 1e-9;
        // key = add6(abs_yaw, abs_pitch, abs_roll, abs_vel_yaw, abs_vel_pitch, abs_vel_roll, time);
        key = add6(yaw_res, pitch_res, roll_res, abs_vel_yaw, abs_vel_pitch, abs_vel_roll, time);
        if(key == 'c') {mpu6050_dmp_ptr->calib_zero_auto();}
        // graph_ptr->draw_rotate_vec(yaw_res, pitch_res, roll_res, x_res, y_res, z_res); /// DBG::
#endif // USE_DBG_PLOT
} // END if(mpu6050_dmp_ptr->get_motion(y, r, p, vy, vr, vp))
#endif // USE_I2C_DMP
} // END set_absolute_navigation

void Application::set_status_fields()
{
    // to_bort_tlm_rs232_str = ToBortTelemetry();
    if(tracShats != nullptr)
    {
        sif1.target_tracking_status = tracShats->isInited();
    } // END if(tracShats != nullptr)
    else
    {
        sif1.target_tracking_status = false;
    } // END else
#if defined(USE_TPV_cam)
    sif1.tracked_video_source1 = 1;
    sif1.tracked_video_source2 = 0;
#endif //END #if defined(USE_TPV_cam)
#if defined(USE_TV)
    sif1.tracked_video_source1 = 0;
    sif1.tracked_video_source2 = 0;
#endif //END #if defined(USE_TV)
    sif2.image_enchancement = f_image_enchance;

    rs485_worker_ptr->mut_telemetry_str.lock();
    /// ::AFM
#ifdef USE_MODE_STAB
    if(rs485_worker_ptr->f_azimuth_follow.load()) {sif2.follow_mode = 1;}
    else {sif2.follow_mode = 0;}
    if(rs485_worker_ptr->f_motor_lock.load()) {sif2.electric_lock_mode = 1;}
    else {sif2.electric_lock_mode = 0;}
    if(rs485_worker_ptr->f_motor_on.load()) {sif2.motor_status = 1;}
    else {sif2.motor_status = 0;}
#elif !defined(USE_MODE_STAB)
    bool f_azimuth_follow = rs485_worker_ptr->from_goen_telemetry_str.mems_flag & 0b00000100'00000000'00000000'00000000;
    rs485_worker_ptr->f_azimuth_follow.store(f_azimuth_follow);
    sif2.follow_mode = f_azimuth_follow;

    bool f_motor_lock_status = rs485_worker_ptr->from_goen_telemetry_str.mode_mems == (uint8_t)MODE_TYPE::ROTARY_PLATFORM;
    sif2.electric_lock_mode = f_motor_lock_status;
    rs485_worker_ptr->f_motor_lock.store(f_motor_lock_status);

    bool f_motor_off_status = rs485_worker_ptr->from_goen_telemetry_str.mode_mems == (uint8_t)MODE_TYPE::MOTOR_OFF;
    sif2.motor_status = !f_motor_off_status;
    rs485_worker_ptr->f_motor_on.store(!f_motor_off_status);
#endif // !defined(USE_MODE_STAB)
    uint16_t zoom_ratio = (uint16_t)round(digital_zoom_value * 10);
    bit_ptr->set_goen_status_field(to_bort_tlm_rs232_str, zoom_ratio, sif1, sif2, sif3, sir);
} // END set_status_fields

void Application::set_target_miss()
{
#ifdef USE_TLM_MODE_CHANGER
    if(tm_xy_enable)
    {
#endif // #ifdef USE_TLM_MODE_CHANGER

        if(flag_zahvat)
        {
            if(digital_zoom_value < 1)
            {
                to_bort_tlm_rs232_str.target_miss_x = (int16_t)round((tracShats->getTargetCenter().x - 0.5) * k_rel2field_x * 2.f);
                to_bort_tlm_rs232_str.target_miss_y = (int16_t)round((0.5 - tracShats->getTargetCenter().y) * k_rel2field_y * 2.f);
            } // END  if(digital_zoom_value < 1)
            else
            {
                to_bort_tlm_rs232_str.target_miss_x = (int16_t)round((tracShats->getTargetCenter().x - 0.5) * k_rel2field_x);
                to_bort_tlm_rs232_str.target_miss_y = (int16_t)round((0.5 - tracShats->getTargetCenter().y) * k_rel2field_y);
            } // END  if(digital_zoom_value >= 1)
        } // END if(flag_zahvat)
        else
        {
            to_bort_tlm_rs232_str.target_miss_x = 0;
            to_bort_tlm_rs232_str.target_miss_y = 0;
        } // END if(!flag_zahvat)}
#ifdef USE_TLM_MODE_CHANGER
    } // END if(tm_xy_enable)
    else
    {
        to_bort_tlm_rs232_str.target_miss_x = 0;
        to_bort_tlm_rs232_str.target_miss_y = 0;
    } // END if(!tm_xy_enable)}
#endif // USE_TLM_MODE_CHANGER
} // -- END set_target_miss


#ifdef USE_DBG_PLOT
uchar Application::add6(float z_, float y_, float x_, float vz_, float vy_, float vx_, float t_, bool use_kalman)
{
    plot_ptr->vv_func[0].emplace_back(cv::Point2f(t_, z_));
    plot_ptr->vv_func[1].emplace_back(cv::Point2f(t_, y_));
    plot_ptr->vv_func[2].emplace_back(cv::Point2f(t_, x_));
    plot_ptr->vv_func[3].emplace_back(cv::Point2f(t_, vz_));
    plot_ptr->vv_func[4].emplace_back(cv::Point2f(t_, vy_));
    plot_ptr->vv_func[5].emplace_back(cv::Point2f(t_, vx_));
    if(t_ > plot_ptr->x_max)
    {
        plot_ptr->x_max += plot_ptr->x_step;
        plot_ptr->x_min += plot_ptr->x_step;
    } // END if(time > plot_ptr->x_max)
    // uchar key = plot_ptr->show();
    return key;
} // -- END add6
#endif // #ifdef USE_DBG_PLOT

#ifdef USE_I2C_DMP
void Application::rotate_matrix(int mode, float t_b, float a_b, float k_b, float t_s, float a_s, float& t_res, float& a_res, float& k_res)
{
    a_s *= -1;
    float rad_t_s = k_deg2rad * t_s;
    float rad_a_s = -k_deg2rad * a_s;
    float rad_t_b = k_deg2rad * t_b;
    float rad_a_b = -k_deg2rad * a_b;
    float rad_k_b = k_deg2rad * k_b;
    float c_t_s = cos(rad_t_s);
    float s_t_s = sin(rad_t_s);
    float c_a_s = cos(rad_a_s);
    float s_a_s = sin(rad_a_s);
    float c_t_b = cos(rad_t_b);
    float s_t_b = sin(rad_t_b);
    float c_a_b = cos(rad_a_b);
    float s_a_b = sin(rad_a_b);
    float c_k_b = cos(rad_k_b);
    float s_k_b = sin(rad_k_b);

    auto get_vec = [](float ct, float st, const Point3f& v, Point3f& v_res)
    {
        float ct1 = 1.f - ct;
        float x = (ct+ct1*v.x*v.x)*v_res.x + (ct1*v.x*v.y-st*v.z)*v_res.y + (ct1*v.x*v.z+st*v.y)*v_res.z;
        float y = (ct1*v.x*v.y+st*v.z)*v_res.x + (ct+ct1*v.y*v.y)*v_res.y + (ct1*v.y*v.z-st*v.x)*v_res.z;
        float z = (ct1*v.x*v.z-st*v.y)*v_res.x + (ct1*v.y*v.z+st*v.x)*v_res.y + (ct+ct1*v.z*v.z)*v_res.z;
        v_res = Point3f(x, y, z);
    }; // END get_vec

    auto get_vec_y = [](float ct, float st, Point3f& v_res)
    {
        float x = ct*v_res.x + st*v_res.z;
        float z = -st*v_res.x + ct*v_res.z;
        v_res = Point3f(x, v_res.y, z);
    }; // END get_vec_y

    auto get_vec_z = [](float ct, float st, Point3f& v_res)
    {
        float x = ct*v_res.x - st*v_res.y;
        float y = st*v_res.x + ct*v_res.y;
        v_res = Point3f(x, y, v_res.z);
    }; // END get_vec_z

    // Оси борта Vxb, Vyb, Vzb - в начальный момент (перед поворотами):
    Point3f Vxb(1.f, 0, 0), Vyb(0, 1.f, 0), Vzb(0, 0, 1.f);
    // Поворот по тангажу борта - поворачиваем оси борта Vxb и Vyb вокруг абсолютной оси Vz(0, 0, 1.f):
    get_vec_z(c_t_b, s_t_b, Vxb); // Находим Vxb по тангажу борта.
    get_vec_z(c_t_b, s_t_b, Vyb); // Находим Vyb по тангажу борта.
    // Поворот по азимуту борта - поворачиваем оси борта Vxb, Vyb и Vzb вокруг абсолютной оси Vy(0, 1.f, 0):
    get_vec_y(c_a_b, s_a_b, Vxb); // Находим Vxb по азимуту борта.
    get_vec_y(c_a_b, s_a_b, Vyb); // Находим Vyb по азимуту борта.
    get_vec_y(c_a_b, s_a_b, Vzb); // Находим Vzb по азимуту борта.
    // Поворот по крену борта - поворачиваем оси борта Vyb и Vzb вокруг новой оси борта Vxb:
    get_vec(c_k_b, s_k_b, Vxb, Vyb); // Находим Vyb по крену борта.
    get_vec(c_k_b, s_k_b, Vxb, Vzb); // Находим Vzb по крену борта.

    // Оси шара перед поворотами шара по тангажу и по азимуту (совпадают с осями борта):
    Point3f Vxs = Vxb, Vzs = Vzb;
    // Поворот по азимуту шара - поворачиваем оси шара Vxs и Vzs вокруг оси борта Vyb (она же - вертикальная ось шара):
    get_vec(c_a_s, s_a_s, Vyb, Vxs); // Находим Vxs по азимуту шара.
    get_vec(c_a_s, s_a_s, Vyb, Vzs); // Находим Vzs по азимуту шара.
    // Поворот по тангажу шара - поворачиваем ось шара Vxs вокруг оси шара Vzs:
    get_vec(c_t_s, s_t_s, Vzs, Vxs); // Находим Vxs по тангажу шара.

    // Начало вычисления тангажа и азимута.
    t_res0 = k_rad2deg * asin(Vxs.y);
    a_res0 = k_rad2deg * atan(Vxs.z / Vxs.x);
    x_res = Vxs.x;
    y_res = Vxs.y;
    z_res = Vxs.z;
    if(x_res > eps)
    {
        t_res = t_res0;
        a_res = a_res0;
    } // END if(x_res > eps)
    else if(x_res < -eps)
    {
        t_res = t_res0;
        if(z_res > eps){a_res = 180.f - abs(a_res0);}
        else if(z_res < -eps){a_res = abs(a_res0) - 180.f;}
        else{a_res = 180.f;}
    } // END else if(x_res < -eps)
    else // if(|x_res| <= eps)
    {
        t_res = t_res0;
        if(z_res > eps){a_res = 90.f;}
        else if(z_res < -eps){a_res = -90.f;}
        else // if(|z_res| <= eps)
        {
            if(isnan(a_res0)){a_res = 0;}
            else{a_res = a_res0;}
        } // END if(|z_res| <= eps)
        } // END if(|x_res| <= eps)
    // Конец вычисления тангажа и азимута.

    // Начало вычисления крена:
    float cos2k = 1.f - Vxs.y*Vxs.y;
    if(cos2k < eps){k_res = k_b;}
    else{k_res = -k_rad2deg * asin(Vzs.y / sqrt(cos2k));}
    // Конец вычисления крена.
} // -- END rotate_matrix

void Application::get_vel(float t, float a, float k, float& Vt, float& Va, float& Vk)
{
    function get_V = [&](double Sy, double Sxy, float& V)
    {
        V = (num_mnk * Sxy - Sx * Sy) * Zn_1;
    }; // END function get_V
    stop_get_velocity = chrono::steady_clock::now();
    dur = stop_get_velocity - start_get_velocity;
    double time = dur.count();
    St4V = St4Vel
        {
            time, // Время от начала программы.
            time * time, // Время от начала программы в квадрате.
            t, // Тангаж.
            time * t, // Тангаж на время от начала программы.
            a, // Азимут.
            time * a, // Азимут на время от начала программы.
            k, // Крен.
            time * k // Крен на время от начала программы.
        }; // END St4V
    lSt4V.push_back(St4V);
    Sx  += St4V.x;
    Sxx += St4V.xx;
    St  += St4V.t;
    Sxt += St4V.xt;
    Sa  += St4V.a;
    Sxa += St4V.xa;
    Sk  += St4V.k;
    Sxk += St4V.xk;
    Zn_1 = 1.f / (num_mnk * Sxx - Sx * Sx);
    if(lSt4V.size() == num_mnk)
    {
        get_V(St, Sxt, Vt);
        get_V(Sa, Sxa, Va);
        get_V(Sk, Sxk, Vk);
        St4Vel& St4V0 = lSt4V.front();
        Sx  -= St4V0.x;
        Sxx -= St4V0.xx;
        St  -= St4V0.t;
        Sxt -= St4V0.xt;
        Sa  -= St4V0.a;
        Sxa -= St4V0.xa;
        Sk  -= St4V0.k;
        Sxk -= St4V0.xk;
        lSt4V.pop_front();
    } // END if(lSt4V.size() == N_mnk)
    else{Vt = 0; Va = 0; Vk = 0;}
} // END get_vel

Point Application::transform_vec(Point src0, Point src1, Point src2, Point dst0, Point dst1, Point dst2, Point vec)
{
    Point2f src[3];
    src[0] = src0;
    src[1] = src1;
    src[2] = src2;

    Point2f transf[3];
    transf[0] = dst0;
    transf[1] = dst1;
    transf[2] = dst2;

    Mat warp_matr = getAffineTransform(src, transf);
    Point vec_trans;
    double M00 = warp_matr.at<double>(0, 0);
    double M10 = warp_matr.at<double>(1, 0);
    double M01 = warp_matr.at<double>(0, 1);
    double M11 = warp_matr.at<double>(1, 1);
    double M02 = warp_matr.at<double>(0, 2);
    double M12 = warp_matr.at<double>(1, 2);
    vec_trans.x = round(vec.x * M00 + vec.y * M01 + M02);
    vec_trans.y = round(vec.x * M10 + vec.y * M11 + M12);

    return vec_trans;
} // END transform_vec()
#endif USE_I2C_DMP
