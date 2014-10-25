#include <QString>
#include <QtTest>
#include "licensefiles.h"
#include "test_licensefiles.h"

void LicenseFilesTest::licenseListTest() {
    LicenseFiles files;
    QVERIFY(files.licenseKeys().contains("apache-v2.0"));
    QVERIFY(files.licenseKeys().contains("artistic-v2.0"));
    QVERIFY(files.licenseKeys().contains("bsd-2"));
    QVERIFY(files.licenseKeys().contains("bsd-3"));
    QVERIFY(files.licenseKeys().contains("epl-v1.0"));
    QVERIFY(files.licenseKeys().contains("gnu-agpl-v3.0"));
    QVERIFY(files.licenseKeys().contains("gnu-fdl-v1.3"));
    QVERIFY(files.licenseKeys().contains("gnu-gpl-v2.0"));
    QVERIFY(files.licenseKeys().contains("gnu-gpl-v3.0"));
    QVERIFY(files.licenseKeys().contains("gnu-lgpl-v2.1"));
    QVERIFY(files.licenseKeys().contains("gnu-lgpl-v3.0"));
    QVERIFY(files.licenseKeys().contains("mit"));
    QVERIFY(files.licenseKeys().contains("mpl-v2.0"));
    QVERIFY(files.licenseKeys().contains("unlicense"));
}

void LicenseFilesTest::licensePathTest()
{
    LicenseFiles files;
    files.setLicense("apache-v2.0");
    QCOMPARE(files.licensePath(), QString(":/rest-licenses/apache-v2.0.rst"));
    files.setLicense("original-v2.0");
    QCOMPARE(files.licensePath(), QString(""));
}

void LicenseFilesTest::licenseDisplayNameTest()
{
    LicenseFiles files;
    files.setLicense("apache-v2.0");
    QCOMPARE(files.displayName(), QString("Apache License v2.0"));
}

void LicenseFilesTest::licenseFindTest()
{
    LicenseFiles files;
    QCOMPARE(files.findKey("Apache License v2.0"), QString("apache-v2.0"));
}

void LicenseFilesTest::searchLicenseByAliasTest()
{
    LicenseFiles files;
    QVERIFY(files.findLicense("APACHE"));
    QCOMPARE(files.license(), QString("apache-v2.0"));
    QVERIFY(files.findLicense("Apache"));
    QCOMPARE(files.license(), QString("apache-v2.0"));
    QVERIFY(files.findLicense("Apache2"));
    QCOMPARE(files.license(), QString("apache-v2.0"));
    QVERIFY(files.findLicense("perl"));
    QCOMPARE(files.license(), QString("artistic-v2.0"));
    QVERIFY(files.findLicense("eclipse"));
    QCOMPARE(files.license(), QString("epl-v1.0"));
    QVERIFY(files.findLicense("mozilla"));
    QCOMPARE(files.license(), QString("mpl-v2.0"));
    QVERIFY(files.findLicense("gpl2"));
    QCOMPARE(files.license(), QString("gnu-gpl-v2.0"));
    QVERIFY(files.findLicense("x"));
    QCOMPARE(files.license(), QString("mit"));
    QVERIFY(files.findLicense("bsd"));
    QCOMPARE(files.license(), QString("bsd-3"));
    QVERIFY(files.findLicense("public"));
    QCOMPARE(files.license(), QString("unlicense"));
    QVERIFY(files.findLicense("public domain"));
    QCOMPARE(files.license(), QString("unlicense"));
}


