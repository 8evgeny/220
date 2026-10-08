#ifndef OwletCLIENT_H
#define OwletCLIENT_H

#include <cstdint>
#include <cstring>
#include <iostream>

#include "eth/client.h"

class OwletClient : public Client
{
public:
    OwletClient();
    ~OwletClient() override;
    bool send_cmd(const uint8_t * data, const int & data_length);
private:
    uint8_t * buffer;
};

#endif // OwletCLIENT_H
