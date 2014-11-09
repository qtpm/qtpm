// http://stackoverflow.com/questions/12194256/qt-how-to-organize-unit-test-with-more-than-one-class

#include <QtTest>

#include "test_parameterparser.h"
#include "test_licensefiles.h"
#include "test_qpmpackage.h"
#include "test_qsemvertest.h"
#include "test_module.h"
#include "test_modulemanager.h"
#include "test_c3linearization.h"
#include "test_resulthash.h"
#include "test_gitresultparser.h"
#include "test_archive.h"
#include "test_buildoption.h"
#include "test_aliasdatabase.h"
#include "test_projecttask.h"
#include "test_platformdatabase.h"
#include "test_cleanaction.h"


int main(int argc, char** argv)
{
    int status = 0;
    auto ASSERT_TEST = [&status, argc, argv](QObject* obj) {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    ASSERT_TEST(new ParameterParserTest());
    ASSERT_TEST(new LicenseFilesTest());
    ASSERT_TEST(new QSemverTest());
    ASSERT_TEST(new ModuleTest());
    ASSERT_TEST(new ModuleManagerTest());
    ASSERT_TEST(new C3LinearizationTest());
    ASSERT_TEST(new ResultHashTest());
    ASSERT_TEST(new GitResultParserTest());
    ASSERT_TEST(new ArchiveTest());
    ASSERT_TEST(new QPMPackageTest());
    ASSERT_TEST(new BuildOptionTest());
    ASSERT_TEST(new AliasDatabaseTest());
    ASSERT_TEST(new ProjectTaskTest());
    ASSERT_TEST(new PlatformDatabaseTest());
    ASSERT_TEST(new CleanActionTest());
    return status;
}
