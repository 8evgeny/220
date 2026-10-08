#include "pcapserver.h"
#include <iostream>

#define BUF_SIZE    (8192)

bool PcapServer::opn(const SocketAddress & address)
{
    cls();
    if(!PcapSocket::opn(address)) return false;
    set_non_blocking(false);
    set_timeout(1, 0);
    _execute.store(true, std::memory_order_release);
    _thd = std::thread(&PcapServer::run, this);
    return true;
}
void PcapServer::cls()
{
    if(_execute.load(std::memory_order_acquire))
    {
        _execute.store(false, std::memory_order_release);
        _thd.detach();
        if(_thd.joinable())
        {
            _thd.join();
        }
        PcapSocket::cls();
    }
}
void PcapServer::run()
{
    const uint8_t * buf = nullptr;
    int buf_length;
    SocketAddress src_address;
    while(_execute.load(std::memory_order_acquire))
    {
        buf = rcvfrom(src_address, buf_length);
        if(buf == nullptr) continue;
        handle(src_address, buf, buf_length);
    }
}
void PcapServer::handle(const SocketAddress & address, const void * data, const int & data_length)
{
    const char * c_msg = reinterpret_cast<const char * >(data);
    std::string msg;
    for(int i = 0; i < data_length; ++i)
    {
        msg.push_back(c_msg[i]);
    }
    std::cout << "Received message from " << address.str_ip() << ":" << address.port() << " [" << data_length << "]: " << msg << std::endl;
    std::cout << "Raw data:" << std::endl;
    for(int i = 0; i < data_length; ++i)
    {
        std::cout << int(c_msg[i]) << " ";
    }
    std::cout << std::endl;
}
