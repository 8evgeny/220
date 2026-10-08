#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

#pragma pack(push, 1)

/// периодическая телеметрия от шара
struct ToBortTelemetry
{
    uint8_t sync_code1 = 0xEE;              // <0> Hexadecimal. синхронизирующий код, закреплен
    uint8_t sync_code2 = 0x16;              // <1> Hexadecimal. синхронизирующий код, закреплен
    uint8_t goen_state1 = 0;                // <2> Набор из 8 бит. описан в GoenState1
    uint8_t goen_state2 = 0;                // <3> Набор из 8 бит. описан в GoenState2
    uint8_t zoom_ratio = 0;                 // <4> low 8 bits. U16 with bit0-3 of byte <5>
    uint8_t goen_state3 = 0;                // <5> с 4 по 7 биты: 00 - Visible 1 (zoom/front view)
    //  01 - Visible 2 (wide angle/side view), 10 - Infrared 1, 11 - Infrared 2
    // биты с 0 по 3  Zoom ratio high 4 bits
    int16_t target_miss_x = 0;              // <6-7> УГОЛ отклонения цели от центра по горизонтали. (шаг 0.05 град)
    int16_t target_miss_y = 0;              // <8-9> УГОЛ отклонения цели от центра по вертикали. (шаг 0.05 град)
    int16_t roll_frame_angle = 0;           // <10-11>  res угол крена рамки (шаг 0.01 град)
    int16_t pitch_frame_angle = 0;          // <12-13> угол тангажа рамки (шаг 0.01 град)
    int16_t azimuth_frame_angle = 0;        // <14-15> угол азимута рамки (шаг 0.01 град)
    int16_t abs_roll = 0;                  // <16-19> reserve abs roll
    int16_t abs_pitch = 0;                 // <18-19>  reserve/ abs pitch
    int16_t roll_angular_velocity = 0;      // <20-21> угловая скорость по крену (налево отрицательно, направо положительно 0.01 град / сек)
    int16_t pitch_angular_velocity = 0;     // <22-23> угловая скорость по тангажу (вниз отрицательно, вверх положительно 0.01 град / сек)
    int16_t azimuth_angular_velocity = 0;   // <24-25> угловая скорость по азимуту (против часовой отрицательно, по часовой положительно 0.01 град / сек)
    uint16_t laser_ranging = 0;             // <26-27> лазерный дальнеомер. 0, если неактивен (шаг 0.1 м)
    uint8_t self_test_result = 0;           // <28> Набор из 8 бит 0 и 1. Описан в struct SelfTestResult
    int16_t abs_azimutch = 0;              // <29-30>  reserve abs azimuth
    uint8_t checksum = 0;                   // <31> контрольная сумма по младшим 8 битам
};  // END struct from_goen_telemetry

// byte <2> структуры From_Goen
struct StatusInformationFeedback1
{
    bool tracked_video_source1 = 0;         // <7> bit 00 - TV1, 01 - TV2, 10 - IR1, 11 - IR2
    bool tracked_video_source2 = 0;         // <6> bit
    bool track_algorith_type1 = 0;          // <5> bit 00 always
    bool track_algorith_type2 = 0;          // <4> bit
    bool target_auto_prompt = 0;            // <3> bit 1- on, 0 - off (default)
    bool target_tracking_status = 0;        // <2> bit; 0- search (default), 1 - lock
    bool standby1 = 0;                      // <1> bit - standby
    bool standby2 = 0;                      // <0> bit - standby

    std::vector<bool*> vec
    {
        &tracked_video_source1,
        &tracked_video_source2,
        &track_algorith_type1,
        &track_algorith_type2,
        &target_auto_prompt,
        &target_tracking_status,
        &standby1,
        &standby2
    };//std::vector<bool*> vec
};  // END struct GoenState1

/// бит <3> структуры From_Goen
struct StatusInformationFeedback2
{
    bool image_enchancement = 0;             // <7> bit; 0 - Off(default), 1 - On
    bool reserved1 = 0;                     // <6> bit - Reserved
    bool storage = 0;                       // <5> bit;  0 - off, 1 - On
    bool reserved2 = 0;                     // <4> bit - Reserved
    bool motor_status = 0;                  // <3> bit; 0 - off, 1 - On
    bool follow_mode = 0;                   // <2> bit; 0 - off, 1 - On
    bool electric_lock_mode = 0;            // <1> bit; 0 - off, 1 - On
    bool laser_status = 0;                  // <0> bit; 0 - off, 1 - On

    std::vector<bool*> vec {
        &image_enchancement,
                &reserved1,
                &storage,
                &reserved2,
                &motor_status,
                &follow_mode,
                &electric_lock_mode,
                &laser_status
    };
};  // END struct GoenState1

