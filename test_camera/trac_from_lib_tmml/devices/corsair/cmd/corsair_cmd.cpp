#include "corsair_cmd.h"

#include <cstring>

#define PACKET_LEN_SRV (8)

Corsair_Cmd::Corsair_Cmd(const uint16_t & _cmd, const uint16_t & _scmd):
    Packet(CMD_WR_REQ, 0x00u, 0x0103u),
    _cmd_cmd(_cmd),
    _cmd_scmd(_scmd)
{
}
uint32_t Corsair_Cmd::to_buffer(uint8_t * buffer)
{
    uint32_t data_length = uint32_t(_data.size());
    buffer[0]  =  _cmd;
    buffer[1]  =  _status;
    buffer[2]  =  uint8_t(_src_addr >> 0);
    buffer[3]  =  uint8_t(_src_addr >> 8);
    buffer[4]  =  uint8_t(_dst_addr >> 0);
    buffer[5]  =  uint8_t(_dst_addr >> 8);
    buffer[6]  =  uint8_t(((data_length + 3) / 4 + 1) >> 0);
    buffer[7]  =  uint8_t(((data_length + 3) / 4 + 1) >> 8);
    buffer[8]  =  uint8_t(_cmd_cmd >> 0);
    buffer[9]  =  uint8_t(_cmd_cmd >> 8);
    buffer[10] =  uint8_t(_cmd_scmd >> 0);
    buffer[11] =  uint8_t(_cmd_scmd >> 8);
    memcpy(buffer + PACKET_LEN_SRV + 4, _data.data(), data_length);
    return data_length + PACKET_LEN_SRV + 4;
}
bool Corsair_Cmd::from_buffer(const uint8_t * buffer, const uint32_t & buffer_length)
{
    if(buffer_length < PACKET_LEN_SRV) return false;
    auto * p = reinterpret_cast<const _tpacket *>(buffer);
    if(buffer_length < (p->data_length * 4 + PACKET_LEN_SRV)) return false;
    _cmd = p->cmd;
    _status = p->status;
    _src_addr = p->src_addr;
    _dst_addr = p->dst_addr;
    auto * c = reinterpret_cast<const _tcmd *>(p->data);
    _cmd_cmd = c->cmd;
    _cmd_scmd = c->scmd;
    if(!_data.empty()) _data.clear();
    _data.resize(p->data_length * 4 - 4);
    memcpy(_data.data(), c->data, p->data_length * 4 - 4);
    return true;
}
