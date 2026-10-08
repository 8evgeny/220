#include "fenix2cmd_eth_cmd.hpp"

#include "../../eth_cmd_common_types.hpp"

#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{

EthCmdFenix2CmdData::EthCmdFenix2CmdData()
{
    serialize_size_bytes = sizeof(Fenix2Cmd);
    cmd_id = ETH_CMD_FENIX2_CMD_DATA;
}

int EthCmdFenix2CmdData::get_serialize_size() const
{
    return serialize_size_bytes;
}

bool EthCmdFenix2CmdData::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&fenix2cmd), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN

    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmdFenix2CmdData::deserialize(uint8_t const *cmd_buffer)
{
    fenix2cmd = *(reinterpret_cast<Fenix2Cmd const *>(cmd_buffer));
    cmd_buffer += serialize_size_bytes;
    return true;
}

void EthCmdFenix2CmdData::set(const Fenix2Cmd &_fenix2cmd)
{
    fenix2cmd = _fenix2cmd;
}

void EthCmdFenix2CmdData::get(Fenix2Cmd &_fenix2cmd)
{
    _fenix2cmd = fenix2cmd;
}

} // -- namespace eth_cmd_processing
