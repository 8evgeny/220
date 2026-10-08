#include "main_window_constructor.hpp"

using namespace std;
using namespace cv;
using namespace cvw;

/// DBG::
void cmbbx_edit_com_slot(MainWindowConstructor* mwc, int x, int y)
{
    std::vector<std::string> v_com_names_buf, v_com_names;
    string substr = "USB";
    if(!mwc->rs232_ptr->isOpen())
    {
        v_com_names_buf = mwc->rs232_ptr->getListSerialPorts(); // (v_com_names_buf);

        for(int i = 0; i < v_com_names_buf.size(); i++)
        {
            int pos = 0;
            pos = v_com_names_buf[i].find(substr, pos);
            if(pos != std::string::npos)
            {
                cout << "Emplace " << v_com_names_buf[i] << endl;
                v_com_names.emplace_back(v_com_names_buf[i]);
            } // END if(pos != std::string::npos)
        } // END f аАВТОor(int i = 0; i < v_com_names_buf.size(); i++)
        if(v_com_names.size())
        {
            mwc->cmbbx_edit_com->set_vec(v_com_names);
        } // END if(v_com_names.size())
        else
        {
            v_com_names.emplace_back("Can't find ttyUSB*!");
            mwc->cmbbx_edit_com->set_vec(v_com_names);
        } // END if(!v_com_names.size())
    } // END if(!mw->rs232_ptr->isOpen())
    else
    {
        cout << "PORT iS BUSY" << endl;
    } // END if(mw->rs232_ptr->isOpen())
} // -- END combobox_text_slot

void cmbbx_edit_url_slot(MainWindowConstructor* mwc, int x, int y)
{
    //    std::string filename = "../innnoAirInfoConfig.json";
    //    Json::Value root;
    //    // Json::arrayValue group;

    //    Json::Reader reader;
    //    ifstream test(filename, ifstream::binary);
    //    string cur_line;
    //    bool success = true;
    //    bool parsingSuccessful = reader.parse(test, root, false);

    //    std::vector<string> v_url;
    //    for (const auto v : root["group"])
    //    {
    //        Json::Value member = v;
    //        std::string id = member["id"].asString();
    //        cout << "id: " << id;
    //        std::string url = member["url"].asString();
    //        cout << "; url: " << url << endl;
    //        v_url.emplace_back(url);
    //    } // END  for (const auto v : root["group"])
    //    if(v_url.size())
    //    {
    //        mwc->cmbbx_edit_url->set_vec(v_url);
    //    } // END if(v_com_names.size())
    //    else
    //    {
    //        v_url.emplace_back("Can't find ttyUSB*!");
    //        mwc->cmbbx_edit_url->set_vec(v_url);
    //    } // END if(!v_com_names.size())
    //    mwc->cmbbx_edit_url->set_clr_base(mwc->btn_clr_base_mouse_in, mwc->btn_clr_base + Scalar(15,15,15));
    //    mwc->cmbbx_edit_url->set_res(v_url[0]);
} // -- END combobox_text_slot


/// ##################### SETTINGS SLOTS

void frame_show_device_slot(MainWindowConstructor* mwc, int x, int y)
{
    mwc->set_cmd((uint8_t)CMD_RS232::TRAC_SIZE_CHANGE, mwc->zahvat_size, 0, 0);
    this_thread::sleep_for(50ms);
    mwc->set_cmd((uint8_t)CMD_RS232::TRACKING_START, x, y, 0);
    cout << dec << "frame_show_device slot wasw CALL. Init track in " << Point(x,y) << endl;
} // -- END frame_show_device_slot

void line_edit_url_callback(MainWindowConstructor * mwc, std::string & str)
{
    if(mwc->frame_show_device->device_id == GST_PIPELINE_DEVICE)
    {
        mwc->frame_show_device->location = str;
        mwc->frame_show_device->location_rtsp = "location=" + str;
    } // END if(device_id == GST_PIPELINE_DEVICE)
    cout << "Line Edit callback!"  << endl;
} // -- END line_edit_url_callback

void lbl_fps_timed(MainWindowConstructor * mwc)
{
    cout << "START lbl_fps_timed!" << endl;
    stringstream stream;
    bool ok = false;
    logger_artem::create(mwc->config_path, ok);
    if(!ok)
    {
        cout << "END lbl_fps_timed!" << endl;
        return;
    } // END if(!ok)
#ifdef USE_LOGGER
    while(true)
    {
        if(mwc->frame_show_device->get_state())
        {
            stream << std::fixed << std::setprecision(2) << LoggerArtem::inst().last_fps;
            std::string fps_str = stream.str() + " кадр/с";
            stream.str("");
            cout << "lbl_fps_timed::FPS = " << fps_str << endl;
            mwc->lbl_fps->setText(fps_str, mwc->frame_mainWindow);
            this_thread::sleep_for(1000ms);
        } // END if(mw->frame_show_device != nullptr)
        else
        {
            std::string fps_str = " " + stream.str();
            stream.str("");
            mwc->lbl_fps->setText(fps_str, mwc->frame_mainWindow);
            mwc->lbl_frame_data->setText(fps_str, mwc->frame_mainWindow);
            cout << "END lbl_fps_timed!" << endl;
            return;
        } // END if(mw->frame_show_device == nullptr)
    } // END  while(true)
#endif // USE_LOGGER
} // END lbl_fps_timed


void exec_check_status_device(MainWindowConstructor * mwc)
{
    while (true)
    {
        this_thread::sleep_for(1000ms);
        if(!mwc->frame_show_device->get_state())
        {
            cout << "Device error::stop device!" << endl;
            btn_stop_device_callback(mwc);
            break;
        } // END if(!frame_show_device->get_state())
        else
        {
            cout << "OK device state!" << endl;
        } // END if(!mwc->frame_show_device->get_state())
    } // -- END while(true)
    cout << "END exec_check_status_device!" << endl;
} // -- END exec_check_status_device

void btn_start_device_callback(MainWindowConstructor * mwc)
{
    if(mwc->frame_show_device->start())
    {
        mwc->btn_start_device->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
    } // END if(mw->frame_show_device->start())
    else
    {
        cout << "Error open device" << endl;
    } // END if(!mw->frame_show_device->start())

    if(!mwc->frame_show_device->get_state())
    {
        cout << "!frame_show_device->get_state!" << endl;
        mwc->frame_show_device->stop();
        mwc->btn_start_device->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
    } // END if(!mw->frame_show_device->get_state())
    else
    {
        thread thrd_dev_info(exec_check_status_device, mwc);
        thrd_dev_info.detach();
        cout << "OK detach exec_check_status_device!" << endl;
    } // END if(!mw->frame_show_device->get_state())
} // -- END btn_open_device_callback

void btn_stop_device_callback(MainWindowConstructor * mwc)
{
    mwc->frame_show_device->stop();
    mwc->btn_start_device->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
} // -- END btn_stop_device_callback

void btn_open_com_callback(MainWindowConstructor * mwc)
{
    cout << "Try open COM: " << mwc->cmbbx_edit_com->get_str() << endl;
    if(mwc->rs232_ptr == nullptr)
    {
        mwc->rs232_ptr = make_shared<RS232TransieverLS>();
    } // END if(mw->rs232_ptr == nullptr)

    if(!mwc->rs232_ptr->isOpen())
    {
        cout << "call list of aviable com ports" << endl;
        vector<string> v_com = mwc->rs232_ptr->getListSerialPorts();

        // DBG::
        cout << "COM PORTS:" << endl;
        std::string substr = "USB";
        for(int i = 0; i < v_com.size(); i++)
        {
            int pos_substr = v_com[i].find(substr.c_str());
            if(pos_substr > 0)
            {
                cout << "\t" << v_com[i] << endl;;
            } // END if(pos_substr > 0)
        } // END for(int i = 0; i < v_com.size(); i++)

        if(mwc->rs232_ptr->open(mwc->cmbbx_edit_com->get_str(), 115200))
        {
            cout << "OK open COM PORT " << mwc->cmbbx_edit_com->get_str() << endl;
#ifdef USE_RS232
            std::thread thrd_keep_tm(&MainWindowConstructor::exec_keep_rs232, mwc);
            thrd_keep_tm.detach();
            this_thread::sleep_for(chrono::milliseconds(100));
#endif
            mwc->btn_open_com->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
        } // END if(mw->rs232_ptr->open(to_string(mw->com_port_num)))
        else
        {
            mwc->btn_open_com->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
            btn_close_com_callback(mwc);
            cout << "MainWindowConstructor::NOT OPEN COM " << mwc->cmbbx_edit_com->get_str() << endl;
        } // END if(to_string(mw->com_port_num))
    } // END if(!mw->rs232_ptr)
    else
    {
        cout << "COM device was created early!" << endl;
    } // END if(mw->rs232_ptr->isOpen())
} // -- END btn_open_com_callback

void btn_close_com_callback(MainWindowConstructor * mwc)
{
    if(!mwc->rs232_ptr->isOpen())
    {
        cout << "RS232 Transiever NOT INITED" << endl;
    } // END if(!mw->rs232_ptr)
    else
    {
        mwc->rs232_ptr->sync.f_keep_exec.store(false);
        this_thread::sleep_for(10ms);
        try
        {
            mwc->rs232_ptr->close();
        } // END try com_port_ptr->Close();
        catch (const exception & e)
        {
            cout << "RS232TransieverLS::close ERROR: " << e.what() << endl;
        } // END catch (const exception & e)
        this_thread::sleep_for(chrono::milliseconds(100));
        mwc->btn_open_com->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
    } // END if(mw->rs232_ptr->isOpen())
} // -- END btn_cclose_com_callback

/// ##################### PTZ CONTROL SLOTS #####################

// установка шара в положение 0
void btn_to_zero_position_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::TO_ZERO_POSITION, 0, 0, 0);
    mwc->f_need_stop_command.store(false);
} // -- END btn_to_zero_position_callback

void btn_stop_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::PTZ, 0, 0, 0);
    mwc->f_need_stop_command.store(false);
} // -- END btn_stop_callback

void cmd_stop_after_pause(MainWindowConstructor * mwc)
{
    mwc->f_exec_command_stop_control.store(true);
    chrono::system_clock::time_point tp_call_last_speed_control_cmd;
    chrono::system_clock::time_point tp_call_stop;
    while(mwc->f_exec_command_stop_control.load())
    {
        if(mwc->f_new_speed_command.load())
        {
            tp_call_last_speed_control_cmd = chrono::system_clock::now();
            tp_call_stop = tp_call_last_speed_control_cmd + chrono::seconds(1);
            mwc->f_new_speed_command.store(false);
            mwc->f_need_stop_command.store(true);
        } // END if(mw->f_new_speed_command.load())
        if(mwc->f_need_stop_command.load() && tp_call_stop < chrono::system_clock::now())
        {
            btn_stop_callback(mwc);
        } // END if(mw->f_need_stop_command.load() && tp_call_stop < chrono::system_clock::now())
    } // END while(mw->f_exec_command_stop_control.load())
    cout << "----- END cmd_stop_after_pause! -----" << endl;
} // END cmd_stop_after_pause

void btn_up_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::PTZ, 0, mwc->ptz_speed, 0);
    mwc->f_new_speed_command.store(true);
} // -- END btn_up_callback

void btn_down_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::PTZ, 0, -mwc->ptz_speed, 0);
    mwc->f_new_speed_command.store(true);
} // -- END btn_down_callback

void btn_left_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::PTZ, -mwc->ptz_speed, 0, 0);
    mwc->f_new_speed_command.store(true);
} // -- END btn_left_callback

void btn_right_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::PTZ, mwc->ptz_speed, 0, 0);
    mwc->f_new_speed_command.store(true);
} // -- END btn_right_callback


void btn_suppress_gyro_drift_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::CALIBRATE_DRIFT, (int16_t)mwc->slider_azimuth_drift->getPos(), (int16_t)mwc->slider_pitch_drift->getPos(), 0);
    //    mwc->set_cmd((uint8_t)CMD_RS232::CALIBRATE_DRIFT, mwc->x_drift, mwc->y_drift, 0);
} // -- END btn_suppress_gyro_drift_callback

void slider_drift_callback(MainWindowConstructor * mwc)
{
    mwc->x_drift = (int16_t)mwc->slider_azimuth_drift->getPos();
    string azimut_drift_str = to_string((int16_t)mwc->slider_azimuth_drift->getPos());
    mwc->lbl_azimuth_drift_val->setText(azimut_drift_str, mwc->frame_mainWindow);
    mwc->y_drift = (int16_t)mwc->slider_pitch_drift->getPos();
    string pitch_drift_str = to_string((int)mwc->slider_pitch_drift->getPos());
    mwc->lbl_pitch_drift_val->setText(pitch_drift_str, mwc->frame_mainWindow);
} // -- END slider_azimuth_drift_callback

void btn_suppress_gyro_drift_azimuth_up_callback(MainWindowConstructor * mwc)
{
    mwc->slider_azimuth_drift->setPos(mwc->slider_azimuth_drift->getPos() + 1);
    string azimuth_str = to_string((int)mwc->slider_azimuth_drift->getPos());
    mwc->lbl_azimuth_drift_val->setText(azimuth_str, mwc->frame_mainWindow);
} // -- END btn_suppress_gyro_drift_azimuth_up_callback

void btn_suppress_gyro_drift_azimuth_down_callback(MainWindowConstructor * mwc)
{
    mwc->slider_azimuth_drift->setPos(mwc->slider_azimuth_drift->getPos() - 1);
    string azimuth_str = to_string((int)mwc->slider_azimuth_drift->getPos());
    mwc->lbl_azimuth_drift_val->setText(azimuth_str, mwc->frame_mainWindow);
} // -- END btn_suppress_gyro_drift_azimuth_down_callback

void btn_suppress_gyro_drift_pitch_up_callback(MainWindowConstructor * mwc)
{
    mwc->slider_pitch_drift->setPos(mwc->slider_pitch_drift->getPos() + 1);
    string azimuth_str = to_string((int)mwc->slider_pitch_drift->getPos());
    mwc->lbl_pitch_drift_val->setText(azimuth_str, mwc->frame_mainWindow);
} // -- END btn_suppress_gyro_drift_pitch_up_callback

void btn_suppress_gyro_drift_pitch_down_callback(MainWindowConstructor * mwc)
{
    mwc->slider_pitch_drift->setPos(mwc->slider_pitch_drift->getPos() - 1);
    string pitch_str = to_string((int)mwc->slider_pitch_drift->getPos());
    mwc->lbl_pitch_drift_val->setText(pitch_str, mwc->frame_mainWindow);
} // -- END btn_suppress_gyro_drift_pitch_down_callback


/// I2C zero control
void btn_calib_i2c_zero_callback(MainWindowConstructor * mwc)
{
    uint8_t buf[2] = {0,0};
    memcpy(&buf[0], &mwc->calib_i2c_zero_parameter_auto, 2);
    cout << "buf = " << (int)buf[0] << ", " << (int)buf[1] << endl;

    mwc->set_cmd((uint8_t)CMD_RS232::CALIBRATE_I2C_ZERO, mwc->calib_i2c_zero_parameter_auto, mwc->calib_i2c_zero_parameter_auto, buf[0], buf[1]);
} // -- END btn_calib_i2c_zero_callback

void btn_calib_handle_i2c_zero_callback(MainWindowConstructor * mwc)
{
    uint8_t buf[2] = {0,0};
    mwc->azimuth_i2c = (int16_t)mwc->slider_azimuth_i2c->getPos();
    memcpy(&buf[0], &mwc->azimuth_i2c, 2);
    cout << "mwc->azimuth_i2c = " << (int)mwc->azimuth_i2c << endl;
    cout << "buf = " << (int)buf[0] << ", " << (int)buf[1] << endl;
    mwc->set_cmd((uint8_t)CMD_RS232::CALIBRATE_I2C_ZERO, (int16_t)mwc->slider_roll_i2c->getPos(), (int16_t)mwc->slider_pitch_i2c->getPos(), buf[0], buf[1]);
    //    mwc->set_cmd((uint8_t)CMD_RS232::CALIBRATE_DRIFT, mwc->roll_i2c, mwc->pitch_i2c, 0);
} // -- END btn_calib_handle_i2c_zero_callback

void slider_i2c_callback(MainWindowConstructor * mwc)
{
    mwc->roll_i2c = (int16_t)mwc->slider_roll_i2c->getPos();
    string azimut_i2c_str = to_string((int16_t)mwc->slider_roll_i2c->getPos());
    mwc->lbl_roll_i2c_val->setText(azimut_i2c_str, mwc->frame_mainWindow);
    mwc->pitch_i2c = (int16_t)mwc->slider_pitch_i2c->getPos();
    string pitch_i2c_str = to_string((int)mwc->slider_pitch_i2c->getPos());
    mwc->lbl_pitch_i2c_val->setText(pitch_i2c_str, mwc->frame_mainWindow);
    mwc->azimuth_i2c = (int16_t)mwc->slider_azimuth_i2c->getPos();
    string azimuth_i2c_str = to_string((int)mwc->slider_azimuth_i2c->getPos());
    mwc->lbl_azimuth_i2c_val->setText(azimuth_i2c_str, mwc->frame_mainWindow);
} // -- END slider_roll_i2c_callback

