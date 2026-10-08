#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "tcpsocket.h"

class TCPClient: public TCPSocket
{
public:
    TCPClient():TCPSocket(){}

    bool opn(const SocketAddress & self_address, const SocketAddress & server_address);

    bool send(const void * data, const int & data_length);
    bool send_msg(const std::string & msg);
private:
    SocketAddress _server_address;
};

#endif // TCPCLIENT_H
