#-------------------------------------------------
#
# Project created by QtCreator 2015-01-11T23:12:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#add qExtSerialPort libs
CONFIG += extserialport

TARGET = arduserial
TEMPLATE = app


SOURCES += main.cpp \
    arduserial.cpp

HEADERS  += \
    arduserial.h

FORMS    += \
    arduserial.ui

INCLUDEPATH += /opt/local/include/QtExtSerialPort/
