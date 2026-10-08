#include "application.hpp"
#include <RS232_worker.hpp>
using namespace  std;

bool RS232_worker::isCMD()
{
    if( cmdBuf[0] == to_goen_cmd.sync_code1 && cmdBuf[1] == to_goen_cmd.sync_code2) { return true;}
    return false;
} // END isCMD()

bool RS232_worker::isTelemetry()
{
    if(cmdBuf[0] == from_goen_tlm.sync_code1 && cmdBuf[1] == from_goen_tlm.sync_code1) { return true;}
    return false;
} // END isTelemetry()

int RS232_worker::find_cmd()
{
    if(find_cmd_STOP())                         {return (int)COMMAND_RS232::STOP;}
    else if(find_cmd_LEFT())                    {return (int)COMMAND_RS232::LEFT;}
    else if(find_cmd_RIGHT())                   {return (int)COMMAND_RS232::RIGHT;}
    else if(find_cmd_UP())                      {return (int)COMMAND_RS232::UP;}
    else if(find_cmd_DOWN())                    {return (int)COMMAND_RS232::DOWN;}
    else if(find_cmd_INFRA())                   {return (int)COMMAND_RS232::INFRA;}
    else if(find_cmd_TV())                      {return (int)COMMAND_RS232::TV;}
    else if(find_cmd_ZOOM_MINUS())              {return (int)COMMAND_RS232::ZOOM_MINUS;}
    else if(find_cmd_ZOOM_PLUS())               {return (int)COMMAND_RS232::ZOOM_PLUS;}
    else if(find_cmd_TO_CENTRE())               {return (int)COMMAND_RS232::TO_ZERO_POSITION;}
    else if(find_cmd_TRACKING_START())          {return (int)COMMAND_RS232::TRACKING_START;}
    else if(find_cmd_TRACKING_STOP())           {return (int)COMMAND_RS232::TRACKING_STOP;}
    else if(find_cmd_AZIMUTH_FOLLOW())          {return (int)COMMAND_RS232::AZIMUTH_FOLLOW;}
    else if(find_cmd_CLOSE_FOLLOW())            {return (int)COMMAND_RS232::CLOSE_FOLLOW;}
    else if(find_cmd_ELECTRIC_LOCK_ON())        {return (int)COMMAND_RS232::ELECTRIC_LOCK_ON;}
    else if(find_cmd_ELECTRIC_LOCK_OFF())       {return (int)COMMAND_RS232::ELECTRIC_LOCK_OFF;}
    else if(find_cmd_MOTOR_ON())                {return (int)COMMAND_RS232::MOTOR_ON;}
    else if(find_cmd_MOTOR_OFF())               {return (int)COMMAND_RS232::MOTOR_OFF;}
    else if(find_cmd_SET_ZERO_POSITION())       {return (int)COMMAND_RS232::SET_ZERO_POSITION;}
    else if(find_cmd_SET_FRAME_ANGLE())         {return (int)COMMAND_RS232::SET_AZIMUT_PITCH;}
    else if(find_cmd_POWER_OFF())               {return (int)COMMAND_RS232::POWER_OFF_1;}
    else if(find_cmd_CALIBRATE_DRIFT())         {return (int)COMMAND_RS232::CALIBRATE_DRIFT;}
    else if(find_cmd_CALIBRATE_I2C_ZERO())      {return (int)COMMAND_RS232::CALIBRATE_I2C_ZERO;}
    else if(find_cmd_CAMERA_CONTROL())          {return (int)COMMAND_RS232::CALIBRATE_DRIFT;}
    else if(find_cmd_SPECIFI_ATTITUDE_ANGLE())  {return (int)COMMAND_RS232::CALIBRATE_I2C_ZERO;}
    else if(find_cmd_CALIB_ZERO_POS_FC_ATT())   {return (int)COMMAND_RS232::CALIBRATE_DRIFT;}
#ifdef USE_TLM_MODE_CHANGER
    else if(find_cmd_TLM_SENDER_MODE_SWITCH())  {return (int)COMMAND_RS232::TLM_SENDER_MODE_SWITCH;}
#endif // USE_TLM_MODE_CHANGER
    else if(find_cmd_START_ATTACK())            {return (int)COMMAND_RS232::START_ATTACK;}
    else if(find_cmd_STOP_ATTACK())            {return (int)COMMAND_RS232::STOP_ATTACK;}

    return (int)COMMAND_RS232::notCMD;
} // -- END find_cmd()

