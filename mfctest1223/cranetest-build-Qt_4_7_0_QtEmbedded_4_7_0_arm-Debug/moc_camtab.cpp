/****************************************************************************
** Meta object code from reading C++ file 'camtab.h'
**
** Created: Wed Dec 23 19:32:18 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../1201/camtab.h"
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
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x08,
      33,    7,   29,    7, 0x0a,
      48,    7,   29,    7, 0x0a,
      65,    7,   29,    7, 0x0a,
      84,    7,   29,    7, 0x0a,
      98,    7,   29,    7, 0x0a,
     119,    7,   29,    7, 0x0a,
     133,    7,   29,    7, 0x0a,
     151,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CamTab[] = {
    "CamTab\0\0SendPhotoAvailable()\0int\0"
    "DisplayVideo()\0TakePhotoClick()\0"
    "DeletePhotoClick()\0RecordClick()\0"
    "RecordClickDisplay()\0timerUpdate()\0"
    "RecordStopClick()\0mfcconvertClick()\0"
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
        case 2: { int _r = TakePhotoClick();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: { int _r = DeletePhotoClick();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: { int _r = RecordClick();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: { int _r = RecordClickDisplay();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { int _r = timerUpdate();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: { int _r = RecordStopClick();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: mfcconvertClick(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
