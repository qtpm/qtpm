#include <QtTest>
#include <QDir>
#include <QDebug>
#include "test_buildoption.h"
#include "tasks/buildtask.h"

BuildOptionTest::BuildOptionTest(QObject *parent) :
    QObject(parent)
{
}

void BuildOptionTest::parse_simple_options()
{
    BuildTask task(QDir::temp(), QDir::temp(), nullptr);

    QStringList params = task.parseBuildOption("--static --prefix=/usr/bin/");

    QCOMPARE(params.length(), 2);
    QCOMPARE(params[0], QString("--static"));
    QCOMPARE(params[1], QString("--prefix=/usr/bin/"));
}

void BuildOptionTest::parse_quoted_options()
{
    BuildTask task(QDir::temp(), QDir::temp(), nullptr);

    QStringList params = task.parseBuildOption("--static --prefix \"/Program Files\" --debug");
    QCOMPARE(params.length(), 4);
    QCOMPARE(params[0], QString("--static"));
    QCOMPARE(params[1], QString("--prefix"));
    QCOMPARE(params[2], QString("\"/Program Files\""));
    QCOMPARE(params[3], QString("--debug"));
}
