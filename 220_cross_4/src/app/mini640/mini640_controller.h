#ifndef MINI640_CONTROLLER_H
#define MINI640_CONTROLLER_H

#include "mini640_qserial.h"
#include <QObject>
#include <QTimer>
#include <QMutex>

enum U16_POS
{
    U16_LSB,
    U16_MSB
};

enum I24_POS
{
    I24_LSB,
    I24_MID,
    I24_MSB
};

enum U24_POS
{
    U24_LSB,
    U24_MID,
    U24_MSB
};

enum GOWIN_MINI640_CMD_enum {
    GOWIN_MINI640_GET_TS = '#',
    GOWIN_MINI640_COOLER_ON = '+',
    GOWIN_MINI640_COOLER_OFF = '-'
};


enum MINI640_REG_MAP_enum {

    CCE_CONF             = 0x100,	   //C&CE Board configuration
    CCE_Input_CTRL  	 = 0x102,	   //C&CE Board Synchronization

    CCE_PBIT             = 0x110,	   //Power-Up Built-in-test result
    CCE_CBIT             = 0x111,	   //Continuous Built-in-test result
    CCE_Temp_status      = 0x112,	   //The FPA temperature threshold.
    CCE_Power_status	 = 0x113,	   //Internal power supplies status
    MONITOR_REF2_5V_LSB	 = 0x114,	   //LSB Power supply 2.5V Value
    MONITOR_REF2_5V_MSB	 = 0x115,	   //MSB Power supply 2.5V Value
    MONITOR_3_3V_ADC_LSB = 0x116,	   //LSB Power supply 3.3V_ADC Value
    MONITOR_3_3V_ADC_MSB = 0x117,	   //MSB Power supply 3.3V_ADC Value
    MONITOR_5V_N4V_LSB	 = 0x118,	   //LSB Power supply 5V_N4V Value
    MONITOR_5V_N4V_MSB	 = 0x119,	   //MSB Power supply 5V_N4V Value
    PATTERN_CTRL	     = 0x120,	   //Test Pattern activation
    GPOL_DAC_MSB	     = 0x131,	   //LSB of the voltage bias GPOL value x 43.6
    GPOL_DAC_LSB	     = 0x130,	   //MSB of the voltage bias GPOL value x 43.6
    CCE_Ref_1            = 0x150,	   //Reference the board
    CCE_VersionSft_1	 = 0x154,	   //Embedded software version
    CCE_VersionSft_2	 = 0x155,	   //Embedded software version
    CCE_VersionFPGA_1	 = 0x156,	   //FPGA software version
    CCE_VersionFPGA_2	 = 0x157,	   //FPGA software version


    CCE_Temp_LSB	     = 0x160,	   //FPA Temperature LSB
    CCE_Temp_MSB	     = 0x161,	   //FPA Temperature MSB
    CCE_Temp_Thres_LSB	 = 0x162,	   //Temperature threshold LSB
    CCE_Temp_Thres_MSB	 = 0x163,	   //Temperature threshold MSB
    A1_Temp_Poly_LSB	 = 0x164,	   //Polymonial coefficient A1 (LSB)
    A1_Temp_Poly_Mid	 = 0x165,	   //Polymonial coefficient A1 (Mid)
    A1_Temp_Poly_MSB	 = 0x166,	   //Polymonial coefficient A1 (MSB)
    A2_Temp_Poly_LSB	 = 0x167,	   //Polymonial coefficient A2 (LSB)
    A2_Temp_Poly_Mid	 = 0x168,	   //Polymonial coefficient A2 (Mid)
    A2_Temp_Poly_MSB	 = 0x169,	   //Polymonial coefficient A2 (MSB)
    A3_Temp_Poly_LSB	 = 0x16A,	   //Polymonial coefficient A3 (LSB)
    A3_Temp_Poly_Mid	 = 0x16B,	   //Polymonial coefficient A3 (Mid)
    A3_Temp_Poly_MSB	 = 0x16C,	   //Polymonial coefficient A3 (MSB)
    A4_Temp_Poly_LSB	 = 0x16D,	   //Polymonial coefficient A4 (LSB)
    A4_Temp_Poly_Mid	 = 0x16E,	   //Polymonial coefficient A4 (Mid)
    A4_Temp_Poly_MSB	 = 0x16F,	   //Polymonial coefficient A4 (MSB)
    A5_Temp_Poly_LSB	 = 0x170,	   //Polymonial coefficient A5 (LSB)
    A5_Temp_Poly_Mid	 = 0x171,	   //Polymonial coefficient A5 (Mid)
    A5_Temp_Poly_MSB	 = 0x172,	   //Polymonial coefficient A5 (MSB)
    FPA_CONF             = 0x1A0,	   //FPA control
    INT_PERIOD_TIME_1	 = 0x1B0,	   //Integration period (Byte 1)
    INT_PERIOD_TIME_2	 = 0x1B1,	   //Integration period (Byte 2)
    INT_PERIOD_TIME_3	 = 0x1B2,	   //Integration period (Byte 3)
    INT_HIGH_LEVEL_1	 = 0x1B4,	   //Integration high level (Byte 1)
    INT_HIGH_LEVEL_2	 = 0x1B5,	   //Integration high level (Byte 2)
    INT_HIGH_LEVEL_3	 = 0x1B6,	   //Integration high level (Byte 3)
    X1MIN_LSB            = 0x1D0,	   //LSB of X1min
    X1MIN_MSB            = 0x1D1,	   //MSB of X1min
    Y1MIN_LSB            = 0x1D2,	   //LSB of Y1min
    Y1MIN_MSB            = 0x1D3,	   //MSB of Y1min
    X1MAX_LSB            = 0x1D4,	   //LSB of X1max
    X1MAX_MSB            = 0x1D5,	   //MSB of X1max
    Y1MAX_LSB            = 0x1D6,	   //LSB of Y1max
    Y1MAX_MSB            = 0x1D7,	   //MSB of Y1max



};

const char MINI640_SOF = 0x40;
const char MINI640_WR = 0x57;
const char MINI640_RD = 0x52;
const char MINI640_FLUSH = 0x3E; //Copy registers data to flash user data
const char MINI640_RESTORE = 0x3C; //Copy flash factory data to flash customer data

const char GOWIN_CMD = 0xFF; //Request timer data from Gowin IC

const char GOWIN_SOF = '#';


typedef struct {
    uint8_t reserv:4;
    uint8_t video_inversion:1;
    uint8_t reserv_2:3;
} __attribute__((packed)) CCE_CONF_STR;

typedef struct {
    uint8_t master_clk_src:1; // [0]: Master clock sources (MC), 0: MC master clock source = internal FPGA clock
                                                           //    1: MC master clock source is external CC2 signal (DCLKIN)
    uint8_t master_clk_freq:2; //[2:1]: Master clock frequency (if MC is internal) x0: MC is 5MHz when MC is local
                                                                             //    x1: MC is 2.5MHz when MC is local
    uint8_t frame_source:1; //[3]: Frame sources (INT) 0: INT is generated inside the FPGA
                                                 //    1: INT source is external CC1 (FSYNC)
    uint8_t frame_source_mod:1;//[4]: Frame sources mode 0: CC1 (FSYNC) is used directly to drive the sensor INT signal.
                                                       //1: CC1 is used to start the onboard generation of INT.
    uint8_t pixel_position:2;//[6:5]: Position of the sampling in the pixel 00: Position 1 (0°)
                                                                        //  01: Position 2 (90°)
                                                                        //  10: Position 3 (180°)
                                                                        //  11: Position 4 (270°)
    uint8_t reserv:1;
} __attribute__((packed)) CCE_Input_CTRL_STR;

typedef struct {
    uint8_t reserv:1;
    uint8_t memory_checksum_state:1; //[1]: Memory Checksum status 0: Memory checksum NOK.
                                                               //  1: Memory checksum OK.
    uint8_t reserv_2:6;
} __attribute__((packed)) CCE_PBIT_STR;

typedef struct {
    uint8_t power_supply_stat:1; //[0]: Power Supply status    0: Power supply NOK.
                                                            // 1: Power supply OK.
    uint8_t reserv:1;
    uint8_t serdat_err:1; //[2]: Serdat Error 0: No default detected on the serdat signal
                                           // 1: Default detected on the serdat signal
    uint8_t reserv_2:5;
} __attribute__((packed)) CCE_CBIT_STR;

typedef struct {
    uint8_t temp_low:1; //[0]: Temperature Low   0: The FPA temperature is higher than the temperature threshold.
                                           //    1: The FPA temperature is lower than the temperature threshold.
    uint8_t reserv:7;
} __attribute__((packed)) CCE_TEMP_STAT_STR;

