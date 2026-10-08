/*
 *  Created on: Feb 10, 2025
 *  Author: user
 */
#ifndef CONFIGURARTION_GLOBALSTRUCT_HPP_
#define CONFIGURARTION_GLOBALSTRUCT_HPP_
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#include "GlobalTypes.hpp"
#include <cstdint>
#ifdef QT_CORE_LIB
#include <QObject>
#endif
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#ifdef QT_CORE_LIB
class GDeviceStruct
{
    Q_GADGET
    GDeviceStruct() = delete;
    using GStruct = GDeviceStruct;
    using GTypes = GDeviceTypes;
public:
#else
namespace GStruct
{
#endif

#pragma pack(push,1)
    /**
     * @brief Motors
     */
    union MotorErrors
    {
      struct
      {
          /** @brief motorOvercurrent - превышение допустимого тока драйвера мотора, [-] */
          uint8_t motorOvercurrent    : 1;
          /** @brief motorOverheat - перегрев драйвера мотора, [-] */
          uint8_t motorOverheat       : 1;
          /** @brief asRotationOverSpeed - превышение разрешенной для датчика угла скорости вращения, [-] */
          uint8_t asRotationOverSpeed : 1;
          /** @brief asWeakMagneticField - неустойчивое магнитное поле датчика угла, [-] */
          uint8_t asWeakMagneticField : 1;
          /** @brief asUnderVoltage - напряжение питания датчика угла вне диапазона, [-] */
          uint8_t asUnderVoltage      : 1;
      };
      uint8_t raw;
    };

    union MotorFlags
    {
        struct
        {
          /** @brief error - наличие ошибок в работе устройства, [-] */
          uint8_t error             : 1;
          /** @brief boundingEnable - включён режим ограничения сектора вращения, [-] */
          uint8_t boundingEnable    : 1;
          /** @brief calibrationEnable - включена калибровка, [-] */
          uint8_t calibrationEnable : 1;
          /** @brief isNonSavedData - есть несохраненные данные, [-] */
          uint8_t isNonSavedData    : 1;
        };
        uint8_t raw;
    };

    struct MotorStatus
    {
      /** @mode - GTypes::OperatingModeMotor */
      uint8_t mode;
      /** errors - MotorErrors */
      uint8_t errors;
      /** flags - MotorFlags */
      uint8_t flags;
      float angle;
      float speed;
    };

    /** Статус калибровки получаемый через CAN  */
    struct MotorCalibrationInfo
    {
        /** @brief type - тип калибровки, [GDeviceTypes::MotorCalibrationType] */
        uint8_t type;
        /** @brief status - статус калибровки, [GDeviceTypes::MotorCalibrationStatus] */
        uint8_t status;
        /** @brief data - данные калибровки, [-] */
        float data;
        /** @brief pwmA - усилие на обмотке A, [отн.ед] */
        float pwmA;
        /** @brief pwmB - усилие на обмотке B, [отн.ед] */
        float pwmB;
        /** @brief pwmC - усилие на обмотке C, [отн.ед] */
        float pwmC;
    };
    union MemsErrors {
        uint8_t _raw;
        struct {
            uint8_t AllMemsDead : 1;
            uint8_t reserve     : 7;
        } err;
    };
    struct MemsFlags {
        /** Мемс-дачтик не сконфигурировался при запуске и настройке */
        union {
            uint8_t _raw;
            struct {
                uint8_t Mems1 : 1;
                uint8_t Mems2 : 1;
                uint8_t Mems3 : 1;
                uint8_t Mems4 : 1;
                uint8_t Mems5 : 1;
                uint8_t Mems6 : 1;
                uint8_t Mems7 : 1;
                uint8_t Mems8 : 1;
            };
        } MemsNotConfig;
        /** Мемс-датчик шлет повторяющиеся или битые данные */
        union {
            uint8_t _raw{0x00};
            struct {
                uint8_t Mems1 : 1;
                uint8_t Mems2 : 1;
                uint8_t Mems3 : 1;
                uint8_t Mems4 : 1;
                uint8_t Mems5 : 1;
                uint8_t Mems6 : 1;
                uint8_t Mems7 : 1;
                uint8_t Mems8 : 1;
            };
        } MemsBadData;
        /** Мемс-датчик не отвечает по SPI */
        union {
            uint8_t _raw;
            struct {
                uint8_t Mems1 : 1;
                uint8_t Mems2 : 1;
                uint8_t Mems3 : 1;
                uint8_t Mems4 : 1;
                uint8_t Mems5 : 1;
                uint8_t Mems6 : 1;
                uint8_t Mems7 : 1;
                uint8_t Mems8 : 1;
            };
        } MemsNoSpiAns;
        /** От мемс-датчика не приходит DRI, опрос происходит по таймеру */
        union {
            uint8_t _raw;
            struct {
                uint8_t noIntMode    : 1;
                uint8_t nonSavedData : 1;
                uint8_t reserve      : 6;
            };
        } NoIntMode;
    };

    struct MemsBoardStatus {
        GTypes::OperatingModeStabilizer mode;
        MemsErrors 					errors;
        MemsFlags 					flags;
        uint8_t 						AxisSwitchMode{0};
        uint8_t 						MemsProcessMode{0};
        float 							speedYaw{0};
        float 							speedPitch{0};
        float 							angleYaw{0};
		float 							    anglePitch{0};
    };

    struct TelemetryUART
    {
      GTypes::OperatingModeStabilizer mode;
      MemsBoardStatus mems;
      MotorStatus motorYaw;
      MotorStatus motorPitch;
    };

#pragma pack(pop)
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#endif /* CONFIGURARTION_GLOBALSTRUCT_HPP_ */
