#ifndef ASYNC_UDP_CLIENT_HPP
#define ASYNC_UDP_CLIENT_HPP

#include <string>
#include <queue>
#include <atomic>
#include <mutex>

#include "command_processing/cmd_manager/cmd_manager.hpp"
#include "command_processing/commands_zoo/eth_cmd_base.hpp"

#include <netinet/in.h>


struct AsyncUDPCliCfg
{
    std::string dst_ip                      = "127.0.0.1";
    int dst_port                            = 8080;
    int cmd_tx_loop_delay_mcs               = 500;
    uint16_t single_command_buf_size_byte   = 1492;     // 1492 - classic max for MTU 1500
    int setup_watchdog_time_ms              = 1000;     // максимально разрешенное время на подготовку объекта класса к эксплуатации.
                                                        // Иначе говоря - максимальное время блокировки потока конструктором класса
    int setup_check_delay_mcs               = 100;      // интервал проверки статуса установки клиента в конструкторе
    int quit_watchdog_time_ms               = 2000;     // максимально разрешенное время на корректное завершение работы клиента в методе quit(). 
                                                        // Иначе говоря - максимальное время блокировки потока методом quit();
    int quit_check_delay_ms                 = 5;        // интервал проверки закрытия внутреннего потока клиента
    eth_cmd_manager::Settings cmdManager; // cmd manager settings
}; // -- struct AsyncUDPCliCfg

class AsyncUDPClient
{
public:
    AsyncUDPClient(const AsyncUDPCliCfg &_cfg, bool &ok);
    bool send_cmd(std::shared_ptr<eth_cmd_processing::EthCmdBase> cmd);
    bool quit();
    ~AsyncUDPClient();
private:
    void exec();
    AsyncUDPCliCfg cfg;
    std::queue<std::shared_ptr<eth_cmd_processing::EthCmdBase>> cmd_snd_buf;
    std::shared_ptr<eth_cmd_manager::CmdManager> cmdManager = nullptr; 
    struct Sync
    {
        std::atomic<bool> success_setup         = {false};  // подтверждение корректной настройки и готовности к работе
        std::atomic<bool> failed_setup          = {false};  // настройка прошла с ошибкой
        std::atomic<bool> execute               = {false};  // индикатор исполнения потока отправки 
        std::atomic<bool> quit_was_call         = {false};  // защитный механизм хранения состояния факта первого вызова метода quit
        std::atomic<bool> exec_complete         = {false};  // устанавливается из хвоста метода exec, подтверждает завершение исполнения потока отправки
        std::atomic<bool> success_quit          = {false};  // флаг успешности завершения работы класса
        std::mutex cmd_snd_buf_mtx;                         // потоковая безопасность очереди команд
    } sync;
}; // - class AsyncUDPClient

#endif /* ASYNC_UDP_CLIENT_HPP */