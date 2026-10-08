#ifndef UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER_HPP
#define UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER_HPP

// gstcv -- gstreamer + opencv videocapture / videowriter
// NOT thread safe!

#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

namespace eth_video::udp::raw::gstcv::receiver
{
    struct Settings
    {
        std::string path2ini = "../settings.ini";
        std::string ini_section_name = "udp_eth_gstcv_raw_video_receiver";
        int receiver_port = 8080;
        int frame_width = 640;
        int frame_height = 480;
        int clock_rate = 90'000;
        int max_buffers = 3 ;
    }; // -- Settings

    class UDPEthGstCVRawVideoReceiver
    {
        public:
            UDPEthGstCVRawVideoReceiver(const Settings &_settings);
            ~UDPEthGstCVRawVideoReceiver();
            bool launch();
            bool quit();
            bool get_frame(cv::Mat &rec_frame);
            std::string get_log_alias(); // возвращает алиас текущего модуля eth-приемника видео
        private:
            UDPEthGstCVRawVideoReceiver();
            bool configure_pipeline();
            bool launch_receive();
            std::string log_module_alias = "UDPEthGstCVRawVideoReceiver";
            Settings settings;
            cv::Mat frame_receive, frame_receive_raw;
            std::string rx_pipeline;
            bool execute = false;
            cv::VideoCapture cap_receive;
    }; // -- class UDPEthGstCVRawVideoReceiver
} // namespace eth_video::udp::raw::gstcv::receiver

#endif //UDP_ETH_GSTCV_RAW_VIDEO_RECEIVER_HPP