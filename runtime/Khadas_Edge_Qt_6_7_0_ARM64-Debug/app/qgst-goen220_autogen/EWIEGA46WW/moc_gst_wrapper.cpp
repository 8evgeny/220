/****************************************************************************
** Meta object code from reading C++ file 'gst_wrapper.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/app/gst_wrapper.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gst_wrapper.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSGST_WrapperENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSGST_WrapperENDCLASS = QtMocHelpers::stringData(
    "GST_Wrapper",
    "sig_STREAM_RDY",
    "",
    "sig_RstIRPipeline",
    "sig_RstTVPipeline",
    "sig_rdyOutObject",
    "cx_out",
    "cy_out",
    "w_out",
    "h_out",
    "sig_rdyOutRoi",
    "sig_rdyActiveLockTracking",
    "active_out",
    "tracking_out",
    "handleFpsUpdateTV",
    "fps",
    "active_stream",
    "handleFpsUpdateIR",
    "proc_RestartIRPipeline",
    "proc_RestartTVPipeline",
    "proc_Thread_Finished",
    "proc_getOutObject",
    "proc_getOutRoi",
    "proc_setTargetObject",
    "cx",
    "cy",
    "w",
    "h",
    "proc_setStopTracking",
    "proc_getActiveLockTracking",
    "proc_setRoiExt",
    "ext",
    "proc_stopCapture",
    "onMpegTsWatchdogTimeout",
    "proc_MPEG_TS_Telemetry",
    "datagram"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGST_WrapperENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  134,    2, 0x06,    1 /* Public */,
       3,    0,  135,    2, 0x06,    2 /* Public */,
       4,    0,  136,    2, 0x06,    3 /* Public */,
       5,    4,  137,    2, 0x06,    4 /* Public */,
      10,    4,  146,    2, 0x06,    9 /* Public */,
      11,    2,  155,    2, 0x06,   14 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      14,    2,  160,    2, 0x08,   17 /* Private */,
      17,    2,  165,    2, 0x08,   20 /* Private */,
      18,    0,  170,    2, 0x08,   23 /* Private */,
      19,    0,  171,    2, 0x08,   24 /* Private */,
      20,    0,  172,    2, 0x08,   25 /* Private */,
      21,    0,  173,    2, 0x0a,   26 /* Public */,
      22,    0,  174,    2, 0x0a,   27 /* Public */,
      23,    4,  175,    2, 0x0a,   28 /* Public */,
      28,    0,  184,    2, 0x0a,   33 /* Public */,
      29,    0,  185,    2, 0x0a,   34 /* Public */,
      30,    1,  186,    2, 0x0a,   35 /* Public */,
      32,    0,  189,    2, 0x0a,   37 /* Public */,
      33,    0,  190,    2, 0x0a,   38 /* Public */,
      34,    1,  191,    2, 0x0a,   39 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt,    6,    7,    8,    9,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt,    6,    7,    8,    9,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,   12,   13,

 // slots: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Bool,   15,   16,
    QMetaType::Void, QMetaType::Double, QMetaType::Bool,   15,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt, QMetaType::UInt,   24,   25,   26,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   35,

       0        // eod
};

Q_CONSTINIT const QMetaObject GST_Wrapper::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSGST_WrapperENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGST_WrapperENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGST_WrapperENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GST_Wrapper, std::true_type>,
        // method 'sig_STREAM_RDY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_RstIRPipeline'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_RstTVPipeline'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sig_rdyOutObject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        // method 'sig_rdyOutRoi'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        // method 'sig_rdyActiveLockTracking'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'handleFpsUpdateTV'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'handleFpsUpdateIR'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'proc_RestartIRPipeline'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_RestartTVPipeline'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_Thread_Finished'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_getOutObject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_getOutRoi'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_setTargetObject'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        QtPrivate::TypeAndForceComplete<uint, std::false_type>,
        // method 'proc_setStopTracking'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_getActiveLockTracking'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_setRoiExt'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'proc_stopCapture'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMpegTsWatchdogTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'proc_MPEG_TS_Telemetry'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QByteArray, std::false_type>
    >,
    nullptr
} };

void GST_Wrapper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GST_Wrapper *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->sig_STREAM_RDY(); break;
        case 1: _t->sig_RstIRPipeline(); break;
        case 2: _t->sig_RstTVPipeline(); break;
        case 3: _t->sig_rdyOutObject((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[4]))); break;
        case 4: _t->sig_rdyOutRoi((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[4]))); break;
        case 5: _t->sig_rdyActiveLockTracking((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->handleFpsUpdateTV((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 7: _t->handleFpsUpdateIR((*reinterpret_cast< std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 8: _t->proc_RestartIRPipeline(); break;
        case 9: _t->proc_RestartTVPipeline(); break;
        case 10: _t->proc_Thread_Finished(); break;
        case 11: _t->proc_getOutObject(); break;
        case 12: _t->proc_getOutRoi(); break;
        case 13: _t->proc_setTargetObject((*reinterpret_cast< std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<uint>>(_a[4]))); break;
        case 14: _t->proc_setStopTracking(); break;
        case 15: _t->proc_getActiveLockTracking(); break;
        case 16: _t->proc_setRoiExt((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 17: _t->proc_stopCapture(); break;
        case 18: _t->onMpegTsWatchdogTimeout(); break;
        case 19: _t->proc_MPEG_TS_Telemetry((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GST_Wrapper::*)();
            if (_t _q_method = &GST_Wrapper::sig_STREAM_RDY; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GST_Wrapper::*)();
            if (_t _q_method = &GST_Wrapper::sig_RstIRPipeline; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GST_Wrapper::*)();
            if (_t _q_method = &GST_Wrapper::sig_RstTVPipeline; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (GST_Wrapper::*)(uint , uint , uint , uint );
            if (_t _q_method = &GST_Wrapper::sig_rdyOutObject; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (GST_Wrapper::*)(uint , uint , uint , uint );
            if (_t _q_method = &GST_Wrapper::sig_rdyOutRoi; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (GST_Wrapper::*)(bool , bool );
            if (_t _q_method = &GST_Wrapper::sig_rdyActiveLockTracking; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *GST_Wrapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GST_Wrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGST_WrapperENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GST_Wrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void GST_Wrapper::sig_STREAM_RDY()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void GST_Wrapper::sig_RstIRPipeline()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void GST_Wrapper::sig_RstTVPipeline()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void GST_Wrapper::sig_rdyOutObject(uint _t1, uint _t2, uint _t3, uint _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GST_Wrapper::sig_rdyOutRoi(uint _t1, uint _t2, uint _t3, uint _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GST_Wrapper::sig_rdyActiveLockTracking(bool _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