typedef struct {
    uint8_t ref_2V5:1; //[0]: Ref 2V5 Status  0: Power supply Ref 2V5 OK
                                        //    1: Power supply Ref 2V5 NOK
    uint8_t ref_3V3_ADC:1; //1]: Power supply 3.3V ADC Status  0: Power supply 3.3V ADC OK
                                                         //    1: Power supply 3.3V ADC NOK
    uint8_t ref_5V:1; //[2]: Power supply 5V Status    0: Power supply 5V OK
                                                 //    1: Power supply 5V NOK
    uint8_t ref_5V_M4V:1; // [3]: Power supply +5V / -4V Status 0: Power supply of +5V and -4V OK
                                                          //    1: Power supply of +5V and -4V NOK
    uint8_t reserv:4;
} __attribute__((packed)) CCE_POWER_STAT_STR;

typedef struct {
    uint8_t test_pattern_act:2; //[1..0]: Test pattern activation 00: Normal Mode.
                                                            //    01: Test pattern forced.
                                                            //    10: Automatic mode.
                                                            //    11: Not used.
    uint8_t power_on_fpa:1; //[2]: Power-On FPA    0: FPA powers supplies are shut down.
                                             //    1: FPA power supplies are ON.
    uint8_t reserv:5;
} __attribute__((packed)) FPA_PATTERN_CTRL_STR;

typedef struct {
    uint8_t readout_mod_sel:2;  //[1..0]: Readout mode selection (UPCOL, UPROW) 00: From Pixel RB,CB to RA,CA
                                                                             // 01: From Pixel RA,CB to RB,CA
                                                                             // 10: From Pixel RB,CA to RA,CB
                                                                             // 11: From Pixel RA,CA to RB,CB0: Direction Inverted.
    uint8_t video_size_sel:2; //[3..2]: Video size selection (SIZE_A, SIZE_B)   00: 640x512 (windowing mode)
                                                                             // 01: 512x512
                                                                             // 10: 640x480
                                                                             // 11: 640x512
    uint8_t reserv:4;
} __attribute__((packed)) FPA_CONF_STR;




typedef struct {
    uint16_t ref2_5V;
    uint16_t ref3_3V;
    uint16_t ref5_N4V;
    uint16_t gpol_DAC;
    uint16_t cce_temp;

}__attribute__((packed)) MINI640_MONITOR_RAW;

typedef struct {
    float ref2_5V;
    float ref3_3V;
    float ref5_N4V;
    float gpol_DAC;
    float cce_temp;

}__attribute__((packed)) MINI640_MONITOR;

typedef struct {
    uint16_t cce_temp_th;
}__attribute__((packed)) MINI640_TH_RAW;

typedef struct {
    float cce_temp_th;
}__attribute__((packed)) MINI640_TH;

typedef struct {
    int32_t ival:24;
}__attribute__((packed)) I24;

typedef struct {
    int32_t uval:24;
}__attribute__((packed)) U24;

typedef struct {
    uchar lsb_val:4;
    uchar msb_val:4;
}__attribute__((packed)) U8_CL;

typedef struct {
    uchar lsb0_val:4;
    uchar msb0_val:4;
    uchar lsb1_val:4;
    uchar msb1_val:4;
}__attribute__((packed)) U16_CL;

typedef struct {
    uchar lsb_val:4;
    uchar mid_val:4;
    uchar msb_val:4;
    uchar res:4;
}__attribute__((packed)) U12_CL;

typedef struct {
    char a1_temp_poly[3];
    char a2_temp_poly[3];
    char a3_temp_poly[3];
    char a4_temp_poly[3];
    char a5_temp_poly[3];
}__attribute__((packed)) MINI640_TEMP_POLY_RAW;


typedef struct {
    double a1_temp_poly;
    double a2_temp_poly;
    double a3_temp_poly;
    double a4_temp_poly;
    double a5_temp_poly;
}__attribute__((packed)) MINI640_TEMP_POLY;

typedef struct {
    uint8_t CCE_Ref_1;	   //Reference the board
    uint8_t CCE_VersionSft_1;	   //Embedded software version
    uint8_t CCE_VersionSft_2;
    uint8_t CCE_VersionFPGA_1;  	   //FPGA software version
    uint8_t CCE_VersionFPGA_2;  	   //FPGA software version
    }__attribute__((packed)) MINI640_HSW_VERSION;

typedef struct {
    uint32_t INT_PERIOD_TIME;	   //Reference the board
    uint32_t INT_HIGH_LEVEL;	   //Embedded software version
}__attribute__((packed)) MINI640_EXP;

typedef struct {
    uint16_t X1MIN;
    uint16_t Y1MIN;
    uint16_t X1MAX;
    uint16_t Y1MAX;
}__attribute__((packed)) MINI640_ROI;


