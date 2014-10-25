#include "test_qsemvertest.h"
#include <QtTest>
#include "qsemver.h"

QSemverTest::QSemverTest()
{
}

void QSemverTest::filterTest()
{
    auto versions = QStringList() << "1.0.5" << "1.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("1.0.x", versions);

    QCOMPARE(result.length(), 2);
    QCOMPARE(result[0], QString("1.0.1"));
    QCOMPARE(result[1], QString("1.0.5"));
}

void QSemverTest::filterTest2()
{
    auto versions = QStringList() << "1.0.5" << "1.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("~1.0.2", versions);

    QCOMPARE(result.length(), 1);
    QCOMPARE(result[0], QString("1.0.5"));
}

void QSemverTest::filterTest3()
{
    auto versions = QStringList() << "1.0.5" << "1.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("~2.0.0", versions);

    QCOMPARE(result.length(), 0);
}

void QSemverTest::filterTest4()
{
    auto versions = QStringList() << "1.0.5" << "2.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("> 2.0.0", versions);

    QCOMPARE(result.length(), 1);
}

void QSemverTest::filterTest5()
{
    auto versions = QStringList() << "1.0.5" << "2.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("< 1.1.0", versions);

    QCOMPARE(result.length(), 2);
}

void QSemverTest::filterTest6()
{
    auto versions = QStringList() << "1.0.5" << "2.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter(">= 1.2.3", versions);

    QCOMPARE(result.length(), 2);
}

void QSemverTest::filterTest7()
{
    auto versions = QStringList() << "1.0.5" << "2.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("<= 1.2.3", versions);

    QCOMPARE(result.length(), 3);
}

void QSemverTest::filterTest8()
{
    auto versions = QStringList() << "1.0.5" << "2.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("2.x", versions);

    QCOMPARE(result.length(), 1);
}

void QSemverTest::filterTest9()
{
    auto versions = QStringList() << "1.0.5" << "2.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("1.0.x", versions);

    QCOMPARE(result.length(), 2);
}

void QSemverTest::filterWithReverseTest()
{
    auto versions = QStringList() << "1.0.5" << "1.1.2" << "1.2.3" << "1.0.1";
    auto result = QSemVer::filter("1.0.x", versions, true);

    QCOMPARE(result.length(), 2);
    QCOMPARE(result[0], QString("1.0.5"));
    QCOMPARE(result[1], QString("1.0.1"));
}

void QSemverTest::filterWithMultipleRangesTest()
{
    auto ranges = QStringList() << "1.0.x" << ">1.0.3";
    auto versions = QStringList() << "1.0.5" << "1.1.2" << "1.2.3" << "1.0.1" << "1.0.6";
    auto result = QSemVer::filter(ranges, versions);

    QCOMPARE(result.length(), 2);
    QCOMPARE(result[0], QString("1.0.5"));
    QCOMPARE(result[1], QString("1.0.6"));

}

void QSemverTest::filterWithMultipleRangesWithReverseTest()
{
    auto ranges = QStringList() << "1.0.x" << ">1.0.3";
    auto versions = QStringList() << "1.0.5" << "1.1.2" << "1.2.3" << "1.0.1" << "1.0.6";
    auto result = QSemVer::filter(ranges, versions, true);

    QCOMPARE(result.length(), 2);
    QCOMPARE(result[0], QString("1.0.6"));
    QCOMPARE(result[1], QString("1.0.5"));

}
