#include "pcapclient.h"

bool PcapClient::opn(const SocketAddress & self_address, const SocketAddress & server_address)
{
    _server_address = server_address;
    if(PcapSocket::opn(self_address))
    {
        set_timeout(0, 100);
        return true;
    }
    return false;
}
bool PcapClient::send(const void * data, const int & data_length)
{
    return sndto(_server_address, data, data_length);
}
bool PcapClient::send_msg(const std::string & msg)
{
    return send(msg.data(), msg.length());
}
