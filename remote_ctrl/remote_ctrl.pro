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
           proto.cpp \
           board.cpp

HEADERS  += mainwindow.h \
            boardvalue.h \
            ../common/commands.h \
            ../common/protocol.h \
            ../common/trace.h \
            ../common/udpProtocol.h \
            ../common/values.h \
            proto.h \
            board.h


FORMS    += mainwindow.ui
