#include "headers/application.hpp"
#include <string>
#include <bits/math-vector.h>
#include <bitset>

using namespace std;
using namespace cv;
using namespace chrono;

Application::~Application()
{
    cout << "Application Destructor" << endl;
    // rs485_worker_ptr->set_mode_Goen((uint8_t)MODE_TYPE::ROTARY_PLATFORM);
    rs485_worker_ptr->set_mode_Goen((uint8_t)MODE_TYPE::MOTOR_OFF);
    this_thread::sleep_for(10ms);
} // -- END Application::~Application()

Application::Application(const string & pathToConfig, bool &ok) : config_path(pathToConfig)
{
    ok = get_ini_params(config_path);
    if(!ok){cout << "Not ok get_ini_params!" << endl; return;}

#ifdef USE_LOGGER
    logger_artem::create(config_path, ok);
    if(!ok)
    {
        ok = false;
        quit();
        return;
    } // END if(!create_logger_status)
#endif //USE_LOGGER

    switch(device_id)
    {
#ifdef USE_FOLDER_READER
    case FOLDER:
        device = make_shared<FolderReader>(config_path, ok, dev_fps);
        cout << "make_shared<FolderReader> OK=" << ok << endl;
        if(!ok){cout << "Not ok FolderReader!" << endl; return;}
        break;
#endif // END USE_FOLDER_READER
#ifdef USE_WEB_CAMERA
    case WEBCAMERA:
        device = make_shared<WebCamera>(config_path, ok, dev_fps);
        if(!ok){return;}
        break;
#endif // END USE_WEB_CAMERA

    default:
        break;
    } // -- END switch

    if(device != nullptr)
    {
        device->register_frame_handler(this);
        register_frame_handler(bind(&Application::handleDeviceFrame, this,
                                    placeholders::_1, placeholders::_2,
                                    placeholders::_3, placeholders::_4,
                                    placeholders::_5));
    } // END if(device != nullptr)
    else
    {
#ifdef USE_LOGGER
        LoggerArtem::inst().log("Device doesn't exist: choose correct device in configuration file or rebuild application with current device");
#endif //USE_LOGGER
        ok = false;
        quit();
    } // END if(device == nullptr)
    for(int key = 0; key < 256; key++){wh_2_ext[key] = Point(0, 0);}
    wh_2_ext['z'] = Point(-1, -1);
    wh_2_ext['x'] = Point(1, 1);
    wh_2_ext['4'] = Point(-1, 0);
    wh_2_ext['7'] = Point(-1, 1);
    wh_2_ext['8'] = Point(0, 1);
    wh_2_ext['9'] = Point(1, 1);
    wh_2_ext['6'] = Point(1, 0);
    wh_2_ext['3'] = Point(-1, 1);
    wh_2_ext['2'] = Point(0, -1);
    wh_2_ext['1'] = Point(-1, -1);
#ifdef USE_GUI
    namedWindow(app_win_name, WINDOW_NORMAL);
#endif //USE_GUI

#ifdef USE_NUC_CONTROL
    nuc_control_ptr = make_shared<NucControl>(serial_port_rstpv, ok);
    if(!ok)
    {
        cout << "Application::ERROR create nuc_control_ptr!" << endl;
        common_data_ptr->set_need_quit(true);
    } // END if(!ok)
    else
    {
        cout << "Application:: OK create nuc_control_ptr with port " << serial_port_rstpv << endl;
    } // END (ok)
#endif // USE_NUC_CONTROL

    bit_ptr = make_shared<BoolUintTransformer>();
    if(bit_ptr)
    {
        cout << "Application:: OK create BoolUintTransformer pointer" << endl;
    } // END if(bit_ptr)
    else
    {
        cout << "Application:: ERROR create BoolUintTransformer pointer!" << endl;
        ok = false;
        common_data_ptr->set_need_quit(true);
    } // END if(!bit_ptr)

        // ::KALMAN

#ifdef USE_DBG_PLOT
    plot_ptr = make_shared<Plot>("plot", config_path, ok);
    if(!ok)
    {
        cout << "Application::Not ok create MPU6050UseDMP pointer!" << endl;
        common_data_ptr->set_need_quit(true);
    } // END if(!ok)
    else
    {
        plot_ptr->v_clr = {color::blue, color::green, color::red, color::blue, color::green, color::red};
        plot_ptr->v_thik = {2,2,2,1,1,1};
        tp_startapp = system_clock::now();
        cout << "Application:: SUCCES create MPU6050UseDMP pointer" << endl;
    } // END else

    graph_ptr = make_shared<Graph>(ok);
    if(!ok)
    {
        cout << "Application::Not ok create graph_ptr pointer!" << endl;
        common_data_ptr->set_need_quit(true);
    } // END if(!ok)
    else
    {
        cout << "Application:: SUCCES create graph_ptr pointer!" << endl;
    } // END else

#endif // END USE_DBG_PLOT

#ifdef USE_LOG_CMD
    log_cmd_ptr = make_shared<LogCmd>(config_path, "log_cmd", ok);
    if(!ok)
    {
        cout << "Application::Error create LogCmd!" << endl;
    } // END if(!ok)
    else
    {
        cout << "Application:: OK create LogCmd" << endl;
        log_cmd_ptr->start();
    } // END if(ok)
#endif // USE_LOG_CMD

#ifdef USE_I2C_DMP
    mpu6050_dmp_ptr = make_shared<MPU6050UseDMP>(config_path, ok);
    if(!ok)
    {
        cout << "Application::Not ok create MPU6050UseDMP pointer!" << endl;
        common_data_ptr->set_need_quit(true);
    } // END if(!ok)
    else
    {
        cout << "Application:: SUCCES create MPU6050UseDMP pointer" << endl;
    } // END else
#endif // END USE_I2C_DMP


    common_data_ptr = make_shared<common_data>();
    init_ports(pathToConfig);

    thread thr(&Application::check_press_powerOFF, this);
    check_press_button_power.swap(thr);
    check_press_button_power.detach();
    time_point_old = system_clock::now();
} // -- END Application

bool Application::FileIsExist(const string& filePath)
{
    bool isExist = false;
    ifstream fin(filePath.c_str());
    if(fin.is_open()){isExist = true;}
    fin.close();
    return isExist;
} // -- END FileIsExist

