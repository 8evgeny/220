#ifndef GyroPlatformVCD_CONTROLLER_H
#define GyroPlatformVCD_CONTROLLER_H

#include "gpf_qserial.h"
#include <QObject>
#include <QTimer>
#include <QMutex>


#define CRC_TABLE_SIZE 256

/** CRC polynome table */
uint8_t _CRC8Table[CRC_TABLE_SIZE];

/** Polynome width */
uint32_t _poly_width = 8;

/** Initial value */
uint8_t _init_value = 0;

/** Final XOR value */
uint8_t _xor_value = 0;

/** Initialization flag */
uint8_t _is_inited = 0;


enum VCD_ERROR_ENUM
{
    VCD_ERROR_STUB = 0x7f,
    VCD_ERROR_SEND = 0x01,
    VCD_ERROR_BUF_SIZE = 0x02,
    VCD_ERROR_NO_DEV = 0x03,
    VCD_ERROR_NO_FUNC = 0x04,
};


enum GyroPlatformVCD_DEST_MAP_enum {

    VCD_HOST_ADDR = 0x01,
    VCD_GYRO_ADDR = 0x01,
};

enum GyroPlatformVCD_FUNC_MAP_enum {

    GyroPlatformVCD_MODE 	            =	0x01, 	//	Установка режима работы
    GyroPlatformVCD_CONTROL_POSITION 	=	0x02, 	//	Управление по положению
    GyroPlatformVCD_CONTROL_SPEED 	    =	0x03, 	//	Управление по скорости
    GyroPlatformVCD_CONTROL_TRACKING 	=	0x04, 	//	Управление относительными углами
    GyroPlatformVCD_STATUS 	            =	0x05, 	//	Состояние устройства (см. 5.1 Структура  TelemetryUART)
    GyroPlatformVCD_VERSION 	        =	0x00 	//	Версии протокола и устройства (см 6.3. Структура Version)

};

const char GyroPlatformVCD_SOF = 0x40;
const char GyroPlatformVCD_WR = 0x57;
const char GyroPlatformVCD_RD = 0x52;
const char GyroPlatformVCD_FLUSH = 0x3E; //Copy registers data to flash user data
const char GyroPlatformVCD_RESTORE = 0x3C; //Copy flash factory data to flash customer data


const char VCD_SOF = 0xAA;



typedef struct {
    uint8_t mode; //Установка режима    работы
} __attribute__((packed)) VPF_MODE_STR;

typedef struct {
    float angle_course; //Угловое положение по оси  курс
    float angle_tang; //Угловое положение по оси тангаж
} __attribute__((packed)) VPF_CONTROL_POS_STR;

typedef struct {
    float speed_course; //Курс
    float speed_tang; //Тангаж
} __attribute__((packed)) VPF_CONTROL_SPEED_STR;


typedef struct {
    float delta_course; //Рассогласование по оси курса
    float delta_tang; //Рассогласование по оси  тангажа
    float lim_speed_course; //Ограничение ускорения по оси курса
    float lim_speed_tang; //Ограничение ускорения по оси тангажа

} __attribute__((packed)) VPF_CONTROL_TRACK_STR;


typedef struct {
    uint8_t error_all_mems:1; // ошибка всех MEMS
    uint8_t reserv:7; // зарезервировано

} __attribute__((packed)) VPF_MEMS_ERRORS_STR;

typedef struct {
    uint8_t unset_arr;
    uint8_t illegal_data_arr;
    uint8_t spi_timeout_arr;
    uint8_t reserv_arr;

} __attribute__((packed)) VPF_MEMS_FLAG_STR;

typedef struct {
    uint8_t mode; //
    VPF_MEMS_ERRORS_STR errors;
    VPF_MEMS_FLAG_STR flags;
    uint8_t AxisSwitchMode;
    uint8_t MemsProcessMode;
    float speedYaw;
    float speedPitch;
    float angleYaw;
    float anglePitch;
} __attribute__((packed)) VPF_MEMS_BOARD_STATUS_STR;





typedef struct {
    uint8_t overcurrent:1; //перегрузка по току
    uint8_t overheat:1;    //перегрев
    uint8_t overspeed:1;   //превышение скорости
    uint8_t low_magnetic:1;//слабое магнитное поле
    uint8_t low_voltage:1; //слабое магнитное поле
    uint8_t reserv:3;      //резерв
} __attribute__((packed)) VPF_MOTOR_ERRORS_STR;

typedef struct {
    uint8_t error:1; //ошибка
    uint8_t sector_lim_en:1;    //ограничение сектора включено
    uint8_t calib_en:1;   //калибровка включена
    uint8_t unsave_data_flag:1;//несохраненные данные
    uint8_t reserv:3;      //резерв
} __attribute__((packed)) VPF_MOTOR_FLAGS_STR;


