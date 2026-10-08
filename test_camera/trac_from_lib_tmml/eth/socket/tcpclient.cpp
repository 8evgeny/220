#include "tcpclient.h"

bool TCPClient::opn(const SocketAddress & self_address, const SocketAddress & server_address)
{
    _server_address = server_address;
    if(TCPSocket::opn(self_address))
    {
        set_broadcast(true);
        set_timeout(1, 0);
        if(cnct(_server_address)) return true;
        cls();
        return false;
    }
    return false;
}
bool TCPClient::send(const void * data, const int & data_length)
{
    return snd(data, data_length);
}
bool TCPClient::send_msg(const std::string & msg)
{
    return send(msg.data(), msg.length());
}
