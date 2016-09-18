#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtNetwork/QUdpSocket>
#include <QNetworkInterface>
#include <QMainWindow>

#include <stdint.h>

#include "boardvalue.h"
#include "proto.h"

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
    void answer_received(QByteArray datagram);
    void on_buttonScan_clicked();
    void on_listBoard_currentIndexChanged(int index);

    void ans_who_received(QDataStream &stream);
    void ans_get_desc_received(QDataStream &stream);
    void ans_list_received(QDataStream &stream);
    void ans_read_received(QDataStream &stream);

    void on_sendValues_clicked();

private:
    void clear_values();
    void cmd_read_value(quint32 group, quint32 value_idx);
    void cmd_write_value(quint32 group, quint32 value_idx, QByteArray *buf);
    void cmd_get_desc_value(quint32 group, quint32 value_idx);
    QString getStringFromStream(QDataStream &stream);

private:
    Ui::MainWindow *ui;
    proto * prot;
    QList<boardValue*> _list_values;
    int _read_value_idx;              // Current read_value pending
};

#endif // MAINWINDOW_H
