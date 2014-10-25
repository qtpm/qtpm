#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include <QObject>

class ModuleTest : public QObject
{
    Q_OBJECT

public:
    ModuleTest();

private Q_SLOTS:
    void successfulCaseTest();
    void invalidCaseTest();
};

#endif // TEST_MODULE_H
