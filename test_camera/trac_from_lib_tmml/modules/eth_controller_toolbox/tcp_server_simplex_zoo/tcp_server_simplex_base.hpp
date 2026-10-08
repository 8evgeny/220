#ifndef TCP_SERVER_SIMPLEX_BASE_HPP
#define TCP_SERVER_SIMPLEX_BASE_HPP

#include <string>
#include <opencv2/core/types.hpp>
#include <memory>
#include "../command_processing/commands_zoo/eth_cmd_base.hpp"
#include <atomic>

namespace tcp_server_simplex::base
{
    struct NetworkSettings
    {
        std::string network_node_alias; // Наименование узла в сети при связи, берется по названию EthController'а (Например, EthControllerCmd_1 или EthControllerCmd_first)
        int server_port = 8080;
        std::string server_ip_v4 = "127.0.0.1";
    }; // -- END NetworkSettings

    /*
    Информация о перенаправлении получаемых через Ethernet cmd или frame //TODO: реализация через паттерн "стратегия"
    */
    struct DistributeSettings
    {
        // Уведомление подписчиков 
        struct NotifyHandlers
        {
            // Мгновенное уведомление при поступлении контента по Eth
            struct Instant
            {
                bool enable = false;
            } instant;

            // Регулярное уведомление при лимитировании fps, используется буфер
            struct Regular
            {
                bool enable = false;
                int fps = 0; 
                int buffer_size = 1;
            } regular;
        } notify_handlers;

        // исключительное сохранение во внутренний буфер с возможностью выдачи по запросу buf_pop_frame()
        struct Save2Buffer
        {
            bool enable = false;
            int buffer_size = 1;
        } save2buffer;
    }; // -- END DistributeReceiveContentMode

    struct SettingsBase
    {
        NetworkSettings net_settings;
        DistributeSettings distrib_settings;
    }; // -- SettingsBase

    class TCPServerSimplexBase
    {
        public:
            TCPServerSimplexBase(SettingsBase settings_base);
            virtual ~TCPServerSimplexBase();
        /*
               bool launch() -- Сервер открывает сокет и входит в режим слушателя
            Запускается ожидание сообщения от клиента.
            Блокирует текущий поток, ожидает, когда с ним свяжется "клиент". 
            В момент связи клиента сервер получает IP и порт клиента, 
            но в настоящей модели взаимодействия используется 
            два симплексных вместо одного дуплексного 
            по соображениям симметричности модуля
        */
            bool launch(); // Первичный запуск и установка сокет-сокет соединения

            virtual bool quit(); // Корректное завершение работы 
            virtual bool exec(); // цикл приема кадров / команд / и т.д.

            struct Sync
            {
                std::atomic<bool> execute_receive = {false};
                std::atomic<bool> accept_success_done = {false};
                std::atomic<bool> need_quit = {false};
                std::atomic<bool> exec_quit_success = {false};
            } sync;

        protected:
            TCPServerSimplexBase();
            std::string node_alias; // псевдоним хоста примитива tcp взаимодействия точка-точка
            SettingsBase settings_base;
            int sockfd;
            int connfd; // дескриптор сокета приема данных
    }; // -- END class TCPServerSimplexBase

    bool getIniSettings(
        const std::string &path2ini, 
        const std::string &ini_section_name, 
        SettingsBase &settings_base);

    bool checkValidIniSettings(
        const SettingsBase &settings_base);
} // -- END namespace tcp_server_simplex::base

#endif //TCP_SERVER_SIMPLEX_BASE_HPP