#include "video_eth_controllers/gstc/rtp_server_x264/rtp_server.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <iostream>
#include <chrono>

std::string source_video_path = "/home/vitaly/data_sets/vehicle_traffic_2_720p.mp4";
cv::Size frame_show_size = {640, 480};
int main()
{
    rtp::server::SettingsPtr settings = std::make_shared<rtp::server::Settings>();
    rtp::RtpServer server(settings);
    server.setup();
    server.start();

    unsigned int cnt = 0;

    cv::VideoCapture cap_source(source_video_path);
    if(!cap_source.isOpened())
    {
        std::cout << "Error: cap_source not open!" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "cap_source open success!" << std::endl;
    }
    
    cv::Mat frame_receive, frame_show, frame_prepared;
    cv::Size frame_tx_size = cv::Size(settings->video.width, settings->video.height);
    
    std::chrono::system_clock::time_point tp_runtime;
    uint32_t duration_delay_ms = 1000 / settings->video.framerate;
    cv::namedWindow("app_tx");
    char key;

    int n_ch_ppl = -1;
    if(settings->video.format == "GRAY8")
    {
        n_ch_ppl = 1;
    }
    else if (settings->video.format == "BGR")
    {
        n_ch_ppl = 3;
    }
    else
    {
        return 1;
    }

    while (cap_source.read(frame_receive))
    {
        // tp_runtime = std::chrono::high_resolution_clock::now();
        cv::resize(frame_receive, frame_prepared, frame_tx_size);
        cv::resize(frame_receive, frame_show, frame_show_size);
        if(n_ch_ppl == 1)
        {
            cv::cvtColor(frame_prepared, frame_prepared, cv::COLOR_BGR2GRAY);
        }
        server.newFrame(frame_prepared.data, frame_prepared.cols * frame_prepared.rows* frame_prepared.channels());
        cv::imshow("app_tx", frame_show);
        key = cv::waitKey(1);
        if(key == '`')
        {
            break;
        }
        // std::this_thread::sleep_until(tp_runtime + std::chrono::milliseconds(duration_delay_ms));
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    std::cout << "End execution" << std::endl;
    server.stop();
    std::cout << "Pipeline was stop" << std::endl;
    cap_source.release();
    std::cout << "END main" << std::endl;

}