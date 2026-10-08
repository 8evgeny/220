#include "cv_frame_show.hpp"

using namespace std;
using namespace cv;
using namespace chrono;

namespace cvw
{

CVFrameShow::CVFrameShow(){ std::cout << "Default constructor for CVFrameShow" << std::endl;     frame_receive = Mat(Size(frame_w,frame_h), CV_8UC3, clr_base);}

CVFrameShow::CVFrameShow(const std::string & config, bool & ok, std::string name_, CVMainWindow * main_window, cv::Rect2f rct_, std::function<void(MainWindowConstructor *, int, int)> func, MainWindowConstructor * mw_)
{
    config_path = config;
#ifdef USE_LOGGER
    bool create_logger_status;
    logger_artem::create(config_path, create_logger_status);
    if(!create_logger_status)
    {
        return;
    } // END if(!create_logger_status)
#endif //USE_LOGGER
    ok = get_ini_params(config_path, "main_settings");
    if(!ok)
    {
        cout << "CVFrameShow::ERROR read ini-params!" << endl;
        return;
    }
    name = name_;
    std::cout << "Create " << name << " in " << rct_ << std::endl;
    rct = Rect(round(rct_.x * main_window->getSize().width), round(rct_.y * main_window->getSize().height), round(rct_.width * main_window->getSize().width), round(rct_.height * main_window->getSize().height));
    frame_receive = Mat(Size(frame_w,frame_h), CV_8UC3, clr_base);
    main_window->add_mouse_rect(rct);

    func_mouse_handler = std::bind(&CVFrameShow::mouse_handler, this,  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    main_window->add_mouse_handler(func_mouse_handler);

    func_show = std::bind(&CVFrameShow::show, this,  std::placeholders::_1);
    main_window->add_child_show(func_show);
    mw_default = mw_;
    add_slot(func, mw_);

#ifdef USE_RESEND_RTSP
    int fps = 30;
    mat2rtsp_sender = make_shared<mat2rtsp>(config_path, ok, frame_w, frame_h, fps);
    if(!ok) {cout << "\n================================\n"
                     "ERROR create mat2rtsp_sender!\n================================\n";}
#endif // USE_RESEND_RTSP


} // -- END CVFrameShow

void CVFrameShow::show(cv::Mat & img)
{
    if(f_close)
    {
        f_close = false;
        if(!frame_receive.empty())
        {
            resize(frame_receive, img(rct), rct.size());
        } // END if(!frame_receive.empty())
    } // END if f_close
    if(device)
    {
        if(frame_receive.empty())
        {
            return;
        } // END if(!frame_receive.data)
        else
        {
            resize(frame_receive, img(rct), rct.size());
            if(show_win) {imshow(winname, frame_receive);}
        } // -- END CVFrameShow
    } // END if(device)
} // -- END show

bool CVFrameShow::start()
{
    cout << "Call start device" << endl;
    if(show_win) {namedWindow(winname, WINDOW_NORMAL);}
    bool ok = false;
    if (device != nullptr)
    {
        cout << "Device is inited!" << endl;
        return ok;
    } // END if (device != nullptr)

    switch(device_id)
    {
#ifdef USE_FOLDER_READER
    case FOLDER:
        device = make_shared<FolderReader>(config_path, ok);
        cout << "make_shared<FolderReader> OK=" << ok << endl;
        if(!ok)
        {
            device = nullptr;
            cout << "Not ok FolderReader!" << endl;
            return ok;
        }
        break;
#endif // END USE_FOLDER_READER

#ifdef USE_GST_PIPELINE_DEVICE
    case GST_PIPELINE_DEVICE:
        device = devices::gst_pipeline_device::create(config_path, ok, "NETWORK", location_rtsp);
        if(device == nullptr)
        {
            std::cout << "ERROR: create GOEN_400_RAW device failed" << std::endl;
            ok = false;
            device = nullptr;
            return ok;
        } // END if(device == nullptr)
        device->setup();
        device->start();
        break;
#endif // USE_GST_PIPELINE_DEVICE
    default:
        break;
    } // -- END switch

    if(device != nullptr)
    {
        if(!parallel && !(device_id == FOLDER || device_id == VIDEO))
        {
            parallel = true;
        } // END if(!parallel && !(device_id == FOLDER || device_id == VIDEO))
        if(parallel)
        {
            device->register_frame_handler(this);
            register_frame_handler(bind(&CVFrameShow::handleDeviceFrame, this,
                                        placeholders::_1, placeholders::_2,
                                        placeholders::_3, placeholders::_4,
                                        placeholders::_5));
        } // -- END if(parallel)
    } // END if(device != nullptr)
    else
    {
#ifdef USE_LOGGER
        LoggerArtem::inst().log("Device doesn't exist: choose correct device in configuration file or rebuild application with current device");
#endif //USE_LOGGER
    } // END if(device == nullptr)

    return true;
} // -- END device

bool CVFrameShow::stop()
{
    if(show_win) {if(!f_close) {destroyWindow(winname);}}
    if(device != nullptr)
    {
        cout << "\t --- BEGIN device->quit()" << endl;
        device->quit();
        device.reset();
        cout << "\t --- END device->quit()" << endl;
    } // END if(device != nullptr)
    else
    {
        cout << "Not found device to close" << endl;
    } // END else

    if(frame_receive.data)
    {
        frame_receive = clr_base;
    } // END if(frame_receive.data)
    f_close = true;

    cout << "Call stop device" << endl;
    return true;
} // -- END stop

bool CVFrameShow::get_state()
{
    if(device)
    {
        return device->getState();
    } // END if(device)
    else
    {
        return false;
    } // END if(!device)
} // -- END get_state

void CVFrameShow::setZahvatSize(int sz_)
{
    if(channel_id == 1) // TPV
    {
        zahvat_size_TPV = sz_;
        wh_2_zahvat_TPV = cv::Point(0.5 * sz_, 0.5 * sz_);
        if(zoom_val > 1.f)
        {
            trac_w_2 = wh_2_zahvat_TPV.x * TV_TPV_rel * zoom_val;
            trac_h_2  = wh_2_zahvat_TPV.y * TV_TPV_rel * zoom_val;
        } // END if(zoom_val > 1.f)
        else
        {
            trac_w_2 = wh_2_zahvat_TPV.x * TV_TPV_rel;
            trac_h_2  = wh_2_zahvat_TPV.y * TV_TPV_rel;
        } // END if(zoom_val <= 1.f)
    }   // END if (channel_id == 1)
    else
    {
        zahvat_size_TV = sz_;
        wh_2_zahvat_TV = cv::Point(0.5 * sz_, 0.5 * sz_);
        if(zoom_val > 1.f)
        {
            trac_w_2 = wh_2_zahvat_TV.x * zoom_val;
            trac_h_2 = wh_2_zahvat_TV.y * zoom_val;
        } // END if(zoom_val > 1.f)
        else
        {
            trac_w_2 = wh_2_zahvat_TV.x;
            trac_h_2 = wh_2_zahvat_TV.y;
        } // END if(zoom_val <= 1.f)
    } // END if (channel_id == 0)
} // -- END setZahvatSize

void CVFrameShow::setTracPos2Show(int pt_x, int pt_y)
{
    pt_trac2show=cv::Point(pt_x, pt_y);
    f_show_trac = true;
} // -- END setTracPos2Show

Size CVFrameShow::get_frame_size()
{
    if(device)
    {
        return Size(frame_w, frame_h);
    } // END if(device)
    else
    {
        return Size(0,0);
    } // END if(!device)

} // -- END get_frame_size


void CVFrameShow::mouse_handler(int &event, int x, int y)
{
    // -1 - за пределами, 0 - мышь наведена, 1 - мышь нажата, 2 - мышь отпущена, 3 - двойной клик.
    mouse_status = event;
    if(event == CVWidgetMouseEvent::LBTN_DOWN)
    {
        int x_rct = 0;
        int y_rct = 0;

        if(channel_id == 1) // if IR
        {
            x_rct = x - rct.x ;
            y_rct = y - rct.y ;
        } // END if(channel_id == 1) // if IR
        else
        {
            x_rct = x - rct.x;
            y_rct = y - rct.y;
        } // END else
        rct_mouse = Rect(x_rct - trac_w_2, y_rct - trac_h_2 , 2 * trac_w_2, 2 * trac_h_2);
        f_draw_mouse_pos = true;
    } // END if(event == 1)

    if(event == CVWidgetMouseEvent::LBTN_UP)
    {
        int x_rct = x - rct.x - rct.width * 0.5;
        int y_rct = -(y - rct.y - rct.height * 0.5);
        cout << "x_rct rel = " << x_rct * TV_w_1 << endl;
        cout << "y_rct rel = " << y_rct * TV_h_1 << endl;
        cout << "Click in " << Point(x_rct, y_rct) << endl;
        if(channel_id == 1)
        {
            cout << "TPV_w_bord_2 - trac_w_2 = " << TPV_w_bord_2 - trac_w_2 << endl;
            if((abs(x_rct) < (TPV_w_bord_2 - trac_w_2)) && (abs(y_rct) < 0.5 * TV_h))
            {
                slot(mw_default, x_rct, y_rct);
            } // END if(abs(x_rct) < 1375)
        } // END if(channel_id == 1)
        else
        {
            slot(mw_default, x_rct, y_rct);
        } // END if(channel_id == 0)
    } // END if(event == 2)
} // -- END mouse_handler

void CVFrameShow::handleDeviceFrame(uint8_t *f, int w, int h, int num, int id)
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
        cout << "first_frame = " << first_frame << endl;

        int cvmat_type = CV_8UC3;
        frame_proc_0_mutex.lock();
        frame_process_0 = Mat(Size(w,h), cvmat_type);
        device->getFormatedImage(f, w, h, id, frame_process_0);
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
            device->getFormatedImage(f, w, h, id, frame_process_1);
            frame_proc_1_mutex.unlock();
            break;

        case 1:
            frame_proc_0_mutex.lock();
            device->getFormatedImage(f, w, h, id, frame_process_0);
            frame_proc_0_mutex.unlock();
            break;

        default:
            throw runtime_error("Error: incorrect process_frame_id");
            break;
        } // END switch(process_frame_id)
        frameReady.store(true);
    } // -- END if(!frameReady.load())

    if(frameReady.load())
    {
        next_frame_async();
    } // END if(frameReady.load())
} // -- END handleDeviceFrame

