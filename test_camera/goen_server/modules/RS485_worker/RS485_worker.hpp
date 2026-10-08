#pragma once

#include <RS232_worker.hpp>
#include <libserial/SerialPort.h>
#include <opencv2/core/core.hpp>
#include "common_data.hpp"
#include "rs485_struct.hpp"
#include <INIReader.h>
#include <iostream>
#include <fstream>

enum class DEVICES : uint8_t
{
    GOEN =  0x01,
 };


enum class REQUEST_TYPE : uint8_t
{
    read =  0x01,
    write = 0x00
};// END enum class REQUEST_TYPE

enum class STABILIZATION_TYPE : uint8_t
{
    ABS_STAB = 0x01,
    ANGLE_STAB = 0x02
}; // END enum class STABILIZATION_TYPE : uint8_t

enum class MODE_TYPE : uint8_t
{
    STABILISATION =     0xC1,
    ROTARY_PLATFORM =   0xC2,
    PARKING =           0xC3,
    MOTOR_OFF =         0xC4,
};// END enum class MODE_TYPE

enum class COMMAND_TO_RS485 : uint8_t
{
    GET_VERSION =       0x00, //read
    MODE =              0x01, //write
    CONTROL_POSITION =  0x02, //write
    CONTROL_SPEED =     0x03, //write
    CONTROL_TRACKING =  0x04, //write
    GET_STATUS =        0x05, //read
    STAB_MODE =         0x06, //write - переключение режимов стабилизации
    notCMD =            0xFF,

    // CPU_RESET =         0x00F1,    //generate in func work_rs232_rs485()
    // Motor_Control =     0x0001,
    // Stab_Control =      0x0002,    //No realised
    // CALIBRATIONS =      0x0020,
    // StabSpeed_Z =       0x0010,
    // StabSpeed_X =       0x0011,
    // Stab_A_Angle_Z =    0x0012,
    // Stab_A_Angle_X =    0x0013,
    // Stab_R_Angle_Z =    0x0014,
    // Stab_R_Angle_X =    0x0015,
    // SetAngleOffset_Z =  0x0042,
    // SetAngleOffset_X =  0x0043,
    // Save =              0x0056,
};//END enum class COMMAND_TO_RS485

enum class SAVE : uint32_t
{
    All =     0xAAAAFFFF,
    Default = 0x00000000
};//END enum class COMMAND_TO_RS485

enum class Motor : uint8_t
{
    ON = 0x01,
    OFF = 0x00
};//END enum class Motor

enum class CameraType : uint8_t
{
    NOt_CAMERA = 0x00,
    TV = 0x01,
    TPV = 0x02
};//END enum class CameraType

enum class Stab : uint8_t
{
    ON = 0x01,
    OFF = 0x00
};//END enum class Stab

class RS485_worker
{
public:
    uint8_t modeStab = (uint8_t)Stab::ON;
    uint32_t ax_max = 8; //Tracking
    uint32_t ay_max = 8;
    RS485_worker(int speed, const std::string & port);
    RS485_worker(int speed);//Only for 2 method
    ~RS485_worker();

    float obj_xy_x = 0; // Безразмерная x-координата центра цели на текущем кадре (в единицах ширины фрейма).
    float obj_xy_y = 0; // Безразмерная y-координата центра цели на текущем кадре (в единицах высоты фрейма).
    char validate = 0; // Степень валидации захвата на текущем кадре.
    char validate_min = 15; // Минимальное значение валидации непрерывного повторения нахождения рамки, после которого поиск считается успешным.
    char ok_match = 0; // Признак успешного захвата на текущем кадре.
    int zahvat = 0; // Признак успешного захвата на текущем кадре.
    bool is_Tracking_escape = false;

    float Vmin = 200.f;  // min Speed
    float Vmax = 500.f; // max Speed
    float Vmin_x = 0.f, Vmax_x = 0.f, Vmin_y = 0.f, Vmax_y = 0.f;
    int scale05 = 0;
    void setBautrade(int speed);
    void init_crc_calculation();
    uint8_t crc_calc(uint8_t *data, uint8_t size);
    void sendCMD_rs485(uint8_t cmd, int16_t X, int16_t Y);
    void clearCommandPacket();
    bool readFromRS485(std::string & data_from_rs485);
    void writeToRS485();
    void get_status_Goen();
    void get_version_Goen();
    void set_mode_Goen(uint8_t newMode);
    void set_mode_Stab(uint8_t newMode); // newMode=1 - абсолютная стабилизация, newMode=2 - стабилизация по углу
    void control_speed_Goen(int16_t X, int16_t Y);
    void control_position_Goen(int16_t X, int16_t Y);
    void control_tracking_Goen(float X, float Y, float ax_max, float ay_max);
    void control_tracking_Goen_self(float X, float Y);
    bool data_ready_for_read_in_port(int & numberRead);
    void stop();
    void left_right(int16_t, int16_t);
    void up_down(int16_t, int16_t);
    void check_need_correct();
    void to_zero();
    void set_azimut_pitch(int16_t X, int16_t Y);
    void tracking(int16_t X, int16_t Y);
    void stab_control_ON();
    void stab_control_OFF();
    void save_all();
    void save_to_default();
    void printDataRS485();
    void printDataRS485(uint8_t * buf, uint8_t num_byte);
    void clearReadBuf();
    void send_read_request();
    void tracking_start();
    void tracking_stop();
    void calculate_Vmax_for_tracking_automat(float startAzimut, float startPitch, int16_t Vmin, int16_t Vmax, int16_t Amax,
                                             int16_t &Vmax_X_for_first_half_azimut, int16_t &Vmax_Y_for_first_half_pitch);
    // void convert_coordinates_to_radian(cv::Rect2f workRect, float &azimut, float &pitch);
    void copy_telemetry_to_stuct();

