/****************************************************************************
** Meta object code from reading C++ file 'ldc20i_controller.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/ldc20i/ldc20i_controller.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ldc20i_controller.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_LDC20I_Controller_t {
    uint offsetsAndSizes[40];
    char stringdata0[18];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[19];
    char stringdata4[9];
    char stringdata5[3];
    char stringdata6[20];
    char stringdata7[13];
    char stringdata8[4];
    char stringdata9[10];
    char stringdata10[10];
    char stringdata11[19];
    char stringdata12[13];
    char stringdata13[13];
    char stringdata14[4];
    char stringdata15[13];
    char stringdata16[21];
    char stringdata17[17];
    char stringdata18[15];
    char stringdata19[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LDC20I_Controller_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_LDC20I_Controller_t qt_meta_stringdata_LDC20I_Controller = {
    {
        QT_MOC_LITERAL(0, 17),  // "LDC20I_Controller"
        QT_MOC_LITERAL(18, 17),  // "comInitConnection"
        QT_MOC_LITERAL(36, 0),  // ""
        QT_MOC_LITERAL(37, 18),  // "comCloseConnection"
        QT_MOC_LITERAL(56, 8),  // "sendData"
        QT_MOC_LITERAL(65, 2),  // "ba"
        QT_MOC_LITERAL(68, 19),  // "recieved_LDC_DS_ANS"
        QT_MOC_LITERAL(88, 12),  // "LDC_ANS_enum"
        QT_MOC_LITERAL(101, 3),  // "cmd"
        QT_MOC_LITERAL(105, 9),  // "LDC_STATE"
        QT_MOC_LITERAL(115, 9),  // "dev_state"
        QT_MOC_LITERAL(125, 18),  // "comDeviceConnected"
        QT_MOC_LITERAL(144, 12),  // "send_LDC_CMD"
        QT_MOC_LITERAL(157, 12),  // "LDC_CMD_enum"
        QT_MOC_LITERAL(170, 3),  // "par"
        QT_MOC_LITERAL(174, 12),  // "proc_LDC_CMD"
        QT_MOC_LITERAL(187, 20),  // "proc_deviceConnected"
        QT_MOC_LITERAL(208, 16),  // "proc_timerUpdate"
        QT_MOC_LITERAL(225, 14),  // "proc_dataParse"
        QT_MOC_LITERAL(240, 20)   // "proc_Thread_Finished"
    },
    "LDC20I_Controller",
    "comInitConnection",
    "",
    "comCloseConnection",
    "sendData",
    "ba",
    "recieved_LDC_DS_ANS",
    "LDC_ANS_enum",
    "cmd",
    "LDC_STATE",
    "dev_state",
    "comDeviceConnected",
    "send_LDC_CMD",
    "LDC_CMD_enum",
    "par",
    "proc_LDC_CMD",
    "proc_deviceConnected",
    "proc_timerUpdate",
    "proc_dataParse",
    "proc_Thread_Finished"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_LDC20I_Controller[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    1 /* Public */,
       3,    0,   81,    2, 0x06,    2 /* Public */,
       4,    1,   82,    2, 0x06,    3 /* Public */,
       6,    2,   85,    2, 0x06,    5 /* Public */,
      11,    0,   90,    2, 0x06,    8 /* Public */,
      12,    2,   91,    2, 0x06,    9 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    2,   96,    2, 0x0a,   12 /* Public */,
      16,    0,  101,    2, 0x0a,   15 /* Public */,
      17,    0,  102,    2, 0x0a,   16 /* Public */,
      18,    1,  103,    2, 0x0a,   17 /* Public */,
      19,    0,  106,    2, 0x0a,   19 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    5,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 9,    8,   10,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 13, QMetaType::UInt,    8,   14,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 13, QMetaType::UInt,    8,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    5,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LDC20I_Controller::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LDC20I_Controller.offsetsAndSizes,
    qt_meta_data_LDC20I_Controller,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LDC20I_Controller_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LDC20I_Controller, std::true_type>,
        // method 'comInitConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'comCloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'recieved_LDC_DS_ANS'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_ANS_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_STATE, std::false_type>,
        // method 'comDeviceConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send_LDC_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_CMD_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_LDC_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_CMD_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_deviceConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_timerUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_dataParse'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LDC20I_Controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LDC20I_Controller *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->comInitConnection(); break;
        case 1: _t->comCloseConnection(); break;
        case 2: _t->sendData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 3: _t->recieved_LDC_DS_ANS((*reinterpret_cast< std::add_pointer_t<LDC_ANS_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<LDC_STATE>>(_a[2]))); break;
        case 4: _t->comDeviceConnected(); break;
        case 5: _t->send_LDC_CMD((*reinterpret_cast< std::add_pointer_t<LDC_CMD_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 6: _t->proc_LDC_CMD((*reinterpret_cast< std::add_pointer_t<LDC_CMD_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 7: _t->proc_deviceConnected(); break;
        case 8: _t->proc_timerUpdate(); break;
        case 9: _t->proc_dataParse((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 10: _t->proc_Thread_Finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LDC20I_Controller::*)();
            if (_t _q_method = &LDC20I_Controller::comInitConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LDC20I_Controller::*)();
            if (_t _q_method = &LDC20I_Controller::comCloseConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (LDC20I_Controller::*)(QByteArray );
            if (_t _q_method = &LDC20I_Controller::sendData; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (LDC20I_Controller::*)(LDC_ANS_enum , LDC_STATE );
            if (_t _q_method = &LDC20I_Controller::recieved_LDC_DS_ANS; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (LDC20I_Controller::*)();
            if (_t _q_method = &LDC20I_Controller::comDeviceConnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (LDC20I_Controller::*)(LDC_CMD_enum , quint32 );
            if (_t _q_method = &LDC20I_Controller::send_LDC_CMD; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *LDC20I_Controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LDC20I_Controller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LDC20I_Controller.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LDC20I_Controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void LDC20I_Controller::comInitConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void LDC20I_Controller::comCloseConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void LDC20I_Controller::sendData(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void LDC20I_Controller::recieved_LDC_DS_ANS(LDC_ANS_enum _t1, LDC_STATE _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void LDC20I_Controller::comDeviceConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void LDC20I_Controller::send_LDC_CMD(LDC_CMD_enum _t1, quint32 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
