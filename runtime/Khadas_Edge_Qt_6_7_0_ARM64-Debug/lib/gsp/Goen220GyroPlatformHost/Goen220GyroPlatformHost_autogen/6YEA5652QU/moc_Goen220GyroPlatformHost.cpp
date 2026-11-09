/****************************************************************************
** Meta object code from reading C++ file 'Goen220GyroPlatformHost.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../src/lib/gsp/Goen220GyroPlatformHost/include/Goen220GyroPlatformHost.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Goen220GyroPlatformHost.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSGoen220GyroPlatformHostENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSGoen220GyroPlatformHostENDCLASS = QtMocHelpers::stringData(
    "Goen220GyroPlatformHost",
    "stateConnectToSerialPortChanged",
    "",
    "con",
    "txRawDataSerialPortText",
    "text",
    "rxRawDataSerialPortText",
    "versionChanged",
    "uint32_t",
    "versionProtocolVdb",
    "versionProtocol",
    "versionDevice",
    "modeChanged",
    "Mode",
    "mode",
    "memsBoardStatusCommonChanged",
    "MemsBoardStatusCommon",
    "memsBoardStatus",
    "memsBoardStatusSpeedAndAngleChanged",
    "MemsBoardStatusSpeedAndAngle",
    "yawMotorStatusChanged",
    "MotorStatus",
    "motorStatus",
    "pitchMotorStatusChanged",
    "setModeCommandCompleted",
    "setControlPositionCommandCompleted",
    "setControlSpeedCommandCompleted",
    "setControlTrackingCommandCompleted",
    "restart",
    "stateConnectToRemoteDeviceChanged",
    "serialPortsChanged",
    "serialPortNameChanged",
    "serialPortErrorStringChanged",
    "printTxRawDataSerialPortChanged",
    "printRxRawDataSerialPortChanged",
    "connect",
    "setSerialPortName",
    "portName",
    "setPrintTxRawDataSerialPort",
    "enable",
    "setPrintRxRawDataSerialPort",
    "setHostId",
    "uint8_t",
    "hostId",
    "setDeviceId",
    "deviceId",
    "setTimeoutWaitingResponse",
    "msec",
    "setTimeBetweenStatusRequests",
    "setMode",
    "getModeNames",
    "setControlPosition",
    "yaw",
    "pitch",
    "setControlSpeed",
    "setControlTracking",
    "yawMisalignment",
    "pitchMisalignment",
    "yawAccelLimit",
    "pitchAccelLimit",
    "serialPorts",
    "serialPortName",
    "serialPortErrorString",
    "printTxRawDataSerialPort",
    "printRxRawDataSerialPort",
    "Dummy",
    "Stabilization",
    "RotaryPlatform",
    "Parkin",
    "Off"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGoen220GyroPlatformHostENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       5,  303, // properties
       1,  328, // enums/sets
       0,    0, // constructors
       0,       // flags
      20,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  212,    2, 0x06,    7 /* Public */,
       4,    1,  215,    2, 0x06,    9 /* Public */,
       6,    1,  218,    2, 0x06,   11 /* Public */,
       7,    3,  221,    2, 0x06,   13 /* Public */,
      12,    1,  228,    2, 0x06,   17 /* Public */,
      15,    1,  231,    2, 0x06,   19 /* Public */,
      18,    1,  234,    2, 0x06,   21 /* Public */,
      20,    1,  237,    2, 0x06,   23 /* Public */,
      23,    1,  240,    2, 0x06,   25 /* Public */,
      24,    0,  243,    2, 0x06,   27 /* Public */,
      25,    0,  244,    2, 0x06,   28 /* Public */,
      26,    0,  245,    2, 0x06,   29 /* Public */,
      27,    0,  246,    2, 0x06,   30 /* Public */,
      28,    0,  247,    2, 0x06,   31 /* Public */,
      29,    1,  248,    2, 0x06,   32 /* Public */,
      30,    0,  251,    2, 0x06,   34 /* Public */,
      31,    0,  252,    2, 0x06,   35 /* Public */,
      32,    0,  253,    2, 0x06,   36 /* Public */,
      33,    0,  254,    2, 0x06,   37 /* Public */,
      34,    0,  255,    2, 0x06,   38 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      35,    1,  256,    2, 0x02,   39 /* Public */,
      36,    1,  259,    2, 0x02,   41 /* Public */,
      38,    1,  262,    2, 0x02,   43 /* Public */,
      40,    1,  265,    2, 0x02,   45 /* Public */,
      41,    1,  268,    2, 0x02,   47 /* Public */,
      44,    1,  271,    2, 0x02,   49 /* Public */,
      46,    1,  274,    2, 0x02,   51 /* Public */,
      48,    1,  277,    2, 0x02,   53 /* Public */,
      49,    1,  280,    2, 0x02,   55 /* Public */,
      50,    0,  283,    2, 0x102,   57 /* Public | MethodIsConst  */,
      51,    2,  284,    2, 0x02,   58 /* Public */,
      54,    2,  289,    2, 0x02,   61 /* Public */,
      55,    4,  294,    2, 0x02,   64 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, 0x80000000 | 8, 0x80000000 | 8, 0x80000000 | 8,    9,   10,   11,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, 0x80000000 | 19,   17,
    QMetaType::Void, 0x80000000 | 21,   22,
    QMetaType::Void, 0x80000000 | 21,   22,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QString,   37,
    QMetaType::Void, QMetaType::Bool,   39,
    QMetaType::Void, QMetaType::Bool,   39,
    QMetaType::Void, 0x80000000 | 42,   43,
    QMetaType::Void, 0x80000000 | 42,   45,
    QMetaType::Void, 0x80000000 | 8,   47,
    QMetaType::Void, 0x80000000 | 8,   47,
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::QStringList,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   52,   53,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   52,   53,
    QMetaType::Void, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   56,   57,   58,   59,

 // properties: name, type, flags
      60, QMetaType::QStringList, 0x00015001, uint(15), 0,
      61, QMetaType::QString, 0x00015103, uint(16), 0,
      62, QMetaType::QString, 0x00015001, uint(17), 0,
      63, QMetaType::Bool, 0x00015103, uint(18), 0,
      64, QMetaType::Bool, 0x00015103, uint(19), 0,

 // enums: name, alias, flags, count, data
      13,   13, 0x0,    5,  333,

 // enum data: key, value
      65, uint(Goen220GyroPlatformHost::Dummy),
      66, uint(Goen220GyroPlatformHost::Stabilization),
      67, uint(Goen220GyroPlatformHost::RotaryPlatform),
      68, uint(Goen220GyroPlatformHost::Parkin),
      69, uint(Goen220GyroPlatformHost::Off),

       0        // eod
};