    std::unique_ptr<LibSerial::SerialPort> serial_ptr = nullptr;
    std::string namePort = "";
    bool port_open_OK = false;

    uint8_t getCMD() const;
    void setCMD_TO_485(uint8_t newCMD_TO_485);

    static constexpr uint8_t goen_telemetry_len = 47;    //Protocol for Gyroplatphorm
    uint8_t good_answer_from_rs485[2] = {0xAA, 0x81};   //answer for first request

    uint8_t dataFromGoenBuf[100];
    uint8_t dataToGoenBuf[100];
    uint8_t telemetryBuf[goen_telemetry_len];
    FromGoenTelemetry from_goen_telemetry_str;
    std::mutex mut_telemetry_str;

    uint8_t num_send = 0;
    std::string data_485;
    char char_from_485;
    bool begin_read_485 = false;


    float Vx_dreyf = 0.f; // скорость дрейфа ГОЭН
    float Vy_dreyf = 0.f;
    float ang0_x = 0.f; // значение "нулевого угла" - возвращаемся в него командой  go_to_zero
    float ang0_y = 0.f;
    float pos_azimuth_err = 0; // ошибка управления по углу
    float pos_pitch_err = 0;
    // cv::Rect2f workRect = cv::Rect2f(0,0,0,0); // текущая рамка цели
    bool is_tracking_start = false;
    bool is_correcting_pozition_in_mode_azimut_follow  = false;
    bool need_return_to_correct_position = false;
    float min_value_for_CMD_TO_ZERO = 0.25;
    bool moving_to_zero_position = false;
    bool is_tracking_init_after_start = false;
    bool is_workerRS485_start = false;
    bool is_thread_send_cmd_request_status = false;

    ///  Переключение режимов стабилизации (по курсу или абсолютная)
    // Azimuth Follow Mode ::AFM
    std::atomic<bool> f_azimuth_follow = {true}; // флаг работы режима сопровождения
    std::atomic<bool> f_motor_lock = {false}; // флаг блокировки двигателя (режим поворотной платформы)
    std::atomic<bool> f_motor_on = {true}; // флаг включения двигателя
    uint8_t last_mode = 0x00;
    //    std::atomic<bool> f_speed_cmd = {false};
    //    std::atomic<bool> f_
    bool f_cmd_speed_control = false;
    // флаг поднимается в режиме сопровождения азимута при подаче комманды управления по скорости.
    // Опускается одновременно с коммандой установки текущего угла после команды стоп по RS232(управление по скорости x=0,y=0) или при подаче другой внешней команды управления
    bool f_cmd_angle_control = false; //
    // флаг поднимается в режиме абсолютной стабилизации при подаче комманды управления по углу.
    // Опускается одновременно с коммандой стоп(управление по скорости x=0,y=0) по достижении угла отличного от заданного на 0,1 градус или при подаче другой внешней команды управления
    float motor_yaw_target_angle = 0.f;
    float motor_pitch_target_angle = 0.f;


private:
    struct
    {
        uint8_t startByte = 0xAA;
        uint8_t CMD = 0x00;
        uint8_t STATUS = 0x00;
        union
        {
            struct
            {
                uint8_t function;
                uint8_t device;
            };
            uint16_t src;
        }SRC;
        union
        {
            struct
            {
                uint8_t function;
                uint8_t device;
            };
            uint16_t dst;
        }DST;
        union
        {
            uint16_t byteCount16;
            uint8_t byteCount8[2];
        }BYTE_COUNT;
        union
        {
            uint8_t data_1byte;
            uint8_t data_8byte[8];
            uint8_t data_16byte[16];
        }DATA;
        union
        {
            uint8_t data_4byte[4];
            uint16_t X;
            float x;
        }DATA_X;
        union
        {
            uint8_t data_4byte[4];
            uint16_t Y;
            float y;
        }DATA_Y;
        union
        {
            uint32_t ax_32 = 0;
            uint8_t ax_8[4];
        }AX_MAX;
        union
        {
            uint32_t ay_32 = 0;
            uint8_t ay_8[4];
        }AY_MAX;
        uint32_t ay_max = 0;

        uint8_t CRC = 0x00;
    }commandPacket;

    #define CRC_TABLE_SIZE 256
    uint8_t _CRC8Table[CRC_TABLE_SIZE];
    uint32_t _poly_width = 8;
    uint8_t _CMD_TO_485 = 0;
    std::string data = "";
    std::shared_ptr<common_data> common_data_ptr = nullptr;
    float k_speed_control = 0.01;
    float k_cmd_speed_control = 10;
    uint8_t crc_polynom = 0x31;
    uint8_t crc_init_value = 0xFF;
    std::string config_path = "../config.ini";
    std::string section_name = "tracking";
    bool get_ini_params(const std::string &config, const std::string &section);
    bool FileIsExist(const std::string &filePath);
};//END class RS485_worker



