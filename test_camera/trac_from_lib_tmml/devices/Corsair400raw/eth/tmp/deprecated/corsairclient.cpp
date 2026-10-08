#include "corsairclient.hpp"

#include <cstdint>
#include <string.h>
#include "eth/EthGlobal.hpp"
#include <memory>

#define UDP_BUFFER_LEN  (4096)

CorsairClient::CorsairClient():
#ifdef PCAP
    PcapClient(),
#else
    UDPClient(CMD_BUF_LEN),
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
    std::copy(data, data + data_length, packet->data);
    return send(buffer, sizeof(cmd_packet) + data_length);
}
