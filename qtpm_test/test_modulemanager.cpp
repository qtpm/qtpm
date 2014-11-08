#include <QtTest>
#include "test_modulemanager.h"
#include "modulemanager.h"
#include "module.h"
#include <QScopedPointer>
#include <QDebug>

ModuleManagerTest::ModuleManagerTest()
{
}

void ModuleManagerTest::initialDepthTest()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    // first round
    manager->_addRemoteModuleDependent("app", "lib1", "~1.0.0");
    auto availableVersions = QStringList() << "1.0.1" << "1.1.1";
    manager->setAvailableVersions("lib1", availableVersions);
    auto nextModules = manager->shift(); // it returns first modules
    QCOMPARE(nextModules.length(), 1); // lib1
    QCOMPARE(nextModules[0]->name(), QString("lib1"));
    QCOMPARE(nextModules[0]->finalVersion(), QString("1.0.1"));
    QCOMPARE(manager->versionErrorModules().size(), 0);
}

void ModuleManagerTest::secondDepthTest1()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    // first depth
    manager->_addRemoteModuleDependent("app", "lib1", "~1.0.0");
    auto availableVersions1 = QStringList() << "1.0.1" << "1.1.1";
    manager->setAvailableVersions("lib1", availableVersions1);
    manager->shift(); // it returns first modules

    // second depth
    manager->_addRemoteModuleDependent("lib1", "lib2", "~2.0.0");
    auto availableVersions2 = QStringList() << "1.5.1" << "2.0.1";
    manager->setAvailableVersions("lib2", availableVersions2);
    auto nextModules = manager->shift(); // it returns second depth modules
    QCOMPARE(nextModules.length(), 1); // lib2
    QCOMPARE(nextModules[0]->name(), QString("lib2"));
    QCOMPARE(nextModules[0]->finalVersion(), QString("2.0.1"));
    QCOMPARE(manager->versionErrorModules().size(), 0);
}

void ModuleManagerTest::secondDepthTest2()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    // first depth
    manager->_addRemoteModuleDependent("app", "lib1", "~1.0.0");
    auto availableVersions1 = QStringList() << "1.0.1" << "1.1.1";
    manager->setAvailableVersions("lib1", availableVersions1);

    manager->_addRemoteModuleDependent("app", "lib2", "~2.0.0");
    auto availableVersions2 = QStringList() << "1.5.1" << "2.0.1";
    manager->setAvailableVersions("lib2", availableVersions2);

    manager->shift(); // it returns first modules

    // second depth
    // app and lib1 uses same lib2 version
    manager->_addRemoteModuleDependent("lib1", "lib2", "~2.0.0");
    manager->setAvailableVersions("lib2", availableVersions2);

    auto nextModules = manager->shift(); // it returns second depth modules

    QCOMPARE(nextModules.length(), 0); // lib2 is already processed
    QCOMPARE(manager->versionErrorModules().size(), 0);
}

void ModuleManagerTest::versionConflictAtFirstDepthTest()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());
    manager->_addRemoteModuleDependent("app", "lib1", "~2.0.0");
    auto availableVersions = QStringList() << "1.0.1" << "1.1.1"; // 2.0.x is not available
    manager->setAvailableVersions("lib1", availableVersions);
    auto nextModules = manager->shift(); // it returns first modules
    QCOMPARE(nextModules.length(), 0); // lib1 is not in next module
    QCOMPARE(manager->versionErrorModules().size(), 1);
    QCOMPARE(manager->versionErrorModules()[0]->name(), QString("lib1"));
}

void ModuleManagerTest::versionConflictAtSecondDepthTest1()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    // first depth
    manager->_addRemoteModuleDependent("app", "lib1", "~1.0.0");
    auto availableVersions1 = QStringList() << "1.0.1" << "1.1.1";
    manager->setAvailableVersions("lib1", availableVersions1);
    manager->shift(); // it returns first modules

    // second depth
    // target version is not available
    manager->_addRemoteModuleDependent("lib1", "lib2", "~3.0.0");
    auto availableVersions2 = QStringList() << "1.5.1" << "2.0.1";
    manager->setAvailableVersions("lib2", availableVersions2);
    auto nextModules = manager->shift(); // it returns second depth modules
    QCOMPARE(nextModules.length(), 0); // lib2
    QCOMPARE(manager->versionErrorModules().size(), 1);
    QCOMPARE(manager->versionErrorModules()[0]->name(), QString("lib2"));
}

