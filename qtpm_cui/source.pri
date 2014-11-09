HEADERS += \
    ../qtpm_cui/qsemver.h \
    ../qtpm_cui/parameterparser.h \
    ../qtpm_cui/licensefiles.h \
    ../qtpm_cui/qpmpackage.h \
    ../qtpm_cui/semver/semver.h \
    ../qtpm_cui/semver/semver-private.h \
    ../qtpm_cui/modulemanager.h \
    ../qtpm_cui/module.h \
    ../qtpm_cui/c3liniearization.h \
    ../qtpm_cui/gitresultparser.h \
    ../qtpm_cui/tasks/resulthash.h \
    ../qtpm_cui/tasks/installtask.h \
    ../qtpm_cui/qarchive.h \
    ../qtpm_cui/tasks/extracttask.h \
    ../qtpm_cui/tasks/initlibtask.h \
    ../qtpm_cui/NLTemplate/NLTemplate/NLTemplate.h \
    ../qtpm_cui/tasks/initapptask.h \
    ../qtpm_cui/tasks/buildtask.h \
    ../qtpm_cui/tasks/externalprocess.h \
    $$PWD/aliasdatabase.h \
    $$PWD/tasks/refreshaliastask.h \
    $$PWD/tasks/searchtask.h \
    $$PWD/tasks/projectfiletask.h \
    $$PWD/platformdatabase.h \
    $$PWD/qtpathresolver.h \
    $$PWD/tasks/configtask.h \
    $$PWD/tasks/cleantask.h

SOURCES += \
    ../qtpm_cui/qsemver.cpp \
    ../qtpm_cui/parameterparser.cpp \
    ../qtpm_cui/licensefiles.cpp \
    ../qtpm_cui/qpmpackage.cpp \
    ../qtpm_cui/semver/spec.cpp \
    ../qtpm_cui/semver/range.cpp \
    ../qtpm_cui/semver/component.cpp \
    ../qtpm_cui/semver/private.cpp \
    ../qtpm_cui/modulemanager.cpp \
    ../qtpm_cui/module.cpp \
    ../qtpm_cui/c3liniearization.cpp \
    ../qtpm_cui/gitresultparser.cpp \
    ../qtpm_cui/tasks/installtask.cpp \
    ../qtpm_cui/qarchive.cpp \
    ../qtpm_cui/tasks/extracttask.cpp \
    ../qtpm_cui/tasks/initlibtask.cpp \
    ../qtpm_cui/NLTemplate/NLTemplate/NLTemplate.cpp \
    ../qtpm_cui/tasks/initapptask.cpp \
    ../qtpm_cui/tasks/buildtask.cpp \
    ../qtpm_cui/tasks/externalprocess.cpp \
    $$PWD/aliasdatabase.cpp \
    $$PWD/tasks/refreshaliastask.cpp \
    $$PWD/tasks/searchtask.cpp \
    $$PWD/tasks/projectfiletask.cpp \
    $$PWD/platformdatabase.cpp \
    $$PWD/qtpathresolver.cpp \
    $$PWD/tasks/configtask.cpp \
    $$PWD/tasks/cleantask.cpp

INCLUDEPATH += \
    ../qtpm_cui/deps/include \
    ../qtpm_cui/NLTemplate/NLTemplate
/include

LIBS += -L$$PWD/../qtpm_cui/deps/lib -larchive -lbz2 -llzma -lz

#QMAKE_LFLAGS += -print-search-dirs
