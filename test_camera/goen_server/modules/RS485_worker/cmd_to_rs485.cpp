#include "RS485_worker.hpp"
#include <iostream>
#include <thread>

using namespace std;

void RS485_worker::get_status_Goen()
{
    clearCommandPacket();
    commandPacket.CMD = (uint8_t)REQUEST_TYPE::read;
    commandPacket.DST.device = (uint8_t)DEVICES::GOEN;
    commandPacket.DST.function = (uint8_t)COMMAND_TO_RS485::GET_STATUS;
    writeToRS485();
} // -- END get_status_Goen()

void RS485_worker::get_version_Goen()
{
    clearCommandPacket();
    commandPacket.CMD = (uint8_t)REQUEST_TYPE::read;
    commandPacket.DST.device = (uint8_t)DEVICES::GOEN;
    commandPacket.DST.function = (uint8_t)COMMAND_TO_RS485::GET_VERSION;
    writeToRS485();
} // -- END get_version_Goen()

void RS485_worker::set_mode_Goen(uint8_t newMode)
{
    clearCommandPacket();
    cout << "======  mode_Goen send to RS485  ======> ";
    if(newMode == (uint8_t)MODE_TYPE::ROTARY_PLATFORM)
    {
        cout << "ROTARY_PLATFORM\n";
    } // END if (newMode = (uint8_t)MODE_TYPE::ROTARY_PLATFORM)
    else if(newMode == (uint8_t)MODE_TYPE::STABILISATION)
    {
        cout << "STABILISATION\n";
    } // END if(newMode = (uint8_t)MODE_TYPE::STABILISATION)
    else
    {
        cout << "MOTOR OFF\n";
    } // END else
    commandPacket.CMD = (uint8_t)REQUEST_TYPE::write;
    commandPacket.DST.device = (uint8_t)DEVICES::GOEN;
    commandPacket.DST.function = (uint8_t)COMMAND_TO_RS485::MODE;
    commandPacket.BYTE_COUNT.byteCount16 = 0x0001;
    commandPacket.DATA.data_1byte = newMode;
    writeToRS485();
} // -- END set_mode_Goen

void RS485_worker::set_mode_Stab(uint8_t newMode)
{
    clearCommandPacket();
    cout << "======  set_mode_Stab send to RS485  ======> ";
    if(newMode == (uint8_t)MODE_TYPE::ROTARY_PLATFORM)
    {
        cout << "ROTARY_PLATFORM\n";
    } // END if (newMode = (uint8_t)MODE_TYPE::ROTARY_PLATFORM)
    else
    {
        cout << "STABILISATION\n";
    } // END if(newMode = (uint8_t)MODE_TYPE::STABILISATION)
    commandPacket.CMD = (uint8_t)REQUEST_TYPE::write;
    commandPacket.DST.device = (uint8_t)DEVICES::GOEN;
    commandPacket.DST.function = (uint8_t)COMMAND_TO_RS485::STAB_MODE;
    commandPacket.BYTE_COUNT.byteCount16 = 0x0001;
    commandPacket.DATA.data_1byte = newMode;
    writeToRS485();
} // -- END set_mode_Stab()

void RS485_worker::control_speed_Goen(int16_t X, int16_t Y)
{
    if(f_motor_lock.load())
    {
        // cout << "MOTOR LOCK! CAN'T SEND control_speed_Goen CMD" << endl;
        return;
    } // END if(f_motor_lock.load())
    clearCommandPacket();
    // cout<<"===================  control_speed_Goen send to RS485 " << cv::Point(X, Y) << " ========================" <<endl;
    commandPacket.CMD = (uint8_t)REQUEST_TYPE::write;
    commandPacket.DST.device = (uint8_t)DEVICES::GOEN;
    commandPacket.DST.function = (uint8_t)COMMAND_TO_RS485::CONTROL_SPEED;
    commandPacket.BYTE_COUNT.byteCount16 = 8;
    commandPacket.DATA_X.x = -k_speed_control * (X + Vx_dreyf); /// DBG::
    commandPacket.DATA_Y.y = k_speed_control * (Y + Vy_dreyf);
    writeToRS485();
} // -- END control_speed_Goen()

