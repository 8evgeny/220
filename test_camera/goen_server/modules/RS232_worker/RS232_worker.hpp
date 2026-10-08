#pragma once

#include <libserial/SerialPort.h>
#include <iostream>
#include <memory>
#include <thread>
#include "to_goen_struct.hpp"
#include "common_data.hpp"

enum class COMMAND_RS232 : uint8_t
{
    notCMD =                0x00,
    TV =                    0x01,
    INFRA =                 0x03,
    IMAGE_ENCHANCE_ON =     0x05,
    IMAGE_ENCHANCE_OFF =    0x06,
    STORAGE_ON =            0x09,
    STORAGE_OFF =           0x0A,
    TRACKING_STOP =         0x0E,
    TRACKING_START =        0x0D,
    IR_WHITE_HEAT =         0x11,
    IR_BLACK_HEAT =         0x12,
    PTZ =                   0x24,
    ZOOM =                  0x25,
    SET_AZIMUT_PITCH =      0x26,
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
    SPECIFI_ATTITUDE_ANGLE =0x3B,
    CALIB_ZERO_POS_FC_ATT = 0x3C,
    SET_ZERO_POSITION =     0xB1,

    /// OUR COMMANDS
    TRAC_SIZE_CHANGE =      0xC0,
    POWER_OFF_1 =           0xC1,
    POWER_OFF_2 =           0xC2,
    POWER_OFF_3 =           0xC3,
    CALIBRATE_DRIFT =       0xC4,
    CALIBRATE_I2C_ZERO =    0xC5,
    CAMERA_CONTROL =        0xC6,   /// X - тип команды, Y - аргумент
    TLM_SENDER_MODE_SWITCH =0xC7,   /// параметр Х определяет раздел, параметр Y определяет текущий мод
    START_ATTACK =          0xC8,   /// меняет статус атаки на 1, если он 0 и захвачен трекер
    STOP_ATTACK =           0xC9,   /// меняет статус атаки на 0, если он 1 и захвачен трекер

    STOP =                  0xF0,
    LEFT =                  0xF1,
    RIGHT =                 0xF2,
    UP =                    0xF3,
    DOWN =                  0xF4,
    ZOOM_MINUS =            0xF5,
    ZOOM_PLUS =             0xF6,
    SEND_READ_REQUEST =     0xEE,
}; // END enum class COMMAND_RS232 : uint8_t

enum class TYPE_DATA : uint8_t
{
    CMD = 0,
    TELEMETRY = 1,
};


class RS232_worker
{
public:
    RS232_worker(int speed, const  std::string & port);
    ~RS232_worker();

    void setBautrade(int speed);
    uint8_t check_sum(const uint8_t *buf, const size_t buf_size);
    bool check_check_sum();
    int find_cmd();
    bool isCMD();
    bool isTelemetry();
    void clearCMD_buff();
    bool find_cmd_STOP();
    bool find_cmd_LEFT();
    bool find_cmd_RIGHT();
    bool find_cmd_UP();
    bool find_cmd_DOWN();
    bool find_cmd_INFRA();
    bool find_cmd_TV();
    bool find_cmd_ZOOM_MINUS();
    bool find_cmd_ZOOM_PLUS();
    bool find_cmd_TO_CENTRE();
    bool find_cmd_TRACKING_START();
    bool find_cmd_TRACKING_STOP();
    bool find_cmd_AZIMUTH_FOLLOW();
    bool find_cmd_CLOSE_FOLLOW();
    bool find_cmd_ELECTRIC_LOCK_ON();
    bool find_cmd_ELECTRIC_LOCK_OFF();
    bool find_cmd_MOTOR_ON();
    bool find_cmd_MOTOR_OFF();
    bool find_cmd_SET_ZERO_POSITION();
    bool find_cmd_SET_FRAME_ANGLE();
    bool find_cmd_POWER_OFF();
    bool find_cmd_CALIBRATE_DRIFT();
    bool find_cmd_CALIBRATE_I2C_ZERO();
    bool find_cmd_CAMERA_CONTROL();
    bool find_cmd_SPECIFI_ATTITUDE_ANGLE();
    bool find_cmd_CALIB_ZERO_POS_FC_ATT();
    bool find_cmd_TLM_SENDER_MODE_SWITCH();
    bool find_cmd_START_ATTACK();
    bool find_cmd_STOP_ATTACK();
    void printDataRS232(TYPE_DATA type); // type = 0 - command; 1 - telemetry
    uint8_t getCMD() const;
    void setCMD(uint8_t newCMD);
    int16_t calibrate_drift_parameter_X = 0;
    int16_t calibrate_drift_parameter_Y = 0;
    int16_t calibrate_i2c_zero_parameter_roll = 0;
    int16_t calibrate_i2c_zero_parameter_pitch = 0;
    int16_t calibrate_i2c_zero_parameter_yaw = 0;
    int16_t calibrate_i2c_zero_parameter_auto = -30000;
    int16_t max_calibrate_value = 200;
    static constexpr uint8_t len_CMD_from_Board = 16; //from Board
    uint8_t cmdBuf[len_CMD_from_Board];

    static constexpr uint8_t _telemetryLenToBoard = 32;
    static constexpr uint8_t _telemetryLenFromBoard = 32;
    uint8_t telemetryBuf[_telemetryLenToBoard];

    std::unique_ptr<LibSerial::SerialPort> serial_ptr = nullptr;
    bool port_open_OK = false;
    std::string data_from_rs232;
    bool need_send_to_RS485 = false;
    bool is_workerRS232_start = false;
    union
    {
        uint8_t buf8[2];
        int16_t buf16;
    } parametr_X;

    union
    {
        uint8_t buf8[2];
        int16_t buf16;
    } parametr_Y;

private:
    uint8_t _CMD_FROM_RS232 = 0;
    to_goen_command to_goen_cmd;
    from_goen_telemetry from_goen_tlm;
    std::shared_ptr<common_data> common_data_ptr = nullptr;
    int zoom_min_value = 0x70;
};// END class RS232_worker



