#ifndef CORSAIRCLIENT_H
#define CORSAIRCLIENT_H

#ifdef ETH_PROTOCOL_PCAP
#include "eth/protocols/pcap/PCAPClient.h"
class CorsairClient : public PcapClient
#else
#include "eth/protocols/udp/UDPClient.hpp"
class CorsairClient : public UDPClient
#endif
{
public:
    CorsairClient();
    ~CorsairClient();
    bool send_cmd(const uint8_t * data, const int & data_length);
private:
    uint8_t * buffer;
};

#endif // CORSAIRCLIENT_H
