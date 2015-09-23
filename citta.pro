#-------------------------------------------------
#
# Project created by QtCreator 2015-09-17T19:31:59
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += network
QT       += xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = citta
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    pic.qrc

DISTFILES += \
    pics/see.png
