#include "SocketAddress.hpp"

#ifdef WIN32
typedef int socklen_t;
#include <winsock2.h>
#include <ws2tcpip.h>
#else
typedef int SOCKET;
#define INVALID_SOCKET (SOCKET)(~0)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <sstream>
#include <cstring>
#include <iostream>

SocketAddress::SocketAddress(const uint16_t & port, const uint32_t & ip, const std::string & mac, const std::string & dev)
{
    _dev  = dev;
    _ip   = ip;
    _port = port;
    memset(_mac, 0x00, 6);
    if(mac.length() > 0)
    {
        set_mac(mac);
    } // END if(mac.length() > 0)
} // END SocketAddress

SocketAddress::SocketAddress(const uint16_t & port, const std::string & ip, const std::string & mac, const std::string & dev)
{
    _dev = dev;
    if(mac.length() > 0)
    {
        set_mac(mac);
    } // END if(mac.length() > 0)
    else
    {
        memset(_mac, 0x00, 6);
    } // END else
    if(ip.length() > 0)
    {
        _ip = ntohl(inet_addr(ip.c_str()));
    } // END if(ip.length() > 0)
    else
    {
        _ip = INADDR_ANY;
    } // END else
    _port = port;
} // END SocketAddress

void SocketAddress::set_mac(const std::string & mac)
{
    std::stringstream ss(mac);
    for(int i = 0; i < 6; ++i)
    {
        std::string b;
        std::getline(ss, b, '-');
        std::stringstream bss(b);
        int t;
        bss >> std::hex >> t;
        _mac[5 - i] = uint8_t(t);
    } // END for(int i = 0; i < 6; ++i)
} // END set_mac

void SocketAddress::set_mac(const uint8_t * mac)
{
    for(int i = 0; i < 6; ++i)
    {
        _mac[5 - i] = mac[i];
    } // END for(int i = 0; i < 6; ++i)
} // END set_mac

void SocketAddress::set_ip(const std::string& ip)
{
    _ip = ntohl(inet_addr(ip.c_str()));
} // END set_ip

void SocketAddress::set_nip(const uint32_t& nip)
{
    _ip = ntohl(nip);
} // END set_nip

void SocketAddress::set_ip(const uint8_t * ip)
{
    uint8_t * _pip = reinterpret_cast<uint8_t *>(&_ip);
    for(int i = 0; i < 4; ++i)
    {
        _pip[3 - i] = ip[i];
    } // END for(int i = 0; i < 4; ++i)
} // END set_ip

void SocketAddress::set_nport(const uint16_t& nport)
{
    _port = ntohs(nport);
} // END set_nport

uint32_t SocketAddress::nip() const
{
    return htonl(_ip);
} // END nip

uint16_t SocketAddress::nport() const
{
    return htons(_port);
} // END nport

std::string SocketAddress::str_mac() const
{
    std::ostringstream os;
    os << std::hex << int(_mac[0]) << ":"
       << std::hex << int(_mac[1]) << ":"
       << std::hex << int(_mac[2]) << ":"
       << std::hex << int(_mac[3]) << ":"
       << std::hex << int(_mac[4]) << ":"
       << std::hex << int(_mac[5]);
    return os.str();
} // END str_mac

std::string SocketAddress::str_ip() const
{
    int a = (_ip >> 24) & 0xFF;
    int b = (_ip >> 16) & 0xFF;
    int c = (_ip >> 8)  & 0xFF;
    int d = (_ip >> 0)  & 0xFF;
    std::ostringstream os;
    os << a << "."
       << b << "."
       << c << "."
       << d;
    return os.str();
} // END str_ip

std::string SocketAddress::str_port() const
{
    std::ostringstream os;
    os << _port;
    return os.str();
} // END str_port

SocketAddress & SocketAddress::operator=(const SocketAddress & other)
{
    _dev  = other._dev;
    _ip   = other._ip;
    _port = other._port;
    memcpy(_mac, other._mac, 6);
    return *this;
} // END SocketAddress::operator=

bool SocketAddress::operator==(const SocketAddress & other) const
{
    return (_dev    == other._dev)    &&
           (_ip     == other._ip)     &&
           (_port   == other._port)   &&
           (_mac[0] == other._mac[0]) &&
           (_mac[1] == other._mac[1]) &&
           (_mac[2] == other._mac[2]) &&
           (_mac[3] == other._mac[3]) &&
           (_mac[4] == other._mac[4]) &&
           (_mac[5] == other._mac[5]);
} // END SocketAddress::operator==

bool SocketAddress::operator!=(const SocketAddress & other) const
{
    return (_dev    != other._dev)    ||
           (_ip     != other._ip)     ||
           (_port   != other._port)   ||
           (_mac[0] != other._mac[0]) ||
           (_mac[1] != other._mac[1]) ||
           (_mac[2] != other._mac[2]) ||
           (_mac[3] != other._mac[3]) ||
           (_mac[4] != other._mac[4]) ||
           (_mac[5] != other._mac[5]);
} // END SocketAddress::operator!=