void btn_calib_i2c_zero_roll_up_callback(MainWindowConstructor * mwc)
{
    mwc->slider_roll_i2c->setPos(mwc->slider_roll_i2c->getPos() + 1);
    string roll_str = to_string((int)mwc->slider_roll_i2c->getPos());
    mwc->lbl_roll_i2c_val->setText(roll_str, mwc->frame_mainWindow);
} // -- END btn_calib_i2c_zero_roll_up_callback

void btn_calib_i2c_zero_roll_down_callback(MainWindowConstructor * mwc)
{
    mwc->slider_roll_i2c->setPos(mwc->slider_roll_i2c->getPos() - 1);
    string roll_str = to_string((int)mwc->slider_roll_i2c->getPos());
    mwc->lbl_roll_i2c_val->setText(roll_str, mwc->frame_mainWindow);
} // -- END btn_calib_i2c_zero_roll_down_callback

void btn_calib_i2c_zero_pitch_up_callback(MainWindowConstructor * mwc)
{
    mwc->slider_pitch_i2c->setPos(mwc->slider_pitch_i2c->getPos() + 1);
    string roll_str = to_string((int)mwc->slider_pitch_i2c->getPos());
    mwc->lbl_pitch_i2c_val->setText(roll_str, mwc->frame_mainWindow);
} // -- END btn_calib_i2c_zero_pitch_up_callback

void btn_calib_i2c_zero_pitch_down_callback(MainWindowConstructor * mwc)
{
    mwc->slider_pitch_i2c->setPos(mwc->slider_pitch_i2c->getPos() - 1);
    string pitch_str = to_string((int)mwc->slider_pitch_i2c->getPos());
    mwc->lbl_pitch_i2c_val->setText(pitch_str, mwc->frame_mainWindow);
} // -- END btn_calib_i2c_zero_pitch_down_callback

void btn_calib_i2c_zero_azimuth_up_callback(MainWindowConstructor * mwc)
{
    mwc->slider_azimuth_i2c->setPos(mwc->slider_azimuth_i2c->getPos() + 1);
    string roll_str = to_string((int)mwc->slider_azimuth_i2c->getPos());
    mwc->lbl_azimuth_i2c_val->setText(roll_str, mwc->frame_mainWindow);
} // -- END btn_calib_i2c_zero_azimuth_up_callback

void btn_calib_i2c_zero_azimuth_down_callback(MainWindowConstructor * mwc)
{
    mwc->slider_azimuth_i2c->setPos(mwc->slider_azimuth_i2c->getPos() - 1);
    string azimuth_str = to_string((int)mwc->slider_azimuth_i2c->getPos());
    mwc->lbl_azimuth_i2c_val->setText(azimuth_str, mwc->frame_mainWindow);
} // -- END btn_calib_i2c_zero_azimuth_down_callback


void btn_tm_xy_mode_off_callback(MainWindowConstructor * mwc)
{
    mwc->tm_xy_mode = 0;
    mwc->btn_tm_xy_mode_off->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
    mwc->btn_tm_xy_mode_on->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
    mwc->set_cmd((uint8_t)CMD_RS232::TLM_SENDER_MODE_SWITCH, 0, (int16_t)mwc->tm_xy_mode, 0);
} // -- END btn_tm_xy_mode_off_callback

void btn_tm_xy_mode_on_callback(MainWindowConstructor * mwc)
{
    mwc->tm_xy_mode = 1;
    mwc->btn_tm_xy_mode_off->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
    mwc->btn_tm_xy_mode_on->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
    mwc->set_cmd((uint8_t)CMD_RS232::TLM_SENDER_MODE_SWITCH, 0, (int16_t)mwc->tm_xy_mode, 0);
} // -- END btn_tm_xy_mode_on_callback

void btn_abs_angle_mode_off_callback(MainWindowConstructor * mwc)
{
    mwc->abs_angle_mode = 0;
    mwc->btn_abs_angle_mode_off->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
    mwc->btn_abs_angle_mode_complex->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
    mwc->set_cmd((uint8_t)CMD_RS232::TLM_SENDER_MODE_SWITCH, 1, (int16_t)mwc->abs_angle_mode, 0);
} // -- END btn_abs_angle_mode_off_callback


void btn_abs_angle_mode_complex_callback(MainWindowConstructor * mwc)
{
    mwc->abs_angle_mode = 1;
    mwc->btn_abs_angle_mode_off->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
    mwc->btn_abs_angle_mode_complex->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
    mwc->set_cmd((uint8_t)CMD_RS232::TLM_SENDER_MODE_SWITCH, 1, (int16_t)mwc->abs_angle_mode, 0);
} // -- END btn_abs_angle_mode_complex_callback

void btn_angular_velocity_mode_off_callback(MainWindowConstructor * mwc)
{
    mwc->tm_xy_mode = 0;
    mwc->btn_angular_velocity_mode_off->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
    mwc->btn_angular_velocity_mode_on->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
    mwc->set_cmd((uint8_t)CMD_RS232::TLM_SENDER_MODE_SWITCH, 2, (int16_t)mwc->tm_xy_mode, 0);
} // -- END btn_angular_velocity_mode_off_callback
void btn_angular_velocity_mode_on_callback(MainWindowConstructor * mwc)
{
    mwc->tm_xy_mode = 1;
    mwc->btn_angular_velocity_mode_off->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
    mwc->btn_angular_velocity_mode_on->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
    mwc->set_cmd((uint8_t)CMD_RS232::TLM_SENDER_MODE_SWITCH, 2, (int16_t)mwc->tm_xy_mode, 0);
} // -- END btn_angular_velocity_mode_on_callback




void key_control_callback(MainWindowConstructor *mwc, unsigned char key)
{
    switch((int)key)
    {
    case (int)'w': {btn_up_callback(mwc); break;}
    case (int)'W': {btn_up_callback(mwc); break;}
    case (int)'s': {btn_down_callback(mwc); break;}
    case (int)'S': {btn_down_callback(mwc); break;}
    case (int)'a': {btn_left_callback(mwc); break;}
    case (int)'A': {btn_left_callback(mwc); break;}
    case (int)'d': {btn_right_callback(mwc); break;}
    case (int)'D': {btn_right_callback(mwc); break;}
    case (int)'x': {btn_trac_size_up_callback(mwc); break;}
    case (int)'X': {btn_trac_size_up_callback(mwc); break;}
    case (int)'z': {btn_trac_size_down_callback(mwc); break;}
    case (int)'Z': {btn_trac_size_down_callback(mwc); break;}
    case 32: {btn_stop_callback(mwc); break;}  /*SPACE*/
    case 27: {btn_stop_tracking_callback(mwc); break;} /*ESCAPE*/
    case (int)'r':
    {
        this_thread::sleep_for(5ms);
        rectangle(mwc->frame_mainWindow, mwc->rct_record, Scalar(0,255,0), -1);
        if(mwc->mw.get_record_status()) {mwc->lbl_record_on->setImage(mwc->img_record_off);}
        else {mwc->lbl_record_on->setImage(mwc->img_record_on);}
        break;
    } // END case (int)'r':
    case (int)'R':
    {
        this_thread::sleep_for(5ms);
        rectangle(mwc->frame_mainWindow, mwc->rct_record, Scalar(0,255,0), -1);
        if(mwc->mw.get_record_status()) {mwc->lbl_record_on->setImage(mwc->img_record_off);}
        else {mwc->lbl_record_on->setImage(mwc->img_record_on);}
        break;
    } // END  case (int)'R':
    default: { break; }
    } // END switch((int)key)
    return;
} // END key_position_control_callback


void btn_stop_tracking_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::TRACKING_STOP, 0, 0, 0);
} // -- END btn_stop_tracking_callback

void btn_TPV_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::INFRA, 0, 0, 0);
} // -- END btn_TPV_callback

void btn_TV_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::TV, 0, 0, 0);
} // -- END btn_TV_callback

void btn_set_angle_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::SET_AZIMUTH_PITCH, (int)(mwc->slider_edit_azimuth->getPos() * 100), (int)(mwc->slider_edit_pitch->getPos() * 100), 0);
    mwc->f_need_stop_command.store(false);
} // -- END on_pushButton_clicked

///// ANOTER COMMANDS

void btn_abs_stab_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::MOTOR_ON, 0, 0, 0);
    this_thread::sleep_for(80ms);
    mwc->set_cmd((uint8_t)CMD_RS232::ELECTRIC_LOCK_OFF, 0, 0, 0);
    this_thread::sleep_for(80ms);
    mwc->set_cmd((uint8_t)CMD_RS232::CLOSE_FOLLOW, 0, 0, 0);
} // -- END btn_abs_stab_callback

void btn_ang_stab_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::MOTOR_ON, 0, 0, 0);
    this_thread::sleep_for(80ms);
    mwc->set_cmd((uint8_t)CMD_RS232::ELECTRIC_LOCK_OFF, 0, 0, 0);
    this_thread::sleep_for(80ms);
    mwc->set_cmd((uint8_t)CMD_RS232::AZIMUTH_FOLLOW, 0, 0, 0);
} // -- END btn_ang_stab_callback

void btn_rotaty_platform_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::MOTOR_ON, 0, 0, 0);
    this_thread::sleep_for(80ms);
    mwc->set_cmd((uint8_t)CMD_RS232::ELECTRIC_LOCK_ON, 0, 0, 0);
} // -- END btn_rotaty_platform_callback

void btn_motor_off_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::MOTOR_OFF, 0, 0, 0);
} // -- END btn_motor_off_callback

void btn_exit_programm_callback(MainWindowConstructor * mwc)
{
    cout << "Button quit handle clicked!" << endl;
    mwc->mw.quit();
#ifdef USE_LOG_CMD
    mwc->log_cmd_ptr->quit();
#endif // USE_LOG_CMD
#ifdef USE_RESEND_RTSP
    mwc->frame_show_device->mat2rtsp_sender->~mat2rtsp();
#endif // USE_RESEND_RTSP
} // -- END btn_exit_programm_callback

//void btn_image_enhancement_callback(MainWindowConstructor * mwc)
//{
//    if(!mwc->f_image_enchance)
//    {
//        mwc->set_cmd((uint8_t)CMD_RS232::IMAGE_ENCHANCE_ON, 0, 0, 0);
//    } // END if(!mw->f_image_enchance)
//    else
//    {
//        mwc->set_cmd((uint8_t)CMD_RS232::IMAGE_ENCHANCE_OFF, 0, 0, 0);
//    } // END if(mw->f_image_enchance)
//} // -- END btn_image_enhancement_callback

//void btn_infrared_inversion_callback(MainWindowConstructor * mwc)
//{
//    if(!mwc->f_infrared_inversion)
//    {
//        if(mwc->set_cmd((uint8_t)CMD_RS232::IR_BLACK_HEAT, 0, 0, 0))
//        {
//            mwc->btn_infrared_inversion->set_clr_base(mwc->btn_clr_active, mwc->btn_clr_active_mouse_in);
//            mwc->f_infrared_inversion = true;
//        } // END if(mw->set_cmd((uint8_t)CMD_RS232::IR_BLACK_HEAT, 0, 0, 0))
//    }  // END if(!mw->f_infrared_inversion)
//    else
//    {
//        if(mwc->set_cmd((uint8_t)CMD_RS232::IR_WHITE_HEAT, 0, 0, 0))
//        {
//            mwc->btn_infrared_inversion->set_clr_base(mwc->btn_clr_base, mwc->btn_clr_base_mouse_in);
//            mwc->f_infrared_inversion = false;
//        } // END if(mw->set_cmd((uint8_t)CMD_RS232::IR_WHITE_HEAT, 0, 0, 0))
//    }  // if(mw->f_infrared_inversion)
//} // -- END btn_infrared_inversion_callback

//void btn_on_servo_callback(MainWindowConstructor * mwc)
//{
//    mwc->set_cmd((uint8_t)CMD_RS232::MOTOR_ON, 0, 0, 0);
//} // -- END btn_on_servo_callback

//void btn_off_servo_callback(MainWindowConstructor * mwc)
//{
//    mwc->set_cmd((uint8_t)CMD_RS232::MOTOR_OFF, 0, 0, 0);
//} // -- END btn_off_servo_callback

void btn_update_null_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::SET_ZERO_POSITION, 0, 0, 0);
} // -- END btn_update_null_callback

/// Панель управления размером рамки

void btn_trac_size_up_callback(MainWindowConstructor * mwc)
{
    mwc->zahvat_size = mwc->frame_show_device->getZahvatSize();
    mwc->zahvat_size += 4;
    if(mwc->zahvat_size > 128) {mwc->zahvat_size = 128;}
    if(mwc->zahvat_size < 8) {mwc->zahvat_size = 8;} // END if(mw->zahvat_size < 8)
    mwc->frame_show_device->setZahvatSize(mwc->zahvat_size);
    std::string trac_size_str = to_string(mwc->zahvat_size);
    mwc->lbl_trac_size_value->setText(trac_size_str, mwc->frame_mainWindow);
    mwc->set_cmd((uint8_t)CMD_RS232::TRAC_SIZE_CHANGE, mwc->zahvat_size, 0, 0);
} // -- END btn_trac_size_up_callback

void btn_trac_size_down_callback(MainWindowConstructor * mwc)
{
    mwc->zahvat_size = mwc->frame_show_device->getZahvatSize();
    mwc->zahvat_size -= 4;
    if(mwc->zahvat_size > 128) {mwc->zahvat_size = 128;}
    if(mwc->zahvat_size < 8) {mwc->zahvat_size = 8;}
    std::string trac_size_str = to_string(mwc->zahvat_size);
    mwc->lbl_trac_size_value->setText(trac_size_str, mwc->frame_mainWindow);
    mwc->frame_show_device->setZahvatSize(mwc->zahvat_size);
    mwc->set_cmd((uint8_t)CMD_RS232::TRAC_SIZE_CHANGE, mwc->zahvat_size, 0, 0);
} // -- END btn_trac_size_down_callback

/// Панель управления скоростью движения и зумом

void slider_set_angle_callback(MainWindowConstructor * mwc)
{
    string azimuth_str = to_string((int)mwc->slider_edit_azimuth->getPos());
    string pitch_str = to_string((int)mwc->slider_edit_pitch->getPos());
    mwc->lbl_set_azimuth_now->setText(azimuth_str, mwc->frame_mainWindow);
    mwc->lbl_set_pitch_now->setText(pitch_str, mwc->frame_mainWindow);
} // -- END slider_set_angle_callback

void btn_azimuth_up_callback(MainWindowConstructor *  mwc)
{
    mwc->slider_edit_azimuth->setPos(mwc->slider_edit_azimuth->getPos() + 1);
    string azimuth_str = to_string((int)mwc->slider_edit_azimuth->getPos());
    mwc->lbl_set_azimuth_now->setText(azimuth_str, mwc->frame_mainWindow);
} // -- END btn_azimuth_up_callback

void btn_azimuth_down_callback(MainWindowConstructor *  mwc)
{
    mwc->slider_edit_azimuth->setPos(mwc->slider_edit_azimuth->getPos() - 1);
    string azimuth_str = to_string((int)mwc->slider_edit_azimuth->getPos());
    mwc->lbl_set_azimuth_now->setText(azimuth_str, mwc->frame_mainWindow);
} // -- END btn_azimuth_down_callback

void btn_pitch_up_callback(MainWindowConstructor *  mwc)
{
    mwc->slider_edit_pitch->setPos(mwc->slider_edit_pitch->getPos() + 1);
    string pitch_str = to_string((int)mwc->slider_edit_pitch->getPos());
    mwc->lbl_set_pitch_now->setText(pitch_str, mwc->frame_mainWindow);
} // -- END btn_pitch_up_callback

void btn_pitch_down_callback(MainWindowConstructor *  mwc)
{
    mwc->slider_edit_pitch->setPos(mwc->slider_edit_pitch->getPos() - 1);
    string pitch_str = to_string((int)mwc->slider_edit_pitch->getPos());
    mwc->lbl_set_pitch_now->setText(pitch_str, mwc->frame_mainWindow);
} // -- END btn_pitch_down_callback

void slider_ptz_speed_callback(MainWindowConstructor * mwc)
{
    mwc->ptz_speed = mwc->slider_ptz_speed->getPos();
    string lbl_str = to_string(mwc->ptz_speed) + "/" + to_string(mwc->slider_ptz_speed->getMax());
    mwc->lbl_ptz_speed_value->setText(lbl_str, mwc->frame_mainWindow);
} // -- END btn_close_device_callback

void btn_zoom_up_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::ZOOM, 0, 0, (int8_t)mwc->zoom_factor);
} // -- END btn_zoom_up_callback

void btn_zoom_down_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::ZOOM, 0, 0, -(int8_t)mwc->zoom_factor);
} // -- END btn_zoom_down_callback

void wid_draw_ruler_callback(MainWindowConstructor * mwc)
{
    mwc->draw_ruler_mat = Scalar(100,80,80);
    mwc->drawRuler(mwc->draw_ruler_mat, mwc->draw_roll, mwc->draw_pitch);
    mwc->wid_draw_ruler->setImage(mwc->draw_ruler_mat);
} // -- END wid_draw_ruler_callback

