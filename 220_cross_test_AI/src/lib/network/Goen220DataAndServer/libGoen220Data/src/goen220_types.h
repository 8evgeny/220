#pragma once

#include <QtGlobal>

namespace Goen220 {

Q_NAMESPACE

/// @brief Типы каналов RTP
enum class CHANNEL : quint8 {
    Tv = 0x00, ///< Канал ТВ
    Mw = 0x20  ///< Канал ТПВ
};
Q_ENUM_NS(CHANNEL)

/// @brief Коды ошибок библиотеки libGoen220
enum class Error {
    None = 0,      ///< Ошибок нет
    CtrlConn,      ///< Коммуникационная ошибка соединения канала управления ГОЭН-220
    TelemetryConn, ///< Коммуникационная ошибка соединения канала передачи телеметрии
    RspInvalDevFn  ///< Неверный код команды-устройства в ответе ГОЭН-220
};
Q_ENUM_NS(Error)

/// @brief Состояния соединения канала управления ГОЭН-220
enum class CtrlConnState {
    Unconnected = 0, ///< Соединение отсутствует
    Connecting,      ///< Фаза установления соединения
    Connected,       ///< Соедиение установлено
    Disconnecting    ///< Фаза закрытия соединения
};
Q_ENUM_NS(CtrlConnState)

enum class Dev : quint16 {
    Workstation = 0x0000,
    Gyro = 0x0100,
    Goen220 = 0x0200,
    Tv = 0x0300,
    Ir = 0x0400
};
Q_ENUM_NS(Dev)

/// @brief Режимы работы ЛЦД
enum class LFR_MODE : quint16 {
    BLOCK = 0x00,
    SINGLE_DIST = 0xAA55,
    CONTINUOUS = 0xB5AD,
    SINGLE_TARGET = 0x9EED
};

Q_ENUM_NS(LFR_MODE)

/// @brief Фиксированные угловые положения гироплатформы
enum class GYRO_DEF_POS : quint8 { ZERO = 0xA1, HIKE = 0xA2, PILOTING = 0xA3, PARKING = 0xA4 };
Q_ENUM_NS(GYRO_DEF_POS)

/// @brief Идентификаторы целей трекре
enum class TR_ID : quint8 {_0 = 0, _1 = 1, _2 = 2, _3 = 3};
Q_ENUM_NS(TR_ID)

/// @brief Вспомогательный enum для типа BOOL
enum class BOOL_V : quint8 {OFF = 1, ON = 2};
Q_ENUM_NS(BOOL_V)

/// @brief Специальный boolean для параметров вида On Off
struct BOOL {
    BOOL () = default;
    BOOL (BOOL_V v) : _v(v) {}
    BOOL (bool s) : _v(s ? BOOL_V::ON : BOOL_V::OFF) {}
    BOOL (quint8 s) : _v((s == (quint8)BOOL_V::ON) ? BOOL_V::ON : BOOL_V::OFF) {}
    BOOL (int s) : _v((s == (int)BOOL_V::ON) ? BOOL_V::ON : BOOL_V::OFF) {}
    operator bool() const {return (_v == BOOL_V::ON);}
    operator quint8() const {return (quint8)_v;}
    operator BOOL_V() const {return _v;}

    BOOL_V _v = BOOL_V::OFF;
};

/// @brief Режимы гамма-коррекции Evcam
enum class EVCAM_GAMMA : quint8 {STANDARD = 1, STRAIT = 2};
Q_ENUM_NS(EVCAM_GAMMA)

/// @brief Стабилизация EVCAM
enum class EVCAM_STABILIZER : quint8 {OFF = 1, ON = 2, HOLD = 3};
Q_ENUM_NS(EVCAM_STABILIZER)

/// @brief Режим стабилизации EVCAM
enum class EVCAM_STABILIZER_LEVEL : quint8 {SUPER = 2, SUPER_PLUS = 3};
Q_ENUM_NS(EVCAM_STABILIZER_LEVEL)

/// @brief EVCAM HDR уровень яркости
enum class EVCAM_HDR_BRTNSS : quint8 { _0 = 0, _1, _2, _3, _4, _5, _6 };
Q_ENUM_NS(EVCAM_HDR_BRTNSS)

/// @brief EVCAM HDR уровень компенсации яркости
enum class EVCAM_HDR_BRTNSS_COMP_LVL : quint8 { _0 = 0, _1, _2, _3 };
Q_ENUM_NS(EVCAM_HDR_BRTNSS_COMP_LVL)

/// @brief EVCAM HDR уровень компенсации
enum class EVCAM_HDR_COMP_LVL : quint8 { _0 = 0, _1, _2 };
Q_ENUM_NS(EVCAM_HDR_COMP_LVL)

/// @brief EVCAM уровень шумоподавления
enum class EVCAM_NR_LVL : quint8 {OFF = 0, _1, _2, _3, _4, _5, _2D_3D_INDP = 0x7F};
Q_ENUM_NS(EVCAM_NR_LVL)

/// @brief Режим фокусировки EVCAM
enum class EVCAM_FOCUS_MODE : quint8 {MANUAL = 1, AUTO = 2};
Q_ENUM_NS(EVCAM_FOCUS_MODE)

/// @brief Режим поворота изображнения ТПВ Mini640
enum class MINI640_FPA_CONF : quint8 {_0 = 0, _90 = 1, _180 = 2, _270 = 3 };
Q_ENUM_NS(MINI640_FPA_CONF)

/// @brief Команды управления ГОЭН-220
enum class Cmd {

