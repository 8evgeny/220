#ifndef FENIX2META_HPP
#define FENIX2META_HPP
#include <chrono>
// телеметрия, формируемая на стороне сервера (например, шара)

#pragma pack(push,1)
struct Fenix2Meta
{
bool track_is_init = 0; // состояние инициализации трека
float trackx = -1; // положение трека по x
float tracky = -1; // положение трека по у
float diff_trackx = -1; // разница между положением трека и центром экрана по х
float diff_tracky = -1; // -- у
unsigned long int tp_send = 0; // dbg::
unsigned long int tp_keep = 0;
// float gimble_yaw = 0; // рыскание подвеса камеры
// float fimbal_pitch = 0; // тангаж подвеса камеры
// float velocity gimnal_yaw = 0; // угловые скорости камеры по осям вращения
// float velocity_gimbal_pitch = 0;

};
#pragma pack(pop)

#endif // FENIX2META_HPP
