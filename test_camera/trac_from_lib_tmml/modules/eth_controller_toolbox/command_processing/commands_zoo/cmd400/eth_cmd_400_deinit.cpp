#include "eth_cmd_400_deinit.hpp"

#include "../../eth_cmd_common_types.hpp"

#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{

EthCmd400_deinit::EthCmd400_deinit()
{
    serialize_size_bytes = sizeof(Cmd400_deinit);
    cmd_id = ETH_CMD_400_DEINIT;
} // END EthCmd400_deinit

int EthCmd400_deinit::get_serialize_size() const
{
    return serialize_size_bytes;
} // END get_serialize_size

bool EthCmd400_deinit::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&cmd_struct), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN

    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmd400_deinit::deserialize(uint8_t const *cmd_buffer)
{
    cmd_struct = *(reinterpret_cast<Cmd400_deinit const *>(cmd_buffer));
    cmd_buffer += serialize_size_bytes;
    return true;
}

//void EthCmd400_deinit::set(const Cmd400_deinit &_trackmeta)
//{
//    cmd_struct = _trackmeta;
//} // END set

void EthCmd400_deinit::get(Cmd400_deinit &_trackmeta)
{
    _trackmeta = cmd_struct;
} // END get

} // -- namespace eth_cmd_processing
