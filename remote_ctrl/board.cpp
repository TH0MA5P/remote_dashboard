#include "board.h"
#include "acq_proto.h"

board::board(QObject *parent) : QObject(parent)
{
    connect(prot, SIGNAL(signalReceived(QByteArray)), this, SLOT(answer_received(QByteArray)));
}

void board::clear_values() {
    foreach (boardValue *value, _list_values) {
        delete value;
    }
    _list_values.clear();
}


void board::cmd_get_desc_value(quint32 value_idx)
{
    prot->sendGetDescValue(value_idx);
}

void board::cmd_read_value(quint32 value_idx)
{
    qDebug() << "read value" << value_idx;
    prot->sendReadValue(value_idx);
}

void board::cmd_write_value(quint32 value_idx, QByteArray *buf)
{
    prot->sendWriteValue(value_idx, buf);
}

void board::ans_who_received(QDataStream &stream)
{
    boardName = getStringFromStream(stream);
}

void board::ans_list_received(QDataStream &stream)
{
    qint16 nb_value;
    stream >> nb_value;

    qDebug() << nb_value << " values found";

    for (int i = 0; i < nb_value; i++) {
        qint32 id;
        qint16 nb_x, nb_y, flags;
        stream >> id >> nb_x >> nb_y >> flags;
        boardValue *value = new boardValue(id, flags, nb_x, nb_y);
        _list_values.append(value);
        qDebug() << "New value " << id << " " << value->str_id();
    }
    if (nb_value > 0) {
        _read_value_idx = 0;
        cmd_read_value(_list_values.at(_read_value_idx)->id());
    }
}

void board::ans_read_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;
    boardValue *value = _list_values.at(_read_value_idx);
    value->setData(stream);

    qDebug() << "Value " << value->str_id() << " ready to display";
    cmd_get_desc_value(_list_values.at(_read_value_idx)->id());
}

QString board::getStringFromStream(QDataStream &stream)
{
    char desc[100];
    stream.readRawData(desc, 100);
    QString string = QString::fromLocal8Bit((const char *)desc, strlen(desc));
    return string;
}

void board::ans_get_desc_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;
    qDebug() << "Receive get desc answer";
    QString string = getStringFromStream(stream);

    if (_read_value_idx + 1 < _list_values.size())
    {
        _read_value_idx++;
        cmd_read_value(_list_values.at(_read_value_idx)->id());
    }
}

void board::answer_received(QByteArray datagram)
{
     QDataStream stream(datagram);
     qint8 cmd, ack;
     qint16 status, size, src, dst;
     stream >> src >> dst >> cmd >> ack >> size >> status;
     if (status < 0) {
         char msg_error[size];
         stream.readRawData(msg_error, sizeof(msg_error));
         qDebug() << "Board returned error status " << status << " on cmd " << cmd << " : " << msg_error;
         return;
     }
     switch ((cmd_type_t)cmd) {
     case CMD_WHO:
         ans_who_received(stream);
         break;
     case CMD_LIST_VALUE:
         ans_list_received(stream);
         break;
     case CMD_READ_VALUE:
         ans_read_received(stream);
         break;
     case CMD_WRITE_VALUE:
         break;
     case CMD_GET_DESC_VALUE:
         ans_get_desc_received(stream);
         break;
     default:
         qDebug() << "Unhandled answer " << cmd;
         break;
     }
}


