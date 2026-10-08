#include "udp_eth_gstcv_raw_video_transmitter.hpp"

#include <iostream>
#include <opencv2/imgproc.hpp>

namespace eth_video::udp::raw::gstcv::transmitter
{
    UDPEthGstCVRawVideoTransmitter::UDPEthGstCVRawVideoTransmitter()
    {

    }
    
    UDPEthGstCVRawVideoTransmitter::UDPEthGstCVRawVideoTransmitter(const Settings &_settings)
    {
        log_module_alias = "[" + settings.ini_section_name + "] UDPEthGstCVRawVideoTransmitter";
        this->settings = _settings;
    }

    UDPEthGstCVRawVideoTransmitter::~UDPEthGstCVRawVideoTransmitter()
    {
        std::cout << "Деструктор " << log_module_alias << std::endl; 
    } // -- END ~UDPEthGstCVRawVideoTransmitter

    bool UDPEthGstCVRawVideoTransmitter::launch()
    {
        std::cout << "BEGIN " << log_module_alias << "::launch()" << std::endl;
        bool launch_report = true;
        if(!configure_pipeline())
        {
            launch_report = false;
            std::cout << "Error configure_pipeline" << std::endl;
        }
        if(!launch_transmit())
        {
            launch_report = false;
            std::cout << "Error launch_receive" << std::endl;
        }
        std::cout << "END " << log_module_alias << "::launch()" << std::endl;
        return launch_report;
    }

    bool UDPEthGstCVRawVideoTransmitter::quit()
    {
        std::cout << "BEGIN " << log_module_alias << "::quit()" << std::endl;
        cv_writer.release();
        std::cout << "END " << log_module_alias << "::quit()" << std::endl;
        return true;
    } // -- END quit()

    bool UDPEthGstCVRawVideoTransmitter::send_frame(const cv::Mat &frame_tx)
    {
        if(frame_tx.empty())
        {
            return false;
        }
        #if defined(CCM_8UC1)
            if(frame_tx.channels() == 1)
            {
                frame_transmit_intermediate = frame_tx;
            }
            else if(frame_tx.channels() == 3)
            {
                cv::cvtColor(frame_tx, frame_transmit_intermediate, cv::COLOR_BGR2GRAY);
            }
            else
            {
                return false;
            }
        #elif defined(CCM_8UC3)
            if(frame_tx.channels() == 3)
            {
                frame_transmit_intermediate = frame_tx;
            }
            else if(frame_tx.channels() == 1)
            {
                cv::cvtColor(frame_tx, frame_transmit_intermediate, cv::COLOR_GRAY2BGR);
            }
            else
            {
                return false;
            }
        #else
            throw std::runtime_error("Not supported color space for output format");
        #endif // defined CCM_8UC1 defined CCM_8UC3

        // if(frame_transmit_intermediate.size() != cv::Size(settings.frame_width, settings.frame_height));
        // {
        //     cv::resize(frame_transmit_intermediate, frame_transmit_prepare, cv::Size(settings.frame_width, settings.frame_height));
        // }
        // else
        // {
        //     frame_transmit_prepare = frame_transmit_intermediate;
        // }
        // if(!cv_writer.write(frame_transmit_prepare))
        // {
        //     return false;
        // }
        // return true;
    } // -- END get_frame

} // namespace eth_video::udp::raw::gstcv::transmitter
