#include "udpserver.h"
#include <iostream>

#define BUF_SIZE    (8192)

bool UDPServer::opn(const SocketAddress & address)
{
    cls();
    if(!UDPSocket::opn(address)) return false;
    set_non_blocking(true);
    set_timeout(1, 0);
    set_buffer_size(1920 * 1080 * 2);
    _execute.store(true, std::memory_order_release);
    _thd = std::thread(&UDPServer::run, this);
    return true;
}
void UDPServer::cls()
{
    if(_execute.load(std::memory_order_acquire))
    {
        _execute.store(false, std::memory_order_release);
        if(_thd.joinable())
        {
            _thd.join();
        }
        UDPSocket::cls();
    }
}
void UDPServer::run()
{
    uint8_t * buf = new uint8_t[BUF_SIZE];
    SocketAddress src_address;
    while(_execute.load(std::memory_order_acquire))
    {
        int buf_length = rcvfrom(src_address, buf, BUF_SIZE);
//                std::cout << "buf_length    " << buf_length << std::endl;
        if(buf_length <= 0) continue;
        handle(src_address, buf, buf_length);
    }
    delete [] buf;
}
void UDPServer::handle(const SocketAddress & address, const void * data, const int & data_length)
{
    const char * c_msg = reinterpret_cast<const char * >(data);
    std::string msg;
    for(int i = 0; i < data_length; ++i)
    {
        msg.push_back(c_msg[i]);
    }
    for(int i = 0; i < data_length; ++i)
    {
        std::cout << int(c_msg[i]) << " ";
    }
    std::cout << std::endl;
}
