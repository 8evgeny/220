/****************************************************************************
** Meta object code from reading C++ file 'CommandExecutor.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../src/lib/gsp/Goen220Vdb/src/NCommands/CommandExecutor.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CommandExecutor.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSCommandExecutorENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSCommandExecutorENDCLASS = QtMocHelpers::stringData(
    "CommandExecutor",
    "modeChanged",
    "",
    "Mode",
    "mode",
    "versionChanged",
    "uint32_t",
    "versionProtocolVdb",
    "versionProtocol",
    "versionDevice",
    "setModeCommandCompleted",
    "setControlPositionCommandCompleted",
    "setControlSpeedCommandCompleted",
    "setControlTrackingCommandCompleted",
    "memsModeChanged",
    "uint8_t",
    "errorAllMemsChanged",
    "errors",
    "memsNotConfiguredChanged",
    "flags",
    "memsIllegalDataChanged",
    "memsNoResponseOnSpiChanged",
    "memsReserveChanged",
    "memsAxisSwitchModeChanged",
    "memsProcessModeChanged",
    "memsBoardStatusCommonChanged",
    "errorAllMems",
    "memsNotConfiguredFlags",
    "memsIllegalDataFlags",
    "memsNoResponseOnSpiFlags",
    "reserveFlags",
    "memsAxisSwitchMode",
    "memsProcessMode",
    "memsBoardStatusSpeedAndAngleChanged",
    "speedYaw",
    "speedPitch",
    "angleYaw",
    "anglePitch",
    "motorYawModeChanged",
    "motorYawErrorsChanged",
    "motorYawFlagsChanged",
    "motorYawAngleChanged",
    "angle",
    "motorYawSpeedChanged",
    "speed",
    "yawMotorStatusChanged",
    "motorPitchModeChanged",
    "motorPitchErrorsChanged",
    "motorPitchFlagsChanged",
    "motorPitchAngleChanged",
    "motorPitchSpeedChanged",
    "pitchMotorStatusChanged",
    "speedYawChanged",
    "speedPitchChanged",
    "angleYawChanged",
    "anglePitchChanged",
    "controlPositionChanged",
    "yaw",
    "pitch",
    "controlSpeedChanged",
    "controlTrackingChanged",
    "yawMisalignment",
    "pitchMisalignment",
    "yawAccelLimit",
    "pitchAccelLimit",
    "unknownCommandCounterChanged",
    "int32_t",
    "unknownCommands",
    "statusWithErrorCommandCounterChanged",
    "statusWithErrorCommands",
    "airConditionerStateChanged",
    "fanSpeed",
    "heaterOn",
    "int8_t",
    "peltierI",
    "t1",
    "t2",
    "t3",
    "setAirConditionerCommandCompleted",
    "sendCmd",
    "Command*",
    "cmd",
    "getHostId",
    "setHostId",
    "hostId",
    "getDeviceId",
    "setDeviceId",
    "deviceId",
    "setMode",
    "getVersion",
    "getStatus",
    "setStatus",
    "setVersion",
    "setMemsMode",
    "setErrorAllMems",
    "error",
    "setMemsNotConfigured",
    "setMemsIllegalData",
    "setMemsNoResponseOnSpi",
    "setMemsReserve",
    "setMemsAxisSwitchMode",
    "setMemsProcessMode",
    "setMemsBoardStatusCommon",
    "setMemsBoardStatusSpeedAndAngle",
    "setMotorYawMode",
    "setMotorYawErrors",
    "setMotorYawFlags",
    "setMotorYawAngle",
    "setMotorYawSpeed",
    "setYawMotorStatus",
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
    "setControlSpeed",
    "setControlTracking",
    "setUnknownCommand",
    "dstFunction",
    "setStatusWithErrorCommand",
    "errorCode",
    "setAirConditionerCmd",
    "setAirConditionerState"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSCommandExecutorENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      82,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      40,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  506,    2, 0x06,    1 /* Public */,
       5,    3,  509,    2, 0x06,    3 /* Public */,
      10,    0,  516,    2, 0x06,    7 /* Public */,
      11,    0,  517,    2, 0x06,    8 /* Public */,
      12,    0,  518,    2, 0x06,    9 /* Public */,
      13,    0,  519,    2, 0x06,   10 /* Public */,
      14,    1,  520,    2, 0x06,   11 /* Public */,
      16,    1,  523,    2, 0x06,   13 /* Public */,
      18,    1,  526,    2, 0x06,   15 /* Public */,
      20,    1,  529,    2, 0x06,   17 /* Public */,
      21,    1,  532,    2, 0x06,   19 /* Public */,
      22,    1,  535,    2, 0x06,   21 /* Public */,
      23,    1,  538,    2, 0x06,   23 /* Public */,
      24,    1,  541,    2, 0x06,   25 /* Public */,
      25,    8,  544,    2, 0x06,   27 /* Public */,
      33,    4,  561,    2, 0x06,   36 /* Public */,
      38,    1,  570,    2, 0x06,   41 /* Public */,
      39,    1,  573,    2, 0x06,   43 /* Public */,
      40,    1,  576,    2, 0x06,   45 /* Public */,
      41,    1,  579,    2, 0x06,   47 /* Public */,
      43,    1,  582,    2, 0x06,   49 /* Public */,
      45,    5,  585,    2, 0x06,   51 /* Public */,
      46,    1,  596,    2, 0x06,   57 /* Public */,
      47,    1,  599,    2, 0x06,   59 /* Public */,
      48,    1,  602,    2, 0x06,   61 /* Public */,
      49,    1,  605,    2, 0x06,   63 /* Public */,
      50,    1,  608,    2, 0x06,   65 /* Public */,
      51,    5,  611,    2, 0x06,   67 /* Public */,
      52,    1,  622,    2, 0x06,   73 /* Public */,
      53,    1,  625,    2, 0x06,   75 /* Public */,
      54,    1,  628,    2, 0x06,   77 /* Public */,
      55,    1,  631,    2, 0x06,   79 /* Public */,
      56,    2,  634,    2, 0x06,   81 /* Public */,
      59,    2,  639,    2, 0x06,   84 /* Public */,
      60,    4,  644,    2, 0x06,   87 /* Public */,
      65,    1,  653,    2, 0x06,   92 /* Public */,
      68,    1,  656,    2, 0x06,   94 /* Public */,
      70,    7,  659,    2, 0x06,   96 /* Public */,
      78,    0,  674,    2, 0x06,  104 /* Public */,
      79,    1,  675,    2, 0x06,  105 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      82,    0,  678,    2, 0x10a,  107 /* Public | MethodIsConst  */,
      83,    1,  679,    2, 0x0a,  108 /* Public */,
      85,    0,  682,    2, 0x10a,  110 /* Public | MethodIsConst  */,
      86,    1,  683,    2, 0x0a,  111 /* Public */,
      88,    1,  686,    2, 0x0a,  113 /* Public */,
      89,    0,  689,    2, 0x0a,  115 /* Public */,
      90,    0,  690,    2, 0x0a,  116 /* Public */,
      91,    1,  691,    2, 0x0a,  117 /* Public */,
      92,    3,  694,    2, 0x0a,  119 /* Public */,
      93,    1,  701,    2, 0x0a,  123 /* Public */,
      94,    1,  704,    2, 0x0a,  125 /* Public */,
      96,    1,  707,    2, 0x0a,  127 /* Public */,
      97,    1,  710,    2, 0x0a,  129 /* Public */,
      98,    1,  713,    2, 0x0a,  131 /* Public */,
      99,    1,  716,    2, 0x0a,  133 /* Public */,
     100,    1,  719,    2, 0x0a,  135 /* Public */,
     101,    1,  722,    2, 0x0a,  137 /* Public */,
     102,    8,  725,    2, 0x0a,  139 /* Public */,
     103,    4,  742,    2, 0x0a,  148 /* Public */,
     104,    1,  751,    2, 0x0a,  153 /* Public */,
     105,    1,  754,    2, 0x0a,  155 /* Public */,
     106,    1,  757,    2, 0x0a,  157 /* Public */,
     107,    1,  760,    2, 0x0a,  159 /* Public */,
     108,    1,  763,    2, 0x0a,  161 /* Public */,
     109,    5,  766,    2, 0x0a,  163 /* Public */,
     110,    1,  777,    2, 0x0a,  169 /* Public */,
     111,    1,  780,    2, 0x0a,  171 /* Public */,
     112,    1,  783,    2, 0x0a,  173 /* Public */,
     113,    1,  786,    2, 0x0a,  175 /* Public */,
     114,    1,  789,    2, 0x0a,  177 /* Public */,
     115,    5,  792,    2, 0x0a,  179 /* Public */,
     116,    1,  803,    2, 0x0a,  185 /* Public */,
     117,    1,  806,    2, 0x0a,  187 /* Public */,
     118,    1,  809,    2, 0x0a,  189 /* Public */,
     119,    1,  812,    2, 0x0a,  191 /* Public */,
     120,    2,  815,    2, 0x0a,  193 /* Public */,
     121,    2,  820,    2, 0x0a,  196 /* Public */,
     122,    4,  825,    2, 0x0a,  199 /* Public */,
     123,    1,  834,    2, 0x0a,  204 /* Public */,
     125,    1,  837,    2, 0x0a,  206 /* Public */,
     127,    3,  840,    2, 0x0a,  208 /* Public */,
     128,    7,  847,    2, 0x0a,  212 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 6, 0x80000000 | 6,    7,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, QMetaType::Bool,   17,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 15, QMetaType::Bool, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15,    4,   26,   27,   28,   29,   30,   31,   32,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   34,   35,   36,   37,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, QMetaType::Float,   42,
    QMetaType::Void, QMetaType::Float,   44,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, QMetaType::Float, QMetaType::Float,    4,   17,   19,   42,   44,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, QMetaType::Float,   42,
    QMetaType::Void, QMetaType::Float,   44,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, QMetaType::Float, QMetaType::Float,    4,   17,   19,   42,   44,
    QMetaType::Void, QMetaType::Float,   44,
    QMetaType::Void, QMetaType::Float,   44,
    QMetaType::Void, QMetaType::Float,   42,
    QMetaType::Void, QMetaType::Float,   42,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   57,   58,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   57,   58,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   61,   62,   63,   64,
    QMetaType::Void, 0x80000000 | 66,   67,
    QMetaType::Void, 0x80000000 | 66,   69,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 73, 0x80000000 | 73, 0x80000000 | 73, 0x80000000 | 73, 0x80000000 | 6,   71,   72,   74,   75,   76,   77,   17,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 80,   81,

 // slots: parameters
    0x80000000 | 15,
    QMetaType::Void, 0x80000000 | 15,   84,
    0x80000000 | 15,
    QMetaType::Void, 0x80000000 | 15,   87,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 6, 0x80000000 | 6,    7,    8,    9,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, QMetaType::Bool,   95,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 15, QMetaType::Bool, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15,    4,   26,   27,   28,   29,   30,   31,   32,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   34,   35,   36,   37,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, QMetaType::Float,   42,
    QMetaType::Void, QMetaType::Float,   44,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, QMetaType::Float, QMetaType::Float,    4,   17,   19,   42,   44,
    QMetaType::Void, 0x80000000 | 15,    4,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, 0x80000000 | 15,   19,
    QMetaType::Void, QMetaType::Float,   42,
    QMetaType::Void, QMetaType::Float,   44,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 15, QMetaType::Float, QMetaType::Float,    4,   17,   19,   42,   44,
    QMetaType::Void, QMetaType::Float,   44,
    QMetaType::Void, QMetaType::Float,   44,
    QMetaType::Void, QMetaType::Float,   42,
    QMetaType::Void, QMetaType::Float,   42,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   57,   58,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   57,   58,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   61,   62,   63,   64,
    QMetaType::Void, 0x80000000 | 15,  124,
    QMetaType::Void, 0x80000000 | 15,  126,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 73,   71,   72,   74,
    QMetaType::Void, 0x80000000 | 15, 0x80000000 | 15, 0x80000000 | 73, 0x80000000 | 73, 0x80000000 | 73, 0x80000000 | 73, 0x80000000 | 6,   71,   72,   74,   75,   76,   77,   17,

       0        // eod
};

