#include "telemetry_orientation_eth_cmd.hpp"

#include "../../eth_cmd_common_types.hpp"
#include <cstring>
#include <cstdint>
#include <cassert>
#include "../../eth_serialization_tools.hpp"


namespace eth_cmd_processing
{

EthCmdOrientTelemetry::EthCmdOrientTelemetry()
{
    serialize_size_bytes = sizeof(data);
    cmd_id = ETH_CMD_ORIENTATION_TELEMETRY;
} // -- constructor EthCmdOrientTelemetry

int EthCmdOrientTelemetry::get_serialize_size() const
{
    return serialize_size_bytes;
}

bool EthCmdOrientTelemetry::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&data), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN


    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmdOrientTelemetry::deserialize(uint8_t const *cmd_buffer)
{
    data = *(reinterpret_cast<OrientationData const *>(cmd_buffer));
    cmd_buffer += serialize_size_bytes;
    return true;
}

void EthCmdOrientTelemetry::set(const OrientationData &_data)
{
    data = _data;
}

void EthCmdOrientTelemetry::get(OrientationData &_tele_dst)
{
    _tele_dst = data;
}

} // -- namespace eth_cmd_processing