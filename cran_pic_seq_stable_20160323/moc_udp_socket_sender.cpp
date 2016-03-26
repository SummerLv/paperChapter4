/****************************************************************************
** Meta object code from reading C++ file 'udp_socket_sender.h'
**
** Created: Wed Mar 23 16:33:08 2016
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "udp_socket_sender.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'udp_socket_sender.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Udp_Socket_Sender[] = {

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

static const char qt_meta_stringdata_Udp_Socket_Sender[] = {
    "Udp_Socket_Sender\0"
};

const QMetaObject Udp_Socket_Sender::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Udp_Socket_Sender,
      qt_meta_data_Udp_Socket_Sender, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Udp_Socket_Sender::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Udp_Socket_Sender::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Udp_Socket_Sender::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Udp_Socket_Sender))
        return static_cast<void*>(const_cast< Udp_Socket_Sender*>(this));
    return QObject::qt_metacast(_clname);
}

int Udp_Socket_Sender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Udp_Socket_Receiver[] = {

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

static const char qt_meta_stringdata_Udp_Socket_Receiver[] = {
    "Udp_Socket_Receiver\0"
};

const QMetaObject Udp_Socket_Receiver::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Udp_Socket_Receiver,
      qt_meta_data_Udp_Socket_Receiver, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Udp_Socket_Receiver::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Udp_Socket_Receiver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Udp_Socket_Receiver::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Udp_Socket_Receiver))
        return static_cast<void*>(const_cast< Udp_Socket_Receiver*>(this));
    return QObject::qt_metacast(_clname);
}

int Udp_Socket_Receiver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
