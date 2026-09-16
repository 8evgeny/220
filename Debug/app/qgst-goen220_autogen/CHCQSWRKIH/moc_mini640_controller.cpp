/****************************************************************************
** Meta object code from reading C++ file 'mini640_controller.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/mini640/mini640_controller.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mini640_controller.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MINI640_Controller_t {
    uint offsetsAndSizes[50];
    char stringdata0[19];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[19];
    char stringdata4[9];
    char stringdata5[3];
    char stringdata6[21];
    char stringdata7[17];
    char stringdata8[4];
    char stringdata9[10];
    char stringdata10[19];
    char stringdata11[17];
    char stringdata12[21];
    char stringdata13[5];
    char stringdata14[4];
    char stringdata15[15];
    char stringdata16[17];
    char stringdata17[15];
    char stringdata18[21];
    char stringdata19[19];
    char stringdata20[15];
    char stringdata21[15];
    char stringdata22[21];
    char stringdata23[20];
    char stringdata24[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MINI640_Controller_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MINI640_Controller_t qt_meta_stringdata_MINI640_Controller = {
    {
        QT_MOC_LITERAL(0, 18),  // "MINI640_Controller"
        QT_MOC_LITERAL(19, 17),  // "comInitConnection"
        QT_MOC_LITERAL(37, 0),  // ""
        QT_MOC_LITERAL(38, 18),  // "comCloseConnection"
        QT_MOC_LITERAL(57, 8),  // "sendData"
        QT_MOC_LITERAL(66, 2),  // "ba"
        QT_MOC_LITERAL(69, 20),  // "recieved_MINI640_ANS"
        QT_MOC_LITERAL(90, 16),  // "MINI640_ANS_enum"
        QT_MOC_LITERAL(107, 3),  // "cmd"
        QT_MOC_LITERAL(111, 9),  // "sig_READY"
        QT_MOC_LITERAL(121, 18),  // "comDeviceConnected"
        QT_MOC_LITERAL(140, 16),  // "send_MINI640_CMD"
        QT_MOC_LITERAL(157, 20),  // "MINI640_REG_MAP_enum"
        QT_MOC_LITERAL(178, 4),  // "addr"
        QT_MOC_LITERAL(183, 3),  // "par"
        QT_MOC_LITERAL(187, 14),  // "send_GOWIN_CMD"
        QT_MOC_LITERAL(202, 16),  // "proc_MINI640_CMD"
        QT_MOC_LITERAL(219, 14),  // "proc_GOWIN_CMD"
        QT_MOC_LITERAL(234, 20),  // "proc_deviceConnected"
        QT_MOC_LITERAL(255, 18),  // "proc_timeout_event"
        QT_MOC_LITERAL(274, 14),  // "proc_dataParse"
        QT_MOC_LITERAL(289, 14),  // "proc_InitReady"
        QT_MOC_LITERAL(304, 20),  // "proc_Thread_Finished"
        QT_MOC_LITERAL(325, 19),  // "proc_initConnection"
        QT_MOC_LITERAL(345, 20)   // "proc_closeConnection"
    },
    "MINI640_Controller",
    "comInitConnection",
    "",
    "comCloseConnection",
    "sendData",
    "ba",
    "recieved_MINI640_ANS",
    "MINI640_ANS_enum",
    "cmd",
    "sig_READY",
    "comDeviceConnected",
    "send_MINI640_CMD",
    "MINI640_REG_MAP_enum",
    "addr",
    "par",
    "send_GOWIN_CMD",
    "proc_MINI640_CMD",
    "proc_GOWIN_CMD",
    "proc_deviceConnected",
    "proc_timeout_event",
    "proc_dataParse",
    "proc_InitReady",
    "proc_Thread_Finished",
    "proc_initConnection",
    "proc_closeConnection"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MINI640_Controller[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  116,    2, 0x06,    1 /* Public */,
       3,    0,  117,    2, 0x06,    2 /* Public */,
       4,    1,  118,    2, 0x06,    3 /* Public */,
       6,    1,  121,    2, 0x06,    5 /* Public */,
       9,    0,  124,    2, 0x06,    7 /* Public */,
      10,    0,  125,    2, 0x06,    8 /* Public */,
      11,    3,  126,    2, 0x06,    9 /* Public */,
      15,    1,  133,    2, 0x06,   13 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      16,    3,  136,    2, 0x0a,   15 /* Public */,
      17,    1,  143,    2, 0x0a,   19 /* Public */,
      18,    0,  146,    2, 0x0a,   21 /* Public */,
      19,    0,  147,    2, 0x0a,   22 /* Public */,
      20,    1,  148,    2, 0x0a,   23 /* Public */,
      21,    0,  151,    2, 0x0a,   25 /* Public */,
      22,    0,  152,    2, 0x0a,   26 /* Public */,
      23,    0,  153,    2, 0x0a,   27 /* Public */,
      24,    0,  154,    2, 0x0a,   28 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12, QMetaType::UInt, QMetaType::UInt,   13,    8,   14,
    QMetaType::Void, QMetaType::UInt,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 12, QMetaType::UInt, QMetaType::UInt,   13,    8,   14,
    QMetaType::Void, QMetaType::UInt,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Int,
    QMetaType::Int,

       0        // eod
};