void key_confirm_handler(MainWindowConstructor * mwc, unsigned char key)
{
    if(key == 27 || key == 'n' || key == 'N') { btn_confirm_no_callback(mwc); return;} // 27 - esc
    if(key == 13 || key == 'y' || key == 'Y') { btn_confirm_yes_callback(mwc); return;} // 13 - endter
} // END confirm_key_handler

void btn_poweroff_goen_callback(MainWindowConstructor * mwc)
{
    cout << "SET CMD POWEROFF!" << endl;
    cout << "Call confirm callback!" << endl;
    bool ok = false;
    mwc->window_confirm.exec(true);
    if(mwc->f_confirm)
    {
        uint16_t poweroff_cmd_bytes = 0;
        uint8_t buf[2] = {(uint8_t)CMD_RS232::POWER_OFF_2, (uint8_t)CMD_RS232::POWER_OFF_3};
        memcpy(&poweroff_cmd_bytes, buf, sizeof(poweroff_cmd_bytes));
        mwc->set_cmd((uint8_t)CMD_RS232::POWER_OFF_1, poweroff_cmd_bytes, 0, 0);
        cout << "YES" << endl;
        mwc->f_confirm = false;
    } // END if(mwc->f_confirm)
    else
    {
        cout << "NO" << endl;
        mwc->f_confirm = false;
        return;
    } // END if(!mwc->f_confirm)
} // -- END btn_poweroff_goen_callback

void btn_confirm_yes_callback(MainWindowConstructor * mwc)
{
    mwc->window_confirm.quit();
    mwc->f_confirm = true;
} // -- END btn_confirm_yes_callback

void btn_confirm_no_callback(MainWindowConstructor * mwc)
{
    mwc->window_confirm.quit();
    mwc->f_confirm = false;
} // -- END btn_confirm_no_callback

void btn_attack_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::START_ATTACK, 0, 0, 0);
} // -- END btn_attack_callback

void btn_stop_attack_callback(MainWindowConstructor * mwc)
{
    mwc->set_cmd((uint8_t)CMD_RS232::STOP_ATTACK, 0, 0, 0);
} // -- END btn_attack_callback


void btn_specify_attitude_angle_callback(MainWindowConstructor * mwc)
{
    cout << "Call btn SPECIFY_ATTITUDE_ANGLE handle control!" << endl;
    mwc->set_cmd((uint8_t)CMD_RS232::SPECIFY_ATTITUDE_ANGLE, 0, 0, 0);
} // -- END btn_specify_attitude_angle_callback

void  btn_calib_zero_pos_fc_att_callback(MainWindowConstructor * mwc)
{
    cout << "Call btn NUC handle control!" << endl;
    mwc->set_cmd((uint8_t)CMD_RS232::CALIB_ZERO_POS_FC_ATT, 0, 0, 0, 0);
} // -- btn_calib_zero_pos_fc_att_callback


void btn_nuc_handle_control_callback(MainWindowConstructor * mwc)
{
    cout << "Call btn NUC handle control!" << endl;
    mwc->set_cmd((uint8_t)CMD_RS232::CAMERA_CONTROL, 0, 0, 0);
} // -- END btn_nuc_handle_control_callback

//.cpp (функция для вызова в конструкторе, предварительно нужно зарезервировать для каждого вектора где-то 100 элементов)
void MainWindowConstructor::drawRuler(Mat& img, float roll, float pitch)
{
    // Одно деление по ТЗ всегда равно 5. Исходя из этого и ведутся дальнейшие расчеты
    Point center(circleCenterX, circleCenterY);
    ellipse(img, center, Size(circleRadius, circleRadius), 0, 0, 180, yellow, FILLED);

    double rad = roll * k_deg2rad;
    double cos_theta = cos(rad);
    double sin_theta = sin(rad);

    int l = 60; //переменная для отрисовки отчетов крена
    float up_y = - pitch * 0.2;
    for (int i = 0; i < points_ruler_roll.size(); i+=2)
    {
        float x1_translated = points_ruler_roll[i].x - circleCenterX;
        float y1_translated = points_ruler_roll[i].y - circleCenterY;
        float x2_translated = points_ruler_roll[i + 1].x - circleCenterX;
        float y2_translated = points_ruler_roll[i + 1].y - circleCenterY;

        float x1_rotated = x1_translated * cos_theta - y1_translated * sin_theta;
        float y1_rotated = x1_translated * sin_theta + y1_translated * cos_theta;
        float x2_rotated = x2_translated * cos_theta - y2_translated * sin_theta;
        float y2_rotated = x2_translated * sin_theta + y2_translated * cos_theta;

        x1_rotated += circleCenterX;
        y1_rotated += circleCenterY;
        x2_rotated += circleCenterX;
        y2_rotated += circleCenterY;
        line(img, Point(x1_rotated, y1_rotated), Point(x2_rotated, y2_rotated), clr_line, thickness_ruler_roll);
        if (l < 0){ putText(img, to_string(-l), Point(round((x2_rotated - 5) - l / 6 + 1), y2_rotated - 4), FONT_HERSHEY_SIMPLEX, fontScale_text, clr_line, thickness_text);}
        else { putText(img, to_string(l), Point(round((x2_rotated - 5) - l / 6 + 1), y2_rotated - 4), FONT_HERSHEY_SIMPLEX, fontScale_text, clr_line, thickness_text);}
        l -= 15;
    } // END for (int i = 0; i < points_ruler_roll.size(); i+=2)

    for (int i = 0; i < points_ruler_up.size(); i += 2)
    {
        float x1_translated = points_ruler_up[i].x - circleCenterX;
        float y1_translated = points_ruler_up[i].y - circleCenterY + spacing * 0.5 * up_y;
        float x2_translated = points_ruler_up[i + 1].x - circleCenterX;
        float y2_translated = points_ruler_up[i + 1].y - circleCenterY + spacing * 0.5 * up_y;

        float x1_rotated = x1_translated * cos_theta - y1_translated * sin_theta;
        float y1_rotated = x1_translated * sin_theta + y1_translated * cos_theta;
        float x2_rotated = x2_translated * cos_theta - y2_translated * sin_theta;
        float y2_rotated = x2_translated * sin_theta + y2_translated * cos_theta;

        x1_rotated += circleCenterX;
        y1_rotated += circleCenterY;
        x2_rotated += circleCenterX;
        y2_rotated += circleCenterY;

        if ((((circleCenterX - x1_rotated)*(circleCenterX - x1_rotated) + (circleCenterY - y1_rotated)*(circleCenterY - y1_rotated))  <= circleRadius * circleRadius) &&
                (((circleCenterX - x2_rotated)*(circleCenterX - x2_rotated) + (circleCenterY - y2_rotated)*(circleCenterY - y2_rotated))  <= circleRadius * circleRadius))
        {
            line(img, Point(x1_rotated, y1_rotated), Point(x2_rotated, y2_rotated), clr_line, thickness_ruler);
        } // END if

        if ((((circleCenterX - x1_rotated)*(circleCenterX - x1_rotated) + (circleCenterY - y1_rotated)*(circleCenterY - y1_rotated))  <= (circleRadius - 15) * (circleRadius - 15)) && (i % 4 == 0))
        {
            if (round(i * 2.5) < 100)
            {
                if (!(i == 0)){putText(img, to_string((int)round(i * 2.5)), Point(x1_rotated - 20, y1_rotated + 5), FONT_HERSHEY_SIMPLEX, fontScale_text, clr_text, thickness_text);}
                else {putText(img, to_string(0), Point(x1_rotated - 20, y1_rotated + 5), FONT_HERSHEY_SIMPLEX, fontScale_text, clr_text, thickness_text);}
            } // END  if ((round(i * 2.5)) < 100)
            else
            {
                if (!(i == 0)){putText(img, to_string((int)round(i * 2.5)), Point(x1_rotated - 26, y1_rotated + 5), FONT_HERSHEY_SIMPLEX, fontScale_text, clr_text, thickness_text);}

                else {putText(img, to_string(0), Point(x1_rotated - 26, y1_rotated + 5), FONT_HERSHEY_SIMPLEX, fontScale_text, clr_text, thickness_text);}
            } // END else
        } // END if
    } // END  for (int i = 0; i < points_ruler_up.size(); i += 2)

    for (int i = 0; i < points_ruler_down.size(); i += 2)
    {
        float x1_translated = points_ruler_down[i].x - circleCenterX;
        float y1_translated = points_ruler_down[i].y - circleCenterY + spacing * 0.5 * up_y;
        float x2_translated = points_ruler_down[i + 1].x - circleCenterX;
        float y2_translated = points_ruler_down[i + 1].y - circleCenterY + spacing * 0.5 * up_y;

        float x1_rotated = x1_translated * cos_theta - y1_translated * sin_theta;
        float y1_rotated = x1_translated * sin_theta + y1_translated * cos_theta;
        float x2_rotated = x2_translated * cos_theta - y2_translated * sin_theta;
        float y2_rotated = x2_translated * sin_theta + y2_translated * cos_theta;

        x1_rotated += circleCenterX;
        y1_rotated += circleCenterY;
        x2_rotated += circleCenterX;
        y2_rotated += circleCenterY;

        if ((((circleCenterX - x1_rotated)*(circleCenterX - x1_rotated) + (circleCenterY - y1_rotated)*(circleCenterY - y1_rotated))  <= circleRadius * circleRadius) &&
                (((circleCenterX - x2_rotated)*(circleCenterX - x2_rotated) + (circleCenterY - y2_rotated)*(circleCenterY - y2_rotated))  <= circleRadius * circleRadius))
        {
            line(img, Point(x1_rotated, y1_rotated), Point(x2_rotated, y2_rotated), clr_line, thickness_ruler);
        } // END if

        if ((((circleCenterX - x1_rotated)*(circleCenterX - x1_rotated ) + (circleCenterY - y1_rotated)*(circleCenterY - y1_rotated))  <= (circleRadius - 30) * (circleRadius - 30)) && (i % 4 == 0))
        {
            if (!(i == 0)) {putText(img, to_string(-(int)round(i * 2.5)), Point(x1_rotated - 33, y1_rotated + 5), FONT_HERSHEY_SIMPLEX, fontScale_text, clr_text, thickness_text);}
        } // END if
    } // END  for (int i = 0; i < points_ruler_down.size(); i += 2)

    for (int i = 0; i < points_plain.size(); ++i)
    {
        float x1_translated = points_plain[i].x - circleCenterX;
        float y1_translated = points_plain[i].y - circleCenterY + spacing * 0.5 * up_y;
        float x2_translated = points_plain[i + 1].x - circleCenterX;
        float y2_translated = points_plain[i + 1].y - circleCenterY + spacing * 0.5 * up_y;

        float x1_rotated = x1_translated * cos_theta - y1_translated * sin_theta;
        float y1_rotated = x1_translated * sin_theta + y1_translated * cos_theta;
        float x2_rotated = x2_translated * cos_theta - y2_translated * sin_theta;
        float y2_rotated = x2_translated * sin_theta + y2_translated * cos_theta;

        x1_rotated += circleCenterX;
        y1_rotated += circleCenterY;
        x2_rotated += circleCenterX;
        y2_rotated += circleCenterY;

        if ((((circleCenterX - x1_rotated) * (circleCenterX - x1_rotated) + (circleCenterY - y1_rotated) * (circleCenterY - y1_rotated))  <= circleRadius * circleRadius) &&
                (((circleCenterX - x2_rotated) * (circleCenterX - x2_rotated) + (circleCenterY - y2_rotated) * (circleCenterY - y2_rotated))  <= circleRadius * circleRadius))
        {
            line(img, Point(x1_rotated, y1_rotated), Point(x2_rotated, y2_rotated), clr_crosshair, thickness_ruler + 2);
        } // END if
    } // END for (int i = 0; i < points_plain.size(); ++i)
    circle(img, center, circleRadius, clr_line, thickness_circle);
} // -- END void App::drawRuler(Mat& img, float roll, float pitch)

void MainWindowConstructor::fill_vector(int rows, int cols)
{
    points_plain.reserve(100);
    points_ruler_roll.reserve(100);
    points_ruler_up.reserve(100);
    points_ruler_down.reserve(100);
    circleCenterX = cols * 0.5;
    circleCenterY = rows * 0.5;
    circleRadius = 0.4 * min(rows, cols);
    spacing = circleRadius / 3;

    double rad_30 = 30 * k_deg2rad;
    double rad_45 = 45 * k_deg2rad;
    double rad_60 = 60 * k_deg2rad;
    double rad_15 = 15 * k_deg2rad;

    float sin_30 = sin(rad_30);
    float sin_60 = sin(rad_60);
    float sin_15 = sin(rad_15);
    float cos_30 = cos(rad_30);
    float cos_45 = cos(rad_45);
    float cos_60 = cos(rad_60);
    float cos_15 = cos(rad_15);

    float rad_sin_30 = circleRadius *  sin_30;
    float rad_cos_30 = circleRadius *  cos_30;
    float rad_sin_60 = circleRadius *  sin_60;
    float rad_cos_60 = circleRadius *  cos_60;
    float rad_cos_45 = circleRadius *  cos_45;
    float rad_sin_15 = circleRadius *  sin_15;
    float rad_cos_15 = circleRadius *  cos_15;

    float longLineLength = circleRadius * 0.8;
    float shortLineLength = circleRadius * 0.5;
    float startx = circleCenterX - longLineLength * 0.5;
    float endx = circleCenterX + longLineLength * 0.5;
    float startx1 = circleCenterX - shortLineLength * 0.5;
    float endx1 = circleCenterX + shortLineLength * 0.5;

    //заполняю вектор для самолета
    points_plain.emplace_back(startx - shortLineLength * 0.5, circleCenterY);
    points_plain.emplace_back(startx - shortLineLength * 0.5 + shortLineLength, circleCenterY);
    points_plain.emplace_back(startx - shortLineLength * 0.5 + shortLineLength + ((endx + shortLineLength* 0.5 - shortLineLength)-(startx - shortLineLength * 0.5 + shortLineLength))/4, circleCenterY + spacing * 0.5);
    points_plain.emplace_back(circleCenterX, circleCenterY);
    points_plain.emplace_back(startx - shortLineLength * 0.5 + shortLineLength + ((endx + shortLineLength* 0.5 - shortLineLength)-(startx - shortLineLength * 0.5 + shortLineLength))/4*3, circleCenterY + spacing * 0.5);
    points_plain.emplace_back(endx + shortLineLength* 0.5 - shortLineLength, circleCenterY);
    points_plain.emplace_back(endx + shortLineLength * 0.5, circleCenterY);

    //заполняю вектор для линейки тангажа сверху
    points_ruler_roll.emplace_back(circleCenterX - rad_sin_60, circleCenterY - rad_cos_60); // - 60
    points_ruler_roll.emplace_back(circleCenterX - rad_sin_60 - circleRadius/11, circleCenterY - rad_cos_60 - circleRadius/15); // - 60
    points_ruler_roll.emplace_back(circleCenterX - rad_cos_45, circleCenterY - rad_cos_45); // - 45
    points_ruler_roll.emplace_back(circleCenterX - rad_cos_45 - circleRadius/14, circleCenterY - rad_cos_45 - circleRadius/15); // - 45
    points_ruler_roll.emplace_back(circleCenterX -  rad_sin_30, circleCenterY - rad_cos_30); // - 30
    points_ruler_roll.emplace_back(circleCenterX -  rad_sin_30 - circleRadius/17, circleCenterY - rad_cos_30 - circleRadius/12); // - 30
    points_ruler_roll.emplace_back(circleCenterX -  rad_sin_15, circleCenterY - rad_cos_15); // - 15
    points_ruler_roll.emplace_back(circleCenterX -  rad_sin_15 - circleRadius/25, circleCenterY - rad_cos_15 - circleRadius/10); // - 15
    points_ruler_roll.emplace_back(circleCenterX,circleCenterY - circleRadius); // 0
    points_ruler_roll.emplace_back(circleCenterX,circleCenterY - circleRadius - circleRadius/10); // 0
    points_ruler_roll.emplace_back(circleCenterX +  rad_sin_15, circleCenterY - rad_cos_15); // 15
    points_ruler_roll.emplace_back(circleCenterX +  rad_sin_15 + circleRadius/25, circleCenterY - rad_cos_15 - circleRadius/10); // 15
    points_ruler_roll.emplace_back(circleCenterX +  rad_sin_30, circleCenterY - rad_cos_30); // 30
    points_ruler_roll.emplace_back(circleCenterX +  rad_sin_30 + circleRadius/17, circleCenterY - rad_cos_30 - circleRadius/12); // 30
    points_ruler_roll.emplace_back(circleCenterX + rad_cos_45, circleCenterY - rad_cos_45); // 45
    points_ruler_roll.emplace_back(circleCenterX + rad_cos_45 + circleRadius/14, circleCenterY - rad_cos_45 - circleRadius/15); // 45
    points_ruler_roll.emplace_back(circleCenterX + rad_sin_60, circleCenterY - rad_cos_60); // 60
    points_ruler_roll.emplace_back(circleCenterX + rad_sin_60 + circleRadius/11, circleCenterY - rad_cos_60 - circleRadius/15); // 60

    for (int i = 0; i <= 40; i++)
    {
        points_ruler_up.emplace_back(startx, circleCenterY + spacing * i);
        points_ruler_up.emplace_back(endx, circleCenterY + spacing * i);
        points_ruler_up.emplace_back(startx1, circleCenterY + spacing * i + spacing * 0.5);
        points_ruler_up.emplace_back(endx1, circleCenterY + spacing * i + spacing * 0.5);
    } // END  for (int i = 0; i <= 40; i++)

    for (int i = 0; i >= - 40; i--)
    {
        points_ruler_down.emplace_back(startx, circleCenterY + spacing * i);
        points_ruler_down.emplace_back(endx, circleCenterY + spacing * i);
        points_ruler_down.emplace_back(startx1, circleCenterY + spacing * i + spacing * 0.5);
        points_ruler_down.emplace_back(endx1, circleCenterY + spacing * i + spacing * 0.5);
    } // END  for (int i = 0; i >= - 40; i--)
} // -- END fill_vector


