#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

#pragma pack(push, 1)
struct ToGoenCommand
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
    uint8_t empty_param1 = 0;               // <9-14> - резерв
    uint8_t empty_param2 = 0;
    uint8_t empty_param3 = 0;
    uint8_t empty_param4 = 0;
    uint8_t empty_param5 = 0;
    uint8_t empty_param6 = 0;
    uint8_t checksum = 0x00;                // <15> контрольная сумма по младшим 8 битам
}; // END struct to_goen_command

struct To_Goen_Telemetry
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

#pragma pack(pop)
