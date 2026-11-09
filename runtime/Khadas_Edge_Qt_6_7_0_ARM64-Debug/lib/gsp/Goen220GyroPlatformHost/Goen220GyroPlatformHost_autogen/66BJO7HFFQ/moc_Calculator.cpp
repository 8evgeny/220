/****************************************************************************
** Meta object code from reading C++ file 'Calculator.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../src/lib/gsp/Goen220GyroPlatformHost/src/NDevices/Calculator.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Calculator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSCalculatorENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCalculatorENDCLASS = QtMocHelpers::stringData(
    "Calculator",
    "getHostId",
    "uint8_t",
    "",
    "setHostId",
    "hostId",
    "getDeviceId",
    "setDeviceId",
    "deviceId",
    "setMode",
    "Mode",
    "mode",
    "getVersion",
    "getStatus",
    "setStatus",
    "setVersion",
    "uint32_t",
    "versionProtocolVdb",
    "versionProtocol",
    "versionDevice",
    "setMemsMode",
    "setErrorAllMems",
    "error",
    "setMemsNotConfigured",
    "flags",
    "setMemsIllegalData",
    "setMemsNoResponseOnSpi",
    "setMemsReserve",
    "setMemsAxisSwitchMode",
    "setMemsProcessMode",
    "setMemsBoardStatusCommon",
    "errorAllMems",
    "memsNotConfiguredFlags",
    "memsIllegalDataFlags",
    "memsNoResponseOnSpiFlags",
    "reserveFlags",
    "memsAxisSwitchMode",
    "memsProcessMode",
    "setMemsBoardStatusSpeedAndAngle",
    "speedYaw",
    "speedPitch",
    "angleYaw",
    "anglePitch",
    "setMotorYawMode",
    "setMotorYawErrors",
    "setMotorYawFlags",
    "setMotorYawAngle",
    "angle",
    "setMotorYawSpeed",
    "speed",
    "setYawMotorStatus",
    "errors",
    "setMotorPitchMode",
    "setMotorPitchErrors",
    "setMotorPitchFlags",
    "setMotorPitchAngle",
    "setMotorPitchSpeed",
    "setPitchMotorStatus",
    "setSpeedYaw",
    "setSpeedPitch",
    "setAngleYaw",
    "setAnglePitch",
    "setControlPosition",
    "yaw",
    "pitch",
    "setControlSpeed",
    "setControlTracking",
    "yawMisalignment",
    "pitchMisalignment",
    "yawAccelLimit",
    "pitchAccelLimit",
    "setUnknownCommand",
    "dstFunction",
    "setStatusWithErrorCommand",
    "errorCode",
    "setAirConditionerCmd",
    "fanSpeed",
    "heaterOn",
    "int8_t",
    "peltierI",
    "setAirConditionerState",
    "t1",
    "t2",
    "t3"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCalculatorENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      42,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  266,    3, 0x10a,    1 /* Public | MethodIsConst  */,
       4,    1,  267,    3, 0x0a,    2 /* Public */,
       6,    0,  270,    3, 0x10a,    4 /* Public | MethodIsConst  */,
       7,    1,  271,    3, 0x0a,    5 /* Public */,
       9,    1,  274,    3, 0x0a,    7 /* Public */,
      12,    0,  277,    3, 0x0a,    9 /* Public */,
      13,    0,  278,    3, 0x0a,   10 /* Public */,
      14,    1,  279,    3, 0x0a,   11 /* Public */,
      15,    3,  282,    3, 0x0a,   13 /* Public */,
      20,    1,  289,    3, 0x0a,   17 /* Public */,
      21,    1,  292,    3, 0x0a,   19 /* Public */,
      23,    1,  295,    3, 0x0a,   21 /* Public */,
      25,    1,  298,    3, 0x0a,   23 /* Public */,
      26,    1,  301,    3, 0x0a,   25 /* Public */,
      27,    1,  304,    3, 0x0a,   27 /* Public */,
      28,    1,  307,    3, 0x0a,   29 /* Public */,
      29,    1,  310,    3, 0x0a,   31 /* Public */,
      30,    8,  313,    3, 0x0a,   33 /* Public */,
      38,    4,  330,    3, 0x0a,   42 /* Public */,
      43,    1,  339,    3, 0x0a,   47 /* Public */,
      44,    1,  342,    3, 0x0a,   49 /* Public */,
      45,    1,  345,    3, 0x0a,   51 /* Public */,
      46,    1,  348,    3, 0x0a,   53 /* Public */,
      48,    1,  351,    3, 0x0a,   55 /* Public */,
      50,    5,  354,    3, 0x0a,   57 /* Public */,
      52,    1,  365,    3, 0x0a,   63 /* Public */,
      53,    1,  368,    3, 0x0a,   65 /* Public */,
      54,    1,  371,    3, 0x0a,   67 /* Public */,
      55,    1,  374,    3, 0x0a,   69 /* Public */,
      56,    1,  377,    3, 0x0a,   71 /* Public */,
      57,    5,  380,    3, 0x0a,   73 /* Public */,
      58,    1,  391,    3, 0x0a,   79 /* Public */,
      59,    1,  394,    3, 0x0a,   81 /* Public */,
      60,    1,  397,    3, 0x0a,   83 /* Public */,
      61,    1,  400,    3, 0x0a,   85 /* Public */,
      62,    2,  403,    3, 0x0a,   87 /* Public */,
      65,    2,  408,    3, 0x0a,   90 /* Public */,
      66,    4,  413,    3, 0x0a,   93 /* Public */,
      71,    1,  422,    3, 0x0a,   98 /* Public */,
      73,    1,  425,    3, 0x0a,  100 /* Public */,
      75,    3,  428,    3, 0x0a,  102 /* Public */,
      80,    7,  435,    3, 0x0a,  106 /* Public */,

 // slots: parameters
    0x80000000 | 2,
    QMetaType::Void, 0x80000000 | 2,    5,
    0x80000000 | 2,
    QMetaType::Void, 0x80000000 | 2,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 2,   11,
    QMetaType::Void, 0x80000000 | 16, 0x80000000 | 16, 0x80000000 | 16,   17,   18,   19,
    QMetaType::Void, 0x80000000 | 2,   11,
    QMetaType::Void, QMetaType::Bool,   22,
    QMetaType::Void, 0x80000000 | 2,   24,
    QMetaType::Void, 0x80000000 | 2,   24,
    QMetaType::Void, 0x80000000 | 2,   24,
    QMetaType::Void, 0x80000000 | 2,   24,
    QMetaType::Void, 0x80000000 | 2,   11,
    QMetaType::Void, 0x80000000 | 2,   11,
    QMetaType::Void, 0x80000000 | 2, QMetaType::Bool, 0x80000000 | 2, 0x80000000 | 2, 0x80000000 | 2, 0x80000000 | 2, 0x80000000 | 2, 0x80000000 | 2,   11,   31,   32,   33,   34,   35,   36,   37,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   39,   40,   41,   42,
    QMetaType::Void, 0x80000000 | 2,   11,
    QMetaType::Void, 0x80000000 | 2,   24,
    QMetaType::Void, 0x80000000 | 2,   24,
    QMetaType::Void, QMetaType::Float,   47,
    QMetaType::Void, QMetaType::Float,   49,
    QMetaType::Void, 0x80000000 | 2, 0x80000000 | 2, 0x80000000 | 2, QMetaType::Float, QMetaType::Float,   11,   51,   24,   47,   49,
    QMetaType::Void, 0x80000000 | 2,   11,
    QMetaType::Void, 0x80000000 | 2,   24,
    QMetaType::Void, 0x80000000 | 2,   24,
    QMetaType::Void, QMetaType::Float,   47,
    QMetaType::Void, QMetaType::Float,   49,
    QMetaType::Void, 0x80000000 | 2, 0x80000000 | 2, 0x80000000 | 2, QMetaType::Float, QMetaType::Float,   11,   51,   24,   47,   49,
    QMetaType::Void, QMetaType::Float,   49,
    QMetaType::Void, QMetaType::Float,   49,
    QMetaType::Void, QMetaType::Float,   47,
    QMetaType::Void, QMetaType::Float,   47,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   63,   64,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   63,   64,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   67,   68,   69,   70,
    QMetaType::Void, 0x80000000 | 2,   72,
    QMetaType::Void, 0x80000000 | 2,   74,
    QMetaType::Void, 0x80000000 | 2, 0x80000000 | 2, 0x80000000 | 78,   76,   77,   79,
    QMetaType::Void, 0x80000000 | 2, 0x80000000 | 2, 0x80000000 | 78, 0x80000000 | 78, 0x80000000 | 78, 0x80000000 | 78, 0x80000000 | 16,   76,   77,   79,   81,   82,   83,   51,

       0        // eod
};

