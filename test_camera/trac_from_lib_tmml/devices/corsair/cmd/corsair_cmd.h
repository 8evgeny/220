#ifndef CORSAIR_CMD_H
#define CORSAIR_CMD_H

#include "corsair_packet.h"

#define CONFIRM_CODE_AUTO_INSPECTION  0xBBE3
#define CONFIRM_CODE_ALIGNMENT 0xD8A4

enum COMNEW : uint8_t
{
    CH_ENABLE         = 0x00, //Разрешить формирование выходного видеопотока (PCCOM_STREAM_MUX = 0xAA)
    CONTR_ENHANCEMENT = 0x01, //Разрешить обработку видеопотока для повышения контраста (PCCOM_AGC_TV     = 0xF5)
    CONTR_PROCESSING_BRIGHTNESS = 0x4A, //Разрешить обработку видеопотока для повышения контраста (PCCOM_AGC_TV     = 0xF5)
    CONTR_PROCESSING_CONTRAST = 0x4B, //Разрешить обработку видеопотока для повышения контраста (PCCOM_AGC_TV     = 0xF5)
    CONTR_INVERSION_MW = 0x4C, //Разрешить обработку видеопотока для повышения контраста (PCCOM_AGC_TV     = 0xF5)
    AUTOFOCUS         = 0x02, // Автоматическая автофокуссировка (PCCOM_AUTOFOCUS  = 0xF0)
    // Управление приводом
    MAN_ZOOM_IN       = 0x03, //фокусное расстояние (PCCOM_TV_LENS(Контроллер ТВ-объектива), PCCOM_LWIR_LENS (LWIR), PCCOM_SWIR_LENS (SWIR))
    MAN_ZOOM_OUT      = 0x04,
    MAN_ZOOM_STOP     = 0x05,
    MAN_ZOOM_POSITION = 0x1A,
    MAN_FOCUS_FAR     = 0x06, //привод резкости
    MAN_FOCUS_NEAR    = 0x07,
    MAN_FOCUS_STOP    = 0x08,
    MAN_FOCUS_POSITION= 0x1B,
    MAN_IRIS_OPEN     = 0x09, //диафрагма
    MAN_IRIS_CLOSE    = 0x0A,
    MAN_IRIS_STOP     = 0x0B,
    FIELD_SYNC       = 0x0C, //синхронизация угловых полей (PCCOM_FIELDSYNС)
    FIELD_SYNC_REG   = 0x0D,
    FIELD_SYNC_NARROW = 0x0E, // Синхронизация в узком поле. Тлевизор в узкое поле, тепловизор подъезжает к нему
    AUTO_INSPECTION   = 0xA1, // self test en
    //Управление дальномером
    LRF_MAKE_RADIATION = 0x11,
    LRF_POWER_ON       = 0x12,
    LRF_SET_FREQ       = 0x13,
    //Управление платформой гиростабилизации
    ARR_SET           = 0x20,  //PCCOM_PLATFORM
    STAB_EN           = 0x21,
    SET_POS           = 0x23,
    SET_SPEED         = 0x24,
    CMD_GSP_CONTROLER = 0x25,
    CONTROL_MODE      = 0x26,
    SET_ANGLE         = 0x27, // установить отосительный угол
    SET_ABS_ANGLE     = 0x29, // установить абсолютный угол
    GSP_CLICK_CONTROL = 0x2A,
    ALIGNMENT         = 0xA0, // установить ноль платформы
    //Управление автоматом захвата и сопровождеиня
    TRACKING_EN_SINGLE  = 0x30,
    TRACKING_DIS_SINGLE = 0x31,
    TRACKING_EN_MULTI   = 0x33,
    TRACKING_DIS_MULTI  = 0x34,
    TRACKING_CHS_MULTI  = 0x32,
    TRACKING_DEVIATION  = 0x35,
    // Отправка служебного пакета DSP
    SERVICE_DSP       = 0x32,
    // КВИК управление LOW/HIGH mode
    SWIR_GAIN_MODE    = 0x40,
    //Управление пультом
    PANEL_CONNECTION_NOTICE = 0xE0,
    PANEL_STROBE_SIZE       = 0xE1,
    // Сохранение порядкового номера, времени наработки и т.д.
    SAVE_FLASH_DATA = 0xFA,
    //Резерв
    REZERV            = 0xF0, //0xF0...0xFE
    //ПИНГ
    PING              = 0xFE,
    //Код для командпо старому протоколу
    DESIGNER_CMD      = 0xFF
};
//enum PCCOM : uint8_t
//{
//    PCCOM_NOP        = 0x00, ///< Пустая команда
//    PCCOM_PLATFORM   = 0x55, ///< Управление платформой
//    PCCOM_TV_LENS    = 0x90, ///< Контроллер ТВ - объектива
//    PCCOM_LWIR_LENS  = 0x91, ///< Контроллер LWIR - объектива
//    PCCOM_SWIR_LENS  = 0x92, ///< Контроллер SWIR - объектива
//    PCCOM_LRF        = 0x93, ///< Дальномер
//    PCCOM_APTINA     = 0xA0, ///< Запись регистра Aptina
//    PCCOM_SWIR       = 0xA2, ///< Команда для SWIR
//    PCCOM_CYGNET     = 0xA3, ///< \bug Команда не определена в документации
//    //Этот код не используется для команд объективам давно, для этого есть 3 отдельных кода. 0xAA - управляет мультиплексором видеопотоков в неподвижной плате (см. далее)
//    PCCOM_STREAM_MUX = 0xAA,
//    PCCOM_AUTOFOCUS  = 0xF0, ///< \bug Команда не определена в документации
//    PCCOM_FIELDSYNC  = 0xF1, ///< \bug Команда не определена в документации
//    PCCOM_TELEMETRY  = 0xF3, ///< \bug Команда не определена в документации
//    PCCOM_DSP        = 0xF4, ///< \bug Команда не определена в документации
//    PCCOM_AGC_TV     = 0xF5,
//    PCCOM_TEST_CONN  = 0x77,
//    PCCOM_XENICS_AUTOEXP = 0xF9
//};
//enum OBJ_CMD: uint8_t
//{
//    OBJ_THERMOSTAT_ON = 0x30u,
//    OBJ_THERMOSTAT_OFF = 0x31u,
//    OBJ_HEAT_ON = 0x10u,
//    OBJ_HEAT_OFF = 0x11u,
//    OBJ_AUTOFOCUS_EN = 0x13u,
//    OBJ_AUTOFOCUS_DIS = 0x14u,
//    OBJ_ZOOM_POS      = 0x1A, ///< Привод увеличения в координату
//    OBJ_FOCUS_POS     = 0x1B, ///< Привод резкости
//    OBJ_IRIS_POS      = 0x1C,
//};
enum t_gsp_default_position : uint8_t
{
    en_gdp_zero     = 0xA1, // ноль
    en_gdp_hike     = 0xA2, // поход
    en_gdp_piloting = 0xA3, // пилотирование
    en_gdp_parking  = 0xA4, // парковка
    en_gdp_sarter   = 0xA5, // разарретировать
    en_gdp_arr      = 0xA6  // арретировать
};

class Corsair_Cmd : public Packet
{
public:
    typedef struct _tcmd {
        uint16_t cmd;
        uint16_t scmd;
        uint8_t data[12];
    } tcmd;
    Corsair_Cmd(const uint16_t & _cmd = 0, const uint16_t & _scmd = 0);
    void set_cmd_cmd(const uint16_t & v) {_cmd_cmd = v;}
    void set_cmd_scmd(const uint16_t & v) {_cmd_scmd = v;}
    uint16_t cmd_cmd() const {return _cmd_cmd;}
    uint16_t cmd_scmd() const {return _cmd_scmd;}
    virtual uint32_t to_buffer(uint8_t *) override;
    virtual bool from_buffer(const uint8_t *, const uint32_t &) override;
protected:
    uint16_t _cmd_cmd;
    uint16_t _cmd_scmd;
};

#endif // CORSAIR_CMD_H
