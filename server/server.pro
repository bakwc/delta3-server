#-------------------------------------------------
#
# Project created by QtCreator 2012-07-09T12:53:45
#
#-------------------------------------------------

QT       += core network
QT       -= gui

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp \
    server.cpp \
    application.cpp \
    client.cpp

HEADERS += \
    server.h \
    application.h \
    client.h
