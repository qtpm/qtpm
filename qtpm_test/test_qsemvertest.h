#ifndef TST_QSEMVERTEST_H
#define TST_QSEMVERTEST_H

#include <QObject>

class QSemverTest : public QObject
{
    Q_OBJECT

public:
    QSemverTest();

private Q_SLOTS:
    void filterTest();
    void filterTest2();
    void filterTest3();
    void filterTest4();
    void filterTest5();
    void filterTest6();
    void filterTest7();
    void filterTest8();
    void filterTest9();
    void filterWithReverseTest();
    void filterWithMultipleRangesTest();
    void filterWithMultipleRangesWithReverseTest();
};

#endif // TST_QSEMVERTEST_H
