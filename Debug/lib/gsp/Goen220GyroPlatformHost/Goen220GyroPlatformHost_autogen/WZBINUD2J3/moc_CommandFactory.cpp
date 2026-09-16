/****************************************************************************
** Meta object code from reading C++ file 'CommandFactory.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../lib/gsp/Goen220GyroPlatformHost/src/NCore/CommandFactory.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CommandFactory.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CommandFactory_t {
    uint offsetsAndSizes[24];
    char stringdata0[15];
    char stringdata1[29];
    char stringdata2[1];
    char stringdata3[8];
    char stringdata4[16];
    char stringdata5[15];
    char stringdata6[9];
    char stringdata7[4];
    char stringdata8[15];
    char stringdata9[8];
    char stringdata10[8];
    char stringdata11[11];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CommandFactory_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CommandFactory_t qt_meta_stringdata_CommandFactory = {
    {
        QT_MOC_LITERAL(0, 14),  // "CommandFactory"
        QT_MOC_LITERAL(15, 28),  // "invalidMessageCounterChanged"
        QT_MOC_LITERAL(44, 0),  // ""
        QT_MOC_LITERAL(45, 7),  // "int32_t"
        QT_MOC_LITERAL(53, 15),  // "invalidMessages"
        QT_MOC_LITERAL(69, 14),  // "commandCreated"
        QT_MOC_LITERAL(84, 8),  // "Command*"
        QT_MOC_LITERAL(93, 3),  // "cmd"
        QT_MOC_LITERAL(97, 14),  // "executeMessage"
        QT_MOC_LITERAL(112, 7),  // "Message"
        QT_MOC_LITERAL(120, 7),  // "message"
        QT_MOC_LITERAL(128, 10)   // "setMessage"
    },
    "CommandFactory",
    "invalidMessageCounterChanged",
    "",
    "int32_t",
    "invalidMessages",
    "commandCreated",
    "Command*",
    "cmd",
    "executeMessage",
    "Message",
    "message",
    "setMessage"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CommandFactory[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   38,    2, 0x06,    1 /* Public */,
       5,    1,   41,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       8,    1,   44,    2, 0x0a,    5 /* Public */,
      11,    1,   47,    2, 0x0a,    7 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

Q_CONSTINIT const QMetaObject CommandFactory::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CommandFactory.offsetsAndSizes,
    qt_meta_data_CommandFactory,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CommandFactory_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CommandFactory, std::true_type>,
        // method 'invalidMessageCounterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const int32_t, std::false_type>,
        // method 'commandCreated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<Command * const, std::false_type>,
        // method 'executeMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Message, std::false_type>,
        // method 'setMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const Message, std::false_type>
    >,
    nullptr
} };

void CommandFactory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CommandFactory *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->invalidMessageCounterChanged((*reinterpret_cast< std::add_pointer_t<int32_t>>(_a[1]))); break;
        case 1: _t->commandCreated((*reinterpret_cast< std::add_pointer_t<Command*>>(_a[1]))); break;
        case 2: _t->executeMessage((*reinterpret_cast< std::add_pointer_t<Message>>(_a[1]))); break;
        case 3: _t->setMessage((*reinterpret_cast< std::add_pointer_t<Message>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CommandFactory::*)(const int32_t );
            if (_t _q_method = &CommandFactory::invalidMessageCounterChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CommandFactory::*)(Command * const );
            if (_t _q_method = &CommandFactory::commandCreated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject *CommandFactory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CommandFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CommandFactory.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int CommandFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void CommandFactory::invalidMessageCounterChanged(const int32_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CommandFactory::commandCreated(Command * const _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
