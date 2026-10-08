#include "gstcv_mjpg_eth_controller_factory.hpp"

#include "INIReader.h"
#include "iostream"
#include "eth_cnt_common_utils/eth_cnt_common_utils.hpp"
#include "toolbox_common_utils/toolbox_common_utils.hpp"

using namespace toolbox_utils;

namespace rtp_eth::controller_V1
{
    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &ini_section_name, 
        RTPEthCntSettings &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN MJPGEthController " 
            << ini_section_name << " read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" << path2ini << "\"" << std::endl; 
            return false;
        }

        settings.path2ini = path2ini;
        settings.ini_section_name = ini_section_name;

        settings.node_alias = reader.Get(ini_section_name, "node_alias", "oops");
        if(settings.node_alias == "oops")
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : node_alias error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t node_alias = " << settings.node_alias << std::endl;
        }

        std::string communication_mode_tmp = reader.Get(ini_section_name, "communication_mode", "oops");
        if(communication_mode_tmp == "oops")
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : communication_mode error" << std::endl;
            return false;
        }
        else
        {
            if(!eth_common::utils::convertCommunicationModeStr2enum(
                communication_mode_tmp, settings.communication_mode))
            {
                std::cout << "Error: not recognize \"communication_mode\" = \"" << communication_mode_tmp << "\"" << std::endl;
                return false;
            }
            std::cout << "\t communication_mode = " << communication_mode_tmp << std::endl;
        }

        settings.src_ip_v4 = reader.Get(ini_section_name, "src_ip_v4", "oops");
        if(settings.src_ip_v4 == "oops")
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : src_ip_v4 error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t src_ip_v4 = " << settings.src_ip_v4 << std::endl;
        }

        settings.src_port = reader.GetInteger(ini_section_name, "src_port", -1);
        if(settings.src_port == -1)
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : src_port error" << std::endl;
        }
        else
        {
            std::cout << "\t src_port = " << settings.src_port << std::endl;
        }

        settings.dst_ip_v4 = reader.Get(ini_section_name, "dst_ip_v4", "oops");
        if(settings.dst_ip_v4 == "oops")
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : dst_ip_v4 error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t dst_ip_v4 = " << settings.dst_ip_v4 << std::endl;
        }

        settings.dst_port = reader.GetInteger(ini_section_name, "dst_port", -1);
        if(settings.dst_port == -1)
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : dst_port error" << std::endl;
        }
        else
        {
            std::cout << "\t src_port = " << settings.dst_port << std::endl;
        }

        settings.txFrameSize.width = reader.GetInteger(
            ini_section_name, "transmit_frame_w", -1);
        if(settings.txFrameSize.width == -1)
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : transmit_frame_w error" << std::endl;
        }
        else
        {
            std::cout << "\t transmit_frame_w = " << settings.txFrameSize.width << std::endl;
        }


        settings.txFrameSize.height = reader.GetInteger(
            ini_section_name, "transmit_frame_h", -1);
        if(settings.txFrameSize.height == -1)
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : transmit_frame_w error" << std::endl;
        }
        else
        {
            std::cout << "\t transmit_frame_h = " << settings.txFrameSize.height << std::endl;
        }

        // test new technique work with inireader in simple case
        bool parse_ini_report = true;

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "tx_fps", settings.tx_fps);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "receive_frame_w", settings.rxFrameSize.width);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "receive_frame_h", settings.rxFrameSize.height);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "rx_max_buffers", settings.rx_max_buffers);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "gst_tx_max_size_buffers", settings.gst_tx_max_size_buffers);

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "gst_rx_frame_rate", settings.gst_rx_frame_rate);
        
        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "gst_rx_clock_rate", settings.gst_rx_clock_rate);     

        parse_ini_report &= ini_reader_tool::GetInteger(
            reader, ini_section_name, "gst_rx_payload", settings.gst_rx_payload);
        
        if(!parse_ini_report)
        {
            std::cout << "INIReader parse error" << std::endl;
        }

        std::cout << "END MJPGEthController " 
            << ini_section_name << " read ini settings" << std::endl; 
        return true;
    } // -- END getIniSettings

    bool checkValidIniSettings(RTPEthCntSettings &settings)
    {
        if(settings.gst_tx_max_size_buffers < 0)
        {
            std::cout << "Error: gst_tx_max_size_buffers < 0" << std::endl;
            return false;
        }
        return true;
    } // -- END checkValidIniSettings

    std::shared_ptr<MJPGEthController> create(
        const std::string &path2ini, 
        const std::string &ini_section_name)
    {
        RTPEthCntSettings settings;
        if(!getIniSettings(path2ini, ini_section_name, settings))
        {
            std::cout << "Error: MJPGEthController::getIniSettings(...) [section_name = " << ini_section_name << "] return false!" << std::endl;
            return nullptr;
        }

        if(!checkValidIniSettings(settings))
        {
            std::cout << "Error: MJPGEthController::checkValidIniSettings(...) [section_name = " << ini_section_name << "] return false!" << std::endl;
            return nullptr;
        }
        std::shared_ptr<MJPGEthController> rtp_eth_cnt_v1_ptr = std::make_shared<MJPGEthController>(settings);
        return rtp_eth_cnt_v1_ptr;
    } // -- END create
}