bool Application::DirContent(string& path, vector<string>& fileList)
{
    fileList.clear();
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(path.c_str())) != NULL)
    {
        while((ent = readdir(dir)) != NULL)
        {
            string filename = string(ent->d_name);
            // -- typ = 4 (folder), typ = 8 (file).
            if(ent->d_type == 8 && filename != "." && filename != "..")
            {
                fileList.emplace_back(filename);
            } // END if(ent->d_type == 8 && filename != "." && filename != "..")
            } // -- END while((ent = readdir (dir)) != NULL)
        closedir(dir);
        if(fileList.size() > 1){sort(fileList.begin(), fileList.end());}
        cout << "Dir " << path << " open success." << endl;
        return 1;
    } // -- END if((dir = opendir(way)) != NULL)
    cout << "Dir " << path << " can't open!" << endl;
    return 0;
} // -- END DirContent

void Application::quit()
{
    if(quit_was_called.load())
    {
        return;
    } // END if(quit_was_called.load())
    quit_was_called.store(true);

    _execute.store(false, memory_order_release);

    cout << " --- BEGIN Application::quit" << endl;
#ifdef USE_LOGGER
    cout << "\t --- BEGIN logger::quit()" << endl;
    bool logger_quit_report = LoggerArtem::inst().quit();
    if(logger_quit_report)
    {
        cout << "\tlogger quit success" << endl;
    } // END if(logger_quit_report)
    else
    {
        cout << "\t logger quit failed" << endl;
    } // END else
    cout << " \t --- END logger::quit()" << endl;
#endif //USE_LOGGER

#ifdef USE_LOG_CMD
    log_cmd_ptr->quit();
#endif // USE_LOG_CMD

    if(device != nullptr)
    {
        cout << "\t --- BEGIN device->quit()" << endl;
        if(device != nullptr)
        {
            device->quit();
            // std::cout<<"===*=== 2"<<std::endl;
        } // END if(device != nullptr)
        cout << "\t --- END device->quit()" << endl;
    } // END if(device != nullptr)
#ifdef USE_GUI
    destroyAllWindows();
#endif
    cout << " --- END Application::quit" << endl;
} // -- END quit

void Application::stop()
{
    cout << "Application::stop()\n";
    common_data_ptr->set_need_quit(true);
}  // -- END stop()

bool Application::quit_async()
{
    /*
        Проверка на предмет раннего вызова метода quit() или quit_async();
        Для гарантии безопасного выхода требуется
        обеспечить возможность только единократного вызова quit() или quit_async()
    */
    if(_quit_async_was_called.load() || quit_was_called.load()){return true;}

    _quit_async_was_called.store(true);
    cout << "BEGIN quit_async()" << endl;
    _execute.store(false);
    // Максимально допустимое время ожидания закрытия основного цикла приложения
    // При превышении приложение закрывается не дожидаясь закрытия потока функции exec()ж
    const int watchdog_close_exec_time_ms = 3000;
    // Общее количество отсчетов, которое выполняется
    const int subdivisions_total = 100;
    // Временной интервал проверки состояния потока метода exec() за предмет завершенности (закрытости)
    int check_interval_ms = 1;
    
    // Проверки безопасности введенных пользовательских значений watchdog_close_exec_time_ms и subdivisions_total
    if(subdivisions_total > 0){check_interval_ms = watchdog_close_exec_time_ms / subdivisions_total;}
    if(check_interval_ms <= 0){check_interval_ms = 1;}

    cout << "\t watchdog_close_exec_time_ms = " << watchdog_close_exec_time_ms << endl;
    cout << "\t subdivisions_total = " << subdivisions_total << endl;
    cout << "\t check_interval_ms = " << check_interval_ms << endl;
    bool success_close_exec = false;
    for(int i = 0; i < subdivisions_total; ++i)
    {
        if(_exec_complete_success.load())
        {
            cout << "\tSuccess close exec(), total wait time = " << i * check_interval_ms << " (ms)" << endl;
            success_close_exec = true;
            break;
        } // END if(_exec_complete_success.load())
        this_thread::sleep_for(milliseconds(check_interval_ms));
    } // END for(int i = 0; i < subdivisions_total; ++i)

    if(!success_close_exec){cout << "Error close exec()!" << endl;}
    quit();
    _quit_async_complete.store(true);
    cout << "END quit_async()" << endl;
    return success_close_exec;
} // -- END quit_async

void Application::start()
{
    device->start();
    _execute.store(true, memory_order_release);
    exec();
} // -- END start

