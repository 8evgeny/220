#ifndef VTRACK_DATA_STRUCT_HPP
#define VTRACK_DATA_STRUCT_HPP

#include <cstdint>
#include <vector>
#include <string>

namespace track_pos_sender
{

#pragma pack(push,1)
struct trackPos
{
    float x = -1;
    float y = -1;
    float w = -1;
    float h = -1;
}; // END trackPos
#pragma pack(pop)


#pragma pack(push,1)
struct vtrackDataStruct
{
    std::vector<trackPos> v_track;
}; // END vtrackDataStruct


struct StateCmdStruct
{
    uint8_t cmd_id = 0;
    uint8_t state = 0;
    uint8_t buf_zeros[64];
}; // END StateCmdStruct


#pragma pack(pop)

struct Settings
{
    std::string ip_v4 = "127.0.0.1";
    int port = 8080 ;
    uint8_t track_cmd_id = 198;
    uint8_t state_cmd_id = 199;
}; // END Settings



/// Вид пакета:
/// cmd_id (1байт - uint8_t) + size_vec (1байт uint8_t) + trackPos_0 (16 байт float*4) + ... trackPos[size_vec - 1] (16 байт)
/// где x, y - положение положение (левого верхнего угла? или центра?) рамки трека (в относительных координатах)
/// w, h - размер рамки трека в относительных координатах



}; // END namespace track_pos_sender
#endif // VTRACK_DATA_STRUCT_HPP
