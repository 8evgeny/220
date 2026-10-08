#ifndef ETHCONTROLLER_H
#define ETHCONTROLLER_H

#include <stdint.h>
#include <string>
#include <iostream>
#include <typeinfo>
#include <vector>

class Server;
class Client;

class EthController
{
public:
    EthController();

    static EthController & inst();
    ~EthController();

    bool start(const std::string & dev,
               const std::string & src_mac, const std::string & src_ip, const uint16_t & src_port,
               const std::string & dst_mac, const std::string & dst_ip, const uint16_t & dst_port);
    bool stop();

    bool is_started();

    bool send(const std::vector<uint8_t> & cmd);
    bool send_cmd(const uint8_t * data, const int & data_length);
    virtual bool send_frame(const uint8_t *f, int & w, int & h, int & id, int & num);

    uint8_t * rcv_cmd(int & data_length);
    uint8_t * rcv_frame(int & w, int & h, int & id, int & num);
    int get_frame_num() const;

    Server *getServer();
    Client *getClient();

protected:
    Server * server = nullptr;
    Client * client = nullptr;
};

#endif // ETHCONTROLLER_H
