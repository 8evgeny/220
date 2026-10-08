#ifndef SHAREDMEMORYCLIENT_HPP
#define SHAREDMEMORYCLIENT_HPP

#include <cstdint>
#include <cstring>
#include <iostream>

#include "eth/client.h"

class SharedMemoryClient : public Client
{
public:
    SharedMemoryClient():Client(){}
    bool send(const std::vector<uint8_t> & cmd);
    bool send_cmd(const uint8_t * data, const int & data_length){}
};

#endif // SHAREDMEMORYCLIENT_HPP
