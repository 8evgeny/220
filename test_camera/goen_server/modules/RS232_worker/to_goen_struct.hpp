#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct to_goen_command
{
    uint8_t sync_code1 = 0xEB;              // <0> Hexadecimal. синхронизирующий код, закреплен
    uint8_t sync_code2 = 0x90;              // <1> Hexadecimal. синхронизирующий код, закреплен
    uint8_t control_param = 0x00;           // <2> Hexadecimal. Основной код команды. В зависимости от его значения задействуются различные функции
    int16_t parameter_x = 0;                // <3-4> Если control_param==0x0D "pointing trackong": отклонение по х (0 в центре изображения)
                                            //       Если control_param==0x24 "PTZ search": скорость по азимуту (0.1 град.сек)
                                            //       Если control_param==0x26 "specified frame angle" or control_param==0x3B "specified space angle":
                                            //            ручная установка азимута (c шагом 0.01 град)
                                            //       Если control_param==0x2С "Suppress gyro drift": коррекция ошибки гироскопа (-2000 до +2000)
                                            //       Если control_param==0x5A "specified zoom factor": ручная установка zoom (шаг 0.1 раз)
                                            //       Если control_param==0xB0 "lift control":
                                            //            0 - stop, 1 - поднять, 2 - опустить
                                            //       Если control_param==0x4A "image board power control":
                                            //            0- restart, 1 - power on, 2 - power off
    int16_t parameter_y = 0;                // <5-6> Если control_param==0x0D "pointing trackong": отклонение по y (0 в центре изображения)
                                            //       Если control_param==0x24 "PTZ search": скорость по тангажу (0.1 град.сек)
                                            //       Если control_param==0x26 "specified frame angle" or control_param==0x3B "specified space angle":
                                            //            ручная установка тангажа (c шагом 0.01 град)
    uint8_t parameter3 = 0;                 // <7> для индикации нескольких окон.чтобы отенить мультиокна, равен 0
    int8_t zoom_rate = 0;                   // <8> если control_param == 0x25, значение от 0 до +100 увеличение
                                            //      от 0 до - 100 уменьшение
                                            // <9-14> - резерв
    uint8_t checksum = 0x00;                // <15> контрольная сумма по младшим 8 битам
}; // END struct to_goen_command

struct to_goen_telemetry
{
    uint8_t sync_code1 = 0xEB;              // <0> Hexadecimal. синхронизирующий код, закреплен
    uint8_t sync_code2 = 0x91;              // <1> Hexadecimal. синхронизирующий код, закреплен
    int16_t roll_angle = 0;                 // <2-3> угол крена. (от хвоста к носу). наклон вправо - положительный, влево - отрицательный
    int16_t pitch_angle = 0;                // <4-5> угол тангажа. Нос вверх - положительный, вниз отрицательный
    int16_t yaw_angle = 0;                  // <6-7> угол рысканья. по часовой стрелке - положительный, против - отрицательный
    int32_t gps_latitude = 0;               // <8-11> широта
    int32_t gps_longitude = 0;              // <12-15> долгота
    int16_t altitude = 0;                   // <16-17> истинная высота
    int16_t relative_altitude = 0;          // <18-19> относительная высота
    uint8_t year = 0;                       // <20> Год (+2000)
    uint8_t month = 0;                      // <21> Месяц
    uint8_t day = 0;                        // <22> День
    uint8_t hour = 0;                       // <23> час
    uint8_t minute = 0;                     // <24> минуты
    uint8_t second = 0;                     // <25> секунды
    uint8_t centisecond = 0;                // <26> сотая доля секунды (шаг 10 ms)
    uint16_t airspeed = 0;                  // <27-28> Воздушная скорость (шаг 0.5 м / сек)
    uint16_t satellite_earth_velocity = 0;  // <29-30> (шаг 0.5 м / сек)
    uint8_t checksum = 0;                   // <31> контрольная сумма по младшим 8 битам
};  // END struct to_goen_telemetry

struct from_goen_telemetry
{
    uint8_t sync_code1 = 0xEE;             // <0> Hexadecimal. синхронизирующий код, закреплен
    uint8_t sync_code2 = 0x16;             // <1> Hexadecimal. синхронизирующий код, закреплен
    uint8_t goen_state1 = 0;               // <2> Набор из 8 бит. описан в GoenState1
    uint8_t goen_state2 = 0;               // <3> Набор из 8 бит. описан в GoenState2
    uint8_t zoom_ratio = 0;                // <4> low 8 bits. U16 with bit0-3 of byte <5>
    uint8_t goen_state3 = 0;               // <5> с 4 по 7 биты: 00 - Visible 1 (zoom/front view)
                                               //  01 - Visible 2 (wide angle/side view), 10 - Infrared 1, 11 - Infrared 2
                                               // биты с 0 по 3  Zoom ratio high 4 bits
    int16_t target_miss_x = 0;             // <6-7> УГОЛ отклонения цели от центра по горизонтали. (шаг 0.05 град)
    int16_t target_miss_y = 0;             // <8-9> УГОЛ отклонения цели от центра по вертикали. (шаг 0.05 град)
    int16_t roll_frame_angle = 0;          // <10-11> угол крена рамки (шаг 0.05 град)
    int16_t pitch_frame_angle = 0;         // <12-13> угол тангажа рамки (шаг 0.05 град)
    int16_t azimuth_frame_angle = 0;       // <14-15> угол азимута рамки (шаг 0.05 град)
                                           // <16-19> reserve
    int16_t roll_angular_velocity = 0;     // <20-21> угловая скорость по крену (налево отрицательно, направо положительно 0.01 град / сек)
    int16_t pitch_angular_velocity = 0;    // <22-23> угловая скорость по тангажу (вниз отрицательно, вверх положительно 0.01 град / сек)
    int16_t azimuth_angular_velocity = 0;  // <24-25> угловая скорость по азимуту (против часовой отрицательно, по часовой положительно 0.01 град / сек)
    uint16_t laser_ranging = 0;            // <26-27> лазерный дальнеомер. 0, если неактивен (шаг 0.1 м)
    uint8_t self_test_result = 0;          // <28> Набор из 8 бит 0 и 1. Описан в struct SelfTestResult
                                           // <29-30>  reserve
    uint8_t checksum = 0;                  // <31> контрольная сумма по младшим 8 битам
};  // END struct from_goen_telemetry


struct FromGoenSingleStatusReturn
{
    uint8_t sync_word_1 = 0xEE;                     // <0> синхронизирующие байты
    uint8_t sync_wod_2 = 0x19;                      // <1>
    uint8_t corresponding_control_word = 0x00;      // <2> код принятой к исполнению комманды
    uint8_t parametr_length = 0x00;                 // <3> не равен нулю только
                                                    // <4 - N+3> здесь резервируется N = parametr_length байт под дополнительные сведения
    uint8_t checksum = 0x00;                        // <N + 4>
    /// Если коды комманд   |   0xB0 - lift control     | то N != 0
    ///                     |   0x3A - digital indexing |
};  // END struct FromGoenSingleStatusReturn

struct GoenState1
{

};  // END struct GoenState1

#pragma pack(pop)
