#include <QString>
#include <QtTest>
#include "[[.Dir]][[.ClassNameSmall]].h"

class Test[[.ClassName]] : public QObject {
    Q_OBJECT

public:
    Test[[.ClassName]]() {
    }

private Q_SLOTS:
    void initTestCase() {
        // This method is called before the first testfunction is executed.
    }
    void cleanupTestCase() {
        // This method is called after the last testfunction was executed.
    }
    void init() {
        // This method is called before each testfunction is executed.
    }
    void cleanup() {
        // This method is called after every testfunction.
    }

    void testCase1() {
        QVERIFY(true);
        QVERIFY2(true, "Failure");
        QCOMPARE(1, 1);
    }
};

QTEST_APPLESS_MAIN(Test[[.ClassName]])

#include "[[.ClassNameSmall]]_test.moc"
