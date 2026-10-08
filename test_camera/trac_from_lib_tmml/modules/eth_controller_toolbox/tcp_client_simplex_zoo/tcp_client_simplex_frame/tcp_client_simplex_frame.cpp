#include "tcp_client_simplex_frame.hpp"

#include "INIReader.h"
#include <thread>
#include <memory>

namespace tcp_client_simplex::frame
{
    TCPClientSimplexFrame::TCPClientSimplexFrame(
        const tcp_client_simplex::base::NetworkSettings &_network_settings,
        const RXTX_FrameSettings &_rxtx_frame_settings) :
                TCPClientSimplexBase(_network_settings),
                rxtx_frame_settings(_rxtx_frame_settings)
    {

    }

    TCPClientSimplexFrame::TCPClientSimplexFrame()
    {

    }
    TCPClientSimplexFrame::~TCPClientSimplexFrame()
    {
        close(sockfd);
    }

    bool TCPClientSimplexFrame::send_frame(const cv::Mat& frame, const cv::Point& obj_predict_danger)
    {
        if(frame.empty())
        {
            return false;
        }

        // preprocessing: size & num channels
        if(frame.size() != rxtx_frame_settings.transmit_frame_size)
        {
            cv::resize(frame, frame_send, rxtx_frame_settings.transmit_frame_size, cv::InterpolationFlags::INTER_NEAREST);
        }
        else
        {
            frame_send = frame;
        }


        // if(frame_send.channels() == 3 && rxtx_frame_settings.transmit_num_ch == 1)
        // {
        //     cv::cvtColor(frame_send, frame_send, cv::COLOR_BGR2GRAY);
        // }
        // else if(frame_send.channels() == 1 && rxtx_frame_settings.transmit_num_ch == 3)
        // {
        //     cv::cvtColor(frame_send, frame_send, cv::COLOR_GRAY2BGR);
        // }


        int p00 = obj_predict_danger.x/256;
        int p01 = obj_predict_danger.x - p00*256;
        int p10 = obj_predict_danger.y/256;
        int p11 = obj_predict_danger.y - p10*256;

#if defined(CCM_8UC1)
        frame_send.at<uchar>(0, 0) = p00;
        frame_send.at<uchar>(0, 1) = p01;
        frame_send.at<uchar>(1, 0) = p10;
        frame_send.at<uchar>(1, 1) = p11;
#elif defined(CCM_8UC3)
        frame_send.at<cv::Vec3b>(0, 0)[0] = p00;
        frame_send.at<cv::Vec3b>(0, 1)[0] = p01;
        frame_send.at<cv::Vec3b>(1, 0)[0] = p10;
        frame_send.at<cv::Vec3b>(1, 1)[0] = p11;
#else
        throw std::runtime_error("Error: not recognize/supported color channel mode");
#endif

        int img_size = frame_send.total() * frame_send.elemSize();
        //auto tp_begin = std::chrono::high_resolution_clock::now();
        int bytes = send(sockfd, frame_send.data, img_size, 0);
        if(bytes != img_size)
        {
            std::cout << "TCPClientSimplex: error send frame" << std::endl;
            return false;
        }
        //auto tp_end = std::chrono::high_resolution_clock::now();
        // std::cout << "TCPClientSimplex: Success send frame. Total send time [mcs] = " <<
        //     std::chrono::duration_cast<std::chrono::microseconds>(tp_end-tp_begin).count()
        //     << std::endl;
        return true;
    } // -- END send_frame

     bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        RXTX_FrameSettings &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN " << sectionName << " read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            throw std::runtime_error("INIReader ParseError ini file path=\"" + path2ini + "\"");
        }

        settings.transmit_frame_size.width = reader.GetInteger(sectionName, "transmit_frame_w", -1);
        if(settings.transmit_frame_size.width == -1)
        {
            std::cout <<"\t INIReader parse [" << sectionName << "] : transmit_frame_w error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t transmit_frame_w = " << settings.transmit_frame_size.width << std::endl;
        }

        settings.transmit_frame_size.height = reader.GetInteger(sectionName, "transmit_frame_h", -1);
        if(settings.transmit_frame_size.width == -1)
        {
            std::cout <<"\t INIReader parse [" << sectionName << "] : transmit_frame_h error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t transmit_frame_h = " << settings.transmit_frame_size.height << std::endl;
        }

        std::cout << "END " << sectionName << " read ini settings" << std::endl;
        return true;
    }

    bool checkValidIniSettings(const RXTX_FrameSettings &settings)
    {
        // check valid ipv4 : exist 3 dot, all nums in range [0 .. 255]
        // check valid port (>0) and maybe another tests
        return true;
    }

    std::shared_ptr<TCPClientSimplexFrame> create(
        const std::string &path2ini,
        const std::string &sectionName)
    {
        std::cout << "BEGIN create " << sectionName << std::endl;
        // чтение настроек базового класса
        tcp_client_simplex::base::NetworkSettings net_settings;
        bool report_read_ini_base = 
            tcp_client_simplex::base::getIniSettings(path2ini, sectionName, net_settings);
        bool report_check_ini_settings_base = 
            tcp_client_simplex::base::checkValidIniSettings(net_settings);

        // чтение настроек дочернего класса
        tcp_client_simplex::frame::RXTX_FrameSettings frame_settings;
        bool report_read_ini_frame = 
            tcp_client_simplex::frame::getIniSettings(path2ini, sectionName, frame_settings);
        bool report_check_ini_settings_frame =
            tcp_client_simplex::frame::checkValidIniSettings(frame_settings);
        
        if(!(
            report_read_ini_base &&
            report_check_ini_settings_base &&
            report_read_ini_frame &&
            report_check_ini_settings_frame))
        {
            std::cout << "Error create TCPClientSimplexFrame" << std::endl;
            return nullptr;
        }

        std::shared_ptr<TCPClientSimplexFrame> tcp_client_simplex_frame_ptr = 
            std::make_shared<TCPClientSimplexFrame>(
                net_settings, frame_settings);
        
        std::cout << "END create " << sectionName << std::endl;
        return tcp_client_simplex_frame_ptr;
    }
}
