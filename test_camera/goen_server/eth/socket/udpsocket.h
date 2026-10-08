#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "socket.h"

class UDPSocket: public Socket
{
public:
    UDPSocket():Socket(){}
    bool opn(const SocketAddress & address) override {return Socket::opn(address);}

    bool sndto(const SocketAddress & dst, const void * data, const int & size);
    int rcvfrom(SocketAddress & src, void * data, const int & max_size);
private:
    bool opn() override;
};

#endif // UDPSOCKET_H
