#ifndef CORSAIRCLIENT_H
#define CORSAIRCLIENT_H

#include <cstdint>
#include <cstring>
#include <iostream>

#include "eth/client.h"

class CorsairClient : public Client
{
public:
    CorsairClient();
    ~CorsairClient() override;
    bool send_cmd(const uint8_t * data, const int & data_length);
private:
    uint8_t * buffer;
};

#endif // CORSAIRCLIENT_H
