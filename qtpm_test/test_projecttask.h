#ifndef TEST_PROJECTTASK_H
#define TEST_PROJECTTASK_H

#include <QObject>
#include <QDir>
class TestUtil;

class ProjectTaskTest : public QObject
{
    Q_OBJECT
public:
    explicit ProjectTaskTest(QObject *parent = 0);

private Q_SLOTS:
    void init();
    void parseFolderTest();
    void dumpInfoTest();
    void writeInfoTest();
    void modifyRootProjectTest();
    void cleanup();
private:
    TestUtil* _util;
    QDir _workdir;
};

#endif // TEST_PROJECTTASK_H
