#ifndef TELEMETRY_H
#define TELEMETRY_H

//#include "cmd/cmd.h"
#include "owlet_cmd_v2.h"

class Telemetry : /*public Cmd,*/ public Cmd_v2
{
public:
    typedef enum {
        eth_mux_tv_bayer = 		0x0,
        eth_mux_lwir = 			0x1,
        eth_mux_tv_plus_lw = 	0x2,
        eth_mux_tv_grayscale = 	0x3,
        eth_mux_fusion = 		0x4,
        eth_mux_test_pattern = 	0x7
    } t_ethernet_stream_multiplexer_channels;

//BUG: __attribute__ ((__packed__)) в MinGW не работает, используем #pragma pack()
#pragma pack(push, 1)
typedef struct _t_video_channel_struct
{
      uint32_t debug_data;
      uint16_t sensor_status; //0-1
      uint16_t fps;			 //2-3
      uint16_t vres;			 //4-5
      uint16_t hres;			 //6-7
      float focus_mm;		 //8-11
      float field_grad;		 //12-15
      int16_t contrast;
      int16_t brightness;
      int16_t sharpness;
      int16_t inversion;
      uint16_t temperature;
      int16_t algorithm;
      uint16_t left_table_code;
      uint16_t right_table_code;
      int16_t uart_byte_cnt;
      int16_t uart_wr_pos;
} t_video_channel_struct; // 16 байт
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _t_gsp_struct
{
    uint32_t status; //0-3
    uint32_t mode;
    uint32_t abs_angle_z;//4-7
    uint32_t abs_angle_x;//8-11
    float   angle_z_deg;//12-15
    float   angle_x_deg;//16-19
    int32_t  speed_z;//20-23
    int32_t  speed_x;//24-27
    float   speed_z_deg_per_sec;//28-31
    float   speed_x_deg_per_sec;//32-35
    uint32_t zero_abs_angle_z;
    uint32_t zero_abs_angle_x;
    uint32_t telemetry_msg_cnt;
    int16_t uart_byte_cnt;
    int16_t uart_wr_pos;
} t_gsp_struct; // 36 байт
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _t_flash_struct
{
    uint8_t year; //Год изготовлени изделия
    uint8_t month;//Месяц изготовления
    uint8_t id;//Порядковый номер
    uint8_t hard_ver;//Версия аппаратуры
    uint8_t soft_ver;//Версия ПО
    uint16_t hours;//1 year = 12 months
    uint8_t minutes;//1 month = 30 days
} t_flash_struct; // 8 байт
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _t_telemetry_struct
{
  t_video_channel_struct tv; //0 - 15 // 16 байт
  t_video_channel_struct lw; //16 - 31 // 16 байт
  t_gsp_struct gsp; // 32 - 67 // 36 байт
  t_flash_struct controller;//68 - 75 // 8 байт
} t_telemetry_struct; //76 байт
#pragma pack(pop)

    explicit Telemetry():Cmd_v2(){}

    bool from_buffer(const uint8_t * buffer, const uint32_t & buffer_len) override;

    t_telemetry_struct * get();

protected:
    t_telemetry_struct telemetry;
};

#endif // TELEMETRY_H