void Application::exec()
{
    bool ok = false;
    // int counter = 0;
    cout << "Application::exec START" << endl;

    while(!frameReady.load())
    {
        this_thread::sleep_for(milliseconds(1));
    } // END while(!frameReady.load())

    frame_w = frame_process_0.cols;
    frame_h = frame_process_0.rows;
    frame_w_1 = 1.f / (float)frame_w;
    frame_h_1 = 1.f / (float)frame_h;
#ifdef USE_GUI
    frame_show_height = round(frame_w_1 * frame_h * frame_show_width);
#endif // USE_GUI
    frame_w_2 = round(frame_w * 0.5);
    frame_h_2 = round(frame_h * 0.5);
    rct_zoom = {0, 0, frame_w, frame_h};
    img_send_w = round((float)frame_send_h * frame_w / frame_h);
    img_send_w_1 = 1.f / img_send_w ;
#if defined(CCM_8UC1)
    frame_send = Mat(Size(frame_send_w, frame_send_h), CV_8UC1, Scalar(0));
#elif defined(CCM_8UC3)
    frame_send = Mat(Size(frame_send_w, frame_send_h), CV_8UC3, Scalar(0,0,0));
#endif // END defined(CCM_8UC1)

    img2send_rct = Rect(frame_send.cols * 0.5 - img_send_w * 0.5, 0, img_send_w, frame_send_h);
    //    cout << "img_send_w = " << img_send_w << endl;
    //    int dev_fps = 30;;
    tracShats = make_shared<TracShats>(config_path, frame_process_0, ok);
    if(!ok || tracShats == nullptr)
    {
        cout << "Application::exec:: ERROR create tracShats!" << endl;
        common_data_ptr->set_need_quit(true);
        return;
    } // END if(!ok)

    mat2rtsp_sender = make_unique<mat2rtsp>(config_path, ok, frame_send_w, frame_send_h, dev_fps);
    if(!ok)
    {
        cout << "NOT make_unique<mat2rtsp>!\n";
        common_data_ptr->set_need_quit(true);
        return;
    } // END if (!ok)

#ifdef USE_I2C_DMP
    mpu6050_dmp_ptr->start();

#endif // USE_I2C_DMP

    cout << "exec1:: frame size : frame_w = " << frame_w << "; frame_h = " << frame_h << endl;
#ifdef USE_GUI
    frame_show_setup();
#endif // USE_GUI

#ifdef USE_LOGGER
    tp_full0 = system_clock::now();
#endif // USE_LOGGER

    // основной цикл выполнения пиложения
    while(_execute.load(memory_order_acquire))
    {
        if(frameReady.load())   // если кадр получен
        {
            next_frame_async();
#ifdef USE_LOGGER
            time_point_tracker0 = system_clock::now();
#endif // USE_LOGGER
            prepareFrameForTracShats();
            workflowShats();
#ifdef USE_NUC_CONTROL
            if(tracShats->isInited() && nuc_control_ptr->autonuc_status()) {nuc_control_ptr->autonuc_off();}
            if(!tracShats->isInited() && !nuc_control_ptr->autonuc_status())
            {
                nuc_control_ptr->autonuc_on();
                // nuc_control_ptr->set_nuc_interval(1);
            } // END if(!tracShats->isInited() && !nuc_control_ptr->autonuc_status())
#endif // USE_NUC_CONTROL

#ifdef USE_LOGGER
            time_point_tracker1 = system_clock::now();
            if(tracShats->isInited())
            {
                LoggerArtem::inst().logTimedBasedFPS("TrackShats FPS = ",
                                                     duration<double>(time_point_tracker1 - time_point_tracker0).count());
            } // END if(tracShats->isInited())
#endif // USE_LOGGER

            if(signal_flag)
            {
                common_data_ptr->set_need_quit(true);
                this_thread::sleep_for(10ms);
            }//END signal_flag)

            copy_workRect();
            exec_rcv_rs232_cmd();
            f_need_send_tlm.store(true);

#ifdef USE_GUI
            resize(frame_receive, frame_show, Size(frame_show_width, frame_show_height));
            if(mouseHandler(frame_show, rectm)) // инициализация трекера, если указана новая рамка цели
            {
                Rect2f rectm_rel = Rect2f(rectm.x * frame_show_w_1, rectm.y * frame_show_h_1,
                                          rectm.width * frame_show_w_1, rectm.height * frame_show_h_1);
                handle_treatment(rectm_rel);
            } // END if(mouseHandler(frame_show, rectm))
#endif // USE_GUI

            if(flag_zahvat)
            {
                // приведение рамки цели к абсолютным координатам
                Rect2f rectShats(aimRectShats.x * frame_w, aimRectShats.y * frame_h,
                                 aimRectShats.width * frame_w, aimRectShats.height * frame_h);
#ifdef USE_TV
                rectangle(frame_receive, rectShats, Scalar(0,255,0), 2);
#else
                rectangle(frame_receive, rectShats, Scalar(0), 2);
                rectangle(frame_receive, rectShats, Scalar(255), 1);
#endif // END #ifdef !USE_TV
            } // END if(flag_zahvat)

                // transparent_image();
// scale05
#ifdef USE_TV
            if(digital_zoom_value > 1)
            {
                Mat img1;
                //Mat img2send = frame_receive(rct_zoom);
                resize(frame_receive(rct_zoom), img1, Size(img_send_w, frame_send_h));
                frame_send = img1;
            } // END if(digital_zoom_value == 1)
            else
            {
                frame_send = frame_receive;
            } // END if(digital_zoom_value != 1)
#else // !USE_TV
            resize(frame_receive(rct_zoom), frame_send(img2send_rct), Size(img_send_w, frame_send_h));
#endif // !USE_TV
            Point pt_c = Point(round(frame_send.cols * 0.5), round(frame_send.rows * 0.5));
            int zero_sz = 10;
            int line_sz = 28 + zero_sz;
            // Отрисовка перекрестия
            line(frame_send, pt_c + Point(zero_sz, 0), pt_c + Point(line_sz, 0), Scalar(0,0,0), 4);
            line(frame_send, pt_c - Point(zero_sz, 0), pt_c - Point(line_sz, 0), Scalar(0,0,0), 4);
            line(frame_send, pt_c + Point(0, zero_sz), pt_c + Point(0, line_sz), Scalar(0,0,0), 4);
            line(frame_send, pt_c - Point(0, zero_sz), pt_c - Point(0, line_sz), Scalar(0,0,0), 4);
            line(frame_send, pt_c + Point(zero_sz, 0), pt_c + Point(line_sz, 0), Scalar(255,255,255), 2);
            line(frame_send, pt_c - Point(zero_sz, 0), pt_c - Point(line_sz, 0), Scalar(255,255,255), 2);
            line(frame_send, pt_c + Point(0, zero_sz), pt_c + Point(0, line_sz), Scalar(255,255,255), 2);
            line(frame_send, pt_c - Point(0, zero_sz), pt_c - Point(0, line_sz), Scalar(255,255,255), 2);
#ifdef USE_TLM_MODE_CHANGER
            std::string mode_switch_status = "";
            mode_switch_status += to_string(angular_velocity_enable);
            mode_switch_status += to_string(abs_angle_enable);
            // if(abs_angle_enable == 0) {mode_switch_status += "0";}
            // else {mode_switch_status += "1";}
            // if(angular_velocity_enable == 0) {mode_switch_status += "0";}
            // else {mode_switch_status += "1";}
            putText(frame_send, mode_switch_status, Point(20,20), 0, 1, color::red, 2);
            // if(tm_xy_enable == 0) {putText(frame_send, "tm_XY Disable!", Point(40,50), 0, 1, color::red, 2);}
            // if(tm_xy_enable == 1) {putText(frame_send, "tm_XY Enable!", Point(40,50), 0, 1, color::red, 2);}
#endif // USE_TLM_MODE_CHANGER


#if defined(CCM_8UC1)            
            cvtColor(frame_send, frame_send_bgr, COLOR_GRAY2BGR);
            mat2rtsp_sender->sendToRTSPServer(frame_send_bgr);
#elif defined(CCM_8UC3)
            mat2rtsp_sender->sendToRTSPServer(frame_send);
#endif // END !defined(CCM_8UC1)


#ifdef USE_LOGGER
            tp_full1 = system_clock::now();
            LoggerArtem::inst().logTimedBasedFPS(
                "SEND FPS = ",
                duration<double>(tp_full1 - tp_full0).count());
            tp_full0 = system_clock::now();
#endif // USE_LOGGER

            if(common_data_ptr->is_need_quit())
            {
                cout << "need_quit from application"<<endl;
                rs485_worker_ptr->set_mode_Goen((uint8_t)MODE_TYPE::MOTOR_OFF);
                this_thread::sleep_for(50ms);
                rs485_worker_ptr->set_mode_Goen((uint8_t)MODE_TYPE::MOTOR_OFF);
                this_thread::sleep_for(500ms);
                quit();
                if(thread_rs232_485.joinable()) thread_rs232_485.join();
                if(thread_only_rs232.joinable()) thread_only_rs232.join();
#ifdef USE_I2C_DMP
                mpu6050_dmp_ptr->stop();
#endif // USE_I2C_DMP

                break;
            } // END if(common_data_ptr->is_need_quit())
            } // END if(frameReady.load())
        else
        {
            while(!frameReady.load())
            {
                this_thread::sleep_for(milliseconds(5));
            } // -- END while(!frameReady.load())
        } // -- END if(!frameReady.load())
        } // -- END while(_execute.load(memory_order_acquire))

    cout << "Complete Application::exec()" << endl;
    _exec_complete_success.store(true);
    this_thread::sleep_for(milliseconds(10));
    bool success_wait_quit_async_complete = false; // флаг успешного завершения работы метода quit_async()
    if(_quit_async_was_called.load())
    {
        for(int i = 0; i < 100; ++i)
        {
            if(_quit_async_complete.load())
            {
                success_wait_quit_async_complete = true;
                cout << "_quit_async_complete detected <<true>>" << endl;
                this_thread::sleep_for(milliseconds(20));
                break;
            } // if(_quit_async_complete.load())
            } // END for(int i = 0; i < 100; ++i)
    } // END if(_quit_async_complete.load())
    cout << "END exec" << endl;
} // -- END exec



