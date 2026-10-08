/*
 *  Created on: Feb 10, 2025
 *  Author: user
 */
#ifndef CONFIGURARTION_GLOBALTYPES_HPP_
#define CONFIGURARTION_GLOBALTYPES_HPP_
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#include <cstdint>
#ifdef QT_CORE_LIB
#include <QObject>
#include <QMetaEnum>
#define REGISTRATE_ENUM(TypeEnum) Q_ENUM(TypeEnum)
#else
#define REGISTRATE_ENUM(TypeEnum)
#endif
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#ifdef QT_CORE_LIB
class GDeviceTypes
{
    Q_GADGET
    GDeviceTypes() = delete;
public:
#else
namespace GTypes
{
#endif

   //-------------------------------------------------------------------------------------------------
   /** @brief Режимы работы стабилизатора */
   enum class BoardType : uint8_t
   {
     /* Пустышка */
     none = 0x00,
     /* Управляющая плата с массивом из 8 MEMS датчиков */
     memsArray = 0xA1,
     /* Плата управления мотором оси курса */
     servoMotorYaw = 0xA2,
     /* Плата управления мотором оси тангажа */
     servoMotorPitch = 0xA3,
   };
    REGISTRATE_ENUM(BoardType);

   /**
   *  @brief Режимы работы стабилизатора
   */
   enum class OperatingModeStabilizer : uint8_t
   {
     /* Режим стабилизации */
     stabilization  = 0xC1,
     /* Режим поворотной платформы */
     rotaryPlatform = 0xC2,
     /* Режим парковки */
     parking        = 0xC3,
     /* Выключеные моторы */
     off            = 0xC4,
   };
   REGISTRATE_ENUM(OperatingModeStabilizer);
   /**
   *  @brief Режимы пересчета измерительной оси для стабилизации вокруг оси курса
   */
   enum class StabilizerAxisSwitchMode : uint8_t
   {
       /* Простой режим */
       simpleMode  = 0x01,
       /* Сложный режим */
       complexMode = 0x02
   };
   REGISTRATE_ENUM(StabilizerAxisSwitchMode);
   /**
   *  @brief Режимы обработки данных с 8 датчиков
   */
   enum class StabilizerMemsProcessingMode : uint8_t
   {
       /* Режим осреднения */
       averagingMode = 0x01,
       /* Режим фильтрации */
       filteringMode = 0x02
   };
   REGISTRATE_ENUM(StabilizerMemsProcessingMode);

   /** @brief Режимы работы мотора  */
   enum class OperatingModeMotor : int8_t
   {
     /* Переходное состояние */
     dummy = 0,
     /* Выключен мотор */
     off,
     /* Управление по положению */
     position,
     /* Управление по мкорости */
     speed,
     /* Управление по мгновенной скорости от стабилизатора */
     instSpeed,
     /* Управление по значению момента */
     direct,
     /* Управление шаговое */
     steper,
     /* Автоматическая настройка */
     StateSearchingMotorZeroOffset,
     StateMotorMagnetCalibration,
     StateSearchingPoleQuantity
   };
   REGISTRATE_ENUM(OperatingModeMotor);

   /** @brief Оси гироскопа */
   enum GyroscopeAxes : uint8_t
   {
       /* Ось X */
     gyroAxisX = 0,
       /* Ось Y */
     gyroAxisY = 1,
       /* Ось Z */
     gyroAxisZ = 2,
     /* Количество осей гироскопа */
     gyroAxesNumber
   };
   REGISTRATE_ENUM(GyroscopeAxes);

   /** @brief Физические оси стабилизации */
   enum StabilizationAxes : uint8_t
   {
       /* Ось рыскания (== Z) */
       yaw    = 0x00,
       /* Ось тангажа  (== X) */
       pitch  = 0x01,
       /* Точная ось курса */
//       yawAccurate    = 0x02,
       /* Точная ось тангажа */
       // pitchAccurate  = 0x03*/
   };
   REGISTRATE_ENUM(StabilizationAxes);

   /** @brief Обозначение типов регуляторов */
   enum class RegulatorType : uint8_t
   {
       /* Регулятор управления по положению в режими стабилизации */
       stabilizationPosition = 0x01,
       /* Регулятор управления угламм рассогласования от трекера в режими стабилизации */
       stabilizationTracking,
       /* Регулятор управления по скорости в режими стабилизации */
       instSpeed,
       /* Регулятор управления по положению в режими поворотной платформы */
       servoPosition,
       /* Регулятор управления по положению в режими поворотной платформы */
       servoSpeed,
   };
   REGISTRATE_ENUM(RegulatorType);

   /** @brief Обозначение типов автоматической настройки мотора */
   enum class MotorAutoconfigurationType : uint8_t
   {
       /* Поиск кол-ва пар полюсов мотора */
       searchingPoleQuantity = 0x01,
       /* Поиск смещения нулевого полюса мотора относительно нуля датчика угла */
       searchingMotorZeroOffset,
       /* Компенсация влияний конструкции на магнитного поля измерительного магнита */
       searchingAngleSensorCalibration,
   };
   REGISTRATE_ENUM(MotorAutoconfigurationType);

   /** @brief Обозначение границ движения мотора */
   enum class BoundOrder : uint8_t
   {
       /* Верхняя граница */
       top  = 0x01,
       /* Нижняя граница */
       down,
   };
   REGISTRATE_ENUM(BoundOrder);

   enum Direction : int8_t
   {
     direct = 1,
     invert = -1,
   };
   REGISTRATE_ENUM(Direction);

   /**
    * @brief Обозначение интерфейса передачи
    */
   enum class ConnectionInterface : int8_t
   {
       InterfaceUART     = 1,
       InterfaceCAN      = 2,
       InterfaceEthernet = 3,
   };
   REGISTRATE_ENUM(ConnectionInterface);
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#ifdef QT_CORE_LIB
template<typename QEnum>
QString QtEnumToQString(const QEnum value)
{
    return QVariant::fromValue(value).toString();
    // return QString(QMetaEnum::fromType<QEnum>().valueToKey(value));
}
#endif
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#endif /* CONFIGURARTION_GLOBALTYPES_HPP_ */