MainWindowConstructor::MainWindowConstructor(const std::string & config, bool & ok)
{
    cout << "Constructor MainWindowConstructor" << endl;
    float n3_1 = 1.f / 3;
    float mw_h_1 = 1.f / mw_h, mw_w_1 = 1.f / mw_w;
    config_path = config;
    rs232_ptr = make_shared<RS232TransieverLS>();
#ifdef USE_UDP
    eth_udp_ptr = make_shared<get_send_data>(ok, this->config_path);
    if(!ok)
    {
        return;
    } // END if(!ok)
    if(eth_udp_ptr->eth_open())
    {
        cout << "socket opened succesfully" << endl;
    } // END if(eth_udp_ptr->open())
    else
    {
        bool ok = false;
        cout << "socket not opened!!!" << endl;
    } // END if(!eth_udp_ptr->open())
#endif // USE_UDP
#ifdef USE_LOG_CMD
    log_cmd_ptr = make_shared<LogCmd>(config_path, "log_cmd", ok);
    if(!ok)
    {
        cout << "Application::Error create LogCmd!" << endl;
        return;
    } // END if(!ok)
    else
    {
        cout << "Application:: OK create LogCmd!" << endl;
        log_cmd_ptr->start();
    } // END if(ok)

    log_cmd_ptr->log_(tlm_header_str, false);
#endif // USE_LOG_CMD

    thread thrd(&cmd_stop_after_pause, this);
    thrd.detach();
    ok = get_ini_params(config_path);
    if(!ok) {cout << "MainWindowConstructor::ERROR ini" << endl; return;}

#ifdef USE_UDP
    winname = "GOEN_client_UDP";
#endif
#ifdef USE_RS232
    winname = "GOEN_client_RS232";
#endif
    winname += "_" + open_close;
    frame_mainWindow = Mat(mw_h, mw_w, CV_8UC3, btn_clr_base);
    mw = CVMainWindow(config_path, ok, winname, frame_mainWindow);
    if(!ok) {cout << "MainWindowConstructor::ERROR create CVMainWindow!" << endl; return;}
    mw.setWindowConstructor(this);
    mw.add_key_handler(key_control_callback);
    //    namedWindow(winname, WINDOW_GUI_NORMAL);
    float lbl_height = 48.f * mw_h_1 ;
    /// Окно для отображения видео
    Rect device_rct = Rect(Point(48,dist), Size(1920, 1080));
    Panel device_panel(rct_pix2f(device_rct + Size(8,8), frame_mainWindow.size()), 1, 1, 4, frame_mainWindow.size());
    device_panel.show(frame_mainWindow);

    frame_show_device = make_shared<CVFrameShow>(config_path, ok, "FrameShow", &mw,  device_panel.get_pos_f(0,0), frame_show_device_slot, this);
    if(!ok)
    {
        cout << "MainWindowConstructor::ERROR CREATE frame_show_device!" << endl;
        return;
    } // END if(!ok)

    rct_record = Rect(5, dist, 20, 20);
    rectangle(frame_mainWindow, rct_record, Scalar(0,255,0), -1);
    img_record_on = cv::Mat(rct_record.size(), CV_8UC3, btn_clr_base);
    img_record_off = cv::Mat(rct_record.size(), CV_8UC3, btn_clr_base);
    float fontscale = getFontScaleFromHeight(3, rct_record.height);
    putText(img_record_on, "R", Point(0, rct_record.height), 3, fontscale, Scalar(0,0,255), 2);
    putText(img_record_off, "R", Point(0, rct_record.height), 3, fontscale, Scalar(0,0,0), 2);
    lbl_record_on = make_shared<CVWidget>("record", &mw, rct_pix2f(rct_record, frame_mainWindow.size()));
    lbl_record_on->setImage(img_record_off);
    lbl_record_on->setShowType(2);
    Rect rct_poweroff = Rect(5,75,20,20);
    btn_poweroff_goen = make_shared<CVButton>("ВЫКЛ. ГОЭН", &mw, rct_pix2f(rct_poweroff, frame_mainWindow.size()), &btn_poweroff_goen_callback, this);
    btn_poweroff_goen->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
    cv::Mat icon_poweroff(rct_poweroff.size(), CV_8UC1, Scalar(0));
    circle(icon_poweroff, Point(0.5 * rct_poweroff.size().width, 0.5 * rct_poweroff.size().height), 10, Scalar(255), -2);
    //    line(icon_poweroff, Point(0.5 * rct_poweroff.size().width, 2),  Point(0.5 * rct_poweroff.size().width, 0.5 * rct_poweroff.size().height - 2), Scalar(0), 7);
    //    line(icon_poweroff, Point(0.5 * rct_poweroff.size().width, 3),  Point(0.5 * rct_poweroff.size().width, 0.5 * rct_poweroff.size().height - 3), Scalar(255), 2);
    btn_poweroff_goen->setColorIcon(Scalar(0,0,255));
    btn_poweroff_goen->setBordThic(1);
    btn_poweroff_goen->setIcon(icon_poweroff);
    btn_poweroff_goen->setShowType(1);

    /// Общая панель размещения кнопок
    cout << "dbg:: ======= Rect PANEL 0 = " << Rect(pt_markup, Size(720, 1080)) << endl;
    pt_markup = pt_markup + Point(device_rct.width, 0) + Point(dist, 0);
    Panel panel0(rct_pix2f(Rect(pt_markup, Size(720, 1080)), frame_mainWindow.size()), 1, 1, 4, frame_mainWindow.size());
    line(frame_mainWindow, pt_markup, pt_markup + Point(panel0.main_rct.width * frame_mainWindow.cols,0), btn_clr_base_mouse_in, 2);

    /// Панель ввода и отображения URL
    cout << "rect 2 init panel url = " << Rect(panel0.get_pos_pix(0,0, frame_mainWindow.size()).tl(), Size(panel0.get_pos_pix(0,0, frame_mainWindow.size()).width, 3 * lbl_height * frame_mainWindow.rows)) << endl;

    /// Панель для размещения кнопок управления видео и COM-портом
    Rect rct_panel_url = Rect(panel0.get_pos_pix(0,0, frame_mainWindow.size()).tl(), Size(panel0.get_pos_pix(0,0, frame_mainWindow.size()).width, 2 * lbl_height * frame_mainWindow.rows));
    Panel panel_url(rct_pix2f(rct_panel_url, frame_mainWindow.size()), 1, 2, 2, frame_mainWindow.size());

    Panel panel_edit_url(panel_url.get_pos_f(0,0), 7, 1, 2,frame_mainWindow.size());
    lbl_edit_url = make_shared<CVWidget>("ВИДЕО:", &mw, panel_edit_url.get_pos_f(0,0));
    line_edit_url = make_shared<CVLineEdit>(frame_show_device->location, &mw, panel_edit_url.get_pos_f(1,0) + Size2f(panel_edit_url.get_pos_f(1,0).width * 3 ,0));
    line_edit_url->add_slot(line_edit_url_callback, this);
    line_edit_url->setType(EDIT_TEXT);
    btn_start_device = make_shared<CVButton>("СТАРТ", &mw, panel_edit_url.get_pos_f(5,0), btn_start_device_callback, this);
    btn_stop_device = make_shared<CVButton>("СТОП", &mw, panel_edit_url.get_pos_f(6,0), btn_stop_device_callback, this);

    Panel panel_edit_com(panel_url.get_pos_f(0,1), 7, 1, 2,frame_mainWindow.size());

    /// dbg::
    cout << "Start create cmbbx_edit_com" << endl;
    cmbbx_edit_com = make_shared<CVComboBox>("EDIT COM", &mw, panel_edit_com.get_pos_f(1,0) + Size2f(panel_edit_com.get_pos_f(1,0).width * 3,0), cmbbx_edit_com_slot, this);
    cout << "OK create cmbbx_edit_com" << endl;

    std::vector<std::string> v_com_names_buf, v_com_names;
    cout << "OK create rs_ptr in MainWindowConstructor" << endl;

    try
    {
        v_com_names_buf = rs232_ptr->getListSerialPorts(); // (v_com_names_buf);
    } // END try
    catch (const std::exception & err)
    {
        cout << "ERROR::" << err.what() << " Can't connect to serial port!" << endl;
        this_thread::sleep_for(3s);
        return;
    } // END catch

    cout << "v_com_names_buf = " << v_com_names_buf.size() << endl;
    string substr = "USB";
    for(int i = 0; i < v_com_names_buf.size(); i++)
    {
        cout << v_com_names_buf[i] << endl;
    } // END for(int i = 0; i < v_com_names_buf.size(); i++)
    for(int i = 0; i < v_com_names_buf.size(); i++)
    {
        int pos = 0;
        pos = v_com_names_buf[i].find(substr, pos);
        if(pos != std::string::npos)
        {
            cout << "Emplace " << v_com_names_buf[i] << endl;
            v_com_names.emplace_back(v_com_names_buf[i]);
        } // END if(pos != std::string::npos)
    } // END for(int i = 0; i < v_com_names_buf.size(); i++)

    if(v_com_names.size())
    {
        cmbbx_edit_com->set_vec(v_com_names);
    } // END if(v_com_names.size())
    else
    {
        v_com_names.emplace_back("Can't find ttyUSB*!");
        cmbbx_edit_com->set_vec(v_com_names);
    } // END if(!v_com_names.size())
    cmbbx_edit_com->set_clr_base(btn_clr_base_mouse_in, btn_clr_base + Scalar(15,15,15));
    cmbbx_edit_com->set_res(v_com_names[0]);
#ifdef USE_RS232
    btn_open_com = make_shared<CVButton>("ОТКРЫТЬ", &mw, panel_edit_com.get_pos_f(5,0), btn_open_com_callback, this);
    btn_close_com = make_shared<CVButton>("ЗАКРЫТЬ", &mw, panel_edit_com.get_pos_f(6,0), btn_close_com_callback, this);
#endif
    pt_markup = panel0.get_pos_pix(0,0, frame_mainWindow.size()).tl() + Point(0, panel_url.rows * lbl_height * frame_mainWindow.rows);
    line(frame_mainWindow, pt_markup, pt_markup + Point(panel0.main_rct.width * frame_mainWindow.cols,0), btn_clr_base_mouse_in, 2);

    /// Панель размещения кнопок управления двигателем

    Panel panel_ptz_blocks(rct_pix2f(Rect(pt_markup , Size(panel0.get_pos_pix(0,0, frame_mainWindow.size()).width, panel0.get_pos_pix(0,0, frame_mainWindow.size()).width * n3_1)), frame_mainWindow.size()), 3, 1, 4, frame_mainWindow.size());

    int ptz_btn_size = panel_ptz_blocks.get_pos_pix(1,0, frame_mainWindow.size()).width * n3_1 - 3 * panel_ptz_blocks.bord;

    /// Панель расположения кнопок управления движением камеры
    Panel panel_ptz(rct_pix2f(Rect(panel_ptz_blocks.get_pos_pix(0,0, frame_mainWindow.size())), frame_mainWindow.size()), 3, 3, 4, frame_mainWindow.size());

    pt_markup += Point(0, 3 * ptz_btn_size + 1.5 * dist);
    line(frame_mainWindow, pt_markup, pt_markup + Point(panel0.main_rct.width * frame_mainWindow.cols,0), btn_clr_base_mouse_in, 2);

    btn_set_null = make_shared<CVButton>("НОЛЬ", &mw, panel_ptz.get_pos_f(0,0), btn_to_zero_position_callback, this);
    btn_stop_tracking = make_shared<CVButton>("СБРОС", &mw, panel_ptz.get_pos_f(2,0), btn_stop_tracking_callback, this);
    btn_TV_TPV = make_shared<CVButton>(" ТПВ ", &mw, panel_ptz.get_pos_f(0,2), btn_TPV_callback, this);

    double ptz_bordx_2 = 3.f * mw_w_1;
    double ptz_bordx = 2 * ptz_bordx_2;
    double ptz_bordy_2 = 3.f * mw_h_1;
    double ptz_bordy = 2 * ptz_bordy_2;
    Rect2f rct_btn = panel_ptz.get_pos_f(1,0);
    rct_btn.x -= ptz_bordx_2 ;
    rct_btn.y -= ptz_bordy_2 ;
    rct_btn.width += ptz_bordx ;
    rct_btn.height += ptz_bordy ;

    btn_up = make_shared<CVButton>("ВВЕРХ", &mw, panel_ptz.get_pos_f(1,0), btn_up_callback, this);
    btn_up->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
    cv::Mat icon_direction = imread("../arrow.png", IMREAD_GRAYSCALE);
    bitwise_not(icon_direction, icon_direction);
    rotate(icon_direction, icon_direction, ROTATE_90_COUNTERCLOCKWISE);
    btn_up->setIcon(icon_direction);
    btn_up->setShowType(1);

    btn_left = make_shared<CVButton>("ЛЕВО", &mw, panel_ptz.get_pos_f(0,1), btn_left_callback, this);
    btn_left->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
    rotate(icon_direction, icon_direction, ROTATE_90_COUNTERCLOCKWISE);
    btn_left->setIcon(icon_direction);
    btn_left->setShowType(1);

    btn_stop = make_shared<CVButton>("СТОП", &mw, panel_ptz.get_pos_f(1,1), btn_stop_callback, this);
    btn_stop->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
    cv::Mat icon_stop = cv::Mat(Size(48,48), CV_8UC1, Scalar(0));
    rectangle(icon_stop, Point(12,12), Point(36,36), Scalar(255), -1);
    btn_stop->setColorIcon(Scalar(0,0,210));
    btn_stop->setIcon(icon_stop);
    btn_stop->setShowType(1);

    btn_right = make_shared<CVButton>("ПРАВО", &mw, panel_ptz.get_pos_f(2,1), btn_right_callback, this);
    btn_right->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
    rotate(icon_direction, icon_direction, ROTATE_180);
    btn_right->setIcon(icon_direction);
    btn_right->setShowType(1);

    btn_down = make_shared<CVButton>("ВНИЗ", &mw, panel_ptz.get_pos_f(1,2), btn_down_callback, this);
    btn_down->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
    rotate(icon_direction, icon_direction, ROTATE_90_CLOCKWISE);
    btn_down->setIcon(icon_direction);
    btn_down->setShowType(1);

    /// Панель управления зумом и скоростью
    Rect rct_ptz_zoom_speed_control = Rect(panel_ptz_blocks.get_pos_pix(1,0, frame_mainWindow.size()).x,
                                           panel_ptz_blocks.get_pos_pix(1,0, frame_mainWindow.size()).y,
                                           panel_ptz_blocks.get_pos_pix(1,0, frame_mainWindow.size()).width * 2,
                                           panel_ptz_blocks.get_pos_pix(1,0, frame_mainWindow.size()).height
                                           );
    Panel panel_ptz_zoom_speed_control(rct_pix2f(rct_ptz_zoom_speed_control, frame_mainWindow.size()), 1, 5, 4, frame_mainWindow.size());

    Panel panel_trac_size(panel_ptz_zoom_speed_control.get_pos_f(0,0), 8, 1, 2, frame_mainWindow.size());
    zahvat_size = frame_show_device->getZahvatSize();
    Rect2f rct_buf = Rect2f( panel_trac_size.get_pos_f(1,0).x,  panel_trac_size.get_pos_f(1,0).y,  3.f *panel_trac_size.get_pos_f(0,0).width,  panel_trac_size.get_pos_f(0,0).height);
    lbl_trac_size = make_shared<CVWidget>("   РАМКА:", &mw, rct_buf);
    rct_buf = Rect2f( panel_trac_size.get_pos_f(4,0).x,  panel_trac_size.get_pos_f(4,0).y,  2.f *panel_trac_size.get_pos_f(4,0).width,  panel_trac_size.get_pos_f(4,0).height);
    lbl_trac_size_value = make_shared<CVLabel>(" ", &mw, rct_buf);
    string trac_size_value_str = to_string(zahvat_size);
    lbl_trac_size_value->setText(trac_size_value_str, frame_mainWindow);
    btn_trac_size_down = make_shared<CVButton>(" - ", &mw, panel_trac_size.get_pos_f(6,0), btn_trac_size_down_callback, this);
    btn_trac_size_up = make_shared<CVButton>(" + ", &mw, panel_trac_size.get_pos_f(7,0), btn_trac_size_up_callback, this);

    slider_ptz_speed = make_shared<CVSlider>("ptz_speed", &mw, panel_ptz_zoom_speed_control.get_pos_f(0,3));
    slider_ptz_speed->add_slot(&slider_ptz_speed_callback, this);
    slider_ptz_speed->setInterval(0.f, 500.f);
    slider_ptz_speed->setStep(50.f);
    slider_ptz_speed->setPos(ptz_speed);

    Panel panel_ptz_speed(panel_ptz_zoom_speed_control.get_pos_f(0,4), 8, 1, 2, frame_mainWindow.size());
    rct_buf = Rect2f( panel_ptz_speed.get_pos_f(1,0).x,
                      panel_ptz_speed.get_pos_f(1,0).y,
                      3.f *panel_ptz_speed.get_pos_f(1,0).width,
                      panel_ptz_speed.get_pos_f(1,0).height);
    lbl_ptz_speed = make_shared<CVWidget>("СКОРОСТЬ:", &mw, rct_buf);
    rct_buf = Rect2f( panel_ptz_speed.get_pos_f(4,0).x,
                      panel_ptz_speed.get_pos_f(4,0).y,
                      4.f *panel_ptz_speed.get_pos_f(4,0).width,
                      panel_ptz_speed.get_pos_f(4,0).height);
    lbl_ptz_speed_value = make_shared<CVLabel>(to_string(ptz_speed) + "/" + to_string(slider_ptz_speed->getMax()), &mw, rct_buf);

    Panel panel_ptz_zoom(panel_ptz_zoom_speed_control.get_pos_f(0,1), 8, 1, 2, frame_mainWindow.size());
    rct_buf = Rect2f( panel_ptz_zoom.get_pos_f(1,0).x,
                      panel_ptz_zoom.get_pos_f(1,0).y,
                      3.f *panel_ptz_zoom.get_pos_f(1,0).width,
                      panel_ptz_zoom.get_pos_f(1,0).height);
    lbl_zoom = make_shared<CVWidget>("     ЗУМ:", &mw, rct_buf);
    rct_buf = Rect2f( panel_ptz_zoom.get_pos_f(4,0).x,
                      panel_ptz_zoom.get_pos_f(4,0).y,
                      2.f * panel_ptz_zoom.get_pos_f(4,0).width,
                      panel_ptz_zoom.get_pos_f(4,0).height);
    lbl_zoom_ratio = make_shared<CVLabel>(" ", &mw, rct_buf);
    btn_zoom_down = make_shared<CVButton>(" - ", &mw, panel_ptz_zoom.get_pos_f(6,0), btn_zoom_down_callback, this);
    btn_zoom_up = make_shared<CVButton>(" + ", &mw, panel_ptz_zoom.get_pos_f(7,0), btn_zoom_up_callback, this);

    Panel panel_set_angle(rct_pix2f(Rect(pt_markup, Size(panel0.get_pos_pix(0,0,frame_mainWindow.size()).width, 2 * lbl_height * frame_mainWindow.rows)), frame_mainWindow.size()),
                          1, 2, 2, frame_mainWindow.size());
    pt_markup += Point(0, 2 * lbl_height * frame_mainWindow.rows);

    // SET AZIMUTH PANEL::
    Panel panel_set_azimuth0(panel_set_angle.get_pos_f(0,0), 16, 1, 2, frame_mainWindow.size());
    //    panel_set_azimuth0.show(frame_mainWindow);
    float w_p = panel_set_azimuth0.get_pos_f(0,0).width  + (float)panel_set_azimuth0.bord * mw_w_1;
    float h_p = panel_set_azimuth0.get_pos_f(0,0).height ;

    lbl_set_azimuth = make_shared<CVWidget>(" АЗ:", &mw, panel_set_azimuth0.get_pos_f(0,0));
    rct_buf = Rect2f(panel_set_azimuth0.get_pos_f(1,0).tl(), Size2f(w_p * 2, h_p));
    lbl_set_azimuth_now = make_shared<CVLabel>(" ", &mw, rct_buf);

    rct_buf = Rect2f(panel_set_azimuth0.get_pos_f(3,0).tl(), Size2f(w_p * 10, h_p));
    slider_edit_azimuth = make_shared<CVSlider>("azimuth", &mw, rct_buf);
    slider_edit_azimuth->add_slot(slider_set_angle_callback, this);
    slider_edit_azimuth->setStep(10);
    slider_edit_azimuth->setInterval(min_angle_azimuth, max_angle_azimuth);
    slider_edit_azimuth->setPos(0);
    string max_angle_azimuth_str = to_string(max_angle_azimuth);
    string min_angle_azimuth_str = "-" + to_string(max_angle_azimuth);
    btn_azimuth_down = make_shared<CVButton>(" - ", &mw, panel_set_azimuth0.get_pos_f(13,0), btn_azimuth_down_callback, this);
    btn_azimuth_up = make_shared<CVButton>(" + ", &mw, panel_set_azimuth0.get_pos_f(14,0), btn_azimuth_up_callback, this);

    // SET PITCH PANEL::
    Panel panel_set_pitch0(panel_set_angle.get_pos_f(0,1), 16, 1, 2, frame_mainWindow.size());
    rct_buf = Rect2f(panel_set_pitch0.get_pos_f(3,0).tl(), Size2f(w_p * 10, h_p));

    lbl_set_pitch = make_shared<CVWidget>("ТАН:", &mw, panel_set_pitch0.get_pos_f(0,0));
    rct_buf = Rect2f(panel_set_pitch0.get_pos_f(1,0).tl(), Size2f(w_p * 2, h_p));
    lbl_set_pitch_now = make_shared<CVLabel>(" ", &mw, rct_buf);

    rct_buf = Rect2f(panel_set_pitch0.get_pos_f(3,0).tl(), Size2f(w_p * 10, h_p));
    slider_edit_pitch = make_shared<CVSlider>("pitch", &mw, rct_buf);
    slider_edit_pitch->add_slot(slider_set_angle_callback, this);
    slider_edit_pitch->setStep(10);
    slider_edit_pitch->setInterval(min_angle_pitch, max_angle_pitch);
    slider_edit_pitch->setPos(0);
    string max_angle_pitch_str = to_string(max_angle_pitch);
    string min_angle_pitch_str = "-" + to_string(max_angle_pitch);
    btn_pitch_down = make_shared<CVButton>(" - ", &mw, panel_set_pitch0.get_pos_f(13,0), btn_pitch_down_callback, this);
    btn_pitch_up = make_shared<CVButton>(" + ", &mw, panel_set_pitch0.get_pos_f(14,0), btn_pitch_up_callback, this);

    rct_buf = Rect2f(panel_set_azimuth0.get_pos_f(15,0).tl(), Size2f(w_p, 2.f * (h_p + 2 * (float)panel_set_azimuth0.bord * mw_h_1)));
    btn_set_angle = make_shared<CVButton>("У\nС\nТ", &mw, rct_buf, btn_set_angle_callback, this);
    slider_set_angle_callback(this);

    line(frame_mainWindow, pt_markup, pt_markup + Point(panel0.main_rct.width * frame_mainWindow.cols, 0), btn_clr_base_mouse_in, 2);

    Panel panel_state_control(rct_pix2f(Rect(pt_markup, Size(panel0.get_pos_pix(0,0,frame_mainWindow.size()).width, 2 * lbl_height * frame_mainWindow.rows)), frame_mainWindow.size()), 3, 2, 4, frame_mainWindow.size());
    //    btn_image_enhancement = make_shared<CVButton>("контраст", &mw, panel_state_control.get_pos_f(3,1), btn_image_enhancement_callback, this);
    //    btn_infrared_inversion = make_shared<CVButton>("инверсия ТПВ", &mw, panel_state_control.get_pos_f(2,1), btn_infrared_inversion_callback, this);
    btn_abs_stab = make_shared<CVButton>("СТАБ. АБС.", &mw, panel_state_control.get_pos_f(0,0),btn_abs_stab_callback, this);
    btn_ang_stab = make_shared<CVButton>("СТАБ. ПО ОСН.", &mw, panel_state_control.get_pos_f(1,0), btn_ang_stab_callback, this);
    btn_rotaty_platform = make_shared<CVButton>("ПОВ. ПЛАТФ.", &mw, panel_state_control.get_pos_f(0,1), btn_rotaty_platform_callback, this);
    btn_motor_off = make_shared<CVButton>("ВЫКЛ. ДВИГ.", &mw, panel_state_control.get_pos_f(1,1),btn_motor_off_callback, this);

    btn_exit_programm = make_shared<CVButton>("ВЫХОД", &mw, panel_state_control.get_pos_f(2,1),btn_exit_programm_callback, this);
    btn_exit_programm->set_clr_base({190,190,250}, {220,220,250});

    pt_markup += Point(0, 2 * lbl_height * frame_mainWindow.rows);
    line(frame_mainWindow, pt_markup, pt_markup + Point(panel0.main_rct.width * frame_mainWindow.cols, 0), btn_clr_base_mouse_in, 2);

    line(frame_mainWindow, pt_markup, pt_markup + Point(panel0.main_rct.width * frame_mainWindow.cols, 0), btn_clr_base_mouse_in, 2);
    lbl_height = 40.f * mw_h_1 ;
    /// Панель отображения входящей телеметрии
    Panel panel_telemetry(rct_pix2f(Rect(pt_markup, Size(panel0.get_pos_pix(0,0,frame_mainWindow.size()).width, 8 * lbl_height * frame_mainWindow.rows)), frame_mainWindow.size()), 8, 8, 4, frame_mainWindow.size());
    lbl_target_miss = make_shared<CVWidget>("ЦЕЛЬ:", &mw, panel_telemetry.get_pos_f(0,0));
    lbl_tmy = make_shared<CVWidget>("Y:", &mw, panel_telemetry.get_pos_f(1,1));
    lbl_tmx = make_shared<CVWidget>("X:", &mw, panel_telemetry.get_pos_f(1,0));
    lbl_show_tmy = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(2,1));
    lbl_show_tmx = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(2,0));
    lbl_deg0 = make_shared<CVWidget>("[град]", &mw, panel_telemetry.get_pos_f(3,0));
    lbl_deg1 = make_shared<CVWidget>("[град]", &mw, panel_telemetry.get_pos_f(3,1));

    wid_abs_angles = make_shared<CVWidget>("ОСН.", &mw, panel_telemetry.get_pos_f(0,2));
    wid_16_17_abs_roll = make_shared<CVWidget>("КРЕН", &mw, panel_telemetry.get_pos_f(1,2));
    wid_18_19_abs_pitch = make_shared<CVWidget>("ТАНГАЖ", &mw, panel_telemetry.get_pos_f(1,3));
    wid_29_30_abs_azimuth = make_shared<CVWidget>("АЗИМУТ", &mw, panel_telemetry.get_pos_f(1,4));
    lbl_16_17_abs_roll = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(2,2));
    lbl_18_19_abs_pitch = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(2,3));
    lbl_29_30_abs_azimuth = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(2,4));
    lbl_deg_8 =  make_shared<CVWidget>("[град]", &mw, panel_telemetry.get_pos_f(3,2));
    lbl_deg_9 =  make_shared<CVWidget>("[град]", &mw, panel_telemetry.get_pos_f(3,3));
    lbl_deg_10 =  make_shared<CVWidget>("[град]", &mw, panel_telemetry.get_pos_f(3,4));

    //    lbl_frame_angle = make_shared<CVWidget>("ПУСТО:", &mw, panel_telemetry.get_pos_f(0,5));

    //    lbl_roll0 = make_shared<CVWidget>("КРЕН:", &mw, panel_telemetry.get_pos_f(1,5));
    //    lbl_roll_show = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(2,5));
    //    lbl_deg2 = make_shared<CVWidget>("[град]", &mw, panel_telemetry.get_pos_f(3,5));
    //    wid_las_ranging = make_shared<CVWidget>("ЛАЗЕР", &mw, panel_telemetry.get_pos_f(1,6));
    //    lbl_las_ranging = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(2,6));
    //    wid_01m = make_shared<CVWidget>("[м]", &mw, panel_telemetry.get_pos_f(3,6));

    wid_tracked_video_source = make_shared<CVWidget>("канал:", &mw, panel_telemetry.get_pos_f(4,0));
    lbl_tracked_video_source = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(5,0));

    wid_tracking_algorithm_type = make_shared<CVWidget>("алгоритм", &mw, panel_telemetry.get_pos_f(4,1));
    lbl_tracking_algorithm_type = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(5,1));

    wid_target_automatic_prompt = make_shared<CVWidget>("автозахват", &mw, panel_telemetry.get_pos_f(4,2));
    lbl_target_automatic_prompt = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(5,2));

    wid_target_tracking_status = make_shared<CVWidget>("трекер", &mw, panel_telemetry.get_pos_f(4,3));
    lbl_target_tracking_status = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(5,3));

    // status information feedback 2
    wid_image_enchancement = make_shared<CVWidget>("контраст", &mw, panel_telemetry.get_pos_f(4,4));
    lbl_image_enchancement = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(5,4));

    wid_storage = make_shared<CVWidget>("запись", &mw, panel_telemetry.get_pos_f(4,5));
    lbl_storage = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(5,5));

    wid_motor_status = make_shared<CVWidget>("двигатель", &mw, panel_telemetry.get_pos_f(4,6));
    lbl_motor_status = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(5,6));

    wid_follow_mode = make_shared<CVWidget>("сопр. азимута", &mw, panel_telemetry.get_pos_f(4,7));
    lbl_follow_mode = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(5,7));

    wid_electric_lock_mode = make_shared<CVWidget>("блок", &mw, panel_telemetry.get_pos_f(6,0));
    lbl_electric_lock_mode = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(7,0));

    wid_laser_status = make_shared<CVWidget>("дальномер", &mw, panel_telemetry.get_pos_f(6,1));
    lbl_laser_status = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(7,1));

    // status information feedback 3
    wid_large_screen_displayed = make_shared<CVWidget>("экран 1", &mw, panel_telemetry.get_pos_f(6,2));
    lbl_large_screen_displayed = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(7,2));

    wid_small_screen_displayed = make_shared<CVWidget>("экран 2", &mw, panel_telemetry.get_pos_f(6,3));
    lbl_small_screen_displayed = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(7,3));

    // self inspection result
    wid_imaging_plate = make_shared<CVWidget>("изображение", &mw, panel_telemetry.get_pos_f(6,4));
    lbl_imaging_plate = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(7,4));

    wid_encoder_and_servo_drive = make_shared<CVWidget>("управление", &mw, panel_telemetry.get_pos_f(6,5));
    lbl_encoder_and_servo_drive = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(7,5));

    wid_gyroscope_calibration = make_shared<CVWidget>("калибровка", &mw, panel_telemetry.get_pos_f(6,6));
    lbl_gyroscope_calibration = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(7,6));

    wid_self_inspection_complete = make_shared<CVWidget>("проверка", &mw, panel_telemetry.get_pos_f(6,7));
    lbl_self_inspection_completed = make_shared<CVLabel>(" ", &mw, panel_telemetry.get_pos_f(7,7));

    pt_markup += Point(0, 8 * lbl_height * frame_mainWindow.rows);
    line(frame_mainWindow, pt_markup, pt_markup + Point(panel0.main_rct.width * frame_mainWindow.cols,0), btn_clr_base_mouse_in, 1);
    line(frame_mainWindow,
         panel_telemetry.get_pos_pix(4,0, frame_mainWindow.size()).tl() - Point(2,2),
         panel_telemetry.get_pos_pix(4,7, frame_mainWindow.size()).tl() + Point(-2, lbl_height * frame_mainWindow.rows - 4),
         btn_clr_base_mouse_in, 1);

    rct_buf = rct_pix2f(Rect(pt_markup, Size(panel0.get_pos_pix(0,0,frame_mainWindow.size()).width, 12 * lbl_height * frame_mainWindow.rows)), frame_mainWindow.size());
    Panel panel_horizontal(rct_buf, 1, 1, 4, frame_mainWindow.size());
    rct_ruler = panel_horizontal.get_pos_pix(0,0,frame_mainWindow.size());

    fill_vector(rct_buf.height * frame_mainWindow.rows, rct_buf.width * frame_mainWindow.cols);
    wid_draw_ruler = make_shared<CVWidget>("ruler", &mw, panel_horizontal.get_pos_f(0,0));
    wid_draw_ruler->set_clr_base(Scalar(100,80,80), btn_clr_active);
    wid_draw_ruler->setShowType(2);
    draw_ruler_mat = cv::Mat(rct_ruler.size(), CV_8UC3, Scalar(0,0,0));// frame_mainWindow(rct_ruler);
    wid_draw_ruler->setImage(draw_ruler_mat);
    wid_draw_ruler_callback(this);
    //    panel_horizontal.show(frame_mainWindow);


    /// MODE_CHANGER
    rct_buf = rct_pix2f(Rect(
                            0.5 * (rct_ruler.x + rct_ruler.br().x),
                            rct_ruler.br().y ,
                            0.5 * rct_ruler.width,
                            3 * lbl_height * frame_mainWindow.rows
                            ), frame_mainWindow.size());
    Panel panel_mode_change(rct_buf, 2, 3, 4, frame_mainWindow.size());
    //    panel_mode_change.show(frame_mainWindow);
    //    lbl_height = 48.f * mw_h_1;

    Panel panel_tm_xy_modes(panel_mode_change.get_pos_f(1,0), 2, 1, 4, frame_mainWindow.size());
    //    panel_tm_xy_modes.show(frame_mainWindow);
    //        wid_tm_xy_mode = make_shared<CVWidget>("ОТКЛОНЕНИЕ ТРЕКА", &mw,  panel_mode_change.get_pos_f(0,0));
    //        btn_tm_xy_mode_off = make_shared<CVButton>("ВЫКЛ.", &mw, panel_tm_xy_modes.get_pos_f(0,0), btn_tm_xy_mode_off_callback, this);
    //        btn_tm_xy_mode_off->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
    //        btn_tm_xy_mode_on = make_shared<CVButton>("ВКЛ.", &mw, panel_tm_xy_modes.get_pos_f(1,0), btn_tm_xy_mode_on_callback, this);

    Panel panel_angular_velocity_modes(panel_mode_change.get_pos_f(1,1), 2, 1, 4, frame_mainWindow.size());
    //    panel_angular_velocity_modes.show(frame_mainWindow);
    wid_angular_velocity_mode_change = make_shared<CVWidget>("УГЛОВЫЕ СКОРОСТИ", &mw,  panel_mode_change.get_pos_f(0,1));
    btn_angular_velocity_mode_off = make_shared<CVButton>("ВЫКЛ.", &mw, panel_angular_velocity_modes.get_pos_f(0,0), btn_angular_velocity_mode_off_callback, this);
    btn_angular_velocity_mode_on = make_shared<CVButton>("ВКЛ.", &mw, panel_angular_velocity_modes.get_pos_f(1,0), btn_angular_velocity_mode_on_callback, this);
    btn_angular_velocity_mode_on->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);

    Panel panel_abs_angle_modes(panel_mode_change.get_pos_f(1,2), 2, 1, 4, frame_mainWindow.size());
    //    panel_abs_angle_modes.show(frame_mainWindow);
    wid_abs_angle_mode_change = make_shared<CVWidget>("АБСОЛЮТНЫЕ УГЛЫ", &mw,  panel_mode_change.get_pos_f(0,2));
    btn_abs_angle_mode_off = make_shared<CVButton>("ВЫКЛ.", &mw, panel_abs_angle_modes.get_pos_f(0,0), btn_abs_angle_mode_off_callback, this);
    btn_abs_angle_mode_complex = make_shared<CVButton>("ВКЛ.", &mw, panel_abs_angle_modes.get_pos_f(1,0), btn_abs_angle_mode_complex_callback, this);
    btn_abs_angle_mode_complex->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);




    pt_markup += Point(0, 0.5 * lbl_height * frame_mainWindow.rows);

    pt_markup = device_panel.get_pos_pix(0,0,frame_mainWindow.size()).tl() +  Point(0, lbl_height + device_panel.get_pos_pix(0,0,frame_mainWindow.size()).height + round(0.5 * lbl_height * frame_mainWindow.rows));

    Rect2f rct_bttm_tlm = Rect2f(pt_markup.x * mw_w_1,
                                 pt_markup.y * mw_h_1,
                                 device_panel.main_rct.width  /*+  panel0.main_rct.width*/,
                                 lbl_height);
    cout << "rct_bttm_tlm = " << rct_bttm_tlm << endl;
    Panel panel_bottom_tlm(rct_bttm_tlm,18, 1, 4, frame_mainWindow.size());
    //    panel_bottom_tlm.show(frame_mainWindow);
    /// dbg::
    wid_motor_angles = make_shared<CVWidget>("ШАР:", &mw, panel_bottom_tlm.get_pos_f(0,0));

    lbl_pitch0 = make_shared<CVWidget>("ТАНГАЖ:", &mw, panel_bottom_tlm.get_pos_f(1,0));
    lbl_pitch_show = make_shared<CVLabel>(" ", &mw, panel_bottom_tlm.get_pos_f(2,0));
    lbl_deg3 = make_shared<CVWidget>("[град]", &mw, panel_bottom_tlm.get_pos_f(3,0));

    lbl_azimuth0 = make_shared<CVWidget>("АЗИМУТ:", &mw, panel_bottom_tlm.get_pos_f(4,0));
    lbl_azimuth_show = make_shared<CVLabel>(" ", &mw, panel_bottom_tlm.get_pos_f(5,0));
    lbl_deg4 = make_shared<CVWidget>("[град]", &mw, panel_bottom_tlm.get_pos_f(6,0));

    lbl_av_roll0 = make_shared<CVWidget>("Wкрен:", &mw, panel_bottom_tlm.get_pos_f(7,0));
    lbl_av_roll_show = make_shared<CVLabel>(" ", &mw, panel_bottom_tlm.get_pos_f(8,0));
    lbl_deg5 = make_shared<CVWidget>("[град/сек]", &mw, panel_bottom_tlm.get_pos_f(9,0));

    lbl_av_pitch0 = make_shared<CVWidget>("Wтангаж:", &mw, panel_bottom_tlm.get_pos_f(10,0));
    lbl_av_pitch_show = make_shared<CVLabel>(" ", &mw, panel_bottom_tlm.get_pos_f(11,0));
    lbl_deg6 = make_shared<CVWidget>("[град/сек]", &mw, panel_bottom_tlm.get_pos_f(12,0));

    lbl_av_azimuth0 = make_shared<CVWidget>("Wазимут:", &mw, panel_bottom_tlm.get_pos_f(13,0));
    lbl_av_azimuth_show = make_shared<CVLabel>(" ", &mw, panel_bottom_tlm.get_pos_f(14,0));
    lbl_deg7 = make_shared<CVWidget>("[град/сек]", &mw, panel_bottom_tlm.get_pos_f(15,0));

