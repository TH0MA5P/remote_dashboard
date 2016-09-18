#-------------------------------------------------
#
# Project created by QtCreator 2012-10-09T16:00:26
#
#-------------------------------------------------

QT       += core gui network widgets

TARGET = remote_ctrl
TEMPLATE = app

SOURCES += main.cpp\
           mainwindow.cpp \
           boardvalue.cpp \
    proto.cpp

HEADERS  += mainwindow.h \
            boardvalue.h \
            ../common/udp.h \
    proto.h


FORMS    += mainwindow.ui