void RS232_worker::clearCMD_buff()
{
    for (int i = 0; i< len_CMD_from_Board; ++i)
    {
        cmdBuf[i] = 0;
    } // END for (int i = 0; i< len_CMD_from_Board; ++i)
}// END clearCMD_buff()

bool RS232_worker::find_cmd_STOP()
{
    if ((cmdBuf[2] == (int)COMMAND_RS232::PTZ) && (cmdBuf[3] == 0x00) && (cmdBuf[4] == 0x00) && (cmdBuf[5] == 0x00) && (cmdBuf[6] == 0x00))
    {
        cout<<"STOP\n";
        return true;
    }//END if ((cmdBuf[2] == (int)COMMAND_RS232::PTZ) && (cmdBuf[3] == 0x00) && (cmdBuf[4] == 0x00) && (cmdBuf[5] == 0x00) && (cmdBuf[6] == 0x00))
    return false;
} // -- END find_cmd_STOP()

bool RS232_worker::find_cmd_LEFT()
{
    if (cmdBuf[2] == (int)COMMAND_RS232::PTZ)
    {
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        if (parametr_X.buf16 < 0)
        {
            printf("parametr_X = %d\n", parametr_X.buf16);
            cout<<"LEFT"<<endl;
            return true;
        } // END if (parametr_X.buf16 < 0)
    }//END if (cmdBuf[2] == (int)COMMAND_RS232::PTZ)
    return false;
} // -- END find_cmd_LEFT()

bool RS232_worker::find_cmd_RIGHT()
{
    if (cmdBuf[2] == (int)COMMAND_RS232::PTZ)
    {
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        if (parametr_X.buf16 > 0)
        {
            printf("parametr_X = %d\n", parametr_X.buf16);
            cout<<"RIGHT\n";
            return true;
        } // END if (parametr_X.buf16 > 0)
    } //END if (cmdBuf[2] == (int)COMMAND_RS232::PTZ)
    return false;
} // -- END find_cmd_RIGHT()

bool RS232_worker::find_cmd_UP()
{
    if (cmdBuf[2] == (int)COMMAND_RS232::PTZ)
    {
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        if (parametr_Y.buf16 > 0)
        {
            printf("parametr_Y = %d\n", parametr_Y.buf16);
            cout<<"UP\n";
            return true;
        } // END if (parametr_Y.buf16 > 0)
    }//END if (cmdBuf[2] == (int)COMMAND_RS232::PTZ)
    return false;
} // -- END find_cmd_UP()

bool RS232_worker::find_cmd_DOWN()
{
    if (cmdBuf[2] == (int)COMMAND_RS232::PTZ)
    {
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        if (parametr_Y.buf16 < 0)
        {
            printf("parametr_Y = %d\n", parametr_Y.buf16);
            cout<<"DOWN\n";
            return true;
        } // END if (parametr_Y.buf16 < 0)
    }//END if (cmdBuf[2] == (int)COMMAND_RS232::PTZ)
    return false;
} // -- END find_cmd_DOWN()

bool RS232_worker::find_cmd_INFRA()
{
    if (cmdBuf[2] == (int)COMMAND_RS232::INFRA)
    {
        cout<<"INFRA\n";
        return true;
    }//END if (cmdBuf[2] == (int)COMMAND_RS232::INFRA)
    return false;
} // -- END find_cmd_INFRA()

bool RS232_worker::find_cmd_TV()
{
    if (cmdBuf[2] == (int)COMMAND_RS232::TV)
    {
        cout<<"TV\n";
        return true;
    }//END if (cmdBuf[2] == (int)COMMAND_RS232::TV)
    return false;
} // -- END find_cmd_TV()

bool RS232_worker::find_cmd_ZOOM_MINUS()
{
    if (cmdBuf[2] == (int)COMMAND_RS232::ZOOM && cmdBuf[8] > zoom_min_value )
    {
        cout<<"ZOOM_MINUS\n";
        return true;
    }//END if (cmdBuf[2] == (int)COMMAND_RS232::ZOOM && cmdBuf[8] > zoom_min_value )
    return false;
} // -- END find_cmd_ZOOM_MINUS()

