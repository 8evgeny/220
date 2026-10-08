#ifndef SOCKET_H
#define SOCKET_H

#include "socketaddress.h"

class Socket
{
public:
    Socket():_sock(INVALID_SOCKET){}
    virtual ~Socket(){cls();}
    virtual bool opn(const SocketAddress & address) {_address = address;return opn();}
    virtual void cls();
    virtual inline bool is_open() const {return (_sock != INVALID_SOCKET);}

    void set_sock(const SOCKET & sock);

    virtual bool set_broadcast(const bool & broadcast);
    virtual bool set_reusable(const bool & reusable);
    virtual bool set_non_blocking(const bool & non_blocking);
    virtual bool set_timeout(const int & sec, const int & usec);
    virtual bool set_buffer_size(const int & size);

    bool set_address(const SocketAddress & address){return opn(address);}
    SocketAddress address() const {return _address;}

    static bool init_sockets();
    static void deinit_sockets();
protected:
    virtual bool opn() = 0;
protected:
    SOCKET _sock;
    SocketAddress _address;
};

#endif // SOCKET_H
