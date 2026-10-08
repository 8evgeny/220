#include "ap_telemetry_eth_cmd.hpp"

#include "../../eth_cmd_common_types.hpp"
#include <cstring>
#include <cstdint>
#include <cassert>
#include "../../eth_serialization_tools.hpp"


namespace eth_cmd_processing
{

EthCmdAPTelemetry::EthCmdAPTelemetry()
{
    serialize_size_bytes = sizeof(ParitelTelemetryAP);
    cmd_id = ETH_CMD_AP_TELEMETRY;
} // -- constructor EthCmdAPTelemetry

int EthCmdAPTelemetry::get_serialize_size() const
{
    return serialize_size_bytes;
}

bool EthCmdAPTelemetry::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&tele_data), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN


    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmdAPTelemetry::deserialize(uint8_t const *cmd_buffer)
{
    tele_data = *(reinterpret_cast<ParitelTelemetryAP const *>(cmd_buffer));
    cmd_buffer += serialize_size_bytes;
    return true;
}

void EthCmdAPTelemetry::set(const ParitelTelemetryAP &_tele_src)
{
    tele_data = _tele_src;
}

void EthCmdAPTelemetry::get(ParitelTelemetryAP &_tele_dst)
{
    _tele_dst = tele_data;
}

} // -- namespace eth_cmd_processing