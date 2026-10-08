#include "TCPServer.hpp"
#include <iostream>

#define BUF_SIZE    (8192)

TCPServer::TCPServer()
    : BaseThread(10)
{

}

TCPServer::~TCPServer()
{

}

bool TCPServer::open([[maybe_unused]]const SocketAddress & srcAddress, [[maybe_unused]]const SocketAddress& dstAddress)
{
//    close();
//    if(open(srcAddress))
//    {
//        if(TCPSocket::lstn())
//        {
//            setRcvTimeout(1, 0);
//            _execute.store(true, std::memory_order_release);
//            _thd = std::thread(&TCPServer::run, this);
//            return true;
//        }
//        else
//        {
//            TCPSocket::close();
//            return false;
//        }
//    }
    return false;
}
void TCPServer::close()
{
//    if(_execute.load(std::memory_order_acquire))
//    {
//        _execute.store(false, std::memory_order_release);
//        if(_thd.joinable())
//        {
//            _thd.join();
//        }
//        TCPSocket::close();
//    }
}

bool TCPServer::isConnected()
{
  return false;
}

uint8_t* TCPServer::get([[maybe_unused]]int32_t& length)
{
  return nullptr;
}

bool TCPServer::send([[maybe_unused]]const uint8_t* data, [[maybe_unused]]const int32_t& length)
{
  return false;
}
void TCPServer::run()
{
//    uint8_t * buf = new uint8_t[BUF_SIZE];
//    int buf_length;
//    TCPSocket client;
//    while(_execute.load(std::memory_order_acquire))
//    {
//        if(acc(client))
//        {
//            while(_execute.load(std::memory_order_acquire))
//            {
//                buf_length = client.rcv(buf, BUF_SIZE);
//                if(buf_length <= 0) break;
//                handle(client, buf, buf_length);
//            }
//            client.close();
//        }
//    }
//    delete [] buf;
}
void TCPServer::handle([[maybe_unused]]const SocketAddress& , [[maybe_unused]]const void* data, [[maybe_unused]]const int32_t& length)
{
//    const char * c_msg = reinterpret_cast<const char * >(data);
//    std::string msg;
//    for(int i = 0; i < data_length; ++i)
//    {
//        msg.push_back(c_msg[i]);
//    }
//    std::cout << "Received message from " << client.address().str_ip() << ":" << client.address().port() << " [" << data_length << "]: " << msg << std::endl;
//    std::cout << "Raw data:" << std::endl;
//    for(int i = 0; i < data_length; ++i)
//    {
//        std::cout << int(c_msg[i]) << " ";
//    }
//    std::cout << std::endl;
}
