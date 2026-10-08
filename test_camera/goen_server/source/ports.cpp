#include "application.hpp"
using namespace  std;
using namespace LibSerial;
using namespace chrono;

void Application::init_ports(const string& config)
{
    set_ports();

    if (common_data_ptr->is_init_all_ports())
    {
        INIReader reader(config);

#if defined(USE_TV)
        rs485_worker_ptr->Vmin = reader.GetReal("tracking", "Vmin_TV", -1);
        if(rs485_worker_ptr->Vmin == -1) {cout << "Not found Vmin_TV \n"; }
        else {cout << "Vmin_TV = " << rs485_worker_ptr->Vmin << endl;}

        rs485_worker_ptr->Vmax = reader.GetReal("tracking", "Vmax_TV", -1);
        if(rs485_worker_ptr->Vmax == -1) {cout << "Not found Vmax_TV \n"; }
        else {cout << "Vmax_TV = " << rs485_worker_ptr->Vmax << endl;}

#endif // END #if defined(USE_TV)

#if defined(USE_TPV_cam)
        rs485_worker_ptr->Vmin = reader.GetReal("tracking", "Vmin_TPV", -1);
        if(rs485_worker_ptr->Vmin == -1) {cout << "Not found Vmin_TPV \n"; }
        else {cout << "Vmin_TPV = " << rs485_worker_ptr->Vmin << endl;}

        rs485_worker_ptr->Vmax = reader.GetReal("tracking", "Vmax_TPV", -1);
        if(rs485_worker_ptr->Vmax == -1) {cout << "Not found Vmax_TPV \n"; }
        else {cout << "Vmax_TPV = " << rs485_worker_ptr->Vmax << endl;}
#endif // END #if defined(USE_TPV_cam)

    } // END if (common_data_ptr->is_init_all_ports())
    else
    {
        cout << "\n==========================  port RS232 or RS485 not init !!!  ==========================\n\n";
    } // END if(!common_data_ptr->is_init_all_ports())

} // END init_ports()

void Application::get_system_result(const std::string& system_str, std::string& cmd_result)
{
    cmd_result = "";
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(system_str.c_str(), "r"), pclose);
    if(!pipe){throw std::runtime_error("popen() failed!");}
    while(fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr){cmd_result += buffer.data();}
} // END get_system_result

void Application::set_ports()
{
    cout<< "===============  method set USB ports = 0  ===================\n";
    cout << "USB_for_232: " << SERIAL_PORT_RS232 << endl;
    cout << "USB_for_485: " << SERIAL_PORT_RS485 << endl;
    rs232_worker_ptr = make_unique<RS232_worker>(rs232_speed, SERIAL_PORT_RS232);
    rs485_worker_ptr = make_unique<RS485_worker>(rs485_speed, SERIAL_PORT_RS485);
    rs485_worker_ptr->Vx_dreyf = Vx_dreyf0;
    rs485_worker_ptr->Vy_dreyf = Vy_dreyf0;
    cout << "-----------config_path=" << config_path << "----------------" << endl;
    cmd_udp_ptr = make_unique<CmdOverUdpReceiver>(config_path);
    if(rs232_worker_ptr->port_open_OK && rs485_worker_ptr->port_open_OK)
    {
        common_data_ptr->set_init_all_ports(true);
        thread thr(&Application::work_rs232_rs485, this);
        thread_rs232_485.swap(thr);
    } // END if (rs232_worker_ptr->port_open_OK && rs485_worker_ptr->port_open_OK)
    else if(rs232_worker_ptr->port_open_OK && !rs485_worker_ptr->port_open_OK)
    {
        cout << "===========================  485 damaged !!!!!!!!!!!!!!!  ==============================" <<endl;
        thread thr(&Application::work_only_rs232, this);
        thread_only_rs232.swap(thr);
    }// END else if (rs232_worker_ptr->port_open_OK && rs485_worker_ptr->port_open_OK)
} // END set_ports_by_0_method()

string Application::array_uint8_to_string(uint8_t * request, int num)
{
    std::ostringstream conv;
    for (int i = 0; i < num; ++i)
    {
        conv << request[i];
    } // END for (int i = 0; i < num; ++i)
    return conv.str();
} // END array_uint8_to_string(uint8_t * request, int num)