void Application::handle_treatment(Rect2f & rectm_rel)
{
    cout << "Attempt tacking capture: \n " << rectm_rel << endl
         << Rect(rectm_rel.x * frame_w, rectm_rel.y * frame_h, rectm_rel.width * frame_w, rectm_rel.height * frame_h) << endl;;
    bool change = 0;
    if (rectm_rel.width  > 0.2)  {rectm_rel.width  = 0.2 - frame_w_1; cout << "change width" << endl; change = 1;}
    if (rectm_rel.height > 0.2) {rectm_rel.height = 0.2 - frame_h_1; cout << "change height" << endl; change = 1;}
    if (rectm_rel.x < frame_w_1) {rectm_rel.x = 3 * frame_w_1; cout << "move x position" << endl; change = 1;}
    if (rectm_rel.y < frame_h_1) {rectm_rel.y = 3 * frame_h_1; cout << "move y position" << endl; change = 1;}
    if ((rectm_rel.x + rectm_rel.width) > (1.f - frame_w_1)) {rectm_rel.x = 1.f - 3 * frame_w_1 - rectm_rel.width; cout << "move x position" << endl; change = 1;}
    if ((rectm_rel.y + rectm_rel.height) > (1.f - frame_h_1)) {rectm_rel.y = 1.f - 3 * frame_h_1 - rectm_rel.height; cout << "move y position" << endl; change = 1;}
    if (change) {cout << "Attempt tacking capture:  " << rectm_rel  << "; " << Rect(rectm_rel.x * frame_w, rectm_rel.y * frame_h, rectm_rel.width * frame_w, rectm_rel.height * frame_h) << endl;}
    bool initFlag = false;
    if(tracShats->isInited())
    {
        tracShats->deinit();
        workflowShats();
    } // END if(tracShats->isInited())
    initFlag = tracShats->init(rectm_rel);
    tracShats->setWorkNumber(device->getFrameCounter());
    if(initFlag)
    {
        if(!isTracShatsFirstInitedFlag){isTracShatsFirstInitedFlag = true;}
        isTracShatsInitedFlag = true;
    } // -- END if(initFlag)
    //    } // -- END if(!tracShats->isInited())
    return;
} // --END handle_treatment

void Application::calc_to_goen_tlm_rs232_str_params()
{
    /// Получение телеметрии борта
    ang_pitch_deg = ((float)from_bort_tlm_rs232_str.aircraft_pitch_angle) / 10;
    ang_roll_deg = ((float)from_bort_tlm_rs232_str.aircraft_roll_angle) / 10;
    ang_heading_deg = ((float)from_bort_tlm_rs232_str.aircraft_yaw_angle) / 10;
    return;
} // -- END calc_to_goen_tlm_rs232_str_params

void Application::next_frame_async()
{
    /*
    Обособленный mutex'ом фрагмент не осуществляет клонирования frame,
    реализуется оптимальная передача по ссылке,
    но дополнительная потокобезопасность обеспечивается
    frame_proc_(0/1)_mutex без существенных накладных расходов
    */
    switch(process_frame_id) // id буффера на предыдущем шаге
    {
    case 0:
    {
        frame_proc_1_mutex.lock();
        process_frame_id = 1;
        frame_receive = frame_process_1;
        // imshow("dbg1::", frame_process_1);
        frame_proc_1_mutex.unlock();
    } // END case 0:
    break;

    case 1:
    {
        frame_proc_0_mutex.lock();
        process_frame_id = 0;
        frame_receive = frame_process_0;
        frame_proc_0_mutex.unlock();
    } // END case 1:
    break;

    default:
    {
        throw runtime_error("Error: incorrect process_frame_id");
    } // END default:
    break;
    } // END switch(process_frame_id)
    frameReady.store(false);

    return;
} // -- END next_frame_async

