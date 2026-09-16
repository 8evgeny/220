/****************************************************************************
** Meta object code from reading C++ file 'lens_j200_wrapper.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/j200/lens_j200_wrapper.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lens_j200_wrapper.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_LensJ200Wrapper_t {
    uint offsetsAndSizes[24];
    char stringdata0[16];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[14];
    char stringdata4[3];
    char stringdata5[17];
    char stringdata6[13];
    char stringdata7[14];
    char stringdata8[4];
    char stringdata9[5];
    char stringdata10[5];
    char stringdata11[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_LensJ200Wrapper_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_LensJ200Wrapper_t qt_meta_stringdata_LensJ200Wrapper = {
    {
        QT_MOC_LITERAL(0, 15),  // "LensJ200Wrapper"
        QT_MOC_LITERAL(16, 15),  // "sig_ReportReady"
        QT_MOC_LITERAL(32, 0),  // ""
        QT_MOC_LITERAL(33, 13),  // "J200_DEVSTATE"
        QT_MOC_LITERAL(47, 2),  // "ds"
        QT_MOC_LITERAL(50, 16),  // "proc_timerUpdate"
        QT_MOC_LITERAL(67, 12),  // "proc_SendCMD"
        QT_MOC_LITERAL(80, 13),  // "J200_CMD_enum"
        QT_MOC_LITERAL(94, 3),  // "cmd"
        QT_MOC_LITERAL(98, 4),  // "par1"
        QT_MOC_LITERAL(103, 4),  // "par2"
        QT_MOC_LITERAL(108, 20)   // "proc_Thread_Finished"
    },
    "LensJ200Wrapper",
    "sig_ReportReady",
    "",
    "J200_DEVSTATE",
    "ds",
    "proc_timerUpdate",
    "proc_SendCMD",
    "J200_CMD_enum",
    "cmd",
    "par1",
    "par2",
    "proc_Thread_Finished"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_LensJ200Wrapper[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       5,    0,   53,    2, 0x08,    3 /* Private */,
       6,    3,   54,    2, 0x0a,    4 /* Public */,
       6,    2,   61,    2, 0x2a,    8 /* Public | MethodCloned */,
       6,    1,   66,    2, 0x2a,   11 /* Public | MethodCloned */,
      11,    0,   69,    2, 0x0a,   13 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7, QMetaType::UInt, QMetaType::UInt,    8,    9,   10,
    QMetaType::Void, 0x80000000 | 7, QMetaType::UInt,    8,    9,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject LensJ200Wrapper::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_LensJ200Wrapper.offsetsAndSizes,
    qt_meta_data_LensJ200Wrapper,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_LensJ200Wrapper_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<LensJ200Wrapper, std::true_type>,
        // method 'sig_ReportReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<J200_DEVSTATE, std::false_type>,
        // method 'proc_timerUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_SendCMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<J200_CMD_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<const quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<const quint32, std::false_type>,
        // method 'proc_SendCMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<J200_CMD_enum, std::false_type>,
        QtPrivate::TypeAndForceComplete<const quint32, std::false_type>,
        // method 'proc_SendCMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<J200_CMD_enum, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void LensJ200Wrapper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LensJ200Wrapper *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sig_ReportReady((*reinterpret_cast< std::add_pointer_t<J200_DEVSTATE>>(_a[1]))); break;
        case 1: _t->proc_timerUpdate(); break;
        case 2: _t->proc_SendCMD((*reinterpret_cast< std::add_pointer_t<J200_CMD_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[3]))); break;
        case 3: _t->proc_SendCMD((*reinterpret_cast< std::add_pointer_t<J200_CMD_enum>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 4: _t->proc_SendCMD((*reinterpret_cast< std::add_pointer_t<J200_CMD_enum>>(_a[1]))); break;
        case 5: _t->proc_Thread_Finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (LensJ200Wrapper::*)(J200_DEVSTATE );
            if (_t _q_method = &LensJ200Wrapper::sig_ReportReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *LensJ200Wrapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LensJ200Wrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LensJ200Wrapper.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LensJ200Wrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void LensJ200Wrapper::sig_ReportReady(J200_DEVSTATE _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
