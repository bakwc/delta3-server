#-------------------------------------------------
#
# Project created by QtCreator 2012-07-15T11:41:55
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = alltests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += \
    tests.cpp \
    alltests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
HEADERS += ../server/utils.h \
    ../server/netextract.h \
    alltests.h
