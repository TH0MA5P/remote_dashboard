#include "../common/commands.h"
#include "../common/values.h"
#include "proto.h"
#include "board.h"


Board::Board(QObject *parent) : QObject(parent),
    prot(new proto())
{
    connect(prot, SIGNAL(signalReceived(QByteArray)), this, SLOT(answer_received(QByteArray)));
}

void Board::clear_values() {
    foreach (BoardValue *value, _list_values) {
        delete value;
    }
    _list_values.clear();
}

void Board::cmd_get_list()
{
    prot->sendListValue();
}


void Board::cmd_get_desc_value(quint32 group, quint32 value_idx)
{
    prot->sendGetDescValue(group, value_idx);
}

void Board::cmd_read_value(quint32 group, quint32 value_idx)
{
    qDebug() << "read value" << value_idx;
    prot->sendReadValue(group, value_idx);
}

void Board::cmd_write_value(quint32 group, quint32 value_idx, QByteArray *buf)
{
    prot->sendWriteValue(group, value_idx, buf);
}

void Board::cmd_get_value_limit(quint32 group, quint32 value_idx, bool isLowLimit)
{
    prot->sendReadValueLimit(group, value_idx, isLowLimit);
}

void Board::ans_who_received(QDataStream &stream)
{
    qDebug() << "Who received";
    boardName = getStringFromStream(stream);
}

void Board::ans_list_received(QDataStream &stream)
{
    qDebug() << "List received";
    qint16 nb_value;
    stream >> nb_value;

    qDebug() << nb_value << " values found";

    for (int i = 0; i < nb_value; i++) {
        qint32 id, group;
        qint16 nb_x, nb_y;
        uint8_t read, write, type;
        stream >> group >> id >> nb_x >> nb_y >> read >> write >> type;
        BoardValue *value = new BoardValue(group, id, nb_x, nb_y, read, write, type);
        _list_values.append(value);
        qDebug() << "New value " << id << " " << value->str_id();
    }
    if (nb_value > 0) {
        _read_value_idx = 0;
        cmd_read_value(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id());
    }
}

void Board::ans_read_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;
    BoardValue *value = _list_values.at(_read_value_idx);
    value->setData(stream);

    qDebug() << "Value " << value->str_id() << " ready to display";
    cmd_get_desc_value(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id());
}

QString Board::getStringFromStream(QDataStream &stream)
{
    char desc[100];
    stream.readRawData(desc, 100);
    QString string = QString::fromLocal8Bit((const char *)desc, strlen(desc));
    return string;
}

void Board::ans_get_desc_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;
    qDebug() << "Receive get desc answer";
    QString string = getStringFromStream(stream);
    BoardValue *value = _list_values.at(_read_value_idx);
    value->desc = string;

    cmd_get_value_limit(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id(), true);
}

void Board::ans_get_limit_low_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;
    BoardValue *value = _list_values.at(_read_value_idx);
    qDebug() << "Receive get low limit answer";

    QByteArray dataStream;
    dataStream.reserve(value->size());
    if (stream.readRawData(dataStream.data(), value->size()) > 0)
    {
        value->minVal = value->getStringFromData(dataStream);
    }

    cmd_get_value_limit(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id(), false);
}

void Board::ans_get_limit_high_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;
    BoardValue *value = _list_values.at(_read_value_idx);
    qDebug() << "Receive get high limit answer";

    QByteArray dataStream;
    dataStream.reserve(value->size());
    if (stream.readRawData(dataStream.data(), value->size()) > 0)
    {
        value->maxVal = value->getStringFromData(dataStream);
    }

    if (_read_value_idx + 1 < _list_values.size())
    {
        _read_value_idx++;
        cmd_read_value(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id());
    }
    else
    {
         emit updated();
    }
}


void Board::answer_received(QByteArray datagram)
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
     switch (cmd) {
     case CODE_CMD_WHO:
         ans_who_received(stream);
         break;
     case CODE_CMD_LIST_VALUES:
         ans_list_received(stream);
         break;
     case CODE_CMD_READ_VALUES:
         ans_read_received(stream);
         break;
     case CODE_CMD_WRITE_VALUES:
         break;
     case CODE_CMD_DESC_VALUES:
         ans_get_desc_received(stream);
         break;
     case CODE_CMD_READ_VALUES_LIMIT_LOW:
         ans_get_limit_low_received(stream);
         break;
     case CODE_CMD_READ_VALUES_LIMIT_HIGH:
         ans_get_limit_high_received(stream);
         break;
     default:
         qDebug() << "Unhandled answer " << cmd;
         break;
     }
}



