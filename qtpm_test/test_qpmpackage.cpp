#include <QtTest>
#include "test_qpmpackage.h"
#include "testutil.h"
#include "qpmpackage.h"

QPMPackageTest::QPMPackageTest(QObject *parent) :
    QObject(parent)
{
}

void QPMPackageTest::initializeTest()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->setDescription("test project");
    package->setAuthors(QStringList() << "Yoshiki Shibukawa <yoshiki@shibu.jp>");
    delete package;
    QVERIFY(dir.exists("qtpackage.ini"));
}

void QPMPackageTest::existanceTest1()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    QVERIFY(!QpmPackage::hasQtPackageIni(dir));
}

void QPMPackageTest::existanceTest2()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    delete package;
    QVERIFY(QpmPackage::hasQtPackageIni(dir));
}

void QPMPackageTest::searchTest()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    auto iniPath = package->filePath();
    delete package;

    dir.mkdir("child_dir");
    auto childDir = QDir(dir.filePath("child_dir"));

    auto setting = QpmPackage::search(childDir);
    QVERIFY(setting != nullptr);
    QCOMPARE(setting->filePath(), iniPath);
}

void QPMPackageTest::searchTest2()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    // Doesn't have package.
    //auto package = new QpmPackage(dir);
    //delete package;

    dir.mkdir("child_dir");
    auto childDir = QDir(dir.filePath("child_dir"));

    auto setting = QpmPackage::search(childDir);
    QVERIFY(setting == nullptr);
}

void QPMPackageTest::addBundlePackageTest1()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->addBundlePackage("testlib", "1.0.1");
    delete package;

    auto package2 = new QpmPackage(dir);
    QVERIFY(package2->hasBundlePackage("testlib"));
    QCOMPARE(package2->packageVersion("testlib"), QString("1.0.1"));

    auto packages = package2->bundlePackages();
    QCOMPARE(packages.count(), 1);
    QCOMPARE(packages["testlib"], QString("1.0.1"));

    delete package2;
}

void QPMPackageTest::addBundlePackageTest2()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->addBundlePackage("testlib", "1.0.1");
    delete package;

    auto package2 = new QpmPackage(dir);
    QVERIFY(!package2->hasBundlePackage("testlib2"));
    delete package2;
}

void QPMPackageTest::updateBundlePackageTest1()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->addBundlePackage("testlib", "1.0.1");
    delete package;

    auto package2 = new QpmPackage(dir);
    QVERIFY(package2->updateBundlePackage("testlib", "1.0.10"));
    QCOMPARE(package2->packageVersion("testlib"), QString("1.0.10"));
    delete package2;
}

void QPMPackageTest::updateBundlePackageTest2()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    delete package;

    auto package2 = new QpmPackage(dir);
    QVERIFY(!package2->updateBundlePackage("testlib", "1.0.20"));
    delete package2;
}

void QPMPackageTest::addDevelopmentPackageTest1()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->addDevelopmentPackage("testlib", "1.0.1");
    delete package;

    auto package2 = new QpmPackage(dir);
    QVERIFY(package2->hasDevelopmentPackage("testlib"));
    QCOMPARE(package2->packageVersion("testlib"), QString("1.0.1"));

    auto packages = package2->developmentPackages();
    QCOMPARE(packages.count(), 1);
    QCOMPARE(packages["testlib"], QString("1.0.1"));

    delete package2;
}

void QPMPackageTest::addDevelopmentPackageTest2()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->addDevelopmentPackage("testlib", "1.0.1");
    delete package;

    auto package2 = new QpmPackage(dir);
    QVERIFY(!package2->hasDevelopmentPackage("testlib2"));
    delete package2;
}

void QPMPackageTest::updateDevelopmentPackageTest1()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->addDevelopmentPackage("testlib", "1.0.1");
    delete package;

    auto package2 = new QpmPackage(dir);
    QVERIFY(package2->updateDevelopmentPackage("testlib", "1.0.10"));
    QCOMPARE(package2->packageVersion("testlib"), QString("1.0.10"));
    delete package2;
}

void QPMPackageTest::updateDevelopmentPackageTest2()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    delete package;

    auto package2 = new QpmPackage(dir);
    QVERIFY(!package2->updateDevelopmentPackage("testlib", "1.0.20"));
    delete package2;
}

void QPMPackageTest::duplicatePackageDetectionTest1()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->addBundlePackage("testlib", "1.0.1");
    QVERIFY(!package->addDevelopmentPackage("testlib", "1.0.1"));
    delete package;
}

void QPMPackageTest::duplicatePackageDetectionTest2()
{
    TestUtil util;
    auto dir = util.makeTestDir();
    auto package = new QpmPackage(dir);
    package->setName("test");
    package->addDevelopmentPackage("testlib", "1.0.1");
    QVERIFY(!package->addBundlePackage("testlib", "1.0.1"));
    delete package;
}

