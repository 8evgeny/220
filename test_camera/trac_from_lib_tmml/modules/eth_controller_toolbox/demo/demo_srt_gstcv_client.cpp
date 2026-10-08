#include <iostream>
#include "video_eth_controllers/srt/gstcv_srt_client/gstcv_srt_client.hpp"
#include <opencv2/highgui.hpp>
#include <atomic>
#include <signal.h>


using namespace gstcv;


std::function<void(int)> quit_handler = [](int sig){std::cout << "WARNING: empty quit_handler" << std::endl;};
void systemSignalsHandler(int signal) { quit_handler(signal);} // оболочка над лямбда-выражением

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

    SRTClientCfg cli_cfg;
    cli_cfg.srv_uri = "srt://192.168.1.25:8888";
    cli_cfg.max_buffers = 1;
    bool ok_cli = false;
    SRTClient client(cli_cfg, ok_cli);

    cv::Mat frame_rcv;
    std::string win_name = "srt_client";
    cv::namedWindow(win_name, cv::WINDOW_NORMAL);
    while(true)
    {
        if(!client.getFrame(frame_rcv))
        {
            break;
        }
        cv::imshow(win_name, frame_rcv);
        char key = cv::waitKey(1);
        if(key == '`')
        {
            break;
        }
        if(need_quit.load())
        {
            break;
        }
    }
    client.quit();
    std::cout << "END main" << std::endl;
    return 0;
} // -- END main
