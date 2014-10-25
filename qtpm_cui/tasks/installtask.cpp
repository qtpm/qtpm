#include "installtask.h"
#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QDir>
#include <QRegExp>
#include "parameterparser.h"
#include "gitresultparser.h"
#include "modulemanager.h"
#include "qpmpackage.h"
#include "resulthash.h"
#include <iostream>

static const QString SourceDir = "third_party";
static const QString DestinationDir = "deps";

InstallTask::InstallTask(ParameterParser *param, QObject *parent) :
    QObject(parent), _param(param)
{
}

void InstallTask::run()
{
    qDebug() << "install command";
    auto app = QCoreApplication::instance();
    QDir currentDir = QDir::current();
    if (currentDir.exists(SourceDir)) {
        currentDir.mkdir(SourceDir);
    }
    QDir installDir = QDir(currentDir.filePath(SourceDir));

    if (currentDir.exists(DestinationDir)) {
        currentDir.mkdir(DestinationDir);
    }
    QDir depsDir = QDir(currentDir.filePath(DestinationDir));

    auto args = this->_param->args();
    ModuleManager moduleManager;
    QpmPackage* package = nullptr;

    if (QpmPackage::hasQtPackageIni(currentDir)) {
        package = new QpmPackage(currentDir);
    }

    if ((args.length() == 0) and !package) {
        std::cerr << "Couldn't read dependencies to install." << std::endl
                  << "You should specify one ore more modules on parameters or use qtpackage.ini." << std::endl;
        if (app) {
            app->exit(1);
        }
        return;
    }

    if (args.length() != 0) {
        if ((this->_param->flag("save") or this->_param->flag("save-dev")) and !package) {
            std::cerr << "--save and --save-dev options need qtpackage.ini file to store information." << std::endl
                      << "Run 'qtpm init' option first." << std::endl;
            if (app) {
                app->exit(1);
            }
            return;
        }
        foreach (const QString& arg, args) {
            moduleManager.addModuleDependent("app", arg, installDir);
        }

    } else {
        package = new QpmPackage(currentDir);
        QMapIterator<QString, QString> i(package->bundlePackages());
        while (i.hasNext()) {
            i.next();
            moduleManager.addModuleDependent("app", i.value(), installDir);
        }
        if (!this->_param->flag("bundle-only")) {
            QMapIterator<QString, QString> i(package->developmentPackages());
            while (i.hasNext()) {
                i.next();
                moduleManager.addModuleDependent("app", i.value(), installDir);
            }
        }
    }
    QList<Module*> modules = moduleManager.shift();
    while (modules.length() > 0) {
        foreach(Module* module, modules) {
            if (module->status() == Module::LocalModule) {
                qDebug() << "local name:" << module->name();
            } else if (module->status() == Module::RemoteModule) {
                qDebug() << "remote name:" << module->name();
            }
        }
        modules = moduleManager.shift();
    }

    if (app) {
        app->exit(0);
    }
}
