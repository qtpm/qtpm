#include "refreshaliastask.h"
#include "aliasdatabase.h"
#include "externalprocess.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QCoreApplication>
#include <iostream>

RefreshAliasTask::RefreshAliasTask(ParameterParser *param, QObject *parent) :
    QObject(parent), _param(param)
{
}


bool RefreshAliasTask::run()
{
    auto app = QCoreApplication::instance();

    QDir home = QDir::home();
    if (!home.exists(".qtpm")) {
        home.mkdir(".qtpm");
    }
    QDir qtpmDir = QDir(home.filePath(".qtpm"));
    QDir wikiDir = QDir(qtpmDir.filePath("wiki"));

    ExternalProcess process(true);
    bool error = false;

    if (qtpmDir.exists("wiki")) {
        QStringList args;
        args << "pull" << "--depth" << "1";
        process.run("git", args, wikiDir);
        // todo: error check;
        process.waitForFinished();
        if (process.exitCode() != 0) {
            std::cerr << "fail to download" << std::endl;
            error = true;
        }
    } else {
        QStringList args;
        args << "clone" << "--depth" << "1" << "https://github.com/qtpm/qtpm.wiki.git";
        process.run("git", args, wikiDir);
        // todo: error check;
        process.waitForFinished();
        if (process.exitCode() != 0) {
            std::cerr << "fail to download" << std::endl;
            error = true;
        }
    }
    if (error) {
        app->exit(1);
        return false;
    }

    QFile file(wikiDir.filePath("packages.rest"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "fail to open packages.rest" << std::endl;
        app->exit(1);
        return false;
    }
    QTextStream in(&file);
    auto contents = in.readAll();

    AliasDatabase database(qtpmDir);
    database.refresh(contents);
    app->exit(0);
    return true;
}
