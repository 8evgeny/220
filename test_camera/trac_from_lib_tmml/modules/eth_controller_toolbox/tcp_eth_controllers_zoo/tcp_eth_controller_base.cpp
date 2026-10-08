#include "tcp_eth_controller_base.hpp"
#include "INIReader.h"
#include <thread>
#include "eth_cnt_common_utils.hpp"

namespace eth_controller::base
{
    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        EthCntSettingsBase &settings)
    {
        INIReader reader(path2ini);
        std::cout << "BEGIN EthController::base read ini settings" << std::endl;
        if(reader.ParseError() < 0)
        {
            std::cout << "Error: INIReader ParseError ini file path=\"" + path2ini + "\"" << std::endl;
            return false;
        }

        settings.node_alias = reader.Get(ini_section_name, "alias", "oops");
        if(settings.node_alias == "oops")
        {
            std::cout << "Error: INIReader parse [" + ini_section_name+ "] : alias error" << std::endl;
            return false;
        }
        else
        {
            std::cout << "\talias = " << settings.node_alias << std::endl;
        }

        std::string communication_mode_tmp = reader.Get(ini_section_name, "communication_mode", "oops");
        if(communication_mode_tmp == "oops")
        {
            std::cout << "Error: INIReader parse [" + ini_section_name+ "] : communication_mode error" << std::endl;
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
            std::cout << "\tcommunication_mode = " << communication_mode_tmp << std::endl;
        }
        std::cout << "END EthController::base read ini settings" << std::endl;
        return true;
    } // -- END getIniSettings

    bool checkValidIniSettings(const EthCntSettingsBase &settings_base)
    {
        return true;
    } // -- END checkValidIniSettings

    bool EthControllerBase::client_try_flow_connect_to_server()
    {
        while(true)
        {
            if(client_simplex_base->try_connect_to_server())
            {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(connect_to_server_request_delay_ms));
        }
    } // -- END EthControllerBase::client_try_flow_connect_to_server()

    EthControllerBase::EthControllerBase(const EthCntSettingsBase &settings_base_) : settings_base(settings_base_)
    {

    } // -- END EthControllerBase(NetworkSettings settings)

    EthControllerBase::EthControllerBase()
    {

    } // -- END EthControllerBase

    bool EthControllerBase::launch()
    {
        std::cout << "BEGIN " << settings_base.node_alias << " EthControllerBase::launch()" << std::endl;
        /*
            Настройка одностороннего клиента разделена на два этапа:
            первичная (setup) и вторичная (client_try_flow_connect_to_server).
            В первичной настройке конфигурируется сокет.
            Во вторичной клиент запрашивает соединение с сервером в цикле с временным периодом.
        */
       
        if(!client_simplex_base->setup())
        {
            std::cout << "\t TCPClientSimplex::setup() -- Failed setup client socket" << std::endl;
        }
        else
        {
            std::cout << "\t TCPClientSimplex::setup() -- client socket open success" << std::endl;
        }

        // Настройка сервера в другом потоке, чтобы два процесса не блокировались взаимно
        std::thread server_setup_thread(
            &tcp_server_simplex::base::TCPServerSimplexBase::launch, this->server_simplex_base);
        server_setup_thread.detach();


        // цикл попыток обратиться к удаленному серверу
        client_try_flow_connect_to_server();
        server_simplex_base->sync.execute_receive.store(true); // установка флага, разрешающего процесс приема и отображения фреймов

        // Отчет об успешном установлении двустороннего взаимодействия только если на удаленный хост успешно подключился к настоящему серверу
        while(!server_simplex_base->sync.accept_success_done.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        std::cout << "duplex EthControllerBase success connection established" << std::endl; // отчет об установлении успешного двустороннего соединения
        connectionEstablished = true;
        std::cout << "END " << settings_base.node_alias << " EthControllerBase::launch()" << std::endl;
        return true;
    } // -- END EthControllerBase::launch()

    bool EthControllerBase::quit()
    {
        std::cout << " --- BEGIN EthController::quit()" << std::endl;
        
        std::cout << "\t BEGIN client_simplex_base->quit()" << std::endl;
        bool client_report = client_simplex_base->quit();
        std::cout << "\t END client_simplex_base->quit()" << std::endl;

        std::cout << "\t BEGIN server_simplex_base->quit()" << std::endl;
        bool server_report = server_simplex_base->quit();
        std::cout << "\t END server_simplex_base->quit()" << std::endl;



        std::cout << " --- END EthController::quit()" << std::endl;
        return server_report && client_report;
    } // -- END quit

    EthControllerBase::~EthControllerBase()
    {
        std::cout << "--- Деструктор EthControllerBase" << std::endl;
    } // -- END ~EthControllerBase

    std::string EthControllerBase::getCurrentNodeAlias() const
    {
        return settings_base.node_alias;
    } // -- END getCurrentNodeAlias
}
