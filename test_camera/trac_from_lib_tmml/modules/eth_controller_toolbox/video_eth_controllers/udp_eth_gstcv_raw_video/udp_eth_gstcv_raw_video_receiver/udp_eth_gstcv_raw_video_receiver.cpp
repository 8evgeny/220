#include "udp_eth_gstcv_raw_video_receiver.hpp"

#include <iostream>
#include <opencv2/imgproc.hpp>

namespace eth_video::udp::raw::gstcv::receiver
{
    UDPEthGstCVRawVideoReceiver::UDPEthGstCVRawVideoReceiver()
    {

    } // -- END UDPEthGstCVRawVideoReceiver()

    UDPEthGstCVRawVideoReceiver::UDPEthGstCVRawVideoReceiver(const Settings &_settings)
    {
        this->settings = _settings;
        log_module_alias = "[" + settings.ini_section_name + "] UDPEthGstCVRawVideoReceiver";
    } // -- END UDPEthGstCVRawVideoReceiver(const Settings &_settings)

    UDPEthGstCVRawVideoReceiver::~UDPEthGstCVRawVideoReceiver()
    {
        std::cout << "Деструктор " << log_module_alias << std::endl; 
    } // -- END ~UDPEthGstCVRawVideoReceiver

    bool UDPEthGstCVRawVideoReceiver::launch()
    {
        std::cout << "BEGIN " << log_module_alias << "::launch()" << std::endl;
        bool launch_report = true;
        if(!configure_pipeline())
        {
            launch_report = false;
            std::cout << "Error configure_pipeline" << std::endl;
        }
        if(!launch_receive())
        {
            launch_report = false;
            std::cout << "Error launch_receive" << std::endl;
        }
        std::cout << "END " << log_module_alias << "::launch()" << std::endl;
        return launch_report;
    } // -- END launch()

    bool UDPEthGstCVRawVideoReceiver::quit()
    {
        std::cout << "BEGIN " << log_module_alias << "::quit()" << std::endl;
        cap_receive.release();
        std::cout << "END " << log_module_alias << "::quit()" << std::endl;
        return true;
    } // -- END quit()

    bool UDPEthGstCVRawVideoReceiver::get_frame(cv::Mat &rec_frame)
    {
        #if defined(CCM_8UC1)
            if(!cap_receive.read(frame_receive_raw))
            {
                return false;
            }
            cv::cvtColor(frame_receive_raw, rec_frame, cv::COLOR_BGR2GRAY);
            return true;
        #elif defined(CCM_8UC3)
            return cap_receive.read(rec_frame);
        #else
            throw std::runtime_error("Not supported color space for output format");
        #endif
        return false;
    } // -- END get_frame

    std::string UDPEthGstCVRawVideoReceiver::get_log_alias()
    {
        return log_module_alias;
    } // -- END get_log_alias

    bool UDPEthGstCVRawVideoReceiver::configure_pipeline()
    {
        rx_pipeline = "udpsrc port=" + std::to_string(settings.receiver_port) + 
            " ! application/x-rtp, media=(string)video, clock-rate=(int)" + std::to_string(settings.clock_rate) + 
            ", encoding-name=(string)RAW, sampling=(string)YCbCr-4:2:0, depth=8, width=(string)" + std::to_string(settings.frame_width) + 
            ", height=(string)" + std::to_string(settings.frame_height) + 
            " ! rtpvrawdepay ! videoconvert ! appsink max-buffers=" + std::to_string(settings.max_buffers) + 
            " drop=True";
        std::cout << log_module_alias << " rx_pipeline = " << rx_pipeline << std::endl;
        return true;
    } // -- END configure_pipeline

    bool UDPEthGstCVRawVideoReceiver::launch_receive()
    {
        #if defined(CCM_8UC1)
            frame_receive_raw = cv::Mat(cv::Size(settings.frame_width, settings.frame_height), CV_8UC3);
        #endif

        cap_receive = cv::VideoCapture(rx_pipeline, cv::CAP_GSTREAMER);
        if(!cap_receive.isOpened())
        {
            std::cout << "Error " << log_module_alias << "failed open gst rx pipeline" << std::endl;
            return false;
        }
        else
        {
            std::cout << log_module_alias << "success open gstreamer pipeline" << std::endl;
        }
        return true;
    } // -- END launch_receive
} // -- namespace eth_video::udp::raw::gstcv::receiver