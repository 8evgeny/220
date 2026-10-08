#include "owlet_telemetry.h"

#include <cstring>

bool Telemetry::from_buffer(const uint8_t * buffer, const uint32_t & buffer_len)
{
    if(!Cmd_v2::from_buffer(buffer, buffer_len)) return false;
    if(dst_addr != ((en_vbd_host << 8) | (telemetry_addr))) return false;
    memcpy(&telemetry, data.data(), sizeof(t_telemetry_struct));
    return true;
}

Telemetry::t_telemetry_struct * Telemetry::get()
{
    return &telemetry;
}
