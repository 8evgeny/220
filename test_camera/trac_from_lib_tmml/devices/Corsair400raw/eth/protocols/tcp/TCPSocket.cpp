#include "TCPSocket.hpp"
#include "eth/base/SocketAddress.hpp"

#include <iostream>
#include <cstring>

TCPSocket::TCPSocket()
    : Socket(SOCK_STREAM, IPPROTO_TCP)
{

}

bool TCPSocket::cnct([[maybe_unused]]const SocketAddress & other)
{
//    if(!isOpen()) return false;
//    // Create Address
//    sockaddr_in other_address;
//    memset(&other_address, 0, sizeof(other_address));
//    other_address.sin_family = AF_INET;
//    other_address.sin_addr.s_addr = other.nip();
//    other_address.sin_port = other.nport();
//    // Open Connection
//    if(connect(_sock, reinterpret_cast<sockaddr *>(&other_address), sizeof(other_address)) < 0)
//    {
//#ifdef DEBUG
//       std::cout << "connection error" << std::endl;
//#endif
        return false;
//    }
//    return true;
}
bool TCPSocket::snd([[maybe_unused]]const void * data, [[maybe_unused]]const int & size)
{
//    if(!isOpen()) return false;
//    // Send data
//    int sent_bytes = send(_sock, reinterpret_cast<const char*>(data), size, 0);
//    if(sent_bytes != size)
//    {
//#ifdef DEBUG
//       std::cout << "sending error" << std::endl;
//#endif
        return false;
//    }
//    return true;
}
int TCPSocket::rcv([[maybe_unused]]void * data, [[maybe_unused]]const int & max_size)
{
//    if(!isOpen()) return false;
//    // Receive data
//    int received_bytes = recv(_sock, reinterpret_cast<char *>(data), max_size, 0);
//    return received_bytes;
  return 0;
}
bool TCPSocket::lstn()
{
//    if(!isOpen()) return false;
//    int res = listen(_sock, SOMAXCONN);
//    if(res < 0)
//    {
//#ifdef DEBUG
//       std::cout << "listen failed" << std::endl;
//#endif
        return false;
//    }
//    return true;
}
bool TCPSocket::acc([[maybe_unused]]TCPSocket & client)
{
//    if(!isOpen()) return false;
//    // Accept a client socket
//    SOCKET client_socket = accept(_sock, nullptr, nullptr);
//    if(client_socket == INVALID_SOCKET)
//    {
        return false;
//    }
//    client.setSock(client_socket);
//    return true;
}
//bool TCPSocket::open()
//{
//    close();

//    // Create Socket
//    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if(_sock == INVALID_SOCKET)
//    {
//#ifdef DEBUG
//        std::cout << "failed to create socket" << std::endl;
//#endif
//        return false;
//    }
//    // Set Socket Port Reusable
//    if(!setReusable(true))
//    {
//#ifdef DEBUG
//        std::cout << "failed to set socket port reusable" << std::endl;
//#endif
//    }
//    // Create Address
//    sockaddr_in address;
//    memset(&address, 0, sizeof(address));
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = _socketAddress.nip();
//    address.sin_port = _socketAddress.nport();
//    // Bind Socket to Address
//    if(bind(_sock, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
//    {
//#ifdef DEBUG
//        std::cout << "failed to bind socket" << std::endl;
//#endif
//        close();
//        return false;
//    }
//#ifdef DEBUG
//   std::cout << "tcp socket opened on " << _socketAddress.str_ip() <<":" << _socketAddress.port() << std::endl;
//#endif
//    return true;
//}