//    wid_zoom_boet_panel = make_shared<CVWidget>("ЗУМ:", &mw, panel_bottom_tlm.get_pos_f(16,0));
//    lbl_zoom_bot_panel = make_shared<CVLabel>(" ", &mw, panel_bottom_tlm.get_pos_f(17,0));

#ifndef USE_RS232
    thread thr_eth_udp(&MainWindowConstructor::exec_receive_eth, this);
    thr_eth_udp.detach();
    this_thread::sleep_for(chrono::milliseconds(100));
#endif

    line(frame_mainWindow, pt_markup + Point(0, lbl_height * frame_mainWindow.rows), pt_markup + Point(device_panel.main_rct.width * frame_mainWindow.cols, lbl_height * frame_mainWindow.rows), btn_clr_base_mouse_in, 2);

    ///dbg::

    lbl_height = 48.f * mw_h_1;
    pt_markup += Point(0, lbl_height * frame_mainWindow.rows);
    rct_buf = Rect2f(pt_markup.x * mw_w_1,
                     pt_markup.y * mw_h_1,
                     2.f * device_panel.main_rct.width * n3_1  /*+  panel0.main_rct.width*/,
                     5 * lbl_height);
    Panel cmd_AP_panel(rct_buf, 5, 5, 4, frame_mainWindow.size());
    //        cmd_AP_panel.show(frame_mainWindow);
    btn_attack = make_shared<CVButton>("АТАКА", &mw, cmd_AP_panel.get_pos_f(0,0), btn_attack_callback, this);
    btn_attack->set_clr_base({190,190,250}, {220,220,250});

    btn_stop_attack = make_shared<CVButton>("ОТМЕНА АТАКИ", &mw, cmd_AP_panel.get_pos_f(1,0), btn_stop_attack_callback, this);
    btn_attack->set_clr_base({190,190,250}, {220,220,250});


    btn_nuc_handle_control = make_shared<CVButton>("ШТОРКА", &mw, cmd_AP_panel.get_pos_f(0,1), btn_nuc_handle_control_callback, this);
    //    btn_attack->set_clr_base({190,190,250}, {220,220,250});
    btn_update_null = make_shared<CVButton>("УСТ. НУЛЯ ШАР.", &mw, cmd_AP_panel.get_pos_f(0,2),btn_update_null_callback, this);

    btn_calib_i2c_zero = make_shared<CVButton>("УСТ. НУЛЯ ОСН.", &mw, cmd_AP_panel.get_pos_f(0,3), btn_calib_i2c_zero_callback, this);


