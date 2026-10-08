#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>

class SocketAddress;

class TCPClient
{
public:
    TCPClient();
    ~TCPClient();

    bool open(const SocketAddress & self_address, const SocketAddress & server_address);

    bool send(const void * data, const int & data_length);
    bool send_msg(const std::string & msg);
private:
    SocketAddress* _serverAddress;
};

#endif // TCPCLIENT_H