bool Application::get_ini_params(const string &config)
{
    cout << "\nBEGIN get_ini_params Application" << endl;
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

    device_id = reader.GetInteger("main_settings", "device_id", ini_err_value);
    if(device_id == ini_err_value)
    {
        cout << "\tdevice_id not declared!\n";
        return 0;
    } // END if(device_id == ini_err_value)
    cout << "\tdevice_id = " << device_id << ";\n";

#if defined USE_TV
    zahvat_size = reader.GetInteger("tracking", "zahvat_size_TV", ini_err_value);
    if(zahvat_size == ini_err_value)
    {
        cout << "\tNot found zahvat_size_TV in Application::getMainSettings!\n";
        return false;
    } // END if(zahvat_size_TV == -1)
    cout << "\tzahvat_size = " << zahvat_size << endl;
#else  // END  #if defined USE_TV

#ifdef USE_NUC_CONTROL
    serial_port_rstpv = reader.Get("RS_232_485", "SERIAL_PORT_RSTPV", "oops");
    if(serial_port_rstpv == "oops") {\tcout << "Not found SERIAL_PORT_RSTPV" << endl; return false;}
    else {cout << "\t[RS232_485]: serial_port_rstpv = " << serial_port_rstpv << endl;}
#endif // USE_NUC_CONTROL

#ifdef USE_UDP_TLM
    valid_ap_step_decrease = reader.GetReal("NETWORK", "valid_ap_step_decrease", ini_err_value);
    if(valid_ap_step_decrease == -1) {cout << "\tNot found valid_ap_step_decrease \n"; return false;}
    else {cout << "\tvalid_ap_step_decrease = " << valid_ap_step_decrease << endl;}

#endif // USE_UDP_TLM

    zahvat_size = reader.GetInteger("tracking", "zahvat_size_TPV", ini_err_value);
    if(zahvat_size == -1)
    {
        cout << "\tNot found zahvat_size_TPV in Application::getMainSettings!\n";
        return false;
    } // END if(zahvat_size_TPV == -1)
    cout << "\tzahvat_size = " << zahvat_size << endl;
#endif  // END #if ! defined USE_TV

    wh_2_zahvat = Point2i(zahvat_size, zahvat_size);

#ifdef USE_GUI
    frame_show_width = reader.GetInteger("demonstration_settings", "frame_show_width", ini_err_value);
    if(frame_show_width == -1)
    {
        cout << "\tNot found width in Application::getMainSettings!\n";
        return false;
    } // END if(frame_show_width == -1)
    cout << "\tframe_show_width = " << frame_show_width << endl;
    demonstration_mode = reader.GetInteger("demonstration_settings", "demonstration_mode", ini_err_value);
    if(demonstration_mode == -1)
    {
        cout << "N\tot found mode in Application::getMainSettings!\n";
        return false;
    } // END if(demonstration_mode == -1)
    cout << "\tdemonstration_mode = " << demonstration_mode << endl;
#endif // USE_GUI
    show_command_interval = reader.GetInteger("RS_232_485", "show_command_interval", ini_err_value);
    if(show_command_interval == -1) {cout << "\tNot found show_command_interval \n"; return false;}
    else {cout << "\tshow_command_interval = " << show_command_interval << endl;}

    rs232_speed = reader.GetInteger("RS_232_485", "rs232_speed", ini_err_value);
    if(rs232_speed == -1) {cout << "\tNot found rs232_speed \n"; return false;}
    else {cout << "\trs232_speed = " << rs232_speed << endl;}

    rs485_speed = reader.GetInteger("RS_232_485", "rs485_speed", ini_err_value);
    if(rs485_speed == -1) {cout << "\tNot found rs485_speed \n"; return false;}
    else {cout << "\trs485_speed = " << rs485_speed << endl;}

    SERIAL_PORT_RS232 = reader.Get("RS_232_485", "SERIAL_PORT_RS232", "oops");
    if(SERIAL_PORT_RS232 == "oops") {cout << "\tNot found SERIAL_PORT_RS232 \n"; return false;}
    else {cout << "\tSERIAL_PORT_RS232 = " << SERIAL_PORT_RS232 << endl;}

    SERIAL_PORT_RS485 = reader.Get("RS_232_485", "SERIAL_PORT_RS485", "oops");
    if(SERIAL_PORT_RS485 == "oops") {cout << "\tNot found SERIAL_PORT_RS485 \n"; return false;}
    else {cout << "\tSERIAL_PORT_RS485 = " << SERIAL_PORT_RS485 << endl;}

    send_CMD_BLOCK_OFF = reader.GetInteger("RS_232_485", "send_CMD_BLOCK_OFF", ini_err_value);
    if(send_CMD_BLOCK_OFF == -1) {cout << "\tNot found send_CMD_BLOCK_OFF \n"; return false;}
    else {cout << "\tsend_CMD_BLOCK_OFF = " << send_CMD_BLOCK_OFF << endl;}


    Vx_dreyf0 = reader.GetReal("tracking", "Vx_dreyf", ini_err_value);
    if(Vx_dreyf0 == -1) {cout << "\tNot found Vx_dreyf \n"; return false;}
    else {cout << "\tVx_dreyf0 = " << Vx_dreyf0 << endl;}

    Vy_dreyf0 = reader.GetReal("tracking", "Vy_dreyf", ini_err_value);
    if(Vy_dreyf0 == -1) {cout << "\tNot found Vy_dreyf \n"; return false;}
    else {cout << "\tVy_dreyf0 = " << Vy_dreyf0 << endl;}


    int key_quit_handler_int = (char)reader.GetInteger("demonstration_settings", "key_quit_handler", ini_err_value);
    if(key_quit_handler_int == -1) {cout << "\tNot found key_quit_handler_int \n"; return false;}
    else {cout << "\tkey_quit_handler_int = " << key_quit_handler_int << endl;}
    key_quit_handler = (uchar)key_quit_handler_int;

    tm_xy_enable = reader.GetInteger("NETWORK", "tm_xy_enable", ini_err_value);
    if(tm_xy_enable == ini_err_value) {cout << "\tNot found tm_xy_enable \n"; return false;}
    else {cout << "\ttm_xy_enable = " << tm_xy_enable << endl;}

    cout << "END get_ini_params Application\n" << endl;
    return 1;
} // -- END get_ini_params

bool Application::processShats()
{
    // обновление рамки цели по текущему кадру
    bool flag = tracShats->update(frame_process_tracshats, aimRectShats);
    return flag;
} // -- END processShats

void Application::prepareFrameForTracShats()
{
    frame_process_tracshats = frame_receive; //.clone();
} // -- END prepareFrameForTracShats

void Application::workflowShats()
{
    //cout << "isTracShatsFirstInitedFlag=" << isTracShatsFirstInitedFlag << endl;
    if(isTracShatsFirstInitedFlag)
    {
        flag_zahvat = processShats(); // обработка кадра
        if(!flag_zahvat)
        {
            if(!tracShats->isInited()){isTracShatsInitedFlag = false;}
        } // END if(!flag_zahvat)
        } // END if(isTracShatsFirstInitedFlag)
    else if(tracShatsInitReqFlag) // инициализация трекера при поступлении запроса
    {
        tracShatsInitReqFlag = false;
        cout << "Init tracker" << endl;
        if(!tracShats->isInited())
        {
            bool initFlag = false;
            initFlag = tracShats->init(aimRectShats);
            tracShats->setWorkNumber(device->getFrameCounter());
            if(initFlag)
            {
                if(!isTracShatsFirstInitedFlag){isTracShatsFirstInitedFlag = true;}
                isTracShatsInitedFlag = true;
            } // -- END if(initFlag)
            // подготовка нового кадра (т.к. для неинициализированного трекера кадр для обработки не подготовлен)
            prepareFrameForTracShats();
            // обработка кадра "на месте" для корректной инициализации трекера
            workflowShats();
        } // -- END if(!tracShats->isInited())
        } // -- END else if(tracShatsInitReqFlag)
    } // -- END workflowShats


