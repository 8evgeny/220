#ifndef ETHERNET_CNTR_H
#define ETHERNET_CNTR_H

#include "ethcontroller.h"

#include "server.h"
#include "client.h"

// Host UDP port - 52515
// Device UDP port - 52516

EthController::EthController()
{

}

EthController & EthController::inst()
{
    static EthController inst;
    return inst;
}

EthController::~EthController()
{
    Socket::deinit_sockets();
}

bool EthController::start(const std::string & dev,
                          const std::string & src_mac, const std::string & src_ip, const uint16_t & src_port,
                          const std::string & dst_mac, const std::string & dst_ip, const uint16_t & dst_port)
{
#ifdef PCAP
    if(client->opn(SocketAddress(src_port, src_ip, src_mac, dev), SocketAddress(dst_port, dst_ip, dst_mac, dev)))
    {
        if(server->opn(SocketAddress(src_port, src_ip, src_mac, dev)))
        {
            return true;
        }
        client->cls();
    }
    return false;
#else
    if(client->opn(SocketAddress(/*0, src_ip*/), SocketAddress(dst_port, dst_ip)))//INADDR_BROADCAST)))
    {
        if(server->opn(SocketAddress(src_port, /*INADDR_ANY*/src_ip)))
        {
            return true;
        }
        client->cls();
    }
    return false;
#endif
}
bool EthController::stop()
{
    server->cls();
    client->cls();
    return true;
}
bool EthController::is_started()
{
    return client->is_open() || server->is_open();
}

bool EthController::send(const std::vector<uint8_t> &cmd)
{
    return client->send(cmd.data(), int(cmd.size()));
}
bool EthController::send_cmd(const uint8_t * data, const int & data_length)
{
//    static uint32_t cmd_number = 0;
//    ++cmd_number;
    return client->send_cmd(data, data_length);
}

bool EthController::send_frame(const uint8_t *f, int &w, int &h, int &id, int &num)
{
    return false;
}
uint8_t * EthController::rcv_cmd(int & len)
{
    return server->rcv_cmd(len);
}
uint8_t * EthController::rcv_frame(int & w, int & h, int & id, int & num)
{
    return server->rcv_frame(w, h, id, num);
}

int EthController::get_frame_num() const
{
    return server->get_frame_num();
}

Server *EthController::getServer()
{
    return server;
}

Client *EthController::getClient()
{
    return client;
}

#endif // ETHERNET_CNTR_H
