#ifndef LDC20I_CONTROLLER_H
#define LDC20I_CONTROLLER_H

#include "ldc20i_qserial.h"
#include <QObject>
#include <QTimer>
#include <QMutex>
#define WINDOW_SIZE 10  // количество последних измерений

typedef struct {
    float buffer[WINDOW_SIZE];
    int index;
    int count;
    float sum;
    float sum_sq;
} RollingStats;

typedef struct {
    uint8_t emit_state_code; //Код состояния излучателя
    uint8_t rad_tc_state_code; // Код состояния термоконтроллера радиатора
    uint8_t ld_tc_state_code; // Код состояния термоконтроллера ЛД
    uint8_t res;
    float temp_power_sw; // Температура силового ключа измеренная
    float temp_rad; // Температура силового ключа измеренная
    float temp_ld; // Температура ЛД измеренная
    float rad_heater_curr; // Ток потребления нагревателя корпуса измеренный
    float voltage; // Напряжение питания измеренный
    float tem_ld_curr; // Ток потребления ТЭМ канала ЛД измеренный
    float tem_ld_voltage_drop; // Падение напряжения на ТЭМ канала ЛД измеренное
    float temp_fd; // Температура ФД измеренная

} __attribute__((packed)) MEASUREMENTS_LDC;

typedef struct {
    uint8_t emit_state_code; //Код состояния излучателя
    uint8_t rad_tc_state_code; // Код состояния термоконтроллера радиатора
    uint8_t ld_tc_state_code; // Код состояния термоконтроллера ЛД
    uint8_t res;
    uint8_t temp_power_sw_raw[4]; // Температура силового ключа измеренная
    uint8_t temp_rad_raw[4]; // Температура силового ключа измеренная
    uint8_t temp_ld_raw[4]; // Температура ЛД измеренная
    uint32_t rad_heater_curr; // Ток потребления нагревателя корпуса измеренный
    uint32_t voltage; // Напряжение питания измеренный
    uint8_t tem_ld_curr_raw[4]; // Ток потребления ТЭМ канала ЛД измеренный
    uint8_t tem_ld_voltage_drop_raw[4]; // Падение напряжения на ТЭМ канала ЛД измеренное
    uint8_t temp_fd_raw[4]; // Температура ФД измеренная
}__attribute__((packed)) MEASUREMENTS_LDC_RAW;

typedef struct {
    uint32_t impulse_emit_cnt_total; //Количество стартовых импульсов с момента подачи питания
    uint32_t impulse_recieve_cnt_total; //Количество принятых импульсов с момента подачи питания
    uint32_t distance;  // Дальность
} __attribute__((packed)) DISTANCE_LDC_RAW;

typedef struct {
    uint32_t impulse_emit_cnt_total; //Количество стартовых импульсов с момента подачи питания
    uint32_t impulse_recieve_cnt_total; //Количество принятых импульсов с момента подачи питания
    float distance;  // Дальность
    float distance_mean;  // Дальность средняя
    float distance_stddev;  // Дальность средняя
}  DISTANCE_LDC; // __attribute__((packed))

typedef struct {
    uint32_t impulse_period; //Количество стартовых импульсов с момента подачи питания
    uint32_t impulse_mfreq; //Количество принятых импульсов с момента подачи питания
} __attribute__((packed)) EMIT_PAR_LDC;

typedef struct {
    uint8_t emit_mode; //Режим работы излучателя 0x01 – однократный;  0x02 – импульсный;
} __attribute__((packed)) EMIT_MODE_LDC_RAW;



typedef struct
{
    uint8_t temp_reg_rad; //Состояние температурного регулятора корпуса радиатора
    uint8_t temp_reg_ld; //Состояние температурного регулятора ЛД
} LDC_TEMP_REG_STATE_ANS;

typedef struct {
    uint8_t HDR;
    uint8_t LEN;
    uint8_t CMD;

} LDC_HEAD_STR;

typedef struct {
    MEASUREMENTS_LDC meas;
    DISTANCE_LDC dist;
    LDC_TEMP_REG_STATE_ANS treg_state;
    EMIT_PAR_LDC impulse_p;
    bool continous_emit;
    bool emit_en;

} LDC_STATE;


enum LDC_DEVSTATE_enum
{
    LDC_Device
};

enum LDC_CMD_enum
{
    //Общие запросы
    LDC_GET_MEASUREMENTS = 0x09,
    LDC_GET_DISTANCE = 0x0A,
    LDC_GET_TEMP_REG_STATE  = 0x10,
    LDC_SET_TEMP_REG_STATE  = 0xF1,
    LDC_TEMP_REG_STATE  = 0x10,


