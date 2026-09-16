/****************************************************************************
** Meta object code from reading C++ file 'lens_j200_worker.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/j200/lens_j200_worker.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lens_j200_worker.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_LensJ200Worker_t {
    uint offsetsAndSizes[28];
    char stringdata0[15];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[10];
    char stringdata5[9];
    char stringdata6[18];
    char stringdata7[14];
    char stringdata8[14];
    char stringdata9[10];
    char stringdata10[5];
    char stringdata11[14];
    char stringdata12[15];
    char stringdata13[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LensJ200Worker_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_LensJ200Worker_t qt_meta_stringdata_LensJ200Worker = {
    {
        QT_MOC_LITERAL(0, 14),  // "LensJ200Worker"
        QT_MOC_LITERAL(15, 9),  // "connected"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 9),  // "onConnect"
        QT_MOC_LITERAL(36, 9),  // "isConnect"
        QT_MOC_LITERAL(46, 8),  // "portName"
        QT_MOC_LITERAL(55, 17),  // "onCloseConnection"
        QT_MOC_LITERAL(73, 13),  // "onSendRequest"
        QT_MOC_LITERAL(87, 13),  // "Lens::Command"
        QT_MOC_LITERAL(101, 9),  // "commandID"
        QT_MOC_LITERAL(111, 4),  // "data"
        QT_MOC_LITERAL(116, 13),  // "onReceivedAck"
        QT_MOC_LITERAL(130, 14),  // "onTimerTimeout"
        QT_MOC_LITERAL(145, 20)   // "proc_Thread_Finished"
    },
    "LensJ200Worker",
    "connected",
    "",
    "onConnect",
    "isConnect",
    "portName",
    "onCloseConnection",
    "onSendRequest",
    "Lens::Command",
    "commandID",
    "data",
    "onReceivedAck",
    "onTimerTimeout",
    "proc_Thread_Finished"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_LensJ200Worker[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       3,    2,   65,    2, 0x0a,    3 /* Public */,
       6,    0,   70,    2, 0x0a,    6 /* Public */,
       7,    2,   71,    2, 0x0a,    7 /* Public */,
       7,    1,   76,    2, 0x2a,   10 /* Public | MethodCloned */,
      11,    1,   79,    2, 0x0a,   12 /* Public */,
      12,    0,   82,    2, 0x0a,   14 /* Public */,
      13,    0,   83,    2, 0x0a,   15 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    4,    5,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8, QMetaType::QByteArray,    9,   10,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LensJ200Worker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LensJ200Worker.offsetsAndSizes,
    qt_meta_data_LensJ200Worker,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LensJ200Worker_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LensJ200Worker, std::true_type>,
        // method 'connected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onConnect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'onCloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSendRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Lens::Command, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        // method 'onSendRequest'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Lens::Command, std::false_type>,
        // method 'onReceivedAck'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onTimerTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LensJ200Worker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LensJ200Worker *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connected((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->onConnect((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->onCloseConnection(); break;
        case 3: _t->onSendRequest((*reinterpret_cast< std::add_pointer_t<Lens::Command>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 4: _t->onSendRequest((*reinterpret_cast< std::add_pointer_t<Lens::Command>>(_a[1]))); break;
        case 5: _t->onReceivedAck((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->onTimerTimeout(); break;
        case 7: _t->proc_Thread_Finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LensJ200Worker::*)(bool );
            if (_t _q_method = &LensJ200Worker::connected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *LensJ200Worker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LensJ200Worker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LensJ200Worker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LensJ200Worker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void LensJ200Worker::connected(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