Q_CONSTINIT const QMetaObject Calculator::staticMetaObject = { {
    QMetaObject::SuperData::link<CommandExecutor::staticMetaObject>(),
    qt_meta_stringdata_CLASSCalculatorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCalculatorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCalculatorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Calculator, std::true_type>,
        // method 'getHostId'
        QtPrivate::TypeAndForceComplete<uint8_t, std::false_type>,
        // method 'setHostId'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'getDeviceId'
        QtPrivate::TypeAndForceComplete<uint8_t, std::false_type>,
        // method 'setDeviceId'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Mode, std::false_type>,
        // method 'getVersion'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'getStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setVersion'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        // method 'setMemsMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setErrorAllMems'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'setMemsNotConfigured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMemsIllegalData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMemsNoResponseOnSpi'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMemsReserve'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMemsAxisSwitchMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMemsProcessMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMemsBoardStatusCommon'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMemsBoardStatusSpeedAndAngle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setMotorYawMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMotorYawErrors'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMotorYawFlags'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMotorYawAngle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setMotorYawSpeed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setYawMotorStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setMotorPitchMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMotorPitchErrors'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMotorPitchFlags'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setMotorPitchAngle'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setMotorPitchSpeed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setPitchMotorStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setSpeedYaw'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setSpeedPitch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setAngleYaw'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setAnglePitch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setControlPosition'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setControlSpeed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setControlTracking'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'setUnknownCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setStatusWithErrorCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setAirConditionerCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        // method 'setAirConditionerState'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>
    >,
    nullptr
} };