void Application::handleDeviceFrame(uint8_t *f, int w, int h, int num, int id)
{
    static auto prevFrameReceiveTime = high_resolution_clock::now();
    auto receiveTime = high_resolution_clock::now();

#ifdef USE_LOGGER
    LoggerArtem::inst().logTimedBasedFPS(
        "Device FPS = ",
        duration<double>(receiveTime - prevFrameReceiveTime).count());
#endif //USE_LOGGER

    prevFrameReceiveTime = receiveTime;

    static bool first_frame = true;

    if(first_frame)
    {
#if defined(CCM_8UC1)
        int cvmat_type = CV_8UC1;
#elif defined(CCM_8UC3)
        int cvmat_type = CV_8UC3;
#else
        throw runtime_error("Error: not recognize/supported color channel mode");
#endif
        frame_proc_0_mutex.lock();
        frame_process_0 = Mat(Size(w, h), cvmat_type);
        device->getFormatedImage(f, w, h, round(digital_zoom_value * 10), frame_process_0);

        frame_proc_0_mutex.unlock();

        frame_proc_1_mutex.lock();
        frame_process_1 = frame_process_0.clone();
        frame_proc_1_mutex.unlock();

        first_frame = false;
        frameReady.store(true);
    } // END if(first_frame)
    else if(!frameReady.load()) // -- END if(first_frame)
    {
        switch(process_frame_id) // двойная буфферизация
        {
        case 0:
            frame_proc_1_mutex.lock();
            device->getFormatedImage(f, w, h, round(digital_zoom_value * 10), frame_process_1);

            frame_proc_1_mutex.unlock();
            break;

        case 1:
            frame_proc_0_mutex.lock();
            device->getFormatedImage(f, w, h, round(digital_zoom_value * 10), frame_process_0);

            frame_proc_0_mutex.unlock();
            break;

        default:
            throw runtime_error("Error: incorrect process_frame_id");
            break;
        } // END switch(process_frame_id)
        frameReady.store(true);
    } // -- END if(!frameReady.load())
    } // -- END handleDeviceFrame

