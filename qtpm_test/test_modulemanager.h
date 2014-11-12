#ifndef TEST_DEPENDENCYRESOLVER_H
#define TEST_DEPENDENCYRESOLVER_H

#include <QObject>

class ModuleManagerTest : public QObject
{
    Q_OBJECT

public:
    ModuleManagerTest();

private Q_SLOTS:
    void initialDepthTest();
    void secondDepthTest1();
    void secondDepthTest2();

    void initialDepthBranchTest();
    void secondDepthBranchTest1();
    void secondDepthBranchTest2();
    void conflictRemoteModuleTest1();
    void conflictRemoteModuleTest2();

    void versionConflictAtFirstDepthTest();
    void versionConflictAtSecondDepthTest1();
    void versionConflictAtSecondDepthTest2();

    void useInstalledModuleTest1();
    void useInstalledModuleTest2();
    void useLocalModuleTest1();
    void useLocalModuleTest2();

    void aliasTest1();
    void aliasTest2();
    void aliasTest3();
};

#endif // TEST_DEPENDENCYRESOLVER_H