void CVFrameShow::next_frame_sync()
{
    if(!parallel)
    {
#ifdef USE_LOGGER
        time_point_device0 = high_resolution_clock::now();
#endif // END USE_LOGGER

        device->getFrame(frame_receive);

#ifdef USE_LOGGER
        time_point_device1 = high_resolution_clock::now();
        LoggerArtem::inst().logTimedBasedFPS(
                    "Device FPS = ",
                    duration<double>(time_point_device1 - time_point_device0).count());
#endif // END USE_LOGGER
        frameReady.store(true);
    } // END if(!parallel)
    return;
} // END // -- END next_frame_sync

void CVFrameShow::next_frame_async()
{
    if(parallel) // TODO: паттерн "стратегия" с назначением lambda
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
            if(f_draw_mouse_pos)
            {
                rectangle(frame_receive, rct_mouse, Scalar(0,0,255), 2);
                drawMarker(frame_receive, Point(rct_mouse.x + trac_w_2, rct_mouse.y + trac_h_2), Scalar(0,0,255), MARKER_CROSS, 2 * trac_w_2, 1);
                f_draw_mouse_pos = false;
            } // END if(f_draw_mouse_pos)

            if(f_show_trac.load())
            {
                f_show_trac.store(false);
            } // END if(f_show_trac.load())

#ifdef USE_RESEND_RTSP
            mat2rtsp_sender->sendToRTSPServer(frame_receive);
#endif // USE_RESEND_RTSP
            frame_proc_1_mutex.unlock();
        } // END case 0:
            break;
        case 1:
        {
            frame_proc_0_mutex.lock();
            process_frame_id = 0;
            frame_receive = frame_process_0;
            if(f_draw_mouse_pos)
            {
                rectangle(frame_receive, rct_mouse, Scalar(0,0,255), 2);
                drawMarker(frame_receive, Point(rct_mouse.x + trac_w_2, rct_mouse.y + trac_h_2), Scalar(0,0,255), MARKER_CROSS, 2 * trac_w_2, 1);
                f_draw_mouse_pos = false;
            }  // END if(f_draw_mouse_pos)

            if(f_show_trac.load())
            {
                f_show_trac.store(false);
            } // END if(f_show_trac.load())
#ifdef USE_RESEND_RTSP
            mat2rtsp_sender->sendToRTSPServer(frame_receive);
#endif // USE_RESEND_RTSP
            frame_proc_0_mutex.unlock();
        } // END case 1:
            break;
        default:
        {
            throw runtime_error("Error: incorrect process_frame_id");
        } // END default:
            break;
        } // END switch(process_frame_id)
    } // -- END if(parallel)

    frameReady.store(false);
    return;
} // -- END next_frame_async

