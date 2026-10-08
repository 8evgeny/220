#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <cstdint>

/// FROM PROTOCOL::
enum class CMD_RS232 : uint8_t
{
    TV =                    0x01,
    INFRA =                 0x03,
    IMAGE_ENCHANCE_ON =     0x05,
    IMAGE_ENCHANCE_OFF =    0x06,
    STORAGE_ON =            0x09,
    STORAGE_OFF =           0x0A,
    TRACKING_START =        0x0D,
    TRACKING_STOP =         0x0E,
    IR_WHITE_HEAT =         0x11,
    IR_BLACK_HEAT =         0x12,
    PTZ =                   0x24,
    ZOOM =                  0x25,
    SET_AZIMUTH_PITCH =     0x26,
    MOTOR_ON =              0x27,
    MOTOR_OFF =             0x28,
    CLOSE_FOLLOW =          0x29,
    AZIMUTH_FOLLOW =        0x2A,
    TO_ZERO_POSITION =      0x2B,
    SUPPRESS_GYRO_DRIFT =   0x2C,
    LASER_RANGIGNG_ON =     0x2D,
    LASER_RANGING_OFF =     0x2E,
    ELECTRIC_LOCK_ON =      0x30,
    ELECTRIC_LOCK_OFF =     0x31,
    SPECIFY_ATTITUDE_ANGLE =0x3B,
    CALIB_ZERO_POS_FC_ATT = 0x3C,
    SET_ZERO_POSITION =     0xB1,

    /// OUR COMMANDS

    TRAC_SIZE_CHANGE =      0xC0,
    POWER_OFF_1 =           0xC1,
    POWER_OFF_2 =           0xC2,
    POWER_OFF_3 =           0xC3,
    CALIBRATE_DRIFT =       0xC4,
    CALIBRATE_I2C_ZERO =    0xC5,
    CAMERA_CONTROL =        0xC6,   /// параметр X определяет тип команды, параметр Y аргумент. XY = 0,0 - ручной щелчок затвором, XY = 0,1 - калибровка фона; XY = 1,[0..9] - режимы улучшения изображения ::TODO
    TLM_SENDER_MODE_SWITCH =0xC7,   /// параметр X определяет раздел, параметр Y определяет текущий мод
    START_ATTACK =          0xC8,
    STOP_ATTACK =           0xC9,

    STOP =                  0xF0,
    LEFT =                  0xF1,
    RIGHT =                 0xF2,
    UP =                    0xF3,
    DOWN =                  0xF4,
    ZOOM_MINUS =            0xF5,
    ZOOM_PLUS =             0xF6,
    SEND_READ_REQUEST =     0xEE,
}; // END enum class CMD_RS232 : uint8_t

#endif // COMMANDS_HPP
