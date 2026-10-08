#include "gstcv_mjpg_eth_controller.hpp"
#include <iostream>
#include "INIReader.h"
#include "eth_cnt_common_utils/eth_cnt_common_utils.hpp"
#include "toolbox_common_utils/toolbox_common_utils.hpp"
#include <opencv2/imgproc.hpp>

using namespace toolbox_utils;

namespace rtp_eth::controller_V1
{
    MJPGEthController::MJPGEthController(const RTPEthCntSettings &_settings) : settings(_settings)
    {
        if(settings.tx_fps == 0)
        {
            min_send_frame_delay_ms = 0;
        }
        else
        {
            min_send_frame_delay_ms = 1000 / settings.tx_fps;
        }
        tp_send_frame = std::chrono::system_clock::now();
    } // -- END MJPGEthController::MJPGEthController(...)

    bool MJPGEthController::configureTransmissionPipeline()
    {
        gst_TX = "appsrc ! queue max-size-buffers=" + std::to_string(settings.gst_tx_max_size_buffers) 
            + " ! videoconvert ! video/x-raw,format=YUY2 ! jpegenc ! rtpjpegpay ! udpsink host=" 
            + settings.dst_ip_v4 
            + " port=" + std::to_string(settings.dst_port);
        std::cout << "MJPGEthController -- TX gst pipeline [section_name = " << settings.ini_section_name << "]: " << std::endl;
        std::cout << "\t " << gst_TX << std::endl;
        return true;
    } // -- END configureTransmissionPipeline

    bool MJPGEthController::configureReceivePipeline()
    {
        gst_RX = "udpsrc port=" 
        + std::to_string(settings.src_port) + " ! application/x-rtp, media=video, encoding-name=JPEG, framerate="
        + std::to_string(settings.gst_rx_frame_rate) + "/1, payload=" 
        + std::to_string(settings.gst_rx_payload) +", clock-rate=" 
        + std::to_string(settings.gst_rx_clock_rate) + " ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink max-buffers="
        + std::to_string(settings.rx_max_buffers) + " drop=True";
        std::cout << "MJPGEthController -- RX gst pipeline [section_name = " << settings.ini_section_name << "]: " << std::endl;
        std::cout << "\t " << gst_RX << std::endl;
        return true;
    } // -- END configureReceivePipeline

    bool MJPGEthController::launch()
    {
        std::cout << "BEGIN MJPGEthController::launch()" << std::endl;

        bool launch_status = true;

        // launch transmitter if enable
        if(settings.communication_mode == eth_common::CommunicationModes::SIMPLEX_TRANSMIT ||
            settings.communication_mode == eth_common::CommunicationModes::DUPLEX)
        {
            configureTransmissionPipeline();
            if(launchTransmission())
            {
                execute_tx = true;
            }
            else
            {
                launch_status = false;
            }
        } // communication mode is SIMPLEX_TRANSMIT or DUPLEX 

        if(settings.communication_mode == eth_common::CommunicationModes::SIMPLEX_RECEIVE ||
            settings.communication_mode == eth_common::CommunicationModes::DUPLEX)
        {            
            configureReceivePipeline();
            if(launchReceive())
            {
                execute_rx = true;
            }
            else
            {
                launch_status = false;
            }
        }

        std::cout << "END MJPGEthController::launch()" << std::endl;
        return launch_status;
    } // -- END launch()

    bool MJPGEthController::launchTransmission()
    {
        bool isColour = false; // true - send BGR frames, false - send GRAY frames

        #if defined(CCM_8UC1)
            isColour = false;
        #elif defined(CCM_8UC3)
            isColour = true;
        #else
            throw std::runtime_error("Not supported color space for output format");
        #endif

        writer_transmit = cv::VideoWriter(gst_TX, cv::CAP_GSTREAMER, 0, settings.tx_fps, settings.txFrameSize, isColour);
        if(!writer_transmit.isOpened())
        {
            std::cout << "Error: MJPGEthController::TX " << settings.ini_section_name << "failed open gstreamer pipeline" << std::endl;
            return false;
        }
        else
        {
            std::cout << "MJPGEthController::TX " << settings.ini_section_name << "success open gstreamer pipeline" << std::endl;
        }
        return true;
    } // -- END launchTransmission

    bool MJPGEthController::launchReceive()
    {
        #if defined(CCM_8UC1)
            frame_receive_raw = cv::Mat(settings.rxFrameSize, CV_8UC3);
        #endif

        cap_receive = cv::VideoCapture(gst_RX, cv::CAP_GSTREAMER);
        if(!cap_receive.isOpened())
        {
            std::cout << "Error: MJPGEthController::RX " << settings.ini_section_name << "failed open gstreamer pipeline" << std::endl;
            return false;
        }
        else
        {
            std::cout << "MJPGEthController::RX " << settings.ini_section_name << "success open gstreamer pipeline" << std::endl;
        }
        return true;
    } // -- END launchReceive

    bool MJPGEthController::quit()
    {
        std::cout << "BEGIN MJPGEthController::quit()" << std::endl;
        cap_receive.release();
        writer_transmit.release();
        execute_rx = false;
        execute_tx = false;
        std::cout << "END MJPGEthController::quit()" << std::endl;
        return true;
    } // -- END quit()

    bool MJPGEthController::send_frame(const cv::Mat& frame)
    {
        if(execute_tx)
        {
            double delay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp_send_frame).count();
            if(delay > min_send_frame_delay_ms)
            {
                writer_transmit.write(frame);
                tp_send_frame = std::chrono::system_clock::now();
                return true;
            }
            return false;
        }
        return false;
    } // -- END send_frame

    bool MJPGEthController::get_frame(cv::Mat &_frame_receive)
    {
        if(execute_rx)
        {
            #if defined(CCM_8UC1)
                if(!cap_receive.read(frame_receive_raw))
                {
                    return false;
                }
                cv::cvtColor(frame_receive_raw, _frame_receive, cv::COLOR_BGR2GRAY);
                return true;
            #elif defined(CCM_8UC3)
                return cap_receive.read(_frame_receive);
            #else
                throw std::runtime_error("Not supported color space for output format");
            #endif
            return false;
        }
        return false;
    } // -- END get_frame
} // namespace rtp_eth::controller_V1