/****************************************************************************
** Meta object code from reading C++ file 'mini640_worker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/app/mini640/mini640_worker.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mini640_worker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSMINI640_WorkerENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMINI640_WorkerENDCLASS = QtMocHelpers::stringData(
    "MINI640_Worker",
    "recv_MINI640_State",
    "",
    "MINI640_DEV_STATE",
    "dev_state",
    "send_MINI640_CMD",
    "MINI640_REG_MAP_enum",
    "addr",
    "cmd",
    "par",
    "send_GOWIN_CMD",
    "recv_MINI640_ans_notify",
    "sig_READY",
    "sig_CtrlInitConnection",
    "sig_CtrlCloseConnection",
    "proc_recieved_MINI640_ANS",
    "MINI640_ANS_enum",
    "ans",
    "proc_COMDeviceConnected",
    "proc_timerUpdate",
    "proc_MINI640_ExecCmd",
    "MINI640_CMD_enum",
    "uvalue",
    "fvalue",
    "proc_InitReady",
    "proc_CtrlInitConnection",
    "proc_CtrlCloseConnection",
    "proc_Thread_Finished"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMINI640_WorkerENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  116,    2, 0x06,    1 /* Public */,
       5,    3,  119,    2, 0x06,    3 /* Public */,
      10,    1,  126,    2, 0x06,    7 /* Public */,
      11,    0,  129,    2, 0x06,    9 /* Public */,
      12,    0,  130,    2, 0x06,   10 /* Public */,
      13,    0,  131,    2, 0x06,   11 /* Public */,
      14,    0,  132,    2, 0x06,   12 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      15,    1,  133,    2, 0x0a,   13 /* Public */,
      18,    0,  136,    2, 0x0a,   15 /* Public */,
      19,    0,  137,    2, 0x0a,   16 /* Public */,
      20,    3,  138,    2, 0x0a,   17 /* Public */,
      20,    2,  145,    2, 0x2a,   21 /* Public | MethodCloned */,
      20,    1,  150,    2, 0x2a,   24 /* Public | MethodCloned */,
      24,    0,  153,    2, 0x0a,   26 /* Public */,
      25,    0,  154,    2, 0x0a,   27 /* Public */,
      26,    0,  155,    2, 0x0a,   28 /* Public */,
      27,    0,  156,    2, 0x0a,   29 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6, QMetaType::UInt, QMetaType::UInt,    7,    8,    9,
    QMetaType::Void, QMetaType::UInt,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 21, QMetaType::UInt, QMetaType::Float,    8,   22,   23,
    QMetaType::Void, 0x80000000 | 21, QMetaType::UInt,    8,   22,
    QMetaType::Void, 0x80000000 | 21,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MINI640_Worker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSMINI640_WorkerENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMINI640_WorkerENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMINI640_WorkerENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MINI640_Worker, std::true_type>,
        // method 'recv_MINI640_State'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_DEV_STATE, std::false_type>,
        // method 'send_MINI640_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_REG_MAP_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'send_GOWIN_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'recv_MINI640_ans_notify'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_READY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_CtrlInitConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_CtrlCloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_recieved_MINI640_ANS'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_ANS_enum, std::false_type>,
        // method 'proc_COMDeviceConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_timerUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_MINI640_ExecCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_CMD_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<float, std::false_type>,
        // method 'proc_MINI640_ExecCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_CMD_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_MINI640_ExecCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<MINI640_CMD_enum, std::false_type>,
        // method 'proc_InitReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_CtrlInitConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_CtrlCloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MINI640_Worker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MINI640_Worker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->recv_MINI640_State((*reinterpret_cast< std::add_pointer_t<MINI640_DEV_STATE>>(_a[1]))); break;
        case 1: _t->send_MINI640_CMD((*reinterpret_cast< std::add_pointer_t<MINI640_REG_MAP_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[3]))); break;
        case 2: _t->send_GOWIN_CMD((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1]))); break;
        case 3: _t->recv_MINI640_ans_notify(); break;
        case 4: _t->sig_READY(); break;
        case 5: _t->sig_CtrlInitConnection(); break;
        case 6: _t->sig_CtrlCloseConnection(); break;
        case 7: _t->proc_recieved_MINI640_ANS((*reinterpret_cast< std::add_pointer_t<MINI640_ANS_enum>>(_a[1]))); break;
        case 8: _t->proc_COMDeviceConnected(); break;
        case 9: _t->proc_timerUpdate(); break;
        case 10: _t->proc_MINI640_ExecCmd((*reinterpret_cast< std::add_pointer_t<MINI640_CMD_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<float>>(_a[3]))); break;
        case 11: _t->proc_MINI640_ExecCmd((*reinterpret_cast< std::add_pointer_t<MINI640_CMD_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 12: _t->proc_MINI640_ExecCmd((*reinterpret_cast< std::add_pointer_t<MINI640_CMD_enum>>(_a[1]))); break;
        case 13: _t->proc_InitReady(); break;
        case 14: _t->proc_CtrlInitConnection(); break;
        case 15: _t->proc_CtrlCloseConnection(); break;
        case 16: _t->proc_Thread_Finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MINI640_Worker::*)(MINI640_DEV_STATE );
            if (_t _q_method = &MINI640_Worker::recv_MINI640_State; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MINI640_Worker::*)(MINI640_REG_MAP_enum , quint32 , quint32 );
            if (_t _q_method = &MINI640_Worker::send_MINI640_CMD; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MINI640_Worker::*)(quint32 );
            if (_t _q_method = &MINI640_Worker::send_GOWIN_CMD; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MINI640_Worker::*)();
            if (_t _q_method = &MINI640_Worker::recv_MINI640_ans_notify; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MINI640_Worker::*)();
            if (_t _q_method = &MINI640_Worker::sig_READY; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MINI640_Worker::*)();
            if (_t _q_method = &MINI640_Worker::sig_CtrlInitConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MINI640_Worker::*)();
            if (_t _q_method = &MINI640_Worker::sig_CtrlCloseConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
    }
}

const QMetaObject *MINI640_Worker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MINI640_Worker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMINI640_WorkerENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MINI640_Worker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void MINI640_Worker::recv_MINI640_State(MINI640_DEV_STATE _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MINI640_Worker::send_MINI640_CMD(MINI640_REG_MAP_enum _t1, quint32 _t2, quint32 _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MINI640_Worker::send_GOWIN_CMD(quint32 _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MINI640_Worker::recv_MINI640_ans_notify()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MINI640_Worker::sig_READY()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void MINI640_Worker::sig_CtrlInitConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void MINI640_Worker::sig_CtrlCloseConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
