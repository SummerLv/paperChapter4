/****************************************************************************
** Meta object code from reading C++ file 'zigbeereceive.h'
**
** Created: Sat Oct 24 03:26:20 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "zigbeereceive.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'zigbeereceive.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ZigbeeReceive[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      50,   29,   14,   14, 0x05,
     112,   78,   14,   14, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ZigbeeReceive[] = {
    "ZigbeeReceive\0\0resetDevice()\0"
    "info_type,info_index\0reqest_Info(quint8,quint32)\0"
    "info_type,info_version,info_index\0"
    "transimit_Info(quint8,quint32,quint32)\0"
};

const QMetaObject ZigbeeReceive::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ZigbeeReceive,
      qt_meta_data_ZigbeeReceive, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ZigbeeReceive::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ZigbeeReceive::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ZigbeeReceive::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ZigbeeReceive))
        return static_cast<void*>(const_cast< ZigbeeReceive*>(this));
    return QThread::qt_metacast(_clname);
}

int ZigbeeReceive::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resetDevice(); break;
        case 1: reqest_Info((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2]))); break;
        case 2: transimit_Info((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void ZigbeeReceive::resetDevice()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ZigbeeReceive::reqest_Info(quint8 _t1, quint32 _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ZigbeeReceive::transimit_Info(quint8 _t1, quint32 _t2, quint32 _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
