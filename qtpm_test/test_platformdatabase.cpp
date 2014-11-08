#include "test_platformdatabase.h"
#include "platformdatabase.h"
#include <QDir>
#include <QTest>

PlatformDatabaseTest::PlatformDatabaseTest(QObject *parent) :
    QObject(parent)
{
}

void PlatformDatabaseTest::init()
{
#if defined(Q_WS_WIN)
    QString qtpathStr = "win";
#elif defined(Q_OS_LINUX)
    QString qtpathStr = "unix";
#elif defined(Q_OS_MACX)
    QString qtpathStr = "Qt/5.3/clang_64";
#endif
    this->_platform = new PlatformDatabase();
    this->_platform->setQtPath(QDir::home().filePath(qtpathStr), false);
}

void PlatformDatabaseTest::currentPlatformTest()
{
#if defined(Q_WS_WIN)
    QString result = "win32";
#elif defined(Q_OS_LINUX)
    QString result = "linux";
#elif defined(Q_OS_MACX)
    QString result = "macx-clang";
#endif
    QCOMPARE(this->_platform->currentPlatform(), result);
}

void PlatformDatabaseTest::searchRelatedPlatformTest()
{
    auto platforms = this->_platform->searchRelatedPlatforms("macx-clang");
    QVERIFY(platforms.contains("unix"));
    QVERIFY(platforms.contains("posix"));
    QVERIFY(platforms.contains("mac"));
    QVERIFY(platforms.contains("macx"));
    QVERIFY(platforms.contains("osx"));
}

void PlatformDatabaseTest::cleanup()
{
    delete this->_platform;
    this->_platform = nullptr;
}
