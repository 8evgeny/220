#ifndef SOCKETADDRESS_H
#define SOCKETADDRESS_H

#include <string>

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

class SocketAddress
{
public:
    SocketAddress(const uint16_t & port,     const std::string & ip,              const std::string & mac = "", const std::string & dev = "");
    SocketAddress(const uint16_t & port = 0, const uint32_t    & ip = INADDR_ANY, const std::string & mac = "", const std::string & dev = "");

    void set_dev(  const std::string & dev)   {_dev = dev;}
    void set_mac(  const std::string & mac);
    void set_mac(  const uint8_t     * mac);
    void set_ip(   const std::string & ip);
    void set_ip(   const uint32_t    & ip)    {_ip = ip;}
    void set_nip(  const uint32_t    & nip);
    void set_ip(   const uint8_t     * ip);
    void set_port( const uint16_t    & port)  {_port = port;}
    void set_nport(const uint16_t    & nport);

    std::string dev()     const {return _dev;}
    const uint8_t * mac() const {return _mac;}
    const uint8_t * pip() const {return reinterpret_cast<const uint8_t *>(&_ip);}
    uint32_t ip()         const {return _ip;}
    uint32_t nip()        const;
    uint16_t port()       const {return _port;}
    uint16_t nport()      const;

    std::string str_mac() const;
    std::string str_ip() const;
    std::string str_port() const;

    SocketAddress & operator=(const SocketAddress & other);
    bool operator==(const SocketAddress & other) const;
    bool operator!=(const SocketAddress & other) const;

private:
    std::string _dev;
    uint8_t     _mac[6];
    uint32_t    _ip;
    uint16_t    _port;
};

#endif // SOCKETADDRESS_H