Q_CONSTINIT const QMetaObject MINI640_Controller::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MINI640_Controller.offsetsAndSizes,
    qt_meta_data_MINI640_Controller,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MINI640_Controller_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MINI640_Controller, std::true_type>,
        // method 'comInitConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'comCloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'recieved_MINI640_ANS'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_ANS_enum, std::false_type>,
        // method 'sig_READY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'comDeviceConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'send_MINI640_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_REG_MAP_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'send_GOWIN_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_MINI640_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_REG_MAP_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_GOWIN_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_deviceConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_timeout_event'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_dataParse'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'proc_InitReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_initConnection'
        QtPrivate::TypeAndForceComplete<qint32, std::false_type>,
        // method 'proc_closeConnection'
        QtPrivate::TypeAndForceComplete<qint32, std::false_type>
    >,
    nullptr
} };

void MINI640_Controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MINI640_Controller *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->comInitConnection(); break;
        case 1: _t->comCloseConnection(); break;
        case 2: _t->sendData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 3: _t->recieved_MINI640_ANS((*reinterpret_cast< std::add_pointer_t<MINI640_ANS_enum>>(_a[1]))); break;
        case 4: _t->sig_READY(); break;
        case 5: _t->comDeviceConnected(); break;
        case 6: _t->send_MINI640_CMD((*reinterpret_cast< std::add_pointer_t<MINI640_REG_MAP_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[3]))); break;
        case 7: _t->send_GOWIN_CMD((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1]))); break;
        case 8: _t->proc_MINI640_CMD((*reinterpret_cast< std::add_pointer_t<MINI640_REG_MAP_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[3]))); break;
        case 9: _t->proc_GOWIN_CMD((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1]))); break;
        case 10: _t->proc_deviceConnected(); break;
        case 11: _t->proc_timeout_event(); break;
        case 12: _t->proc_dataParse((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 13: _t->proc_InitReady(); break;
        case 14: _t->proc_Thread_Finished(); break;
        case 15: { qint32 _r = _t->proc_initConnection();
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = std::move(_r); }  break;
        case 16: { qint32 _r = _t->proc_closeConnection();
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MINI640_Controller::*)();
            if (_t _q_method = &MINI640_Controller::comInitConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MINI640_Controller::*)();
            if (_t _q_method = &MINI640_Controller::comCloseConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MINI640_Controller::*)(QByteArray );
            if (_t _q_method = &MINI640_Controller::sendData; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MINI640_Controller::*)(MINI640_ANS_enum );
            if (_t _q_method = &MINI640_Controller::recieved_MINI640_ANS; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MINI640_Controller::*)();
            if (_t _q_method = &MINI640_Controller::sig_READY; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MINI640_Controller::*)();
            if (_t _q_method = &MINI640_Controller::comDeviceConnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MINI640_Controller::*)(MINI640_REG_MAP_enum , quint32 , quint32 );
            if (_t _q_method = &MINI640_Controller::send_MINI640_CMD; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (MINI640_Controller::*)(quint32 );
            if (_t _q_method = &MINI640_Controller::send_GOWIN_CMD; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject *MINI640_Controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MINI640_Controller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MINI640_Controller.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MINI640_Controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void MINI640_Controller::comInitConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MINI640_Controller::comCloseConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MINI640_Controller::sendData(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MINI640_Controller::recieved_MINI640_ANS(MINI640_ANS_enum _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MINI640_Controller::sig_READY()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void MINI640_Controller::comDeviceConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void MINI640_Controller::send_MINI640_CMD(MINI640_REG_MAP_enum _t1, quint32 _t2, quint32 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void MINI640_Controller::send_GOWIN_CMD(quint32 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