typedef struct {
    uint8_t video_inv;
    uint8_t master_clk_src;
    uint8_t master_clk_freq;
    uint8_t frame_source;
    uint8_t frame_source_mod;
    uint8_t pixel_position;
    uint8_t memory_checksum_state;
    uint8_t power_supply_stat;
    uint8_t serdat_err;
    uint8_t temp_low;
    uint8_t ref_2V5;
    uint8_t ref_3V3_ADC;
    uint8_t ref_5V;
    uint8_t ref_5V_M4V;
    uint8_t test_pattern_act;
    uint8_t power_on_fpa;
    uint8_t readout_mod_sel;
    uint8_t video_size_sel;

    double temp_vdiode;
    MINI640_MONITOR_RAW mon_raw;
    MINI640_MONITOR mon;
    MINI640_TH_RAW th_raw;
    MINI640_TH th;
    MINI640_HSW_VERSION hsv;
    MINI640_TEMP_POLY_RAW temp_poly_raw;
    MINI640_TEMP_POLY temp_poly;
    MINI640_EXP exp;
    MINI640_ROI roi;

    bool isCoolerEn;
    uint64_t cooler_timer;
    bool isTimeout;
   // uint8_t raw_reg[512];
}__attribute__((packed)) MINI640_DEV_STATE;




typedef struct {
    uint8_t sof; //начало пакета
    uint8_t ack; //результат исполнения
    uint8_t crc16[4]; //CRC16 MODBUS 3 = [15:12] 2 = [11:8] 1 = [7:4] 0 = [3:0]
} __attribute__((packed)) RESP_CMD_RAW;

typedef struct {
    uint8_t sof; //начало пакета
    uint8_t ack; //результат исполнения
    uint8_t rd_data[2];
    uint8_t crc16[4]; //CRC16 MODBUS 3 = [15:12] 2 = [11:8] 1 = [7:4] 0 = [3:0]
} __attribute__((packed)) RESP_RD_CMD_RAW;


typedef struct {
    uint8_t sof; //начало пакета
    uint8_t ack; //результат исполнения
    uint8_t rd_data[6];
    uint8_t crc16[4]; //CRC16 MODBUS 3 = [15:12] 2 = [11:8] 1 = [7:4] 0 = [3:0]
} __attribute__((packed)) GOWIN_RESP_RD_CMD_RAW;


typedef struct {

    uint16_t addr;
    uint8_t cmd;
    uint8_t wr_data;
    uint8_t isRequest;
}  REQ_CMD;

typedef struct {

    uint16_t addr;
    uint8_t cmd;
    uint8_t ack; //результат исполнения
    uint32_t rd_data;
    uint8_t crc_ok;
} __attribute__((packed)) RESP_RD_CMD;

typedef struct {
    uint8_t sof; //начало пакета
    uint8_t cmd; // код команды, 0х57 Запись
} __attribute__((packed)) HEAD_RAW;


typedef struct {
    uint8_t cmd; // код команды, 0х57 Запись
    MINI640_REG_MAP_enum addr; //адрес
    uint32_t data; //данные
} __attribute__((packed)) WRITE_CMD;

typedef struct {
    uint8_t sof; //начало пакета
    uint8_t cmd; // код команды, 0х52 Чтение
    uint8_t addr[3]; //адрес 2 = [11:8] 1 = [7:4] 0 = [3:0]
    uint8_t data[2]; //адрес 2 = [11:8] 1 = [7:4] 0 = [3:0]
    uint8_t crc16[4]; //CRC16 MODBUS 3 = [15:12] 2 = [11:8] 1 = [7:4] 0 = [3:0]
} __attribute__((packed)) MINI640_WRITE_CMD_RAW;



typedef struct {
    uint8_t sof; //начало пакета
    uint8_t cmd; // код команды, 0х52 Чтение
    uint8_t addr[3]; //адрес 2 = [11:8] 1 = [7:4] 0 = [3:0]
    uint8_t crc16[4]; //CRC16 MODBUS 3 = [15:12] 2 = [11:8] 1 = [7:4] 0 = [3:0]
} __attribute__((packed)) MINI640_READ_CMD_RAW;

typedef struct {
    uint8_t sof; //начало пакета
    uint8_t cmd; // код команды, 0х52 Чтение
    MINI640_REG_MAP_enum addr; //адрес
    uint16_t crc16; //CRC16 MODBUS
} __attribute__((packed)) READ_CMD;


