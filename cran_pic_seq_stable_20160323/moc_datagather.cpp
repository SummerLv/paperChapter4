/****************************************************************************
** Meta object code from reading C++ file 'datagather.h'
**
** Created: Wed Mar 23 16:33:58 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "datagather.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datagather.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataGather[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_DataGather[] = {
    "DataGather\0"
};

const QMetaObject DataGather::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_DataGather,
      qt_meta_data_DataGather, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataGather::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataGather::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataGather::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataGather))
        return static_cast<void*>(const_cast< DataGather*>(this));
    return QThread::qt_metacast(_clname);
}

int DataGather::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_DataRead[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_DataRead[] = {
    "DataRead\0"
};

const QMetaObject DataRead::staticMetaObject = {
    { &DataGather::staticMetaObject, qt_meta_stringdata_DataRead,
      qt_meta_data_DataRead, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataRead::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataRead::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataRead::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataRead))
        return static_cast<void*>(const_cast< DataRead*>(this));
    return DataGather::qt_metacast(_clname);
}

int DataRead::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DataGather::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_DataSend[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_DataSend[] = {
    "DataSend\0"
};

const QMetaObject DataSend::staticMetaObject = {
    { &DataGather::staticMetaObject, qt_meta_stringdata_DataSend,
      qt_meta_data_DataSend, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataSend::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataSend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataSend::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataSend))
        return static_cast<void*>(const_cast< DataSend*>(this));
    return DataGather::qt_metacast(_clname);
}

int DataSend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DataGather::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
