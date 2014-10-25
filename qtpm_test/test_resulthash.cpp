#include "test_resulthash.h"
#include <QtTest>
#include "tasks/resulthash.h"

ResultHashTest::ResultHashTest(QObject *parent) :
    QObject(parent)
{
}

void ResultHashTest::hashTest()
{
    ResultHash<int, int> hash;
    hash.insert(1, 2);
    hash.insert(3, 4);
    auto result = hash.results();
    QCOMPARE(result[1], 2);
    QCOMPARE(result[3], 4);
}
