/****************************************************************************
** Meta object code from reading C++ file 'mini640_qserial.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/mini640/mini640_qserial.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mini640_qserial.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_MINI640_QSerial_t {
    uint offsetsAndSizes[32];
    char stringdata0[16];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[3];
    char stringdata4[16];
    char stringdata5[10];
    char stringdata6[15];
    char stringdata7[16];
    char stringdata8[15];
    char stringdata9[12];
    char stringdata10[29];
    char stringdata11[6];
    char stringdata12[19];
    char stringdata13[6];
    char stringdata14[14];
    char stringdata15[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MINI640_QSerial_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MINI640_QSerial_t qt_meta_stringdata_MINI640_QSerial = {
    {
        QT_MOC_LITERAL(0, 15),  // "MINI640_QSerial"
        QT_MOC_LITERAL(16, 9),  // "dataParse"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 2),  // "ba"
        QT_MOC_LITERAL(30, 15),  // "deviceConnected"
        QT_MOC_LITERAL(46, 9),  // "sig_READY"
        QT_MOC_LITERAL(56, 14),  // "initConnection"
        QT_MOC_LITERAL(71, 15),  // "closeConnection"
        QT_MOC_LITERAL(87, 14),  // "serialReceived"
        QT_MOC_LITERAL(102, 11),  // "serialError"
        QT_MOC_LITERAL(114, 28),  // "QSerialPort::SerialPortError"
        QT_MOC_LITERAL(143, 5),  // "error"
        QT_MOC_LITERAL(149, 18),  // "serialBytesWritten"
        QT_MOC_LITERAL(168, 5),  // "bytes"
        QT_MOC_LITERAL(174, 13),  // "proc_sendData"
        QT_MOC_LITERAL(188, 20)   // "proc_Thread_Finished"
    },
    "MINI640_QSerial",
    "dataParse",
    "",
    "ba",
    "deviceConnected",
    "sig_READY",
    "initConnection",
    "closeConnection",
    "serialReceived",
    "serialError",
    "QSerialPort::SerialPortError",
    "error",
    "serialBytesWritten",
    "bytes",
    "proc_sendData",
    "proc_Thread_Finished"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MINI640_QSerial[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   74,    2, 0x06,    1 /* Public */,
       4,    0,   77,    2, 0x06,    3 /* Public */,
       5,    0,   78,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    0,   79,    2, 0x0a,    5 /* Public */,
       7,    0,   80,    2, 0x0a,    6 /* Public */,
       8,    0,   81,    2, 0x0a,    7 /* Public */,
       9,    1,   82,    2, 0x0a,    8 /* Public */,
      12,    1,   85,    2, 0x0a,   10 /* Public */,
      14,    1,   88,    2, 0x0a,   12 /* Public */,
      15,    0,   91,    2, 0x0a,   14 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, QMetaType::LongLong,   13,
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MINI640_QSerial::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MINI640_QSerial.offsetsAndSizes,
    qt_meta_data_MINI640_QSerial,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MINI640_QSerial_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MINI640_QSerial, std::true_type>,
        // method 'dataParse'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'deviceConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_READY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'initConnection'
        QtPrivate::TypeAndForceComplete<qint32, std::false_type>,
        // method 'closeConnection'
        QtPrivate::TypeAndForceComplete<qint32, std::false_type>,
        // method 'serialReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'serialError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QSerialPort::SerialPortError, std::false_type>,
        // method 'serialBytesWritten'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
        // method 'proc_sendData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MINI640_QSerial::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MINI640_QSerial *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataParse((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 1: _t->deviceConnected(); break;
        case 2: _t->sig_READY(); break;
        case 3: { qint32 _r = _t->initConnection();
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = std::move(_r); }  break;
        case 4: { qint32 _r = _t->closeConnection();
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = std::move(_r); }  break;
        case 5: _t->serialReceived(); break;
        case 6: _t->serialError((*reinterpret_cast< std::add_pointer_t<QSerialPort::SerialPortError>>(_a[1]))); break;
        case 7: _t->serialBytesWritten((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 8: _t->proc_sendData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 9: _t->proc_Thread_Finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MINI640_QSerial::*)(QByteArray );
            if (_t _q_method = &MINI640_QSerial::dataParse; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MINI640_QSerial::*)();
            if (_t _q_method = &MINI640_QSerial::deviceConnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MINI640_QSerial::*)();
            if (_t _q_method = &MINI640_QSerial::sig_READY; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *MINI640_QSerial::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MINI640_QSerial::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MINI640_QSerial.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MINI640_QSerial::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void MINI640_QSerial::dataParse(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MINI640_QSerial::deviceConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MINI640_QSerial::sig_READY()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
