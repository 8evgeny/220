#include "tcp_eth_controller_cmd.hpp"
#include "INIReader.h"
#include <thread>

namespace eth_controller::cmd
{
    EthControllerCmd::EthControllerCmd(
            const EthCntSettingsCmd &_settings_cmd,
            const eth_controller::base::EthCntSettingsBase _settings_base,
            std::shared_ptr<tcp_client_simplex::cmd::TCPClientSimplexCmd> _client_simplex,
            std::shared_ptr<tcp_server_simplex::cmd::TCPServerSimplexCmd> _server_simplex)
    {
        this->settings_base = _settings_base;
        this->settings_cmd = _settings_cmd;

        this->client_simplex = _client_simplex;
        this->server_simplex = _server_simplex;

        this->client_simplex_base = std::static_pointer_cast<tcp_client_simplex::base::TCPClientSimplexBase>(client_simplex);
        this->server_simplex_base = std::static_pointer_cast<tcp_server_simplex::base::TCPServerSimplexBase>(server_simplex);
    } // -- END EthControllerCmd

    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        EthCntSettingsCmd &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN " << ini_section_name <<  " EthController::cmd read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            std::cout << "INIReader ParseError ini file path=\"" + path2ini + "\"" << std::endl;
            return false;
        }

        settings.max_transmit_cps = reader.GetInteger(ini_section_name, "max_transmit_cps", -1);
        if(settings.max_transmit_cps == -1)
        {
            std::cout << "INIReader parse [" << ini_section_name << "] : max_transmit_cps error \n";
        }
        else
        {
            std::cout << "\t max_transmit_cps = " << settings.max_transmit_cps << std::endl;
        }

        std::cout << "END " << ini_section_name <<  " EthController::cmd read ini settings" << std::endl;
        return true;
    } // -- END getIniSettings

    bool checkValidIniSettings(const EthCntSettingsCmd &settings)
    {
        if(settings.max_transmit_cps < 0) 
        {
            std::cout << "settings.max_transmit_cps out of valid range" << std::endl;
            return false;
        }
        return true;
    } // -- END checkValidIniSettings

    std::shared_ptr<EthControllerCmd> create(
        const std::string &path2ini, const std::string &sectionName)
    {
        eth_controller::base::EthCntSettingsBase base_settings;
        bool report_read_ini_base = 
            eth_controller::base::getIniSettings(path2ini, sectionName, base_settings);
        bool report_check_ini_base = eth_controller::base::checkValidIniSettings(base_settings);

        eth_controller::cmd::EthCntSettingsCmd cmd_settings;
        bool report_read_ini_cmd = 
            eth_controller::cmd::getIniSettings(path2ini, sectionName, cmd_settings);
        bool report_check_ini_cmd = eth_controller::cmd::checkValidIniSettings(cmd_settings);

        std::shared_ptr<tcp_client_simplex::cmd::TCPClientSimplexCmd> client_simplex_tmp =
            tcp_client_simplex::cmd::create(path2ini, sectionName);
            
        std::shared_ptr<tcp_server_simplex::cmd::TCPServerSimplexCmd> server_simplex_tmp =
            tcp_server_simplex::cmd::create(path2ini, sectionName);

        bool success_create_simplex_client = (client_simplex_tmp != nullptr);
        bool success_create_simplex_server = (server_simplex_tmp != nullptr);

        if(!(
            report_read_ini_base && 
            report_check_ini_base &&
            report_read_ini_cmd &&
            report_check_ini_cmd &&
            success_create_simplex_client &&
            success_create_simplex_server))
        {
            std::cout << "Error create EthControllerCmd" << std::endl;
            return nullptr;
        }
        base_settings.content_type = eth_common::EthControllerType::CMD;

        std::shared_ptr<EthControllerCmd> eth_controller = 
            std::make_shared<EthControllerCmd>(
                cmd_settings, 
                base_settings, 
                client_simplex_tmp,
                server_simplex_tmp);        
        return eth_controller;
    } // -- END create

    bool EthControllerCmd::launch()
    {
        return eth_controller::base::EthControllerBase::launch();
    } // -- END EthControllerBase::launch()

    bool EthControllerCmd::quit()
    {
        return eth_controller::base::EthControllerBase::quit();
    } // -- END quit

    EthControllerCmd::~EthControllerCmd()
    {
        std::cout << "--- Деструктор EthControllerBase" << std::endl;
    } // -- END ~EthControllerBase

    bool EthControllerCmd::send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> command) const
    {
        return this->client_simplex->send_cmd(command);
    } // -- END send_cmd

    void EthControllerCmd::register_cmd_handler(ICmdHandler *handler)
    {
        this->server_simplex->register_cmd_handler(handler);
    } // -- END register_cmd_handler
    
    void EthControllerCmd::register_notify_handler(IEthNotifyHandler *handler)
    {
        this->notify_handlers.push_back(handler);
        /*
        Необходимо подписать обработчика также на вложенные объекты 
        данного класса (композиция), т.к. класс Controller напрямую не открывает 
        потоки для eth-взаимодействия. Основная взаимодействие реализуется в 
        client_simplex и server_simplex
        */
        this->server_simplex->register_notify_handler(handler);
        this->client_simplex->register_notify_handler(handler);
    } // -- END register_notify_handler

    // bool EthControllerCmd::wait_receive_cmd(eth_controller::cmd::EthCmdCopterAlarm &receive_cmd) const
    // {
    //     return server_simplex->wait_receive_cmd(receive_cmd);
    // } // -- END bool wait_receive_cmd(eth_controller::cmd::EthCmdCopterAlarm &receive_cmd)

    // bool EthControllerCmd::exec_cmd_receive()
    // {
    //     return true;
    // } // -- END void exec_cmd_receive()
} // -- END namespace eth_controller::cmd
