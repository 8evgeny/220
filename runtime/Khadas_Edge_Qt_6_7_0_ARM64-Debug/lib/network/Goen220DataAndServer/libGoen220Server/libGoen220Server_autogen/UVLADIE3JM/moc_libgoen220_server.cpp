/****************************************************************************
** Meta object code from reading C++ file 'libgoen220_server.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../../src/lib/network/Goen220DataAndServer/libGoen220Server/src/libgoen220_server.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'libgoen220_server.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEIntrfENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEIntrfENDCLASS = QtMocHelpers::stringData(
    "Goen220::Server::Intrf",
    "errorOccured",
    "",
    "Error",
    "error",
    "clientConnectionStateChanged",
    "state",
    "cmdReceived",
    "Goen220::Cmd",
    "cmd",
    "payload",
    "ackRequired",
    "setListening",
    "setKeepAliveProcEnabled",
    "setTcpPort",
    "port",
    "setTlmUdpPort",
    "setTlmTransmissionIntervalMs",
    "ms",
    "cmdAck",
    "Goen220::CmdAck",
    "ack",
    "None",
    "TcpServer",
    "TcpClient",
    "Udp"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGoen220SCOPEServerSCOPEIntrfENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       1,  101, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   68,    2, 0x06,    2 /* Public */,
       5,    1,   71,    2, 0x06,    4 /* Public */,
       7,    3,   74,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    1,   81,    2, 0x0a,   10 /* Public */,
      13,    1,   84,    2, 0x0a,   12 /* Public */,
      14,    1,   87,    2, 0x0a,   14 /* Public */,
      16,    1,   90,    2, 0x0a,   16 /* Public */,
      17,    1,   93,    2, 0x0a,   18 /* Public */,
      19,    2,   96,    2, 0x0a,   20 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, 0x80000000 | 8, QMetaType::QByteArray, QMetaType::Bool,    9,   10,   11,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::UShort,   15,
    QMetaType::Void, QMetaType::UShort,   15,
    QMetaType::Void, QMetaType::Int,   18,
    QMetaType::Void, 0x80000000 | 8, 0x80000000 | 20,    9,   21,

 // enums: name, alias, flags, count, data
       3,    3, 0x2,    4,  106,

 // enum data: key, value
      22, uint(Goen220::Server::Intrf::Error::None),
      23, uint(Goen220::Server::Intrf::Error::TcpServer),
      24, uint(Goen220::Server::Intrf::Error::TcpClient),
      25, uint(Goen220::Server::Intrf::Error::Udp),

       0        // eod
};

Q_CONSTINIT const QMetaObject Goen220::Server::Intrf::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEIntrfENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGoen220SCOPEServerSCOPEIntrfENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEIntrfENDCLASS_t,
        // enum 'Error'
        QtPrivate::TypeAndForceComplete<Intrf::Error, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Intrf, std::true_type>,
        // method 'errorOccured'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Error, std::false_type>,
        // method 'clientConnectionStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'cmdReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Goen220::Cmd, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setListening'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setKeepAliveProcEnabled'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'setTcpPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'setTlmUdpPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint16, std::false_type>,
        // method 'setTlmTransmissionIntervalMs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'cmdAck'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Goen220::Cmd, std::false_type>,
        QtPrivate::TypeAndForceComplete<Goen220::CmdAck, std::false_type>
    >,
    nullptr
} };

void Goen220::Server::Intrf::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Intrf *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->errorOccured((*reinterpret_cast< std::add_pointer_t<Error>>(_a[1]))); break;
        case 1: _t->clientConnectionStateChanged((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->cmdReceived((*reinterpret_cast< std::add_pointer_t<Goen220::Cmd>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 3: _t->setListening((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->setKeepAliveProcEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->setTcpPort((*reinterpret_cast< std::add_pointer_t<quint16>>(_a[1]))); break;
        case 6: _t->setTlmUdpPort((*reinterpret_cast< std::add_pointer_t<quint16>>(_a[1]))); break;
        case 7: _t->setTlmTransmissionIntervalMs((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->cmdAck((*reinterpret_cast< std::add_pointer_t<Goen220::Cmd>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Goen220::CmdAck>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Intrf::*)(Error );
            if (_t _q_method = &Intrf::errorOccured; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Intrf::*)(bool );
            if (_t _q_method = &Intrf::clientConnectionStateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Intrf::*)(Goen220::Cmd , QByteArray , bool );
            if (_t _q_method = &Intrf::cmdReceived; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *Goen220::Server::Intrf::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Goen220::Server::Intrf::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEIntrfENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Goen220::Server::Intrf::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Goen220::Server::Intrf::errorOccured(Error _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Goen220::Server::Intrf::clientConnectionStateChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Goen220::Server::Intrf::cmdReceived(Goen220::Cmd _t1, QByteArray _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
