#ifndef ASYNC_UDP_ETH_CONTROLLER_HPP
#define ASYNC_UDP_ETH_CONTROLLER_HPP

#include "async_udp_client_simplex_cmd/async_udp_client_simplex_cmd.hpp"
#include "async_udp_server_simplex_cmd/async_udp_server_simplex_cmd.hpp"

#include "command_processing/commands_zoo/eth_cmd_base.hpp"
#include "command_processing/icmdhandler.hpp"
#include "notification_handler/iethnotifyhandler.hpp"
#include "eth_cntrl_common_types.hpp"
#include "command_processing/cmd_manager/cmd_manager.hpp"
#include "command_processing/cmd_tail_tools/checksum/checksum_common.hpp"

#include <mutex>
#include <vector>
#include <atomic>
#include <memory>

namespace eth_controller::cmd::async::udp
{
    struct AsyncUDPEthControllerCmdSettings
    {
        std::string src_ip_v4;
        int src_port;
        std::string dst_ip_v4;
        int dst_port;

        int single_command_buf_size_byte = 1492;
        int cmd_tx_loop_delay_ms = 5;

        std::string ini_section_name;
        eth_common::EthControllerType content_type = eth_common::EthControllerType::CMD; // FRAME, CMD ? 
        eth_common::CommunicationModes communication_mode; // SIMPLEX/DUPLEX?

        std::string path2ini;
        std::string node_alias;
    }; // -- AsyncUDPEthCntSettingsCmd

    struct MinorSettings
    {
        eth_cmd_manager::Settings cmd_manager_client;
        eth_cmd_manager::Settings cmd_manager_server;
    }; // -- END MinorSettings

    class AsyncUDPEthControllerCmd
    {
        public:
            AsyncUDPEthControllerCmd(
                const AsyncUDPEthControllerCmdSettings &_settings,
                std::shared_ptr<eth_controller::cmd::async::udp::client_simplex::AsyncUDPClientSimplexCmd> client_smp_ptr,
                std::shared_ptr<eth_controller::cmd::async::udp::server_simplex::AsyncUDPServerSimplexCmd> server_smp_ptr);
            ~AsyncUDPEthControllerCmd();
            bool set_minor_settings(const MinorSettings &_minor_settings); // Устанавливается ДО launch
            MinorSettings get_minor_settings();
            bool launch();
            bool quit();
            bool send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd);
            void register_cmd_handler(ICmdHandler *handler);
            void register_notify_handler(IEthNotifyHandler *handler);
        private:
            void exec_client_simplex();
            void exec_server_simplex();
            bool update_minor_settings();

            bool support_tx  = false;
            bool support_rx  = false;

            AsyncUDPEthControllerCmdSettings settings;
            MinorSettings minor_settings;
            
            std::shared_ptr<eth_controller::cmd::async::udp::client_simplex::AsyncUDPClientSimplexCmd> _client_simplex;
            std::shared_ptr<eth_controller::cmd::async::udp::server_simplex::AsyncUDPServerSimplexCmd> _server_simplex;

            struct Sync
            {
                std::atomic<bool> launch_was_call   = {false};
                std::atomic<bool> _execute_rx_tx    = {false};
                std::atomic<bool> quit_was_called   = {false};
            } sync;

            const int wait_established_duplex_connection_delay_ms = 100; // интервал между опросом состояния соединения сервера и клиента на хоста с удаленными клиентом и сервером соответственно
            std::vector<IEthNotifyHandler *> notify_handlers;
    }; // -- END class AsyncTcpEthControllerCmd

} // -- namespace eth_controller::cmd::async::udp::server_simplex

#endif //ASYNC_UDP_ETH_CONTROLLER_HPP