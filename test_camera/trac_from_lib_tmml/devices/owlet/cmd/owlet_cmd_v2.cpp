#include "owlet_cmd_v2.h"

#include <cstring>

#define OWLET_CMD_HEADER_LEN (8)

Cmd_v2::Cmd_v2(Cmd_v2 * c) :
    command(c->get_cmd()),
    status(c->get_status()),
    src_addr(c->get_src_addr()),
    dst_addr(c->get_dst_addr()),
    data(c->get_data())
{
}

Cmd_v2::Cmd_v2(const uint16_t & dst_addr,
               const uint8_t  & command,
               const uint8_t  & status,
               const uint16_t & src_addr) :
    command(command),
    status(status),
    src_addr(src_addr),
    dst_addr(dst_addr)
{
    data.clear();
}

Cmd_v2::~Cmd_v2()
{
    data.clear();
}

void Cmd_v2::set_cmd(const uint8_t & v)
{
    command = v;
}

void Cmd_v2::set_status(const uint8_t & v)
{
    status = v;
}

void Cmd_v2::set_src_addr(const uint16_t & v)
{
    src_addr = v;
}

void Cmd_v2::set_dst_addr(const uint16_t & v)
{
    dst_addr = v;
}

void Cmd_v2::set_data(const std::vector<uint8_t> & v)
{
    data = v;
}

void Cmd_v2::clear_data()
{
    data.clear();
}

void Cmd_v2::append_data(const uint8_t *v, const uint32_t &l)
{
    for(uint32_t i = 0; i < l; ++i)
    {
        data.push_back(v[i]);
    }
}

void Cmd_v2::append_data(const uint8_t &v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}

void Cmd_v2::append_data(const uint16_t &v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}

void Cmd_v2::append_data(const uint32_t & v)
{
    append_data(reinterpret_cast<const uint8_t *>(&v), sizeof(v));
}

uint8_t Cmd_v2::get_cmd()
{
    return command;
}

uint8_t Cmd_v2::get_status()
{
    return status;
}

uint16_t Cmd_v2::get_src_addr()
{
    return src_addr;
}

uint16_t Cmd_v2::get_dst_addr()
{
    return dst_addr;
}

std::vector<uint8_t> Cmd_v2::get_data()
{
    return data;
}

uint32_t Cmd_v2::get_data_len()
{
    return uint32_t(data.size());
}

uint8_t *Cmd_v2::get_data(uint32_t &data_len)
{
    data_len = uint32_t(data.size());
    return data.data();
}

uint32_t Cmd_v2::to_buffer(uint8_t *buffer)
{
    uint32_t data_len = uint32_t(data.size());
    uint32_t full_len = data_len*sizeof(data[0]) + OWLET_CMD_HEADER_LEN; // в байтах
    buffer[0] =  uint8_t(command);
    buffer[1] =  uint8_t(status);
    buffer[2] =  uint8_t(src_addr >> 0);
    buffer[3] =  uint8_t(src_addr >> 8);
    buffer[4] =  uint8_t(dst_addr >> 0);
    buffer[5] =  uint8_t(dst_addr >> 8);
    buffer[6] =  uint8_t((data_len>>2) >> 0);
    buffer[7] =  uint8_t((data_len>>2) >> 8);
    memcpy(buffer + OWLET_CMD_HEADER_LEN, reinterpret_cast<const uint8_t *>(data.data()), data_len*sizeof(data[0]));
    return full_len;
}

bool Cmd_v2::from_buffer(const uint8_t * buffer, const uint32_t & buffer_len)
{
    if(buffer_len < OWLET_CMD_HEADER_LEN) return false;
    auto * c = reinterpret_cast<const Cmd_v2::cmd *>(buffer);
    if(buffer_len < (c->data_length*sizeof(data[0]))) return false;
    command  = c->command;
    status   = c->status;
    src_addr = c->src_addr;
    dst_addr = c->dst_addr;
    uint16_t data_len = 4*c->data_length;
    if(!data.empty()) data.clear();
    data.resize(data_len);
    memcpy(data.data(), c->data, data_len);
    //data.resize(data_len*sizeof(data[0]));
    //memcpy(data.data(), c->data, data_len*sizeof(data[0]));
    return true;
}
