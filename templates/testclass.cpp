#include <QString>
#include <QtTest>
#include "[[.TargetSmall]].h"

class Test[[.Target]] : public QObject {
    Q_OBJECT

public:
    Test[[.Target]]();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testCase1();
};

Test[[.Target]]::Test[[.Target]]() {
}

void Test[[.Target]]::initTestCase() {
    // This method is called before the first testfunction is executed.
}

void Test[[.Target]]::cleanupTestCase() {
    // This method is called after the last testfunction was executed.
}

void Test[[.Target]]::init() {
    // This method is called before each testfunction is executed.
}

void Test[[.Target]]::cleanup() {
    // This method is called after every testfunction.
}

void Test[[.Target]]::testCase1() {
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(Test[[.Target]])

#include "[[.TargetSmall]]_test.moc"
