#ifndef TEST_C3LINEARIZATION_H
#define TEST_C3LINEARIZATION_H

#include <QObject>

class C3LinearizationTest : public QObject
{
    Q_OBJECT
public:
    explicit C3LinearizationTest(QObject *parent = 0);

private Q_SLOTS:
    void linearizeTest_1();
    void linearizeTest_2();
    void linearizeTest_3();
    void linearizeTest_4();

    void linearizeErrorTest_1();
    void linearizeErrorTest_2();
};

#endif // TEST_C3LINEARIZATION_H
