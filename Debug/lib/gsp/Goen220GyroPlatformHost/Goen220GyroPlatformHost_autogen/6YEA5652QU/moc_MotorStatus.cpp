/****************************************************************************
** Meta object code from reading C++ file 'MotorStatus.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../lib/gsp/Goen220GyroPlatformHost/include/MotorStatus.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MotorStatus.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MotorStatus_t {
    uint offsetsAndSizes[14];
    char stringdata0[12];
    char stringdata1[5];
    char stringdata2[8];
    char stringdata3[7];
    char stringdata4[6];
    char stringdata5[6];
    char stringdata6[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MotorStatus_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MotorStatus_t qt_meta_stringdata_MotorStatus = {
    {
        QT_MOC_LITERAL(0, 11),  // "MotorStatus"
        QT_MOC_LITERAL(12, 4),  // "mode"
        QT_MOC_LITERAL(17, 7),  // "uint8_t"
        QT_MOC_LITERAL(25, 6),  // "errors"
        QT_MOC_LITERAL(32, 5),  // "flags"
        QT_MOC_LITERAL(38, 5),  // "angle"
        QT_MOC_LITERAL(44, 5)   // "speed"
    },
    "MotorStatus",
    "mode",
    "uint8_t",
    "errors",
    "flags",
    "angle",
    "speed"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MotorStatus[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       5,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       4,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       3, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       4, 0x80000000 | 2, 0x0001500b, uint(-1), 0,
       5, QMetaType::Float, 0x00015003, uint(-1), 0,
       6, QMetaType::Float, 0x00015003, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject MotorStatus::staticMetaObject = { {
    nullptr,
    qt_meta_stringdata_MotorStatus.offsetsAndSizes,
    qt_meta_data_MotorStatus,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MotorStatus_t,
        // property 'mode'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'errors'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'flags'
        QtPrivate::TypeAndForceComplete<uint8_t, std::true_type>,
        // property 'angle'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // property 'speed'
        QtPrivate::TypeAndForceComplete<float, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MotorStatus, std::true_type>
    >,
    nullptr
} };

void MotorStatus::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
if (_c == QMetaObject::ReadProperty) {
        auto *_t = reinterpret_cast<MotorStatus *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< uint8_t*>(_v) = _t->mode; break;
        case 1: *reinterpret_cast< uint8_t*>(_v) = _t->errors; break;
        case 2: *reinterpret_cast< uint8_t*>(_v) = _t->flags; break;
        case 3: *reinterpret_cast< float*>(_v) = _t->angle; break;
        case 4: *reinterpret_cast< float*>(_v) = _t->speed; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = reinterpret_cast<MotorStatus *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->mode != *reinterpret_cast< uint8_t*>(_v)) {
                _t->mode = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 1:
            if (_t->errors != *reinterpret_cast< uint8_t*>(_v)) {
                _t->errors = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 2:
            if (_t->flags != *reinterpret_cast< uint8_t*>(_v)) {
                _t->flags = *reinterpret_cast< uint8_t*>(_v);
            }
            break;
        case 3:
            if (_t->angle != *reinterpret_cast< float*>(_v)) {
                _t->angle = *reinterpret_cast< float*>(_v);
            }
            break;
        case 4:
            if (_t->speed != *reinterpret_cast< float*>(_v)) {
                _t->speed = *reinterpret_cast< float*>(_v);
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
