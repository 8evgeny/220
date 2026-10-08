#include "tcpsocket.h"

#include <iostream>
#include <cstring>

bool TCPSocket::cnct(const SocketAddress & other)
{
    if(!is_open()) return false;
    // Create Address
    sockaddr_in other_address;
    memset(&other_address, 0, sizeof(other_address));
    other_address.sin_family = AF_INET;
    other_address.sin_addr.s_addr = other.nip();
    other_address.sin_port = other.nport();
    // Open Connection
    if(connect(_sock, reinterpret_cast<sockaddr *>(&other_address), sizeof(other_address)) < 0)
    {
#ifdef DEBUG
       std::cout << "connection error" << std::endl;
#endif
        return false;
    }
    return true;
}
bool TCPSocket::snd(const void * data, const int & size)
{
    if(!is_open()) return false;
    // Send data
    int sent_bytes = send(_sock, reinterpret_cast<const char*>(data), size, 0);
    if(sent_bytes != size)
    {
#ifdef DEBUG
       std::cout << "sending error" << std::endl;
#endif
        return false;
    }
    return true;
}
int TCPSocket::rcv(void * data, const int & max_size)
{
    if(!is_open()) return false;
    // Receive data
    int received_bytes = recv(_sock, reinterpret_cast<char *>(data), max_size, 0);
    return received_bytes;
}
bool TCPSocket::lstn()
{
    if(!is_open()) return false;
    int res = listen(_sock, SOMAXCONN);
    if(res < 0)
    {
#ifdef DEBUG
       std::cout << "listen failed" << std::endl;
#endif
        return false;
    }
    return true;
}
bool TCPSocket::acc(TCPSocket & client)
{
    if(!is_open()) return false;
    // Accept a client socket
    SOCKET client_socket = accept(_sock, nullptr, nullptr);
    if(client_socket == INVALID_SOCKET)
    {
        return false;
    }
    client.set_sock(client_socket);
    return true;
}
bool TCPSocket::opn()
{
    cls();
    // Create Socket
    _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_sock == INVALID_SOCKET)
    {
#ifdef DEBUG
        std::cout << "failed to create socket" << std::endl;
#endif
        return false;
    }
    // Set Socket Port Reusable
    if(!set_reusable(true))
    {
#ifdef DEBUG
        std::cout << "failed to set socket port reusable" << std::endl;
#endif
    }
    // Create Address
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = _address.nip();
    address.sin_port = _address.nport();
    // Bind Socket to Address
    if(bind(_sock, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
    {
#ifdef DEBUG
        std::cout << "failed to bind socket" << std::endl;
#endif
        cls();
        return false;
    }
#ifdef DEBUG
   std::cout << "tcp socket opened on " << _address.str_ip() <<":" << _address.port() << std::endl;
#endif
    return true;
}
