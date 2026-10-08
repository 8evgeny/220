#ifndef ASYNC_UDP_CLIENT_SIMPLEX_CMD_HPP
#define ASYNC_UDP_CLIENT_SIMPLEX_CMD_HPP

// #include "eth_cmd_base.hpp"
// #include "eth_cmd_base.hpp"
// #include "commands_zoo/eth_cmd_base.hpp"
#include "command_processing/commands_zoo/eth_cmd_base.hpp"
#include "notification_handler/iethnotifyhandler.hpp"

#include <string>
#include <mutex>
#include <atomic>
#include <queue>
#include <memory>

#if defined(__linux__)
    #include <netinet/in.h>
#elif defined(_WIN32) 
    #include <Winsock2.h>
#endif

#include "command_processing/cmd_manager/cmd_manager.hpp"


namespace eth_controller::cmd::async::udp::client_simplex
{
    struct AsyncUDPClientSimplexCmdSettings
    {
        std::string node_alias                  = "async_udp_client_simplex_cmd_settings";
        std::string server_ip_v4                = "127.0.0.1";
        int server_port                         = 8080;
        uint16_t single_command_buf_size_byte   = 1492; // 1492 - classic max for MTU 1500
        uint16_t cmd_tx_loop_delay_ms           = 5; // задержка между отправками команд
    }; // -- END AsUDPEthClCmdSettings

    struct MinorSettings
    {
        eth_cmd_manager::Settings cmdManager;
    };

    class AsyncUDPClientSimplexCmd
    {
        public:
            AsyncUDPClientSimplexCmd(const AsyncUDPClientSimplexCmdSettings &_settings);
            ~AsyncUDPClientSimplexCmd();
            bool set_minor_settings(const MinorSettings &_minor_settings); // вызывается до setup
            bool setup(); // первичная конфигурация и настройка соединения сокет-сокет
            bool exec(); // запуск loop регулярной отправки команд
            bool quit(); // корректное завершение работы
            void free_buffer(); // освобождение буфера
            // bool close_socket(bool force = false); // закрывает рабочий сокет, если успешно был выполнен выход из exec() (проверяется по внутренним флагам)
            bool send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd);
            bool is_connect_established(); // возвращает true, если соединение сокет-сокет успешно установлено
            bool is_success_quit(); // возвращает true, если выход произведен успешно
            bool is_error_connect(); // возвращает true, если возникла ошибка во время соединения
            void register_notify_handler(IEthNotifyHandler *handler); // регистрация подписчика на события, инициируемые настоящим классом
    
        private:
            AsyncUDPClientSimplexCmd();

            int sockfd;
            struct sockaddr_in servaddr;

            AsyncUDPClientSimplexCmdSettings settings;
            MinorSettings minor_settings;

            std::queue<std::shared_ptr<eth_cmd_processing::EthCmdBase>> cmd_snd_buf; // командный буфер на отправку
            uint8_t* cmd_bytebuffer;
            std::shared_ptr<eth_cmd_manager::CmdManager> cmdManager = nullptr; 

            struct Sync
            {
                std::atomic<bool> setup_was_call = {false};
                std::atomic<bool> success_setup = {false};
                std::atomic<bool> execute_tx = {false};
                std::atomic<bool> success_quit = {false};
                std::mutex cmd_snd_buf_mtx;
                std::atomic<bool> success_free_buffer = {false};
            } sync;
            std::vector<IEthNotifyHandler *> notify_handlers;
    }; // -- class AsyncUDPEthClientCmd
} // namespace eth_controller::cmd::async::udp

#endif //ASYNC_UDP_CLIENT_SIMPLEX_CMD_HPP