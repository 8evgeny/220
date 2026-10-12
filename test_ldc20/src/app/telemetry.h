#ifndef TELEMETRY_H
#define TELEMETRY_H
#include "ev9500m/camera_ev9500m.h"
#include "dbus_wrapper.h"
#include "gst_wrapper.h"
#include "j200/lens_j200_wrapper.h"
#include "ldc20i/ldc20i_controller.h"
#include "mini640/mini640_controller.h"
//#include "v4l2.h"
#include <QObject>

#include <cstdint>

const qsizetype udp_payload_len = 5120;


class Telemetry
{

public:
    explicit Telemetry();
    
    
    
    QByteArray setData(uint16_t video_pkt_id, char* video_subframe);
    QByteArray setVideoServiceData(uint16_t video_frame_num, uint16_t video_width, uint16_t video_height, uint16_t video_frame_type);
    QByteArray setServiceData();
    void updateServiceData();
    void procVICAPstatus(CameraStatus status);
    void procLDCstatus(LDC_STATE dev_state);
    void procJ200status(J200_DEVSTATE ds);

    //void procUpdateTracker();
    void proc_MINI640_State(MINI640_DEV_STATE dev_state);

    void proc_UpdateTrackerObject(ushort cx, ushort cy, ushort w, ushort h);
    void proc_UpdateTrackerActiveLockTracking(bool active, bool tracking);


    void procGspStatus( const bool connected);
    void procGspPositionMode(   const bool zero, const bool pohod, const bool pilot, const bool park,
                                const bool stabilization);

    void procGspMotorAngle( const float z, const float y);
    void procGspMotorSpeed( const float z, const float y);

    float getTvFieldGrad( float *const aovV = nullptr);


    typedef struct {
        uint8_t conn_pc_eth:1;
        uint8_t conn_server_fc:1;
        uint8_t reserv:1;
        uint8_t conn_gsp_rs422:1;
        uint8_t conn_cpu_eth:1;
        uint8_t conn_mobile_seriallite:1;
        uint8_t conn_lrf_rs485:1;
        uint8_t conn_sensor_tv_uart:1;

        uint8_t conn_sensor_tv_camlink	:1;
        uint8_t conn_lens_tv_rs232	:1;
        uint8_t conn_sensor_sw_uart	:1;
        uint8_t conn_sensor_sw_camlink	:1;
        uint8_t conn_lens_sw_rs232	:1;
        uint8_t conn_sensor_mw_uart	:1;
        uint8_t conn_sensor_mw_camlink	:1;
        uint8_t conn_lens_mw_rs232 	:1;

        uint8_t video_tv_bayer	:1;
        uint8_t video_tv_mono	:1;
        uint8_t video_sw	:1;
        uint8_t video_mw	:1;
        uint8_t video_fusion	:1;
        uint8_t ready_lrf	:1;
        uint8_t ready_gsp	    :1;
        uint8_t ready_tv_image	:1;

        uint8_t ready_sw_image  :1;
        uint8_t ready_mw_image  :1;
        uint8_t drive_tv_zoom	:1;
        uint8_t drive_tv_focus	:1;
        uint8_t drive_sw_zoom	:1;
        uint8_t drive_sw_focus	:1;
        uint8_t drive_mw_zoom	:1;
        uint8_t drive_mw_focus	:1;

        uint8_t drive_gsp_azimuth	:1;
        uint8_t drive_gsp_elevation	:1;
        uint8_t reserv_2:6;

        uint8_t reserv_b;
    } __attribute__((packed)) selftest_status_str;


    typedef struct {
        uint8_t TARGET_STATE;
        uint16_t TARGET_X;
        uint16_t TARGET_Y;
        uint16_t TARGET_X_SZ;
        uint16_t TARGET_Y_SZ;
        float    TARGET_A_Z;
        float    TARGET_A_X;
        float    TARGET_A_Z_SZ;
        float    TARGET_A_X_SZ;
    } __attribute__((packed))   target_str;

    // typedef struct {
    //     uint8_t LDC_TREG_LD:1;
    //     uint8_t LDC_TREG_RAD:1;
    //     uint8_t LDC_EMIT_EN:1;
    //     uint8_t LDC_OVERHEAT:1;
    //     uint8_t LDC_COOL_EN:1;
    //     uint8_t LDC_CONTINOUS_EN:1;
    //     uint8_t LDC_COOL_PAUSE:1;
    //     uint8_t reserv_1:1;

    //     uint8_t reserv_2;
    // } __attribute__((packed))   rf_state_str;


