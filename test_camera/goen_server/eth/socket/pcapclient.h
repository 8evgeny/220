#ifndef PCAPCLIENT_H
#define PCAPCLIENT_H

#include "pcapsocket.h"

class PcapClient: public PcapSocket
{
public:
    PcapClient():PcapSocket(){}

    bool opn(const SocketAddress & self_address, const SocketAddress & server_address);

    bool send(const void * data, const int & data_length);
    bool send_msg(const std::string & msg);
private:
    SocketAddress _server_address;
};

#endif // PCAPCLIENT_H
