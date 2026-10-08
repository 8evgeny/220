#include "corsairclient.h"

#include <cstdint>
#include <cstring>
#include "corsair_global.h"

#define CORSAIR_UDP_BUFFER_LEN  (4096)

CorsairClient::CorsairClient():
    buffer(new uint8_t[CORSAIR_UDP_BUFFER_LEN])
{
    memset(buffer, 0, CORSAIR_UDP_BUFFER_LEN);
}
CorsairClient::~CorsairClient()
{
    delete [] buffer;
}
bool CorsairClient::send_cmd(const uint8_t * data, const int & data_length)
{
    std::cout << "corsair send_cmd" << std::endl;
    cmd_packet * packet = reinterpret_cast<cmd_packet *>(buffer);
    packet->preambule = 0;
    packet->flag = CORSAIR_UDP_CMD_FLAG;
    memcpy(packet->data, data, size_t(data_length));
    return send(buffer, CORSAIR_UDP_CMD_LEN);
}
