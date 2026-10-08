#include "track_meta_eth_cmd.hpp"

#include "../../eth_cmd_common_types.hpp"

#include <cstring>
#include <cstdint>
#include <cassert>

namespace eth_cmd_processing
{

EthCmdTrackerMetaData::EthCmdTrackerMetaData()
{
    serialize_size_bytes = sizeof(TrackMeta2AP);
    cmd_id = ETH_CMD_TRACKER_METADATA;
}

int EthCmdTrackerMetaData::get_serialize_size() const 
{
    return serialize_size_bytes;
}

bool EthCmdTrackerMetaData::serialize_noalloc(uint8_t *cmd_buffer, int &buffer_size_bytes) const
{
    memcpy(cmd_buffer, reinterpret_cast<uint8_t const *>(&trackmeta), serialize_size_bytes);

    #ifdef SYSTEM_BIG_ENDIAN
    ReverseBytes(byte_arr_ptr, serialize_size_bytes);
    #endif //SYSTEM_BIG_ENDIAN

    cmd_buffer += serialize_size_bytes;
    buffer_size_bytes = serialize_size_bytes;
    return true;
}

bool EthCmdTrackerMetaData::deserialize(uint8_t const *cmd_buffer)
{
    trackmeta = *(reinterpret_cast<TrackMeta2AP const *>(cmd_buffer));
    cmd_buffer += serialize_size_bytes;
    return true;
}

void EthCmdTrackerMetaData::set(const TrackMeta2AP &_trackmeta)
{
    trackmeta = _trackmeta;
}

void EthCmdTrackerMetaData::get(TrackMeta2AP &_trackmeta)
{
    _trackmeta = trackmeta;
}

} // -- namespace eth_cmd_processing
