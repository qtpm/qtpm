#include "configtask.h"
#include <QCoreApplication>
#include "platformdatabase.h"
#include "parameterparser.h"
#include <iostream>

ConfigTask::ConfigTask(ParameterParser *param, QObject *parent) :
    QObject(parent), _param(param)
{
}

void ConfigTask::run()
{
    auto app = QCoreApplication::instance();
    PlatformDatabase database(this);
    auto args = this->_param->args();
    switch (args.count()) {
    case 0:
        if (database.resolveQtPath()) {
            this->_showQtDir(database);
            this->_showPlatform(database);
            this->_showPlatformAlias(database);
        } else {
            std::cerr << "Can't detect QTDIR" << std::endl;
        }
        break;
    case 1:
        if (database.resolveQtPath()) {
            if (args[0] == "QTDIR") {
                this->_showQtDir(database);
            } else if (args[0] == "PLATFORM") {
                this->_showPlatform(database);
            } else if (args[0] == "PLATFORMALIAS") {
                this->_showPlatformAlias(database);
            } else {
                std::cerr << "Can not find " << args[0].toStdString() << std::endl;
            }
        } else {
            std::cerr << "Can't detect QTDIR" << std::endl;
        }
        break;
    case 2:
        if (args[0] == "QTDIR") {
            if (!database.setQtPath(args[1], true)) {
                std::cerr << args[1].toStdString() << " is not valid QtDir. Can not find qmake." << std::endl;
            }
        } else {
            std::cerr << "Can not set " << args[1].toStdString() << std::endl;
        }
        break;
    }
    if (app) {
        app->exit(0);
    }
}

void ConfigTask::_showQtDir(PlatformDatabase &database) const
{
    std::cout << "QTDIR" << ":" << std::endl
                  << "    " << database.qtPath().toStdString()
                  << " " << database.detectMethod().toStdString() << std::endl;
}

void ConfigTask::_showPlatform(PlatformDatabase &database) const
{
    std::cout << "PLATFORM" << ":" << std::endl
              << "    " << database.currentPlatform().toStdString() << std::endl;
}

void ConfigTask::_showPlatformAlias(PlatformDatabase &database) const
{
    std::cout << "PLATFORMALIAS" << ":" << std::endl
              << "    " << database.searchRelatedPlatforms().join(", ").toStdString() << std::endl;
}
