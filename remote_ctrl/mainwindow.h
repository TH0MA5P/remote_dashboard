#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtNetwork/QUdpSocket>
#include <QNetworkInterface>
#include <QMainWindow>

#include <stdint.h>

#include "boardvalue.h"
#include "proto.h"
#include "board.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_buttonScan_clicked();
    void on_listBoard_currentIndexChanged(int index);

    void on_sendValues_clicked();

    void dataReceived();

private:
    void clear_values();

private:
    Ui::MainWindow *ui;
    Board board;
};

#endif // MAINWINDOW_H