        typedef struct {
    uint16_t ready_reserve : 1;  // всегда 1
    uint16_t ready         : 1;
    uint16_t power_on      : 1;
    uint16_t body_ovrht    : 1;
    uint16_t zero_reserve  : 1; // всегда 0
    uint16_t radiation     : 1;
    uint16_t cooling       : 1;
    uint16_t test_mode     : 1;
    uint8_t reserv_2;
} __attribute__((packed))   rf_state_str;


    typedef union
    {
        uint16_t        flags;

        struct
        {
            uint16_t    reserv_0        : 3;
            uint16_t    stabilization   : 1;
            uint16_t    zero            : 1;
            uint16_t    pohod           : 1;
            uint16_t    pilot           : 1;
            uint16_t    reserv_1        : 1;

            uint16_t    park            : 1;
            uint16_t    reserv_2        : 7;
        };
    }
    __attribute__((packed))   gsp_mode;



    typedef struct
    {
        uint8_t TV_SENSOR_STATUS;         //преамбула UDP
        uint8_t TV_LENS_STATUS;//флаг видео пакета
        uint8_t  TV_FPS;
        uint16_t TV_VRES;
        uint16_t TV_HRES;
        uint16_t TV_EXP;
        uint16_t TV_ZOOM_DRIVE;
        uint16_t TV_FOCUS_DRIVE;
        int8_t TV_LENS_TEMP;
        float TV_FOCUS_MM;
        float TV_FIELD_GRAD;
        uint8_t SW_SENSOR_STATUS;
        uint8_t SW_LENS_STATUS;
        uint8_t SW_FPS;
        uint16_t SW_VRES;
        uint16_t SW_HRES;
        uint16_t SW_EXP;
        uint16_t SW_ZOOM_DRIVE;
        uint16_t SW_FOCUS_DRIVE;
        int8_t SW_LENS_TEMP;
        float  SW_FOCUS_MM;
        float SW_FIELD_GRAD;
        uint8_t MW_SENSOR_STATUS;
        uint8_t MW_LENS_STATUS;
        uint8_t MW_FPS;
        uint16_t MW_VRES;
        uint16_t MW_HRES;
        uint16_t MW_EXP;
        uint16_t MW_ZOOM_DRIVE;
        uint16_t MW_FOCUS_DRIVE;
        int8_t   MW_LENS_TEMP;
        float    MW_FOCUS_MM;
        float    MW_FIELD_GRAD;
        uint8_t RF_STATUS;
        uint8_t RF_MEAS_STATE;
        float RF_LAST_DISTANCE;
        float RF_AVG_DISTANCE;
        float RF_RMS_DISTANCE;
        rf_state_str RF_STATE;
        uint16_t RESERV;
        float RF_BDT;

        uint8_t GSP_STATUS;
        gsp_mode GSP_MODE;
        uint32_t POS_Y;
        uint32_t POS_Z;
        float ANGLE_Y;
        float ANGLE_Z;
        float SPEED_Y;
        float SPEED_Z;

        uint8_t TV_AGC;
        uint8_t SW_AGC;
        uint8_t MW_AGC;
        uint8_t AF_STATE;
        uint8_t REGULAR_FIELD_SYNC_STATE;
        uint8_t TRACK_STATE;
        uint8_t TARGET_STATE;
        int16_t TRK_STR_X;
        int16_t TRK_STR_Y;
        int16_t TRK_STR_XSZ;
        int16_t TRK_STR_YSZ;
        float TARGET_Z;
        float TARGET_X;
        float TARGET_Z_SZ;
        float TARGET_X_SZ;
        uint8_t FC0_STATUS;
        uint8_t FC1_STATUS;
        uint8_t ETH_STATUS;
        uint8_t COM_STATUS;
        uint8_t ID_YEAR;
        uint8_t ID_MONTH;
        uint8_t ID_ID;
        uint8_t ID_HARD_VER;
        uint8_t ID_SOFT_VER;
        uint16_t ID_HOURS;
        uint8_t ID_MINUTES;
        uint8_t  SW_GAIN;
        selftest_status_str SELF_TEST;
        uint8_t SELF_TEST_IN_PROGRESS;
        float SELF_TEST_SUC_PERC;
        uint16_t SELF_TEST_COUNT;
        uint32_t ALIGNMENT_POS_Y;
        uint32_t ALIGNMENT_POS_Z;
        uint8_t TRACKER_CHANNEL;
        uint8_t TARGET_ID;

        uint8_t TARGET_1_STATE;
        uint16_t TARGET_1_X;
        uint16_t TARGET_1_Y;
        uint16_t TARGET_1_X_SZ;
        uint16_t TARGET_1_Y_SZ;
        float    TARGET_1_A_Z;
        float    TARGET_1_A_X;
        float    TARGET_1_A_Z_SZ;
        float    TARGET_1_A_X_SZ;

        uint8_t TARGET_2_STATE;
        uint16_t TARGET_2_X;
        uint16_t TARGET_2_Y;
        uint16_t TARGET_2_X_SZ;
        uint16_t TARGET_2_Y_SZ;
        float    TARGET_2_A_Z;
        float    TARGET_2_A_X;
        float    TARGET_2_A_Z_SZ;
        float    TARGET_2_A_X_SZ;

        uint8_t TARGET_3_STATE;
        uint16_t TARGET_3_X;
        uint16_t TARGET_3_Y;
        uint16_t TARGET_3_X_SZ;
        int8_t HOLE1[11];
        uint16_t TARGET_3_Y_SZ;
        float    TARGET_3_A_Z;
        float    TARGET_3_A_X;
        float    TARGET_3_A_Z_SZ;
        float    TARGET_3_A_X_SZ;

        uint8_t TARGET_4_STATE;
        uint16_t TARGET_4_X;
        uint16_t TARGET_4_Y;
        uint16_t TARGET_4_X_SZ;
        uint16_t TARGET_4_Y_SZ;
        float    TARGET_4_A_Z;
        float    TARGET_4_A_X;
        float    TARGET_4_A_Z_SZ;
        float    TARGET_4_A_X_SZ;

        uint8_t AGC;
        uint8_t RESERV2[412-292];
        uint16_t FrameNumber;
        uint16_t MesID;
        uint16_t DevID;
        uint8_t RESERV3[512-420];
    } __attribute__((packed))  telematic_status;