void Application::exec_rcv_rs232_cmd()
{
    if(receivedCMD)
    {
#ifdef USE_GUI
        f_receivedCMD = true;
#endif // USE_GUI \ \
        // cout << "Execute rs232 command: " << hex << (int)receivedCMD << dec << endl;
        switch(receivedCMD)
        {
        //    cout << "Application:: receivedCMD = " << receivedCMD << endl;
        case (int)COMMAND_RS232::notCMD:
        {
            //        cout << "Application::switch(receiveRS232_cmd):: NULL INSTRUCTION CMD!" << endl;
            break;
        } // END case (int)COMMAND_RS232::notCMD:

        case (int)COMMAND_RS232::PTZ:
        {
            // cout << "Application::switch(receiveRS232_cmd)::PTZ CMD" << endl;
            break;
        } // END case (int)COMMAND_RS232::PTZ:

        case (int)COMMAND_RS232::INFRA:
        {
            cout << "Application::switch(receiveRS232_cmd):: INFRA CMD" << endl;
            break;
        } // END case (int)COMMAND_RS232::INFRA:

        case (int)COMMAND_RS232::TV:
        {
            cout << "Application::switch(receiveRS232_cmd):: TV CMD" << endl;
            break;
        } // END case (int)COMMAND_RS232::TV:

        case (int)COMMAND_RS232::ZOOM:
        {
            cout << "Application::switch(receiveRS232_cmd):: ZOOM CMD! Change zoom for " << (int)from_bort_cmd_rs232_str.zoom_rate << endl;

            if(from_bort_cmd_rs232_str.zoom_rate > 0)
            {
                f_command_str += "ZOOM UP";
                if(zoom_index < v_zoom_value.size() - 1) {zoom_index++;}
            } // END if(to_goen_cmd_rs232_str.zoom_rate > 0)
            if(from_bort_cmd_rs232_str.zoom_rate < 0)
            {
                f_command_str += "ZOOM DOWN";
                if (zoom_index > 0) {zoom_index--;}
            } // END if(to_goen_cmd_rs232_str.zoom_rate < 0 && zoom_index > 0)

            digital_zoom_value = v_zoom_value[zoom_index];
            if(digital_zoom_value < 1)
            {
                digital_zoom_value_1 = 1.f;
                rs485_worker_ptr->scale05 = 1;
            } // END if(digital_zoom_value < 1)
            else
            {
                digital_zoom_value_1 = 1.f / digital_zoom_value;
                rs485_worker_ptr->scale05 = 0;
            } // END if(!digital_zoom_value >= 1)
            rct_zoom = {(int)round(frame_w_2 - frame_w_2 * digital_zoom_value_1),
                        (int)round(frame_h_2 - frame_h_2 * digital_zoom_value_1),
                        (int)round(frame_w * digital_zoom_value_1),
                        (int)round(frame_h * digital_zoom_value_1)};
            break;
        } // END case (int)COMMAND_RS232::ZOOM:

        case (int)COMMAND_RS232::TO_ZERO_POSITION:
        {
            cout << "Application::switch(receiveRS232_cmd):: TO_ZERO_POSITION CMD" << endl;
            break;
        } // END case (int)COMMAND_RS232::TO_ZERO_POSITION:

        case (int)COMMAND_RS232::TRACKING_START:
        {
            cout << "Application::switch(receiveRS232_cmd):: TRACKING_START CMD" << endl;
            TrackTarget track_target_2_aim;
            memcpy(&track_target_2_aim, &from_bort_cmd_rs232_str.parameter_x, sizeof(TrackTarget));

            Rect2f rectm_rel = Rect2f(((int)track_target_2_aim.x_offset * digital_zoom_value_1 * img_send_w_1 + 0.5) - wh_2_zahvat.x * frame_w_1,
                                      (-(int)track_target_2_aim.y_offset * digital_zoom_value_1 * frame_send_h_1 + 0.5) - wh_2_zahvat.y * frame_h_1,
                                      2 * wh_2_zahvat.x * frame_w_1,
                                      2 * wh_2_zahvat.y * frame_h_1);
            handle_treatment(rectm_rel);
            break;
        } // END case (int)COMMAND_RS232::TRACKING_START:

        case (int)COMMAND_RS232::TRACKING_STOP:
        {
            cout << "Application::switch(receiveRS232_cmd):: TRACKING_STOP CMD" << endl;
            tracShats->deinit();
            break;
        } // END case (int)COMMAND_RS232::TRACKING_STOP:

        case (int)COMMAND_RS232::AZIMUTH_FOLLOW:
        {
            cout << "Application::switch(receiveRS232_cmd):: AZIMUTH_FOLLOW CMD" << endl;
            break;
        } // END case (int)COMMAND_RS232::AZIMUTH_FOLLOW:

        case (int)COMMAND_RS232::CLOSE_FOLLOW:
        {
            cout << "Application::switch(receiveRS232_cmd):: CLOSE_FOLLOW CMD" << endl;
            break;
        } // END case (int)COMMAND_RS232::CLOSE_FOLLOW:

        case (int)COMMAND_RS232::ELECTRIC_LOCK_ON:
        {
            if(rs485_worker_ptr->f_motor_on.load())
            {
                cout << "Application::switch(receiveRS232_cmd):: ELECTRIC_LOCK_ON CMD! GO TO ROTARY PLATFORM MODE" << endl;
            } // END if(rs485_worker_ptr->f_motor_on.load())
            else
            {
                cout << "Application::switch(receiveRS232_cmd):: ELECTRIC_LOCK_ON CMD! STAY IN MOTOR OFF MODE" << endl;
            } // END if(!rs485_worker_ptr->f_motor_on.load())
            break;
        } // END case (int)COMMAND_RS232::ELECTRIC_LOCK_ON:

        case (int)COMMAND_RS232::ELECTRIC_LOCK_OFF:
        {
            if(rs485_worker_ptr->f_motor_on.load())
            {
                cout << "Application::switch(receiveRS232_cmd):: ELECTRIC_LOCK_OFF CMD. GO TO STABILIZATION MODE" << endl;
            } // END if(rs485_worker_ptr->f_motor_on.load())
            else
            {
                cout << "Application::switch(receiveRS232_cmd):: ELECTRIC_LOCK_OFF CMD. GO TO ROTARY_PLATFORM MODE" << endl;
            } // END if(!rs485_worker_ptr->f_motor_on.load())
            break;
        } // END case (int)COMMAND_RS232::ELECTRIC_LOCK_OFF:

        case (int)COMMAND_RS232::MOTOR_ON:
        {
            if(rs485_worker_ptr->f_motor_lock.load())
            {
                cout << "Application::switch(receiveRS232_cmd):: MOTOR_ON CMD! GO TO ROTARY_PLATFORM MODE" << endl;
            } // END if(rs485_worker_ptr->f_motor_lock.load())
            else
            {
                cout << "Application::switch(receiveRS232_cmd):: MOTOR_ON CMD! GO TO STABILISATION MODE" << endl;
            } // END if(!rs485_worker_ptr->f_motor_lock.load())
            break;
        } // END case (int)COMMAND_RS232::MOTOR_ON:

        case (int)COMMAND_RS232::MOTOR_OFF:
        {
            cout << "Application::switch(receiveRS232_cmd):: MOTOR_OFF CMD" << endl;
            break;
        } // END case (int)COMMAND_RS232::MOTOR_OFF:

        case (int)COMMAND_RS232::SET_ZERO_POSITION:
        {
            cout << "Application::switch(receiveRS232_cmd):: SET_ZERO_POSITION CMD" << endl;
            rs485_worker_ptr->ang0_x = rs485_worker_ptr->from_goen_telemetry_str.motor_yaw.angle;
            rs485_worker_ptr->ang0_y = rs485_worker_ptr->from_goen_telemetry_str.motor_pitch.angle;
            cout << "New zero angle = " << Point2f(rs485_worker_ptr->ang0_x, rs485_worker_ptr->ang0_y) << endl;
            break;
        } // END case (int)COMMAND_RS232::SET_ZERO_POSITION:

        case (int)COMMAND_RS232::SET_AZIMUT_PITCH:
        {
            cout << "Application::switch(receiveRS232_cmd):: SET_AZIMUT_PITCH CMD" << endl;
            break;
        } // END case (int)COMMAND_RS232::SET_AZIMUT_PITCH:

        case (int)COMMAND_RS232::IR_WHITE_HEAT:
        {
#if defined(USE_TPV_cam)
            f_ir_black_heat = 0;
            cout << "Application::switch(receiveRS232_cmd):: IR_WHITE_HEAT" << endl;
#endif //END  #if defined(USE_TPV_cam)
            break;
        } // END IR_WHITE_HEAT

        case (int)COMMAND_RS232::IR_BLACK_HEAT:
        {
#if defined(USE_TPV_cam)
            f_ir_black_heat = 1;
            cout << "Application::switch(receiveRS232_cmd):: IR_BLACK_HEAT" << endl;
#endif //END  #if defined(USE_TPV_cam)
            break;
        } // END IR_BLACK_HEAT

        case (int)COMMAND_RS232::IMAGE_ENCHANCE_ON:
        {
            f_image_enchance = 1;
            cout << "Application::switch(receiveRS232_cmd):: IMAGE_ENCHANCE_ON" << endl;
            break;
        } // END case (int)COMMAND_RS232::IMAGE_ENCHANCE_ON:

        case (int)COMMAND_RS232::IMAGE_ENCHANCE_OFF:
        {
            f_image_enchance = 0;
            cout << "Application::switch(receiveRS232_cmd):: IMAGE_ENCHANCE_OFF" << endl;
            break;
        } // END case (int)COMMAND_RS232::IMAGE_ENCHANCE_OFF:

        case(int)COMMAND_RS232::TRAC_SIZE_CHANGE:
        {
            wh_2_zahvat.x = round(0.5 * (int)from_bort_cmd_rs232_str.parameter_x);
            wh_2_zahvat.y = round(0.5 * (int)from_bort_cmd_rs232_str.parameter_x);
            if(wh_2_zahvat.x < 4 || wh_2_zahvat.y < 4 )
            {
                wh_2_zahvat = Point(4,4);
            } // END if(wh_2_zahvat.x < 4 || wh_2_zahvat.y < 4 )
            if(wh_2_zahvat.x > 64 || wh_2_zahvat.y > 64 )
            {
                wh_2_zahvat = Point(64,64);
            } // END if(wh_2_zahvat.x < 4 || wh_2_zahvat.y < 4 )

            cout << "Application::switch(receiveRS232_cmd):: TRAC_SIZE_CHANGE" << endl;
            break;
        } // END  case(int)COMMAND_RS232::TRAC_SIZE_CHANGE:

        case((int)COMMAND_RS232::CAMERA_CONTROL):
        {
            cout << "Application::switch(receiveRS232_cmd):: CAMERA_CONTROL: HANDLE NUC CORRECTION" << endl;
#ifdef USE_NUC_CONTROL
            nuc_control_ptr->shutter_correction();
#endif // END USE_NUC_CONTROL
            break;
        } //  END case COMMAND_RS232::CAMERA_CONTROL

        case((int)COMMAND_RS232::CALIB_ZERO_POS_FC_ATT):
        {
            cout << "\nApplication::switch(receiveRS232_cmd):: CALIB_ZERO_POS_FC_ATT: HANDLE NUC CORRECTION" << endl;
            cout << "\t======== param X: " << (int)rs232_worker_ptr->parametr_X.buf16 << endl;
            cout << "\t======== param Y: " << (int)rs232_worker_ptr->parametr_Y.buf16 << endl ;
            cout << "\t======== param 3: " << (int)rs232_worker_ptr->cmdBuf[7]  << endl;
            cout << "\t======== zoom: " << (int)rs232_worker_ptr->cmdBuf[8]  << endl << endl;
            break;
        } //  END case COMMAND_RS232::CALIB_ZERO_POS_FC_ATT

        case((int)COMMAND_RS232::SPECIFI_ATTITUDE_ANGLE):
        {
            cout << "\nApplication::switch(receiveRS232_cmd):: SPECIFI_ATTITUDE_ANGLE: HANDLE NUC CORRECTION" << endl;
            cout << "\t======== param X: " << (int)rs232_worker_ptr->parametr_X.buf16 << endl;
            cout << "\t======== param Y: " << (int)rs232_worker_ptr->parametr_Y.buf16 << endl ;
            cout << "\t======== param 3: " << (int)rs232_worker_ptr->cmdBuf[7]  << endl;
            cout << "\t======== zoom: " << (int)rs232_worker_ptr->cmdBuf[8]  << endl << endl;
            break;
        } //  END case SPECIFI_ATTITUDE_ANGLE


#ifdef USE_TLM_MODE_CHANGER
        case((int)COMMAND_RS232::TLM_SENDER_MODE_SWITCH):
        {
            cout << "Application::switch(receiveRS232_cmd):: TLM_SENDER_MODE_SWITCH. " << endl;
            cout << "New mode: [" << (int)rs232_worker_ptr->parametr_X.buf16 << ", "
                 << (int)rs232_worker_ptr->parametr_Y.buf16  << " ,"
                 << (int)rs232_worker_ptr->cmdBuf[7] << ", "
                 << (int)rs232_worker_ptr->cmdBuf[8] << "];" << endl;
            if((int)rs232_worker_ptr->parametr_X.buf16 == 0)
            {
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 0) {cout << "tm_XY Disable!" << endl;}
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 1) {cout << "tm_XY Enable!" << endl;}
                tm_xy_enable = (int)rs232_worker_ptr->parametr_Y.buf16;
            } // END if((int)rs232_worker_ptr->parametr_X.buf16 == 0)

            if((int)rs232_worker_ptr->parametr_X.buf16 == 1)
            {
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 0){cout << "ABS ANGLES Disable!" << endl;}
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 1){cout << "ABS ANGLES FROM DMP I2C" << endl;}
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 2) {cout << "ABS ANGLES COMPLEX (rotate matrix mode: 1)" << endl;}
                abs_angle_enable = (int)rs232_worker_ptr->parametr_Y.buf16;
            } // END if((int)rs232_worker_ptr->parametr_X.buf16 == 1)

            if((int)rs232_worker_ptr->parametr_X.buf16 == 2)
            {
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 0) {cout << "ANGULAR VELOCITY: Disable!" << endl;}
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 1) {cout << "ANGULAR VELOCITY: Enable" << endl;}
                angular_velocity_enable = (int)rs232_worker_ptr->parametr_Y.buf16;
            } // END if((int)rs232_worker_ptr->parametr_X.buf16 == 2)
            if((int)rs232_worker_ptr->parametr_X.buf16 == 3)
            {
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 0) {cout << "MOTOR ANGLE: Disable!" << endl;} /// (????)
                if((int)rs232_worker_ptr->parametr_Y.buf16 == 1) {cout << "MOTOR ANGLE: Enable" << endl;}
            } // END if((int)rs232_worker_ptr->parametr_X.buf16 == 3)

            break;
        } //  END case TLM_SENDER_MODE_SWITCH
