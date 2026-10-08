#include "DataClient.hpp"
#include <memory>

DataClient::DataClient(const uint32_t& sndBufSize,
                       const UDPPacketParametrs& msgPacketPar)
    : UDPClient(sndBufSize)
    , _msgPar{msgPacketPar}
{
}

DataClient::~DataClient()
{

}

bool DataClient::sendCmd(const uint8_t* data, const int32_t& length)
{
    cmd_packet* packet = reinterpret_cast<cmd_packet*>(_sndBuf);
    packet->preambule = 0;
    packet->flag = _msgPar.flag;
    std::copy(data, data + length, packet->data);
    return send(_sndBuf, sizeof(cmd_packet) + length);
}
