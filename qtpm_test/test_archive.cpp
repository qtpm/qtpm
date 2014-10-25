#include "test_archive.h"
#include "testutil.h"
#include "qarchive.h"
#include <QTest>
#include <QDebug>

ArchiveTest::ArchiveTest(QObject *parent) :
    QObject(parent)
{
}

void ArchiveTest::extract_tar()
{
    TestUtil util;
    auto workdir = util.makeTestDir();

    QVERIFY(QArchive::extract(":/resource/sample.tar", workdir));
    QVERIFY(workdir.exists("sample/file1.txt"));
}

void ArchiveTest::extract_tar_gz()
{
    TestUtil util;
    auto workdir = util.makeTestDir();

    QVERIFY(QArchive::extract(":/resource/sample.tar.gz", workdir));
    QVERIFY(workdir.exists("sample/file1.txt"));
}

void ArchiveTest::extract_tar_bz2()
{
    TestUtil util;
    auto workdir = util.makeTestDir();

    QVERIFY(QArchive::extract(":/resource/sample.tar.bz2", workdir));
    QVERIFY(workdir.exists("sample/file1.txt"));
}

void ArchiveTest::extract_tar_xz()
{
    TestUtil util;
    auto workdir = util.makeTestDir();

    QVERIFY(QArchive::extract(":/resource/sample.tar.xz", workdir));
    QVERIFY(workdir.exists("sample/file1.txt"));
}

void ArchiveTest::extract_tar_lzma()
{
    TestUtil util;
    auto workdir = util.makeTestDir();

    QVERIFY(QArchive::extract(":/resource/sample.tar.lzma", workdir));
    QVERIFY(workdir.exists("sample/file1.txt"));
}

void ArchiveTest::extract_zip()
{
    TestUtil util;
    auto workdir = util.makeTestDir();

    QVERIFY(QArchive::extract(":/resource/sample.zip", workdir));
    QVERIFY(workdir.exists("sample/file1.txt"));
}