void RS485_worker::control_position_Goen(int16_t X, int16_t Y)
{
    // if(f_motor_lock.load()) {cout << "MOTOR LOCK! CAN'T SEND control_speed_Goen CMD" << endl; return;}
    clearCommandPacket();
    cout<<"===================  control_position_Goen send to RS485 " << cv::Point(X, Y) << " ========================" <<endl;
    commandPacket.CMD = (uint8_t)REQUEST_TYPE::write;
    commandPacket.DST.device = (uint8_t)DEVICES::GOEN;
    commandPacket.DST.function = (uint8_t)COMMAND_TO_RS485::CONTROL_POSITION;
    commandPacket.BYTE_COUNT.byteCount16 = 8;
    if(f_motor_lock)
    {
        commandPacket.DATA_X.x = -((float)X * 0.01 - ang0_x);
        commandPacket.DATA_Y.y = (float)Y * 0.01 + ang0_y;
    } // END if(f_motor_lock)
    else
    {
        commandPacket.DATA_X.x = -((float)X * 0.01 - ang0_x + pos_azimuth_err);
        commandPacket.DATA_Y.y = (float)Y * 0.01 + ang0_y + pos_pitch_err;
    } // END if(!f_motor_lock)
    cout << "commandPacket.DATA_X = " << cv::Point2f(commandPacket.DATA_X.x, commandPacket.DATA_Y.y) << endl;
    writeToRS485();
} // -- END control_position_Goen()

void RS485_worker::control_tracking_Goen_self(float X, float Y)
{
    int k = 4000;
    int maxSpeed = 70;
    float deltaX_speed = X * k ;
    float deltaY_speed = Y * k ;
    if (deltaX_speed > maxSpeed) deltaX_speed = maxSpeed;
    if (deltaY_speed > maxSpeed) deltaY_speed = maxSpeed;
    control_speed_Goen(deltaX_speed, deltaY_speed );
} // -- END control_tracking_Goen_self

void RS485_worker::control_tracking_Goen(float X, float Y, float ax_max, float ay_max)
{
    clearCommandPacket();
    cout << "===================  control_tracking_Goen send to RS485  ========================" << endl;
    commandPacket.CMD = (uint8_t)REQUEST_TYPE::write;
    commandPacket.DST.device = (uint8_t)DEVICES::GOEN;
    commandPacket.DST.function = (uint8_t)COMMAND_TO_RS485::CONTROL_TRACKING;
    commandPacket.BYTE_COUNT.byteCount16 = 16;
    commandPacket.DATA_X.x = X;
    commandPacket.DATA_Y.y = Y;
    commandPacket.AX_MAX.ax_32 = ax_max;
    commandPacket.AY_MAX.ay_32 = ay_max;
    writeToRS485();
} // -- END control_tracking_Goen()

void RS485_worker::check_need_correct()
{
    if(is_correcting_pozition_in_mode_azimut_follow)
    {
        is_correcting_pozition_in_mode_azimut_follow = false;
        need_return_to_correct_position = true;
    }//END if(is_correcting_pozition_in_mode_azimut_follow)
} // -- END check_need_correct()

void RS485_worker::stop()
{
    clearCommandPacket();
    // cout << "===================  send STOP to RS485  ========================" << endl;
    if(f_azimuth_follow)
    {
        control_position_Goen(-round((from_goen_telemetry_str.motor_yaw.angle - ang0_x) * 100), round((from_goen_telemetry_str.motor_pitch.angle - ang0_y) * 100));
    } // END if(f_azimuth_follow)
    else
    {
        control_speed_Goen(0,0);
    } // END if(!f_azimuth_follow)
} // -- END stop()

void RS485_worker::set_azimut_pitch(int16_t X, int16_t Y)
{
    clearCommandPacket();
    cout << "===================  set_azimut_pitch send to RS485 " << cv::Point(X, Y) << "  ========================" << endl;
    control_position_Goen(X,Y);
} // -- END set_azimut_pitch()

void RS485_worker::left_right(int16_t X, int16_t Y)
{
    clearCommandPacket();
    cout << "===================  left_right send to RS485  ========================" << endl;
    control_speed_Goen(X, Y);
} // -- END left_right(int16_t X, int16_t Y)

void RS485_worker::up_down(int16_t X, int16_t Y)
{
    clearCommandPacket();
    cout << "===================  up_down send to RS485  ========================" << endl;
    control_speed_Goen(X, Y);
} // -- END up_down(int16_t X, int16_t Y)

void RS485_worker::to_zero()
{
    clearCommandPacket();
    cout << "===================  to_zero send to RS485  ========================" << endl;
    control_position_Goen(0,0);
} // -- END to_zero()

void RS485_worker::tracking_start()
{
    is_tracking_start = true;
} // -- END tracking_start()

void RS485_worker::tracking_stop()
{
    if(is_tracking_start)
    {
        cout << "================tracking_stop()===================" << endl;
        is_tracking_start = false;
        is_tracking_init_after_start = false;
        stop();
    }//END if(is_tracking_start)
} // -- END tracking_stop()

void RS485_worker::send_read_request()
{
    clearCommandPacket();
    commandPacket.CMD = 0x01;
    cout<<"sending read request to port " << namePort << endl;
    writeToRS485();
} // -- END send_read_request()

void RS485_worker::tracking(int16_t X, int16_t Y)
{
    clearCommandPacket();
    this_thread::sleep_for(chrono::milliseconds(10));
    clearCommandPacket();
    writeToRS485();
} // -- END tracking(int16_t X, int16_t Y)

