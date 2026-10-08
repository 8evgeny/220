#ifndef CORSAIR_TELEMETRY_H
#define CORSAIR_TELEMETRY_H

#include "corsair_cmd.h"

class Corsair_Telemetry : public Corsair_Cmd
{
public:
    typedef enum {
        eth_mux_tv_bayer = 		0x0,
        eth_mux_swir =			0x1,
        eth_mux_lwir = 			0x2,
        eth_mux_tv_plus_lw = 	0x3,
        eth_mux_tv_grayscale = 	0x4,
        eth_mux_fusion = 		0x5,
        eth_mux_dsp = 			0x6,
        eth_mux_test_pattern = 	0x7
    } t_ethernet_stream_multiplexer_channels;


#pragma pack(push, 1)
    typedef struct {
        uint16_t ready_reserve : 1;  // всегда 1
        uint16_t ready         : 1;
        uint16_t power_on      : 1;
        uint16_t body_ovrht    : 1;
        uint16_t zero_reserve  : 1; // всегда 0
        uint16_t radiation     : 1;
        uint16_t cooling       : 1;
        uint16_t test_mode     : 1;
    } rf_state_flags;
#pragma pack(pop)

    typedef union {
        uint16_t word;
        rf_state_flags flags;
    } t_union_rf_state; // 2