void Application::rs232_tlm_send_to_Board()
{
    if(f_need_send_tlm.load())
    {
        set_from_goen_telemetry();
        cmd_udp_ptr->send_telemetry(to_bort_tlm_rs232_str);
#ifdef USE_UDP_TLM
        // cout <<
        //     "\nvalid =" << tlm_ap_str.valid << endl <<       // byte 0-3
        //     "target_miss_x =" << tlm_ap_str.target_miss_x << endl <<       // byte 0-3
        //     "target_miss_y =" << tlm_ap_str.target_miss_y << endl <<       // byte 0-3
        //     "motor_azimuth =" << tlm_ap_str.motor_azimuth << endl <<       // byte 0-3
        //     "motor_pitch =" << tlm_ap_str.motor_pitch << endl <<       // byte 0-3
        //     "frame_roll =" << tlm_ap_str.frame_roll << endl <<       // byte 0-3
        //     "frame_pitch =" << tlm_ap_str.frame_pitch << endl <<       // byte 0-3
        //     "azimuth_angular_velocity =" << tlm_ap_str.azimuth_angular_velocity << endl <<       // byte 0-3
        //     "pitch_angular_velocity =" << tlm_ap_str.pitch_angular_velocity << endl <<       // byte 0-3
        //     "attack =" << tlm_ap_str.attack << endl << endl;      // byte 0-3
        int res_ap = cmd_udp_ptr->send_extension_tlm(tlm_ap_str);
        // cout << "Send extenstion res = " << res_ap << endl;
#endif // USE_UDP_TLM

#ifdef USE_SERIAL_PTR
        memcpy(&tlm_byte_buf, &to_bort_tlm_rs232_str, tlm_size);
        tlm_byte_buf[rs232_worker_ptr->_telemetryLenToBoard - 1] = rs232_worker_ptr->check_sum(tlm_byte_buf, rs232_worker_ptr->_telemetryLenToBoard - 1);
        DataBuffer tlm_buf;
        tlm_buf.resize(tlm_size);
        for(int i = 0; i < tlm_size; i++)
        {
            tlm_buf[i] = tlm_byte_buf[i];
        } // END for(int i = 0; i < ssr_buf.size(); i++)
        rs232_worker_ptr->serial_ptr->Write(tlm_buf);
        rs232_worker_ptr->serial_ptr->DrainWriteBuffer();
#endif // USE_SERIAL_PTR
        // this_thread::sleep_for(6ms);
        f_need_send_tlm.store(false);
    } // END if(f_need_send_tlm.load())
} // END rs232_tlm_send_to_Board

bool Application::send_single_status_return(uint8_t control_param)
{
#ifdef USE_SERIAL_PTR
    ssr_byte_buf[2] = control_param;
    ssr_byte_buf[4] = rs232_worker_ptr->check_sum(ssr_byte_buf, 4);
    DataBuffer ssr_buf;
    ssr_buf.resize(sizeof(ReplayToCmdToBort));
    for(int i = 0; i < ssr_buf.size(); i++)
    {
        ssr_buf[i] = ssr_byte_buf[i];
    } // END for(int i = 0; i < ssr_buf.size(); i++)
    cout << "Start write ssr" << endl;
    rs232_worker_ptr->serial_ptr->Write(ssr_buf);
    rs232_worker_ptr->serial_ptr->DrainWriteBuffer();
    cout << "Ok write ssr" << endl;
#endif
    return true;
} // END send_single_status_return

void Application::work_only_rs232()
{
    thread thr232;
    int i_dbg = 0;
    while(_execute.load(memory_order_acquire))
    {
        i_dbg++;
        cout << "dbg::Next [" << i_dbg << "]cycle! " << endl;
        try
        {
            //work with RS232 in single thread
            cout << "dbg::Call rs232_work" << endl;
            rs232_work(thr232);
            cout << "dbg::END rs232_work" << endl;
        } // END try
        catch (const exception & e) // Error in hardware 232
        {
            cout << "======== Port RS232 is NOT AVAILABLE !!! ========  " << e.what() << endl;
            common_data_ptr->set_error_rs232(true);
        } // END catch (const exception & e)
    } // END while(_execute.load(memory_order_acquire))
    if(thr232.joinable()){thr232.join();}
} // END work_only_rs232()


void Application::check_press_powerOFF()
{
    while(1)
    {
       string cmd = "journalctl | tail -n50 | grep 'Power key pressed'";
       string result = "";
       get_system_result(cmd, result);
       if(result != "")
       {
            cout << " =========== Power key pressed !!! =================" << endl;
            signal_flag = true;
            this_thread::sleep_for(5000ms);
            string cmd = "poweroff";
            int res = system(cmd.c_str());
            cout << "PowerOFF was called with result " << res << endl;
            break;
       } // END if(result != "")
       this_thread::sleep_for(500ms);
    } // END while(1)
} // END check_press_powerOFF()

