#ifndef ETHCONTROLLER_H
#define ETHCONTROLLER_H

#include <cstdint>
#include <string>

class RAWVideoDataServer;

class EthController
{
private:
    EthController();
public:
    static EthController & inst();
    ~EthController();

    bool start(const std::string & dev,
               const std::string & src_mac, const std::string & src_ip, const uint16_t & src_port,
               const std::string & dst_mac, const std::string & dst_ip, const uint16_t & dst_port);
    bool stop();

    bool isStarted();

    bool sendCmd(const uint8_t* data, const int32_t& length);

    uint8_t* rcvCmd(int32_t& length);
    uint8_t* rcvFrame(int32_t& w, int32_t& h, int32_t& id, int32_t& num);

private:
    RAWVideoDataServer* _server;

    uint32_t _cmdCounter;
};

#endif // ETHCONTROLLER_H