//    btn_specify_attitude_angle = make_shared<CVButton>("SPECIFY_ATTITUDE", &mw, cmd_AP_panel.get_pos_f(0,3), btn_specify_attitude_angle_callback, this);
//    btn_calib_zero_pos_fc_att = make_shared<CVButton>("CALIB_ZERO_POS", &mw, cmd_AP_panel.get_pos_f(0,4), btn_calib_zero_pos_fc_att_callback, this);




    /// Дополнительные данные девайса
    pt_markup += Point(device_panel.main_rct.width * 2.f * n3_1 * frame_mainWindow.cols, 0);
    // circle(frame_mainWindow, pt_markup, 3, Scalar(0,0,255), -1);
    rct_buf = Rect2f(pt_markup.x * mw_w_1,
                     pt_markup.y * mw_h_1,
                     device_panel.main_rct.width * n3_1  /*+  panel0.main_rct.width*/,
                     3 * lbl_height);

    Panel supress_gyro_drift_panel(rct_buf, 8, 3, 4, frame_mainWindow.size());
    //    device_info_panel.show(frame_mainWindow);

    // VAR 1
    float w_slot = supress_gyro_drift_panel.get_pos_f(1,0).width;
    float h_slot = supress_gyro_drift_panel.get_pos_f(1,0).height;

    wid_azimuth_drift_val = make_shared<CVWidget>("X:", &mw, supress_gyro_drift_panel.get_pos_f(0,0));
    rct_buf = {supress_gyro_drift_panel.get_pos_f(1,0).x,
               supress_gyro_drift_panel.get_pos_f(1,0).y,
               4 * (w_slot + supress_gyro_drift_panel.bord * mw_w_1),
               h_slot};
    slider_azimuth_drift = make_shared<CVSlider>("azimuth drift", &mw, rct_buf);
    slider_azimuth_drift->add_slot(slider_drift_callback, this);
    slider_azimuth_drift->setStep(20);
    slider_azimuth_drift->setInterval(-200, 200);
    slider_azimuth_drift->setPos(0);
    lbl_azimuth_drift_val = make_shared<CVLabel>(to_string(slider_azimuth_drift->getPos()), &mw,  supress_gyro_drift_panel.get_pos_f(5,0));

    wid_pitch_drift_val = make_shared<CVWidget>("Y:", &mw, supress_gyro_drift_panel.get_pos_f(0,1));
    rct_buf = {supress_gyro_drift_panel.get_pos_f(1,1).x,
               supress_gyro_drift_panel.get_pos_f(1,1).y,
               4 * (w_slot + supress_gyro_drift_panel.bord * mw_w_1),
               h_slot};
    slider_pitch_drift = make_shared<CVSlider>("pitch drift", &mw, rct_buf);
    slider_pitch_drift->add_slot(slider_drift_callback, this);
    slider_pitch_drift->setStep(20);
    slider_pitch_drift->setInterval(-200, 200);
    slider_pitch_drift->setPos(0);
    lbl_pitch_drift_val = make_shared<CVLabel>(to_string(slider_pitch_drift->getPos()), &mw,  supress_gyro_drift_panel.get_pos_f(5,1));

    rct_buf = {supress_gyro_drift_panel.get_pos_f(6,0).x,
               supress_gyro_drift_panel.get_pos_f(6,0).y,
               2 * (w_slot + supress_gyro_drift_panel.bord * mw_w_1),
               3 * (h_slot + supress_gyro_drift_panel.bord * mw_h_1)};
    Panel panel_drift_control(rct_buf, 3, 3, 2, frame_mainWindow.size());
    Rect rct_show = rct_f2pix(panel_drift_control.main_rct, frame_mainWindow.size());
    rectangle(frame_mainWindow, rct_show, btn_clr_base_mouse_in, 2);
    float lbl_height2width = lbl_height * frame_mainWindow.rows * mw_w_1;
    btn_suppress_gyro_drift_azimuth_down = make_shared<CVButton>("-", &mw, panel_drift_control.get_pos_f(0,1), btn_suppress_gyro_drift_azimuth_down_callback, this);
    btn_suppress_gyro_drift_azimuth_up = make_shared<CVButton>("+", &mw, panel_drift_control.get_pos_f(2,1), btn_suppress_gyro_drift_azimuth_up_callback, this);
    btn_suppress_gyro_drift_pitch_down = make_shared<CVButton>("-", &mw, panel_drift_control.get_pos_f(1,2), btn_suppress_gyro_drift_pitch_down_callback, this);
    btn_suppress_gyro_drift_pitch_up = make_shared<CVButton>("+", &mw, panel_drift_control.get_pos_f(1,0), btn_suppress_gyro_drift_pitch_up_callback, this);

    rct_buf = {supress_gyro_drift_panel.get_pos_f(1,2).x,
               supress_gyro_drift_panel.get_pos_f(1,2).y,
               4 * (w_slot + supress_gyro_drift_panel.bord * mw_w_1),
               h_slot};
    btn_suppress_gyro_drift = make_shared<CVButton>("ДРЕЙФ", &mw, rct_buf, btn_suppress_gyro_drift_callback, this);

    ///     pt_markup -= Point(device_panel.main_rct.width * 1.f * n3_1 * frame_mainWindow.cols, 0);
    ///     поле, расположенное левее окна компенсации дрейфа
    /// I2C

    //    pt_markup += Point(0, supress_gyro_drift_panel.bord_y_h * frame_mainWindow.rows);
    rct_buf = Rect2f(pt_markup.x * mw_w_1 - device_panel.main_rct.width * n3_1,
                     pt_markup.y * mw_h_1,
                     device_panel.main_rct.width * n3_1  /*+  panel0.main_rct.width*/,
                     4 * lbl_height);
    Panel calib_i2c_zero_panel(rct_buf, 8, 4, 4, frame_mainWindow.size());


    wid_roll_i2c_val = make_shared<CVWidget>("КРЕН:", &mw, calib_i2c_zero_panel.get_pos_f(0,0));
    rct_buf = {calib_i2c_zero_panel.get_pos_f(1,0).x,
               calib_i2c_zero_panel.get_pos_f(1,0).y,
               4 * (w_slot + calib_i2c_zero_panel.bord * mw_w_1),
               h_slot};
    slider_roll_i2c = make_shared<CVSlider>("roll i2c", &mw, rct_buf);
    slider_roll_i2c->add_slot(slider_i2c_callback, this);
    slider_roll_i2c->setStep(200);
    slider_roll_i2c->setInterval(-2000, 2000);
    slider_roll_i2c->setPos(0);
    lbl_roll_i2c_val = make_shared<CVLabel>(to_string(slider_roll_i2c->getPos()), &mw,  calib_i2c_zero_panel.get_pos_f(5,0));

    wid_pitch_i2c_val = make_shared<CVWidget>("ТАНГАЖ:", &mw, calib_i2c_zero_panel.get_pos_f(0,1));
    rct_buf = {calib_i2c_zero_panel.get_pos_f(1,1).x,
               calib_i2c_zero_panel.get_pos_f(1,1).y,
               4 * (w_slot + calib_i2c_zero_panel.bord * mw_w_1),
               h_slot};
    slider_pitch_i2c = make_shared<CVSlider>("pitch i2c", &mw, rct_buf);
    slider_pitch_i2c->add_slot(slider_i2c_callback, this);
    slider_pitch_i2c->setStep(200);
    slider_pitch_i2c->setInterval(-2000, 2000);
    slider_pitch_i2c->setPos(0);
    lbl_pitch_i2c_val = make_shared<CVLabel>(to_string(slider_pitch_i2c->getPos()), &mw,  calib_i2c_zero_panel.get_pos_f(5,1));

    wid_azimuth_i2c_val = make_shared<CVWidget>("АЗИМУТ:", &mw, calib_i2c_zero_panel.get_pos_f(0,2));
    rct_buf = {calib_i2c_zero_panel.get_pos_f(1,2).x,
               calib_i2c_zero_panel.get_pos_f(1,2).y,
               4 * (w_slot + calib_i2c_zero_panel.bord * mw_w_1),
               h_slot};
    slider_azimuth_i2c = make_shared<CVSlider>("azimuth i2c", &mw, rct_buf);
    slider_azimuth_i2c->add_slot(slider_i2c_callback, this);
    slider_azimuth_i2c->setStep(200);
    slider_azimuth_i2c->setInterval(-2000, 2000);
    slider_azimuth_i2c->setPos(0);
    lbl_azimuth_i2c_val = make_shared<CVLabel>(to_string(slider_azimuth_i2c->getPos()), &mw,  calib_i2c_zero_panel.get_pos_f(5,2));


    rct_buf = {calib_i2c_zero_panel.get_pos_f(6,0).x,
               calib_i2c_zero_panel.get_pos_f(6,0).y,
               4 / 3.f * (w_slot + calib_i2c_zero_panel.bord * mw_w_1),
               3 * (h_slot + calib_i2c_zero_panel.bord * mw_h_1)};
    Panel panel_i2c_control(rct_buf, 2, 3, 2, frame_mainWindow.size());
    rct_show = rct_f2pix(panel_i2c_control.main_rct, frame_mainWindow.size());
    rectangle(frame_mainWindow, rct_show, btn_clr_base_mouse_in, 2);
    btn_calib_i2c_zero_roll_down = make_shared<CVButton>("-", &mw, panel_i2c_control.get_pos_f(0,0), btn_calib_i2c_zero_roll_down_callback, this);
    btn_calib_i2c_zero_roll_up = make_shared<CVButton>("+", &mw, panel_i2c_control.get_pos_f(1,0), btn_calib_i2c_zero_roll_up_callback, this);
    btn_calib_i2c_zero_pitch_down = make_shared<CVButton>("-", &mw, panel_i2c_control.get_pos_f(0,1), btn_calib_i2c_zero_pitch_down_callback, this);
    btn_calib_i2c_zero_pitch_up = make_shared<CVButton>("+", &mw, panel_i2c_control.get_pos_f(1,1), btn_calib_i2c_zero_pitch_up_callback, this);
    btn_calib_i2c_zero_azimuth_down = make_shared<CVButton>("-", &mw, panel_i2c_control.get_pos_f(0,2), btn_calib_i2c_zero_azimuth_down_callback, this);
    btn_calib_i2c_zero_azimuth_up = make_shared<CVButton>("+", &mw, panel_i2c_control.get_pos_f(1,2), btn_calib_i2c_zero_azimuth_up_callback, this);

    rct_buf = {calib_i2c_zero_panel.get_pos_f(1,3).x,
               calib_i2c_zero_panel.get_pos_f(1,3).y,
               3 * (w_slot + calib_i2c_zero_panel.bord * mw_w_1),
               h_slot};
    btn_calib_handle_i2c_zero= make_shared<CVButton> ("УСТАНОВИТЬ", &mw, rct_buf, btn_calib_handle_i2c_zero_callback, this);
    rct_buf = {calib_i2c_zero_panel.get_pos_f(4,3).x,
               calib_i2c_zero_panel.get_pos_f(4,3).y,
               1 * (w_slot + calib_i2c_zero_panel.bord * mw_w_1),
               h_slot};

    frame_confirm = cv::Mat(Size(640,480), CV_8UC3, btn_clr_base);
    window_confirm = CVMainWindow(config_path, ok, confirm_str, frame_confirm);
    window_confirm.setWindowConstructor(this);
    //    window_confirm.add_key_handler(key_confirm_handler);
    btn_confirm_yes = make_shared<CVButton>("ДА", &window_confirm, Rect2f(0.1, 0.8, 0.35, 0.1), btn_confirm_yes_callback, this);
    btn_confirm_no = make_shared<CVButton>("НЕТ", &window_confirm, Rect2f(0.55, 0.8, 0.35, 0.1), btn_confirm_no_callback, this);
    lbl_confirm = make_shared<CVLabel>("!!!ВНИМАНИЕ!!!\n"
                                       "Данная команда выключит ГОЭН.\n"
                                       "Выключить?", &window_confirm, Rect2f(0.05, 0.1, 0.9, 0.6));

    //    lbl_video_data = make_shared<CVWidget>("Данные видео:", &mw, device_info_panel.get_pos_f(0,0));
    //    lbl_frame_data = make_shared<CVLabel>(" ", &mw, device_info_panel.get_pos_f(1,0));
    //            lbl_fps = make_shared<CVLabel>(" ", &mw, device_info_panel.get_pos_f(2,0));
    //    cout << "device panel rct = " << device_panel.get_pos_pix(0,0,frame_mainWindow.size()) << endl;
    //        device_info_panel.show(frame_mainWindow);

    /// ::JSON dbg::
    //    rct_buf = Rect2f(device_info_panel.get_pos_f(0,0).x, device_info_panel.get_pos_f(0,0).y, 4.f * device_info_panel.get_pos_f(0,0).width, 1.3 * device_info_panel.get_pos_f(0,0).height);
    //    cmbbx_edit_url = make_shared<CVComboBox>("EDIT URL", &mw, rct_buf, cmbbx_edit_url_slot, this);
    //    std::string filename = "../innnoAirInfoConfig.json";
    //    Json::Value root;
    //    Json::Reader reader;
    //    ifstream test(filename, ifstream::binary);
    //    string cur_line;
    //    bool success = true;
    //    bool parsingSuccessful = reader.parse(test, root, false);

    //    std::vector<string> v_url;
    //    for (const auto v : root["group"])
    //    {
    //        Json::Value member = v;
    //        std::string id = member["id"].asString();
    //        cout << "id: " << id;
    //        std::string url = member["url"].asString();
    //        cout << "; url: " << url << endl;
    //        v_url.emplace_back(url);
    //    } // END  for (const auto v : root["group"])
    //    if(v_url.size())
    //    {
    //        cout << "set_vec = " << v_url.size() << endl;
    //        cmbbx_edit_url->set_vec(v_url);
    //    } // END if(v_com_names.size())
    //    else
    //    {
    //        v_url.emplace_back("Can't find URL in file!!");
    //        cmbbx_edit_url->set_vec(v_url);
    //    } // END if(!v_com_names.size())
    //    cmbbx_edit_url->set_clr_base(btn_clr_base_mouse_in, btn_clr_base + Scalar(15,15,15));
    //    cmbbx_edit_url->set_res(v_url[0]);
    //    cmbbx_edit_url->show(frame_mainWindow);
    cout << "OK MainWindowConstructor!" << endl;
} // END MainWindowConstructor