void Application::work_rs232_rs485()
{
    this_thread::sleep_for(100ms);
    thread thr232;
    thread thr485;
    thread thr_request_status;

    while(_execute.load(memory_order_acquire))
    {
      try{
            if(common_data_ptr->is_need_quit())
            {
                this_thread::sleep_for(10ms);
                if(rs485_worker_ptr->is_tracking_start)
                {
                    rs485_worker_ptr->tracking_stop();
                } // END if(rs485_worker_ptr->is_tracking_start)
                if(thr232.joinable()){thr232.join();}
                if(thr_request_status.joinable()){thr_request_status.join();}
                if(thr485.joinable()){thr485.join();}
                break;
            } // END if(common_data_ptr->is_need_quit())

            if(common_data_ptr->is_error_rs485())
            {
                if(rs485_worker_ptr->is_tracking_start)
                {
                    rs485_worker_ptr->tracking_stop();
                } // END if(rs485_worker_ptr->is_tracking_start)
                if (thr_request_status.joinable()){thr_request_status.join();}
                if (thr485.joinable()){thr485.join();}
            } // END if(common_data_ptr->is_error_rs485())

            if(common_data_ptr->is_error_rs232())
            {
                if(rs485_worker_ptr->is_tracking_start)
                {
                    rs485_worker_ptr->tracking_stop();
                } // END if(rs485_worker_ptr->is_tracking_start)
                if(thr232.joinable()){thr232.join();}
            } // END if(common_data_ptr->is_error_rs232())

            // work with RS232 in single thread
            rs232_work(thr232);
            // work with RS485 in single thread
            rs485_work(thr485);
        } // END try
        catch (const exception & e) // Error in hardware 232
        {
            cout << "======== Port RS232 or RS485 is NOT AVAILABLE !!! ========  " << e.what() << endl;
            common_data_ptr->set_error_rs232(true);
            common_data_ptr->set_error_rs485(true);
        } // END catch

        this_thread::sleep_for(100ms);

    } // END while(_execute.load(memory_order_acquire))
} // END work_rs232_rs485()

void Application::get_status_goen()
{
    rs485_worker_ptr->setCMD_TO_485((uint8_t)COMMAND_TO_RS485::GET_STATUS);
    rs485_worker_ptr->get_status_Goen();
} // -- END get_status_goen()

void Application::get_version_goen()
{
    rs485_worker_ptr->setCMD_TO_485((uint8_t)COMMAND_TO_RS485::GET_VERSION);
    rs485_worker_ptr->get_version_Goen();
} // -- END get_status_goen()

void Application::set_mode_goen(uint8_t newMode)
{
    rs485_worker_ptr->setCMD_TO_485((uint8_t)COMMAND_TO_RS485::MODE);
    rs485_worker_ptr->set_mode_Goen(newMode);
} // -- END get_status_goen()

void Application::reset_goen()
{
    // rs485_worker_ptr->setCMD_TO_485((int)COMMAND_TO_RS485::CPU_RESET);
    this_thread::sleep_for(500ms);
    cout <<"------- send RESET to RS485 ---------\n";
} // -- END reset_goen()

void Application::rs232_work(thread & thrread_232)
{
    function fo_workRS232 = [&]()
    {
        rs232_worker_ptr->is_workerRS232_start = true;
        cmd_udp_ptr->start();
//        cout << "dbg::start cmd_udp_ptr" << endl;
        while(!common_data_ptr->is_error_rs232())
        {
            // check CMD receiver from UDP
            if(cmd_udp_ptr->receivedCMD)
            {
                printf("\nreceived from udp :  ");
                for(int i = 0; i < rs232_worker_ptr->len_CMD_from_Board; ++i)
                {
                    printf( "%02X ", cmd_udp_ptr->buf[i]);
                    rs232_worker_ptr->cmdBuf[i] = cmd_udp_ptr->buf[i];
                } // END for (int i = 0; i < rs232_worker_ptr->len_CMD_from_Board; ++i)
                printf("\n");

                cmd_udp_ptr->receivedCMD = false;
                this_thread::sleep_for(50ms);

                if(rs232_worker_ptr->isCMD())
                {
                    // rs232_worker_ptr->printDataRS232(TYPE_DATA::CMD);
                    rs232_worker_ptr->data_from_rs232.clear();
                    memcpy(&from_bort_cmd_rs232_str, rs232_worker_ptr->cmdBuf, rs232_worker_ptr->len_CMD_from_Board);

                    receivedCMD = rs232_worker_ptr->find_cmd();
                    if(rs232_worker_ptr->check_check_sum())
                    {
                        // cout<<"check_sum_CMD OK"<<endl;
                        rs232_worker_ptr->setCMD(receivedCMD);



                        rs232_worker_ptr->setCMD(receivedCMD);
                        receivedCMD = from_bort_cmd_rs232_str.control_param;
                        // Send confirm CMD to BOARD
//                        send_single_status_return(from_bort_cmd_rs232_str.control_param);
                        // Set flag to resend CMD
                        rs232_worker_ptr->need_send_to_RS485 = true;
                        rs485_worker_ptr->num_send = 0;



                        if(receivedCMD == (int)COMMAND_RS232::CALIBRATE_DRIFT)
                        {
                            cout << "==== RECEIVED from UDP CALIBRATE_DRIFT ====\n";
                            calibrateDrift();
                        } // END if(receivedCMD == (int)COMMAND_RS232::CALIBRATE_DRIFT)

                        if(receivedCMD == (int)COMMAND_RS232::CALIBRATE_I2C_ZERO)
                        {
                            cout << "==== RECEIVED from RS232 CALIBRATE_I2C_ZERO ====\n";
#ifdef USE_I2C_DMP
                            if(rs232_worker_ptr->calibrate_i2c_zero_parameter_yaw == rs232_worker_ptr->calibrate_i2c_zero_parameter_auto &&
                                rs232_worker_ptr->calibrate_i2c_zero_parameter_pitch == rs232_worker_ptr->calibrate_i2c_zero_parameter_auto &&
                                rs232_worker_ptr->calibrate_i2c_zero_parameter_roll == rs232_worker_ptr->calibrate_i2c_zero_parameter_auto)
                            {
                            mpu6050_dmp_ptr->calib_zero_auto();
                            } // END  if(!rs232_worker_ptr->calibrate_i2c_zero_parameter_yaw && ..
                            else
                            {
                                mpu6050_dmp_ptr->calib_zero_handle(rs232_worker_ptr->calibrate_i2c_zero_parameter_yaw * 0.01,
                                                                   rs232_worker_ptr->calibrate_i2c_zero_parameter_pitch * 0.01,
                                                                   rs232_worker_ptr->calibrate_i2c_zero_parameter_roll * 0.01);
                            } // END else  if(!rs232_worker_ptr->calibrate_i2c_zero_parameter_yaw && ..
#endif // USE_I2C_DMP
                        } // END if(receivedCMD == (int)COMMAND_RS232::CALIBRATE_I2C_ZERO)

                        if(receivedCMD == (int)COMMAND_RS232::POWER_OFF_1)
                        {
                            cout << "==== RECEIVED from UDP CMD POWER_OFF ====\n";
                            rs485_worker_ptr->set_mode_Goen((uint8_t)MODE_TYPE::MOTOR_OFF);
                            this_thread::sleep_for(50ms);
                            rs485_worker_ptr->set_mode_Goen((uint8_t)MODE_TYPE::MOTOR_OFF);
                            this_thread::sleep_for(200ms);
                            device->quit();
                            this_thread::sleep_for(200ms);
                            cout << "====================== POWER OFF ===================="<<endl;

                            string cmd = "systemctl poweroff";
                            int res = system(cmd.c_str());
                            cout << "PowerOFF was called with result " << res << endl;

                            // signal_flag = true;
                            // this_thread::sleep_for(3000ms);
                            // powerOFF_flag = true;
                            break;
                        } // END if(receivedCMD == (int)COMMAND_RS232::POWER_OFF)

                        receivedCMD = from_bort_cmd_rs232_str.control_param;

                        //Send confirm CMD to BOARD
                        send_single_status_return(from_bort_cmd_rs232_str.control_param);
#ifdef USE_CONFIRMATION
                        cmd_udp_ptr->send_single_status_return_udp(from_bort_cmd_rs232_str.control_param);
#endif
                        //Set flag to resend CMD
                        rs232_worker_ptr->need_send_to_RS485 = true;
                        rs485_worker_ptr->num_send = 0;
                    } // END if (rs232_worker_ptr->check_check_sum())
                    else
                    {
                        cout << "check_sum ERROR!" << endl;
                    } // END else
                } // END if(rs232_worker_ptr->isCMD())
            } // END if(cmd_udp_ptr->receivedCMD)
            try
            {
        //Send CMD Tracking stop
                if(rs485_worker_ptr->is_Tracking_escape)
                {
                    rs485_worker_ptr->is_Tracking_escape = false;
                    receivedCMD = (int)COMMAND_RS232::TRACKING_STOP;
                    //cout << "receivedCMD = (int)COMMAND_RS232::TRACKING_STOP;\n";
                    rs232_worker_ptr->setCMD(receivedCMD);
                    rs232_worker_ptr->need_send_to_RS485 = true;
                    rs485_worker_ptr->num_send = 0;
                } // if(rs485_worker_ptr->is_Tracking_escape)

                if(common_data_ptr->is_need_quit()){break;}
#ifdef USE_SERIAL_PTR
                if(rs232_worker_ptr->serial_ptr->IsDataAvailable())
                {
        //Receive CMD
                    this_thread::sleep_for(2ms); // time receive 16 bytes  - 1,2 ms
                    if(rs232_worker_ptr->serial_ptr->GetNumberOfBytesAvailable() == rs232_worker_ptr->len_CMD_from_Board)
                    {
                        rs232_worker_ptr->serial_ptr->Read(rs232_worker_ptr->data_from_rs232, rs232_worker_ptr->len_CMD_from_Board);

#ifdef USE_LOG_CMD
                        stringstream log_strstream;
#endif // USE_LOG_CMD

                        for(int i = 0; i < rs232_worker_ptr->len_CMD_from_Board; ++i)
                        {
                            rs232_worker_ptr->cmdBuf[i] = rs232_worker_ptr->data_from_rs232[i];
#ifdef USE_LOG_CMD
                            log_strstream << hex << " " << uppercase << (int)rs232_worker_ptr->data_from_rs232[i] << dec;
#endif // USE_LOG_CMD
                        } // END for(int i = 0; i < rs232_worker_ptr->_cmdLen; ++i)

#ifdef USE_LOG_CMD
                        string str2log = log_strstream.str();
                        cout << "Call log " <<  str2log << endl;
                        // cout << "Call log " <<  rs232_worker_ptr->data_from_rs232 << endl;
                        log_cmd_ptr->log_(str2log);
#endif // USE_LOG_CMD

                        if(rs232_worker_ptr->isCMD())
                        {
                            rs232_worker_ptr->printDataRS232(TYPE_DATA::CMD);
                            rs232_worker_ptr->data_from_rs232.clear();
                            memcpy(&from_bort_cmd_rs232_str, rs232_worker_ptr->cmdBuf, rs232_worker_ptr->len_CMD_from_Board);

                            receivedCMD = rs232_worker_ptr->find_cmd();

                            if(rs232_worker_ptr->check_check_sum())
                            {
                                if(receivedCMD == (int)COMMAND_RS232::CALIBRATE_DRIFT)
                                {
                                    cout << "==== RECEIVED from RS232 CALIBRATE_DRIFT ====\n";
                                    calibrateDrift();
                                } // END if(receivedCMD == (int)COMMAND_RS232::CALIBRATE_DRIFT)

                                if(receivedCMD == (int)COMMAND_RS232::CALIBRATE_I2C_ZERO)
                                {
                                    cout << "==== RECEIVED from RS232 CALIBRATE_I2C_ZERO ====\n";

                                } // END if(receivedCMD == (int)COMMAND_RS232::CALIBRATE_I2C_ZERO)

                                if(receivedCMD == (int)COMMAND_RS232::POWER_OFF_1)
                                {
                                    cout << "==== RECEIVED from RS232 CMD POWER_OFF  ====\n";
                                    signal_flag = true;
                                    this_thread::sleep_for(3000ms);
                                    powerOFF_flag = true;
                                    break;
                                } // END if(receivedCMD == (int)COMMAND_RS232::POWER_OFF)

                                rs232_worker_ptr->setCMD(receivedCMD);
                                receivedCMD = from_bort_cmd_rs232_str.control_param;

                                // Send confirm CMD to BOARD
                                send_single_status_return(from_bort_cmd_rs232_str.control_param);
                                // Set flag to resend CMD
                                rs232_worker_ptr->need_send_to_RS485 = true;
                                rs485_worker_ptr->num_send = 0;
                            } // END if(rs232_worker_ptr->check_check_sum())
                            else
                            {
                                cout << "check_sum ERROR!" << endl;
                            } // END if(!rs232_worker_ptr->check_check_sum())
                        } // END if(rs232_worker_ptr->isCMD())
                    } // END if(rs232_worker_ptr->serial_ptr->GetNumberOfBytesAvailable() == rs232_worker_ptr->_cmdLen)
                    else
                    {
                        rs232_worker_ptr->serial_ptr->FlushIOBuffers();
                        cout << "FlushIOBuffers" << endl;
                    } // END if(rs232_worker_ptr->serial_ptr->GetNumberOfBytesAvailable() != rs232_worker_ptr->_cmdLen)

        //Receive telemetry from board
                    if(rs232_worker_ptr->serial_ptr->GetNumberOfBytesAvailable() == rs232_worker_ptr->_telemetryLenFromBoard)
                    {
#ifdef USE_LOG_CMD
                        stringstream log_strstream;
#endif // USE_LOG_CMD

                        rs232_worker_ptr->serial_ptr->Read(rs232_worker_ptr->data_from_rs232, rs232_worker_ptr->_telemetryLenFromBoard);
                        for(int i = 0; i < rs232_worker_ptr->_telemetryLenFromBoard; ++i)
                        {
                            rs232_worker_ptr->telemetryBuf[i] = rs232_worker_ptr->data_from_rs232[i];
#ifdef USE_LOG_CMD
                            log_strstream << hex << " " << uppercase << (int)rs232_worker_ptr->data_from_rs232[i] << dec;
#endif // USE_LOG_CMD
                        } // END for(int i = 0; i < rs232_worker_ptr->_telemetryLen; ++i)

#ifdef USE_LOG_CMD
                    string str2log = log_strstream.str();
                        cout << "Call log " <<  str2log << endl;
                        // cout << "Call log " <<  rs232_worker_ptr->data_from_rs232 << endl;
                    log_cmd_ptr->log_(str2log);
#endif // USE_LOG_CMD


                        if(rs232_worker_ptr->isTelemetry())
                        {
                            rs232_worker_ptr->printDataRS232(TYPE_DATA::TELEMETRY);
                            rs232_worker_ptr->data_from_rs232.clear();
                        } // END if(rs232_worker_ptr->isTelemetry())
                    } // END if(rs232_worker_ptr->serial_ptr->GetNumberOfBytesAvailable() == rs232_worker_ptr->_telemetryLenFromBoard)
                } // END if(rs232_worker_ptr->serial_ptr->IsDataAvailable())
#endif // USE_SERIAL_PTR
        // Resend telemetry data to Board
                rs232_tlm_send_to_Board();
            } // END try
            catch(const exception & e) // Error in hardware 232
            {
                cout << "======== Port RS232 is NOT AVAILABLE !!! ========  " << e.what() << endl;
                common_data_ptr->set_error_rs232(true);
                this_thread::sleep_for(50ms);
            } // END catch(const exception & e)
        } // END while(!common_data_ptr->is_error_rs232())
    }; // END function fo_workRS232
    if(!rs232_worker_ptr->is_workerRS232_start)
    {
        thread workRS232(fo_workRS232);
        thrread_232.swap(workRS232);
        this_thread::sleep_for(10ms);
    } // END if(!rs232_worker_ptr->is_workerRS232_start)
} // -- END work_rs232()

