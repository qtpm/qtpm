#include "test_cleanaction.h"
#include "tasks/cleantask.h"
#include "testutil.h"
#include <QTest>

CleanActionTest::CleanActionTest(QObject *parent) :
    QObject(parent)
{
}

void CleanActionTest::init()
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

    this->_workdir.mkdir("build");

    this->_util->makeFile("qtpackage.pri", "");
}

void CleanActionTest::cleanTest()
{
    CleanTask task(this->_workdir);
    task.run();

    QVERIFY(!this->_workdir.exists("deps.macx"));
    QVERIFY(!this->_workdir.exists("deps.win32"));
    QVERIFY(!this->_workdir.exists("build"));
    QVERIFY(!this->_workdir.exists("qtpackage.pri"));
}

void CleanActionTest::cleanup()
{
    delete this->_util;
    this->_util = nullptr;
}
