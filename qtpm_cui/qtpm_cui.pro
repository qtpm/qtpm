#-------------------------------------------------
#
# Project created by QtCreator 2014-09-27T13:19:54
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = qtpm
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app


SOURCES += main.cpp \
    installworker.cpp \
    initlibworker.cpp \
    basetask.cpp

HEADERS += \
    parameterparser.h \
    installworker.h \
    initlibworker.h \
    basetask.h

RESOURCES += \
    resource.qrc

include(source.pri)