void Application::rs485_work(thread & thread_485)
{
    function fo_workRS485 = [&]()
    {
        function speed = [](float abs_delta, float V_min, float V_max, float sgn, float& V)
        {
            float V1 = abs_delta * V_max;
            if(V1 < V_min){V1 = V_min;}
            V = sgn * V1 * abs(sin(1.6 * M_PI * abs_delta));
        }; // END speed

        rs485_worker_ptr->is_workerRS485_start = true;
        string telemetry_from_485;
        telemetry_from_485.reserve(100);

        if(send_CMD_BLOCK_OFF == 1)
        {
    // set mode STABILISATION
            cout << "========  MODE_TYPE::STABILISATION  ===========\n";
            set_mode_goen((uint8_t)MODE_TYPE::STABILISATION);
#ifndef USE_MODE_STAB
            this_thread::sleep_for(10ms);
            rs485_worker_ptr->setCMD_TO_485((uint8_t)COMMAND_TO_RS485::STAB_MODE);
            rs485_worker_ptr->set_mode_Stab((uint8_t)STABILIZATION_TYPE::ANGLE_STAB);
#endif // END  #ifndef USE_MODE_STAB
            this_thread::sleep_for(10ms);
            rs485_worker_ptr->control_position_Goen(0,0);
        } // END if(send_CMD_BLOCK_OFF == 1)

        this_thread::sleep_for(50ms);
        //Variables for tracking
        float currentX_abs = 0;
        float currentY_abs = 0;
        float currentX = 0;
        float currentY = 0;
        float Speed_X = 0;
        float Speed_Y = 0;
        int signX = 0;
        int signY = 0;
         float Vmin_x = 0;
        float Vmin_y = 0;
        float Vmax_x = 0;
        float Vmax_y = 0;
        float clickX = 0;
        float clickY = 0;
        float prevX = clickX;
        float prevY = clickY;
        while(!common_data_ptr->is_error_rs485())
        {
            telemetry_from_485.clear();
            try
            {
                if(common_data_ptr->is_need_quit()){break;}
        // Resend CMD to rs485
                if(rs232_worker_ptr->need_send_to_RS485)
                {
                    rs485_worker_ptr->sendCMD_rs485(rs232_worker_ptr->getCMD(),
                                                    rs232_worker_ptr->parametr_X.buf16,
                                                    rs232_worker_ptr->parametr_Y.buf16);
                    rs232_worker_ptr->need_send_to_RS485 = false;
                } // END if (need_send_to_RS485)

                if(rs485_worker_ptr->is_tracking_start && !tracShats->isInited())
                {
                    rs485_worker_ptr->control_speed_Goen(0, 0);
                    this_thread::sleep_for(200ms);
                    if(!tracShats->isInited())
                    {
                        rs485_worker_ptr->tracking_stop();
                    } // END if (!tracShats->isInited())
                } // END if (rs485_worker_ptr->is_tracking_start && tracShats->isInited())
        // Request telemetry every 40ms
                get_status_goen();
                this_thread::sleep_for(40ms);
        // Receive telemetry data from rs485
#ifdef USE_SERIAL_PTR
                if(rs485_worker_ptr->serial_ptr->IsDataAvailable())
                {
                    int numberRead = 0;
                    if(rs485_worker_ptr->data_ready_for_read_in_port(numberRead))
                    {
                // cout << "==== "<< numberRead << " byte available in RS485 ====\n";
                        rs485_worker_ptr->serial_ptr->Read(telemetry_from_485, numberRead, 100);

                        for(int i = 0; i < numberRead; ++i)
                        {
                            rs485_worker_ptr->dataFromGoenBuf[i] = telemetry_from_485[i];
                        } // END for(int i = 0; i < numberRead; ++i)

                // rs485_worker_ptr->printDataRS485(rs485_worker_ptr->tmpBuf, numberRead);
                // cout << "\n";
                        //find 0xAA
                        int pos_AA = 0;
                        for(int i = 0; i < sizeof(rs485_worker_ptr->dataFromGoenBuf); ++i)
                        {
                            if(rs485_worker_ptr->dataFromGoenBuf[i] == 0xAA)
                            {
                                pos_AA = i;
                                break;
                            } // END if(rs485_worker_ptr->tmpBuf[i] == 0xAA)
                        } // END for(int i = 0; i < sizeof(rs485_worker_ptr->tmpBuf); ++i)
                        int long_received_data_from_goen = numberRead - pos_AA;
                        memmove(rs485_worker_ptr->dataFromGoenBuf, rs485_worker_ptr->dataFromGoenBuf + pos_AA, long_received_data_from_goen);
                // rs485_worker_ptr->printDataRS485(rs485_worker_ptr->dataFromGoenBuf, long_received_data_from_goen);
        // Check control sum
                        uint8_t CRC = rs485_worker_ptr->crc_calc(rs485_worker_ptr->dataFromGoenBuf, long_received_data_from_goen - 1);

                        if(CRC == rs485_worker_ptr->dataFromGoenBuf[long_received_data_from_goen - 1])
                        {
                            // cout <<"CRC OK\n\n";
        // Check long DATA == 47 bytes
                            if(rs485_worker_ptr->dataFromGoenBuf[7] == 0x2F && // Ansver status
                               rs485_worker_ptr->dataFromGoenBuf[8] == 0x00)
                            {
        // Copy telemetry data to stuct
                                rs485_worker_ptr->copy_telemetry_to_stuct();
                            } // END if(rs485_worker_ptr->dataFromGoenBuf[7] == 0x2F && rs485_worker_ptr->dataFromGoenBuf[7] == 0x00)
                        } // END if(CRC != rs485_worker_ptr->dataFromGoenBuf[long_telemetry_data_from_goen - 1])
                        else
                        {
                            printf("=============  CRC: calc:%02X  received:%02X =============\n", CRC, rs485_worker_ptr->dataFromGoenBuf[long_received_data_from_goen - 1]);
                        } // END if!(CRC == rs485_worker_ptr->dataFromGoenBuf[long_received_data_from_goen - 1])
                    } // END if(rs485_worker_ptr->data_ready_for_read_in_port(numberRead))
                } // END if(rs485_worker_ptr->serial_ptr->IsDataAvailable())
#endif USE_SERIAL_PTR
                //To Zero
                if(rs485_worker_ptr->moving_to_zero_position)
                {
                    if(abs(-(rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle + rs485_worker_ptr->ang0_x) -
                            rs485_worker_ptr->motor_yaw_target_angle) < rs485_worker_ptr->min_value_for_CMD_TO_ZERO &&
                        abs((rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.angle + rs485_worker_ptr->ang0_y) -
                            rs485_worker_ptr->motor_pitch_target_angle) < rs485_worker_ptr->min_value_for_CMD_TO_ZERO)
                    {
                        cout << "OK control angle!\n";
                        if (rs485_worker_ptr->need_return_to_correct_position)
                        {
                            rs485_worker_ptr->is_correcting_pozition_in_mode_azimut_follow = true;
                        }// END if (rs485_worker_ptr->need_return_to_correct_position)
                        rs485_worker_ptr->moving_to_zero_position = false;
                    } // END if(abs(rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle - rs485_worker_ptr->motor_yaw_target_angle) < rs485_worker_ptr->min_value_for_CMD_TO_ZERO)

                    // cout << "1: "<<rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle<<"2: "<<rs485_worker_ptr->motor_yaw_target_angle<<endl;
                    // cout <<abs(rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle - rs485_worker_ptr->motor_yaw_target_angle) <<endl;
                }// END if(rs485_worker_ptr->moving_to_zero_position)
                if(rs485_worker_ptr->is_correcting_pozition_in_mode_azimut_follow && !rs485_worker_ptr->ok_match) // Correcting
                {
                    rs485_worker_ptr->stop();
                        // cout<<"--- Drift correct ---\n";
                } // END if(rs485_worker_ptr->is_correcting_pozition_in_mode_azimut_follow)


        // Tracking
                if(rs485_worker_ptr->is_tracking_start)
                {
        // Init tracking variables
                    if(!rs485_worker_ptr->is_tracking_init_after_start)
                    {
                        rs485_worker_ptr->is_tracking_init_after_start = true;
                        cout << "================tracking_start()===================\n";                        
                        clickX = rs485_worker_ptr->obj_xy_x - 0.5;
                        clickY = rs485_worker_ptr->obj_xy_y - 0.5;
                        prevX = clickX;
                        prevY = clickY;
                        currentX = clickX;
                        currentY = clickY;
                        currentX_abs = abs(currentX);
                        currentY_abs = abs(currentY);
                        rs485_worker_ptr->stop();
                        this_thread::sleep_for(500ms);
#if defined(USE_TV)
                        Vmin_x = rs485_worker_ptr->Vmin * 1.77777;
                        Vmin_y = rs485_worker_ptr->Vmin;
                        Vmax_x = rs485_worker_ptr->Vmax * 3.55555;
                        Vmax_y = rs485_worker_ptr->Vmax * 2.f;
#endif // END #if defined(USE_TV)
#if defined(USE_TPV_cam)
                        Vmin_x = rs485_worker_ptr->Vmin * 1.25;
                        Vmin_y = rs485_worker_ptr->Vmin;
                        Vmax_x = rs485_worker_ptr->Vmax * 2.5;
                        Vmax_y = rs485_worker_ptr->Vmax * 2.f;
#endif // END #if defined(USE_TPV_cam)
                        if(rs485_worker_ptr->scale05)
                        {
                            Vmin_x *= 2.f;
                            Vmin_y *= 2.f;
                            Vmax_x *= 2.f;
                            Vmax_y *= 2.f;
                        } // END if(rs485_worker_ptr->scale05)
                        signX = 1;
                        if(clickX < 0){signX = -1;}
                        signY = 1;
                        if(clickY < 0){signY = -1;}
                    } // END if(!rs485_worker_ptr->is_tracking_init_after_start)
            // END Init tracking variables
            // Start Tracking
                    if(rs485_worker_ptr->zahvat)
                    {
                        if(common_data_ptr->is_need_quit()){break;}
                        currentX = rs485_worker_ptr->obj_xy_x - 0.5;
                        currentY = rs485_worker_ptr->obj_xy_y - 0.5;
                        currentX_abs = abs(currentX);
                        currentY_abs = abs(currentY);
                        signX = 1;
                        if(currentX < 0){signX = -1;}
                        signY = -1;
                        if(currentY < 0){signY = 1;}

                        speed(currentX_abs, Vmin_x, Vmax_x, signX, Speed_X);
                        speed(currentY_abs, Vmin_y, Vmax_y, signY, Speed_Y);

                        if(rs485_worker_ptr->ok_match)
                        {
                            rs485_worker_ptr->control_speed_Goen(Speed_X, Speed_Y);
                        } // END if(rs485_worker_ptr->ok_match)
                        else
                        {
                             rs485_worker_ptr->stop();
                        } // END if(!rs485_worker_ptr->ok_match)
                    } // END if(rs485_worker_ptr->zahvat)
                } // END if(rs485_worker_ptr->is_tracking_start)
            } // END try
            catch(const exception & e) // Error in hardware 485
            {
                cout << "======== Port RS485 is NOT AVAILABLE !!! ========  " << e.what() << endl;
                common_data_ptr->set_error_rs485(true);
                this_thread::sleep_for(50ms);
            } // END catch
            this_thread::sleep_for(10ms);
        } // END while(!common_data_ptr->is_error_rs485())
    }; // END function fo_workRS485

    if(!rs485_worker_ptr->is_workerRS485_start)
    {
        thread workRS485(fo_workRS485);
        thread_485.swap(workRS485);
        this_thread::sleep_for(10ms);
    } // END if(!rs485_worker_ptr->is_workerRS485_start)
} // -- END work_rs485

