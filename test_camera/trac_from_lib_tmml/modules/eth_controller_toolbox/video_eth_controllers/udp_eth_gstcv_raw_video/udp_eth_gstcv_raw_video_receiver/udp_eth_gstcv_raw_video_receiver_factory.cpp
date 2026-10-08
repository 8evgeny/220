#include "udp_eth_gstcv_raw_video_receiver_factory.hpp"

#include "INIReader.h"
#include "toolbox_common_utils/toolbox_common_utils.hpp"
#include "eth_cnt_common_utils/eth_cnt_common_utils.hpp"

#include <iostream>

using namespace toolbox_utils;

namespace eth_video::udp::raw::gstcv::receiver
{
    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &ini_section_name, 
        Settings &settings)
    {
        std::cout << "BEGIN UDPEthGstCVRawVideoReceiver " 
            << ini_section_name << " read ini settings" << std::endl;
        INIReader reader(path2ini);
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" << path2ini << "\"" << std::endl; 
            return false;
        }

        settings.path2ini = path2ini;
        settings.ini_section_name = ini_section_name;

        bool parse_ini_report = true;

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "receiver_port", settings.receiver_port);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "receive_frame_w", settings.frame_width);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "receive_frame_h", settings.frame_height);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "clock_rate", settings.clock_rate);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "max_buffers", settings.max_buffers);

        std::cout << "END UDPEthGstCVRawVideoReceiver " 
            << ini_section_name << " read ini settings" << std::endl;
        return parse_ini_report;
    } // -- END getIniSettings
        
    bool checkValidSettings(const Settings &settings)
    {
        if(settings.max_buffers < 0)
        {
            std::cout << "Error: [" << settings.ini_section_name << "] : max_buffers < 0" << std::endl;
            return false;
        }
        return true;
    } // -- END checkValidSettings

    std::shared_ptr<UDPEthGstCVRawVideoReceiver> create(
        const std::string &path2ini,
        const std::string &ini_section_name)
    {
        Settings settings;
        if(!getIniSettings(path2ini, ini_section_name, settings))
        {
            std::cout << "Error getIniSettings for UDPEthGstCVRawVideoReceiver : ini_section = [" << ini_section_name << "]" << std::endl;
            return nullptr;
        }

        if(!checkValidSettings(settings))
        {
            std::cout << "Error checkValidIniSettings for UDPEthGstCVRawVideoReceiver : ini_section = [" << ini_section_name << "]" << std::endl;
            return nullptr;
        }

        std::shared_ptr<UDPEthGstCVRawVideoReceiver> udp_gstcv_raw_video_receiver 
            = std::make_shared<UDPEthGstCVRawVideoReceiver>(settings);
        return udp_gstcv_raw_video_receiver;
    } // -- END create
} // namespace eth_video::udp::raw::gstcv::receiver
