#ifndef CMD_UDP_CLIENT_HPP
#define CMD_UDP_CLIENT_HPP

#include <vector>
#include <cstdint>
#include <string>
#include <netinet/in.h>

/*
    UDP отправитель ethernet-пакетов (клиент).
    Симплексная передача данных (только отправка).
    Отправка пакетов методом snd_cmd
    НЕ предусмотрен многопоточный доступ (NOT Thread-safe)
    Формат отправляемого сообщения - std::вектор байтов (unsigned char).
*/

class CmdUDPClient
{
public:
    CmdUDPClient(const std::string &dst_ip, int dst_port, bool &ok);
    bool snd_cmd(const std::vector<uint8_t> &_cmd);
    ~CmdUDPClient();
private:
    int sockfd;
    struct sockaddr_in servaddr;
}; // -- CmdUDPClient

#endif // CMD_UDP_CLIENT_HPP