void Application::copy_workRect()
{
    if(common_data_ptr->is_init_all_ports())
    {
        rs485_worker_ptr->obj_xy_x = tracShats->trac_str.obj_xy_x; // Безразмерная x-координата центра цели на текущем кадре (в единицах ширины фрейма).
        rs485_worker_ptr->obj_xy_y = tracShats->trac_str.obj_xy_y; // Безразмерная y-координата центра цели на текущем кадре (в единицах высоты фрейма).
        rs485_worker_ptr->ok_match = tracShats->trac_str.ok_match;
        rs485_worker_ptr->zahvat = tracShats->trac_str.zahvat;
    } // END if(common_data_ptr->is_init_all_ports())
} // -- END copy_workRect

void Application::calibrateDrift()
{
    printf("calibrate_parameter_X: %4X\n", rs232_worker_ptr->calibrate_drift_parameter_X);
    printf("calibrate_parameter_Y: %4X\n", rs232_worker_ptr->calibrate_drift_parameter_Y);

    if((abs(rs232_worker_ptr->calibrate_drift_parameter_X) <= rs232_worker_ptr->max_calibrate_value)
        && (abs(rs232_worker_ptr->calibrate_drift_parameter_Y) <= rs232_worker_ptr->max_calibrate_value))
    {
        cout<<"---- set  Vx_dreyf and Vy_dreyf ----\n";
        rs485_worker_ptr->Vx_dreyf = Vx_dreyf0 + rs232_worker_ptr->calibrate_drift_parameter_X;
        rs485_worker_ptr->Vy_dreyf = Vy_dreyf0 + rs232_worker_ptr->calibrate_drift_parameter_Y;
    } // END if((abs(rs232_worker_ptr->calibrate_parameter_X) <= rs232_worker_ptr->max_calibrate_value) && (abs(rs232_worker_ptr->calibrate_parameter_X) <= rs232_worker_ptr->max_calibrate_value))
    else
    {
        cout<<"==== ERROR X or Y value ====\n";
    } // END if !((abs(rs232_worker_ptr->calibrate_parameter_X) <= rs232_worker_ptr->max_calibrate_value) && (abs(rs232_worker_ptr->calibrate_parameter_X) <= rs232_worker_ptr->max_calibrate_value))
} // -- END calibrateDrift()