    CH_ENABLE = 0x00 + (int) Dev::Goen220,
    CH_MAN_ZOOM_IN = 0x03 + (int) Dev::Goen220,
    CH_MAN_ZOOM_OUT = 0x04 + (int) Dev::Goen220,
    CH_MAN_ZOOM_STOP = 0x05 + (int) Dev::Goen220,
    CH_MAN_ZOOM_POSITION = 0x1A + (int) Dev::Goen220,
    CH_MAN_FOCUS_FAR = 0x06 + (int) Dev::Goen220,
    CH_MAN_FOCUS_NEAR = 0x07 + (int) Dev::Goen220,
    CH_MAN_FOCUS_STOP = 0x08 + (int) Dev::Goen220,
    CH_MAN_FOCUS_POSITION = 0x1B + (int) Dev::Goen220,
    CH_FIELD_SYNC = 0x0C + (int) Dev::Goen220,
    CH_REGULAR_FSYNC = 0x0D + (int) Dev::Goen220,
    CH_SYNC_NARROW_FIELDS = 0x0E + (int) Dev::Goen220,

    LFR_RADIATION = 0x11 + (int) Dev::Goen220,
    LFR_POWER_ON = 0x12 + (int) Dev::Goen220,
    LFR_FREQ = 0x13 + (int) Dev::Goen220,

    GYRO_ARR = 0x20 + (int) Dev::Gyro,
    GYRO_STAB_EN = 0x21 + (int) Dev::Gyro,
    GYRO_POS = 0x23 + (int) Dev::Gyro,
    GYRO_SPEED_DEG = 0xA7 + (int) Dev::Gyro,
    GYRO_RELATIVE_ANGLE = 0x27 + (int) Dev::Gyro,
    GYRO_ABS_ANGLE_DEG = 0xA2 + (int) Dev::Gyro,

    TR_EN_SINGLE = 0x30 + (int) Dev::Goen220,
    TR_DIS_SINGLE = 0x31 + (int) Dev::Goen220,
    TR_CHS = 0x32 + (int) Dev::Goen220,
    TR_EN_4 = 0x33 + (int) Dev::Goen220,
    TR_DIS = 0x34 + (int) Dev::Goen220,

    SRV_ALIGNMENT = 0xA0,

    EVCAM_FlickerReduction = 0x00 + (int) Dev::Tv,
    EVCAM_BackLight = 0x01 + (int) Dev::Tv,
    EVCAM_Gamma = 0x02 + (int) Dev::Tv,
    EVCAM_GammaOffset = 0x03 + (int) Dev::Tv,
    EVCAM_ExExpComp_Reset = 0x04 + (int) Dev::Tv,
    EVCAM_ExExpComp_UP = 0x05 + (int) Dev::Tv,
    EVCAM_ExExpComp_DOWN = 0x06 + (int) Dev::Tv,
    EVCAM_ExExpComp_DIRECT = 0x07 + (int) Dev::Tv,
    EVCAM_Stabilizer = 0x08 + (int) Dev::Tv,
    EVCAM_Stabilizer_Level = 0x09 + (int) Dev::Tv,
    EVCAM_HighSensitivity = 0x0A + (int) Dev::Tv,
    EVCAM_Defog = 0x0B + (int) Dev::Tv,
    EVCAM_WideD = 0x0C + (int) Dev::Tv,
    EVCAM_WideD_SetParameter = 0x0D + (int) Dev::Tv,
    EVCAM_NR = 0x0E + (int) Dev::Tv,
    EVCAM_2NR_3NR = 0x0F + (int) Dev::Tv,
    EVCAM_HLC = 0x10 + (int) Dev::Tv,
    EVCAM_ICR = 0x11 + (int) Dev::Tv,
    EVCAM_AutoICR = 0x12 + (int) Dev::Tv,
    EVCAM_AutoICR_Threshold = 0x14 + (int) Dev::Tv,
    EVCAM_MinShutter = 0x15 + (int) Dev::Tv,
    EVCAM_MinShutter_Limit = 0x16 + (int) Dev::Tv,
    EVCAM_PictureFlip = 0x17 + (int) Dev::Tv,
    EVCAM_AutoFocus = 0x18 + (int) Dev::Tv,

    Mini640_INT_PERIOD_TIME = 0x00 + (int) Dev::Ir,
    Mini640_INT_HIGH_LEVEL = 0x01 + (int) Dev::Ir,
    Mini640_FPA_CONF = 0x02 + (int) Dev::Ir,
    Mini640_COOLER_EN = 0x50 + (int) Dev::Ir
};

Q_ENUM_NS(Cmd)

/// @brief  Код подвтерждения команды
enum class CmdAck : quint8 {
    Ok = 0x80 | 0x00,            ///< Команда выполнена успешно
    Communication = 0x80 | 0x01, ///< Ошибка коммуникации (доставки команды)
    BufferOveflow = 0x80 | 0x02, ///< Переполнение буфера
    InvalidDev = 0x80 | 0x03,    ///< Несуществующее устройство
    InvalidCmd = 0x80 | 0x04,    ///< Несуществующая команда
    // ....
    InvalidParam = 0x80 | 0x10, ///< Неверные парамтры команды
    // ....
    Generic = 0x80 | 0x7E, ///< Ошибка общего вида (неопределенной специализации)
    DbgStub = 0x80 | 0x7F  ///< отладочная "заглушка"
};

Q_ENUM_NS(CmdAck)

} // namespace Goen220

