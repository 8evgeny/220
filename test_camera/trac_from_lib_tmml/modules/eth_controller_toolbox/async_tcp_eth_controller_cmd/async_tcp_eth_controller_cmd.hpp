#ifndef ASYNC_TCP_ETH_CONTROLLER_HPP
#define ASYNC_TCP_ETH_CONTROLLER_HPP

#include <string>

#include "async_tcp_client_simplex_cmd/async_tcp_client_simplex_cmd.hpp"
#include "async_tcp_server_simplex_cmd/async_tcp_server_simplex_cmd.hpp"

#include "../command_processing/commands_zoo/eth_cmd_base.hpp"
#include "../command_processing/icmdhandler.hpp"
#include "../notification_handler/iethnotifyhandler.hpp"
#include "../eth_cntrl_common_types.hpp"
#include <mutex>
#include <vector>
#include <atomic>
#include <memory>


/*
    Отличие (A) AsyncTcpEthControllerCmd от (B) TcpEthControllerCmd:
    1) В объекте класса (B) запрос на отправку команды осуществляется из основного потока приложение 
    => блокировка до момента отправки сообщения, прием осуществляется в отдельном потоке, 
    обработчики (хендлеры) оповещаются в соответствии с механизмом подписки (observer) + маршрутизация; 
    
    В объекте класса (A) два потока, отправка команд осуществляется через промежуточный потокобезопасный буфер,
    прием осуществляется в отдельном потоке через механизм подписки (observer).
*/

namespace eth_controller::cmd::async
{
    struct AsEthCntSettingsCmd
    {
        //net 
        std::string src_ip_v4;
        int src_port;
        std::string dst_ip_v4;
        int dst_port;

        //buf_size
        int single_command_buf_size_byte = 1024;

        //client_simplex
        int cmd_tx_loop_delay_ms = 50; // временнОй период TODO: сделать опцию адаптивной подстройки времени отправки, которая регулирует заполненность буфера

        //base
        std::string path2ini;
        std::string node_alias; //Client, Server, ClientFrame, ServerFrame, ServerCmd1, ClientCmd3 etc..
        std::string ini_section_name; // Наименование блока (секции) в ini-файле
        eth_common::EthControllerType content_type = eth_common::EthControllerType::CMD; // FRAME, CMD ? 
        eth_common::CommunicationModes communication_mode; // SIMPLEX/DUPLEX?
    }; // settings

    class AsyncTcpEthControllerCmd
    {
        public:
            AsyncTcpEthControllerCmd(
                const AsEthCntSettingsCmd &_settings,
                std::shared_ptr<eth_controller::cmd::async::client_simplex::AsyncTcpClientSimplexCmd> client_smp_ptr,
                std::shared_ptr<eth_controller::cmd::async::server_simplex::AsyncTcpServerSimplexCmd> server_smp_ptr);
            ~AsyncTcpEthControllerCmd();
            bool launch();
            bool quit();
            bool send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd);
            void register_cmd_handler(ICmdHandler *handler);
            void register_notify_handler(IEthNotifyHandler *handler);
        private:
            void exec_client_simplex();
            void exec_server_simplex();

            AsEthCntSettingsCmd settings;

            std::shared_ptr<eth_controller::cmd::async::client_simplex::AsyncTcpClientSimplexCmd> _client_simplex;
            std::shared_ptr<eth_controller::cmd::async::server_simplex::AsyncTcpServerSimplexCmd> _server_simplex;

            struct Sync
            {
                std::atomic<bool> _execute_rx_tx = {false};
            } sync;

            const int wait_established_duplex_connection_delay_ms = 100; // интервал между опросом состояния соединения сервера и клиента на хоста с удаленными клиентом и сервером соответственно
            std::vector<IEthNotifyHandler *> notify_handlers;
    }; // -- END class AsyncTcpEthControllerCmd
} // eth_controller::cmd::async
#endif // ASYNC_TCP_ETH_CONTROLLER_HPP