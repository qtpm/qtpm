#include <QString>
#include <QtTest>
#include "package2.h"

class TestPackage2 : public QObject {
    Q_OBJECT

public:
    TestPackage2();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testCase1();
};

TestPackage2::TestPackage2() {
}

void TestPackage2::initTestCase() {
    // This method is called before the first testfunction is executed.
}

void TestPackage2::cleanupTestCase() {
    // This method is called after the last testfunction was executed.
}

void TestPackage2::init() {
    // This method is called before each testfunction is executed.
}

void TestPackage2::cleanup() {
    // This method is called after every testfunction.
}

void TestPackage2::testCase1() {
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TestPackage2)

#include "package2_test.moc"