void ModuleManagerTest::versionConflictAtSecondDepthTest2()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    // first depth
    manager->_addRemoteModuleDependent("app", "lib1", "~1.0.0");
    auto availableVersions1 = QStringList() << "1.0.1" << "1.1.1";
    manager->setAvailableVersions("lib1", availableVersions1);

    manager->_addRemoteModuleDependent("app", "lib2", "~2.0.0");
    auto availableVersions2 = QStringList() << "1.5.1" << "2.0.1";
    manager->setAvailableVersions("lib2", availableVersions2);

    manager->shift(); // it returns first modules

    // second depth
    // app and lib1 uses same lib2 version
    manager->_addRemoteModuleDependent("lib1", "lib2", "~1.0.0");
    manager->setAvailableVersions("lib2", availableVersions2);

    auto nextModules = manager->shift(); // it returns second depth modules

    QCOMPARE(nextModules.length(), 0); // lib2 is already processed
    QCOMPARE(manager->versionErrorModules().size(), 1);
    QCOMPARE(manager->versionErrorModules()[0]->name(), QString("lib2"));
}

void ModuleManagerTest::useInstalledModuleTest1()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    manager->_addInstalledModule("lib1", "1.0.0");
    auto availableVersions = QStringList() << "1.0.0" << "2.0.1";
    manager->_addRemoteModuleDependent("app", "lib1", "~1.0.0");
    manager->setAvailableVersions("lib1", availableVersions);

    auto nextModules = manager->shift();

    QCOMPARE(nextModules.length(), 0);
}

void ModuleManagerTest::useInstalledModuleTest2()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    manager->_addInstalledModule("lib1", "1.0.0");
    auto availableVersions = QStringList() << "1.0.0" << "2.0.1";
    manager->_addRemoteModuleDependent("app", "lib1", "~2.0.0");
    manager->setAvailableVersions("lib1", availableVersions);

    auto nextModules = manager->shift();

    QCOMPARE(nextModules.length(), 1);
    QCOMPARE(nextModules[0]->name(), QString("lib1"));
    QCOMPARE(nextModules[0]->finalVersion(), QString("2.0.1"));
}

/**
 * @brief local path module has higher priority than regular module
 */
void ModuleManagerTest::useLocalModuleTest1()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    manager->_addLocalModuleDependent("app", "lib1", "/home/my/lib1.tgz", Module::LocalFileModule);
    manager->_addRemoteModuleDependent("app", "lib1", "~1.0.0");
    auto availableVersions = QStringList() << "1.0.0" << "2.0.1";
    manager->setAvailableVersions("lib1", availableVersions);

    auto nextModules = manager->shift();

    QCOMPARE(nextModules.length(), 1);
    QCOMPARE(nextModules[0]->status(), Module::LocalFileModule);
    QCOMPARE(nextModules[0]->name(), QString("lib1"));
    QCOMPARE(nextModules[0]->longPath(), QString("/home/my/lib1.tgz"));
}

/**
 * @brief two or more local modules should have same path if it has same name.
 */
void ModuleManagerTest::useLocalModuleTest2()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    manager->_addLocalModuleDependent("app", "lib1", "/home/my/lib1.tgz", Module::LocalFileModule);
    manager->_addLocalModuleDependent("app", "lib1", "/home/my/Downloads/lib1.tgz", Module::LocalFileModule);

    manager->shift();

    QCOMPARE(manager->versionErrorModules().size(), 1);
}

void ModuleManagerTest::aliasTest1()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());
    auto module = new Module("lib1");
    manager->_addModule(module);
    manager->_addAlias("app", "lib1", "lib1", Module::IndexedModule);
    manager->_addAlias("app", "lib1", "github.com/shibukawa/lib1", Module::RemoteModule);
    manager->_addAlias("app", "lib1", "/home/my/lib1.tgz", Module::LocalFileModule);
    manager->checkAliasConflict();
    QVERIFY(manager->isInstalled("lib1"));
    QVERIFY(manager->isInstalled("/home/my/lib1.tgz"));
    QCOMPARE(manager->warningMessages().length(), 0);
}

void ModuleManagerTest::aliasTest2()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    auto module = new Module("lib1");
    module->setStatus(Module::LocalFileModule);
    manager->_addModule(module);
    manager->_addAlias("app", "lib1", "/home/my/lib1.tgz", Module::LocalFileModule);
    manager->_addAlias("app", "lib1", "/home/my/Downloads/lib1.tgz", Module::LocalFileModule);
    manager->checkAliasConflict();
    QVERIFY(manager->isInstalled("lib1"));
    QVERIFY(manager->isInstalled("/home/my/lib1.tgz"));
    QCOMPARE(manager->warningMessages().length(), 1);
}

void ModuleManagerTest::aliasTest3()
{
    QScopedPointer<ModuleManager> manager(new ModuleManager());

    auto module = new Module("lib1");
    module->setStatus(Module::RemoteModule);
    manager->_addModule(module);
    manager->_addAlias("app", "lib1", "github.com/shibukawa/lib1", Module::RemoteModule);
    manager->_addAlias("app", "lib1", "github.com/shibu/lib1", Module::RemoteModule);
    manager->checkAliasConflict();
    QVERIFY(manager->isInstalled("lib1"));
    QVERIFY(manager->isInstalled("github.com/shibukawa/lib1"));
    QCOMPARE(manager->warningMessages().length(), 1);
}
