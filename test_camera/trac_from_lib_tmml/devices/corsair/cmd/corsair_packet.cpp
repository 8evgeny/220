#include "corsair_packet.h"

#include <cstring>

#define PACKET_LEN_SRV (8)

Packet::Packet(Packet * c) :
    _cmd(c->_cmd),
    _status(c->_status),
    _src_addr(c->_src_addr),
    _dst_addr(c->_dst_addr),
    _data_length(0),
    _data(c->_data)
{
}
Packet::Packet(const uint8_t & _cmd, const uint8_t & _status, const uint16_t & _dst_addr, const uint16_t & _data_length) :
    _cmd(_cmd),
    _status(_status),
    _src_addr(0x0000u),
    _dst_addr(_dst_addr),
    _data_length(_data_length)
{
    _data.clear();
}
Packet::~Packet()
{
    _data.clear();
}
void Packet::append_data(const uint8_t * v, const size_t & l)
{
    for(uint32_t i = 0; i < l; ++i)
    {
        _data.push_back(v[i]);
    }
}
void Packet::append_data(const bool & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const float & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const double & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const int8_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const int16_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const int32_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const int64_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const uint8_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const uint16_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const uint32_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const uint64_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}
void Packet::append_data(const std::vector<uint8_t>& data)
{
    _data = data;
}
uint32_t Packet::to_buffer(uint8_t * buffer)
{
    uint32_t data_length = 0;
    if(_data_length != 0) {
        data_length = _data_length;
    } else {
        data_length = uint32_t(_data.size());
    }

    buffer[0] =  _cmd;
    buffer[1] =  _status;
    buffer[2] =  uint8_t(_src_addr >> 0);
    buffer[3] =  uint8_t(_src_addr >> 8);
    buffer[4] =  uint8_t(_dst_addr >> 0);
    buffer[5] =  uint8_t(_dst_addr >> 8);
    buffer[6] =  uint8_t(data_length / 4 >> 0);
    buffer[7] =  uint8_t(data_length / 4 >> 8);
    if(!_data.empty()) { // Необходима проверка, т.к. в запросе на чтение нет данных
        memcpy(buffer + PACKET_LEN_SRV, _data.data(), data_length);
    }
    return data_length + PACKET_LEN_SRV;
}
bool Packet::from_buffer(const uint8_t * buffer, const uint32_t & buffer_length)
{
    if(buffer_length < PACKET_LEN_SRV) return false;
    auto * p = reinterpret_cast<const _tpacket *>(buffer);
    if(buffer_length < (p->data_length * 4 + PACKET_LEN_SRV)) return false;
    _cmd = p->cmd;
    _status = p->status;
    _src_addr = p->src_addr;
    _dst_addr = p->dst_addr;
    if(!_data.empty()) _data.clear();
    _data.resize(p->data_length * 4);
    memcpy(_data.data(), p->data, p->data_length * 4);
    return true;
}
