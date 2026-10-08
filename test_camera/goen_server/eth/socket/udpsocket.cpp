#include "udpsocket.h"

#include <iostream>
#include <cstring>

bool UDPSocket::opn()
{
    cls();
    // Create Socket
    _sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(_sock <= 0)
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
    std::cout << "udp socket opened on " << _address.str_ip() << ":" << _address.port() << std::endl;
#endif
    return true;
}
bool UDPSocket::sndto(const SocketAddress & dst, const void * data, const int & size)
{
    if(!is_open()) return false;
    // Create Destination Address
    sockaddr_in dst_address;
    memset(&dst_address, 0, sizeof(dst_address));
    dst_address.sin_family = AF_INET;
    dst_address.sin_addr.s_addr = dst.nip();
    dst_address.sin_port = dst.nport();
    // Send message
    int sent_bytes = sendto(_sock, reinterpret_cast<const char*>(data), size, 0, reinterpret_cast<sockaddr *>(&dst_address), sizeof(dst_address));
    if(sent_bytes != size)
    {
#ifdef DEBUG
        std::cout << "sending error" << std::endl;
#endif
        return false;
    }
    return true;
}
int UDPSocket::rcvfrom(SocketAddress & src, void * data, const int & max_size)
{
    if(!is_open()) return false;
    // Create Source Address
    sockaddr_in src_address;
    socklen_t src_address_length = sizeof(src_address);
    // Receive data
    int received_bytes = recvfrom(_sock, reinterpret_cast<char *>(data), max_size, 0, reinterpret_cast<sockaddr *>(&src_address), &src_address_length);
    src.set_nip(src_address.sin_addr.s_addr);
    src.set_nport(src_address.sin_port);
    return received_bytes;
}
