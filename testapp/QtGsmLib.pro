#-------------------------------------------------
#
# Project created by QtCreator 2014-10-20T14:21:39
#
#-------------------------------------------------

QT       += core

QT       -= gui

QMAKE_CXXFLAGS += -std=c++11


TARGET = QtGsmLib
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += TEST_MULTIPART TEST_SIMPLE TEST_MMS TEST_SINGLE

include($$PWD/../src/qtgsmlib.pri)

SOURCES += main.cpp

HEADERS += \
    main.h
