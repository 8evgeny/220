#pragma once

#include <QtGlobal>

#include <goen220_utils.h>

namespace Goen220 {

#pragma pack(push, 1)

/// @brief Структура данных телеметрической информаци
struct Tlm
{
    /// @brief Структура данных телеметрической информации канала
    struct Chan {
        quint8  SENSOR_STATUS;
        quint8  LENS_STATUS;
        quint8  FPS;
        quint16 VRES;
        quint16 HRES;
        quint16 EXP;
        quint16 ZOOM_DRIVE;
        quint16 FOCUS_DRIVE;
        qint8   LENS_TEMP;
        float   FOCUS_MM;
        float   FIELD_GRAD;
    };

    /// @brief Структура данных телеметрической информации цели трекинга
    struct Target {
        quint8  STATE;
        quint16 X;
        quint16 Y;
        quint16 X_SZ;
        quint16 YS_Z;
        float   A_Z;
        float   A_X;
        float   A_Z_SZ;
        float   A_X_SZ;
    };

    /// @brief Структура данных телеметрической информации ЛД с ЛЦУ
    struct Rf {
        quint8 STATUS;
        quint8 MEAS_STATE;
        float  LAST_DISTANCE;
        float  AVG_DISTANCE;
        float  RMS_DISTANCE;
        quint16 STATE;
        quint8 rsrvd[2];
        float BDT;
    };


    /// @brief Структура данных телеметрической информации ГСП
    struct Gsp {
        quint8 STATUS;
        quint16 MODE;
        quint32 POS_Y;
        quint32 POS_Z;
        float ANGLE_Y;
        float ANGLE_Z;
        float SPEED_Y;
        float SPEED_Z;
    };

    /// @brief Структура данных телеметрической информации МУ
    struct Ctrl {
        quint8 AF_STATE;
        quint8 REGULAR_FIELD_SYNC_STATE;
        quint8 TRACK_STATE;
        quint8 TARGET_STATE;
        qint16 TRK_STR_X;
        qint16 TRK_STR_Y;
        qint16 TRK_STR_XSZ;
        qint16 TRK_STR_YSZ;
        float TARGET_Z;
        float TARGET_X;
        float TARGET_Z_SZ;
        float TARGET_X_SZ;
        quint8 rsrvd[2];
        quint8 ETH_STATUS;
    };

    /// @brief Структура данных телеметрической сервисной информации
    struct Srvc {
        quint8 ID_YEAR;
        quint8 ID_MONTH;
        quint8 ID_ID;
        quint8 ID_HARD_VER;
        quint8 ID_SOFT_VER;
        quint16 ID_HOURS;
        quint8 ID_MINUTES;
        quint8 rsrvd[14];
        quint32 ALIGNMENT_POS_Y;
        quint32 ALIGNMENT_POS_Z;
    };

    /// @brief Структура данных телеметрической информации автосопровождения
    struct Trk {
        quint8 TRACKER_CHANNEL;
        quint8 TARGET_ID;
        Target TARGET[4];
    };

    /// @brief Структура данных расщиренной телеметрической информации ТВ камеры
    struct Evcam {
        quint8 FlickerReduction;
        quint8 BackLight;
        quint8 Gamma;
        qint8  GammaOffset;
        quint8 ExExpComp_Pos;
        quint8 Stabilizer;
        quint8 Stabilizer_Level;
        quint8 HighSensitivity;
        quint8 Defog;
        quint8 WideD;
        quint8 WideD_Parameter[3];
        quint8 NR;
        quint8 _2NR_3NR[2];
        quint8 HLC;
        quint8 ICR;
        quint8 AutoICR;
        quint8 AutoICR_Threshold;
        quint8 PictureFlip;
        quint8 Autofocus;
        quint8 MinShutter;
        quint8 MinShutter_Limit;
    };

    /// @brief Структура данных расщиренной телеметрической информации ТПВ камеры
    struct Mini640 {
        quint16 INT_PERIOD_TIME;
        quint16 INT_HIGH_LEVEL;
        quint8 FPA_CONF;
        quint8 PCBIT_TSTAT_PSTAT;
        quint16 MONITOR_REF25V;
        quint16 MONITOR_REF33V;
        quint16 MONITOR_REF5V_N4V;
        quint16 CCE_VERSIONSFT;
        quint16 CCE_VERSIONFPGA;
        quint8 COOLER_EN;
        quint32 COOLER_TIMER;
        qint16 IRSensT;
    };


    /// @brief Структура данных расщиренной телеметрической информации статусов ТВ/ТПВ камер
    struct ChExtSt
    {
        quint8 FCB_EV9500M[2];
        quint8 MINI640_IDDCA[2];
    };

    /// @brief Структура данных расщиренной телеметрической информации ЛДЦ-20И
    struct Ldc20 {
        quint8 EMIT_STATE_CODE;
        quint8 RAD_TC_STATE_CODE;
        quint8 LD_TC_STATE_CODE;
        float TEMP_POWER_SW;
        float TEMP_RAD;
        float TEMP_LD;
        float RAD_HEATER_CURR;
        float VOLTAGE;
        float TEM_LD_CURR;
        float TEM_LD_VOLTAGE_DROP;
        float TEMP_FD;
        quint16 Frequency_LRF;
    };

    //------------------------

    Chan TV;
    quint8 rsrvd_0[22]; // gap
    Chan MW;
    Rf RF;
    Gsp GSP;
    quint8  rsrvd_1[3]; // gap
    Ctrl CTRL;
    quint8 rsrvd_2[1]; // gap
    Srvc SRVC;
    Trk TRK;
    quint8 rsrvd_3 [11]; // gap
    Evcam EVCAM;
    Mini640 MINI640;
    quint8 rsrvd_4[5]; // gap
    ChExtSt CH_EXT_ST;
    Ldc20 LDC20;

    //---------------------------

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetTv(Args... args)
    {
        return Utils::offset(&Tlm::TV, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetMw(Args... args)
    {
        return Utils::offset(&Tlm::MW, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetRf(Args... args)
    {
        return Utils::offset(&Tlm::RF, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetGsp(Args... args)
    {
        return Utils::offset(&Tlm::GSP, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetCtrl(Args... args)
    {
        return Utils::offset(&Tlm::CTRL, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetSrvc(Args... args)
    {
        return Utils::offset(&Tlm::SRVC, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetTrk(Args... args)
    {
        return Utils::offset(&Tlm::TRK, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetTarget(size_t index, Args... args)
    {
        return Utils::offset(&Tlm::TRK, &Tlm::Trk::TARGET, index, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetEvcam(Args... args)
    {
        return Utils::offset(&Tlm::EVCAM, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetMini640(Args... args)
    {
        return Utils::offset(&Tlm::MINI640, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetChExtSt(Args... args)
    {
        return Utils::offset(&Tlm::CH_EXT_ST, args...);
    }

    template<typename... Args>
        requires Utils::CanCallOffset<Args...>
    static inline size_t offsetLdc20(Args... args)
    {
        return Utils::offset(&Tlm::LDC20, args...);
    }
};

#pragma pack(pop)



}