bool RS232_worker::find_cmd_ZOOM_PLUS()
{
    if (cmdBuf[2] == (int)COMMAND_RS232::ZOOM && cmdBuf[8] > 0 )
    {
        cout<<"ZOOM_PLUS\n";
        return true;
    }//END if (cmdBuf[2] == (int)COMMAND_RS232::ZOOM && cmdBuf[8] > 0 )
    return false;
} // -- END find_cmd_ZOOM_PLUS()

bool RS232_worker::find_cmd_TO_CENTRE()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::TO_ZERO_POSITION)
    {
        cout<<"TO_ZERO_POSITION\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::TO_ZERO_POSITION)
    return false;
} // -- END find_cmd_TO_CENTRE()

bool RS232_worker::find_cmd_TRACKING_START()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::TRACKING_START)
    {
        cout<<"TRACKING_START\n";
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        printf("X: %d\n", parametr_X.buf16);
        printf("Y: %d\n", parametr_Y.buf16);
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::TRACKING_START)
    return false;
} // -- END find_cmd_TRACKING_START()

bool RS232_worker::find_cmd_TRACKING_STOP()
{
    if ((cmdBuf[2] == (uint8_t)COMMAND_RS232::TRACKING_STOP) && (cmdBuf[3] == 0x00) && (cmdBuf[4] == 0x00) && (cmdBuf[5] == 0x00) && (cmdBuf[6] == 0x00))
    {
        cout<<"TRACKING_STOP\n";
        return true;
    }//END if ((cmdBuf[2] == (uint8_t)COMMAND_RS232::TRACKING_STOP) && (cmdBuf[3] == 0x00) && (cmdBuf[4] == 0x00) && (cmdBuf[5] == 0x00) && (cmdBuf[6] == 0x00))
    return false;
}// END bool RS232_worker::find_cmd_TRACKING_STOP()

bool RS232_worker::find_cmd_AZIMUTH_FOLLOW()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::AZIMUTH_FOLLOW)
    {
        cout << "AZIMUTH FOLLOW\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::AZIMUTH_FOLLOW)
    return false;
} // -- END find_cmd_AZIMUTH_FOLLOW()

bool RS232_worker::find_cmd_CLOSE_FOLLOW()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CLOSE_FOLLOW)
    {
        cout << "CLOSE_FOLLOW\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CLOSE_FOLLOW)
    return false;
} // -- END find_cmd_CLOSE_FOLLOW()

bool RS232_worker::find_cmd_ELECTRIC_LOCK_ON()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::ELECTRIC_LOCK_ON)
    {
        cout << "ELECTRIC_LOCK_ON\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::ELECTRIC_LOCK_ON)
    return false;
} // -- END find_cmd_ELECTRONIC_LOCK_ON()

bool RS232_worker::find_cmd_ELECTRIC_LOCK_OFF()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::ELECTRIC_LOCK_OFF)
    {
        cout << "ELECTRIC_LOCK_OFF\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::ELECTRIC_LOCK_OFF)
    return false;
} // -- END find_cmd_ELECTRONIC_LOCK_OFF()

bool RS232_worker::find_cmd_MOTOR_ON()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::MOTOR_ON)
    {
        cout<<"MOTOR_ON\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::MOTOR_ON)
    return false;
} // -- END find_cmd_MOTOR_ON()

bool RS232_worker::find_cmd_MOTOR_OFF()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::MOTOR_OFF)
    {
        cout<<"MOTOR_OFF\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::MOTOR_OFF)
    return false;
} // -- END find_cmd_MOTOR_OFF()

bool RS232_worker::find_cmd_SET_ZERO_POSITION()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::SET_ZERO_POSITION)
    {
        cout<<"SET_ZERO_POSITION\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::SET_ZERO_POSITION)
    return false;
} // -- END find_cmd_SET_ZERO_POSITION()

bool RS232_worker::find_cmd_SET_FRAME_ANGLE()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::SET_AZIMUT_PITCH)
    {
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        printf("X: %d\n", parametr_X.buf16);
        printf("Y: %d\n", parametr_Y.buf16);
        cout<<"SET_FRAME_ANGLE\n";
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::SET_AZIMUT_PITCH)
    return false;
} // -- END find_cmd_SET_FRAME_ANGLE()

