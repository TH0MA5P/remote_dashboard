#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QList>
#include "boardvalue.h"
#include "proto.h"

class Board : public QObject
{
    Q_OBJECT
public:
    explicit Board(QObject *parent = 0);
    void clear_values();

    void cmd_get_list();
    void cmd_get_desc_value(quint32 group, quint32 value_idx);
    void cmd_get_value_limit(quint32 group, quint32 value_idx, bool isLowLimit);
    void cmd_read_value(quint32 group, quint32 value_idx);
    void cmd_write_value(quint32 group, quint32 value_idx, QByteArray *buf);

    void ans_who_received(QDataStream &stream);
    void ans_list_received(QDataStream &stream);
    void ans_read_received(QDataStream &stream);
    void ans_get_desc_received(QDataStream &stream);
    void ans_get_limit_low_received(QDataStream &stream);
    void ans_get_limit_high_received(QDataStream &stream);

    QString getStringFromStream(QDataStream &stream);

    QList<BoardValue*> _list_values;

signals:
    void updated();

public slots:
    void answer_received(QByteArray datagram);

private:
    proto * prot;
    QString boardName;

    int _read_value_idx;              // Current read_value pending
};

#endif // VALUES_H
