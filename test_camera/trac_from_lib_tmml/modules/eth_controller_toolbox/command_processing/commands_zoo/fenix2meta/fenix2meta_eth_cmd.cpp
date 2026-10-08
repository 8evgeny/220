#include "fenix2meta_eth_cmd.hpp"

#include "../../eth_cmd_common_types.hpp"

#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{

EthCmdFenix2MetaData::EthCmdFenix2MetaData()
{
    serialize_size_bytes = sizeof(Fenix2Meta);
    cmd_id = ETH_CMD_FENIX2_METADATA;
}

int EthCmdFenix2MetaData::get_serialize_size() const
{
    return serialize_size_bytes;
}

bool EthCmdFenix2MetaData::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&fenix2meta), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN

    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmdFenix2MetaData::deserialize(uint8_t const *cmd_buffer)
{
    fenix2meta = *(reinterpret_cast<Fenix2Meta const *>(cmd_buffer));
    cmd_buffer += serialize_size_bytes;
    return true;
}

void EthCmdFenix2MetaData::set(const Fenix2Meta &_fenix2meta)
{
    fenix2meta = _fenix2meta;
}

void EthCmdFenix2MetaData::get(Fenix2Meta &_fenix2meta)
{
    _fenix2meta = fenix2meta;
}

} // -- namespace eth_cmd_processing
