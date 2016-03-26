/****************************************************************************
** Meta object code from reading C++ file 'proZoneDataTab.h'
**
** Created: Wed Dec 23 19:30:10 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../1201/proZoneDataTab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'proZoneDataTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProZoneDataTab[] = {

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
      18,   16,   15,   15, 0x0a,
      37,   16,   15,   15, 0x0a,
      60,   16,   15,   15, 0x0a,
      82,   16,   15,   15, 0x0a,
     105,   16,   15,   15, 0x0a,
     125,   15,   15,   15, 0x0a,
     144,   15,   15,   15, 0x0a,
     162,   15,   15,   15, 0x0a,
     181,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ProZoneDataTab[] = {
    "ProZoneDataTab\0\0i\0proZoneSelect(int)\0"
    "proZoneTypeSelect(int)\0elementNumSelect(int)\0"
    "elementTypeSelect(int)\0elementNSelect(int)\0"
    "saveElementClick()\0delElementClick()\0"
    "saveProZoneClick()\0delProZoneClick()\0"
};

const QMetaObject ProZoneDataTab::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ProZoneDataTab,
      qt_meta_data_ProZoneDataTab, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProZoneDataTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProZoneDataTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProZoneDataTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProZoneDataTab))
        return static_cast<void*>(const_cast< ProZoneDataTab*>(this));
    return QDialog::qt_metacast(_clname);
}

int ProZoneDataTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: proZoneSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: proZoneTypeSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: elementNumSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: elementTypeSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: elementNSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: saveElementClick(); break;
        case 6: delElementClick(); break;
        case 7: saveProZoneClick(); break;
        case 8: delProZoneClick(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
