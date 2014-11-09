#ifndef TST_QPM_PARAMETERPARSER_H
#define TST_QPM_PARAMETERPARSER_H

#include <QObject>

class ParameterParserTest : public QObject
{
    Q_OBJECT

public:
    ParameterParserTest() {

    }

private Q_SLOTS:
    void commandListActionTest();
    void installActionTest1();
    void installActionTest2();
    void installActionTest3();
    void installActionTest4();
    void installActionTest5();
    void installActionTest6();
    void installActionTest7();

    void uninstallActionTest1();
    void uninstallActionTest2();

    void updateActionTest1();
    void updateActionTest2();

    void initAppActionTest1();
    void initAppActionTest2();
    void initAppActionTest3();

    void initLibActionWithSrcTypeTest();
    void initLibActionWithoutTypeTest();
    void initLibActionWithDevTypeTest();

    void refreshActionTest();

    void searchActionTest1();
    void searchActionTest2();

    void configActionTest1();
    void configActionTest2();

    void cleanTest();

    void buildTest();
};

#endif // TST_QPM_PARAMETERPARSER_H
