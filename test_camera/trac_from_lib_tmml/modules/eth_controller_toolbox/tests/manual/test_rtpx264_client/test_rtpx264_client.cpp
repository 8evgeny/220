#include "video_eth_controllers/gstc/rtp_client_x264/rtp_client.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <iostream>

int main()
{
    rtp::client::SettingsPtr settings = std::make_shared<rtp::client::Settings>();
    int mat_type_alloc = settings->video.format == "BGR" ? CV_8UC3 : CV_8UC1;
    cv::Mat frame(cv::Size(settings->video.width, settings->video.height), mat_type_alloc, cv::Scalar::all(0));
    rtp::RtpClient client(settings);
    client.setup();
    client.start();

    unsigned int cnt = 0;
    while (client.is_execute())
    {
        if (client.frame(frame))
        {
            cv::imshow("Frame", frame);
            char key = cv::waitKey(1);
            if(key == '`')
            {
                break;
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }
    }
}