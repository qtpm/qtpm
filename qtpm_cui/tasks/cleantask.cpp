#include "cleantask.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <iostream>

CleanTask::CleanTask(const QDir &dir, bool verbose, QObject *parent) :
    QObject(parent), _dir(dir), _verbose(verbose)
{
}

void CleanTask::run()
{
    auto app = QCoreApplication::instance();
    auto infos = this->_dir.entryInfoList(QStringList() << "deps.*", QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo& info : infos) {
        if (info.isDir()) {
            QDir depsDir(info.absoluteFilePath());
            if (this->_verbose) {
                std::cout << "removing " << depsDir.path().toStdString() << std::endl;
            }
            depsDir.removeRecursively();
        }
    }
    if (this->_dir.exists("build")) {
        QDir buildDir(this->_dir.filePath("build"));
        if (this->_verbose) {
            std::cout << "removing " << buildDir.path().toStdString() << std::endl;
        }
        buildDir.removeRecursively();
    }
    if (this->_dir.exists(".qtpm")) {
        QDir workDir(this->_dir.filePath(".qtpm"));
        if (this->_verbose) {
            std::cout << "removing " << workDir.path().toStdString() << std::endl;
        }
        workDir.removeRecursively();
    }
    if (this->_dir.exists("qtpackage.pri")) {
        if (this->_verbose) {
            std::cout << "removing qtpackage.pri" << std::endl;
        }
        this->_dir.remove("qtpackage.pri");
    }
    if (app) {
        app->exit(0);
    }
}