    //BUG: /*__attribute__ ((__packed__))*/ в MinGW не работает, используем #pragma pack()
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_rangefinder_struct
    {
        uint8_t status;			//0
        uint8_t meas_status;//1
        float last_distance;	//2-5
        float avg_distance;		//6-9
        float rms_distance;		//10-13
        uint16_t state;		//14-17
        uint16_t reserve;		//14-17
        float body_temp;		//18-21
    } t_rangefinder_struct;		//22 bytes
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_video_channel_struct
    {
        uint8_t sensor_status;     //0
        uint8_t lens_status;	      //1
        uint8_t fps;					//2
        uint16_t vres;				//3-4
        uint16_t hres;				//5-6
        uint16_t exp;				//7-8
        uint16_t zoom_drive;			//9-10
        uint16_t focus_drive;		//11-12
        int8_t lens_temp;			//13
        float focus_mm;			    //14-17
        float field_grad;			//18-21
    } t_video_channel_struct; //22 bytes
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_gsp_struct
    {
        uint8_t status;
        union {
            uint16_t __raw_;
            struct {
                // byte 1
                uint16_t mechanical_locking : 1;
                uint16_t electronic_locking : 1;
                uint16_t free_locking       : 1;
                uint16_t gyrostabilization  : 1;
                uint16_t zero               : 1;
                uint16_t hike               : 1;
                uint16_t pilot              : 1;
                uint16_t angle_control      : 1;
                // byte 2
                uint16_t park               : 1;
                uint16_t                    : 2;
                uint16_t can_bit            : 1;
                uint16_t adc_bit            : 1;
            };
        } mode;
        uint32_t z_drive_pos;
        uint32_t x_drive_pos;
        float z_angle;
        float x_angle;
        int32_t speed_z;
        int32_t speed_x;
    } t_gsp_struct; //88 bytes
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_target_struct
    {
        uint8_t target;
        uint16_t x_center;
        uint16_t y_center;
        uint16_t x_size;
        uint16_t y_size;
        float z_angle_center;
        float x_angle_center;
        float z_angle_size;
        float x_angle_size;
    } t_target_struct;
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct
    {
//        uint8_t tracker_state;
        uint8_t tracker_channel;
        uint8_t target_id;
        t_target_struct targets[4];
    } t_tracker_struct;
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_controller_struct
    {

        uint8_t agc_tv;
        uint8_t agc_sw;
        uint8_t agc_mw;
        uint8_t autofocus;
        uint8_t field_sync;
        uint8_t tracker_state;
        t_target_struct target;
        uint8_t fc0_status;
        uint8_t fc1_status;
        uint8_t eth_status;
        uint8_t com_status;
        uint8_t year; //Год изготовлени изделия
        uint8_t month; //Месяц изготовления
        uint8_t id; //Порядковый номер
        uint8_t hard_ver; //Версия аппаратуры
        uint8_t soft_ver; //Версия ПО
        uint16_t hours; //1 year = 12 months
        uint8_t minutes; //1 month = 30 days
        uint8_t sw_gain; //Добавил 140618 для устранения замечения по ПИВ ЦН НПУ
        union t_self_test {
            uint8_t __raw[6];
            #pragma pack(push, 1)
            struct {
                // # Byte 0
                // external interfaces
                uint8_t conn_pc_eth :1;
                uint8_t conn_server_fc :1;
                uint8_t reserve_0_2 :1;
                // internal interfaces
                uint8_t conn_gsp_rs422 :1;
                uint8_t conn_cpu_eth :1;
                uint8_t conn_mobile_seriallite :1;
                uint8_t conn_lrf_rs485 :1;
                uint8_t conn_sensor_tv_uart :1;
                // # Byte 1
                uint8_t conn_sensor_tv_camlink :1;
                uint8_t conn_lens_tv_rs232 :1;
                uint8_t conn_sensor_sw_uart :1;
                uint8_t conn_sensor_sw_camlink :1;
                uint8_t conn_lens_sw_rs232 :1;
                uint8_t conn_sensor_mw_uart :1;
                uint8_t conn_sensor_mw_camlink :1;
                uint8_t conn_lens_mw_rs232 :1;
                // # Byte 2
                // videostreams
                uint8_t video_tv_bayer :1;
                uint8_t video_tv_mono :1;
                uint8_t video_sw :1;
                uint8_t video_mw :1;
                uint8_t video_fusion :1;
                // ready
                uint8_t ready_lrf :1;
                uint8_t ready_gsp :1;
                uint8_t ready_tv_image :1;
                // # Byte 4
                uint8_t ready_sw_image :1;
                uint8_t ready_mw_image :1;
                // mechanics
                uint8_t drive_tv_zoom  :1;
                uint8_t drive_tv_focus :1;
                uint8_t drive_sw_zoom  :1;
                uint8_t drive_sw_focus :1;
                uint8_t drive_mw_zoom  :1;
                uint8_t drive_mw_focus :1;
                // # Byte 5
                uint8_t drive_gsp_azimuth :1;
                uint8_t drive_gsp_elevation :1;
                uint8_t reserve_5_2 :6;
                // # Byte 6
                uint8_t reserve_6;
            } flags;
            #pragma pack(pop)
        } self_test;
        uint8_t self_test_in_progress;
        float self_test_suc_perc;
        uint16_t self_test_count;
        uint32_t alignment_pos_x;
        uint32_t alignment_pos_z;
    } t_controller_struct; //88 bytes
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_video_stream_stat
    {
        uint16_t tv_bayer_fps;
        uint16_t tv_bayer_hsize;
        uint16_t tv_bayer_vsize;
        uint16_t sw_fps;
        uint16_t sw_hsize;
        uint16_t sw_vsize;
        uint16_t mw_fps;
        uint16_t mw_hsize;
        uint16_t mw_vsize;
        uint16_t tv_mono_fps;
        uint16_t tv_mono_hsize;
        uint16_t tv_mono_vsize;
        uint16_t complex_fps;
        uint16_t complex_hsize;
        uint16_t complex_vsize;
    } t_video_stream_stat;
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_lrf_stat
    {
        uint16_t lrf_RMS; // 2 bytes
        uint16_t lrf_Amp; // 2 bytes
        float lrf_correction; // 4 bytes
        uint16_t frequency;
        float vavd;
    } t_lrf_stat;
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_telemetry_for_developers
    {
        uint16_t frame_number;
        t_lrf_stat lrf_stat;
        uint16_t cycle_execution_time_mb;
        uint16_t cycle_execution_time_fb;
        uint16_t time_between_cycles_mb;
        uint16_t time_between_cycles_fb;
        uint16_t mb_tel_counter;
        uint16_t fb_tel_counter;
        uint8_t mb_tel_per_sec;
        uint8_t gsp_per_sec;
        uint8_t cpu_per_sec;
        uint8_t detector_per_sec;
    } t_telemetry_for_developers;
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct _t_telemetry_kalman
    {
        float coord_z;
        float coord_x;
        float speed_z;
        float speed_x;
        float accel_z;
        float accel_x;
    } t_telemetry_kalman;
#pragma pack(pop)
#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_telemetry_struct
    {
        t_video_channel_struct tv;	//0-21
        t_video_channel_struct sw;	//22-43
        t_video_channel_struct mw;	//44-65
        t_rangefinder_struct rf;	//66-87
        t_gsp_struct gsp;	// 26 bytes
        t_controller_struct controller; //35 bytes
        t_tracker_struct tracker;
        struct {
            uint8_t tv_colour_en : 1;
            uint8_t swir_en      : 1;
            uint8_t mwir_en      : 1;
            uint8_t tv_grey_en   : 1;
        } agc;
        uint8_t inversion_mw;
        t_telemetry_for_developers dev;
        t_telemetry_kalman kalman_filter;
    } t_telemetry_struct; // 149 bytes
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct /*__attribute__ ((__packed__))*/ _t_algorithms_state
    {
        unsigned tv_mono_agc : 1;
        unsigned sw_agc      : 1;
        unsigned lw_agc      : 1;
        unsigned tv_col_lagc : 1;
        unsigned tv_autoexp  : 1;
        unsigned sw_autoexp  : 1;
        unsigned lw_autoexp  : 1;
        unsigned autofocus   : 1;
        unsigned field_sync  : 1;
        unsigned tracking    : 1;
    } t_algorithms_state;
#pragma pack(pop)

    explicit Corsair_Telemetry():Corsair_Cmd(){}

    bool from_buffer(const uint8_t * buffer, const uint32_t & buffer_len) override;
    bool from_packet(Packet& packet);

    t_telemetry_struct * get();

protected:
    t_telemetry_struct telemetry;
};

#endif // CORSAIR_TELEMETRY_H
