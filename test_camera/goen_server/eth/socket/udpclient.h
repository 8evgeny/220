#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include "udpsocket.h"

class UDPClient: public UDPSocket
{
public:
    UDPClient():UDPSocket(){}

    bool opn(const SocketAddress & self_address, const SocketAddress & server_address);

    bool send(const void * data, const int & data_length);
    bool send_msg(const std::string & msg);
private:
    SocketAddress _server_address;
};

#endif // UDPCLIENT_H
