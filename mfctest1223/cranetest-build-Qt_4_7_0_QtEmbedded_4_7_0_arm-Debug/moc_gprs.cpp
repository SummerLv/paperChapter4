/****************************************************************************
** Meta object code from reading C++ file 'gprs.h'
**
** Created: Wed Dec 23 19:30:57 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../1201/gprs.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gprs.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GprsConnect[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   12,   12,   12, 0x0a,
      47,   12,   12,   12, 0x0a,
      61,   12,   12,   12, 0x0a,
      89,   12,   85,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GprsConnect[] = {
    "GprsConnect\0\0finger_change_insert()\0"
    "sendData()\0receiveData()\0"
    "receiveData_fromUpper()\0int\0SendPhoto()\0"
};

const QMetaObject GprsConnect::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_GprsConnect,
      qt_meta_data_GprsConnect, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GprsConnect::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GprsConnect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GprsConnect::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GprsConnect))
        return static_cast<void*>(const_cast< GprsConnect*>(this));
    return QThread::qt_metacast(_clname);
}

int GprsConnect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: finger_change_insert(); break;
        case 1: sendData(); break;
        case 2: receiveData(); break;
        case 3: receiveData_fromUpper(); break;
        case 4: { int _r = SendPhoto();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void GprsConnect::finger_change_insert()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
