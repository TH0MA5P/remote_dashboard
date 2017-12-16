#ifndef BOARDVALUE_H
#define BOARDVALUE_H

#include <QByteArray>
#include <QDataStream>
#include <QLineEdit>

#include <stdint.h>

#include "../common/values.h"

class BoardValue
{
public:
    explicit BoardValue(struct T_VALUE_INFO value);
    explicit BoardValue(qint32 group, qint32 id, qint16 nb_x, qint16 nb_y,
                        uint8_t readPermission, uint8_t writePermission, uint8_t type);
    qint32 id() { return _value.id.name; }
    qint32 group() { return _value.id.group; }
    QString str_id();
    QString str_group();
    int size() { return _data.size(); }
    QWidget *dataDumpHex();
    QWidget *dataEditor();
    QString info();

    QString getStringFromData(QByteArray &data);

    void setData(QDataStream &stream);
    bool getData(QByteArray &buf);

    QString desc;
    QString minVal;
    QString maxVal;

private:
	QString convertIntValue(void *src, int base);
	void convertStringToBuf(QDataStream &stream, QString str, int base);

private:
    struct T_VALUE_INFO _value;
    QByteArray _data;
    QString value;
    QString valueHex;
    QLineEdit *_editor;
    QLineEdit *_editorHex;
};

#endif // BOARDVALUE_H
