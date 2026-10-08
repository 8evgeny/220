#ifndef ASYNC_TCP_SERVER_SIMPLEX_CMD_HPP
#define ASYNC_TCP_SERVER_SIMPLEX_CMD_HPP

#include <string>
#include <atomic>

#include "../../command_processing/commands_zoo/eth_cmd_base.hpp"
#include "../../command_processing/eth_cmd_classification.hpp"
#include "../../command_processing/icmdhandler.hpp"
#include "../../notification_handler/iethnotifyhandler.hpp"

namespace eth_controller::cmd::async::server_simplex
{
    struct Settings
    {
        //net
        std::string node_alias;
        std::string server_ip_v4 = "127.0.0.1";
        int server_port = 8080;
        
        //spec
        unsigned int single_command_buf_size_byte = 1024; // размер буфера на прием одной изолированной команды в байтах
    }; // -- struct Settings

    class AsyncTcpServerSimplexCmd
    {
        public:
            AsyncTcpServerSimplexCmd(Settings _settings);
            ~AsyncTcpServerSimplexCmd();

            // Команды setup, exec, quit должны быть выполнены в одном потоке
            bool setup(); // Первичная конфигурация, установка сокет-сокет соединения
            bool exec(); // Цикл приема команд
            bool quit(); // Корректное завершение работы
            bool close_socket(bool force = false);
            void register_cmd_handler(ICmdHandler *handler);
            bool is_connect_established();
            bool is_success_quit();

            // eth notify block
            void register_notify_handler(IEthNotifyHandler *handler);
            void apply_notify_all_handlers(eth_notify_utils::EthNotifyType notify_type);
        private:
            Settings settings;
            int sockfd;
            int connfd;
            struct Sync
            {
                std::atomic<bool> success_setup = {false}; // флаг успешности первичной установки сокета
                std::atomic<bool> exec_receive = {false}; // флаг состояния приема команд
                std::atomic<bool> need_quit = {false}; // флаг, устанавливаемый при необходимости корректного выхода
                std::atomic<bool> success_quit = {false}; // флаг подтверждения корректного выхода
            } sync;

            uint8_t* cmd_bytebuffer;
            eth_cmd_processing::EthCmdDemux eth_cmd_demux;
            std::vector<ICmdHandler *> cmd_handlers;
            std::vector<IEthNotifyHandler *> notify_handlers;
    }; // -- class AsyncTcpServerSimplexCmd
} // -- namespace eth_controller::cmd::async::server_simplex

#endif //ASYNC_TCP_SERVER_SIMPLEX_CMD_HPP