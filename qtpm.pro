TEMPLATE = subdirs

SUBDIRS += \
    qtpm_cui \
    qtpm_test

CONFIG += c++11

QMAKE_MAC_SDK = macosx10.9

qpm_test.depends = qtpm_cui
