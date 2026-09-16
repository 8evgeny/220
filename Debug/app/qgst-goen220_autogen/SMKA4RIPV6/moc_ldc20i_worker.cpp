/****************************************************************************
** Meta object code from reading C++ file 'ldc20i_worker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/ldc20i/ldc20i_worker.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ldc20i_worker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_LDC20I_Worker_t {
    uint offsetsAndSizes[32];
    char stringdata0[14];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[10];
    char stringdata5[13];
    char stringdata6[13];
    char stringdata7[4];
    char stringdata8[6];
    char stringdata9[25];
    char stringdata10[13];
    char stringdata11[24];
    char stringdata12[17];
    char stringdata13[24];
    char stringdata14[16];
    char stringdata15[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LDC20I_Worker_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_LDC20I_Worker_t qt_meta_stringdata_LDC20I_Worker = {
    {
        QT_MOC_LITERAL(0, 13),  // "LDC20I_Worker"
        QT_MOC_LITERAL(14, 14),  // "send_LDC_State"
        QT_MOC_LITERAL(29, 0),  // ""
        QT_MOC_LITERAL(30, 9),  // "LDC_STATE"
        QT_MOC_LITERAL(40, 9),  // "dev_state"
        QT_MOC_LITERAL(50, 12),  // "send_LDC_CMD"
        QT_MOC_LITERAL(63, 12),  // "LDC_CMD_enum"
        QT_MOC_LITERAL(76, 3),  // "cmd"
        QT_MOC_LITERAL(80, 5),  // "value"
        QT_MOC_LITERAL(86, 24),  // "proc_recieved_LDC_DS_ANS"
        QT_MOC_LITERAL(111, 12),  // "LDC_ANS_enum"
        QT_MOC_LITERAL(124, 23),  // "proc_COMDeviceConnected"
        QT_MOC_LITERAL(148, 16),  // "proc_timerUpdate"
        QT_MOC_LITERAL(165, 23),  // "proc_timeoutTimerUpdate"
        QT_MOC_LITERAL(189, 15),  // "proc_LDCSendCmd"
        QT_MOC_LITERAL(205, 20)   // "proc_Thread_Finished"
    },
    "LDC20I_Worker",
    "send_LDC_State",
    "",
    "LDC_STATE",
    "dev_state",
    "send_LDC_CMD",
    "LDC_CMD_enum",
    "cmd",
    "value",
    "proc_recieved_LDC_DS_ANS",
    "LDC_ANS_enum",
    "proc_COMDeviceConnected",
    "proc_timerUpdate",
    "proc_timeoutTimerUpdate",
    "proc_LDCSendCmd",
    "proc_Thread_Finished"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_LDC20I_Worker[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x06,    1 /* Public */,
       5,    2,   65,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       9,    2,   70,    2, 0x0a,    6 /* Public */,
      11,    0,   75,    2, 0x0a,    9 /* Public */,
      12,    0,   76,    2, 0x0a,   10 /* Public */,
      13,    0,   77,    2, 0x0a,   11 /* Public */,
      14,    2,   78,    2, 0x0a,   12 /* Public */,
      15,    0,   83,    2, 0x0a,   15 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, QMetaType::UInt,    7,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10, 0x80000000 | 3,    7,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, QMetaType::UInt,    7,    8,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LDC20I_Worker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LDC20I_Worker.offsetsAndSizes,
    qt_meta_data_LDC20I_Worker,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LDC20I_Worker_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LDC20I_Worker, std::true_type>,
        // method 'send_LDC_State'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_STATE, std::false_type>,
        // method 'send_LDC_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_CMD_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_recieved_LDC_DS_ANS'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_ANS_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_STATE, std::false_type>,
        // method 'proc_COMDeviceConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_timerUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_timeoutTimerUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_LDCSendCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<LDC_CMD_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LDC20I_Worker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LDC20I_Worker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->send_LDC_State((*reinterpret_cast< std::add_pointer_t<LDC_STATE>>(_a[1]))); break;
        case 1: _t->send_LDC_CMD((*reinterpret_cast< std::add_pointer_t<LDC_CMD_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 2: _t->proc_recieved_LDC_DS_ANS((*reinterpret_cast< std::add_pointer_t<LDC_ANS_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<LDC_STATE>>(_a[2]))); break;
        case 3: _t->proc_COMDeviceConnected(); break;
        case 4: _t->proc_timerUpdate(); break;
        case 5: _t->proc_timeoutTimerUpdate(); break;
        case 6: _t->proc_LDCSendCmd((*reinterpret_cast< std::add_pointer_t<LDC_CMD_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 7: _t->proc_Thread_Finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LDC20I_Worker::*)(LDC_STATE );
            if (_t _q_method = &LDC20I_Worker::send_LDC_State; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LDC20I_Worker::*)(LDC_CMD_enum , quint32 );
            if (_t _q_method = &LDC20I_Worker::send_LDC_CMD; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *LDC20I_Worker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LDC20I_Worker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LDC20I_Worker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LDC20I_Worker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void LDC20I_Worker::send_LDC_State(LDC_STATE _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LDC20I_Worker::send_LDC_CMD(LDC_CMD_enum _t1, quint32 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
