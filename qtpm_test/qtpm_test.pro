#-------------------------------------------------
#
# Project created by QtCreator 2014-09-27T13:44:59
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = qtpm_test
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app


SOURCES += \
    main.cpp \
    testutil.cpp \
    test_qpmpackage.cpp \
    test_licensefiles.cpp \
    test_parameterparser.cpp \
    test_qsemvertest.cpp \
    test_module.cpp \
    test_modulemanager.cpp \
    test_c3linearization.cpp \
    test_gitresultparser.cpp \
    test_resulthash.cpp \
    test_archive.cpp \
    test_buildoption.cpp \
    test_aliasdatabase.cpp \
    test_projecttask.cpp \
    test_platformdatabase.cpp \
    test_cleanaction.cpp


INCLUDEPATH += \
    ../qtpm_cui


DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    testutil.h \
    test_qpmpackage.h \
    test_licensefiles.h \
    test_parameterparser.h \
    test_qsemvertest.h \
    test_module.h \
    test_modulemanager.h \
    test_c3linearization.h \
    test_gitresultparser.h \
    test_resulthash.h \
    test_archive.h \
    test_buildoption.h \
    test_aliasdatabase.h \
    test_projecttask.h \
    test_platformdatabase.h \
    test_cleanaction.h

include(../qtpm_cui/source.pri)

RESOURCES += \
    testresource.qrc
