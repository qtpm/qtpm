#include <QCoreApplication>
#include "parameterparser.h"
#include "tasks/installtask.h"
#include "tasks/initlibtask.h"
#include "tasks/initapptask.h"
#include "tasks/buildtask.h"
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

    QString defaultOSName;
#if defined(Q_WS_WIN)
    defaultOSName = "win";
#elif defined(Q_OS_LINUX)
    defaultOSName = "linux";
#elif defined(Q_OS_MACX)
    defaultOSName = "macx";
#endif
    QDir depsDir(QDir::current().filePath(QString("deps.%1").arg(defaultOSName)));
    if (subcommand == ParameterParser::BuildAction) {
        auto command = new BuildTask(QDir::current(), depsDir, &parser, &a);
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::InstallAction) {
        auto command = new InstallTask(&parser, &a);
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::InitAppAction) {
        auto command = new InitAppTask(&parser, &a);
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::InitLibAction) {
        auto command = new InitLibTask(&parser, &a);
        launch = true;
        QTimer::singleShot(0, command, SLOT(run()));
    } else if (subcommand == ParameterParser::CommandListAction) {
        parser.showCommandHelp();
    } else {
        qDebug() << subcommand;
    }
    if (launch) {
        return a.exec();
    } else {
        return returnCode;
    }
}
