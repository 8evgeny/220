/****************************************************************************
** Meta object code from reading C++ file 'CDelimiterWithLenAndCrcMessageParser.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../lib/gsp/Goen220Vdb/src/NCore/CDelimiterWithLenAndCrcMessageParser.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDelimiterWithLenAndCrcMessageParser.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CDelimiterWithLenAndCrcMessageParser_t {
    uint offsetsAndSizes[8];
    char stringdata0[37];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[5];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CDelimiterWithLenAndCrcMessageParser_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CDelimiterWithLenAndCrcMessageParser_t qt_meta_stringdata_CDelimiterWithLenAndCrcMessageParser = {
    {
        QT_MOC_LITERAL(0, 36),  // "CDelimiterWithLenAndCrcMessag..."
        QT_MOC_LITERAL(37, 11),  // "processData"
        QT_MOC_LITERAL(49, 0),  // ""
        QT_MOC_LITERAL(50, 4)   // "data"
    },
    "CDelimiterWithLenAndCrcMessageParser",
    "processData",
    "",
    "data"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CDelimiterWithLenAndCrcMessageParser[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x0a,    1 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QByteArray,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject CDelimiterWithLenAndCrcMessageParser::staticMetaObject = { {
    QMetaObject::SuperData::link<MessageParser::staticMetaObject>(),
    qt_meta_stringdata_CDelimiterWithLenAndCrcMessageParser.offsetsAndSizes,
    qt_meta_data_CDelimiterWithLenAndCrcMessageParser,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CDelimiterWithLenAndCrcMessageParser_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<CDelimiterWithLenAndCrcMessageParser, std::true_type>,
        // method 'processData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray, std::false_type>
    >,
    nullptr
} };

void CDelimiterWithLenAndCrcMessageParser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CDelimiterWithLenAndCrcMessageParser *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->processData((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *CDelimiterWithLenAndCrcMessageParser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDelimiterWithLenAndCrcMessageParser::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CDelimiterWithLenAndCrcMessageParser.stringdata0))
        return static_cast<void*>(this);
    return MessageParser::qt_metacast(_clname);
}

int CDelimiterWithLenAndCrcMessageParser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = MessageParser::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
