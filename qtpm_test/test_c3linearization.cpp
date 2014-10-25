#include <QtTest>
#include "test_c3linearization.h"
#include "c3liniearization.h"
#include <QDebug>

C3LinearizationTest::C3LinearizationTest(QObject *parent) :
    QObject(parent)
{
}

void C3LinearizationTest::linearizeTest_1()
{
    QList<QStringList> mroLists;
    mroLists.append(QStringList() << "A" << "B");
    mroLists.append(QStringList() << "B" << "C");
    auto result = C3Liniearization::linearize(mroLists);
    QVERIFY(!result.hasError());
    QCOMPARE(result.modules, QStringList() << "A" << "B" << "C");
}

void C3LinearizationTest::linearizeTest_2()
{
    QList<QStringList> mroLists;
    mroLists.append(QStringList() << "A" << "B" << "C");
    mroLists.append(QStringList() << "B" << "C");
    auto result = C3Liniearization::linearize(mroLists);
    QVERIFY(!result.hasError());
    QCOMPARE(result.modules, QStringList() << "A" << "B" << "C");
}

void C3LinearizationTest::linearizeTest_3()
{
    QList<QStringList> mroLists;
    mroLists.append(QStringList() << "A" << "B" << "C");
    mroLists.append(QStringList() << "B" << "C");
    mroLists.append(QStringList() << "C" << "D");
    auto result = C3Liniearization::linearize(mroLists);
    QVERIFY(!result.hasError());
    QCOMPARE(result.modules, QStringList() << "A" << "B" << "C" << "D");
}

void C3LinearizationTest::linearizeTest_4()
{
    QList<QStringList> mroLists;
    mroLists.append(QStringList() << "A" << "D");
    mroLists.append(QStringList() << "A" << "C");
    mroLists.append(QStringList() << "B" << "C");
    mroLists.append(QStringList() << "A" << "B");
    mroLists.append(QStringList() << "C" << "D");
    auto result = C3Liniearization::linearize(mroLists);
    QVERIFY(!result.hasError());
    QCOMPARE(result.modules, QStringList() << "A" << "B" << "C" << "D");
}

void C3LinearizationTest::linearizeErrorTest_1()
{
    QList<QStringList> mroLists;
    mroLists.append(QStringList() << "A" << "B");
    mroLists.append(QStringList() << "B" << "A");
    auto result = C3Liniearization::linearize(mroLists);
    QVERIFY(result.hasError());
    QVERIFY(result.errorModules.contains("A"));
    QVERIFY(result.errorModules.contains("B"));
}

void C3LinearizationTest::linearizeErrorTest_2()
{
    QList<QStringList> mroLists;
    mroLists.append(QStringList() << "A" << "B" << "C");
    mroLists.append(QStringList() << "B" << "C");
    mroLists.append(QStringList() << "C" << "A");
    auto result = C3Liniearization::linearize(mroLists);
    QVERIFY(result.hasError());
    QVERIFY(result.errorModules.contains("A"));
    QVERIFY(result.errorModules.contains("B"));
    QVERIFY(result.errorModules.contains("C"));
}