#endif // USE_TLM_MODE_CHANGER

#ifdef USE_UDP_TLM
        case((int)COMMAND_RS232::START_ATTACK):
        {
            cout << "Application::switch(receiveRS232_cmd):: START_ATTACK" << endl;
            if (tracShats->isInited() && flag_zahvat)
            {
                f_attack = 1;
            } // END if (tracShats->isInited() && flag_zahvat)
            else
            {
                cout << "ERROR START ATTACK:: NOT FOUND TRACK!" << endl;
            } // END else
            break;
        } //  END case START_ATTACK

        case((int)COMMAND_RS232::STOP_ATTACK):
        {
            cout << "Application::switch(receiveRS232_cmd):: STOP_ATTACK" << endl;
            f_attack = 0;
            break;
        } //  END case STOP_ATTACK
#endif // END USE_UDP_TLM


        default:
        {
            // cout << "Application::switch(receiveRS232_cmd):: UNKNOWN CMD!" << endl;
            break;
        } // END default

        } // END switch(rs232_cmd_type)

#ifdef USE_GUI
        receivedCMD_show = receivedCMD;
#endif // USE_GUI
        receivedCMD = 0;

    } // END if(receivedCMD)

    return;
} // END exec_rcv_rs232_cmd

void Application::transparent_image()
{
#ifdef USE_LOGGER
    time_point_cv0 = system_clock::now();
#endif // USE_LOGGER
#if defined(USE_TPV_cam)
    if(f_ir_black_heat)
    {
        bitwise_not(frame_receive, frame_receive);
    } // END if(f_ir_black_heat)
#endif  //END #if defined(USE_TPV_cam)
    if(f_image_enchance)
    {
        cvtColor(frame_receive, frame_receive, COLOR_BGR2YCrCb);
        vector<Mat> channels;
        split(frame_receive,channels);
        clahe_cv->apply(channels[0],channels[0]);
        merge(channels,frame_receive);
        cvtColor(frame_receive, frame_receive, COLOR_YCrCb2BGR);
    } // END if(f_image_enchance)

#ifdef USE_LOGGER
        /// Логгирование включается, если произодится дополнительная обработка изображения
    if(f_image_enchance || f_ir_black_heat)
    {
        time_point_cv1 = system_clock::now();
        LoggerArtem::inst().logTimedBasedFPS(
            "IMAGE_TRANSPARENT FPS = ",
            duration<double>(time_point_cv1 - time_point_cv0).count());
    } // END if(f_image_enchance || f_ir_black_heat)
#endif // USE_LOGGER

} // -- END transparent_image()