bool CVFrameShow::get_ini_params(const std::string & pathToSettings, const std::string & ini_section_name)
{
    bool ok = true;
    cout << "Begin MainWindow::get_ini_params in section [" << ini_section_name << "]\n";
    INIReader reader(pathToSettings);
    if (reader.ParseError() < 0)
    {
        cout << "CVFrameShow::ini reader parse error!\n";
        return false;
    } // END if (reader.ParseError() < 0)
    cout << "dbg:: ini_section_name = " << ini_section_name << endl;


    show_win = reader.GetInteger("main_settings", "show_win", -1);
    if(show_win == -1)
    {
        cout << "show_win not declared!\n";
        return 0;
    } // END if(show_win == -1)
    cout << "show_win = " << show_win << ";\n";

    parallel =  reader.GetBoolean("main_settings", "parallel", false);
    cout << "parallel = " << parallel << ";\n";

    device_id = reader.GetInteger("main_settings", "device_id", -1);
    if(device_id == -1)
    {
        cout << "device_id not declared!\n";
        return 0;
    } // END if(device_id == -1)
    cout << "device_id = " << device_id << ";\n";
    std::cout << std::boolalpha;

    zahvat_size_TV = reader.GetInteger("tracking", "zahvat_size_TV", -1);
    if(zahvat_size_TV == -1)
    {
        cout << "zahvat_size_TV not declared!\n";
        return 0;
    } // END if(zahvat_size_TV == -1)
    cout << "zahvat_size_TV = " << zahvat_size_TV << ";\n";
    wh_2_zahvat_TV = Point(round(0.5 * zahvat_size_TV), round(0.5 * zahvat_size_TV));

    zahvat_size_TPV = reader.GetInteger("tracking", "zahvat_size_TPV", -1);
    if(zahvat_size_TPV == -1)
    {
        cout << "zahvat_size_TPV not declared!\n";
        return 0;
    } // END if(zahvat_size_TPV == -1)
    cout << "zahvat_size_TPV = " << zahvat_size_TPV << ";\n";
    wh_2_zahvat_TPV = Point(round(0.5 * zahvat_size_TPV), round(0.5 * zahvat_size_TPV));

    device_id = reader.GetInteger("main_settings", "device_id", -1);
    if(device_id == -1)
    {
        cout << "device_id not declared!\n";
        return 0;
    } // END if(device_id == -1)
    cout << "device_id = " << device_id << ";\n";

    cout << "END MainWindow::get_ini_params: " << ok << endl;
#ifdef USE_GST_PIPELINE_DEVICE
    if(device_id == GST_PIPELINE_DEVICE)
    {
        string src = reader.Get("NETWORK", "src", "oops");

        send_ip = parce_IP(src);
        if(!send_ip.size())
        {
            send_ip = reader.Get("NETWORK", "send_ip", "oops");
        } // END if(!send_ip.size())
        cout << "======== send_IP ======== " << send_ip << "\n";

        if(src == "oops") {cout << "not found [gst_pipeline_device]: src!";}
        else
        {
            cout << "[gst_pipeline_device]:\nsrc = " << src << endl;
            location = "";
            int loc_id;
            loc_id = src.find("location=");
            if(loc_id >= 0)
            {
                loc_id += sizeof("location");
                for(int i = loc_id; i < src.size(); i++)
                {
                    if(src[loc_id] == ' ' ) {break;}
                    location.push_back((char)src.at(loc_id));
                    loc_id++;
                } // END for(int i = start_location_id; ; i++)
            } // END if(loc_id >= 0)
            else
            {
                location = "";
            } // END else
            cout << "get_ini_params::location = " << location << endl;
        } // END else
    } // END if(device_id == GST_PIPELINE_DEVICE)
#endif // USE_GST_PIPELINE_DEVICE
    return ok;
} // END get_ini_params

string CVFrameShow::parce_IP(const string & in)
{
    int pos_start = in.find("//");
    cout << "pos_start = " << pos_start << endl;
    if(pos_start >=  0)
    {
        pos_start += 2;
        int pos_end = in.find_last_of('.');
        while(isdigit(in.at(pos_end + 1))) {++pos_end;}
        string s_out = in.substr(pos_start, pos_end - pos_start + 1);
        return s_out;
    } // END if(pos_start >=  0)
    else
    {
        cout << "No symbol // in string!" << endl;
        return "";
    } // END if(pos_start < 0)
}// END string CVFrameShow::parce_IP(string in)
} // -- END namespace swv
