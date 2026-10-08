#include "eth_cmd_400_record_on.hpp"

#include "../../eth_cmd_common_types.hpp"

#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{

EthCmd400_record_on::EthCmd400_record_on()
{
    serialize_size_bytes = sizeof(Cmd400_record_on);
    cmd_id = ETH_CMD_400_RECORD_ON;
} // END EthCmd400_record_on

int EthCmd400_record_on::get_serialize_size() const
{
    return serialize_size_bytes;
} // END get_serialize_size

bool EthCmd400_record_on::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&cmd_struct), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN

    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmd400_record_on::deserialize(uint8_t const *cmd_buffer)
{
    cmd_struct = *(reinterpret_cast<Cmd400_record_on const *>(cmd_buffer));
    cmd_buffer += serialize_size_bytes;
    return true;
}

//void EthCmd400_recoed_on::set(const Cmd400_record_on &_trackmeta)
//{
//    cmd_struct = _trackmeta;
//} // END set

void EthCmd400_record_on::get(Cmd400_record_on &_trackmeta)
{
    _trackmeta = cmd_struct;
} // END get

} // -- namespace eth_cmd_processing
