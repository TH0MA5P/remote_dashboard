#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDataStream>
#include <QDebug>
#include <QLabel>
#include <QtEndian>
#include <QNetworkInterface>

#include <arpa/inet.h>      // todo linux specific
#include <stdint.h>

#include "../common/protocol.h"
#include "../common/commands.h"
#include "proto.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    prot(new proto()),
    _read_value_idx(-1)
{
    ui->setupUi(this);
    connect(prot, SIGNAL(signalReceived(QByteArray)), this, SLOT(answer_received(QByteArray)));

    on_buttonScan_clicked();
}

MainWindow::~MainWindow()
{
    clear_values();
    delete ui;
}

void MainWindow::clear_values() {
    ui->tableValues->clearContents();
    ui->tableValues->setRowCount(0);
    foreach (boardValue *value, _list_values) {
        delete value;
    }
    _list_values.clear();
}

void MainWindow::on_buttonScan_clicked()
{
    ui->listBoard->clear();
    prot->sendWho();
}

void MainWindow::on_listBoard_currentIndexChanged(int index)
{
    clear_values();

    quint32 ip = ui->listBoard->itemData(index).toUInt();
    QHostAddress board_ip(ip);

    prot->sendListValue();
}

void MainWindow::cmd_get_desc_value(quint32 group, quint32 value_idx)
{
    prot->sendGetDescValue(group, value_idx);
}

void MainWindow::cmd_read_value(quint32 group, quint32 value_idx)
{
    qDebug() << "read value" << value_idx;
    prot->sendReadValue(group, value_idx);
}

void MainWindow::cmd_get_value_limit(quint32 group, quint32 value_idx, bool isLowLimit)
{
    prot->sendReadValueLimit(group, value_idx, isLowLimit);
}

void MainWindow::cmd_write_value(quint32 group, quint32 value_idx, QByteArray *buf)
{
    prot->sendWriteValue(group, value_idx, buf);
}

void MainWindow::ans_who_received(QDataStream &stream)
{
    ui->listBoard->addItem(getStringFromStream(stream), "board");
}

void MainWindow::ans_list_received(QDataStream &stream)
{
    struct T_LIST_VALUE listValues;
    stream.readRawData((char *)&listValues, sizeof(listValues));

    listValues.nbValues = ntohs(listValues.nbValues);

    qDebug() << listValues.nbValues << " values found";

    for (int i = 0; i < listValues.nbValues; i++) {

        listValues.valueInfo[i].id.group = ntohl(listValues.valueInfo[i].id.group);
        listValues.valueInfo[i].id.name = ntohl(listValues.valueInfo[i].id.name);
        listValues.valueInfo[i].nb_elem_x = ntohs(listValues.valueInfo[i].nb_elem_x);
        listValues.valueInfo[i].nb_elem_y = ntohs(listValues.valueInfo[i].nb_elem_y);

        boardValue *value = new boardValue(listValues.valueInfo[i]);
        _list_values.append(value);
        qDebug() << "New value " << value->str_id();
    }
    if (listValues.nbValues > 0) {
        ui->tableValues->setRowCount(listValues.nbValues);
        _read_value_idx = 0;
        cmd_read_value(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id());
    }
}

void MainWindow::ans_read_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;
    boardValue *value = _list_values.at(_read_value_idx);
    value->setData(stream);

    qDebug() << "Value " << value->str_id() << " ready to display";
    int col = 0;
    ui->tableValues->setCellWidget(_read_value_idx, col++, new QLabel(value->str_group()));
    ui->tableValues->setCellWidget(_read_value_idx, col++, new QLabel(value->str_id()));
    ui->tableValues->setCellWidget(_read_value_idx, col++, new QLabel(value->info()));
    ui->tableValues->setCellWidget(_read_value_idx, col++, value->dataDumpHex());
    ui->tableValues->setCellWidget(_read_value_idx, col++, value->dataEditor());

    cmd_get_desc_value(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id());
}

QString MainWindow::getStringFromStream(QDataStream &stream)
{
    char desc[100];
    stream.readRawData(desc, 100);
    QString string = QString::fromLocal8Bit((const char *)desc, strlen(desc));
    return string;
}

void MainWindow::ans_get_desc_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;
    qDebug() << "Receive get desc answer";
    QString string = getStringFromStream(stream);
    ui->tableValues->setCellWidget(_read_value_idx, 7, new QLabel(string));

    cmd_get_value_limit(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id(), true);


}

void MainWindow::ans_get_limit_low_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;

    qDebug() << "Receive get low limit answer";
    boardValue *value = _list_values.at(_read_value_idx);

    QByteArray dataStream;
    dataStream.reserve(value->size());
    if (stream.readRawData(dataStream.data(), value->size()) > 0)
    {
        QString string = value->getStringFromData(dataStream);
        ui->tableValues->setCellWidget(_read_value_idx, 5, new QLabel(string));
    }

    cmd_get_value_limit(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id(), false);
}

void MainWindow::ans_get_limit_high_received(QDataStream &stream)
{
    if (_read_value_idx < 0 || _read_value_idx >= _list_values.size())
        return;

    qDebug() << "Receive get high limit answer";
    boardValue *value = _list_values.at(_read_value_idx);

    QByteArray dataStream;
    dataStream.reserve(value->size());
    if (stream.readRawData(dataStream.data(), value->size()) > 0)
    {
        QString string = value->getStringFromData(dataStream);
        ui->tableValues->setCellWidget(_read_value_idx, 6, new QLabel(string));
    }

    if (_read_value_idx + 1 < _list_values.size())
    {
        _read_value_idx++;
        cmd_read_value(_list_values.at(_read_value_idx)->group(), _list_values.at(_read_value_idx)->id());
    }
}

void MainWindow::answer_received(QByteArray datagram)
{
     QDataStream stream(datagram);
     struct T_ACQ_ETH_REPLY reply;
     stream.readRawData((char *)&reply,  sizeof(reply) - sizeof(reply.data));
     if (reply.status < 0) {
         qDebug() << "Board returned error status " << reply.status << " on cmd " << reply.cmd;
         return;
     }
     switch (reply.cmd) {
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
         qDebug() << "Unhandled answer " << reply.cmd;
         break;
     }
}

void MainWindow::on_sendValues_clicked()
{
    QHostAddress board_ip(ui->listBoard->itemData(ui->listBoard->currentIndex()).toUInt());

    for (int i = 0; i < _list_values.count(); i++)
    {
        QByteArray newData;
        boardValue* value = _list_values.at(i);
        if (value->getData(newData)) {
            cmd_write_value(value->group(),value->id(), &newData);
        }
    }
}