Q_CONSTINIT const QMetaObject Goen220GyroPlatformHost::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSGoen220GyroPlatformHostENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGoen220GyroPlatformHostENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGoen220GyroPlatformHostENDCLASS_t,
        // property 'serialPorts'
        QtPrivate::TypeAndForceComplete<QStringList, std::true_type>,
        // property 'serialPortName'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'serialPortErrorString'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'printTxRawDataSerialPort'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'printRxRawDataSerialPort'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // enum 'Mode'
        QtPrivate::TypeAndForceComplete<Goen220GyroPlatformHost::Mode, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Goen220GyroPlatformHost, std::true_type>,
        // method 'stateConnectToSerialPortChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'txRawDataSerialPortText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'rxRawDataSerialPortText'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'versionChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        // method 'modeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Mode, std::false_type>,
        // method 'memsBoardStatusCommonChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const MemsBoardStatusCommon &, std::false_type>,
        // method 'memsBoardStatusSpeedAndAngleChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const MemsBoardStatusSpeedAndAngle &, std::false_type>,
        // method 'yawMotorStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const MotorStatus &, std::false_type>,
        // method 'pitchMotorStatusChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const MotorStatus &, std::false_type>,
        // method 'setModeCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setControlPositionCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setControlSpeedCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setControlTrackingCommandCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'restart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stateConnectToRemoteDeviceChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'serialPortsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'serialPortNameChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'serialPortErrorStringChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'printTxRawDataSerialPortChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'printRxRawDataSerialPortChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'connect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'setSerialPortName'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setPrintTxRawDataSerialPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'setPrintRxRawDataSerialPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'setHostId'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setDeviceId'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint8_t, std::false_type>,
        // method 'setTimeoutWaitingResponse'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        // method 'setTimeBetweenStatusRequests'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const uint32_t, std::false_type>,
        // method 'setMode'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Mode, std::false_type>,
        // method 'getModeNames'
        QtPrivate::TypeAndForceComplete<QStringList, std::false_type>,
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
        QtPrivate::TypeAndForceComplete<const float, std::false_type>
    >,
    nullptr
} };