    typedef struct
    {
        uint16_t pkt_preambule;        //преамбула UDP
        uint16_t video_pkt_flag;       //флаг видео пакета
        uint16_t video_pkt_id;         //номер фрагмента видеокадра
        uint8_t video_pkt_data[udp_payload_len];  //видеоданные: ТВ-изображения BGGR888 или ТПВ-изображения GRAY8
    } __attribute__((packed))  udp_video_pack;

    typedef struct
    {
        uint16_t pkt_preambule;         //преамбула UDP
        uint16_t service_video_pkt_flag;//флаг видео пакета
        uint16_t video_frame_num;       //номер фрагмента видеокадра
        uint16_t video_width;           //длина строки кадра
        uint16_t video_height;          //количество строк кадра
        uint16_t video_frame_type;      //тип кадра
        uint8_t  res[6];                //дополнение до минимального размера сообщения 60 байт

    } __attribute__((packed))  udp_video_service_pack;




    typedef struct
    {
        uint8_t  TV_SENSOR_STATUS;
        uint8_t  TV_LENS_STATUS;
        uint8_t  TV_FPS;

    } cmd_msg;

    typedef struct
    {
        uint8_t  CMD;
        uint8_t  STATUS;
        uint16_t  SRC;
        uint16_t  DST;
        uint16_t WORD_COUNT;
        telematic_status PAYLOAD;
    } __attribute__((packed))  udp_service_msg;

    typedef struct
    {
        uint8_t  CMD;
        uint8_t  STATUS;
        uint16_t  SRC;
        uint16_t  DST;
        uint16_t WORD_COUNT;
        uint16_t CODE;
        uint16_t SUPP;
        uint8_t PAYLOAD[12];
        uint8_t RESERVE[488];
    } __attribute__((packed))  udp_command_msg;


    typedef struct
    {
        uint16_t pkt_preambule;         //преамбула UDP
        uint16_t service_pkt_flag;//флаг видео пакета
        udp_service_msg  service_msg;                //дополнение до минимального размера сообщения 60 байт

    } __attribute__((packed)) udp_service_pack;

    typedef struct
    {
        uint16_t pkt_preambule;         //преамбула UDP
        uint16_t service_pkt_flag;//флаг видео пакета
        udp_command_msg  service_msg;                //дополнение до минимального размера сообщения 60 байт

    } __attribute__((packed)) udp_command_pack;


private:

    QByteArray ba_vpkt;
    udp_video_pack* vpkt_ptr;

    QByteArray ba_svpkt;
    udp_video_service_pack* svpkt_ptr;

    QByteArray ba_spkt;
    udp_service_pack* spkt_ptr;

    telematic_status s_telstatus;
    QMutex mStatus;

    float 	m_aovV;
};




#endif // TELEMETRY_H
