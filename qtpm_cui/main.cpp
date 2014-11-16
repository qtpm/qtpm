#include <QCoreApplication>
#include "parameterparser.h"
#include "tasks/installtask.h"
#include "tasks/initlibtask.h"
#include "tasks/initapptask.h"
#include "tasks/buildtask.h"
#include "tasks/refreshaliastask.h"
#include "tasks/searchtask.h"
#include "tasks/configtask.h"
#include "tasks/cleantask.h"
#include "platformdatabase.h"
#include <QTimer>
#include <QDebug>
#include <QDir>

static const char* APPLICATION_NAME = "qtpm";
static const char* VERSION = "0.1.0";

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName(APPLICATION_NAME);
    a.setApplicationVersion(VERSION);

    ParameterParser parser;
    auto subcommand = parser.parse(a.arguments(), &a);
    bool launch = false;
    int returnCode = 0;
    QObject* basecommand = nullptr;
    if (subcommand == ParameterParser::BuildAction) {
        auto command = new BuildTask(QDir::current(), parser.flag("verbose"), &parser, nullptr, nullptr, &a);
        basecommand = command;
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::InstallAction) {
        auto command = new InstallTask(&parser, &a);
        basecommand = command;
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::InitAppAction) {
        auto command = new InitAppTask(&parser, &a);
        basecommand = command;
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::InitLibAction) {
        auto command = new InitLibTask(&parser, &a);
        basecommand = command;
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::RefreshAction) {
        auto command = new RefreshAliasTask(&parser, &a);
        basecommand = command;
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::SearchAction) {
        auto command = new SearchTask(&parser, &a);
        basecommand = command;
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::ConfigAction) {
        auto command = new ConfigTask(&parser, &a);
        basecommand = command;
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::CleanAction) {
        auto command = new CleanTask(QDir::current(), parser.flag("verbose"), &a);
        basecommand = command;
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::CommandListAction) {
        parser.showCommandHelp();
    } else {
        parser.showCommandHelp();
    }
    if (launch) {
        returnCode = a.exec();
    }
    if (basecommand) {
        delete basecommand;
    }
    return returnCode;
}
