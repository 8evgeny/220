/****************************************************************************
** Meta object code from reading C++ file 'main_app.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/app/main_app.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_app.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSMain_appENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSMain_appENDCLASS = QtMocHelpers::stringData(
    "Main_app",
    "sig_VISCA_CONNECT",
    "",
    "portName",
    "baudrate",
    "sig_VISCA_DISCONNECT",
    "sig_VISCA_CMD",
    "TVCamera::Command",
    "cmd",
    "value",
    "sig_MINI640_InitConnection",
    "sig_MINI640_CloseConnection",
    "sig_LensJ200_CloseConnection",
    "sig_LDC_CloseConnection",
    "sig_GSP_CloseConnection",
    "sig_GST_StopCapture",
    "proc_video_init_timeout",
    "proc_Switch_Channel",
    "channel",
    "en",
    "proc_MINI640_InitReady",
    "proc_GST_InitReady",
    "proc_deinitGyroPlatform",
    "proc_GST_RDY"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSMain_appENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    2,  104,    2, 0x06,    1 /* Public */,
       5,    0,  109,    2, 0x06,    4 /* Public */,
       6,    2,  110,    2, 0x06,    5 /* Public */,
      10,    0,  115,    2, 0x06,    8 /* Public */,
      11,    0,  116,    2, 0x06,    9 /* Public */,
      12,    0,  117,    2, 0x06,   10 /* Public */,
      13,    0,  118,    2, 0x06,   11 /* Public */,
      14,    0,  119,    2, 0x06,   12 /* Public */,
      15,    0,  120,    2, 0x06,   13 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      16,    0,  121,    2, 0x08,   14 /* Private */,
      17,    2,  122,    2, 0x0a,   15 /* Public */,
      20,    0,  127,    2, 0x0a,   18 /* Public */,
      21,    0,  128,    2, 0x0a,   19 /* Public */,
      22,    0,  129,    2, 0x0a,   20 /* Public */,
      23,    0,  130,    2, 0x0a,   21 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::UInt,    3,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7, QMetaType::UInt,    8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::UChar,   18,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Main_app::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSMain_appENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSMain_appENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSMain_appENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Main_app, std::true_type>,
        // method 'sig_VISCA_CONNECT'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'sig_VISCA_DISCONNECT'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_VISCA_CMD'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<TVCamera::Command, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        // method 'sig_MINI640_InitConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_MINI640_CloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_LensJ200_CloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_LDC_CloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_GSP_CloseConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_GST_StopCapture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_video_init_timeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_Switch_Channel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint32, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint8, std::false_type>,
        // method 'proc_MINI640_InitReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_GST_InitReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_deinitGyroPlatform'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_GST_RDY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Main_app::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Main_app *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sig_VISCA_CONNECT((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 1: _t->sig_VISCA_DISCONNECT(); break;
        case 2: _t->sig_VISCA_CMD((*reinterpret_cast< std::add_pointer_t<TVCamera::Command>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint32>>(_a[2]))); break;
        case 3: _t->sig_MINI640_InitConnection(); break;
        case 4: _t->sig_MINI640_CloseConnection(); break;
        case 5: _t->sig_LensJ200_CloseConnection(); break;
        case 6: _t->sig_LDC_CloseConnection(); break;
        case 7: _t->sig_GSP_CloseConnection(); break;
        case 8: _t->sig_GST_StopCapture(); break;
        case 9: _t->proc_video_init_timeout(); break;
        case 10: _t->proc_Switch_Channel((*reinterpret_cast< std::add_pointer_t<quint32>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint8>>(_a[2]))); break;
        case 11: _t->proc_MINI640_InitReady(); break;
        case 12: _t->proc_GST_InitReady(); break;
        case 13: _t->proc_deinitGyroPlatform(); break;
        case 14: _t->proc_GST_RDY(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Main_app::*)(QString , quint32 );
            if (_t _q_method = &Main_app::sig_VISCA_CONNECT; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Main_app::*)();
            if (_t _q_method = &Main_app::sig_VISCA_DISCONNECT; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Main_app::*)(TVCamera::Command , quint32 );
            if (_t _q_method = &Main_app::sig_VISCA_CMD; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Main_app::*)();
            if (_t _q_method = &Main_app::sig_MINI640_InitConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Main_app::*)();
            if (_t _q_method = &Main_app::sig_MINI640_CloseConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Main_app::*)();
            if (_t _q_method = &Main_app::sig_LensJ200_CloseConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Main_app::*)();
            if (_t _q_method = &Main_app::sig_LDC_CloseConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Main_app::*)();
            if (_t _q_method = &Main_app::sig_GSP_CloseConnection; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Main_app::*)();
            if (_t _q_method = &Main_app::sig_GST_StopCapture; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
    }
}

const QMetaObject *Main_app::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Main_app::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSMain_appENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Main_app::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void Main_app::sig_VISCA_CONNECT(QString _t1, quint32 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Main_app::sig_VISCA_DISCONNECT()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Main_app::sig_VISCA_CMD(TVCamera::Command _t1, quint32 _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Main_app::sig_MINI640_InitConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Main_app::sig_MINI640_CloseConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Main_app::sig_LensJ200_CloseConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Main_app::sig_LDC_CloseConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void Main_app::sig_GSP_CloseConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void Main_app::sig_GST_StopCapture()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