void Calculator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Calculator *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { uint8_t _r = _t->getHostId();
            if (_a[0]) *reinterpret_cast< uint8_t*>(_a[0]) = std::move(_r); }  break;
        case 1: _t->setHostId((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 2: { uint8_t _r = _t->getDeviceId();
            if (_a[0]) *reinterpret_cast< uint8_t*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->setDeviceId((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 4: _t->setMode((*reinterpret_cast< std::add_pointer_t<Mode>>(_a[1]))); break;
        case 5: _t->getVersion(); break;
        case 6: _t->getStatus(); break;
        case 7: _t->setStatus((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 8: _t->setVersion((*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[3]))); break;
        case 9: _t->setMemsMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 10: _t->setErrorAllMems((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->setMemsNotConfigured((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 12: _t->setMemsIllegalData((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 13: _t->setMemsNoResponseOnSpi((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 14: _t->setMemsReserve((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 15: _t->setMemsAxisSwitchMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 16: _t->setMemsProcessMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 17: _t->setMemsBoardStatusCommon((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[8]))); break;
        case 18: _t->setMemsBoardStatusSpeedAndAngle((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4]))); break;
        case 19: _t->setMotorYawMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 20: _t->setMotorYawErrors((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 21: _t->setMotorYawFlags((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 22: _t->setMotorYawAngle((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 23: _t->setMotorYawSpeed((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 24: _t->setYawMotorStatus((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[5]))); break;
        case 25: _t->setMotorPitchMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 26: _t->setMotorPitchErrors((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 27: _t->setMotorPitchFlags((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 28: _t->setMotorPitchAngle((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 29: _t->setMotorPitchSpeed((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 30: _t->setPitchMotorStatus((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[5]))); break;
        case 31: _t->setSpeedYaw((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 32: _t->setSpeedPitch((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 33: _t->setAngleYaw((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 34: _t->setAnglePitch((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 35: _t->setControlPosition((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 36: _t->setControlSpeed((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 37: _t->setControlTracking((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4]))); break;
        case 38: _t->setUnknownCommand((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 39: _t->setStatusWithErrorCommand((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 40: _t->setAirConditionerCmd((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[3]))); break;
        case 41: _t->setAirConditionerState((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[7]))); break;
        default: ;
        }
    }
}

const QMetaObject *Calculator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Calculator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCalculatorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return CommandExecutor::qt_metacast(_clname);
}

int Calculator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CommandExecutor::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 42)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 42;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 42)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 42;
    }
    return _id;
}
QT_WARNING_POP
