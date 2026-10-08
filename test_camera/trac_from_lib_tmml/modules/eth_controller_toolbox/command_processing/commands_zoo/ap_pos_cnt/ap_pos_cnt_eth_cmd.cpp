#include "ap_pos_cnt_eth_cmd.hpp"

#include "../../eth_cmd_common_types.hpp"
#include <cstring>
#include <cstdint>
#include <cassert>
#include "../../eth_serialization_tools.hpp"


namespace eth_cmd_processing
{

EthCmdApPosCnt::EthCmdApPosCnt()
{
    serialize_size_bytes = sizeof(EthCmdDataApPosCnt);
    cmd_id = ETH_CMD_AP_POS_CNT;
} // -- constructor EthCmdAPTelemetry

int EthCmdApPosCnt::get_serialize_size() const
{
    return serialize_size_bytes;
}

bool EthCmdApPosCnt::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&cnt_data), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN

    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmdApPosCnt::deserialize(uint8_t const *cmd_buffer)
{
    cnt_data = *(reinterpret_cast<EthCmdDataApPosCnt const *>(cmd_buffer));
    cmd_buffer += serialize_size_bytes;
    return true;
}

void EthCmdApPosCnt::set(const EthCmdDataApPosCnt &_data)
{
    cnt_data = _data;
}

void EthCmdApPosCnt::get(EthCmdDataApPosCnt &_data)
{
    _data = cnt_data;
}

} // -- namespace eth_cmd_processing