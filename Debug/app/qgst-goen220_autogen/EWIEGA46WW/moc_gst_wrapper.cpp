/****************************************************************************
** Meta object code from reading C++ file 'gst_wrapper.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../app/gst_wrapper.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gst_wrapper.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_GST_Wrapper_t {
    uint offsetsAndSizes[66];
    char stringdata0[12];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[18];
    char stringdata5[17];
    char stringdata6[7];
    char stringdata7[7];
    char stringdata8[6];
    char stringdata9[6];
    char stringdata10[14];
    char stringdata11[26];
    char stringdata12[11];
    char stringdata13[13];
    char stringdata14[18];
    char stringdata15[4];
    char stringdata16[14];
    char stringdata17[18];
    char stringdata18[23];
    char stringdata19[23];
    char stringdata20[21];
    char stringdata21[18];
    char stringdata22[15];
    char stringdata23[21];
    char stringdata24[3];
    char stringdata25[3];
    char stringdata26[2];
    char stringdata27[2];
    char stringdata28[21];
    char stringdata29[27];
    char stringdata30[15];
    char stringdata31[4];
    char stringdata32[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GST_Wrapper_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GST_Wrapper_t qt_meta_stringdata_GST_Wrapper = {
    {
        QT_MOC_LITERAL(0, 11),  // "GST_Wrapper"
        QT_MOC_LITERAL(12, 14),  // "sig_STREAM_RDY"
        QT_MOC_LITERAL(27, 0),  // ""
        QT_MOC_LITERAL(28, 17),  // "sig_RstIRPipeline"
        QT_MOC_LITERAL(46, 17),  // "sig_RstTVPipeline"
        QT_MOC_LITERAL(64, 16),  // "sig_rdyOutObject"
        QT_MOC_LITERAL(81, 6),  // "cx_out"
        QT_MOC_LITERAL(88, 6),  // "cy_out"
        QT_MOC_LITERAL(95, 5),  // "w_out"
        QT_MOC_LITERAL(101, 5),  // "h_out"
        QT_MOC_LITERAL(107, 13),  // "sig_rdyOutRoi"
        QT_MOC_LITERAL(121, 25),  // "sig_rdyActiveLockTracking"
        QT_MOC_LITERAL(147, 10),  // "active_out"
        QT_MOC_LITERAL(158, 12),  // "tracking_out"
        QT_MOC_LITERAL(171, 17),  // "handleFpsUpdateTV"
        QT_MOC_LITERAL(189, 3),  // "fps"
        QT_MOC_LITERAL(193, 13),  // "active_stream"
        QT_MOC_LITERAL(207, 17),  // "handleFpsUpdateIR"
        QT_MOC_LITERAL(225, 22),  // "proc_RestartIRPipeline"
        QT_MOC_LITERAL(248, 22),  // "proc_RestartTVPipeline"
        QT_MOC_LITERAL(271, 20),  // "proc_Thread_Finished"
        QT_MOC_LITERAL(292, 17),  // "proc_getOutObject"
        QT_MOC_LITERAL(310, 14),  // "proc_getOutRoi"
        QT_MOC_LITERAL(325, 20),  // "proc_setTargetObject"
        QT_MOC_LITERAL(346, 2),  // "cx"
        QT_MOC_LITERAL(349, 2),  // "cy"
        QT_MOC_LITERAL(352, 1),  // "w"
        QT_MOC_LITERAL(354, 1),  // "h"
        QT_MOC_LITERAL(356, 20),  // "proc_setStopTracking"
        QT_MOC_LITERAL(377, 26),  // "proc_getActiveLockTracking"
        QT_MOC_LITERAL(404, 14),  // "proc_setRoiExt"
        QT_MOC_LITERAL(419, 3),  // "ext"
        QT_MOC_LITERAL(423, 16)   // "proc_stopCapture"
    },
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
    "proc_stopCapture"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GST_Wrapper[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  122,    2, 0x06,    1 /* Public */,
       3,    0,  123,    2, 0x06,    2 /* Public */,
       4,    0,  124,    2, 0x06,    3 /* Public */,
       5,    4,  125,    2, 0x06,    4 /* Public */,
      10,    4,  134,    2, 0x06,    9 /* Public */,
      11,    2,  143,    2, 0x06,   14 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      14,    2,  148,    2, 0x08,   17 /* Private */,
      17,    2,  153,    2, 0x08,   20 /* Private */,
      18,    0,  158,    2, 0x08,   23 /* Private */,
      19,    0,  159,    2, 0x08,   24 /* Private */,
      20,    0,  160,    2, 0x08,   25 /* Private */,
      21,    0,  161,    2, 0x0a,   26 /* Public */,
      22,    0,  162,    2, 0x0a,   27 /* Public */,
      23,    4,  163,    2, 0x0a,   28 /* Public */,
      28,    0,  172,    2, 0x0a,   33 /* Public */,
      29,    0,  173,    2, 0x0a,   34 /* Public */,
      30,    1,  174,    2, 0x0a,   35 /* Public */,
      32,    0,  177,    2, 0x0a,   37 /* Public */,

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

       0        // eod
};

Q_CONSTINIT const QMetaObject GST_Wrapper::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_GST_Wrapper.offsetsAndSizes,
    qt_meta_data_GST_Wrapper,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GST_Wrapper_t,
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
        QtPrivate::TypeAndForceComplete<void, std::false_type>
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
    if (!strcmp(_clname, qt_meta_stringdata_GST_Wrapper.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int GST_Wrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 18;
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
QT_END_MOC_NAMESPACE
