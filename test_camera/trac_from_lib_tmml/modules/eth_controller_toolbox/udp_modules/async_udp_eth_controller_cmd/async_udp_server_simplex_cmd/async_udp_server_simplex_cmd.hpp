#ifndef ASYNC_UDP_SERVER_SIMPLEX_CMD_HPP
#define ASYNC_UDP_SERVER_SIMPLEX_CMD_HPP

#include <string>
#include <atomic>
#include <vector>

#if defined(__linux__)
    #include <netinet/in.h>
#elif defined(_WIN32) 
    #include <Winsock2.h>
#endif

#include "command_processing/commands_zoo/eth_cmd_base.hpp"
#include "command_processing/icmdhandler.hpp"
#include "command_processing/cmd_manager/cmd_manager.hpp"
#include "command_processing/cmd_tail_tools/checksum/checksum_common.hpp"
#include "notification_handler/iethnotifyhandler.hpp"

namespace eth_controller::cmd::async::udp::server_simplex
{
    struct AsyncUDPClientSimplexCmdSettings
    {
        std::string node_alias                  = "async_udp_server_simplex_cmd";
        std::string server_ip_v4                = "127.0.0.1";
        int server_port                         = 8080;
        uint16_t single_command_buf_size_byte   = 1492; // 1492 - classic max for MTU 1500
    }; // -- END AsUDPEthClCmdSettings

    struct MinorSettings
    {
        eth_cmd_manager::Settings cmdManager;
    };

    class AsyncUDPServerSimplexCmd
    {
        public:
            AsyncUDPServerSimplexCmd(const AsyncUDPClientSimplexCmdSettings &_settings);
            ~AsyncUDPServerSimplexCmd();
            bool set_minor_settings(const MinorSettings &_minor_settings); // установка до setup
            bool setup();
            bool exec();
            bool quit();
            void free_buffer();
            // bool close_socket(bool force = false);
            void register_cmd_handler(ICmdHandler *handler);
            bool is_connect_established();
            bool is_success_quit();
            void register_notify_handler(IEthNotifyHandler *handler);
            void apply_notify_all_handlers(eth_notify_utils::EthNotifyType notify_type);
        
        private:
            AsyncUDPServerSimplexCmd();
            
            AsyncUDPClientSimplexCmdSettings settings;
            MinorSettings minor_settings;

            int sockfd;
            struct sockaddr_in servaddr; // servaddr в соответствии с конфигурацией
            struct Sync
            {
                std::atomic<bool> setup_was_call        = {false}; // индикатор того, что метод setup вызывался
                std::atomic<bool> success_setup         = {false}; // флаг успешности первичной установки сокета
                std::atomic<bool> exec_receive          = {false}; // флаг состояния приема команд
                std::atomic<bool> need_quit             = {false}; // флаг, устанавливаемый при необходимости корректного выхода
                std::atomic<bool> success_quit          = {false}; // флаг подтверждения корректного выхода
                std::atomic<bool> success_free_buffer   = {false}; // высвобождена память буфера
            } sync; 

            uint8_t* cmd_bytebuffer;
            std::shared_ptr<eth_cmd_manager::CmdManager> cmdManager = nullptr; 

            std::vector<ICmdHandler *> cmd_handlers;
            std::vector<IEthNotifyHandler *> notify_handlers;
    }; // -- class AsyncUDPServerSimplexCmd
} // namespace eth_controller::cmd::async::udp

#endif //ASYNC_UDP_SERVER_SIMPLEX_CMD_HPP