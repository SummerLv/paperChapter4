/****************************************************************************
** Meta object code from reading C++ file 'camtab.h'
**
** Created: Wed Mar 23 16:33:50 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "camtab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camtab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CamTab[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x08,
      33,    7,   29,    7, 0x0a,
      48,    7,    7,    7, 0x0a,
      64,    7,   29,    7, 0x0a,
      81,    7,   29,    7, 0x0a,
     100,    7,   29,    7, 0x0a,
     114,    7,   29,    7, 0x0a,
     132,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CamTab[] = {
    "CamTab\0\0SendPhotoAvailable()\0int\0"
    "DisplayVideo()\0DisplayUVideo()\0"
    "TakePhotoClick()\0DeletePhotoClick()\0"
    "RecordClick()\0RecordStopClick()\0"
    "mfcconvertClick()\0"
};

const QMetaObject CamTab::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CamTab,
      qt_meta_data_CamTab, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CamTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CamTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CamTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CamTab))
        return static_cast<void*>(const_cast< CamTab*>(this));
    return QDialog::qt_metacast(_clname);
}

int CamTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: SendPhotoAvailable(); break;
        case 1: { int _r = DisplayVideo();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: DisplayUVideo(); break;
        case 3: { int _r = TakePhotoClick();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: { int _r = DeletePhotoClick();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = RecordClick();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { int _r = RecordStopClick();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: mfcconvertClick(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
static const uint qt_meta_data_CamThread[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_CamThread[] = {
    "CamThread\0\0camFlush()\0"
};

const QMetaObject CamThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_CamThread,
      qt_meta_data_CamThread, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CamThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CamThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CamThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CamThread))
        return static_cast<void*>(const_cast< CamThread*>(this));
    return QThread::qt_metacast(_clname);
}

int CamThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: camFlush(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void CamThread::camFlush()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
