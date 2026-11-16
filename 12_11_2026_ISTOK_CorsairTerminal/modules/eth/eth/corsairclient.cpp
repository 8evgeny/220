#include "corsairclient.h"

#include <cstdint>
#include <cstring>
#include "corsair_global.h"

#define UDP_BUFFER_LEN  (4096)

CorsairClient::CorsairClient():
#ifdef PCAP
    PcapClient(),
#else
    UDPClient(),
#endif
    buffer(new uint8_t[UDP_BUFFER_LEN])
{
    memset(buffer, 0, UDP_BUFFER_LEN);
}
CorsairClient::~CorsairClient()
{
    delete [] buffer;
}
bool CorsairClient::send_cmd(const uint8_t * data, const int & data_length)
{
    cmd_packet * packet = reinterpret_cast<cmd_packet *>(buffer);
    packet->preambule = 0;
    packet->flag = UDP_CMD_FLAG;
    memcpy(packet->data, data, size_t(data_length));
    return send(buffer, UDP_CMD_LEN);
}
