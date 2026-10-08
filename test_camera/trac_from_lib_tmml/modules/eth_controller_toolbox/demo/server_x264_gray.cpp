#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
    std::string path2video = "/home/vitaly/data_sets/vehicle_traffic_2_720p.mp4";

    int tx_fps = 40; 
    cv::Size tx_frame_size = {1920,1080};
    bool is_color = false;

    cv::VideoCapture cap(path2video);

    cv::Mat receive_frame, show_frame, send_frame_intermediate, send_frame;

    std::string gst_tx_pipe = 
        "appsrc max-buffers=3 !"
        "videoconvert ! " 
        "video/x-raw, width=(int)1920, height=(int)1080, framerate=(fraction)40/1 ! " 
        "x264enc tune=zerolatency bitrate=8192 speed-preset=medium !" 
        "rtph264pay mtu=1400 ! udpsink host=192.168.0.103 port=8080";


    cv::VideoWriter writer = cv::VideoWriter(gst_tx_pipe, cv::CAP_GSTREAMER, 0, tx_fps, tx_frame_size, is_color);
    
    if(!writer.isOpened())
    {
        std::cout << "Failed open tx pipeline" << std::endl;
        return 1;
    }
    
    std::string win_name = "App_tx";
    cv::namedWindow(win_name);
    while(cap.read(receive_frame))
    {
        // cv::resize(receive_frame, show_frame, frame_show_size);
        cv::resize(receive_frame, send_frame_intermediate, tx_frame_size);
        // cv::cvtColor(send_frame_intermediate, send_frame, cv::COLOR_BGR2YUV_I420);
        cv::cvtColor(send_frame_intermediate, send_frame, cv::COLOR_BGR2GRAY); 
        // std::cout << send_frame.size() << std::endl;
        writer.write(send_frame);
        // cv::imshow(win_name, send_frame);
        char key = cv::waitKey(1);
        if(key == '`')
        {
            break;
        }
    }

    cap.release();
    writer.release();
    cv::destroyAllWindows();

    std::cout << "END main" << std::endl;
return 0;
}
