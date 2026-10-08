#include "tcp_client_simplex_cmd.hpp"
#include "INIReader.h"
#include <thread>
#include <memory>

namespace tcp_client_simplex::cmd
{
    TCPClientSimplexCmd::TCPClientSimplexCmd(
        tcp_client_simplex::base::NetworkSettings _network_settings, 
        tcp_client_simplex::cmd::RXTX_CmdSettings _rxtx_cmd_settings) : 
            tcp_client_simplex::base::TCPClientSimplexBase(_network_settings), 
            rxtx_cmd_settings(_rxtx_cmd_settings)
    {
        cmd_bytebuffer = new uint8_t[rxtx_cmd_settings.single_command_buf_size_byte];
    } // -- END TCPClientSimplexCmd

    TCPClientSimplexCmd::TCPClientSimplexCmd()
    {
        //pass
    }

    TCPClientSimplexCmd::~TCPClientSimplexCmd()
    {
        close(sockfd);
    } // -- END ~TCPClientSimplexCmd

    bool TCPClientSimplexCmd::quit()
    {
        close(sockfd);
        return true;
    } // -- END quit

    bool TCPClientSimplexCmd::send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> eth_cmd)
    {
        int buff_size_tmp;
        eth_cmd->serialize_noalloc(cmd_bytebuffer, buff_size_tmp);
        int bytes = send(sockfd, cmd_bytebuffer, buff_size_tmp, 0);
        if(bytes != buff_size_tmp)
        {
            std::cout << "TCPClientSimplexCmd: error send cmd" << std::endl;
            return false;
        }
        return true;
    } // -- END send_cmd

    void TCPClientSimplexCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        this->notify_handlers.push_back(handler);
    } // -- END register_notify_handler

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &sectionName,
        RXTX_CmdSettings &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN " << sectionName << " simplex_client::frame read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" + path2ini + "\"" << std::endl;
            return false;
        }

        settings.single_command_buf_size_byte = reader.GetInteger(sectionName, "single_command_buf_size_byte", -1);
        if(settings.single_command_buf_size_byte == -1)
        {
            std::cout << "INIReader parse [" << sectionName << "] : single_command_buf_size_byte error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\t single_command_buf_size_byte = " << settings.single_command_buf_size_byte << std::endl;
        }

        std::cout << "END " << sectionName << " simplex_client::frame read ini settings" << std::endl;
        return true;
    } // -- END getIniSettings

    bool checkValidIniSettings(const RXTX_CmdSettings &settings)
    {
        // check valid ipv4 : exist 3 dot, all nums in range [0 .. 255]
        // check valid port (>0) and maybe another tests
        return true;
    } // -- END checkValidIniSettings

    std::shared_ptr<TCPClientSimplexCmd> create(
        const std::string &path2ini,
        const std::string &sectionName)
    {
        std::cout << "BEGIN create " << sectionName << "::simplex_client" << std::endl;
        // чтение настроек базового класса
        tcp_client_simplex::base::NetworkSettings net_settings;
        bool report_read_ini_base = 
            tcp_client_simplex::base::getIniSettings(path2ini, sectionName, net_settings);
        bool report_check_ini_settings_base = 
            tcp_client_simplex::base::checkValidIniSettings(net_settings);
        
        tcp_client_simplex::cmd::RXTX_CmdSettings cmd_settings;
        bool report_read_ini_cmd = 
            tcp_client_simplex::cmd::getIniSettings(path2ini, sectionName, cmd_settings);
        bool report_check_ini_settings_cmd = 
            tcp_client_simplex::cmd::checkValidIniSettings(cmd_settings);
        

        if(!(
            report_read_ini_base &&
            report_check_ini_settings_base &&
            report_read_ini_cmd &&
            report_check_ini_settings_cmd))
        {
            std::cout << "Error create TCPClientSimplexCmd" << std::endl;
            return nullptr;
        }

        std::shared_ptr<TCPClientSimplexCmd> tcp_client_simplex_cmd_ptr = 
            std::make_shared<TCPClientSimplexCmd>(
                net_settings, cmd_settings);
        
        std::cout << "END create " << sectionName << "::simplex_client" << std::endl;
        return tcp_client_simplex_cmd_ptr;
    } // -- END create
} // -- END namespace tcp_client_simplex::cmd 

