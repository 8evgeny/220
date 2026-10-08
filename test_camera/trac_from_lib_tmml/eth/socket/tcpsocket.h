#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "socket.h"

class TCPSocket : public Socket
{
public:
    TCPSocket():Socket(){}
    bool opn(const SocketAddress & address) override {return Socket::opn(address);}
    bool cnct(const SocketAddress & other);
    bool snd(const void * data, const int & size);
    int rcv(void * data, const int & max_size);
    bool lstn();
    bool acc(TCPSocket & client);
private:
    bool opn() override;
private:
    SOCKET other = 0;
};

#endif // TCPSOCKET_H
