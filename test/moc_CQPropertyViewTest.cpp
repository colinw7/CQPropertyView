/****************************************************************************
** Meta object code from reading C++ file 'CQPropertyViewTest.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CQPropertyViewTest.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQPropertyViewTest.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Dialog_t {
    QByteArrayData data[3];
    char stringdata0[19];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Dialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Dialog_t qt_meta_stringdata_Dialog = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Dialog"
QT_MOC_LITERAL(1, 7, 10), // "filterSlot"
QT_MOC_LITERAL(2, 18, 0) // ""

    },
    "Dialog\0filterSlot\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Dialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Dialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Dialog *_t = static_cast<Dialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->filterSlot(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject Dialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Dialog.data,
      qt_meta_data_Dialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Dialog.stringdata0))
        return static_cast<void*>(const_cast< Dialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_Widget_t {
    QByteArrayData data[21];
    char stringdata0[145];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widget_t qt_meta_stringdata_Widget = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Widget"
QT_MOC_LITERAL(1, 7, 7), // "integer"
QT_MOC_LITERAL(2, 15, 4), // "real"
QT_MOC_LITERAL(3, 20, 6), // "string"
QT_MOC_LITERAL(4, 27, 4), // "enum"
QT_MOC_LITERAL(5, 32, 4), // "Enum"
QT_MOC_LITERAL(6, 37, 5), // "color"
QT_MOC_LITERAL(7, 43, 4), // "font"
QT_MOC_LITERAL(8, 48, 6), // "pointf"
QT_MOC_LITERAL(9, 55, 5), // "rectf"
QT_MOC_LITERAL(10, 61, 5), // "sizef"
QT_MOC_LITERAL(11, 67, 7), // "palette"
QT_MOC_LITERAL(12, 75, 9), // "alignment"
QT_MOC_LITERAL(13, 85, 13), // "Qt::Alignment"
QT_MOC_LITERAL(14, 99, 8), // "lineDash"
QT_MOC_LITERAL(15, 108, 9), // "CLineDash"
QT_MOC_LITERAL(16, 118, 5), // "angle"
QT_MOC_LITERAL(17, 124, 6), // "CAngle"
QT_MOC_LITERAL(18, 131, 3), // "ONE"
QT_MOC_LITERAL(19, 135, 3), // "TWO"
QT_MOC_LITERAL(20, 139, 5) // "THREE"

    },
    "Widget\0integer\0real\0string\0enum\0Enum\0"
    "color\0font\0pointf\0rectf\0sizef\0palette\0"
    "alignment\0Qt::Alignment\0lineDash\0"
    "CLineDash\0angle\0CAngle\0ONE\0TWO\0THREE"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
      13,   14, // properties
       1,   53, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x00095003,
       2, QMetaType::Double, 0x00095103,
       3, QMetaType::QString, 0x00095103,
       4, 0x80000000 | 5, 0x0009510b,
       6, QMetaType::QColor, 0x00095103,
       7, QMetaType::QFont, 0x00095103,
       8, QMetaType::QPointF, 0x00095103,
       9, QMetaType::QRectF, 0x00095103,
      10, QMetaType::QSizeF, 0x00095103,
      11, QMetaType::QPalette, 0x00095103,
      12, 0x80000000 | 13, 0x0009510b,
      14, 0x80000000 | 15, 0x0009510b,
      16, 0x80000000 | 17, 0x0009510b,

 // enums: name, flags, count, data
       5, 0x0,    3,   57,

 // enum data: key, value
      18, uint(Widget::Enum::ONE),
      19, uint(Widget::Enum::TWO),
      20, uint(Widget::Enum::THREE),

       0        // eod
};

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        Widget *_t = static_cast<Widget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getInt(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->getReal(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->getString(); break;
        case 3: *reinterpret_cast< Enum*>(_v) = _t->getEnum(); break;
        case 4: *reinterpret_cast< QColor*>(_v) = _t->color(); break;
        case 5: *reinterpret_cast< QFont*>(_v) = _t->font(); break;
        case 6: *reinterpret_cast< QPointF*>(_v) = _t->pointf(); break;
        case 7: *reinterpret_cast< QRectF*>(_v) = _t->rectf(); break;
        case 8: *reinterpret_cast< QSizeF*>(_v) = _t->sizef(); break;
        case 9: *reinterpret_cast< QPalette*>(_v) = _t->palette(); break;
        case 10: *reinterpret_cast< Qt::Alignment*>(_v) = _t->alignment(); break;
        case 11: *reinterpret_cast< CLineDash*>(_v) = _t->getLineDash(); break;
        case 12: *reinterpret_cast< CAngle*>(_v) = _t->getAngle(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        Widget *_t = static_cast<Widget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setInt(*reinterpret_cast< int*>(_v)); break;
        case 1: _t->setReal(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setString(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setEnum(*reinterpret_cast< Enum*>(_v)); break;
        case 4: _t->setColor(*reinterpret_cast< QColor*>(_v)); break;
        case 5: _t->setFont(*reinterpret_cast< QFont*>(_v)); break;
        case 6: _t->setPointf(*reinterpret_cast< QPointF*>(_v)); break;
        case 7: _t->setRectf(*reinterpret_cast< QRectF*>(_v)); break;
        case 8: _t->setSizef(*reinterpret_cast< QSizeF*>(_v)); break;
        case 9: _t->setPalette(*reinterpret_cast< QPalette*>(_v)); break;
        case 10: _t->setAlignment(*reinterpret_cast< Qt::Alignment*>(_v)); break;
        case 11: _t->setLineDash(*reinterpret_cast< CLineDash*>(_v)); break;
        case 12: _t->setAngle(*reinterpret_cast< CAngle*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject Widget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_Widget.data,
      qt_meta_data_Widget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Widget.stringdata0))
        return static_cast<void*>(const_cast< Widget*>(this));
    return QFrame::qt_metacast(_clname);
}

int Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 13;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 13;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
