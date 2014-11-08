#include <QtTest>
#include "test_module.h"
#include "module.h"

ModuleTest::ModuleTest()
{
}

void ModuleTest::successfulCaseTest()
{
    auto module = new Module("lib");
    QStringList versions;
    versions << "1.0.1" << "1.1.1" << "1.1.2";
    module->setAvailableVersions(versions);
    module->addDependent("app", "~1.1.0");
    QVERIFY(module->findValidVersion());
    QVERIFY(module->valid());
    QCOMPARE(module->finalVersion(), QString("1.1.2"));
    delete module;
}

void ModuleTest::invalidCaseTest()
{
    auto module = new Module("lib");
    QStringList versions;
    versions << "1.0.1" << "1.1.1" << "1.1.2";
    module->setAvailableVersions(versions);
    module->addDependent("app", "~1.2.0");
    QVERIFY(!module->findValidVersion());
    QVERIFY(!module->valid());
    delete module;
}

void ModuleTest::localModuleCaseTest()
{
    auto module = new Module("lib");
    module->setStatus(Module::LocalFileModule);
    QVERIFY(module->findValidVersion());
    QVERIFY(module->valid());
    delete module;
}
