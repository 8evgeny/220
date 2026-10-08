#include <RS485_worker.hpp>
#include <RS232_worker.hpp>

using namespace std;
using namespace LibSerial;

void RS485_worker::setBautrade(int speed)
{
    if (speed == 9600) serial_ptr->SetBaudRate(BaudRate::BAUD_9600);
    else if (speed == 19200) serial_ptr->SetBaudRate(BaudRate::BAUD_19200);
    else if (speed == 38400) serial_ptr->SetBaudRate(BaudRate::BAUD_38400);
    else if (speed == 57600) serial_ptr->SetBaudRate(BaudRate::BAUD_57600);
    else if (speed == 115200) serial_ptr->SetBaudRate(BaudRate::BAUD_115200);
    else if (speed == 230400) serial_ptr->SetBaudRate(BaudRate::BAUD_230400);
    else
    {
        cout<<"incorrect bautrade RS485!!! set 230400\n";
        serial_ptr->SetBaudRate(BaudRate::BAUD_230400);
    }//END else
} // -- END setBautrade(int speed)

void RS485_worker::init_crc_calculation()
{
    const uint32_t bits_mask = (1 << _poly_width) - 1;
    const uint32_t top_bit = 1 << (_poly_width - 1);
    uint32_t index;
    for (index = 0; index < CRC_TABLE_SIZE; ++index)
    {
        uint32_t value = index << (_poly_width - 8);
        uint32_t bit_index;
        for (bit_index = 0; bit_index < 8; ++bit_index)
        {
            if (value & top_bit){value = (value << 1) ^ crc_polynom;}
            else {value = value << 1;}
            value &= bits_mask;
        } // END for (bit_index = 0; bit_index < 8; ++bit_index)
        _CRC8Table[index] = value;
    } // END for (index = 0; index < CRC_TABLE_SIZE; ++index)
} // -- END init_crc_calculation()

uint8_t RS485_worker::crc_calc(uint8_t *data, uint8_t size)
{
    uint8_t crc = crc_init_value;
    while (size--)
    {
        crc = _CRC8Table[crc ^ *data++];
    } // END while (size--)
    return crc;
} // -- END crc_calc(uint8_t *data, uint8_t size)

void RS485_worker::printDataRS485() //50 bytes
{
    printf( "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X "
            "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X "
            "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X \n",
            telemetryBuf[0], telemetryBuf[1], telemetryBuf[2], telemetryBuf[3], telemetryBuf[4], telemetryBuf[5], telemetryBuf[6], telemetryBuf[7],
            telemetryBuf[8], telemetryBuf[9], telemetryBuf[10], telemetryBuf[11], telemetryBuf[12], telemetryBuf[13], telemetryBuf[14], telemetryBuf[15],
            telemetryBuf[16], telemetryBuf[17], telemetryBuf[18], telemetryBuf[19], telemetryBuf[20], telemetryBuf[21], telemetryBuf[22], telemetryBuf[23],
            telemetryBuf[24], telemetryBuf[25], telemetryBuf[26], telemetryBuf[27], telemetryBuf[28], telemetryBuf[29], telemetryBuf[30], telemetryBuf[31],
            telemetryBuf[32], telemetryBuf[33], telemetryBuf[34], telemetryBuf[35], telemetryBuf[36], telemetryBuf[37], telemetryBuf[38], telemetryBuf[39],
            telemetryBuf[40], telemetryBuf[41], telemetryBuf[42], telemetryBuf[43], telemetryBuf[44], telemetryBuf[45], telemetryBuf[46]
            );
} // -- END printDataRS232()

bool RS485_worker::data_ready_for_read_in_port(int & numberRead)
{
#ifdef USE_SERIAL_PTR
    int num = serial_ptr->GetNumberOfBytesAvailable();
    this_thread::sleep_for(3ms);
    int num1 = serial_ptr->GetNumberOfBytesAvailable();
    if (num == num1)
    {
        numberRead = num1;
        return true;
    } // END if (num == num1)
    else
    {
        numberRead = num1;
        return false;
    } // END else
#else // USE_SERIAL_PTR
    return false;
#endif // !USE_SERIAL_PTR
} // -- END data_ready_in_port()

