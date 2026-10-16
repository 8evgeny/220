/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[13];
    char stringdata0[281];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 27), // "onSerialPortsInitialization"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 23), // "onSerialPortsConnection"
QT_MOC_LITERAL(4, 64, 22), // "onSendDataToSerialPort"
QT_MOC_LITERAL(5, 87, 5), // "char*"
QT_MOC_LITERAL(6, 93, 16), // "Timer_MW_timeout"
QT_MOC_LITERAL(7, 110, 36), // "PushButton_OpenCloseSerialPor..."
QT_MOC_LITERAL(8, 147, 30), // "SerialPortsEndOfInitialization"
QT_MOC_LITERAL(9, 178, 26), // "SerialPortsConnectionState"
QT_MOC_LITERAL(10, 205, 35), // "PushButton_SetAPD_BiasVoltage..."
QT_MOC_LITERAL(11, 241, 22), // "PrintSerialPortsBuffer"
QT_MOC_LITERAL(12, 264, 16) // "PrintSettings_SP"

    },
    "MainWindow\0onSerialPortsInitialization\0"
    "\0onSerialPortsConnection\0"
    "onSendDataToSerialPort\0char*\0"
    "Timer_MW_timeout\0PushButton_OpenCloseSerialPort_Click\0"
    "SerialPortsEndOfInitialization\0"
    "SerialPortsConnectionState\0"
    "PushButton_SetAPD_BiasVoltage_Click\0"
    "PrintSerialPortsBuffer\0PrintSettings_SP"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    1,   65,    2, 0x06 /* Public */,
       4,    2,   68,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   73,    2, 0x08 /* Private */,
       7,    0,   74,    2, 0x08 /* Private */,
       8,    1,   75,    2, 0x08 /* Private */,
       9,    1,   78,    2, 0x08 /* Private */,
      10,    0,   81,    2, 0x08 /* Private */,
      11,    1,   82,    2, 0x08 /* Private */,
      12,    3,   85,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 5, QMetaType::UInt,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    2,
    QMetaType::Void, QMetaType::UInt, QMetaType::UShort, QMetaType::UShort,    2,    2,    2,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onSerialPortsInitialization(); break;
        case 1: _t->onSerialPortsConnection((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->onSendDataToSerialPort((*reinterpret_cast< char*(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 3: _t->Timer_MW_timeout(); break;
        case 4: _t->PushButton_OpenCloseSerialPort_Click(); break;
        case 5: _t->SerialPortsEndOfInitialization((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->SerialPortsConnectionState((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->PushButton_SetAPD_BiasVoltage_Click(); break;
        case 8: _t->PrintSerialPortsBuffer((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 9: _t->PrintSettings_SP((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< ushort(*)>(_a[2])),(*reinterpret_cast< ushort(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::onSerialPortsInitialization)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::onSerialPortsConnection)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(char * , uint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::onSendDataToSerialPort)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::onSerialPortsInitialization()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MainWindow::onSerialPortsConnection(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainWindow::onSendDataToSerialPort(char * _t1, uint _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
