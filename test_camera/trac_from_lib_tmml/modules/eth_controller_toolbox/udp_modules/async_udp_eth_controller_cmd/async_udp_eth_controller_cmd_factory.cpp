#include "async_udp_eth_controller_cmd_factory.hpp"

#include "toolbox_common_utils/toolbox_common_utils.hpp"
#include "INIReader.h"
#include "eth_cnt_common_utils/eth_cnt_common_utils.hpp"

#include "async_udp_client_simplex_cmd/async_udp_client_simplex_cmd.hpp"
#include "async_udp_server_simplex_cmd/async_udp_server_simplex_cmd.hpp"

#include <memory>
#include <iostream>

namespace eth_controller::cmd::async::udp
{
    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        AsyncUDPEthControllerCmdSettings &settings)
    {
        std::cout << "BEGIN readIniSettings AsyncUdpEthControllerCmd::" << ini_section_name << std::endl;
        INIReader reader(path2ini);
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" + path2ini + "\"" << std::endl;
            return false;
        }

        bool read_ini_status = true;

        std::string communication_mode_tmp;
        read_ini_status &= toolbox_utils::ini_reader_tool::Get(
            reader, ini_section_name, "communication_mode", communication_mode_tmp);

        read_ini_status &= eth_common::utils::convertCommunicationModeStr2enum(
            communication_mode_tmp, settings.communication_mode);

        if(settings.communication_mode == eth_common::CommunicationModes::SIMPLEX_RECEIVE ||
            settings.communication_mode == eth_common::CommunicationModes::DUPLEX)
        {
            read_ini_status &= toolbox_utils::ini_reader_tool::Get(
            reader, ini_section_name, "src_ip_v4", settings.src_ip_v4);
            read_ini_status &= toolbox_utils::ini_reader_tool::GetInteger(
            reader, ini_section_name, "src_port", settings.src_port);       
        }

        if(settings.communication_mode == eth_common::CommunicationModes::SIMPLEX_TRANSMIT ||
            settings.communication_mode == eth_common::CommunicationModes::DUPLEX)
        {
            read_ini_status &= toolbox_utils::ini_reader_tool::Get(
            reader, ini_section_name, "dst_ip_v4", settings.dst_ip_v4);
            read_ini_status &= toolbox_utils::ini_reader_tool::GetInteger(
            reader, ini_section_name, "dst_port", settings.dst_port);
            read_ini_status &= toolbox_utils::ini_reader_tool::GetInteger(
            reader, ini_section_name, "cmd_tx_loop_delay_ms", settings.cmd_tx_loop_delay_ms);    
        }

        // base 
        settings.path2ini = path2ini;
        settings.ini_section_name = ini_section_name;

        read_ini_status &= toolbox_utils::ini_reader_tool::Get(
            reader, ini_section_name, "node_alias", settings.node_alias);

        if(!read_ini_status)
        {
            std::cout << "Error read ini parameters!" << std::endl;
        }
        std::cout << "END readIniSettings AsyncUdpEthControllerCmd::" << ini_section_name << std::endl;
        return read_ini_status;
    } // -- END getIniSettings
    
    bool checkValidIniSettings(const AsyncUDPEthControllerCmdSettings &settings)
    {
        return true;
    } // -- END checkValidIniSettings

    std::shared_ptr<AsyncUDPEthControllerCmd> create(const std::string &path2ini, const std::string &sectionName)
    {
        AsyncUDPEthControllerCmdSettings settings;

        bool report_read_ini_cmd = true, report_check_ini_cmd = true;
        if(!getIniSettings(path2ini, sectionName, settings))
        {
            std::cout << "Error: getIniSettings(...) called from eth_controller::cmd::async::create(...) return false!" << std::endl;
            report_read_ini_cmd = false;
            return nullptr;
        }

        if(!checkValidIniSettings(settings))
        {
            std::cout << "Error: checkValidIniSettings(...) called from eth_controller::cmd::async::create(...) return false!" << std::endl;
            report_check_ini_cmd = false;
            return nullptr;
        }

        eth_controller::cmd::async::udp::server_simplex::AsyncUDPClientSimplexCmdSettings server_settings;
        eth_controller::cmd::async::udp::client_simplex::AsyncUDPClientSimplexCmdSettings client_settings;

        std::shared_ptr<eth_controller::cmd::async::udp::server_simplex::AsyncUDPServerSimplexCmd> server_smp_ptr = nullptr;
        if(settings.communication_mode == eth_common::CommunicationModes::SIMPLEX_RECEIVE ||
            settings.communication_mode == eth_common::CommunicationModes::DUPLEX)
        {
            server_settings.node_alias = settings.node_alias;
            server_settings.server_ip_v4 = settings.src_ip_v4;
            server_settings.server_port = settings.src_port;
            server_settings.single_command_buf_size_byte = settings.single_command_buf_size_byte;
            server_smp_ptr = std::make_shared<eth_controller::cmd::async::udp::server_simplex::AsyncUDPServerSimplexCmd>(server_settings);
            if(server_smp_ptr == nullptr)
            {
                std::cout << "ERROR: create simplex server failed" << std::endl;
                return nullptr;
            }
        }

        std::shared_ptr<eth_controller::cmd::async::udp::client_simplex::AsyncUDPClientSimplexCmd> client_smp_ptr = nullptr;
        if(settings.communication_mode == eth_common::CommunicationModes::SIMPLEX_TRANSMIT ||
            settings.communication_mode == eth_common::CommunicationModes::DUPLEX)
        {
            client_settings.node_alias = settings.node_alias;
            client_settings.server_ip_v4 = settings.dst_ip_v4;
            client_settings.server_port = settings.dst_port;
            client_settings.single_command_buf_size_byte = settings.single_command_buf_size_byte;
            client_settings.cmd_tx_loop_delay_ms = settings.cmd_tx_loop_delay_ms;
            client_smp_ptr = std::make_shared<eth_controller::cmd::async::udp::client_simplex::AsyncUDPClientSimplexCmd>(client_settings);
            if(client_smp_ptr == nullptr)
            {
                std::cout << "ERROR: create simplex client failed" << std::endl;
            }
        }

        std::shared_ptr<AsyncUDPEthControllerCmd> cnt_ptr 
            = std::make_shared<AsyncUDPEthControllerCmd>(
                settings, client_smp_ptr, server_smp_ptr);
        return cnt_ptr;
    } // -- END create


} // -- namespace eth_controller::cmd::async::udp