void RS485_worker::copy_telemetry_to_stuct()
{
    mut_telemetry_str.lock();
    memcpy(&from_goen_telemetry_str, dataFromGoenBuf + 9, goen_telemetry_len);

    /// ::AFM BLOCK START
#ifdef USE_MODE_STAB
    if(f_cmd_angle_control)
    {
        if (!f_azimuth_follow.load() && (abs(from_goen_telemetry_str.motor_yaw.angle - motor_yaw_target_angle) < 0.1) && (abs(from_goen_telemetry_str.motor_pitch.angle - motor_pitch_target_angle) < 0.1))
        {
            stop();
            f_cmd_angle_control = false;
            cout << "RS485_worker::Call cmd STOP to goen!" << endl;
        } // END if (!f_azimuth_follow.load() && (abs(from_goen_telemetry_str.motor_yaw.angle - motor_yaw_target_angle) < 0.1) && (abs(from_goen_telemetry_str.motor_pitch.angle - motor_pitch_target_angle) < 0.1))
    } // END  if(f_cmd_angle_control)
#endif // #ifdef USE_MODE_STAB
    mut_telemetry_str.unlock();
} // END copy_telemetry_to_stuct()

void RS485_worker::printDataRS485(uint8_t * buf ,uint8_t num_byte)
{
    for (int i = 0; i < num_byte; ++i)
    {
        printf( "%02X ", buf[i]);
    } // END for (int i = 0; i < num_byte; ++i)
    printf("\n");
} // END printDataRS485(uint8_t num_byte)

RS485_worker::RS485_worker(int speed, const string & port)
{
    cout << "RS485_worker Ctor" << endl;

    common_data_ptr = make_shared<common_data>();
#ifdef USE_SERIAL_PTR
    try
    {
        vector<string> ports =  serial_ptr->GetAvailableSerialPorts();
        cout<<"--- Available for 485 SerialPorts ---"<<endl;
        for(auto &i:ports) {cout<<i<<endl;}
        cout<<endl;
        serial_ptr = make_unique<SerialPort>(port);
        port_open_OK = true;
    } // END try
    catch (OpenFailed)
    {
        cout<<"Serial Port 485 error opening !!!" <<endl;
    } // END catch
    if(port_open_OK)
    {
        setBautrade(speed);
        data_485.reserve(100);
        init_crc_calculation();
    } // END if(port_open_OK)
    else
    {
        cout<<"\n=====================  port RS485 not OPEN !!!  ======================\n" <<endl;
    }//END else
#else // USE_SERIAL_PTR
    port_open_OK = true;
#endif // !USE_SERIAL_PTR
    bool ok = get_ini_params(config_path, section_name);
    if(!ok) {common_data_ptr->set_need_quit(true);}
} // -- END RS485_worker::RS485_worker(int speed, const string & port)

RS485_worker::~RS485_worker()
{
    cout << "Destructor RS485_worker" << endl;
}//END ~RS485_worker()


bool RS485_worker::FileIsExist(const std::string& filePath)
{
    bool isExist = false;
    ifstream fin(filePath.c_str());
    if(fin.is_open()){isExist = true;}
    fin.close();
    return isExist;
} // -- END FileIsExist

bool RS485_worker::get_ini_params(const string &config, const string &section)
{
    cout << "BEGIN get_ini_params RS485_worker" << endl;
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    bool configFileExists = FileIsExist(config);
    if(!configFileExists)
    {
        cout << "Config file '" << config << "' not exist!" << endl;
        return 0;
    } // END if(!configFileExists)

    INIReader reader(config);
    if(reader.ParseError() < 0)
    {
        cout << "Can't load '" << config << "'\n";
        return 0;
    } // END if(reader.ParseError() < 0)

    std::cout << "\n[" << section << "]:" << std::endl;

    pos_azimuth_err = reader.GetReal(section, "pos_azimuth_err", -0.00001);
    if(pos_azimuth_err == -0.00001)
    {
        cout << "\tpos_azimuth_err not declared!\n";
        return 0;
    } // END if(pos_azimuth_err == -0.00001)
    cout << "\tpos_azimuth_err = " << pos_azimuth_err << ";\n";

    pos_pitch_err = reader.GetReal(section, "pos_pitch_err", -0.00001);
    if(pos_pitch_err == -0.00001)
    {
        cout << "\tpos_pitch_err not declared!\n";
        return 0;
    } // END if(pos_pitch_err == -0.00001)
    cout << "\tpos_pitch_err = " << pos_pitch_err << ";\n";


    cout << "OK RS485_worker::get_ini_params" << endl;
    return true;
} // END get_ini_params

