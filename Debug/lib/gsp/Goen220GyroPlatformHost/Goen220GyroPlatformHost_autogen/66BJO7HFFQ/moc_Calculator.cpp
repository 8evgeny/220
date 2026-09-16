/****************************************************************************
** Meta object code from reading C++ file 'Calculator.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../lib/gsp/Goen220GyroPlatformHost/src/NDevices/Calculator.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Calculator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_Calculator_t {
    uint offsetsAndSizes[168];
    char stringdata0[11];
    char stringdata1[10];
    char stringdata2[8];
    char stringdata3[1];
    char stringdata4[10];
    char stringdata5[7];
    char stringdata6[12];
    char stringdata7[12];
    char stringdata8[9];
    char stringdata9[8];
    char stringdata10[5];
    char stringdata11[5];
    char stringdata12[11];
    char stringdata13[10];
    char stringdata14[10];
    char stringdata15[11];
    char stringdata16[9];
    char stringdata17[19];
    char stringdata18[16];
    char stringdata19[14];
    char stringdata20[12];
    char stringdata21[16];
    char stringdata22[6];
    char stringdata23[21];
    char stringdata24[6];
    char stringdata25[19];
    char stringdata26[23];
    char stringdata27[15];
    char stringdata28[22];
    char stringdata29[19];
    char stringdata30[25];
    char stringdata31[13];
    char stringdata32[23];
    char stringdata33[21];
    char stringdata34[25];
    char stringdata35[13];
    char stringdata36[19];
    char stringdata37[16];
    char stringdata38[32];
    char stringdata39[9];
    char stringdata40[11];
    char stringdata41[9];
    char stringdata42[11];
    char stringdata43[16];
    char stringdata44[18];
    char stringdata45[17];
    char stringdata46[17];
    char stringdata47[6];
    char stringdata48[17];
    char stringdata49[6];
    char stringdata50[18];
    char stringdata51[7];
    char stringdata52[18];
    char stringdata53[20];
    char stringdata54[19];
    char stringdata55[19];
    char stringdata56[19];
    char stringdata57[20];
    char stringdata58[12];
    char stringdata59[14];
    char stringdata60[12];
    char stringdata61[14];
    char stringdata62[19];
    char stringdata63[4];
    char stringdata64[6];
    char stringdata65[16];
    char stringdata66[19];
    char stringdata67[16];
    char stringdata68[18];
    char stringdata69[14];
    char stringdata70[16];
    char stringdata71[18];
    char stringdata72[12];
    char stringdata73[26];
    char stringdata74[10];
    char stringdata75[21];
    char stringdata76[9];
    char stringdata77[9];
    char stringdata78[7];
    char stringdata79[9];
    char stringdata80[23];
    char stringdata81[3];
    char stringdata82[3];
    char stringdata83[3];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Calculator_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Calculator_t qt_meta_stringdata_Calculator = {
    {
        QT_MOC_LITERAL(0, 10),  // "Calculator"
        QT_MOC_LITERAL(11, 9),  // "getHostId"
        QT_MOC_LITERAL(21, 7),  // "uint8_t"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 9),  // "setHostId"
        QT_MOC_LITERAL(40, 6),  // "hostId"
        QT_MOC_LITERAL(47, 11),  // "getDeviceId"
        QT_MOC_LITERAL(59, 11),  // "setDeviceId"
        QT_MOC_LITERAL(71, 8),  // "deviceId"
        QT_MOC_LITERAL(80, 7),  // "setMode"
        QT_MOC_LITERAL(88, 4),  // "Mode"
        QT_MOC_LITERAL(93, 4),  // "mode"
        QT_MOC_LITERAL(98, 10),  // "getVersion"
        QT_MOC_LITERAL(109, 9),  // "getStatus"
        QT_MOC_LITERAL(119, 9),  // "setStatus"
        QT_MOC_LITERAL(129, 10),  // "setVersion"
        QT_MOC_LITERAL(140, 8),  // "uint32_t"
        QT_MOC_LITERAL(149, 18),  // "versionProtocolVdb"
        QT_MOC_LITERAL(168, 15),  // "versionProtocol"
        QT_MOC_LITERAL(184, 13),  // "versionDevice"
        QT_MOC_LITERAL(198, 11),  // "setMemsMode"
        QT_MOC_LITERAL(210, 15),  // "setErrorAllMems"
        QT_MOC_LITERAL(226, 5),  // "error"
        QT_MOC_LITERAL(232, 20),  // "setMemsNotConfigured"
        QT_MOC_LITERAL(253, 5),  // "flags"
        QT_MOC_LITERAL(259, 18),  // "setMemsIllegalData"
        QT_MOC_LITERAL(278, 22),  // "setMemsNoResponseOnSpi"
        QT_MOC_LITERAL(301, 14),  // "setMemsReserve"
        QT_MOC_LITERAL(316, 21),  // "setMemsAxisSwitchMode"
        QT_MOC_LITERAL(338, 18),  // "setMemsProcessMode"
        QT_MOC_LITERAL(357, 24),  // "setMemsBoardStatusCommon"
        QT_MOC_LITERAL(382, 12),  // "errorAllMems"
        QT_MOC_LITERAL(395, 22),  // "memsNotConfiguredFlags"
        QT_MOC_LITERAL(418, 20),  // "memsIllegalDataFlags"
        QT_MOC_LITERAL(439, 24),  // "memsNoResponseOnSpiFlags"
        QT_MOC_LITERAL(464, 12),  // "reserveFlags"
        QT_MOC_LITERAL(477, 18),  // "memsAxisSwitchMode"
        QT_MOC_LITERAL(496, 15),  // "memsProcessMode"
        QT_MOC_LITERAL(512, 31),  // "setMemsBoardStatusSpeedAndAngle"
        QT_MOC_LITERAL(544, 8),  // "speedYaw"
        QT_MOC_LITERAL(553, 10),  // "speedPitch"
        QT_MOC_LITERAL(564, 8),  // "angleYaw"
        QT_MOC_LITERAL(573, 10),  // "anglePitch"
        QT_MOC_LITERAL(584, 15),  // "setMotorYawMode"
        QT_MOC_LITERAL(600, 17),  // "setMotorYawErrors"
        QT_MOC_LITERAL(618, 16),  // "setMotorYawFlags"
        QT_MOC_LITERAL(635, 16),  // "setMotorYawAngle"
        QT_MOC_LITERAL(652, 5),  // "angle"
        QT_MOC_LITERAL(658, 16),  // "setMotorYawSpeed"
        QT_MOC_LITERAL(675, 5),  // "speed"
        QT_MOC_LITERAL(681, 17),  // "setYawMotorStatus"
        QT_MOC_LITERAL(699, 6),  // "errors"
        QT_MOC_LITERAL(706, 17),  // "setMotorPitchMode"
        QT_MOC_LITERAL(724, 19),  // "setMotorPitchErrors"
        QT_MOC_LITERAL(744, 18),  // "setMotorPitchFlags"
        QT_MOC_LITERAL(763, 18),  // "setMotorPitchAngle"
        QT_MOC_LITERAL(782, 18),  // "setMotorPitchSpeed"
        QT_MOC_LITERAL(801, 19),  // "setPitchMotorStatus"
        QT_MOC_LITERAL(821, 11),  // "setSpeedYaw"
        QT_MOC_LITERAL(833, 13),  // "setSpeedPitch"
        QT_MOC_LITERAL(847, 11),  // "setAngleYaw"
        QT_MOC_LITERAL(859, 13),  // "setAnglePitch"
        QT_MOC_LITERAL(873, 18),  // "setControlPosition"
        QT_MOC_LITERAL(892, 3),  // "yaw"
        QT_MOC_LITERAL(896, 5),  // "pitch"
        QT_MOC_LITERAL(902, 15),  // "setControlSpeed"
        QT_MOC_LITERAL(918, 18),  // "setControlTracking"
        QT_MOC_LITERAL(937, 15),  // "yawMisalignment"
        QT_MOC_LITERAL(953, 17),  // "pitchMisalignment"
        QT_MOC_LITERAL(971, 13),  // "yawAccelLimit"
        QT_MOC_LITERAL(985, 15),  // "pitchAccelLimit"
        QT_MOC_LITERAL(1001, 17),  // "setUnknownCommand"
        QT_MOC_LITERAL(1019, 11),  // "dstFunction"
        QT_MOC_LITERAL(1031, 25),  // "setStatusWithErrorCommand"
        QT_MOC_LITERAL(1057, 9),  // "errorCode"
        QT_MOC_LITERAL(1067, 20),  // "setAirConditionerCmd"
        QT_MOC_LITERAL(1088, 8),  // "fanSpeed"
        QT_MOC_LITERAL(1097, 8),  // "heaterOn"
        QT_MOC_LITERAL(1106, 6),  // "int8_t"
        QT_MOC_LITERAL(1113, 8),  // "peltierI"
        QT_MOC_LITERAL(1122, 22),  // "setAirConditionerState"
        QT_MOC_LITERAL(1145, 2),  // "t1"
        QT_MOC_LITERAL(1148, 2),  // "t2"
        QT_MOC_LITERAL(1151, 2)   // "t3"
    },
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
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Calculator[] = {

 // content:
      10,       // revision
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
    qt_meta_stringdata_Calculator.offsetsAndSizes,
    qt_meta_data_Calculator,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Calculator_t,
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
    if (!strcmp(_clname, qt_meta_stringdata_Calculator.stringdata0))
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
QT_END_MOC_NAMESPACE
