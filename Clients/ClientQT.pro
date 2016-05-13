#-------------------------------------------------
#
# Project created by QtCreator 2016-04-02T18:03:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ClientQT
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    dialogauthorization.cpp \
    TCPSocket.cpp \
    Oerror.cpp \
    client.cpp \
    messagehandler.cpp \
    popup.cpp

HEADERS  += mainwindow.h \
    dialogauthorization.h \
    TCPSocket.h \
    Oerror.h \
    client.h \
    messagehandler.h \
    popup.h

FORMS    += mainwindow.ui \
    dialogauthorization.ui

RESOURCES += \
    images.qrc

DISTFILES +=
