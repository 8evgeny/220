#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "eth/base/Socket.hpp"

class TCPSocket : public Socket
{
public:
    TCPSocket();
    bool cnct(const SocketAddress & other);
    bool snd(const void * data, const int & size);
    int rcv(void * data, const int & max_size);
    bool lstn();
    bool acc(TCPSocket & client);
//private:
//    bool open() override;
private:
    SOCKET other;
};

#endif // TCPSOCKET_H
