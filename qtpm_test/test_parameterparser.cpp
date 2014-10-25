#include <QString>
#include <QtTest>
#include "test_parameterparser.h"
#include "parameterparser.h"


void ParameterParserTest::commandListActionTest() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "--command-help"), ParameterParser::CommandListAction);
}

void ParameterParserTest::installActionTest1() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "install"), ParameterParser::InstallAction);
    QCOMPARE(parser.args().length(), 0);
}

void ParameterParserTest::installActionTest2() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "install" << "--bundle-only"), ParameterParser::InstallAction);
    QVERIFY(parser.flag("bundle-only"));
}

void ParameterParserTest::installActionTest3() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "install" << "--save" << "github.com/shibukawa/testproject"), ParameterParser::InstallAction);
    QVERIFY(parser.flag("save"));
    QCOMPARE(parser.args()[0], QString("github.com/shibukawa/testproject"));
}

void ParameterParserTest::installActionTest4() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "install" << "--save-dev" << "github.com/shibukawa/testproject"), ParameterParser::InstallAction);
    QVERIFY(parser.flag("save-dev"));
    QCOMPARE(parser.args()[0], QString("github.com/shibukawa/testproject"));
}

void ParameterParserTest::installActionTest5() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "--save" << "install" << "github.com/shibukawa/testproject"), ParameterParser::InstallAction);
    QVERIFY(parser.flag("save"));
    QCOMPARE(parser.args()[0], QString("github.com/shibukawa/testproject"));
}

void ParameterParserTest::installActionTest6() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "--update" << "install"), ParameterParser::InstallAction);
    QVERIFY(parser.flag("update"));
}

void ParameterParserTest::uninstallActionTest1()
{
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "uninstall" << "github.com/shibukawa/testproject"), ParameterParser::UninstallAction);
    QCOMPARE(parser.args()[0], QString("github.com/shibukawa/testproject"));
}

void ParameterParserTest::uninstallActionTest2()
{
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "uninstall" << "--remove" << "github.com/shibukawa/testproject"), ParameterParser::UninstallAction);
    QVERIFY(parser.flag("remove"));
    QCOMPARE(parser.args()[0], QString("github.com/shibukawa/testproject"));
}

void ParameterParserTest::updateActionTest1() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "update"), ParameterParser::UpdateAction);
    QCOMPARE(parser.args().length(), 0);
}

void ParameterParserTest::updateActionTest2() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "update" << "github.com/shibukawa/testproject"), ParameterParser::UpdateAction);
    QCOMPARE(parser.args()[0], QString("github.com/shibukawa/testproject"));
}

void ParameterParserTest::initAppActionTest1()
{
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "init"), ParameterParser::InitAppAction);
    QCOMPARE(parser.args().length(), 0);
}

void ParameterParserTest::initAppActionTest2()
{
    ParameterParser parser;
    auto cmd = QStringList() << "qtpm" << "init"
            << "--author" << "yoshiki shibukawa <yoshiki@shibu.jp> (http://www.shibu.jp)"
            << "--contributor" << "yoshiki shibukawa <yoshiki@shibu.jp> (http://www.shibu.jp)"
            << "--license" << "BSD"
            << "--description" << "web server"
            << "--homepage" << "https://github.com/qtpm/qtpm"
            << "--repository" << "https://github.com/qtpm/qtpm.git"
            << "--bugs" << "https://github.com/qtpm/qtpm/issues";
    QCOMPARE(parser.parse(cmd), ParameterParser::InitAppAction);
    QCOMPARE(parser.people("authors")[0].name, QString("yoshiki shibukawa"));
    QCOMPARE(parser.people("authors")[0].email, QString("yoshiki@shibu.jp"));
    QCOMPARE(parser.people("authors")[0].url, QString("http://www.shibu.jp"));
    QCOMPARE(parser.people("contributors")[0].name, QString("yoshiki shibukawa"));
    QCOMPARE(parser.param("license"), QString("BSD"));
    QCOMPARE(parser.param("homepage"), QString("https://github.com/qtpm/qtpm"));
    QCOMPARE(parser.param("repository"), QString("https://github.com/qtpm/qtpm.git"));
    QCOMPARE(parser.param("bugs"), QString("https://github.com/qtpm/qtpm/issues"));
}

