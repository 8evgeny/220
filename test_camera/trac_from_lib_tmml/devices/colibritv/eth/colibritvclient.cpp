#include "colibritvclient.h"

#include <cstdint>
#include <cstring>

#define COLIBRITV_UDP_BUFFER_LEN  (4096)

ColibriTVClient::ColibriTVClient() :
    buffer(new uint8_t[COLIBRITV_UDP_BUFFER_LEN])
{
    memset(buffer, 0, COLIBRITV_UDP_BUFFER_LEN);
}

ColibriTVClient::~ColibriTVClient()
{
    delete [] buffer;
}

bool ColibriTVClient::send_cmd(const uint8_t *data, const int &data_length)
{
    return true;
}
