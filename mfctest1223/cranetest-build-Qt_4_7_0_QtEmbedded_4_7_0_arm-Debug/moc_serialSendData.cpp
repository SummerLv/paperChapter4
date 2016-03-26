/****************************************************************************
** Meta object code from reading C++ file 'serialSendData.h'
**
** Created: Wed Dec 23 19:29:57 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../1201/serialSendData.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialSendData.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SerialSendData[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      37,   16,   15,   15, 0x0a,
     109,   75,   15,   15, 0x0a,
     153,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SerialSendData[] = {
    "SerialSendData\0\0info_type,info_index\0"
    "sendInfo_afterRequest(quint8,quint32)\0"
    "info_type,info_version,info_index\0"
    "storeInfo_afterRecv(quint8,quint32,quint32)\0"
    "ResetDevice()\0"
};

const QMetaObject SerialSendData::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_SerialSendData,
      qt_meta_data_SerialSendData, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SerialSendData::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SerialSendData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SerialSendData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SerialSendData))
        return static_cast<void*>(const_cast< SerialSendData*>(this));
    return QThread::qt_metacast(_clname);
}

int SerialSendData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sendInfo_afterRequest((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2]))); break;
        case 1: storeInfo_afterRecv((*reinterpret_cast< quint8(*)>(_a[1])),(*reinterpret_cast< quint32(*)>(_a[2])),(*reinterpret_cast< quint32(*)>(_a[3]))); break;
        case 2: ResetDevice(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
