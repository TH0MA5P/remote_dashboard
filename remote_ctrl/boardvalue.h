#ifndef BOARDVALUE_H
#define BOARDVALUE_H

#include <QByteArray>
#include <QDataStream>
#include <QLineEdit>

#include <stdint.h>

#include "../common/values.h"

class boardValue
{
public:
    explicit boardValue(struct T_VALUE_INFO value);
    qint32 id() { return _value.id.name; }
    qint32 group() { return _value.id.group; }
    QString str_id();
    QString str_group();
    int size() { return _data.size(); }
    QWidget *dataDumpHex();
    QWidget *dataEditor();
    QString info();

    QString getStringFromData(QByteArray &data);

    void setData(QDataStream &stream) { stream.readRawData(_data.data(), _data.size()); }
    bool getData(QByteArray &buf);

private:
	QString convertIntValue(void *src, int base);
	void convertStringToBuf(QDataStream &stream, QString str, int base);

private:
    struct T_VALUE_INFO _value;
    QByteArray _data;
    QLineEdit *_editor;
    QLineEdit *_editorHex;
};

#endif // BOARDVALUE_H
