/****************************************************************************
** Meta object code from reading C++ file 'finger.h'
**
** Created: Wed Dec 23 19:31:10 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../1201/finger.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'finger.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Finger_Gather[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      35,   14,   14,   14, 0x0a,
      57,   14,   14,   14, 0x0a,
      71,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Finger_Gather[] = {
    "Finger_Gather\0\0fingerScreenTouch()\0"
    "finger_data_process()\0storeTemple()\0"
    "testTimer()\0"
};

const QMetaObject Finger_Gather::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Finger_Gather,
      qt_meta_data_Finger_Gather, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Finger_Gather::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Finger_Gather::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Finger_Gather::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Finger_Gather))
        return static_cast<void*>(const_cast< Finger_Gather*>(this));
    return QThread::qt_metacast(_clname);
}

int Finger_Gather::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: fingerScreenTouch(); break;
        case 1: finger_data_process(); break;
        case 2: storeTemple(); break;
        case 3: testTimer(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Finger_Gather::fingerScreenTouch()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
