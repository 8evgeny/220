#ifndef CMD_UDP_SERVER
#define CMD_UDP_SERVER

#include <string>
#include <vector>
#include <cstdint>
#include <queue>
#include <atomic>
#include <chrono>
#include <mutex>

#include <netinet/in.h>


/*
    UDP приемник ethernet-пакетов (сервер).
    Симплексная передача данных (только прием).
    Способ поучения внешним (клиентским) кодом eth-сообщений - выгрузка по запросу.
    НЕ используется паттерн подписка (observer).
    Предусмотрен многопоточный доступ (Thread-safe)
    Формат принимаемого сообщения - std::вектор байтов (unsigned char).
*/

struct UDPSrvSettings
{
    std::string ip; // ip, на который привязывается сервер
    int port; // порт, который прослушивается сервером 
    int buf_sz_bytes = 1400; // байтовый размер приемного буфера
};

class CmdUDPServer
{
public:
    CmdUDPServer(const UDPSrvSettings &_settings, bool &ok); // если ok = false, сервер создан некорректно
    int get_cmd_count(); // получить количество команд в буфере ожидания отгрузки
    bool pull_one(std::vector<uint8_t> &_cmd); // отгрузка первой в очереди команды
    void quit(); // корректное завершение работы Server'а
    ~CmdUDPServer();
private:
    void exec();
    UDPSrvSettings settings;

    int sockfd;
    struct sockaddr_in servaddr;

    uint8_t* cmd_bytebuffer;
    std::queue<std::vector<uint8_t>> cmd_queue;

    struct Sync
    {
        std::atomic<bool> success_setup = {false};
        std::atomic<bool> failed_setup = {false};
        std::atomic<bool> execute = {false};
        std::atomic<bool> exec_complete = {false};
        std::atomic<bool> quit_was_call = {false};
        std::atomic<bool> need_quit = {false};
        std::atomic<bool> success_quit = {false};
        std::mutex cmd_queue_mtx;
    } sync;

}; // -- CmdUDPServer

#endif // CMD_UDP_SERVER