Q_CONSTINIT const QMetaObject CommandExecutor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSCommandExecutorENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSCommandExecutorENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSCommandExecutorENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CommandExecutor, std::true_type>,
        // method 'modeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Mode, std::false_type>,
        // method 'versionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        // method 'setModeCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setControlPositionCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setControlSpeedCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setControlTrackingCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'memsModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'errorAllMemsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'memsNotConfiguredChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'memsIllegalDataChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'memsNoResponseOnSpiChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'memsReserveChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'memsAxisSwitchModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'memsProcessModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'memsBoardStatusCommonChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'memsBoardStatusSpeedAndAngleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'motorYawModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'motorYawErrorsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'motorYawFlagsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'motorYawAngleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'motorYawSpeedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'yawMotorStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'motorPitchModeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'motorPitchErrorsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'motorPitchFlagsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'motorPitchAngleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'motorPitchSpeedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'pitchMotorStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'speedYawChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'speedPitchChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'angleYawChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'anglePitchChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'controlPositionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'controlSpeedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'controlTrackingChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        QtPrivate::TypeAndForceComplete<const float, std::false_type>,
        // method 'unknownCommandCounterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int32_t, std::false_type>,
        // method 'statusWithErrorCommandCounterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int32_t, std::false_type>,
        // method 'airConditionerStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int8_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        // method 'setAirConditionerCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Command * const, std::false_type>,
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

