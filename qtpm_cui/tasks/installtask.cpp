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
#include "buildtask.h"
#include "projectfiletask.h"
#include "platformdatabase.h"
#include <iostream>
#include <QDebug>

static const QString SourceDir = ".qtpm";

InstallTask::InstallTask(ParameterParser *param, QObject *parent) :
    QObject(parent), _param(param)
{
}

void InstallTask::run()
{
    bool verbose = this->_param->flag("verbose");
    auto app = QCoreApplication::instance();

    PlatformDatabase database(this);
    QString qtpathParam = this->_param->param("qtdir");
    if (qtpathParam.isEmpty()) {
        if (!database.resolveQtPath()) {
            std::cerr << "Can't find qt directory. Use --qtdir option or set QTDIR by using qtpm config subcommand." << std::endl;
            if (app) {
                app->exit(1);
            }
        }
    } else {
        if (!database.setQtPath(qtpathParam, false)) {
            std::cerr << "Specified qt directory is wrong. <qtdir>/bin/qmake should exist." << std::endl;
            if (app) {
                app->exit(1);
            }
        }
    }

    QDir currentDir = QDir::current();
    if (!currentDir.exists(SourceDir)) {
        currentDir.mkpath(SourceDir);
    }
    QDir sourceDir = QDir(currentDir.filePath(SourceDir));
    QDir installDir = database.installDirectory();

    if (verbose) {
        std::cout << "source dir: " << currentDir.filePath(SourceDir).toStdString() << std::endl;
        std::cout << "install dir: " << installDir.path().toStdString() << std::endl;
    }

    auto args = this->_param->args();
    ModuleManager moduleManager;
    moduleManager.setVerbose(verbose);
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
        for (const QString& arg : args) {
            //qDebug () << "adding from command: " << arg;
            moduleManager.addModuleDependent("app", arg, sourceDir);
        }

    } else {
        package = new QpmPackage(currentDir);
        QMapIterator<QString, QString> i(package->bundlePackages());
        while (i.hasNext()) {
            i.next();
            qDebug () << "adding from package as bundle: " << i.value();
            moduleManager.addModuleDependent("app", i.value(), sourceDir);
        }
        if (!this->_param->flag("bundle-only")) {
            QMapIterator<QString, QString> i(package->developmentPackages());
            while (i.hasNext()) {
                i.next();
                qDebug () << "adding from package as dev: " << i.value();
                moduleManager.addModuleDependent("app", i.value(), sourceDir);
            }
        }
    }
    QList<Module*> modules = moduleManager.shift();
    while (modules.length() > 0) {
        foreach(Module* module, modules) {
            qDebug() << module->name();
            if (module->status() == Module::LocalFileModule) {
                QDir moduleDir(QDir::current());
                moduleDir.cd(".qtpm");
                moduleDir.cd(module->name());
                BuildTask task(moduleDir, this->_param->flag("verbose"), nullptr, &database, &installDir);
                task.run();
            } else if (module->status() == Module::LocalDirModule) {
                QDir moduleDir(module->longPath());
                qDebug() << "moduleDir" << module->longPath();
                BuildTask task(moduleDir, this->_param->flag("verbose"), nullptr, &database, &installDir);
                task.run();
            } else if (module->status() == Module::RemoteBranchModule) {
                qDebug() << "remote name:" << module->name();
                qDebug() << module->longPath();
                qDebug() << module->package()->filePath();
            } else if (module->status() == Module::RemoteVersionModule) {
                qDebug() << "remote name:" << module->name();
                QDir moduleDir(module->longPath());
                BuildTask task(moduleDir, this->_param->flag("verbose"), nullptr, &database, &installDir);
                task.run();
            } else {
                qDebug() << "other:" << module->status() << module->name();
            }
        }
        modules = moduleManager.shift();
    }

    ProjectFileTask projectFileTask(currentDir);
    projectFileTask.write();

    if (app) {
        app->exit(0);
    }
}
