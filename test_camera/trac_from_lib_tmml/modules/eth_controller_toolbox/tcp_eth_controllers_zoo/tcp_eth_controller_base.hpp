#ifndef ETH_CONTROLLER_BASE_HPP
#define ETH_CONTROLLER_BASE_HPP

#include "../eth_cntrl_common_types.hpp"

#include "../tcp_client_simplex_zoo/tcp_client_simplex_base.hpp"
#include "../tcp_server_simplex_zoo/tcp_server_simplex_base.hpp"
#include "../command_processing/commands_zoo/eth_cmd_base.hpp"
// #include <vector>
// #include <memory>
// #include <atomic>

// #include "../command_processing/commands_zoo/eth_cmd_base.hpp"
// #include "../command_processing/commands_zoo/copter_alarm/copter_alarm_eth_cmd.hpp"

namespace eth_controller::base
{
    struct EthCntSettingsBase
    {
        std::string node_alias; //Client, Server, ClientFrame, ServerFrame, ServerCmd1, ClientCmd3 etc..
        std::string ini_section_name; // Наименование блока (секции) в ini-файле
        eth_common::EthControllerType content_type; // FRAME, CMD ? 
        eth_common::CommunicationModes communication_mode; // SIMPLEX/DUPLEX?
    }; // END -- EthCntSettingsBase

    class EthControllerBase
    {
    public:
        EthControllerBase(const EthCntSettingsBase &settings_base);
        virtual ~EthControllerBase();
        virtual bool launch(); // // открывает сокет и включает ожидание присоединения dst хоста (блокирует поток, из которого была вызвана до момента соединения с dst узлом)
        virtual bool quit(); // корректно завершает работу EthController
        std::string getCurrentNodeAlias() const;
    protected:
        EthControllerBase();
        EthCntSettingsBase settings_base;

        std::shared_ptr<tcp_server_simplex::base::TCPServerSimplexBase> server_simplex_base;
        std::shared_ptr<tcp_client_simplex::base::TCPClientSimplexBase> client_simplex_base;

        bool connectionEstablished = false; // флаг состояния успешности установленного соединения точка-точка
        const int connect_to_server_request_delay_ms = 50; // интервал запросов на сервер в процессе установки соединения
        bool client_try_flow_connect_to_server(); // блокирует текущий поток, в цикле с определенным временным периодом осуществляет попытку подключения к серверу
    }; // -- class EthController

    bool getIniSettings(
        const std::string &path2ini,
        const std::string &ini_section_name,
        EthCntSettingsBase &settings);
    bool checkValidIniSettings(const EthCntSettingsBase &settings_base);
} // -- namespace eth_controller::base

#endif //ETH_CONTROLLER_BASE_HPP