MainWindowConstructor::~MainWindowConstructor()
{
#ifdef USE_RS232
    try
    {
        rs232_ptr->close();
        rs232_ptr = nullptr;
    } // END try com_port_ptr->Close();
    catch (const exception & e)
    {
        cout << "RS232TransieverLS::close ERROR: " << e.what() << endl;
    } // END catch (const exception & e)
#endif
#ifdef USE_UDP
    eth_udp_ptr->eth_close();
#endif

} // END ~MainWindowConstructor

void MainWindowConstructor::draw_receive_telemetry()
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.roll_frame_angle * 0.01;
    std::string frame_roll_str = stream.str();
    stream.str("");
    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.pitch_frame_angle * 0.01;
    std::string frame_pitch_str = stream.str();
    stream.str("");
    stream.str("");
    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.azimuth_frame_angle * 0.01;
    std::string frame_azimuth_str = stream.str();
    stream.str("");
    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.laser_ranging * 0.1 ;
    std::string laser_ranging_str = stream.str();
    stream.str("");

    //    lbl_roll_show->setText(frame_roll_str, frame_mainWindow);
    lbl_pitch_show->setText(frame_pitch_str, frame_mainWindow);
    lbl_azimuth_show->setText(frame_azimuth_str, frame_mainWindow);
    //    lbl_las_ranging->setText(laser_ranging_str, frame_mainWindow);

    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.roll_angular_velocity * 0.01;
    std::string av_roll_str = stream.str();
    stream.str("");
    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.pitch_angular_velocity * 0.01;
    std::string av_pitch_str = stream.str();
    stream.str("");
    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.azimuth_angular_velocity * 0.01;
    std::string av_azimuth_str = stream.str();
    stream.str("");

    lbl_av_roll_show->setText(av_roll_str, frame_mainWindow);
    lbl_av_pitch_show->setText(av_pitch_str, frame_mainWindow);
    lbl_av_azimuth_show->setText(av_azimuth_str, frame_mainWindow);

    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.target_miss_x * 0.05;
    std::string tm_x_str = stream.str();
    stream.str("");
    stream << std::fixed << std::setprecision(2) << (float)from_goen_telemetry_str.target_miss_y * 0.05;
    std::string tm_y_str = stream.str();
    stream.str("");
    lbl_show_tmx->setText(tm_x_str, frame_mainWindow);
    lbl_show_tmy->setText(tm_y_str, frame_mainWindow);

    // res НЕЗАПРОТОКОЛИРОВАННЫЕ ПОЛЯ
    int16_t res_field = 0;
    memcpy(&res_field, &from_goen_telemetry_str.abs_roll, 2);
    draw_roll = (float)res_field * 0.01;
    stream << std::fixed << std::setprecision(2) << (float)res_field * 0.01;
    std::string abs_roll_str = stream.str();
    stream.str("");

    memcpy(&res_field, &from_goen_telemetry_str.abs_pitch, 2);
    draw_pitch = ((float)res_field) * 0.01; //(float)res_field * 0.01 ;
    stream << std::fixed << std::setprecision(2) << draw_pitch;
    std::string abs_pitch_str = stream.str();
    stream.str("");

    memcpy(&res_field, &from_goen_telemetry_str.abs_azimuth, 2);
    stream << std::fixed << std::setprecision(2) << (float)res_field * 0.01;
    std::string abs_azimuth_str = stream.str();
    stream.str("");

    wid_draw_ruler_callback(this);

    lbl_16_17_abs_roll->setText(abs_roll_str, frame_mainWindow);
    lbl_18_19_abs_pitch->setText(abs_pitch_str, frame_mainWindow);
    lbl_29_30_abs_azimuth->setText(abs_azimuth_str, frame_mainWindow);

    transform_struct2bit.get_goen_status_field(from_goen_telemetry_str, zoom_ratio, sif1_str, sif2_str, sif3_str, sir_str);

    stream << std::fixed << std::setprecision(2) << (float)zoom_ratio * 0.1;
    std::string zoom_ratio_str = stream.str();
    stream.str("");
    lbl_zoom_ratio->setText(zoom_ratio_str, frame_mainWindow);
//    lbl_zoom_bot_panel->setText(zoom_ratio_str, frame_mainWindow);
    frame_show_device->setZoomValue(zoom_ratio * 0.1);
    zoom_now = (float)zoom_ratio * 0.1;
    std::string tracked_video_src_str = " ";

    f_attack = sif2_str.reserved1;
    if(f_attack != f_attack_prev)
    {
        if(f_attack)
        {
            btn_attack->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
        } // END if(f_attack)
        else
        {
            btn_attack->set_clr_base({190,190,250}, {220,220,250});
        } // END if(!f_attack)
        f_attack_prev = f_attack;
    } // END if(f_attack != f_attack_prev)

    if((!sif1_str.tracked_video_source1 && !sif1_str.tracked_video_source2) || (!sif1_str.tracked_video_source1 && sif1_str.tracked_video_source2))
    {
        tracked_video_src_str = "ТВ";
        frame_show_device->SetChannelTV();
        if(channel_tpv != 0 || (zoom_now != zoom_prev))
        {
            zahvat_size = frame_show_device->getZahvatSize();
            frame_show_device->setZahvatSize(zahvat_size);
            string zahvat_size_str = to_string(zahvat_size);
            lbl_trac_size_value->setText(zahvat_size_str, frame_mainWindow);
            btn_TV_TPV->setName(" ТПВ ");
            btn_TV_TPV->add_slot(btn_TPV_callback, this);
        } // END if(channel_tpv != 0)
        channel_tpv = 0;
    } // if(TV)
    if((sif1_str.tracked_video_source1 && !sif1_str.tracked_video_source2) || (sif1_str.tracked_video_source1 && sif1_str.tracked_video_source2))
    {
        tracked_video_src_str = "ТПВ";
        frame_show_device->SetChannelIR();
        if(channel_tpv != 1 || (zoom_now != zoom_prev))
        {
            zahvat_size = frame_show_device->getZahvatSize();
            frame_show_device->setZahvatSize(zahvat_size);
            string zahvat_size_str = to_string(zahvat_size);
            lbl_trac_size_value->setText(zahvat_size_str, frame_mainWindow);
            btn_TV_TPV->setName(" ТВ ");
            btn_TV_TPV->add_slot(btn_TV_callback, this);
        } // END  if(channel_tpv != 1)
        channel_tpv = 1;
    } // if(TPV)
    zoom_prev = zoom_now;
    lbl_tracked_video_source->setText(tracked_video_src_str, frame_mainWindow);
    std::string tracked_algorithm_type_str = " ";
    if(!sif1_str.track_algorith_type1 && !sif1_str.track_algorith_type2) {tracked_algorithm_type_str = "-";}
    if(!sif1_str.track_algorith_type1 && sif1_str.track_algorith_type2) {tracked_algorithm_type_str = "-";}
    if(sif1_str.track_algorith_type1 && !sif1_str.track_algorith_type2) {tracked_algorithm_type_str = "-";}
    if(sif1_str.track_algorith_type1 && sif1_str.track_algorith_type2) {tracked_algorithm_type_str = "-";}
    lbl_tracking_algorithm_type->setText(tracked_algorithm_type_str, frame_mainWindow);

    std::string target_automatic_prompt_str = " ";
    if(sif1_str.target_auto_prompt) {target_automatic_prompt_str = "ВКЛ";} else {target_automatic_prompt_str = "ВЫКЛ";}
    lbl_target_automatic_prompt->setText(target_automatic_prompt_str, frame_mainWindow);

    std::string target_tracking_status_str = " ";
    if(sif1_str.target_tracking_status)
    {
        target_tracking_status_str = "ВКЛ";
        if(channel_tpv)
        {
            frame_show_device->setTracPos2Show((int)round(from_goen_telemetry_str.target_miss_x * k_rel2field_x_tpv_1),
                                               (int)round(from_goen_telemetry_str.target_miss_y * k_rel2field_y_tpv_1));
        } // END if(channel_tpv)
        else
        {
            if(zoom_now < 1)
            {
                frame_show_device->setTracPos2Show((int)round(from_goen_telemetry_str.target_miss_x * k_rel2field_x_tv_1 * 0.5),
                                                   (int)round(from_goen_telemetry_str.target_miss_y * k_rel2field_y_tv_1 * 0.5));
            } // END if(zoom_now < 1)
            else
            {
                frame_show_device->setTracPos2Show((int)round(from_goen_telemetry_str.target_miss_x * k_rel2field_x_tv_1),
                                                   (int)round(from_goen_telemetry_str.target_miss_y * k_rel2field_y_tv_1));
            } // END if(zoom_now >= 1)
        } // END if(!channel_tpv)
    } // END if(sif1_str.target_tracking_status)
    else {target_tracking_status_str = "ВЫКЛ";}
    lbl_target_tracking_status->setText(target_tracking_status_str, frame_mainWindow);

    std::string image_enchancement_str = " ";
    if(sif2_str.image_enchancement)
    {
        if(!f_image_enchance)
        {
            //            btn_image_enhancement->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
            lbl_image_enchancement->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
            f_image_enchance = true;
            image_enchancement_str = "ВКЛ";
        } // END  if(!f_image_enchance)
    } // END if(sif2_str.image_enchancement)
    else
    {
        if(f_image_enchance)
        {
            //            btn_image_enhancement->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
            lbl_image_enchancement->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
            f_image_enchance = true;
            image_enchancement_str = "ВЫКЛ";
        } // END  if(f_image_enchance)
    } // END if(!sif2_str.image_enchancement)
    lbl_image_enchancement->setText(image_enchancement_str, frame_mainWindow);

    std::string storage_str = " ";
    if(sif2_str.storage) {storage_str = "ВКЛ";} else {storage_str = "ВЫКЛ";}
    lbl_storage->setText(storage_str, frame_mainWindow);

    std::string motor_status_str = " ";
    if(sif2_str.motor_status)
    {
        motor_status_str = "ВКЛ";
        if(!f_servo_status)
        {
            f_servo_status = true;
            //            btn_motor_on->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
            //            btn_motor_off->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
        } // END if(!f_servo_status)
    } // END if(sif2_str.motor_status)
    else
    {
        motor_status_str = "ВЫКЛ";
        if(f_servo_status)
        {
            f_servo_status = false;
            //            btn_motor_on->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
            //            btn_motor_off->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
        } // END if(f_servo_status)
    } // END if(!sif2_str.motor_status)
    lbl_motor_status->setText(motor_status_str, frame_mainWindow);

    std::string follow_mode_str = " ";
    if(sif2_str.follow_mode)
    {
        follow_mode_str = "ВКЛ";
        if(!f_azimuth_follow_status  && sif2_str.motor_status)
        {
            f_azimuth_follow_status = true;
            //            btn_follow_start->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
            //            btn_follow_close->set_clr_base(btn_clr_base, btn_clr_base);
        } // END if(!f_azimuth_follow_status)
    } // END if(sif2_str.follow_mode)
    else
    {
        follow_mode_str = "ВЫКЛ";
        if(f_azimuth_follow_status)
        {
            f_azimuth_follow_status = false;
            //            btn_follow_start->set_clr_base(btn_clr_base, btn_clr_base);
            //            btn_follow_close->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
        } // END if(f_azimuth_follow_status)
    } // END if(!sif2_str.follow_mode)
    lbl_follow_mode->setText(follow_mode_str, frame_mainWindow);

    std::string electric_lock_mode_str = " ";
    if(sif2_str.electric_lock_mode)
    {
        electric_lock_mode_str = "ВКЛ";
        if(!f_lock_servo)
        {
            f_lock_servo = true;
            //            btn_lock_mode_on->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
            //            btn_lock_mode_off->set_clr_base(btn_clr_base, btn_clr_base);
        } // END if(!f_lock_servo)
    } // END if(sif2_str.electric_lock_mode)
    else
    {
        electric_lock_mode_str = "ВЫКЛ";
        if(f_lock_servo)
        {
            f_lock_servo = false;
            //            btn_lock_mode_on->set_clr_base(btn_clr_base, btn_clr_base_mouse_in);
            //            btn_lock_mode_off->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);
        } // END if(f_lock_servo)
    } // EDN if(!sif2_str.electric_lock_mode)
    lbl_electric_lock_mode->setText(electric_lock_mode_str, frame_mainWindow);

    if(f_servo_status && !f_lock_servo && !f_azimuth_follow_status) {btn_abs_stab->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);}
    else {btn_abs_stab->set_clr_base(btn_clr_base, btn_clr_base);}
    if(f_servo_status && !f_lock_servo && f_azimuth_follow_status ) {btn_ang_stab->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);}
    else {btn_ang_stab->set_clr_base(btn_clr_base, btn_clr_base);}
    if(f_servo_status && f_lock_servo) {btn_rotaty_platform->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);}
    else {btn_rotaty_platform->set_clr_base(btn_clr_base, btn_clr_base);}
    if(!f_servo_status) {btn_motor_off->set_clr_base(btn_clr_active, btn_clr_active_mouse_in);}
    else {btn_motor_off->set_clr_base(btn_clr_base, btn_clr_base);}

    std::string laser_status_str = " ";
    if(sif2_str.laser_status) {laser_status_str = "ВКЛ";} else {laser_status_str = "ВЫКЛ";}
    lbl_laser_status->setText(laser_status_str, frame_mainWindow);

    std::string large_screen_displayed_str = " ";
    if(!sif3_str.large_screen_displayed0 && !sif3_str.large_screen_displayed1) {large_screen_displayed_str = "ТВ";}
    if(!sif3_str.large_screen_displayed0 && sif3_str.large_screen_displayed1) {large_screen_displayed_str = "ТВ";}
    if(sif3_str.large_screen_displayed0 && !sif3_str.large_screen_displayed1) {large_screen_displayed_str = "ИК";}
    if(sif3_str.large_screen_displayed0 && sif3_str.large_screen_displayed1) {large_screen_displayed_str = "ИК";}
    lbl_large_screen_displayed->setText(large_screen_displayed_str, frame_mainWindow);

    std::string small_screen_displayed_str = " ";
    if(!sif3_str.small_screen_displayed0 && !sif3_str.small_screen_displayed1) {small_screen_displayed_str = "ТВ";}
    if(!sif3_str.small_screen_displayed0 && sif3_str.small_screen_displayed1) {small_screen_displayed_str = "ТВ";}
    if(sif3_str.small_screen_displayed0 && !sif3_str.small_screen_displayed1) {small_screen_displayed_str = "ИК";}
    if(sif3_str.small_screen_displayed0 && sif3_str.small_screen_displayed1) {small_screen_displayed_str = "ИК";}
    lbl_small_screen_displayed->setText(small_screen_displayed_str, frame_mainWindow);

    std::string imaging_plate_str = " ";
    if(sir_str.imaging_plate) {imaging_plate_str = "Ошибка";} else {imaging_plate_str = "Готово";}
    lbl_imaging_plate->setText(imaging_plate_str, frame_mainWindow);

    std::string encoder_and_servo_drive_str = " ";
    if(sir_str.encoder_and_servo_drive) {encoder_and_servo_drive_str = "Ошибка";} else {encoder_and_servo_drive_str = "Готово";}
    lbl_encoder_and_servo_drive->setText(encoder_and_servo_drive_str, frame_mainWindow);

    std::string gyroscope_calibration_str = " ";
    if(sir_str.gyroscope_calibration){gyroscope_calibration_str = "Ошибка";} else{gyroscope_calibration_str = "Завершена";} // END if(sir_str.gyroscope_calibration)
    lbl_gyroscope_calibration->setText(gyroscope_calibration_str, frame_mainWindow);

    std::string self_inspection_completed_str = " ";
    if(sir_str.self_inspection_completed) {self_inspection_completed_str = "Завершена";} else {self_inspection_completed_str = "В процессе";}
    lbl_self_inspection_completed->setText(self_inspection_completed_str, frame_mainWindow);


    /// LOG::
