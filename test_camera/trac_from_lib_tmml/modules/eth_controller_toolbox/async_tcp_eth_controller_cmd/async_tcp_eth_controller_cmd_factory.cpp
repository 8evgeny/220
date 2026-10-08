#include "async_tcp_eth_controller_cmd_factory.hpp"
#include "toolbox_common_utils/toolbox_common_utils.hpp"
#include "INIReader.h"
#include "eth_cnt_common_utils/eth_cnt_common_utils.hpp"

#include "../tcp_client_simplex_zoo/tcp_client_simplex_cmd/tcp_client_simplex_cmd.hpp"
#include "../tcp_server_simplex_zoo/tcp_server_simplex_cmd/tcp_server_simplex_cmd.hpp"

#include <memory>

namespace eth_controller::cmd::async
{
    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        AsEthCntSettingsCmd &settings)
    {
        std::cout << "BEGIN readIniSettings AsyncTcpEthControllerCmd::" << ini_section_name << std::endl;
        INIReader reader(path2ini);
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" + path2ini + "\"" << std::endl;
            return false;
        
        }
        bool read_ini_status = true;

        //net
        read_ini_status &= toolbox_utils::ini_reader_tool::Get(
            reader, ini_section_name, "src_ip_v4", settings.src_ip_v4);
        read_ini_status &= toolbox_utils::ini_reader_tool::GetInteger(
            reader, ini_section_name, "src_port", settings.src_port);
        read_ini_status &= toolbox_utils::ini_reader_tool::Get(
            reader, ini_section_name, "dst_ip_v4", settings.dst_ip_v4);
        read_ini_status &= toolbox_utils::ini_reader_tool::GetInteger(
            reader, ini_section_name, "dst_port", settings.dst_port);

        // client_simplex

        read_ini_status &= toolbox_utils::ini_reader_tool::GetInteger(
            reader, ini_section_name, "cmd_tx_loop_delay_ms", settings.cmd_tx_loop_delay_ms);

        // base 
        settings.path2ini = path2ini;
        settings.ini_section_name = ini_section_name;
        read_ini_status &= toolbox_utils::ini_reader_tool::Get(
            reader, ini_section_name, "node_alias", settings.node_alias);
        
        std::string communication_mode_tmp;
        read_ini_status &= toolbox_utils::ini_reader_tool::Get(
            reader, ini_section_name, "communication_mode", communication_mode_tmp);

        read_ini_status &= eth_common::utils::convertCommunicationModeStr2enum(
            communication_mode_tmp, settings.communication_mode);
        

        if(!read_ini_status)
        {
            std::cout << "Error read ini parameters!" << std::endl;
        }
        std::cout << "END readIniSettings AsyncTcpEthControllerCmd::" << ini_section_name << std::endl;
        return read_ini_status;
    } // -- END getIniSettings

    bool checkValidIniSettings(const AsEthCntSettingsCmd &settings)
    {
        return true;
    } // -- END checkValidIniSettings

    std::shared_ptr<AsyncTcpEthControllerCmd> create(const std::string &path2ini, const std::string &sectionName)
    {
        AsEthCntSettingsCmd settings;

        bool report_read_ini_cmd = true, report_check_ini_cmd = true;
        if(!getIniSettings(path2ini, sectionName, settings))
        {
            std::cout << "Error: getIniSettings(...) called from eth_controller::cmd::async::create(...) return false!" << std::endl;
            report_read_ini_cmd = false;
        }

        if(!checkValidIniSettings(settings))
        {
            std::cout << "Error: checkValidIniSettings(...) called from eth_controller::cmd::async::create(...) return false!" << std::endl;
            report_check_ini_cmd = false;
        }

        eth_controller::cmd::async::server_simplex::Settings server_settings;
        eth_controller::cmd::async::client_simplex::Settings client_settings;

        server_settings.node_alias = settings.node_alias;
        server_settings.server_ip_v4 = settings.src_ip_v4;
        server_settings.server_port = settings.src_port;
        server_settings.single_command_buf_size_byte = settings.single_command_buf_size_byte;

        client_settings.node_alias = settings.node_alias;
        client_settings.server_ip_v4 = settings.dst_ip_v4;
        client_settings.server_port = settings.dst_port;
        client_settings.single_command_buf_size_byte = settings.single_command_buf_size_byte;
        client_settings.cmd_tx_loop_delay_ms = settings.cmd_tx_loop_delay_ms;

        std::shared_ptr<eth_controller::cmd::async::client_simplex::AsyncTcpClientSimplexCmd> client_smp_ptr = 
            std::make_shared<eth_controller::cmd::async::client_simplex::AsyncTcpClientSimplexCmd>(client_settings);
        
        std::shared_ptr<eth_controller::cmd::async::server_simplex::AsyncTcpServerSimplexCmd> server_smp_ptr = 
            std::make_shared<eth_controller::cmd::async::server_simplex::AsyncTcpServerSimplexCmd>(server_settings);
        
        if(!(
            report_read_ini_cmd &&
            report_check_ini_cmd &&
            client_smp_ptr != nullptr &&
            server_smp_ptr != nullptr))
        {
            std::cout << "Error create EthControllerCmd" << std::endl;
            return nullptr;
        }

        std::shared_ptr<AsyncTcpEthControllerCmd> cnt_ptr 
            = std::make_shared<AsyncTcpEthControllerCmd>(
                settings, client_smp_ptr, server_smp_ptr);
        return cnt_ptr;
    } // -- END create
} // -- namespace eth_controller::cmd::async