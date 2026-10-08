#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

#pragma pack(push, 1)

struct MotorStatus              // size = 11
{
    uint8_t mode = 0;               // <0>
    uint8_t errors = 0;             // <1>MotorErrors
    uint8_t flags = 0;              // <2>
    float angle = 0;                // <3-6>
    float speed = 0;                // <7-10>
};// END struct MotorStatus

struct FromGoenTelemetry            // size = 47
{
    uint8_t mode = 0x00;                    // <0>  0xC0 - dummy(команды записи игнорируются)б 0xС1 - stbilization, 0xC2 rotaryPlatform, 0xC3 - parkin, 0xC4 off
    /// Mems board status
    uint8_t mode_mems = 0x00;               // <1>
    uint8_t mems_errors = 0x00; 			// <2>      битовые поля
    uint32_t mems_flag = 0x00;               // <3-6>	битовые поля
    uint8_t axis_switch_mode = 0x00;		// <7>
    uint8_t mems_process_mode = 0x00;		// <8>
    float speed_yaw = 0.f;              // <9-12>
    float speed_pitch = 0.f;			// <13-16>
    float angle_yaw = 0.f;              // <17-20>
    float angle_pitch = 0.f;			// <21-24>
    MotorStatus motor_yaw;				// <25-35>
    MotorStatus motor_pitch;			// <36-46>
}; // END TelemetryUART


struct MotorErrors          // size = 1
{

    uint8_t motorOvercurrent    : 1;
    /** @brief motorOverheat - перегрев драйвера мотора, [-] */
    uint8_t motorOverheat       : 1;
    /** @brief asRotationOverSpeed - превышение разрешенной для датчика угла скорости вращения, [-] */
    uint8_t asRotationOverSpeed : 1;
    /** @brief asWeakMagneticField - неустойчивое магнитное поле датчика угла, [-] */
    uint8_t asWeakMagneticField : 1;
    /** @brief asUnderVoltage - напряжение питания датчика угла вне диапазона, [-] */
    uint8_t asUnderVoltage      : 1;};

//struct FromGoenTelemetry //tabl 5
//{
//    uint8_t start_byte = 0xAA;      // <0>
//    uint8_t cmd = 0x81;             // <1>
//    uint8_t status = 0x00;          // <2>
//    uint16_t src = 0x00FE;          // <3-4>
//    uint16_t dst = 0x0000;          // <5-6>
//    uint16_t word_cnt = 0x000A;     // <7-8>
//    uint32_t state = 0x0000;
//    uint32_t errors = 0x0000;
//    float angle_z = 0.f; // 0.0 - 360.0 [degree]
//    float angle_x = 0.f;
//    float speed_z = 0.f; // +- 60.00 [deg/s]
//    float speed_x = 0.f;
//    float mems_speed_x = 0.f; // gyrosscope speed [deg/s]
//    float mems_speed_y = 0.f;
//    float mems_speed_z = 0.f;
//    uint32_t reserve = 0x00;
//    uint8_t check_sum = 0x00;
//}; // END GyroPlatformState

//struct FromGoenState //tabl 6
//{
//    bool ready = 0;             // <0> 0 - изделие не готово к работе, 1 - изделие готово к работе;
//    bool stabilization = 0;     // <1> 0 - стабилизация выключена, 1 - стабализация включена;
//    bool motors = 0;            // <2> 0 - моторы выключены, 1 - моторы включены;
//    bool self_testing = 0;      // <3> 0 - , 1 - запущено самотестирование;
//    bool calibration = 0;       // <4> 0 - , 1 - запущена калибровка;
//    bool not_saved = 0;         // <5> 0 - , 1 - есть несохранённые данные;

//    std::vector<bool*> vec {&ready,
//                &stabilization,
//                &motors,
//                &self_testing,
//                &calibration,
//                &not_saved};

//}; // END GyroPlatformState_state

//struct FromGoenErrors //tabl 7
//{
//    bool msg_checking_TX_RS422 = 0; // <0> наличие сбоев обмена по линии  по линии TX RS422: 0 - нет сбоев, 1 - сбой;
//    bool msg_checking_RX_RS422 = 0; // <1> наличие сбоев обмена по линии  по линии RX RS422: 0 - нет сбоев, 1 - сбой;
//    bool msg_checking_CAN = 0;      // <2> наличие сбоев обмена по линии  по линии CAN: 0 - нет сбоев, 1 - сбой;
//    /// наличие сбоев датчика
//    bool angle_sensor_z = 0;        // <3> 0 - нет сбоев, 1 - сбой;
//    bool angle_sensor_x = 0;        // <4> 0 - нет сбоев, 1 - сбой;
//    bool MEMS = 0;                  // <5> 0 - нет сбоев, 1 - сбой;
//    bool motor_z = 0;               // <6> 0 - нет сбоев, 1 - сбой;
//    bool motor_x = 0;               // <7> 0 - нет сбоев, 1 - сбой;
//    /// ошибка инициализации датчика
//    bool init_MEMS = 0;             // <8> 0 - нет сбоев, 1 - сбой;
//    bool init_angle_sensor_z = 0;   // <9> 0 - нет сбоев, 1 - сбой;
//    bool init_angle_sensor_x = 0;   // <10> 0 - нет сбоев, 1 - сбой;
//    bool init_motor_z = 0;          // <11> 0 - нет сбоев, 1 - сбой;
//    bool init_motor_x = 0;          // <12> 0 - нет сбоев, 1 - сбой;

//    std::vector<bool*> vec {&msg_checking_TX_RS422,
//                &msg_checking_RX_RS422,
//                &msg_checking_CAN,
//                &angle_sensor_z,
//                &angle_sensor_x,
//                &MEMS,
//                &motor_z,
//                &motor_x,
//                &init_MEMS,
//                &init_angle_sensor_z,
//                &init_angle_sensor_x,
//                &init_motor_z,
//                &init_motor_x
//                           };

//}; // END GyroPlatformState_errors



#pragma pack(pop)
