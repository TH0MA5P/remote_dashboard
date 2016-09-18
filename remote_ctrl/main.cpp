#include <QtWidgets/QApplication>
#include <QMainWindow>
#include <stdint.h>

#include "../common/values.h"

#include "mainwindow.h"
#include "board.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
