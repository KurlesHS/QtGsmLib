#-------------------------------------------------
#
# Project created by QtCreator 2014-10-21T13:00:30
#
#-------------------------------------------------

QT       += core

QT       -= gui

LIBS += -lgtest -lgmock

QMAKE_CXXFLAGS += -std=c++11


include($$PWD/../src/qtgsmlib.pri)

INCLUDEPATH += D:/Develop/LibsAndSdk/gmock-1.7.0/include
INCLUDEPATH += D:/Develop/LibsAndSdk/gtest-1.7.0/include

TARGET = QtGsmLibTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp
