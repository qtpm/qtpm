#include "test_projecttask.h"
#include "testutil.h"
#include "tasks/projectfiletask.h"
#include <QTest>
#include <QDebug>

ProjectTaskTest::ProjectTaskTest(QObject *parent) :
    QObject(parent)
{
}

void ProjectTaskTest::init()
{
    this->_util = new TestUtil();
    this->_workdir = this->_util->makeTestDir();

    this->_workdir.mkpath("deps.macx");

    this->_util->makeFile("deps.macx/zlib.pri", "");
    this->_util->makeFile("deps.macx/libpng.pri", "");

    this->_workdir.mkpath("deps.win32");

    this->_util->makeFile("deps.win32/zlib.pri", "");
    this->_util->makeFile("deps.win32/libpng.pri", "");
    this->_util->makeFile("deps.win32/pthread.pri", "");
}

void ProjectTaskTest::parseFolderTest()
{

    ProjectFileTask task(this->_workdir);
    QVERIFY(task.hasEnv("macx"));
    QVERIFY(task.hasEnv("win32"));

    QVERIFY(task.hasLib("macx", "zlib"));
    QVERIFY(task.hasLib("macx", "libpng"));
    QVERIFY(task.hasLib("win32", "zlib"));
    QVERIFY(task.hasLib("win32", "libpng"));
    QVERIFY(task.hasLib("win32", "pthread"));
}

void ProjectTaskTest::dumpInfoTest()
{
    ProjectFileTask task(this->_workdir);

    QString result = task.dump();
    auto lines = result.split("\n");
    QVERIFY(lines.contains("macx {"));
    QVERIFY(lines.contains("    include(deps.macx/zlib.pri)"));
    QVERIFY(lines.contains("    include(deps.macx/libpng.pri)"));
    QVERIFY(lines.contains("win32 {"));
    QVERIFY(lines.contains("    include(deps.win32/zlib.pri)"));
    QVERIFY(lines.contains("    include(deps.win32/libpng.pri)"));
    QVERIFY(lines.contains("    include(deps.win32/pthread.pri)"));
}

void ProjectTaskTest::writeInfoTest()
{
    ProjectFileTask task(this->_workdir);
    task.write();
    QVERIFY(this->_workdir.exists("qtpackage.pri"));
}

void ProjectTaskTest::modifyRootProjectTest()
{
    ProjectFileTask task(this->_workdir);
    this->_util->makeFile("testproject.pro", "SOURCES=test.cpp\n");
    task.write();
    auto contents = this->_util->readFile("testproject.pro");
    QVERIFY(contents.contains("SOURCES=test.cpp"));
    QVERIFY(contents.contains("include(qtpackage.pri)"));
}

void ProjectTaskTest::cleanup()
{
    delete this->_util;
    this->_util = nullptr;
}
