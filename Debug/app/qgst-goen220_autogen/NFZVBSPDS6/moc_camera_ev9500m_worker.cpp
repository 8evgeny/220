/****************************************************************************
** Meta object code from reading C++ file 'camera_ev9500m_worker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/ev9500m/camera_ev9500m_worker.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camera_ev9500m_worker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_Camera_EV9500M_Worker_t {
    uint offsetsAndSizes[34];
    char stringdata0[22];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[12];
    char stringdata4[11];
    char stringdata5[13];
    char stringdata6[7];
    char stringdata7[15];
    char stringdata8[18];
    char stringdata9[4];
    char stringdata10[6];
    char stringdata11[19];
    char stringdata12[9];
    char stringdata13[9];
    char stringdata14[22];
    char stringdata15[8];
    char stringdata16[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_Camera_EV9500M_Worker_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_Camera_EV9500M_Worker_t qt_meta_stringdata_Camera_EV9500M_Worker = {
    {
        QT_MOC_LITERAL(0, 21),  // "Camera_EV9500M_Worker"
        QT_MOC_LITERAL(22, 9),  // "connected"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 11),  // "isConnected"
        QT_MOC_LITERAL(45, 10),  // "sendStatus"
        QT_MOC_LITERAL(56, 12),  // "CameraStatus"
        QT_MOC_LITERAL(69, 6),  // "status"
        QT_MOC_LITERAL(76, 14),  // "proc_VISCA_CMD"
        QT_MOC_LITERAL(91, 17),  // "TVCamera::Command"
        QT_MOC_LITERAL(109, 3),  // "cmd"
        QT_MOC_LITERAL(113, 5),  // "value"
        QT_MOC_LITERAL(119, 18),  // "proc_VISCA_CONNECT"
        QT_MOC_LITERAL(138, 8),  // "portName"
        QT_MOC_LITERAL(147, 8),  // "baudrate"
        QT_MOC_LITERAL(156, 21),  // "proc_VISCA_DISCONNECT"
        QT_MOC_LITERAL(178, 7),  // "timeout"
        QT_MOC_LITERAL(186, 20)   // "proc_Thread_Finished"
    },
    "Camera_EV9500M_Worker",
    "connected",
    "",
    "isConnected",
    "sendStatus",
    "CameraStatus",
    "status",
    "proc_VISCA_CMD",
    "TVCamera::Command",
    "cmd",
    "value",
    "proc_VISCA_CONNECT",
    "portName",
    "baudrate",
    "proc_VISCA_DISCONNECT",
    "timeout",
    "proc_Thread_Finished"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_Camera_EV9500M_Worker[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   56,    2, 0x06,    1 /* Public */,
       4,    1,   59,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    2,   62,    2, 0x0a,    5 /* Public */,
      11,    2,   67,    2, 0x0a,    8 /* Public */,
      14,    0,   72,    2, 0x0a,   11 /* Public */,
      15,    0,   73,    2, 0x0a,   12 /* Public */,
      16,    0,   74,    2, 0x0a,   13 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 5,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8, QMetaType::UInt,    9,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::UInt,   12,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Camera_EV9500M_Worker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Camera_EV9500M_Worker.offsetsAndSizes,
    qt_meta_data_Camera_EV9500M_Worker,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_Camera_EV9500M_Worker_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Camera_EV9500M_Worker, std::true_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'sendStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<CameraStatus, std::false_type>,
        // method 'proc_VISCA_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<TVCamera::Command, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_VISCA_CONNECT'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'proc_VISCA_DISCONNECT'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'timeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Camera_EV9500M_Worker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Camera_EV9500M_Worker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->sendStatus((*reinterpret_cast< std::add_pointer_t<CameraStatus>>(_a[1]))); break;
        case 2: _t->proc_VISCA_CMD((*reinterpret_cast< std::add_pointer_t<TVCamera::Command>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 3: _t->proc_VISCA_CONNECT((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 4: _t->proc_VISCA_DISCONNECT(); break;
        case 5: _t->timeout(); break;
        case 6: _t->proc_Thread_Finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Camera_EV9500M_Worker::*)(bool );
            if (_t _q_method = &Camera_EV9500M_Worker::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Camera_EV9500M_Worker::*)(CameraStatus );
            if (_t _q_method = &Camera_EV9500M_Worker::sendStatus; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *Camera_EV9500M_Worker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Camera_EV9500M_Worker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Camera_EV9500M_Worker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Camera_EV9500M_Worker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void Camera_EV9500M_Worker::connected(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Camera_EV9500M_Worker::sendStatus(CameraStatus _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
