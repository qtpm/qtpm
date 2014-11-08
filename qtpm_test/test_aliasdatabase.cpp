#include "test_aliasdatabase.h"
#include "aliasdatabase.h"
#include "testutil.h"
#include <QTest>

AliasDatabaseTest::AliasDatabaseTest(QObject *parent) :
    QObject(parent)
{
}

void AliasDatabaseTest::parse_wiki_contents_test()
{
    TestUtil util;
    auto dir = util.makeTestDir();

    QString contents("* `zlib <https://github.com/qtpm/zlib.git>`_ A massively spiffy yet delicately unobtrusive compression library.");

    AliasDatabase database(dir);
    database.refresh(contents);
    QCOMPARE(database.count(), 1);
    QVERIFY(database.contains("zlib"));
    auto entry = database.find("zlib");
    QCOMPARE(entry.name(), QString("zlib"));
    QCOMPARE(entry.description(), QString("A massively spiffy yet delicately unobtrusive compression library."));
    QCOMPARE(entry.longName(), QString("github.com/qtpm/zlib"));
}

void AliasDatabaseTest::parse_wiki_contents_with_ssh_url_test()
{
    TestUtil util;
    auto dir = util.makeTestDir();

    QString contents("* `zlib <git@github.com:qtpm/zlib.git>`_ A massively spiffy yet delicately unobtrusive compression library.");

    AliasDatabase database(dir);
    database.refresh(contents);
    QCOMPARE(database.count(), 1);
    QVERIFY(database.contains("zlib"));
    auto entry = database.find("zlib");
    QCOMPARE(entry.name(), QString("zlib"));
    QCOMPARE(entry.description(), QString("A massively spiffy yet delicately unobtrusive compression library."));
    QCOMPARE(entry.longName(), QString("github.com/qtpm/zlib"));
}

void AliasDatabaseTest::read_write_test()
{
    TestUtil util;
    auto dir = util.makeTestDir();

    QString contents("* `zlib <git@github.com:qtpm/zlib.git>`_ A massively spiffy yet delicately unobtrusive compression library.");

    auto database = new AliasDatabase(dir);
    database->refresh(contents);
    delete database;

    // load from dist
    AliasDatabase newDatabase(dir);

    QCOMPARE(newDatabase.count(), 1);
    QVERIFY(newDatabase.contains("zlib"));
    auto entry = newDatabase.find("zlib");
    QCOMPARE(entry.name(), QString("zlib"));
    QCOMPARE(entry.description(), QString("A massively spiffy yet delicately unobtrusive compression library."));
    QCOMPARE(entry.longName(), QString("github.com/qtpm/zlib"));
}

void AliasDatabaseTest::search_test()
{
    TestUtil util;
    auto dir = util.makeTestDir();

    QString contents("* `zlib <git@github.com:qtpm/zlib.git>`_ A massively spiffy yet delicately unobtrusive compression library.\n"
                     "* `SQLite <git@github.com:qtpm/sqlite.git>`_ SQLite is a software library that implements a self-contained, serverless, zero-configuration, transactional SQL database engine.\n"
                     "* `hiredis <git@github.com:qtpm/hiredis.git>`_ Hiredis is a minimalistic C client library for the Redis database.\n");

    AliasDatabase database(dir);
    database.refresh(contents);

    QCOMPARE(database.count(), 3);

    auto result1 = database.search("library"); // default search checks title, url, description
    QCOMPARE(result1.count(), 3);

    auto result2 = database.search("github.com"); // default search checks title, url, description
    QCOMPARE(result2.count(), 3);

    auto result3 = database.search("redis"); // default search checks title, url, description
    QCOMPARE(result3.count(), 1);
}

void AliasDatabaseTest::search_near_title_test()
{
    TestUtil util;
    auto dir = util.makeTestDir();

    QString contents("* `zlib <git@github.com:qtpm/zlib.git>`_ A massively spiffy yet delicately unobtrusive compression library.\n"
                     "* `SQLite <git@github.com:qtpm/sqlite.git>`_ SQLite is a software library that implements a self-contained, serverless, zero-configuration, transactional SQL database engine.\n"
                     "* `hiredis <git@github.com:qtpm/hiredis.git>`_ Hiredis is a minimalistic C client library for the Redis database.\n");

    AliasDatabase database(dir);
    database.refresh(contents);

    QCOMPARE(database.count(), 3);

    auto result = database.search_near_title("xlib"); // search near title
    QCOMPARE(result.count(), 1);
    QCOMPARE(result[0].name(), QString("zlib"));
}
