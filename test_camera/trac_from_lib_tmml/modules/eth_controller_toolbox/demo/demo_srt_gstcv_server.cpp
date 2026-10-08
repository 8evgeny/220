#include <iostream>
#include "video_eth_controllers/srt/gstcv_srt_server/gstcv_srt_server.hpp"
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <thread>
#include <signal.h>

using namespace gstcv;

std::function<void(int)> quit_handler = [](int sig){std::cout << "WARNING: empty quit_handler" << std::endl;};
void systemSignalsHandler(int signal) { quit_handler(signal);} // оболочка над лямбда-выражением

std::string path2video = "/home/vitaly/data_sets/vehicle_traffic_2_720p.mp4";

int main(int argc, char* argv[])
{
    std::atomic<bool> need_quit = {false};
    quit_handler = [&need_quit](int sig)
    {
        std::cout << "\t" << "\033[1;31mNOTE: catch system signal ---> begin close app\033[0m" << std::endl;
        signal(sig, SIG_DFL);
        need_quit.store(true);
    }; // -- quit_handler
    signal(SIGINT,  systemSignalsHandler);
    signal(SIGABRT, systemSignalsHandler);
    signal(SIGTERM, systemSignalsHandler);
    #ifdef __linux__
    signal(SIGKILL, systemSignalsHandler);
    signal(SIGQUIT, systemSignalsHandler);
    signal(SIGSTOP, systemSignalsHandler);
    #endif // __linux__

    SRTServerCfg srv_cfg;
    srv_cfg.frame_size = {1920, 1080};
    srv_cfg.fps = 25;
    srv_cfg.is_color = true;
    srv_cfg.srv_uri = "srt://:8888";
    bool srv_ok = false;
    SRTServer server(srv_cfg, srv_ok);
    if(!srv_ok)
    {
        return 1;
    }

    cv::VideoCapture cap(path2video);
    if(!cap.isOpened())
    {
        return 1;
    }
    
    cv::Mat fr_rd, fr_snd;
    std::string win_name = "app_server";
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);

    while(cap.read(fr_rd))
    {
        cv::resize(fr_rd, fr_snd, srv_cfg.frame_size);
        if(!server.sendFrame(fr_snd))
        {
            break;
        }
        cv::imshow(win_name, fr_snd);
        char key = cv::waitKey(1);
        if(key == '`')
        {
            break;
        }
        if(need_quit.load())
        {
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }

    server.quit();
    std::cout << "END main" << std::endl;
    return 0;
} // -- END main
