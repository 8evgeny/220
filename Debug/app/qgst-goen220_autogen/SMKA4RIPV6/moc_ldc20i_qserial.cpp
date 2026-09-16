/****************************************************************************
** Meta object code from reading C++ file 'ldc20i_qserial.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/ldc20i/ldc20i_qserial.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ldc20i_qserial.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_LDC20I_QSerial_t {
    uint offsetsAndSizes[24];
    char stringdata0[15];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[3];
    char stringdata4[16];
    char stringdata5[15];
    char stringdata6[16];
    char stringdata7[15];
    char stringdata8[19];
    char stringdata9[6];
    char stringdata10[14];
    char stringdata11[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LDC20I_QSerial_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_LDC20I_QSerial_t qt_meta_stringdata_LDC20I_QSerial = {
    {
        QT_MOC_LITERAL(0, 14),  // "LDC20I_QSerial"
        QT_MOC_LITERAL(15, 9),  // "dataParse"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 2),  // "ba"
        QT_MOC_LITERAL(29, 15),  // "deviceConnected"
        QT_MOC_LITERAL(45, 14),  // "initConnection"
        QT_MOC_LITERAL(60, 15),  // "closeConnection"
        QT_MOC_LITERAL(76, 14),  // "serialReceived"
        QT_MOC_LITERAL(91, 18),  // "serialBytesWritten"
        QT_MOC_LITERAL(110, 5),  // "bytes"
        QT_MOC_LITERAL(116, 13),  // "proc_sendData"
        QT_MOC_LITERAL(130, 20)   // "proc_Thread_Finished"
    },
    "LDC20I_QSerial",
    "dataParse",
    "",
    "ba",
    "deviceConnected",
    "initConnection",
    "closeConnection",
    "serialReceived",
    "serialBytesWritten",
    "bytes",
    "proc_sendData",
    "proc_Thread_Finished"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_LDC20I_QSerial[] = {

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
       4,    0,   65,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,   66,    2, 0x0a,    4 /* Public */,
       6,    0,   67,    2, 0x0a,    5 /* Public */,
       7,    0,   68,    2, 0x0a,    6 /* Public */,
       8,    1,   69,    2, 0x0a,    7 /* Public */,
      10,    1,   72,    2, 0x0a,    9 /* Public */,
      11,    0,   75,    2, 0x0a,   11 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong,    9,
    QMetaType::Void, QMetaType::QByteArray,    3,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LDC20I_QSerial::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LDC20I_QSerial.offsetsAndSizes,
    qt_meta_data_LDC20I_QSerial,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LDC20I_QSerial_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LDC20I_QSerial, std::true_type>,
        // method 'dataParse'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        // method 'deviceConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'initConnection'
        QtPrivate::TypeAndForceComplete<qint32, std::false_type>,
        // method 'closeConnection'
        QtPrivate::TypeAndForceComplete<qint32, std::false_type>,
        // method 'serialReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
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

void LDC20I_QSerial::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LDC20I_QSerial *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataParse((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 1: _t->deviceConnected(); break;
        case 2: { qint32 _r = _t->initConnection();
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = std::move(_r); }  break;
        case 3: { qint32 _r = _t->closeConnection();
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->serialReceived(); break;
        case 5: _t->serialBytesWritten((*reinterpret_cast< std::add_pointer_t<qint64>>(_a[1]))); break;
        case 6: _t->proc_sendData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 7: _t->proc_Thread_Finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LDC20I_QSerial::*)(QByteArray );
            if (_t _q_method = &LDC20I_QSerial::dataParse; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (LDC20I_QSerial::*)();
            if (_t _q_method = &LDC20I_QSerial::deviceConnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *LDC20I_QSerial::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LDC20I_QSerial::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LDC20I_QSerial.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LDC20I_QSerial::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void LDC20I_QSerial::dataParse(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LDC20I_QSerial::deviceConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
