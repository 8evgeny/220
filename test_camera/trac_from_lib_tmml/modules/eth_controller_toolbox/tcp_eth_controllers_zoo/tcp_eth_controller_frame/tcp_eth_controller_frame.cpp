#include "tcp_eth_controller_frame.hpp"
#include "INIReader.h"
#include <thread>

namespace eth_controller::frame
{
    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        EthCntSettingsFrame &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN EthController::frame read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" << path2ini << "\"" << std::endl; 
            return false;
        }

        settings.max_transmit_fps = reader.GetInteger(ini_section_name, "max_transmit_frames_fps", -1);
        if(settings.max_transmit_fps == -1)
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : max_transmit_frames_fps error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t max_transmit_frames_fps = " << settings.max_transmit_fps << std::endl;
        }
        std::cout << "END EthController::frame read ini settings" << std::endl;
        return true;
    } // -- END getIniSettings

    bool checkValidIniSettings(const EthCntSettingsFrame &settings)
    {
        return true;
    } // -- END checkValidIniSettings

    std::shared_ptr<EthControllerFrame> create(const std::string &path2ini, const std::string &sectionName)
    {
        eth_controller::base::EthCntSettingsBase base_settings;
        bool report_read_ini_base = 
            eth_controller::base::getIniSettings(path2ini, sectionName, base_settings);
        bool report_check_ini_base = eth_controller::base::checkValidIniSettings(base_settings);

        eth_controller::frame::EthCntSettingsFrame frame_settings;
        bool report_read_ini_frame = 
            eth_controller::frame::getIniSettings(path2ini, sectionName, frame_settings);
        bool report_check_ini_frame = eth_controller::frame::checkValidIniSettings(frame_settings);

        std::shared_ptr<tcp_client_simplex::frame::TCPClientSimplexFrame> client_simplex_tmp 
            = tcp_client_simplex::frame::create(path2ini, sectionName);
        std::shared_ptr<tcp_server_simplex::frame::TCPServerSimplexFrame> server_simplex_tmp 
            = tcp_server_simplex::frame::create(path2ini, sectionName); 

        bool success_create_simplex_client = (client_simplex_tmp != nullptr);
        bool success_create_simplex_server = (server_simplex_tmp != nullptr);

        if(!(
            report_read_ini_base && 
            report_check_ini_base &&
            report_read_ini_frame &&
            report_check_ini_frame &&
            success_create_simplex_client &&
            success_create_simplex_server))
        {
            std::cout << "Error create EthControllerFrame" << std::endl;
            return nullptr;
        }

        base_settings.content_type = eth_common::EthControllerType::FRAME;

        std::shared_ptr<EthControllerFrame> eth_controller = 
            std::make_shared<EthControllerFrame>(
                frame_settings, 
                base_settings, 
                server_simplex_tmp, 
                client_simplex_tmp);
        return eth_controller;
    } // -- END create

    EthControllerFrame::EthControllerFrame()
    {

    } // -- END EthControllerFrame
    
    EthControllerFrame::EthControllerFrame(const EthCntSettingsFrame &settings_frame, 
                const eth_controller::base::EthCntSettingsBase &settings_base,
                std::shared_ptr<tcp_server_simplex::frame::TCPServerSimplexFrame> simplex_server,
                std::shared_ptr<tcp_client_simplex::frame::TCPClientSimplexFrame> simplex_client) 
    {
        this->settings_base = settings_base;
        this->settings_frame = settings_frame;

        this->client_simplex = simplex_client;
        this->server_simplex = simplex_server;

        this->client_simplex_base = std::static_pointer_cast<tcp_client_simplex::base::TCPClientSimplexBase>(client_simplex);
        this->server_simplex_base = std::static_pointer_cast<tcp_server_simplex::base::TCPServerSimplexBase>(server_simplex);

        min_send_frame_delay_ms = 1000.f / settings_frame.max_transmit_fps;
        tp_send_frame = std::chrono::system_clock::now();
    } // -- END EthControllerFrame

    bool EthControllerFrame::launch()
    {
        return eth_controller::base::EthControllerBase::launch();
    }

    EthControllerFrame::~EthControllerFrame()
    {
        std::cout << "--- Деструктор EthControllerFrame" << std::endl;
    } // -- END ~EthControllerFrame

    bool EthControllerFrame::quit()
    {
        return eth_controller::base::EthControllerBase::quit();
    } // -- END EthControllerFrame::quit()

    bool EthControllerFrame::send_frame(const cv::Mat& frame, const cv::Point& obj_predict_danger)
    {
        double delay = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - tp_send_frame).count();
        if(delay > min_send_frame_delay_ms)
        {
            bool report = client_simplex->send_frame(frame, obj_predict_danger);
            tp_send_frame = std::chrono::system_clock::now();
            return report;
        }
        return false;
    } // -- END send_frame

    bool EthControllerFrame::wait_receive_frame(cv::Mat &receive_frame)
    {
        return server_simplex->wait_receive_frame(receive_frame);
    } // -- END wait_receive_frame
}
