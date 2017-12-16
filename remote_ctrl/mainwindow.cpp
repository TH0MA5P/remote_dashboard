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
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&board, SIGNAL(updated()), this, SLOT(dataReceived()));

    on_buttonScan_clicked();
}

MainWindow::~MainWindow()
{
    clear_values();
    delete ui;
}

void MainWindow::clear_values()
{
    ui->tableValues->clearContents();
    ui->tableValues->setRowCount(0);
}

void MainWindow::dataReceived()
{
    int nbVal = board._list_values.size();
    ui->tableValues->setRowCount(nbVal);

    for (int i = 0; i < nbVal; i++)
    {
        int col = 0;
        BoardValue * value = board._list_values.at(i);
        ui->tableValues->setCellWidget(i, col++, new QLabel(value->str_group()));
        ui->tableValues->setCellWidget(i, col++, new QLabel(value->str_id()));
        ui->tableValues->setCellWidget(i, col++, new QLabel(value->info()));
        ui->tableValues->setCellWidget(i, col++, value->dataDumpHex());
        ui->tableValues->setCellWidget(i, col++, value->dataEditor());
        ui->tableValues->setCellWidget(i, col++, new QLabel(value->minVal));
        ui->tableValues->setCellWidget(i, col++, new QLabel(value->maxVal));
        ui->tableValues->setCellWidget(i, col++, new QLabel(value->desc));
    }
}

void MainWindow::on_buttonScan_clicked()
{
    ui->listBoard->clear();

    board.clear_values();
    board.cmd_get_list();
}

void MainWindow::on_listBoard_currentIndexChanged(int index)
{
    clear_values();

    quint32 ip = ui->listBoard->itemData(index).toUInt();
    QHostAddress board_ip(ip);

    board.clear_values();
    board.cmd_get_list();
}


void MainWindow::on_sendValues_clicked()
{
    QHostAddress board_ip(ui->listBoard->itemData(ui->listBoard->currentIndex()).toUInt());

    for (int i = 0; i < board._list_values.count(); i++)
    {
        QByteArray newData;
        BoardValue* value = board._list_values.at(i);
        if (value->getData(newData)) {
            board.cmd_write_value(value->group(),value->id(), &newData);
        }
    }
}

