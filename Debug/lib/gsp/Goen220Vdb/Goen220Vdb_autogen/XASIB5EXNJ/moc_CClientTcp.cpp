/****************************************************************************
** Meta object code from reading C++ file 'CClientTcp.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../lib/gsp/Goen220Vdb/src/NNetwork/CClientTcp.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CClientTcp.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_NNetwork__CClientTcp_t {
    uint offsetsAndSizes[36];
    char stringdata0[21];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[4];
    char stringdata4[13];
    char stringdata5[5];
    char stringdata6[16];
    char stringdata7[9];
    char stringdata8[26];
    char stringdata9[16];
    char stringdata10[12];
    char stringdata11[29];
    char stringdata12[12];
    char stringdata13[13];
    char stringdata14[29];
    char stringdata15[12];
    char stringdata16[9];
    char stringdata17[9];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NNetwork__CClientTcp_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NNetwork__CClientTcp_t qt_meta_stringdata_NNetwork__CClientTcp = {
    {
        QT_MOC_LITERAL(0, 20),  // "NNetwork::CClientTcp"
        QT_MOC_LITERAL(21, 19),  // "stateConnectChanged"
        QT_MOC_LITERAL(41, 0),  // ""
        QT_MOC_LITERAL(42, 3),  // "con"
        QT_MOC_LITERAL(46, 12),  // "receivedData"
        QT_MOC_LITERAL(59, 4),  // "data"
        QT_MOC_LITERAL(64, 15),  // "connectToServer"
        QT_MOC_LITERAL(80, 8),  // "sendData"
        QT_MOC_LITERAL(89, 25),  // "initializationInNewThread"
        QT_MOC_LITERAL(115, 15),  // "socketReadyRead"
        QT_MOC_LITERAL(131, 11),  // "acceptError"
        QT_MOC_LITERAL(143, 28),  // "QAbstractSocket::SocketError"
        QT_MOC_LITERAL(172, 11),  // "socketError"
        QT_MOC_LITERAL(184, 12),  // "stateChanged"
        QT_MOC_LITERAL(197, 28),  // "QAbstractSocket::SocketState"
        QT_MOC_LITERAL(226, 11),  // "socketState"
        QT_MOC_LITERAL(238, 8),  // "hostName"
        QT_MOC_LITERAL(247, 8)   // "hostPort"
    },
    "NNetwork::CClientTcp",
    "stateConnectChanged",
    "",
    "con",
    "receivedData",
    "data",
    "connectToServer",
    "sendData",
    "initializationInNewThread",
    "socketReadyRead",
    "acceptError",
    "QAbstractSocket::SocketError",
    "socketError",
    "stateChanged",
    "QAbstractSocket::SocketState",
    "socketState",
    "hostName",
    "hostPort"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NNetwork__CClientTcp[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       2,   82, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x06,    3 /* Public */,
       4,    1,   65,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    1,   68,    2, 0x0a,    7 /* Public */,
       7,    1,   71,    2, 0x0a,    9 /* Public */,
       8,    0,   74,    2, 0x09,   11 /* Protected */,
       9,    0,   75,    2, 0x08,   12 /* Private */,
      10,    1,   76,    2, 0x08,   13 /* Private */,
      13,    1,   79,    2, 0x08,   15 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QByteArray,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::QByteArray,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,

 // properties: name, type, flags
      16, QMetaType::QString, 0x00015103, uint(-1), 0,
      17, QMetaType::UShort, 0x00015103, uint(-1), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject NNetwork::CClientTcp::staticMetaObject = { {
    QMetaObject::SuperData::link<NCore::CNewThread::staticMetaObject>(),
    qt_meta_stringdata_NNetwork__CClientTcp.offsetsAndSizes,
    qt_meta_data_NNetwork__CClientTcp,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NNetwork__CClientTcp_t,
        // property 'hostName'
        QtPrivate::TypeAndForceComplete<QString, std::true_type>,
        // property 'hostPort'
        QtPrivate::TypeAndForceComplete<ushort, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CClientTcp, std::true_type>,
        // method 'stateConnectChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'receivedData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray, std::false_type>,
        // method 'connectToServer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const bool, std::false_type>,
        // method 'sendData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray, std::false_type>,
        // method 'initializationInNewThread'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'socketReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'acceptError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>,
        // method 'stateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketState, std::false_type>
    >,
    nullptr
} };

void NNetwork::CClientTcp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CClientTcp *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->stateConnectChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->receivedData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 2: _t->connectToServer((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->sendData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 4: _t->initializationInNewThread(); break;
        case 5: _t->socketReadyRead(); break;
        case 6: _t->acceptError((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 7: _t->stateChanged((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketState>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketState >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CClientTcp::*)(const bool );
            if (_t _q_method = &CClientTcp::stateConnectChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CClientTcp::*)(const QByteArray );
            if (_t _q_method = &CClientTcp::receivedData; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CClientTcp *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->getHostName(); break;
        case 1: *reinterpret_cast< ushort*>(_v) = _t->getHostPort(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CClientTcp *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setHostName(*reinterpret_cast< QString*>(_v)); break;
        case 1: _t->setHostPort(*reinterpret_cast< ushort*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *NNetwork::CClientTcp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NNetwork::CClientTcp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NNetwork__CClientTcp.stringdata0))
        return static_cast<void*>(this);
    return NCore::CNewThread::qt_metacast(_clname);
}

int NNetwork::CClientTcp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = NCore::CNewThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void NNetwork::CClientTcp::stateConnectChanged(const bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NNetwork::CClientTcp::receivedData(const QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
