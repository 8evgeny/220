#include "corsair_telemetry.h"

#include <cstring>

//#define EXT_TELEMETRY_OFFSET (340)//30)//340)
//#define PCCOM_TELEMETRY (0xF3u)

bool Corsair_Telemetry::from_buffer(const uint8_t * buffer, const uint32_t & buffer_len)
{
    // TODO: parse telemetry from standart packet
    if(!Packet::from_buffer(buffer, buffer_len)) return false;
    if(_dst_addr != 0x0000u) return false;
    if(_data.size() < sizeof(t_telemetry_struct)) return false;
    memcpy(&telemetry, &_data.at(0), sizeof(t_telemetry_struct));
    return true;
//    if(!Cmd::from_buffer(buffer, buffer_len)) return false;
//    if(_cmd_cmd != PCCOM_TELEMETRY) return false;
//    if(_data.size() < (/*sizeof(t_telemetry_struct)*/149 + EXT_TELEMETRY_OFFSET)) return false;
//    memcpy(&telemetry, &_data.at(EXT_TELEMETRY_OFFSET), sizeof(t_telemetry_struct));
//    return true;
}

bool Corsair_Telemetry::from_packet(Packet& packet)
{
    _cmd      = packet.cmd();
    _status   = packet.status();
    _src_addr = packet.src_addr();
    _dst_addr = packet.dst_addr();
    _data     = packet.data();
    //_data.resize(packet.data().size());
    size_t data_length;
    memcpy(&telemetry,  packet.ptr_data(data_length), sizeof(t_telemetry_struct));
    //if (data_len != sizeof(t_telemetry_struct)) return false;
    _data_length = _data.size();

    return true;
}

Corsair_Telemetry::t_telemetry_struct * Corsair_Telemetry::get()
{
    return &telemetry;
}
