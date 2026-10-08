#include "eth_cmd_vertelka_close.hpp"

#include "../../eth_cmd_common_types.hpp"

#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{

EthCmdVertelka_close::EthCmdVertelka_close()
{
    serialize_size_bytes = sizeof(CmdVertelka_close);
    cmd_id = ETH_CMD_VERTELKA_CLOSE;
} // END EthCmdVertelka_close

int EthCmdVertelka_close::get_serialize_size() const
{
    return serialize_size_bytes;
} // END get_serialize_size

bool EthCmdVertelka_close::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&cmd_struct), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN

    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmdVertelka_close::deserialize(uint8_t const *cmd_buffer)
{
    cmd_struct = *(reinterpret_cast<CmdVertelka_close const *>(cmd_buffer));
    std::cout <<  "CmdVertelka_close::was called" << std::endl;
    cmd_buffer += serialize_size_bytes;
    return true;
}

//void EthCmdVertelka_close::set(const CmdVertelka_close &_trackmeta)
//{
//    cmd_struct = _trackmeta;
//} // END set

void EthCmdVertelka_close::get(CmdVertelka_close &_trackmeta)
{
    std::cout <<  "EthCmdVertelka_close::get was called" << std::endl;
    _trackmeta = cmd_struct;
} // END get

} // -- namespace eth_cmd_processing