void Goen220GyroPlatformHost::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Goen220GyroPlatformHost *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->stateConnectToSerialPortChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->txRawDataSerialPortText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->rxRawDataSerialPortText((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->versionChanged((*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[3]))); break;
        case 4: _t->modeChanged((*reinterpret_cast< std::add_pointer_t<Mode>>(_a[1]))); break;
        case 5: _t->memsBoardStatusCommonChanged((*reinterpret_cast< std::add_pointer_t<MemsBoardStatusCommon>>(_a[1]))); break;
        case 6: _t->memsBoardStatusSpeedAndAngleChanged((*reinterpret_cast< std::add_pointer_t<MemsBoardStatusSpeedAndAngle>>(_a[1]))); break;
        case 7: _t->yawMotorStatusChanged((*reinterpret_cast< std::add_pointer_t<MotorStatus>>(_a[1]))); break;
        case 8: _t->pitchMotorStatusChanged((*reinterpret_cast< std::add_pointer_t<MotorStatus>>(_a[1]))); break;
        case 9: _t->setModeCommandCompleted(); break;
        case 10: _t->setControlPositionCommandCompleted(); break;
        case 11: _t->setControlSpeedCommandCompleted(); break;
        case 12: _t->setControlTrackingCommandCompleted(); break;
        case 13: _t->restart(); break;
        case 14: _t->stateConnectToRemoteDeviceChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 15: _t->serialPortsChanged(); break;
        case 16: _t->serialPortNameChanged(); break;
        case 17: _t->serialPortErrorStringChanged(); break;
        case 18: _t->printTxRawDataSerialPortChanged(); break;
        case 19: _t->printRxRawDataSerialPortChanged(); break;
        case 20: _t->connect((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 21: _t->setSerialPortName((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->setPrintTxRawDataSerialPort((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 23: _t->setPrintRxRawDataSerialPort((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 24: _t->setHostId((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 25: _t->setDeviceId((*reinterpret_cast< std::add_pointer_t<uint8_t>>(_a[1]))); break;
        case 26: _t->setTimeoutWaitingResponse((*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[1]))); break;
        case 27: _t->setTimeBetweenStatusRequests((*reinterpret_cast< std::add_pointer_t<uint32_t>>(_a[1]))); break;
        case 28: _t->setMode((*reinterpret_cast< std::add_pointer_t<Mode>>(_a[1]))); break;
        case 29: { QStringList _r = _t->getModeNames();
            if (_a[0]) *reinterpret_cast< QStringList*>(_a[0]) = std::move(_r); }  break;
        case 30: _t->setControlPosition((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 31: _t->setControlSpeed((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2]))); break;
        case 32: _t->setControlTracking((*reinterpret_cast< std::add_pointer_t<float>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< MemsBoardStatusCommon >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< MemsBoardStatusSpeedAndAngle >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< MotorStatus >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< MotorStatus >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Goen220GyroPlatformHost::*)(const bool );
            if (_t _q_method = &Goen220GyroPlatformHost::stateConnectToSerialPortChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const QString & );
            if (_t _q_method = &Goen220GyroPlatformHost::txRawDataSerialPortText; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const QString & );
            if (_t _q_method = &Goen220GyroPlatformHost::rxRawDataSerialPortText; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const uint32_t , const uint32_t , const uint32_t );
            if (_t _q_method = &Goen220GyroPlatformHost::versionChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const Mode );
            if (_t _q_method = &Goen220GyroPlatformHost::modeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const MemsBoardStatusCommon & );
            if (_t _q_method = &Goen220GyroPlatformHost::memsBoardStatusCommonChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const MemsBoardStatusSpeedAndAngle & );
            if (_t _q_method = &Goen220GyroPlatformHost::memsBoardStatusSpeedAndAngleChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const MotorStatus & );
            if (_t _q_method = &Goen220GyroPlatformHost::yawMotorStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const MotorStatus & );
            if (_t _q_method = &Goen220GyroPlatformHost::pitchMotorStatusChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::setModeCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::setControlPositionCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::setControlSpeedCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::setControlTrackingCommandCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::restart; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)(const bool );
            if (_t _q_method = &Goen220GyroPlatformHost::stateConnectToRemoteDeviceChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 14;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::serialPortsChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 15;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::serialPortNameChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 16;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::serialPortErrorStringChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 17;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::printTxRawDataSerialPortChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 18;
                return;
            }
        }
        {
            using _t = void (Goen220GyroPlatformHost::*)();
            if (_t _q_method = &Goen220GyroPlatformHost::printRxRawDataSerialPortChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 19;
                return;
            }
        }
    } else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Goen220GyroPlatformHost *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QStringList*>(_v) = _t->getSerialPorts(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->getSerialPortName(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->getSerialPortErrorString(); break;
        case 3: *reinterpret_cast< bool*>(_v) = _t->getPrintTxRawDataSerialPort(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->getPrintRxRawDataSerialPort(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<Goen220GyroPlatformHost *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setSerialPortName(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setPrintTxRawDataSerialPort(*reinterpret_cast< bool*>(_v)); break;
        case 4: _t->setPrintRxRawDataSerialPort(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *Goen220GyroPlatformHost::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Goen220GyroPlatformHost::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGoen220GyroPlatformHostENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Goen220GyroPlatformHost::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Goen220GyroPlatformHost::stateConnectToSerialPortChanged(const bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Goen220GyroPlatformHost::txRawDataSerialPortText(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Goen220GyroPlatformHost::rxRawDataSerialPortText(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Goen220GyroPlatformHost::versionChanged(const uint32_t _t1, const uint32_t _t2, const uint32_t _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Goen220GyroPlatformHost::modeChanged(const Mode _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Goen220GyroPlatformHost::memsBoardStatusCommonChanged(const MemsBoardStatusCommon & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Goen220GyroPlatformHost::memsBoardStatusSpeedAndAngleChanged(const MemsBoardStatusSpeedAndAngle & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Goen220GyroPlatformHost::yawMotorStatusChanged(const MotorStatus & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Goen220GyroPlatformHost::pitchMotorStatusChanged(const MotorStatus & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void Goen220GyroPlatformHost::setModeCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void Goen220GyroPlatformHost::setControlPositionCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void Goen220GyroPlatformHost::setControlSpeedCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void Goen220GyroPlatformHost::setControlTrackingCommandCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void Goen220GyroPlatformHost::restart()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void Goen220GyroPlatformHost::stateConnectToRemoteDeviceChanged(const bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 14, _a);
}

// SIGNAL 15
void Goen220GyroPlatformHost::serialPortsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void Goen220GyroPlatformHost::serialPortNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}

// SIGNAL 17
void Goen220GyroPlatformHost::serialPortErrorStringChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void Goen220GyroPlatformHost::printTxRawDataSerialPortChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 18, nullptr);
}

// SIGNAL 19
void Goen220GyroPlatformHost::printRxRawDataSerialPortChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}
QT_WARNING_POP