void RS485_worker::clearCommandPacket()
{
    commandPacket.CMD = 0x00;
    commandPacket.STATUS = 0x00;
    commandPacket.SRC.src = 0x0000;
    commandPacket.DST.dst = 0x0000;
    commandPacket.BYTE_COUNT.byteCount16 = 0x0000;
    commandPacket.CRC = 0x00;
} // -- END clearCommandPacket()

void RS485_worker::clearReadBuf()
{
    for (int i = 0; i < goen_telemetry_len; ++i)
    {
        telemetryBuf[i] = 0x00;
    }// END for (int i = 0; i < goen_telemetry_len; ++i)
} // -- END clearReadBuf()

bool RS485_worker::readFromRS485(string & data_from_rs485)
{
#ifdef USE_SERIAL_PTR
    if(serial_ptr->GetNumberOfBytesAvailable() == goen_telemetry_len) //Packet telemetry
    {
        data_from_rs485.clear();
        serial_ptr->Read(data_from_rs485, goen_telemetry_len);
        for (int i = 0; i <  goen_telemetry_len; ++i)
        {
            telemetryBuf[i] = data_from_rs485.at(i);
        } //END for (int i = 0; i <  goen_telemetry_len; ++i)
        printDataRS485();
        return true;
    } // END if(serial_ptr->GetNumberOfBytesAvailable() == goen_telemetry_len)
#endif USE_SERIAL_PTR
    return false;
} // -- END readFromRS485

void RS485_worker::writeToRS485()
{
#ifdef USE_SERIAL_PTR
    data.clear();
    num_send = 0;
    data += commandPacket.startByte;
    data += commandPacket.CMD;
    data += commandPacket.STATUS;
    data += commandPacket.SRC.function;
    data += commandPacket.SRC.device;
    data += commandPacket.DST.function;
    data += commandPacket.DST.device;
    data += commandPacket.BYTE_COUNT.byteCount8[0];
    data += commandPacket.BYTE_COUNT.byteCount8[1];
    num_send += 9;
    if(commandPacket.BYTE_COUNT.byteCount16 == 1)
    {
        data += commandPacket.DATA.data_1byte;
        num_send += 1;
    } // END if(commandPacket.BYTE_COUNT.byteCount16 == 1)

    if(commandPacket.BYTE_COUNT.byteCount16 == 8)
    {
        data += commandPacket.DATA_X.data_4byte[0];
        data += commandPacket.DATA_X.data_4byte[1];
        data += commandPacket.DATA_X.data_4byte[2];
        data += commandPacket.DATA_X.data_4byte[3];
        data += commandPacket.DATA_Y.data_4byte[0];
        data += commandPacket.DATA_Y.data_4byte[1];
        data += commandPacket.DATA_Y.data_4byte[2];
        data += commandPacket.DATA_Y.data_4byte[3];
        num_send += 8;
    } // END if(commandPacket.BYTE_COUNT.byteCount16 == 8)

    if(commandPacket.BYTE_COUNT.byteCount16 == 16)
    {
        data += commandPacket.DATA_X.data_4byte[0];
        data += commandPacket.DATA_X.data_4byte[1];
        data += commandPacket.DATA_X.data_4byte[2];
        data += commandPacket.DATA_X.data_4byte[3];
        data += commandPacket.DATA_Y.data_4byte[0];
        data += commandPacket.DATA_Y.data_4byte[1];
        data += commandPacket.DATA_Y.data_4byte[2];
        data += commandPacket.DATA_Y.data_4byte[3];
        data += commandPacket.AX_MAX.ax_8[0];
        data += commandPacket.AX_MAX.ax_8[1];
        data += commandPacket.AX_MAX.ax_8[2];
        data += commandPacket.AX_MAX.ax_8[3];
        data += commandPacket.AY_MAX.ay_8[0];
        data += commandPacket.AY_MAX.ay_8[1];
        data += commandPacket.AY_MAX.ay_8[2];
        data += commandPacket.AY_MAX.ay_8[3];
        num_send += 16;
    } // END if(commandPacket.BYTE_COUNT.byteCount16 == 16)


    commandPacket.CRC =  crc_calc((uint8_t*)data.data(), num_send);
    data += commandPacket.CRC;
    num_send += 1;

    serial_ptr->FlushIOBuffers();
    serial_ptr->Write(data);
    serial_ptr->DrainWriteBuffer();

    for (int i = 0; i < num_send; ++i)
    {
        dataToGoenBuf[i] = data[i];
    }//END for (int i = 0; i < num_send; ++i)

    if(commandPacket.DST.function != (uint8_t)COMMAND_TO_RS485::CONTROL_SPEED && commandPacket.DST.function != (uint8_t)COMMAND_TO_RS485::GET_STATUS)
    {
        cout<<"======== send to RS485 ==========    ";
        switch(commandPacket.DST.function)
        {
        case 0: {cout << "GET_VERSION: "; break;}
        case 1: {cout << " MODE: "; break;}
        case 2: {cout << " CONTROL_POSITION: "; break;}
        case 3: {cout << " CONTROL_SPEED: "; break;}
        case 4: {cout << " CONTROL_TRACKING: "; break;}
        case 5: {cout << " GET_STATUS: "; break;}
        case 6: {cout << " STAB_MODE: "; break;}
        }
        printDataRS485(dataToGoenBuf, num_send);
    }
#endif // USE_SERIAL_PTR

    num_send = 0;
} // -- END writeToRS485()

