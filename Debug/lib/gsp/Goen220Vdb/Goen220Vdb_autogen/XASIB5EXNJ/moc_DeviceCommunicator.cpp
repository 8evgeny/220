/****************************************************************************
** Meta object code from reading C++ file 'DeviceCommunicator.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../lib/gsp/Goen220Vdb/src/NNetwork/DeviceCommunicator.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DeviceCommunicator.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_DeviceCommunicator_t {
    uint offsetsAndSizes[40];
    char stringdata0[19];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[13];
    char stringdata5[24];
    char stringdata6[17];
    char stringdata7[20];
    char stringdata8[13];
    char stringdata9[8];
    char stringdata10[9];
    char stringdata11[5];
    char stringdata12[21];
    char stringdata13[4];
    char stringdata14[6];
    char stringdata15[18];
    char stringdata16[7];
    char stringdata17[9];
    char stringdata18[4];
    char stringdata19[15];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DeviceCommunicator_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DeviceCommunicator_t qt_meta_stringdata_DeviceCommunicator = {
    {
        QT_MOC_LITERAL(0, 18),  // "DeviceCommunicator"
        QT_MOC_LITERAL(19, 19),  // "commandsSentChanged"
        QT_MOC_LITERAL(39, 0),  // ""
        QT_MOC_LITERAL(40, 7),  // "int32_t"
        QT_MOC_LITERAL(48, 12),  // "commandsSent"
        QT_MOC_LITERAL(61, 23),  // "commandsReceivedChanged"
        QT_MOC_LITERAL(85, 16),  // "commandsReceived"
        QT_MOC_LITERAL(102, 19),  // "commandsLostChanged"
        QT_MOC_LITERAL(122, 12),  // "commandsLost"
        QT_MOC_LITERAL(135, 7),  // "restart"
        QT_MOC_LITERAL(143, 8),  // "sendData"
        QT_MOC_LITERAL(152, 4),  // "data"
        QT_MOC_LITERAL(157, 20),  // "deviceConnectChanged"
        QT_MOC_LITERAL(178, 3),  // "con"
        QT_MOC_LITERAL(182, 5),  // "start"
        QT_MOC_LITERAL(188, 17),  // "startCommunicator"
        QT_MOC_LITERAL(206, 6),  // "setCmd"
        QT_MOC_LITERAL(213, 8),  // "Command*"
        QT_MOC_LITERAL(222, 3),  // "cmd"
        QT_MOC_LITERAL(226, 14)   // "setResponseCmd"
    },
    "DeviceCommunicator",
    "commandsSentChanged",
    "",
    "int32_t",
    "commandsSent",
    "commandsReceivedChanged",
    "commandsReceived",
    "commandsLostChanged",
    "commandsLost",
    "restart",
    "sendData",
    "data",
    "deviceConnectChanged",
    "con",
    "start",
    "startCommunicator",
    "setCmd",
    "Command*",
    "cmd",
    "setResponseCmd"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DeviceCommunicator[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    1 /* Public */,
       5,    1,   71,    2, 0x06,    3 /* Public */,
       7,    1,   74,    2, 0x06,    5 /* Public */,
       9,    0,   77,    2, 0x06,    7 /* Public */,
      10,    1,   78,    2, 0x06,    8 /* Public */,
      12,    1,   81,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      14,    1,   84,    2, 0x0a,   12 /* Public */,
      16,    1,   87,    2, 0x0a,   14 /* Public */,
      19,    1,   90,    2, 0x0a,   16 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    6,
    QMetaType::Void, 0x80000000 | 3,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   11,
    QMetaType::Void, QMetaType::Bool,   13,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void, 0x80000000 | 17,   18,

       0        // eod
};

Q_CONSTINIT const QMetaObject DeviceCommunicator::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DeviceCommunicator.offsetsAndSizes,
    qt_meta_data_DeviceCommunicator,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DeviceCommunicator_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DeviceCommunicator, std::true_type>,
        // method 'commandsSentChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int32_t, std::false_type>,
        // method 'commandsReceivedChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int32_t, std::false_type>,
        // method 'commandsLostChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int32_t, std::false_type>,
        // method 'restart'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        // method 'deviceConnectChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'start'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'setCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Command * const, std::false_type>,
        // method 'setResponseCmd'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Command * const, std::false_type>
    >,
    nullptr
} };

void DeviceCommunicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DeviceCommunicator *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->commandsSentChanged((*reinterpret_cast< std::add_pointer_t<int32_t>>(_a[1]))); break;
        case 1: _t->commandsReceivedChanged((*reinterpret_cast< std::add_pointer_t<int32_t>>(_a[1]))); break;
        case 2: _t->commandsLostChanged((*reinterpret_cast< std::add_pointer_t<int32_t>>(_a[1]))); break;
        case 3: _t->restart(); break;
        case 4: _t->sendData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 5: _t->deviceConnectChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->start((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->setCmd((*reinterpret_cast< std::add_pointer_t<Command*>>(_a[1]))); break;
        case 8: _t->setResponseCmd((*reinterpret_cast< std::add_pointer_t<Command*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DeviceCommunicator::*)(const int32_t );
            if (_t _q_method = &DeviceCommunicator::commandsSentChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DeviceCommunicator::*)(const int32_t );
            if (_t _q_method = &DeviceCommunicator::commandsReceivedChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DeviceCommunicator::*)(const int32_t );
            if (_t _q_method = &DeviceCommunicator::commandsLostChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DeviceCommunicator::*)();
            if (_t _q_method = &DeviceCommunicator::restart; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DeviceCommunicator::*)(const QByteArray & );
            if (_t _q_method = &DeviceCommunicator::sendData; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DeviceCommunicator::*)(const bool );
            if (_t _q_method = &DeviceCommunicator::deviceConnectChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *DeviceCommunicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeviceCommunicator::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceCommunicator.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DeviceCommunicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void DeviceCommunicator::commandsSentChanged(const int32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceCommunicator::commandsReceivedChanged(const int32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DeviceCommunicator::commandsLostChanged(const int32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DeviceCommunicator::restart()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DeviceCommunicator::sendData(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DeviceCommunicator::deviceConnectChanged(const bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
