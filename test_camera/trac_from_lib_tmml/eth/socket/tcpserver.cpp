#include "tcpserver.h"
#include <iostream>

#define BUF_SIZE    (8192)

bool TCPServer::opn(const SocketAddress & address)
{
    cls();
    if(TCPSocket::opn(address))
    {
        if(TCPSocket::lstn())
        {
            set_timeout(1, 0);
            _execute.store(true, std::memory_order_release);
            _thd = std::thread(&TCPServer::run, this);
            return true;
        }
        else
        {
            TCPSocket::cls();
            return false;
        }
    }
    return false;
}
void TCPServer::cls()
{
    if(_execute.load(std::memory_order_acquire))
    {
        _execute.store(false, std::memory_order_release);
        if(_thd.joinable())
        {
            _thd.join();
        }
        TCPSocket::cls();
    }
}
void TCPServer::run()
{
    uint8_t * buf = new uint8_t[BUF_SIZE];
    int buf_length;
    TCPSocket client;
    while(_execute.load(std::memory_order_acquire))
    {
        if(acc(client))
        {
            while(_execute.load(std::memory_order_acquire))
            {
                buf_length = client.rcv(buf, BUF_SIZE);
                if(buf_length <= 0) break;
                handle(client, buf, buf_length);
            }
            client.cls();
        }
    }
    delete [] buf;
}
void TCPServer::handle(const TCPSocket & client, const void * data, const int & data_length)
{
    const char * c_msg = reinterpret_cast<const char * >(data);
    std::string msg;
    for(int i = 0; i < data_length; ++i)
    {
        msg.push_back(c_msg[i]);
    }
    std::cout << "Received message from " << client.address().str_ip() << ":" << client.address().port() << " [" << data_length << "]: " << msg << std::endl;
    std::cout << "Raw data:" << std::endl;
    for(int i = 0; i < data_length; ++i)
    {
        std::cout << int(c_msg[i]) << " ";
    }
    std::cout << std::endl;
}
