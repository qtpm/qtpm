#include "test_gitresultparser.h"
#include "gitresultparser.h"
#include <QtTest>

GitResultParserTest::GitResultParserTest(QObject *parent) :
    QObject(parent)
{
}

static const char* src = "25ae77793e5b2d730b31224e2c713d4b1458746a	HEAD\n"
                         "25ae77793e5b2d730b31224e2c713d4b1458746a	refs/heads/master\n"
                         "75c42394b5796a214faf5f17ea66f666ee362208	refs/tags/bugfix\n"
                         "75c42c94b3796a214faf5f17ea66f666ee362208	refs/tags/v0.1.0\n"
                         "31f86a2f83094b466f8a8d053e285dc3ff24000e	refs/tags/v0.1.1\n"
                         "fb14bedca6745c8ed56cd088dcbd739279d2d951	refs/tags/v0.1.2\n"
                         "542f65eb70e85dd0d6b7877b626999ed57c22178	refs/tags/v0.1.3\n";


void GitResultParserTest::parseReferenceListTest()
{
    QByteArray input(src);
    auto result = GitResultParser::parseReferenceList(input);
    QCOMPARE(result.versions.count(), 4);
    QCOMPARE(result.versions["0.1.0"], QByteArray("75c42c94b3796a214faf5f17ea66f666ee362208"));
    QCOMPARE(result.versions["0.1.1"], QByteArray("31f86a2f83094b466f8a8d053e285dc3ff24000e"));
    QCOMPARE(result.versions["0.1.2"], QByteArray("fb14bedca6745c8ed56cd088dcbd739279d2d951"));
    QCOMPARE(result.branches.count(), 1);
    QCOMPARE(result.branches["master"], QByteArray("25ae77793e5b2d730b31224e2c713d4b1458746a"));
    QCOMPARE(result.tags.count(), 1);
    QCOMPARE(result.tags["bugfix"], QByteArray("75c42394b5796a214faf5f17ea66f666ee362208"));
    QCOMPARE(result.head, QString("master"));
    QVERIFY(result.isHeadBranch);
    QVERIFY(!result.isHeadTag);
    QVERIFY(!result.isHeadVersion);
}
