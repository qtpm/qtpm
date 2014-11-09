#ifndef TEST_CLEANACTION_H
#define TEST_CLEANACTION_H

#include <QObject>
#include <QDir>
class TestUtil;

class CleanActionTest : public QObject
{
    Q_OBJECT
public:
    explicit CleanActionTest(QObject *parent = 0);

private Q_SLOTS:
    void init();
    void cleanTest();
    void cleanup();

private:
    TestUtil* _util;
    QDir _workdir;
};

#endif // TEST_CLEANACTION_H
