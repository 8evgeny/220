#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

int main(int argc, char* argv[])
{
    std::string gst_rx_pipeline = 
        "udpsrc port=8080 ! "
        "application/x-rtp, media=(string)video, encoding-name=(string)H264, "
            "clock-rate=(int)90000, payload=(int)26 ! "
        "rtph264depay ! "
        "h264parse ! "
        "avdec_h264 ! "
        "videoconvert ! "
        "appsink max-buffers=3";

    cv::VideoCapture cap(gst_rx_pipeline, cv::CAP_GSTREAMER);

    // cap.set(cv::CAP_PROP_READ_TIMEOUT_MSEC, 3000); // opencv 4.8

    if(!cap.isOpened())
    {
        std::cout << "Failed to open pipeline" << std::endl;
        return 1;
    }

    cv::Mat fr_rcv, fr_show;

    std::string win_name = "app_client";
    cv::namedWindow(win_name);
    while(cap.isOpened())
    {
        cap.read(fr_rcv);
        cv::imshow(win_name, fr_rcv);
        char key = cv::waitKey(1);
        if(key == '`')
        {
            break;
        }
    }

    cap.release();
    std::cout << "END main" << std::endl;
    return 0;
} // -- END main
