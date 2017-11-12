#include "boardvalue.h"
#include <QLineEdit>
#include <QLabel>
#include <QtEndian>
#include <QDebug>
#include <QRegExpValidator>

const int value_type_size[] = {1, 2, 4, 8, 4, 1}; // todo use un common.h
const char *value_type_name[] = {"int8", "int16", "int32", "int64", "float", "string"};

boardValue::boardValue(struct T_VALUE_INFO value) :
    _value(value),
    _editor(0),
    _editorHex(0)
{
    _data.resize(value_type_size[_value.type] * _value.nb_elem_x * _value.nb_elem_y);
}

boardValue::boardValue(qint32 group, qint32 id, qint16 nb_x, qint16 nb_y,
                       uint8_t readPermission, uint8_t writePermission, uint8_t type) :
    _editor(0),
    _editorHex(0)
{
    _value.id.group = group;
    _value.id.name = id;
    _value.nb_elem_x = nb_x;
    _value.nb_elem_y = nb_y;
    _value.readPermission = readPermission;
    _value.writePermission = writePermission;
    _value.type = type;

    _data.resize(value_type_size[_value.type] * _value.nb_elem_x * _value.nb_elem_y);
}

QString boardValue::str_id()
{
    char str[5];
    for (int i = 0; i < 4; i++)
        str[i] = _value.id.name >> (24 - (i << 3));    // id is BigEndian
    str[4] = '\0';
    return QString(str);
}

QString boardValue::str_group()
{
    char str[5];
    for (int i = 0; i < 4; i++)
        str[i] = _value.id.group >> (24 - (i << 3));    // id is BigEndian
    str[4] = '\0';
    return QString(str);
}

QString boardValue::info()
{
    if (_value.nb_elem_x > 1 && _value.nb_elem_y > 1)
        return QString("%1[%2][%3]").arg(value_type_name[_value.type]).arg(_value.nb_elem_x).arg(_value.nb_elem_y);
    if (_value.nb_elem_x > 1)
        return QString("%1[%2]").arg(value_type_name[_value.type]).arg(_value.nb_elem_x);
    return QString(value_type_name[_value.type]);
}

QString boardValue::convertIntValue(void *src, int base)
{
    switch (_value.type) {
	case type_float: {
			int32_t value = qFromBigEndian(*((int32_t *)src));
			return QString::number(*(float *)&value);
		}
	case type_int16:
		return QString::number(qFromBigEndian(*((qint16 *)src)), base);
	case type_int32:
		return QString::number(qFromBigEndian(*((qint32 *)src)), base);
	case type_int64:
		return QString::number(qFromBigEndian(*((qint64 *)src)), base);
	default:
		return QString::number(*((qint8 *)src), base);
	}
}

void boardValue::convertStringToBuf(QDataStream &stream, QString str, int base)
{
	bool ok;
    switch (_value.type) {
	case type_float:
			stream << str.toFloat(); break;
	case type_int16:
		stream << (qint16)str.toInt(&ok, base); break;
	case type_int32:
		stream << (qint32)str.toInt(&ok, base); break;
	case type_int64:
		stream << (qint64)str.toLongLong(&ok, base); break;
	default:
		stream << (qint8)str.toInt(&ok, base); break;
	}
}

QWidget *boardValue::dataDumpHex()
{
    if (_editorHex)
        delete _editorHex;
    _editorHex = new QLineEdit(QString(_data.toHex()));
    _editorHex->setMaxLength(_data.length() * 2);
    QValidator *validatorHexa = new QRegExpValidator(QRegExp("[0-9a-fA-F]*"), _editorHex);
    _editorHex->setValidator(validatorHexa);
    if (!(_value.writePermission)) {
        _editorHex->setReadOnly(true);
        _editorHex->setStyleSheet("color:grey");
    }
    return _editorHex;
}

#define EDITOR_BASE	10

QString boardValue::getStringFromData(QByteArray &data)
{
    QString text("");
    if (_value.type == type_string) {
       text = QString::fromUtf8(data);
    } else {
        char *ptr = data.data();

        for (int i = 0; i < _value.nb_elem_x * _value.nb_elem_y; i++) {
            text.append(convertIntValue(ptr, EDITOR_BASE));
            ptr += value_type_size[_value.type];
            if (i < _value.nb_elem_x * _value.nb_elem_y - 1)
            {
                text.append("; ");
                if ((_value.nb_elem_y > 1) && ((i+1) % _value.nb_elem_y == 0))
                    text.append("; ");
            }
        }
    }
    return text;
}

QWidget *boardValue::dataEditor()
{
    if (_editor)
        delete _editor;
    _editor = new QLineEdit("");
    if (!(_value.writePermission)) {
        _editor->setReadOnly(true);
        _editor->setStyleSheet("color:grey");
    }

    _editor->setText(getStringFromData(_data));

    return _editor;
}

bool boardValue::getData(QByteArray &newData)
{
    if (!_editor || !_editorHex)
        return false;

    if (_value.type == type_string)
    {
        QString text = _editor->text();
        text.truncate(_data.size()-1);
        newData = QByteArray(text.toLocal8Bit());
    }
    else
    {
        newData.resize(_data.size());
        QDataStream newStream(&newData, QIODevice::WriteOnly | QIODevice::Truncate);
        newStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        QString text = _editor->text();
        text.remove(" ");
        text.replace(";;", ";");
        QStringList arrayStr = text.split(";");
        for (int i = 0; i < _value.nb_elem_x * _value.nb_elem_y && i < arrayStr.count(); i++)
        {
            convertStringToBuf(newStream, arrayStr.at(i), EDITOR_BASE);
        }
    }
    return (newData != _data);
}


