#ifndef UDP_ETH_GSTCV_RAW_VIDEO_TRANSMITTER
#define UDP_ETH_GSTCV_RAW_VIDEO_TRANSMITTER

// gstcv -- gstreamer + opencv videocapture / videowriter
// NOT thread safe!

#include <string>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

namespace eth_video::udp::raw::gstcv::transmitter
{
    struct Settings
    {
        std::string path2ini = "../settings.ini";
        std::string ini_section_name = "udp_eth_gstcv_raw_video_receiver";
        int dst_port = 8080;
        std::string dst_ip_v4 = "127.0.0.1";
        int frame_width = 640;
        int frame_height = 480;
        int max_buffers = 3 ;
    }; // -- Settings

    class UDPEthGstCVRawVideoTransmitter
    {
        public:
            UDPEthGstCVRawVideoTransmitter(const Settings &_settings);
            ~UDPEthGstCVRawVideoTransmitter();
            bool launch();
            bool quit();
            bool send_frame(const cv::Mat &rec_frame);
            std::string get_log_alias(); // возвращает алиас текущего модуля eth-приемника видео
        private:
            UDPEthGstCVRawVideoTransmitter();
            bool configure_pipeline();
            bool launch_transmit();
            std::string log_module_alias = "UDPEthGstCVRawVideoTransmitter";
            Settings settings;
            cv::Mat frame_transmit_prepare, frame_transmit_intermediate;
            std::string tx_pipeline;
            bool execute = false;
            cv::VideoWriter cv_writer;
    }; // -- class UDPEthGstCVRawVideoTransmitter
} // namespace eth_video::udp::raw::gstcv::transmitter


#endif //UDP_ETH_GSTCV_RAW_VIDEO_TRANSMITTER