bool RS232_worker::find_cmd_POWER_OFF()
{
    if ((cmdBuf[2] == (uint8_t)COMMAND_RS232::POWER_OFF_1) && (cmdBuf[3] == (uint8_t)COMMAND_RS232::POWER_OFF_2) && (cmdBuf[4] == (uint8_t)COMMAND_RS232::POWER_OFF_3))
    {
        cout << "find_cmd_POWER_OFF\n";
        return true;
    }//END if ((cmdBuf[2] == (uint8_t)COMMAND_RS232::POWER_OFF_1) && (cmdBuf[3] == (uint8_t)COMMAND_RS232::POWER_OFF_2) && (cmdBuf[4] == (uint8_t)COMMAND_RS232::POWER_OFF_3))
    return false;
} // -- END find_cmd_POWER_OFF()

bool RS232_worker::find_cmd_CALIBRATE_DRIFT()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CALIBRATE_DRIFT)
    {
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        calibrate_drift_parameter_X = parametr_X.buf16;
        calibrate_drift_parameter_Y = parametr_Y.buf16;
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CALIBRATE_DRIFT)
    return false;
} // -- END find_cmd_CALIBRATE_DRIFT()

bool RS232_worker::find_cmd_CALIBRATE_I2C_ZERO()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CALIBRATE_I2C_ZERO)
    {
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        calibrate_i2c_zero_parameter_roll = parametr_X.buf16;
        calibrate_i2c_zero_parameter_pitch = parametr_Y.buf16;
        cout << "buf[7,8]" << hex << (int)cmdBuf[7] << ", " << (int)cmdBuf[8] << dec << endl;
        uint8_t buf[2] = {cmdBuf[8], cmdBuf[7]} ;
        memcpy(&calibrate_i2c_zero_parameter_yaw, buf, sizeof(calibrate_i2c_zero_parameter_yaw));
        cout << "Calib params yaw, pitch, roll = " << cv::Point3i((int)calibrate_i2c_zero_parameter_yaw, (int)calibrate_i2c_zero_parameter_pitch, (int)calibrate_i2c_zero_parameter_roll) << endl;
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CALIBRATE_I2C_ZERO)
    return false;
} // -- END find_cmd_CALIBRATE_I2C_ZERO()

bool RS232_worker::find_cmd_CAMERA_CONTROL()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CAMERA_CONTROL)
    {
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CAMERA_CONTROL)
    return false;
} // -- END find_cmd_CAMERA_CONTROL()

bool RS232_worker::find_cmd_SPECIFI_ATTITUDE_ANGLE()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::SPECIFI_ATTITUDE_ANGLE)
    {
        cout << "x: " << (int)cmdBuf[3] << (int)cmdBuf[4] << endl;
        cout << "y: " << (int)cmdBuf[5] << (int)cmdBuf[6] << endl;
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::SPECIFI_ATTITUDE_ANGLE)
    return false;
} // -- END find_cmd_SPECIFI_ATTITUDE_ANGLE()

bool RS232_worker::find_cmd_CALIB_ZERO_POS_FC_ATT()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CALIB_ZERO_POS_FC_ATT)
    {
        cout << "x: " << (int)cmdBuf[3] << (int)cmdBuf[4] << endl;
        cout << "y: " << (int)cmdBuf[5] << (int)cmdBuf[6] << endl;
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::CALIB_ZERO_POS_FC_ATT)
    return false;
} // -- END find_cmd_CALIB_ZERO_POS_FC_ATT()


bool RS232_worker::find_cmd_TLM_SENDER_MODE_SWITCH()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::TLM_SENDER_MODE_SWITCH)
    {
        parametr_X.buf8[0] = cmdBuf[3];
        parametr_X.buf8[1] = cmdBuf[4];
        parametr_Y.buf8[0] = cmdBuf[5];
        parametr_Y.buf8[1] = cmdBuf[6];
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::TLM_SENDER_MODE_SWITCH)
    return false;
} // -- END find_cmd_TLM_SENDER_MODE_SWITCH()

bool RS232_worker::find_cmd_START_ATTACK()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::START_ATTACK)
    {
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::START_ATTACK)
    return false;
} // -- END find_cmd_START_ATTACK()

bool RS232_worker::find_cmd_STOP_ATTACK()
{
    if (cmdBuf[2] == (uint8_t)COMMAND_RS232::STOP_ATTACK)
    {
        return true;
    }//END if (cmdBuf[2] == (uint8_t)COMMAND_RS232::STOP_ATTACK)
    return false;
} // -- END find_cmd_STOP_ATTACK()