    LDC_GET_EMIT_STATE = 0x30,
    LDC_SET_EMIT_STATE = 0xF3,
    LDC_EMIT_STATE = 0x30,

    LDC_SET_EMIT_PERIOD = 0x38, // Задать период следования импульсов, мкс
    LDC_SET_EMIT_MFREQ = 0x39, //  Задать частоту следования импульсов, мГц
    LDC_SET_EMIT_MODE = 0x35 // Режим работы излучателя, однократный - импульсный

};



enum LDC_ANS_enum
{
    LDC_ANS_MEASUREMENTS = 0x49,    // Результаты измерения
    LDC_ANS_DISTANCE     = 0x4A,    // Результаты измерения
    LDC_ANS_TEMP_REG_STATE = 0x50,  // Текущее состояние 2 регуляторов температуры
    LDC_ANS_EMIT_STATE     = 0x70,  // Текущее состояние излучателя
    LDC_ANS_EMIT_MODE     = 0x75,   // Текущий режим работы излучателя
    LDC_ANS_EMIT_PERIOD = 0x78,       // Установленный период следования импульсов, мкс
    LDC_ANS_EMIT_MFREQ = 0x79,         // Установленная частота следования импульсов, мГц

    LDC_ANS_ERR_CMD = 0x40,         // Ошибка. Команда не распознана
    LDC_ANS_NO_REPLY = 0xFF,         // Ошибка. Ответ не принят

} ;


#define CONST_HDR 0xFF




class LDC20I_Controller: public QObject
{
    Q_OBJECT;
    QThread thread;

public:
    LDC20I_Controller(QString port, quint32 baudrate, QObject *parent = nullptr);
    ~LDC20I_Controller();

    qint32 proc_LDC_GET_MEASUREMENTS();
    qint32 proc_LDC_GET_DISTANCE();
    qint32 proc_LDC_SET_TEMP_REG_STATE(uint8_t state);
    qint32 proc_LDC_GET_TEMP_REG_STATE();
    qint32 proc_LDC_GET_EMIT_STATE();
    qint32 proc_LDC_SET_EMIT_STATE(uint8_t state);
    qint32 proc_LDC_SET_IMPULSE_PERIOD(uint32_t val);
    qint32 proc_LDC_SET_IMPULSE_MFREQ(uint32_t val);
    qint32 proc_LDC_SET_EMIT_MODE(uint8_t mode);

    qint32 proc_initConnection();
    qint32 proc_closeConnection();


Q_SIGNALS:
    void comInitConnection();
    void comCloseConnection();
    void sendData(QByteArray ba);
    void recieved_LDC_DS_ANS(LDC_ANS_enum cmd,  LDC_STATE dev_state);
    void comDeviceConnected();
    void send_LDC_CMD(LDC_CMD_enum cmd, quint32 par);
public slots:
    void proc_LDC_CMD(LDC_CMD_enum cmd, quint32 par);
    void proc_deviceConnected();
    void proc_timerUpdate();
    void proc_dataParse(QByteArray ba);
    void proc_Thread_Finished();

private:
    void crc7_generate_table();
    uint8_t crc7_calc(uint8_t crc, uint8_t data);
    uint8_t crc7_calculate_buf(QByteArray message);
    QByteArray pack_data(uint32_t data);
    int32_t unpack_data(uint8_t * buf);
    void     distanceDecode(DISTANCE_LDC_RAW* raw);
    void measurementsDecode(MEASUREMENTS_LDC_RAW* raw);
    void proc_serialANS(QByteArray buf);
    bool bufClear(int32_t* it);
    void checkTRegState();
    QByteArray createLDCSendBuf(LDC_HEAD_STR ldc_head, QByteArray ldc_data = nullptr);
    void rolling_init(RollingStats *rs) ;
    void  rolling_update(RollingStats *rs, float new_value, float *mean, float *stddev) ;
    RollingStats rs={0};

    QTimer* m_NotificationTimer;

    QByteArray buf;
    QString port;
    quint32 baudrate;

    uint8_t crc7_table[256];
    LDC20I_QSerial com;
    int cycle_limit = 10;

    bool await_ans = false;
    LDC_HEAD_STR LDC_HEAD;
    uint8_t LDC_DATA_ptr;
    uint8_t LDC_CRC7;
    LDC_STATE dev_state;
};

#endif // LDC20I_CONTROLLER_H
