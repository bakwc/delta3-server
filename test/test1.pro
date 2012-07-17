#-------------------------------------------------
# This unit test project for server. executable
# Will be generated in the directory `test'
# Maybe we should auto execute tests after building
# `server' project.
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = alltests
CONFIG   += qtestlib
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    tests.cpp \
    ../server/netextract.cpp \
    alltests.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
HEADERS += ../server/utils.h \
    ../server/netextract.h \
    alltests.h
