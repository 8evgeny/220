/****************************************************************************
** Meta object code from reading C++ file 'server_impl.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../../src/lib/network/Goen220DataAndServer/libGoen220Server/src/_internal/server_impl.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'server_impl.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEServerImplENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEServerImplENDCLASS = QtMocHelpers::stringData(
    "Goen220::Server::ServerImpl",
    "setListening",
    "",
    "state",
    "setKeepAliveProcEnabled",
    "setTcpPort",
    "port",
    "setTlmUdpPort",
    "setTlmTransmissionIntervalMs",
    "ms",
    "cmdAck",
    "Goen220::Cmd",
    "cmd",
    "Goen220::CmdAck",
    "ack"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGoen220SCOPEServerSCOPEServerImplENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x0a,    1 /* Public */,
       4,    1,   53,    2, 0x0a,    3 /* Public */,
       5,    1,   56,    2, 0x0a,    5 /* Public */,
       7,    1,   59,    2, 0x0a,    7 /* Public */,
       8,    1,   62,    2, 0x0a,    9 /* Public */,
      10,    2,   65,    2, 0x0a,   11 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::UShort,    6,
    QMetaType::Void, QMetaType::UShort,    6,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, 0x80000000 | 11, 0x80000000 | 13,   12,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject Goen220::Server::ServerImpl::staticMetaObject = { {
    QMetaObject::SuperData::link<Intrf::staticMetaObject>(),
    qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEServerImplENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGoen220SCOPEServerSCOPEServerImplENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEServerImplENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ServerImpl, std::true_type>,
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

void Goen220::Server::ServerImpl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ServerImpl *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->setListening((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->setKeepAliveProcEnabled((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->setTcpPort((*reinterpret_cast< std::add_pointer_t<quint16>>(_a[1]))); break;
        case 3: _t->setTlmUdpPort((*reinterpret_cast< std::add_pointer_t<quint16>>(_a[1]))); break;
        case 4: _t->setTlmTransmissionIntervalMs((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->cmdAck((*reinterpret_cast< std::add_pointer_t<Goen220::Cmd>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<Goen220::CmdAck>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *Goen220::Server::ServerImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Goen220::Server::ServerImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGoen220SCOPEServerSCOPEServerImplENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return Intrf::qt_metacast(_clname);
}

int Goen220::Server::ServerImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Intrf::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
