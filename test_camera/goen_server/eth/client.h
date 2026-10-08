#ifndef CLIENT_H
#define CLIENT_H

#include "ethcontroller.h"

#ifdef PCAP
#include "socket/pcapclient.h"
class Client : public PcapClient
#else
#include "socket/udpclient.h"
class Client : public UDPClient
#endif
{
public:
    Client(){}
    ~Client(){}
    virtual bool send_cmd(const uint8_t * data, const int & data_length) = 0;
    virtual bool send_frame(const uint8_t * f, const int & w, const int & h, const int & num, const int & id)
    {
        std::cout <<  "Can't send frame to current device" << std::endl;
        return false;
    }
private:
    uint8_t * buffer = nullptr;
};

#endif // CLIENT_H
