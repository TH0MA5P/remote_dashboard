#ifndef BOARD_H
#define BOARD_H

#include <QObject>
#include <QList>
#include "boardvalue.h"
#include "proto.h"

class board : public QObject
{
    Q_OBJECT
public:
    explicit board(QObject *parent = 0);
    void clear_values();

    void cmd_get_list();
    void cmd_get_desc_value(quint32 group, quint32 value_idx);
    void cmd_read_value(quint32 group, quint32 value_idx);
    void cmd_write_value(quint32 group, quint32 value_idx, QByteArray *buf);

    void ans_who_received(QDataStream &stream);
    void ans_list_received(QDataStream &stream);
    void ans_read_received(QDataStream &stream);
    QString getStringFromStream(QDataStream &stream);
    void ans_get_desc_received(QDataStream &stream);
    void answer_received(QByteArray datagram);

signals:

public slots:

private:
    proto * prot;
    QString boardName;
    QList<boardValue*> _list_values;
    int _read_value_idx;              // Current read_value pending
};

#endif // VALUES_H
