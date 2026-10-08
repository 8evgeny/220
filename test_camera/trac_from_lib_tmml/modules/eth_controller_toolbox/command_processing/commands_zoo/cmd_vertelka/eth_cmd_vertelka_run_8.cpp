#include "eth_cmd_vertelka_run_8.hpp"

#include "../../eth_cmd_common_types.hpp"

#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{

EthCmdVertelka_run_8::EthCmdVertelka_run_8()
{
    serialize_size_bytes = sizeof(CmdVertelka_run_8);
    cmd_id = ETH_CMD_VERTELKA_RUN8;
} // END EthCmdVertelka_run_8

int EthCmdVertelka_run_8::get_serialize_size() const
{
    return serialize_size_bytes;
} // END get_serialize_size

bool EthCmdVertelka_run_8::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&cmd_struct), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN

    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmdVertelka_run_8::deserialize(uint8_t const *cmd_buffer)
{
    cmd_struct = *(reinterpret_cast<CmdVertelka_run_8 const *>(cmd_buffer));
    std::cout <<  "CmdVertelka_run_8::was called" << std::endl;
    cmd_buffer += serialize_size_bytes;
    return true;
}

//void EthCmdVertelka_run_8::set(const CmdVertelka_run_8 &_trackmeta)
//{
//    cmd_struct = _trackmeta;
//} // END set

void EthCmdVertelka_run_8::get(CmdVertelka_run_8 &_trackmeta)
{
    std::cout <<  "EthCmdVertelka_close::get was called" << std::endl;
    _trackmeta = cmd_struct;
} // END get

} // -- namespace eth_cmd_processing