void CommandExecutor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CommandExecutor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->modeChanged((*reinterpret_cast< std::add_pointer_t<Mode>>(_a[1]))); break;
        case 1: _t->versionChanged((*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[3]))); break;
        case 2: _t->setModeCommandCompleted(); break;
        case 3: _t->setControlPositionCommandCompleted(); break;
        case 4: _t->setControlSpeedCommandCompleted(); break;
        case 5: _t->setControlTrackingCommandCompleted(); break;
        case 6: _t->memsModeChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 7: _t->errorAllMemsChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->memsNotConfiguredChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 9: _t->memsIllegalDataChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 10: _t->memsNoResponseOnSpiChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 11: _t->memsReserveChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 12: _t->memsAxisSwitchModeChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 13: _t->memsProcessModeChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 14: _t->memsBoardStatusCommonChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[8]))); break;
        case 15: _t->memsBoardStatusSpeedAndAngleChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4]))); break;
        case 16: _t->motorYawModeChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 17: _t->motorYawErrorsChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 18: _t->motorYawFlagsChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 19: _t->motorYawAngleChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 20: _t->motorYawSpeedChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 21: _t->yawMotorStatusChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[5]))); break;
        case 22: _t->motorPitchModeChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 23: _t->motorPitchErrorsChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 24: _t->motorPitchFlagsChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 25: _t->motorPitchAngleChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 26: _t->motorPitchSpeedChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 27: _t->pitchMotorStatusChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[5]))); break;
        case 28: _t->speedYawChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 29: _t->speedPitchChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 30: _t->angleYawChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 31: _t->anglePitchChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 32: _t->controlPositionChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 33: _t->controlSpeedChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 34: _t->controlTrackingChanged((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4]))); break;
        case 35: _t->unknownCommandCounterChanged((*reinterpret_cast< std::add_pointer_t<int32_t>>(_a[1]))); break;
        case 36: _t->statusWithErrorCommandCounterChanged((*reinterpret_cast< std::add_pointer_t<int32_t>>(_a[1]))); break;
        case 37: _t->airConditionerStateChanged((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[7]))); break;
        case 38: _t->setAirConditionerCommandCompleted(); break;
        case 39: _t->sendCmd((*reinterpret_cast< std::add_pointer_t<Command*>>(_a[1]))); break;
        case 40: { uint8_t _r = _t->getHostId();
            if (_a[0]) *reinterpret_cast< uint8_t*>(_a[0]) = std::move(_r); }  break;
        case 41: _t->setHostId((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 42: { uint8_t _r = _t->getDeviceId();
            if (_a[0]) *reinterpret_cast< uint8_t*>(_a[0]) = std::move(_r); }  break;
        case 43: _t->setDeviceId((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 44: _t->setMode((*reinterpret_cast< std::add_pointer_t<Mode>>(_a[1]))); break;
        case 45: _t->getVersion(); break;
        case 46: _t->getStatus(); break;
        case 47: _t->setStatus((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 48: _t->setVersion((*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[3]))); break;
        case 49: _t->setMemsMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 50: _t->setErrorAllMems((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 51: _t->setMemsNotConfigured((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 52: _t->setMemsIllegalData((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 53: _t->setMemsNoResponseOnSpi((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 54: _t->setMemsReserve((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 55: _t->setMemsAxisSwitchMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 56: _t->setMemsProcessMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 57: _t->setMemsBoardStatusCommon((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[7])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[8]))); break;
        case 58: _t->setMemsBoardStatusSpeedAndAngle((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4]))); break;
        case 59: _t->setMotorYawMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 60: _t->setMotorYawErrors((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 61: _t->setMotorYawFlags((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 62: _t->setMotorYawAngle((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 63: _t->setMotorYawSpeed((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 64: _t->setYawMotorStatus((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[5]))); break;
        case 65: _t->setMotorPitchMode((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 66: _t->setMotorPitchErrors((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 67: _t->setMotorPitchFlags((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 68: _t->setMotorPitchAngle((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 69: _t->setMotorPitchSpeed((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 70: _t->setPitchMotorStatus((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[5]))); break;
        case 71: _t->setSpeedYaw((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 72: _t->setSpeedPitch((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 73: _t->setAngleYaw((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 74: _t->setAnglePitch((*reinterpret_cast< std::add_pointer_t<float>>(_a[1]))); break;
        case 75: _t->setControlPosition((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 76: _t->setControlSpeed((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 77: _t->setControlTracking((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4]))); break;
        case 78: _t->setUnknownCommand((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 79: _t->setStatusWithErrorCommand((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 80: _t->setAirConditionerCmd((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[3]))); break;
        case 81: _t->setAirConditionerState((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[5])),(*reinterpret_cast< std::add_pointer_t<int8_t>>(_a[6])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[7]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CommandExecutor::*)(const Mode );
            if (_t _q_method = &CommandExecutor::modeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint32_t , const uint32_t , const uint32_t );
            if (_t _q_method = &CommandExecutor::versionChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)();
            if (_t _q_method = &CommandExecutor::setModeCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)();
            if (_t _q_method = &CommandExecutor::setControlPositionCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)();
            if (_t _q_method = &CommandExecutor::setControlSpeedCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)();
            if (_t _q_method = &CommandExecutor::setControlTrackingCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::memsModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const bool );
            if (_t _q_method = &CommandExecutor::errorAllMemsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::memsNotConfiguredChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::memsIllegalDataChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::memsNoResponseOnSpiChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::memsReserveChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::memsAxisSwitchModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::memsProcessModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t , const bool , const uint8_t , const uint8_t , const uint8_t , const uint8_t , const uint8_t , const uint8_t );
            if (_t _q_method = &CommandExecutor::memsBoardStatusCommonChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float , const float , const float , const float );
            if (_t _q_method = &CommandExecutor::memsBoardStatusSpeedAndAngleChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::motorYawModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::motorYawErrorsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::motorYawFlagsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float );
            if (_t _q_method = &CommandExecutor::motorYawAngleChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 19;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float );
            if (_t _q_method = &CommandExecutor::motorYawSpeedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 20;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t , const uint8_t , const uint8_t , const float , const float );
            if (_t _q_method = &CommandExecutor::yawMotorStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 21;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::motorPitchModeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 22;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::motorPitchErrorsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 23;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t );
            if (_t _q_method = &CommandExecutor::motorPitchFlagsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 24;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float );
            if (_t _q_method = &CommandExecutor::motorPitchAngleChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 25;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float );
            if (_t _q_method = &CommandExecutor::motorPitchSpeedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 26;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t , const uint8_t , const uint8_t , const float , const float );
            if (_t _q_method = &CommandExecutor::pitchMotorStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 27;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float );
            if (_t _q_method = &CommandExecutor::speedYawChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 28;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float );
            if (_t _q_method = &CommandExecutor::speedPitchChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 29;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float );
            if (_t _q_method = &CommandExecutor::angleYawChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 30;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float );
            if (_t _q_method = &CommandExecutor::anglePitchChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 31;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float , const float );
            if (_t _q_method = &CommandExecutor::controlPositionChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 32;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float , const float );
            if (_t _q_method = &CommandExecutor::controlSpeedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 33;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const float , const float , const float , const float );
            if (_t _q_method = &CommandExecutor::controlTrackingChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 34;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const int32_t );
            if (_t _q_method = &CommandExecutor::unknownCommandCounterChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 35;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const int32_t );
            if (_t _q_method = &CommandExecutor::statusWithErrorCommandCounterChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 36;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(const uint8_t , const uint8_t , const int8_t , const int8_t , const int8_t , const int8_t , const uint32_t );
            if (_t _q_method = &CommandExecutor::airConditionerStateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 37;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)();
            if (_t _q_method = &CommandExecutor::setAirConditionerCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 38;
                return;
            }
        }
        {
            using _t = void (CommandExecutor::*)(Command * const );
            if (_t _q_method = &CommandExecutor::sendCmd; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 39;
                return;
            }
        }
    }
}

const QMetaObject *CommandExecutor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommandExecutor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSCommandExecutorENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CommandExecutor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 82)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 82;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 82)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 82;
    }
    return _id;
}

// SIGNAL 0
void CommandExecutor::modeChanged(const Mode _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CommandExecutor::versionChanged(const uint32_t _t1, const uint32_t _t2, const uint32_t _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CommandExecutor::setModeCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void CommandExecutor::setControlPositionCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void CommandExecutor::setControlSpeedCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void CommandExecutor::setControlTrackingCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void CommandExecutor::memsModeChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CommandExecutor::errorAllMemsChanged(const bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void CommandExecutor::memsNotConfiguredChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void CommandExecutor::memsIllegalDataChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void CommandExecutor::memsNoResponseOnSpiChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}

// SIGNAL 11
void CommandExecutor::memsReserveChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}

// SIGNAL 12
void CommandExecutor::memsAxisSwitchModeChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void CommandExecutor::memsProcessModeChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}

// SIGNAL 14
void CommandExecutor::memsBoardStatusCommonChanged(const uint8_t _t1, const bool _t2, const uint8_t _t3, const uint8_t _t4, const uint8_t _t5, const uint8_t _t6, const uint8_t _t7, const uint8_t _t8)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t8))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void CommandExecutor::memsBoardStatusSpeedAndAngleChanged(const float _t1, const float _t2, const float _t3, const float _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 15, _a);
}

// SIGNAL 16
void CommandExecutor::motorYawModeChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 16, _a);
}

// SIGNAL 17
void CommandExecutor::motorYawErrorsChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 17, _a);
}

// SIGNAL 18
void CommandExecutor::motorYawFlagsChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 18, _a);
}

// SIGNAL 19
void CommandExecutor::motorYawAngleChanged(const float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 19, _a);
}

// SIGNAL 20
void CommandExecutor::motorYawSpeedChanged(const float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 20, _a);
}

// SIGNAL 21
void CommandExecutor::yawMotorStatusChanged(const uint8_t _t1, const uint8_t _t2, const uint8_t _t3, const float _t4, const float _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 21, _a);
}

// SIGNAL 22
void CommandExecutor::motorPitchModeChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 22, _a);
}

// SIGNAL 23
void CommandExecutor::motorPitchErrorsChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 23, _a);
}

// SIGNAL 24
void CommandExecutor::motorPitchFlagsChanged(const uint8_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 24, _a);
}

// SIGNAL 25
void CommandExecutor::motorPitchAngleChanged(const float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 25, _a);
}

// SIGNAL 26
void CommandExecutor::motorPitchSpeedChanged(const float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 26, _a);
}

// SIGNAL 27
void CommandExecutor::pitchMotorStatusChanged(const uint8_t _t1, const uint8_t _t2, const uint8_t _t3, const float _t4, const float _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 27, _a);
}

// SIGNAL 28
void CommandExecutor::speedYawChanged(const float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 28, _a);
}

// SIGNAL 29
void CommandExecutor::speedPitchChanged(const float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 29, _a);
}

// SIGNAL 30
void CommandExecutor::angleYawChanged(const float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 30, _a);
}

// SIGNAL 31
void CommandExecutor::anglePitchChanged(const float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 31, _a);
}

// SIGNAL 32
void CommandExecutor::controlPositionChanged(const float _t1, const float _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 32, _a);
}

// SIGNAL 33
void CommandExecutor::controlSpeedChanged(const float _t1, const float _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 33, _a);
}

// SIGNAL 34
void CommandExecutor::controlTrackingChanged(const float _t1, const float _t2, const float _t3, const float _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 34, _a);
}

// SIGNAL 35
void CommandExecutor::unknownCommandCounterChanged(const int32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 35, _a);
}

// SIGNAL 36
void CommandExecutor::statusWithErrorCommandCounterChanged(const int32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 36, _a);
}

// SIGNAL 37
void CommandExecutor::airConditionerStateChanged(const uint8_t _t1, const uint8_t _t2, const int8_t _t3, const int8_t _t4, const int8_t _t5, const int8_t _t6, const uint32_t _t7)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))) };
    QMetaObject::activate(this, &staticMetaObject, 37, _a);
}

// SIGNAL 38
void CommandExecutor::setAirConditionerCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 38, nullptr);
}

// SIGNAL 39
void CommandExecutor::sendCmd(Command * const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 39, _a);
}
QT_WARNING_POP
