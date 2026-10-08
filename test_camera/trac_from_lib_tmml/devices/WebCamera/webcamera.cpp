#include "webcamera.hpp"

using namespace std;
using namespace cv;

WebCamera::~WebCamera()
{
    cout << "Деструктор WebCamera : Device" << endl;
    cap_ptr->release();
} // -- END WebCamera::~WebCamera()

WebCamera::WebCamera(const string& path2ini, bool& ok, int & dev_fps)
{
    cout << "Begin Constructor WebCamera\n";
    ok = getSettings(path2ini);
    if(!ok){cout << "Not readIniSettings!!!\n"; return;}
    ok = start_cam();
    if(!ok){cout << "Not start_cam!!!\n"; return;}
    dev_fps = fps;
    tp_get_fr = chrono::high_resolution_clock::now();
    thread execution(&WebCamera::exec, this);
    execution.detach();
    cout << "END Constructor WebCamera\n";
} // -- END WebCamera

bool WebCamera::start_cam()
{
    cap_ptr = make_unique<VideoCapture>();
    if(cap_ptr == nullptr){cout << "Not make_unique<VideoCapture>!\n"; return 0;}
    bool setup_report = true;
    if(backend_id == BACKEND_V4L2)
    {
        setup_report &= cap_ptr->open(webcamera_id, CAP_V4L2);
        cap_ptr->set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    }
    else if(backend_id == BACKEND_GSTREAMER){setup_report &= cap_ptr->open(webcamera_id, CAP_GSTREAMER);}
    else{setup_report &= cap_ptr->open(webcamera_id, CAP_ANY);}

    if(!cap_ptr->isOpened()){cout << "Not opened First Frame!\n"; return 0;}

    cap_ptr->set(CAP_PROP_FRAME_WIDTH, frame_w);
    cap_ptr->set(CAP_PROP_FRAME_HEIGHT, frame_h);
    cap_ptr->set(CAP_PROP_FPS, fps);
    cap_ptr->set(CAP_PROP_BUFFERSIZE, 3);    
    cap_ptr->set(CAP_PROP_CHANNEL, channels_out);
    int channels_get = cap_ptr->get(CAP_PROP_CHANNEL);

    Mat img_buf;
    if(!cap_ptr->read(img_buf)){cout << "Don't read FIRST FRAME!\n"; return 0;}
    frame_w = img_buf.cols;
    frame_h = img_buf.rows;
    int fps_real = cap_ptr->get(CAP_PROP_FPS);
    if(fps_real != -1){fps = fps_real;}
    channels_in = img_buf.channels();    
    cout << "WebCamera FIRST FRAME SIZE=[" << frame_w << ", " << frame_h << "]; channels_in=";
    cout << channels_in << "; channels_out=" << channels_out << "; channels_get=" << channels_get;
    cout << "; fps_real=" << fps_real << "; fps=" << fps << "\n";
    if(channels_in == 3 && channels_out == 3){change_color = 0;}
    else if(channels_in == 1 && channels_out == 1){change_color = 0;}
    else if(channels_in == 3 && channels_out == 1){change_color = COLOR_BGR2GRAY;}
    else if(channels_in == 1 && channels_out == 3){change_color = COLOR_GRAY2BGR;}    
    else{cout << "change_color ERROR!\n"; return 0;}
#ifdef GUI_OK
    if(change_color){cvtColor(img_buf, img_buf, change_color);}
    imshow("first_frame", img_buf);
    unsigned char key = waitKey(1000);
    destroyWindow("first_frame");
    if(key == '`'){return 0;}
#endif // END #ifdef GUI_OK
    return 1;
} // -- END start_cam

void WebCamera::getFrame(Mat& fr)
{
    frame_id++;
    cap_ptr->read(fr);
    if(!frame.data){cout << "NOT data for frame" << endl; return;}
#if defined(CCM_8UC1)
    if(fr.channels() == 3){cvtColor(fr, frame, COLOR_BGR2GRAY);}
#elif defined(CCM_8UC3)
    if(fr.channels() == 1){cvtColor(fr, frame, COLOR_GRAY2BGR);}
#endif
    frame_ready.store(true, memory_order_release);
    frameCV.notify_one();
} // -- END getFrame

