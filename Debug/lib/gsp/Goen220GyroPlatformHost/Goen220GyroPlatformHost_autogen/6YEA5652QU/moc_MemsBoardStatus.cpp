/****************************************************************************
** Meta object code from reading C++ file 'MemsBoardStatus.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../lib/gsp/Goen220GyroPlatformHost/include/MemsBoardStatus.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MemsBoardStatus.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MemsBoardStatusCommon_t {
    uint offsetsAndSizes[20];
    char stringdata0[22];
    char stringdata1[5];
    char stringdata2[8];
    char stringdata3[13];
    char stringdata4[19];
    char stringdata5[17];
    char stringdata6[21];
    char stringdata7[13];
    char stringdata8[15];
    char stringdata9[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MemsBoardStatusCommon_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MemsBoardStatusCommon_t qt_meta_stringdata_MemsBoardStatusCommon = {
    {
        QT_MOC_LITERAL(0, 21),  // "MemsBoardStatusCommon"
        QT_MOC_LITERAL(22, 4),  // "mode"
        QT_MOC_LITERAL(27, 7),  // "uint8_t"
        QT_MOC_LITERAL(35, 12),  // "errorAllMems"
        QT_MOC_LITERAL(48, 18),  // "notConfiguredFlags"
        QT_MOC_LITERAL(67, 16),  // "illegalDataFlags"
        QT_MOC_LITERAL(84, 20),  // "noResponseOnSpiFlags"
        QT_MOC_LITERAL(105, 12),  // "reserveFlags"
        QT_MOC_LITERAL(118, 14),  // "axisSwitchMode"
        QT_MOC_LITERAL(133, 15)   // "memsProcessMode"
    },
    "MemsBoardStatusCommon",
    "mode",
    "uint8_t",
    "errorAllMems",
    "notConfiguredFlags",
    "illegalDataFlags",
    "noResponseOnSpiFlags",
    "reserveFlags",
    "axisSwitchMode",
    "memsProcessMode"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MemsBoardStatusCommon[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       8,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       3, QMetaType::Bool, 0x00015003, uint(-1), 0,
       4, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       5, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       6, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       7, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       8, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       9, 0x80000000 | 2, 0x0001500b, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject MemsBoardStatusCommon::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_MemsBoardStatusCommon.offsetsAndSizes,
    qt_meta_data_MemsBoardStatusCommon,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MemsBoardStatusCommon_t,
        // property 'mode'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'errorAllMems'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'notConfiguredFlags'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'illegalDataFlags'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'noResponseOnSpiFlags'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'reserveFlags'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'axisSwitchMode'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'memsProcessMode'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MemsBoardStatusCommon, std::true_type>
    >,
    nullptr
} };

void MemsBoardStatusCommon::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
if (_c == QMetaObject::ReadProperty) {
        auto *_t = reinterpret_cast<MemsBoardStatusCommon *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< uint8_t*>(_v) = _t->mode; break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->errorAllMems; break;
        case 2: *reinterpret_cast< uint8_t*>(_v) = _t->notConfiguredFlags; break;
        case 3: *reinterpret_cast< uint8_t*>(_v) = _t->illegalDataFlags; break;
        case 4: *reinterpret_cast< uint8_t*>(_v) = _t->noResponseOnSpiFlags; break;
        case 5: *reinterpret_cast< uint8_t*>(_v) = _t->reserveFlags; break;
        case 6: *reinterpret_cast< uint8_t*>(_v) = _t->axisSwitchMode; break;
        case 7: *reinterpret_cast< uint8_t*>(_v) = _t->memsProcessMode; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = reinterpret_cast<MemsBoardStatusCommon *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->mode != *reinterpret_cast< uint8_t*>(_v)) {
                _t->mode = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 1:
            if (_t->errorAllMems != *reinterpret_cast< bool*>(_v)) {
                _t->errorAllMems = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 2:
            if (_t->notConfiguredFlags != *reinterpret_cast< uint8_t*>(_v)) {
                _t->notConfiguredFlags = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 3:
            if (_t->illegalDataFlags != *reinterpret_cast< uint8_t*>(_v)) {
                _t->illegalDataFlags = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 4:
            if (_t->noResponseOnSpiFlags != *reinterpret_cast< uint8_t*>(_v)) {
                _t->noResponseOnSpiFlags = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 5:
            if (_t->reserveFlags != *reinterpret_cast< uint8_t*>(_v)) {
                _t->reserveFlags = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 6:
            if (_t->axisSwitchMode != *reinterpret_cast< uint8_t*>(_v)) {
                _t->axisSwitchMode = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 7:
            if (_t->memsProcessMode != *reinterpret_cast< uint8_t*>(_v)) {
                _t->memsProcessMode = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}
namespace {
struct qt_meta_stringdata_MemsBoardStatusSpeedAndAngle_t {
    uint offsetsAndSizes[10];
    char stringdata0[29];
    char stringdata1[9];
    char stringdata2[11];
    char stringdata3[9];
    char stringdata4[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MemsBoardStatusSpeedAndAngle_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MemsBoardStatusSpeedAndAngle_t qt_meta_stringdata_MemsBoardStatusSpeedAndAngle = {
    {
        QT_MOC_LITERAL(0, 28),  // "MemsBoardStatusSpeedAndAngle"
        QT_MOC_LITERAL(29, 8),  // "speedYaw"
        QT_MOC_LITERAL(38, 10),  // "speedPitch"
        QT_MOC_LITERAL(49, 8),  // "angleYaw"
        QT_MOC_LITERAL(58, 10)   // "anglePitch"
    },
    "MemsBoardStatusSpeedAndAngle",
    "speedYaw",
    "speedPitch",
    "angleYaw",
    "anglePitch"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MemsBoardStatusSpeedAndAngle[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       4,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Float, 0x00015003, uint(-1), 0,
       2, QMetaType::Float, 0x00015003, uint(-1), 0,
       3, QMetaType::Float, 0x00015003, uint(-1), 0,
       4, QMetaType::Float, 0x00015003, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject MemsBoardStatusSpeedAndAngle::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_MemsBoardStatusSpeedAndAngle.offsetsAndSizes,
    qt_meta_data_MemsBoardStatusSpeedAndAngle,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MemsBoardStatusSpeedAndAngle_t,
        // property 'speedYaw'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // property 'speedPitch'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // property 'angleYaw'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // property 'anglePitch'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MemsBoardStatusSpeedAndAngle, std::true_type>
    >,
    nullptr
} };

void MemsBoardStatusSpeedAndAngle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
if (_c == QMetaObject::ReadProperty) {
        auto *_t = reinterpret_cast<MemsBoardStatusSpeedAndAngle *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< float*>(_v) = _t->speedYaw; break;
        case 1: *reinterpret_cast< float*>(_v) = _t->speedPitch; break;
        case 2: *reinterpret_cast< float*>(_v) = _t->angleYaw; break;
        case 3: *reinterpret_cast< float*>(_v) = _t->anglePitch; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = reinterpret_cast<MemsBoardStatusSpeedAndAngle *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->speedYaw != *reinterpret_cast< float*>(_v)) {
                _t->speedYaw = *reinterpret_cast< float*>(_v);
            }
            break;
        case 1:
            if (_t->speedPitch != *reinterpret_cast< float*>(_v)) {
                _t->speedPitch = *reinterpret_cast< float*>(_v);
            }
            break;
        case 2:
            if (_t->angleYaw != *reinterpret_cast< float*>(_v)) {
                _t->angleYaw = *reinterpret_cast< float*>(_v);
            }
            break;
        case 3:
            if (_t->anglePitch != *reinterpret_cast< float*>(_v)) {
                _t->anglePitch = *reinterpret_cast< float*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
