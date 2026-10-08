#include "eth_cmd_id_simple_direct_enc.hpp"

#include <vector>
#include <algorithm>
#include <cstring>

using namespace cmd_id;

double pow_i(int base, unsigned power)
{
    double val = 1;
    for (unsigned i = 0; i < power; i++)
    {
        val *= base;
    }
    return val;
} // -- END pow_i

void dec2bytes(int _src, std::vector<uint8_t> &_dst)
{
    int i = 0;
    int base = 256;
    _dst.clear();
    while(_src != 0)
    {
        _dst[i] = _src % base;
        _src = _src / base;
        ++i;
    }
    std::reverse(_dst.begin(), _dst.end());
} // -- END dec2bytes

void bytes2dec(const std::vector<uint8_t> &_src, int _dst)
{
    int result = 0;
    int base = 256;
    int src_len = _src.size();
    for(int i = 0; i < src_len; ++i)
    {
        result += _src[i] * pow_i(base, src_len - i - 1);
    }
    _dst = result;
} // -- END bytes2dec

CmdIdSimpleDirectEnc::CmdIdSimpleDirectEnc(int _id_byte_len, bool &ok)
{
    ok = true;
    if(_id_byte_len < 1)
    {
        ok = false;
        return;
    }
    serialize_size = _id_byte_len;
    min_support_id = 0;
    max_support_id = pow_i(256, serialize_size) - 1;
} // -- CmdIdSimpleDirectEnc(int _id_byte_len, bool &ok)

bool CmdIdSimpleDirectEnc::check_support(int net_id) const
{
    return (net_id >= 0 && net_id <= max_support_id);
} // -- END check_support

bool CmdIdSimpleDirectEnc::encode(int id, uint8_t* cmd_bytebuffer) const
{
    if(!check_support(id))
    {
        return false;
    }
    if(serialize_size == 1)
    {
        *cmd_bytebuffer = static_cast<uint8_t>(id);
    }
    else
    {
        std::vector<uint8_t> id_vec_bytes;
        dec2bytes(id, id_vec_bytes);
        std::memcpy(cmd_bytebuffer, id_vec_bytes.data(), id_vec_bytes.size());
    }
    return true;
} // -- END encode

bool CmdIdSimpleDirectEnc::decode(uint8_t const * cmd_bytebuffer, int &id) const
{
    if(serialize_size == 1)
    {
        id = *cmd_bytebuffer;
    }
    else
    {
        std::vector<uint8_t> id_vec_bytes;
        id_vec_bytes.reserve(serialize_size);
        for(size_t i = 0; i < serialize_size; ++i)
        {
            id_vec_bytes.push_back(*cmd_bytebuffer);
            cmd_bytebuffer++;
        }
        bytes2dec(id_vec_bytes, id);
    }
    return true;
} // -- END decode