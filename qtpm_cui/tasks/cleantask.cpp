#include "cleantask.h"
#include <QCoreApplication>
#include <QFileInfo>

CleanTask::CleanTask(const QDir &dir, QObject *parent) :
    QObject(parent), _dir(dir)
{
}

void CleanTask::run()
{
    auto app = QCoreApplication::instance();
    auto infos = this->_dir.entryInfoList(QStringList() << "deps.*", QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QFileInfo& info : infos) {
        if (info.isDir()) {
            QDir depsDir(info.absoluteFilePath());
            depsDir.removeRecursively();
        }
    }
    if (this->_dir.exists("build")) {
        QDir buildDir(this->_dir.filePath("build"));
        buildDir.removeRecursively();
    }
    if (this->_dir.exists("qtpackage.pri")) {
        this->_dir.remove("qtpackage.pri");
    }
    if (app) {
        app->exit(0);
    }
}