typedef struct {
    uint8_t mode;

} __attribute__((packed)) VPF_MOTOR_STATUS_STR;


typedef struct {
    uint8_t mode; //Рассогласование по оси курса
    VPF_MEMS_BOARD_STATUS_STR mems; //Рассогласование по оси  тангажа
    VPF_MOTOR_STATUS_STR motorYaw; //Статус мотора курса
    VPF_MOTOR_STATUS_STR motorPitch; //Статус мотора тангажа
} __attribute__((packed)) VPF_STATUS_STR;

typedef struct {
    uint32_t versionProtocolVDB; //Версия протокола VDB (например, 1.0)
    uint32_t versionProtocol; //Версия протокола взаимодействия
    uint32_t versionDevice; //Версия устройства
} __attribute__((packed)) VPF_VERSION_STR;

typedef struct {

    VPF_STATUS_STR status;
    VPF_VERSION_STR ver;

    bool isTimeout;
   // uint8_t raw_reg[512];
}__attribute__((packed)) GyroPlatformVCD_DEV_STATE;






enum GyroPlatformVCD_ANS_enum
{

    ANS_OK = 0x01,         // Ошибка. Ответ не принят
    ANS_ERROR = 0x10,         // Ошибка команды
    ANS_NO_REPLY = 0xFF,         // Ошибка. Ответ не принят

} ;






typedef struct{
    uint8_t function;  //Адрес устройства
    uint8_t device;    // Адрес функции
}__attribute__((packed)) VCD_ADDR_STR;

typedef struct{
    uint8_t      cmd;      //Тип команды (например, запрос на чтение или запись)
    uint8_t      status;   //Статус запроса или ответа (успех/ошибка)
    VCD_ADDR_STR src;      // Адрес отправителя
    VCD_ADDR_STR dst;      //Адрес /Адрес получателя
    uint16_t     byteCount;// Количество байтов в поле данных

} __attribute__((packed)) VCD_HEAD_STR;

typedef struct
{

    GyroPlatformVCD_ANS_enum ans_code;
    VCD_HEAD_STR req;
    //  ANS
} GyroPlatformVCD_ANS_str;



static QList<quint32> getAddressArray();

#define CONST_HDR 0xFF




class GyroPlatformVCD_Controller: public QObject
{
    Q_OBJECT;
    QThread thread;

public:
    GyroPlatformVCD_Controller(QString port, quint32 baudrate, QObject *parent = nullptr);
    qint32 proc_LDC_GET_MEASUREMENTS();
    qint32 proc_LDC_GET_DISTANCE();
    qint32 proc_LDC_SET_TEMP_REG_STATE(uint8_t state);
    qint32 proc_LDC_GET_TEMP_REG_STATE();
    qint32 proc_LDC_GET_EMIT_STATE();
    qint32 proc_LDC_SET_EMIT_STATE(uint8_t state);
    qint32 proc_LDC_SET_IMPULSE_PERIOD(uint32_t val);
    qint32 proc_LDC_SET_IMPULSE_MFREQ(uint32_t val);


    QList<quint32> get_GPFAddr_List();
    GyroPlatformVCD_DEV_STATE get_MINI_DS();

Q_SIGNALS:
    void comInitConnection();
    void comCloseConnection();
    void sendData(QByteArray ba);
    void recieved_GyroPlatformVCD_ANS(GyroPlatformVCD_ANS_enum cmd);
    void sig_READY();

    void comDeviceConnected();
    void send_GyroPlatformVCD_CMD(GyroPlatformVCD_REG_MAP_enum addr, quint32 cmd, quint32 par);

public slots:
    void proc_GyroPlatformVCD_CMD(GyroPlatformVCD_REG_MAP_enum addr, quint32 cmd, quint32 par);
    void proc_GOWIN_CMD(quint32 cmd);


    void proc_deviceConnected();
    void proc_timeout_event();
    void proc_dataParse(QByteArray ba);
    void proc_InitReady();

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

    void proc_serialANS(const VCD_HEAD_STR &resp);

    void parse_serialANS(const VCD_HEAD_STR *head_ans, QByteArray &buf);

    bool bufClear(int32_t* it);
    //void checkTRegState();
    QByteArray proc_GyroPlatformVCD_SendCMD(const VCD_HEAD_STR &req);

    QTimer* m_AnswerTimeoutTimer;

    QList<quint32> mini640_addr_list;

    char await_ans_len = 0;
    char await_ans_cmd = 0;
    VCD_HEAD_STR req_cmd = {0};

    QByteArray buf;
    QString m_port;
    quint32 m_baudrate;

    GyroPlatformVCD_QSerial *com;
    int cycle_limit = 10;

    bool await_ans = false;

    GyroPlatformVCD_DEV_STATE dev_state = {0};

    QMutex mDsUpdateLock;
};

#endif // GyroPlatformVCD_CONTROLLER_H
