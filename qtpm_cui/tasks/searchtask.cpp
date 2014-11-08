#include <iostream>
#include <QCoreApplication>
#include <QDebug>

#include "searchtask.h"
#include "aliasdatabase.h"
#include "parameterparser.h"
#include "refreshaliastask.h"

SearchTask::SearchTask(ParameterParser *param, QObject *parent) :
    QObject(parent), _param(param)
{
}


void SearchTask::run()
{
    auto app = QCoreApplication::instance();
    auto param = this->_param;

    QDir home = QDir::home();
    QDir qtpmDir = QDir(home.filePath(".qtpm"));
    if (!home.exists(".qtpm") || !qtpmDir.exists("alias.ini")) {
        RefreshAliasTask task(param);
        if (!task.run()) {
            return;
        }
    }

    AliasDatabase database(qtpmDir);
    QList<DatabaseEntry> result;
    if (param->flag("name")) {
        int distance = param->param("distance").toInt();
        result = database.search_near_title(param->args()[0], distance);
    } else {
        result = database.search(param->args()[0]);
    }

    foreach (const DatabaseEntry& entry, result) {
        std::cout << entry.name().toStdString() << ": " << entry.url().toStdString() << std::endl;
        std::cout << "    " << entry.description().toStdString() << std::endl << std::endl;
    }

    app->exit(0);
}