uint8_t RS485_worker::getCMD() const
{
    return _CMD_TO_485;
} // -- END getCMD()

void RS485_worker::setCMD_TO_485(uint8_t newCMD_TO_485)
{
    _CMD_TO_485 = newCMD_TO_485;
} // -- END setCMD_TO_485

void RS485_worker::sendCMD_rs485(uint8_t cmd, int16_t X, int16_t Y = 0)
{
    switch (cmd)
    {
    case ((int)COMMAND_RS232::STOP):
    {
        if(!is_tracking_start)
        {
            stop();
            if(need_return_to_correct_position)
            {
                is_correcting_pozition_in_mode_azimut_follow = true;
                moving_to_zero_position = false;
            } // END if(need_return_to_correct_position)
#ifdef USE_MODE_STAB
            if(f_azimuth_follow.load())
            {
                cout << "Set angle " << cv::Point2f(-from_goen_telemetry_str.motor_yaw.angle, from_goen_telemetry_str.motor_pitch.angle) << endl;
                set_azimut_pitch(round(-from_goen_telemetry_str.motor_yaw.angle * 100), round(from_goen_telemetry_str.motor_pitch.angle * 100));
            } // END if (f_azimuth_follow)
            else
            {
                stop();
            } // END  if(f_azimuth_follow.load())
#endif // #ifdef USE_MODE_STAB
        } // END if(!is_tracking_start)
        break;
    } // END  case ((int)COMMAND_RS232::STOP):

    case ((int)COMMAND_RS232::LEFT):
    {
        if(!is_tracking_start)
        {
            check_need_correct();
            if(need_return_to_correct_position && !is_tracking_start)
            {
                moving_to_zero_position = false;
            } // END if(need_return_to_correct_position && !is_tracking_start)
            left_right(X * k_cmd_speed_control, 0);
        } // END         if(!is_tracking_start)
        break;
    } // END case ((int)COMMAND_RS232::LEFT):
    case ((int)COMMAND_RS232::RIGHT):
    {
        if(!is_tracking_start)
        {
            check_need_correct();
            if(need_return_to_correct_position)
            {
                moving_to_zero_position = false;
            } // END if(need_return_to_correct_position)
            left_right(X * k_cmd_speed_control , 0);
        } // END         if(!is_tracking_start)
        break;
    } // END case ((int)COMMAND_RS232::RIGHT):
    case ((int)COMMAND_RS232::UP):
    {
        if(!is_tracking_start)
        {
            check_need_correct();
            if(need_return_to_correct_position)
            {
                moving_to_zero_position = false;
            } // END if(need_return_to_correct_position)
            up_down(0, Y * k_cmd_speed_control);
        } // END         if(!is_tracking_start)
        break;
    } // END case ((int)COMMAND_RS232::UP):
    case ((int)COMMAND_RS232::DOWN):
    {
        if(!is_tracking_start)
        {
            check_need_correct();
            if(need_return_to_correct_position)
            {
                moving_to_zero_position = false;
            } // END if(need_return_to_correct_position)
            up_down(0, Y * k_cmd_speed_control);
        } // END         if(!is_tracking_start)
        break;
    } // END case ((int)COMMAND_RS232::DOWN):
    case ((int)COMMAND_RS232::TO_ZERO_POSITION):
    {
        check_need_correct();
        moving_to_zero_position = true;
        motor_yaw_target_angle = 0;
        motor_pitch_target_angle = 0;
        to_zero();
#ifdef USE_MODE_STAB
        if(!f_azimuth_follow) {f_cmd_angle_control = true; motor_yaw_target_angle = 0.f; motor_pitch_target_angle = 0.f;}
#endif // #ifdef USE_MODE_STAB
        break;
    } // END case ((int)COMMAND_RS232::TO_ZERO_POSITION):

    case ((int)COMMAND_RS232::SET_AZIMUT_PITCH):
    {
        check_need_correct();
        moving_to_zero_position = true;
        motor_yaw_target_angle = (X + ang0_x) * 0.01;
        motor_pitch_target_angle = (Y + ang0_y) * 0.01;
        cout << "target angle = " << cv::Point2f(motor_yaw_target_angle, motor_pitch_target_angle) << endl;
        set_azimut_pitch(X,Y);
#ifdef USE_MODE_STAB
        if(!f_azimuth_follow) {f_cmd_angle_control = true; motor_yaw_target_angle = -(X + ang0_x) * 0.01 ; motor_pitch_target_angle = (Y + ang0_y) * 0.01;}
#endif // #ifdef USE_MODE_STAB
        break;
    } // END case ((int)COMMAND_RS232::SET_AZIMUT_PITCH):

    case ((int)COMMAND_RS232::SEND_READ_REQUEST):{ send_read_request();   break; }
    case((int)COMMAND_RS232::AZIMUTH_FOLLOW):
    {
        is_correcting_pozition_in_mode_azimut_follow = false;
        need_return_to_correct_position = false;
        // Поднимаем флаг режима, переходим в режим управления по углу, установив текущий угол
        cout << "sendCMD_rs485::SET AZIMUTH FOLLOW" << endl;
#ifdef USE_MODE_STAB
        f_azimuth_follow.store(true);
        set_azimut_pitch(round(-from_goen_telemetry_str.motor_yaw.angle * 100), round(from_goen_telemetry_str.motor_pitch.angle * 100));
#elif !defined(USE_MODE_STAB)
        set_mode_Stab((uint8_t)STABILIZATION_TYPE::ANGLE_STAB); // включаем сопровождение угла
#endif // !defined(USE_MODE_STAB)
        break;
    } // END case((int)COMMAND_RS232::AZIMUTH_FOLLOW)

    case((int)COMMAND_RS232::CLOSE_FOLLOW):
    {
        cout << "sendCMD_rs485::SET CLOSE FOLLOW" << endl;
        // Поднимаем флаг режима, переходим в режим управления по скорости, подав команду нулевых скоростей
#ifdef USE_MODE_STAB
        f_azimuth_follow.store(false);
#elif !defined(USE_MODE_STAB)
        set_mode_Stab((uint8_t)STABILIZATION_TYPE::ABS_STAB); // выключаем сопровождение угла
#endif // !defined(USE_MODE_STAB)
        if (!is_correcting_pozition_in_mode_azimut_follow)
        {
            is_correcting_pozition_in_mode_azimut_follow = true;
        } // END if (!is_correcting_pozition_in_mode_azimut_follow)
        break;
    } // END case((int)COMMAND_RS232::CLOSE_FOLLOW)

    case((int)COMMAND_RS232::ELECTRIC_LOCK_ON):
    {
#ifdef USE_MODE_STAB
        if(f_azimuth_follow.load()) {set_azimut_pitch(round(-from_goen_telemetry_str.motor_yaw.angle * 100), round(from_goen_telemetry_str.motor_pitch.angle * 100));}
        else {stop();}
        this_thread::sleep_for(10ms);
        f_motor_lock.store(true);
#endif // #ifdef(USE_MODE_STAB)
        cout << "sendCMD_rs485::SET ELECTRIC_LOCK_ON" << endl;
        if(f_motor_on.load())
        {
            set_mode_Goen((uint8_t)MODE_TYPE::ROTARY_PLATFORM);
        } // END if(f_motor_on.load())
        break;
    } // END case((int)COMMAND_RS232::ELECTRIC_LOCK_ON):

    case((int)COMMAND_RS232::ELECTRIC_LOCK_OFF):
    {
        cout << "sendCMD_rs485::SET ELECTRIC_LOCK_OFF" << endl;
#ifdef USE_MODE_STAB
        f_motor_lock.store(false);
#endif // #ifdef(USE_MODE_STAB)
        if(f_motor_on.load())
        {
            set_mode_Goen((uint8_t)MODE_TYPE::STABILISATION);
#ifdef USE_MODE_STAB
            this_thread::sleep_for(5ms);
            if(f_azimuth_follow.load()) {set_azimut_pitch(round(-from_goen_telemetry_str.motor_yaw.angle * 100), round(from_goen_telemetry_str.motor_pitch.angle * 100));}
            else {stop();}
#endif // #ifdef(USE_MODE_STAB)
        } // END if(rs485_worker_ptr->f_motor_on.load())
        break;
    } // END

    case ((int)COMMAND_RS232::MOTOR_ON):
    {
#ifdef USE_MODE_STAB
        f_motor_on.store(true);
        cout << "sendCMD_rs485::SET MOTOR_ON" << endl;
        if(f_motor_lock.load())
        {
            set_mode_Goen((uint8_t)MODE_TYPE::ROTARY_PLATFORM);
        } // END if(rs485_worker_ptr->f_motor_lock.load())
        else
        {
            set_mode_Goen((uint8_t)MODE_TYPE::STABILISATION);
            this_thread::sleep_for(5ms);
            if(f_azimuth_follow.load()) {set_azimut_pitch(round(-from_goen_telemetry_str.motor_yaw.angle * 100), round(from_goen_telemetry_str.motor_pitch.angle * 100));}
            else {stop();}
        } // END if(!rs485_worker_ptr->f_motor_lock.load())
#elif !defined(USE_MODE_STAB)
        cout << "sendCMD_rs485::SET MOTOR_ON" << endl;
        if(last_mode == (uint8_t)MODE_TYPE::ROTARY_PLATFORM)
        {
            set_mode_Goen((uint8_t)MODE_TYPE::ROTARY_PLATFORM);
        } // END if(rs485_worker_ptr->f_motor_lock.load())
        if(last_mode == (uint8_t)MODE_TYPE::STABILISATION)
        {
            set_mode_Goen((uint8_t)MODE_TYPE::STABILISATION);
            this_thread::sleep_for(5ms);
        } // END if(!rs485_worker_ptr->f_motor_lock.load())
#endif  // !defined(USE_MODE_STAB)
        break;
    } // END case ((int)COMMAND_RS232::MOTOR_ON):

    case ((int)COMMAND_RS232::MOTOR_OFF):
    {
        cout << "sendCMD_rs485::SET MOTOR_OFF" << endl;
#ifdef USE_MODE_STAB
        f_motor_on.store(false);
        set_mode_Goen((uint8_t)MODE_TYPE::MOTOR_OFF);
#elif !defined(USE_MODE_STAB)
        last_mode = from_goen_telemetry_str.mode_mems;
        set_mode_Goen((uint8_t)MODE_TYPE::MOTOR_OFF);
#endif // #elif !defined(USE_MODE_STAB)
        break;
    } // END case ((int)COMMAND_RS232::MOTOR_OFF):

    case ((int)COMMAND_RS232::TRACKING_START):
    {
        tracking_stop();
        if(!is_tracking_start)
        {
            is_tracking_start = true;
            check_need_correct();
        }//END if (!is_tracking_start)
        break;
    } // END case ((int)COMMAND_RS232::TRACKING_START):

    case ((int)COMMAND_RS232::TRACKING_STOP):
    {
        tracking_stop();
        if(need_return_to_correct_position)
        {
            need_return_to_correct_position = false;
            is_correcting_pozition_in_mode_azimut_follow = true;
        }//END if(need_return_to_correct_position)
        break;
    } // END case ((int)COMMAND_RS232::TRACKING_STOP):
    default: break;
    } // END switch
} // -- END sendCMD_rs485()