bool WebCamera::getSettings(const string &path_to_ini)
{
    INIReader reader(path_to_ini);
    if(reader.ParseError() < 0){cout << "ini reader: Parse error\n"; return 0;}

    frame_w = reader.GetInteger("webcamera", "frame_w", -1);
    if(frame_w == -1){cout << "frame_w ini error\n"; return 0;}

    frame_h = reader.GetInteger("webcamera", "frame_h", -1);
    if(frame_h == -1){cout << "frame_h ini error\n"; return 0;}

    fps = reader.GetInteger("webcamera", "fps", -1);
    if(fps == -1){cout << "fps ini error\n"; return 0;}
    sourceFrameDelay_ms = 1000 / fps;

    webcamera_id = reader.GetInteger("webcamera", "webcamera_id", -1);
    if(webcamera_id == -1){cout << "webcamera_id ini error\n"; return 0;}
    cout << "WebCamera get settings success" << endl;

    backend_id = reader.GetInteger("webcamera", "backend_id", -1);
    if(backend_id == -1){cout << "backend_id ini error\n"; return 0;}

    channels_out = reader.GetInteger("webcamera", "channels_out", -1);
    if(channels_out == -1){cout << "channels_out ini error\n"; return 0;}
    return 1;
} // END getSettings

// ====================================== NOT USE:

void WebCamera::keyHandler(uchar &key){return void();} // END keyHandler

int WebCamera::getColorChannels()
{
#ifdef CCM_8UC1
    return 1;
#elif CCM_8UC3
    return 3;
#endif // CCM_8UC1/3
} // END getColorChannels

bool WebCamera::isBayerColorChannel(){return false;} // END isBayerColorChannel

void WebCamera::workflow(){ }

uint8_t *WebCamera::receiveFrame(int &w, int &h, int &id, int &num)
{
    if(frame_ready.load())
    {
        frame_mutex.lock();
        w = frame.cols;
        h = frame.rows;
        return frame.data;
        frame_mutex.unlock();
        frame_ready.store(false);
    } // END if(frame_ready.load())
    return nullptr;
} // -- END receiveFrame

int WebCamera::getFrameCounter(){return frameCnt;} // -- END getFrameCounter

void WebCamera::getFormatedImage(uint8_t * dat, int w, int h, int id, cv::Mat & image)
{
#if defined(CCM_8UC1)
    memcpy(image.data, dat, image.total());
#elif defined(CCM_8UC3)
    memcpy(image.data, dat, 3*image.total());
#else
    throw std::runtime_error("Not supported color space for output format");
#endif
} // END getFormatedImage

void WebCamera::quit()
{
    _execute.store(false, memory_order_release);
    this_thread::sleep_for(chrono::milliseconds(sourceFrameDelay_ms * 2));
} // -- END quit

int WebCamera::getId(){return frame_id;} // END -- getId

void WebCamera::exec()
{
    _execute.store(true, memory_order_release);
    handle_frame_execute.store(true, memory_order_release);
    thread frameHandler(&WebCamera::runHandleFrame, this);
    WatchDog timer(1000);
    bool success = 0;
    while(_execute.load(memory_order_acquire))
    {
        chrono::system_clock::time_point timePoint = chrono::system_clock::now() + chrono::milliseconds(sourceFrameDelay_ms);
        success = getNextFrame(frame);
        if(!success)
        {
            break;
        } // END if(!success)
        this_thread::sleep_until(timePoint);
        // this_thread::sleep_for(chrono::milliseconds(sourceFrameDelay_ms));
    } // -- END while(_execute.load(memory_order_acquire))
    handle_frame_execute.store(false, memory_order_release);
    frameCV.notify_one();
    frameHandler.join();
    quit();
} // -- END exec

bool WebCamera::getNextFrame(Mat& fr)
{
        frame_id++;
        cap_ptr->read(fr);
        if(!frame.data){cout << "NOT data for frame" << endl; return 1;}
#if defined(CCM_8UC1)
        if(fr.channels() == 3){cvtColor(fr, frame, COLOR_BGR2GRAY);}
#elif defined(CCM_8UC3)
        if(fr.channels() == 1){cvtColor(fr, frame, COLOR_GRAY2BGR);}
#endif
        frame_ready.store(true, memory_order_release);
        frameCV.notify_one();
        return 1;
} // -- END getNextFrame

void WebCamera::runHandleFrame()
{
    while(handle_frame_execute.load(memory_order_acquire))
    {
        unique_lock<mutex> lk(frame_mutex);
        frameCV.wait(lk);
        if(!handle_frame_execute.load(memory_order_acquire)){break;}
        for(auto handler : frame_handlers)
        {
            handler->handle(frame.data, frame.cols, frame.rows, 0, 0);
        } // -- END for(auto handler : frame_handlers)
        lk.unlock();
        frame_ready.store(false, memory_order_release);
    } // -- END while
} // -- END runHandleFrame