///
struct StatusInformationFeedback3
{
    bool large_screen_displayed0 = 0;       // <7> 00 - TV1, 01 - TV2, 10 - IR1, 11 - OR2
    bool large_screen_displayed1 = 0;       // <6>
    bool small_screen_displayed1 = 0;       // <5> 00 - TV1, 01 - TV2, 10 - IR1, 11 - OR2
    bool small_screen_displayed0 = 0;       // <4>
    bool zoom_ratio11 = 0;                  // <3> high bits of zoom ratio
    bool zoom_ratio10 = 0;                  // <2>
    bool zoom_ratio9 = 0;                   // <1>
    bool zoom_ratio8 = 0;                   // <0>

    std::vector<bool*> vec {
        &large_screen_displayed0,
                &large_screen_displayed1,
                &small_screen_displayed1,
                &small_screen_displayed0,
                &zoom_ratio11,
                &zoom_ratio10,
                &zoom_ratio9,
                &zoom_ratio8
    };
}; // END struct SelfTestResult

struct SelfInspectionResult
{
    bool self_inspection_completed = 1;         // <7> 1 - complete, 0 - in progress
    bool res3 = 0;                              // <6> reserve bits
    bool res2 = 0;                              // <5>
    bool res1 = 0;                              // <4>
    bool res0 = 0;                              // <3>
    bool gyroscope_calibration = 0;             // <2> 1 - failed, 0 - successful
    bool encoder_and_servo_drive = 0;           // <1> 1 - error, 0 - normal
    bool imaging_plate = 0;                     // <0> 1 - error, 0 - normal

    std::vector<bool*> vec {
        &self_inspection_completed,
                &res3,
                &res2,
                &res1,
                &res0,
                &gyroscope_calibration,
                &encoder_and_servo_drive,
                &imaging_plate
    };
}; // END struct SelfInspectionResult

struct TargetInformation  // от гоен в дятел
{
    uint8_t sync_code1 = 0xEE;              // <0> Hexadecimal. синхронизирующий код, закреплен
    uint8_t sync_code2 = 0x18;              // <1> Hexadecimal. синхронизирующий код, закреплен
    int32_t latitude = 0;                   // <2-5> широта
    int32_t longitude = 0;                  // <6-9> долгота
    int16_t altitude = 0;                   // <10-11> высота
    int16_t relative_altitude = 0;          // <12-13> относительная высота
    uint8_t year = 0;                       // <14> год (+2000)
    uint8_t month = 0;                      // <15> месяц
    uint8_t day = 0;                        // <16> день
    uint8_t hour = 0;                       // <17> час
    uint8_t minute = 0;                     // <18> минуты
    uint8_t second = 0;                     // <19> секунды
    uint8_t centisecond = 0;                // <20> сотая доля секунды (шаг 10 ms)
    // <21-30> Reserved
    uint8_t checksum = 0;                   // <31> контрольная сумма по младшим 8 битам
};  // END struct target_information


struct ReplayToCmdToBort     // от ГОЕН в дятел
{
    uint8_t sync_code1 = 0xEE;              // <0> Hexadecimal. синхронизирующий код, закреплен
    uint8_t sync_code2 = 0x19;              // <1> Hexadecimal. синхронизирующий код, закреплен
    uint8_t control_param = 0x00;           // <2> Hexadecimal. Основной код команды. В зависимости от его значения задействуются различные функции
    uint8_t length = 0;                     // <3> если control_param == 0x3a, то length = 2, если 0xb0, то length=1, иначе 0 (ничего не передается)
    // uint8_t param1 = 0;                     // <4> если control_parmr == 0x31 : Hexadecimal. digital_indexing_command
    //        0x00 Exit geo-tracking, 0x01 geo_tracking the current center of the field of view
    //        0x02 Geographic tracking of a specified location, 0x0a calibrate against known targets
    //     если control_param==0xb0 : jacking_condition
    //        0 - stop, 1 rise, 2 fall, 3 rise in place, 4 fall in place, 0xff - error
    // uint8_t status = 0;                     // <5> учитывается, если control_param == 0x3a. 0 - success, 1 - failure
    // <6-7> empty
    uint8_t checksum = 0;                   // <8> контрольная сумма по младшим 8 битам
};  // END struct single_status_return

struct Tlm4AVAX
{
    float valid = 0;                        // byte 0-3
    float target_miss_x = 0;                // byte 4-7
    float target_miss_y = 0;                // byte 8-11
    float motor_azimuth = 0;                // byte  12-15
    float motor_pitch = 0;                  // 16-19
    float frame_roll = 0;                   // 20-23
    float frame_pitch = 0;                  // 24-27
    float azimuth_angular_velocity = 0;     // 28-31
    float pitch_angular_velocity = 0;       // 32-35
    float attack = 0;                       // 36-39
}; // END Tlm4AVAX

#pragma pack(pop)
