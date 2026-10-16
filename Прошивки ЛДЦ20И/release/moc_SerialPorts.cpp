/****************************************************************************
** Meta object code from reading C++ file 'SerialPorts.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../SerialPorts.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SerialPorts.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SerialPorts_t {
    QByteArrayData data[11];
    char stringdata0[198];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialPorts_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialPorts_t qt_meta_stringdata_SerialPorts = {
    {
QT_MOC_LITERAL(0, 0, 11), // "SerialPorts"
QT_MOC_LITERAL(1, 12, 32), // "onSerialPortsEndOfInitialization"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 28), // "onSerialPortsConnectionState"
QT_MOC_LITERAL(4, 75, 24), // "onPrintSerialPortsBuffer"
QT_MOC_LITERAL(5, 100, 14), // "onSendSettings"
QT_MOC_LITERAL(6, 115, 25), // "SerialPortsInitialization"
QT_MOC_LITERAL(7, 141, 21), // "SerialPortsConnection"
QT_MOC_LITERAL(8, 163, 19), // "SerialPortReadyRead"
QT_MOC_LITERAL(9, 183, 8), // "SendData"
QT_MOC_LITERAL(10, 192, 5) // "char*"

    },
    "SerialPorts\0onSerialPortsEndOfInitialization\0"
    "\0onSerialPortsConnectionState\0"
    "onPrintSerialPortsBuffer\0onSendSettings\0"
    "SerialPortsInitialization\0"
    "SerialPortsConnection\0SerialPortReadyRead\0"
    "SendData\0char*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialPorts[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       3,    1,   57,    2, 0x06 /* Public */,
       4,    1,   60,    2, 0x06 /* Public */,
       5,    3,   63,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   70,    2, 0x0a /* Public */,
       7,    1,   71,    2, 0x0a /* Public */,
       8,    0,   74,    2, 0x08 /* Private */,
       9,    2,   75,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::QByteArray,    2,
    QMetaType::Void, QMetaType::UInt, QMetaType::UShort, QMetaType::UShort,    2,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10, QMetaType::UInt,    2,    2,

       0        // eod
};

void SerialPorts::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SerialPorts *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onSerialPortsEndOfInitialization((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->onSerialPortsConnectionState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->onPrintSerialPortsBuffer((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 3: _t->onSendSettings((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2])),(*reinterpret_cast< ushort(*)>(_a[3]))); break;
        case 4: _t->SerialPortsInitialization(); break;
        case 5: _t->SerialPortsConnection((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->SerialPortReadyRead(); break;
        case 7: _t->SendData((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SerialPorts::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPorts::onSerialPortsEndOfInitialization)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SerialPorts::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPorts::onSerialPortsConnectionState)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SerialPorts::*)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPorts::onPrintSerialPortsBuffer)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SerialPorts::*)(uint , ushort , ushort );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialPorts::onSendSettings)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SerialPorts::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SerialPorts.data,
    qt_meta_data_SerialPorts,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SerialPorts::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialPorts::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SerialPorts.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SerialPorts::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void SerialPorts::onSerialPortsEndOfInitialization(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialPorts::onSerialPortsConnectionState(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialPorts::onPrintSerialPortsBuffer(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialPorts::onSendSettings(uint _t1, ushort _t2, ushort _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
