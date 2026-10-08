#ifndef COLIBRITVCLIENT_H
#define COLIBRITVCLIENT_H

#include <cstdint>
#include <cstring>

#include "eth/client.h"

class ColibriTVClient : public Client
{
public:
    ColibriTVClient();
    ~ColibriTVClient() override;
     bool send_cmd(const uint8_t * data, const int & data_length);
private:
    uint8_t * buffer;
};

#endif // COLIBRITVCLIENT_H