void ParameterParserTest::initAppActionTest3()
{
    ParameterParser parser;
    auto cmd = QStringList() << "qtpm" << "init"
            << "--github" << "github.com/qtpm/qtpm";
    QCOMPARE(parser.parse(cmd), ParameterParser::InitAppAction);
    QCOMPARE(parser.param("homepage"), QString("https://github.com/qtpm/qtpm"));
    QCOMPARE(parser.param("repository"), QString("https://github.com/qtpm/qtpm.git"));
    QCOMPARE(parser.param("bugs"), QString("https://github.com/qtpm/qtpm/issues"));
}

void ParameterParserTest::initLibActionWithSrcTypeTest() {
    auto cmd = QStringList() << "qtpm" << "init-lib" << "openssl"
            << "--type" << "dev"
            << "--build" << "source"
            << "--author" << "yoshiki shibukawa <yoshiki@shibu.jp> (http://www.shibu.jp)"
            << "--committer" << "yoshiki shibukawa <yoshiki@shibu.jp> (http://www.shibu.jp)"
            << "--contributor" << "yoshiki shibukawa <yoshiki@shibu.jp> (http://www.shibu.jp)"
            << "--license=BSD"
            << "--description" << "web server"
            << "--homepage" << "https://github.com/qtpm/qtpm"
            << "--repository" << "https://github.com/qtpm/qtpm.git"
            << "--bugs" << "https://github.com/qtpm/qtpm/issues";

    ParameterParser parser;
    QCOMPARE(parser.parse(cmd), ParameterParser::InitLibAction);
    QCOMPARE(parser.args()[0], QString("openssl"));
    QCOMPARE(parser.param("type"), QString("dev"));
    QCOMPARE(parser.param("buildType"), QString("source"));
    QCOMPARE(parser.people("authors")[0].name, QString("yoshiki shibukawa"));
    QCOMPARE(parser.people("authors")[0].email, QString("yoshiki@shibu.jp"));
    QCOMPARE(parser.people("authors")[0].url, QString("http://www.shibu.jp"));
    QCOMPARE(parser.people("committers")[0].name, QString("yoshiki shibukawa"));
    QCOMPARE(parser.people("contributors")[0].name, QString("yoshiki shibukawa"));
    QCOMPARE(parser.param("license"), QString("BSD"));
    QCOMPARE(parser.param("homepage"), QString("https://github.com/qtpm/qtpm"));
    QCOMPARE(parser.param("repository"), QString("https://github.com/qtpm/qtpm.git"));
    QCOMPARE(parser.param("bugs"), QString("https://github.com/qtpm/qtpm/issues"));
}


void ParameterParserTest::initLibActionWithoutTypeTest() {
    ParameterParser parser;
    QCOMPARE(parser.parse(QStringList() << "qtpm" << "init-lib" << "openssl"), ParameterParser::InitLibAction);
    QCOMPARE(parser.args()[0], QString("openssl"));
}

void ParameterParserTest::initLibActionWithDevTypeTest() {
    auto cmd = QStringList() << "qtpm" << "init-lib" << "openssl"
            << "--build=configure" << "--build-option=\"--prefix=/tmp --enable-static\"";
    ParameterParser parser;
    QCOMPARE(parser.parse(cmd), ParameterParser::InitLibAction);
    QCOMPARE(parser.args()[0], QString("openssl"));
    QCOMPARE(parser.param("buildType"), QString("configure"));
    QCOMPARE(parser.param("buildOption"), QString("\"--prefix=/tmp --enable-static\""));
}

void ParameterParserTest::buildTest()
{
    auto cmd = QStringList() << "qtpm" << "build" << "--build-option=\"--prefix=/tmp --enable-static\"" << "--save";
    ParameterParser parser;
    QCOMPARE(parser.parse(cmd), ParameterParser::BuildAction);
    QCOMPARE(parser.param("buildOption"), QString("\"--prefix=/tmp --enable-static\""));
    QVERIFY(parser.flag("save"));
}
//#include "tst_qtpm_parameterparser.moc"