#ifdef USE_LOG_CMD
    std::stringstream full_cmd_str;
    v_tlm_str.resize(13);
    memcpy(buf_tlm, &from_goen_telemetry_str, sizeof(from_goen_telemetry_str));
    for(int i =0; i < sizeof(from_goen_telemetry_str); i++)
    {
        full_cmd_str << hex << uppercase << to_string((int)buf_tlm[i]) + " " << dec;
    } // END for(int i =0; i < sizeof(from_goen_telemetry_str); i ++)
    std::string log_tlm_str =
            "," + full_cmd_str.str() +
            "," + tm_x_str +
            "," + tm_y_str +
            "," + frame_azimuth_str +
            "," + frame_pitch_str +
            "," + abs_azimuth_str +
            "," + abs_pitch_str +
            "," + abs_roll_str +
            "," + av_azimuth_str +
            "," + av_pitch_str +
            "," + av_roll_str +
            "," + zoom_ratio_str;

    v_tlm_str.emplace_back(tm_x_str);
    v_tlm_str.emplace_back(tm_y_str);
    v_tlm_str.emplace_back(frame_azimuth_str);
    v_tlm_str.emplace_back(frame_pitch_str);
    v_tlm_str.emplace_back(abs_azimuth_str);
    v_tlm_str.emplace_back(abs_pitch_str);
    v_tlm_str.emplace_back(abs_roll_str);
    v_tlm_str.emplace_back(av_azimuth_str);
    v_tlm_str.emplace_back(av_pitch_str);
    v_tlm_str.emplace_back(av_roll_str);
    v_tlm_str.emplace_back(zoom_ratio_str);
    log_cmd_ptr->log_(log_tlm_str);
#endif // USE_LOG_CMD
} // -- END void draw_receive_telemetry()

bool MainWindowConstructor::set_cmd(uint8_t control_param, uint16_t parameter_x, uint16_t parameter_y, uint8_t zoom_rate, uint8_t parameter3)
{
    to_goen_cmd_str = ToGoenCommand();
    to_goen_cmd_str.control_param = control_param;
    to_goen_cmd_str.parameter_x = parameter_x;
    to_goen_cmd_str.parameter_y = parameter_y;
    to_goen_cmd_str.zoom_rate = zoom_rate;
    to_goen_cmd_str.parameter3 = parameter3;
    bool ok = false;
    cout << "SEND CMD:" << hex;
    char buf[16];
    memcpy(&buf[0], &to_goen_cmd_str, sizeof(to_goen_cmd_str));
    for(int i = 0; i < sizeof(to_goen_cmd_str); i++)
    {
        cout << " 0x" << uppercase << ((int)buf[i] % 0xFF);
    } // END for(int i = 0; i < sizeof(to_goen_cmd_str); i++)
    cout << dec << endl;
#ifdef USE_RS232
    if(rs232_ptr->isOpen())
    {
        rs232_ptr->set_cmd(to_goen_cmd_str);
        ok = true;
    } // -- END if(rs232_ptr->isOpen())
#endif // END ifdef USE_RS232
#ifdef USE_UDP
    if(eth_udp_ptr->sync.f_send_exec.load())
    {
        eth_udp_ptr->set_cmd(to_goen_cmd_str);
        ok = true;
    }  // END if(eth_udp_ptr->sync.f_send_exec.load())
#endif // #ifdef USE_UDP
    return ok;
}

bool MainWindowConstructor::get_ini_params(const string &config)
{
    bool ok = true;
    INIReader reader(config);
    if(reader.ParseError() < 0)
    {
        cout << "CVMainWindow::Can't load config_path='" << config_path << "'\n";
        return 0;
    } // -- END if(reader.ParseError() < 0)

    open_close = reader.Get("NETWORK", "open_close", "oops");
    if(open_close == "oops")
    {
        cout << "Not found open_close in [recorder]!\n";
        return false;
    } // END if(open_close == "oops")

    return ok;
} // -- END void set_cmd()

#if defined(USE_UDP) && not defined(USE_RS232)
void MainWindowConstructor::exec_receive_eth()
{
    cout << "START exec_receive_eth()" << endl;
    while(eth_udp_ptr->sync.f_recv_exec.load())
    {
        eth_udp_ptr->receive_data();
        if(eth_udp_ptr->sync.f_recv_telemetry.load())
        {
            eth_udp_ptr->get_receive_telemetry(from_goen_telemetry_str);
            set_medium_val_for_tlm();
            draw_receive_telemetry();
        } // END if(eth_udp_ptr->sync.f_recv_telemetry.load())
#ifdef USE_CONFIRMATION
        if(eth_udp_ptr->sync.f_recv_ssr.load())
        {
            //eth_udp_ptr->get_receive_confirmation(ssr_str);
        }  // END if(eth_udp_ptr->sync.f_recv_ssr.load())
#endif
    }  // END while(eth_udp_ptr->sync.f_recv_exec.load())

    std::string telemetry_when_close_str = " ";
    //    lbl_las_ranging ->setText(telemetry_when_close_str, frame_mainWindow);
    //    lbl_roll_show ->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_pitch_show ->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_azimuth_show ->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_av_roll_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_av_pitch_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_av_azimuth_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_show_tmx->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_show_tmy->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_zoom_ratio->setText(telemetry_when_close_str, frame_mainWindow);
//    lbl_zoom_bot_panel->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_tracked_video_source->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_tracking_algorithm_type->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_target_automatic_prompt->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_target_tracking_status->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_image_enchancement->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_storage->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_motor_status->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_follow_mode->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_electric_lock_mode->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_laser_status->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_large_screen_displayed->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_small_screen_displayed->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_imaging_plate->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_encoder_and_servo_drive->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_gyroscope_calibration->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_self_inspection_completed->setText(telemetry_when_close_str, frame_mainWindow);
    // res dbg::
    lbl_16_17_abs_roll->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_18_19_abs_pitch->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_29_30_abs_azimuth->setText(telemetry_when_close_str, frame_mainWindow);

}  // END void exec_receive_eth()
#endif // END defined(USE_UDP) && not defined(USE_RS232)

#ifdef USE_RS232
void MainWindowConstructor::exec_keep_rs232()
{
    cout << "START exec_keep_rs232()" << endl;
#ifdef USE_LOGGER
    auto tp1_draw_rs232 = chrono::high_resolution_clock::now();
    auto tp0_draw_rs232 = chrono::high_resolution_clock::now();
#endif // USE_LOGGER

    while(rs232_ptr->sync.f_keep_exec.load())
    {
        try
        {
            if(rs232_ptr->sync.f_keep_telemetry.load())
            {
                mutex mtx;
                lock_guard lock(mtx);
#ifdef USE_LOGGER
                tp1_draw_rs232 = chrono::high_resolution_clock::now();
                LoggerArtem::inst().logTimedBasedFPS(
                            "RS232_draw_fps = ",
                            chrono::duration<double>(tp1_draw_rs232 - tp0_draw_rs232).count());
                tp0_draw_rs232 = chrono::high_resolution_clock::now();
#endif //USE_LOGGER
                rs232_ptr->get_telemetry(from_goen_telemetry_str);
                mtx.unlock();
                rs232_ptr->sync.f_keep_telemetry.store(false);

                /// УСРЕДНЕНИЕ ПОЛЕЙ ПО ВСЕЙ СТРУКТУРЕ в течение последней секунды, при параметре max_list_size==1 усреднения не произойдёт
                set_medium_val_for_tlm();
                draw_receive_telemetry();

            } // END if(rs232_ptr->sync.f_keep_telemetry.load())
            else
            {
                this_thread::sleep_for(chrono::milliseconds(1));
            } // END else
        } // END try draw! << endl;
        catch (const exception & err)
        {
            cout << "EXCEPTION in DRAW: " << err.what() << endl;
            btn_close_com_callback(this);
        } // END  catch (const exception & err)
    } // END while(rs232_ptr->sync.f_keep_exec.load())
    cout << "END exec KEEP !!!\n" << endl;
    std::string telemetry_when_close_str = " ";
    //    lbl_las_ranging->setText(telemetry_when_close_str, frame_mainWindow);
    //    lbl_roll_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_pitch_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_azimuth_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_av_roll_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_av_pitch_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_av_azimuth_show->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_show_tmx->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_show_tmy->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_zoom_ratio->setText(telemetry_when_close_str, frame_mainWindow);
//    lbl_zoom_bot_panel->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_tracked_video_source->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_tracking_algorithm_type->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_target_automatic_prompt->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_target_tracking_status->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_image_enchancement->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_storage->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_motor_status->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_follow_mode->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_electric_lock_mode->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_laser_status->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_large_screen_displayed->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_small_screen_displayed->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_imaging_plate->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_encoder_and_servo_drive->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_gyroscope_calibration->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_self_inspection_completed->setText(telemetry_when_close_str, frame_mainWindow);
    // res dbg::
    lbl_16_17_abs_roll->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_18_19_abs_pitch->setText(telemetry_when_close_str, frame_mainWindow);
    lbl_29_30_abs_azimuth->setText(telemetry_when_close_str, frame_mainWindow);
} // -- END exec_keep_rs232()
#endif

void MainWindowConstructor::set_medium_val_for_tlm()
{
    m_tm_azimuth.val = (float)from_goen_telemetry_str.target_miss_x;
    from_goen_telemetry_str.target_miss_x = (int16_t)round(calc_m_value(m_tm_azimuth.val, m_tm_azimuth.sum, m_tm_azimuth.l_val, max_list_size));
    m_tm_pitch.val = (float)from_goen_telemetry_str.target_miss_y;
    from_goen_telemetry_str.target_miss_y = (int16_t)round(calc_m_value(m_tm_pitch.val, m_tm_pitch.sum, m_tm_pitch.l_val, max_list_size));

    m_av_roll.val = (float)from_goen_telemetry_str.roll_angular_velocity;
    from_goen_telemetry_str.roll_angular_velocity = (int16_t)round(calc_m_value(m_av_roll.val, m_av_roll.sum, m_av_roll.l_val, max_list_size));
    m_av_pitch.val = (float)from_goen_telemetry_str.pitch_angular_velocity;
    from_goen_telemetry_str.pitch_angular_velocity = (int16_t)round(calc_m_value(m_av_pitch.val, m_av_pitch.sum, m_av_pitch.l_val, max_list_size));
    m_av_azimuth.val = (float)from_goen_telemetry_str.azimuth_angular_velocity;
    from_goen_telemetry_str.azimuth_angular_velocity = (int16_t)round(calc_m_value(m_av_azimuth.val, m_av_azimuth.sum, m_av_azimuth.l_val, max_list_size));

    int16_t res_field2medium = 0;
    double resf_field2medium = 0;

    // КРЕН в абсолютном пространстве
    memcpy(&res_field2medium, &from_goen_telemetry_str.abs_roll, 2);
    m_abs_roll.val = (float)res_field2medium;
    res_field2medium = (int16_t)round(calc_m_value(m_abs_roll.val, m_abs_roll.sum, m_abs_roll.l_val, max_list_size));
    memcpy(&from_goen_telemetry_str.abs_roll, &res_field2medium, 2);

    //  в абсолютном пространстве
    memcpy(&res_field2medium, &from_goen_telemetry_str.abs_pitch, 2);
    m_abs_pitch.val = (float)res_field2medium;
    res_field2medium = (int16_t)round(calc_m_value(m_abs_pitch.val, m_abs_pitch.sum, m_abs_pitch.l_val, max_list_size));
    memcpy(&from_goen_telemetry_str.abs_pitch, &res_field2medium, 2);

    // АЗИМУТ в абсолютном пространстве
    memcpy(&res_field2medium, &from_goen_telemetry_str.abs_azimuth, 2);
    m_abs_azimutch.val = (float)res_field2medium ;
    res_field2medium = (int16_t)round(calc_m_value(m_abs_azimutch.val, m_abs_azimutch.sum, m_abs_azimutch.l_val, max_list_size));
    memcpy(&from_goen_telemetry_str.abs_azimuth, &res_field2medium, 2);

    m_motor_roll.val = (float)from_goen_telemetry_str.roll_frame_angle;
    from_goen_telemetry_str.roll_frame_angle = (int16_t)round(calc_m_value(m_motor_roll.val, m_motor_roll.sum, m_motor_roll.l_val, max_list_size));
    m_motor_pitch.val = (float)from_goen_telemetry_str.pitch_frame_angle;
    from_goen_telemetry_str.pitch_frame_angle = (int16_t)calc_m_value(m_motor_pitch.val, m_motor_pitch.sum, m_motor_pitch.l_val, max_list_size);
    m_motor_azimuth.val = (float)from_goen_telemetry_str.azimuth_frame_angle;
    from_goen_telemetry_str.azimuth_frame_angle = (int16_t)calc_m_value(m_motor_azimuth.val, m_motor_azimuth.sum, m_motor_azimuth.l_val, max_list_size);
} // -- END set_medium_val_for_tlm()

double MainWindowConstructor::calc_m_value(const double &new_val, double &sum, list<double> &l_val, int max_l_size)
{
    sum += new_val;
    if(l_val.size() < max_l_size)
    {
        l_val.push_front(new_val);
    } // END if(l_val.size() < N)
    else if(l_val.size() == max_l_size)
    {
        l_val.push_front(new_val);
        sum -= l_val.back();
        l_val.pop_back();
    } // END if(l_val.size() == max_l_size)
    else
    {
        cout << "calc_m_value::Algoritm ERROR, list too long!" << endl;
        return new_val;
    } // END else
    return sum / l_val.size();
} // -- END calc_m_value()

void MainWindowConstructor::start()
{
    cout << "MainWindowConstructor::start()" << endl;
    mw.exec();
    btn_close_com_callback(this);
    btn_stop_device_callback(this);
} // -- END start