typedef struct {
    uint8_t sof; //начало пакета
    uint8_t cmd; // код команды, 0х52 Запись
    uint8_t crc16[4]; //CRC16 MODBUS 3 = [15:12] 2 = [11:8] 1 = [7:4] 0 = [3:0]
} __attribute__((packed)) MINI640_FLASH_CMD_RAW;

typedef struct {
    uint8_t sof; //начало пакета
    uint8_t cmd; // код команды, 0х52 Запись
    uint16_t crc16; //CRC16 MODBUS
} __attribute__((packed)) FLASH_CMD;





enum MINI640_ANS_enum
{

    MINI_ANS_OK = 0x01,         // Ошибка. Ответ не принят
    MINI_ANS_ERROR = 0x10,         // Ошибка команды
    MINI_ANS_NO_REPLY = 0xFF,         // Ошибка. Ответ не принят

} ;


typedef struct
{

    MINI640_ANS_enum ans_code;
    REQ_CMD req;
  //  ANS


} MINI640_ANS_str;

static QList<quint32> getAddressArray();

#define CONST_HDR 0xFF




class MINI640_Controller: public QObject
{
    Q_OBJECT;
    QThread thread;

public:
    MINI640_Controller(QString port, quint32 baudrate, QObject *parent = nullptr);
    ~MINI640_Controller();
    qint32 proc_LDC_GET_MEASUREMENTS();
    qint32 proc_LDC_GET_DISTANCE();
    qint32 proc_LDC_SET_TEMP_REG_STATE(uint8_t state);
    qint32 proc_LDC_GET_TEMP_REG_STATE();
    qint32 proc_LDC_GET_EMIT_STATE();
    qint32 proc_LDC_SET_EMIT_STATE(uint8_t state);
    qint32 proc_LDC_SET_IMPULSE_PERIOD(uint32_t val);
    qint32 proc_LDC_SET_IMPULSE_MFREQ(uint32_t val);


    QList<quint32> get_MINIAddr_List();
    MINI640_DEV_STATE get_MINI_DS();

Q_SIGNALS:
    void comInitConnection();
    void comCloseConnection();
    void sendData(QByteArray ba);
    void recieved_MINI640_ANS(MINI640_ANS_enum cmd);
    void sig_READY();

    void comDeviceConnected();
    void send_MINI640_CMD(MINI640_REG_MAP_enum addr, quint32 cmd, quint32 par);
    void send_GOWIN_CMD( quint32 cmd);

public slots:
    void proc_MINI640_CMD(MINI640_REG_MAP_enum addr, quint32 cmd, quint32 par);
    void proc_GOWIN_CMD(quint32 cmd);


    void proc_deviceConnected();
    void proc_timeout_event();
    void proc_dataParse(QByteArray ba);
    void proc_InitReady();
    void proc_Thread_Finished();

    qint32 proc_initConnection();
    qint32 proc_closeConnection();

private:
   // void crc7_generate_table();
    uint8_t crc7_calc(uint8_t crc, uint8_t data);
    uint8_t crc7_calculate_buf(QByteArray message);
    QByteArray pack_data(uint32_t data);
    int32_t unpack_data(uint8_t * buf);


    void CCE_СONF_Decode(uint8_t data);
    void CCE_Input_CTRL_Decode(uint8_t data);
    void CCE_PBIT_Decode(uint8_t data);
    void CCE_CBIT_Decode(uint8_t data);
    void CCE_TEMP_STAT_Decode(uint8_t data);
    void CCE_POWER_STAT_Decode(uint8_t data);
    void FPA_PATTERN_CTRL_Decode(uint8_t data);
    void FPA_CONF_Decode(uint8_t data);

    void proc_serialANS(const RESP_RD_CMD &resp);
    void proc_serialANSGowin(const RESP_RD_CMD &resp);
    bool bufClear(int32_t* it);
    //void checkTRegState();
    QByteArray proc_MINI640_SendCMD(const REQ_CMD &req);

    QTimer* m_AnswerTimeoutTimer;

    QList<quint32> mini640_addr_list;

    char await_ans_len = 0;
    char await_ans_cmd = 0;
    REQ_CMD req_cmd = {0};

    QByteArray buf;
    QString m_port;
    quint32 m_baudrate;

    MINI640_QSerial *com;
    int cycle_limit = 10;

    bool await_ans = false;

    MINI640_DEV_STATE dev_state = {0};

    QMutex mDsUpdateLock;
};

#endif // MINI640_CONTROLLER_H
