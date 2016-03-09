#include <QString>
#include <QtTest>
#include "package1.h"

class TestPackage1 : public QObject {
    Q_OBJECT

public:
    TestPackage1();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testCase1();
};

TestPackage1::TestPackage1() {
}

void TestPackage1::initTestCase() {
    // This method is called before the first testfunction is executed.
}

void TestPackage1::cleanupTestCase() {
    // This method is called after the last testfunction was executed.
}

void TestPackage1::init() {
    // This method is called before each testfunction is executed.
}

void TestPackage1::cleanup() {
    // This method is called after every testfunction.
}

void TestPackage1::testCase1() {
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(TestPackage1)

#include "package1_test.moc"
