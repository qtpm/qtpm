#include "buildtask.h"
#include "qpmpackage.h"
#include "parameterparser.h"
#include <iostream>
#include "externalprocess.h"
#include "platformdatabase.h"
#include <QDebug>
#include <QScopedPointer>


BuildTask::BuildTask(const QDir& package, bool verbose, ParameterParser *param, PlatformDatabase* database, QDir* destination, QObject *parent) :
    QObject(parent), _packageDir(package), _destinationDir(destination), _param(param), _verbose(verbose), _database(database)
{
}

enum ParseStatus {
    SkippingSpace,
    ParsingOption,
    ParsingQuotedOption
};

QStringList BuildTask::parseBuildOption(const QString &option)
{
    QStringList result;

    ParseStatus status = SkippingSpace;
    QChar quote('"');
    QChar escape('\\');
    int i = 0;
    int sectionStart = 0;
    int end = option.length();

    while (i < end) {
        QChar c = option.at(i);
        switch (status) {
        case SkippingSpace:
            if (c.isSpace()) {
                // do nothing
            } else if (c == quote) {
                status = ParsingQuotedOption;
                sectionStart = i;
            } else {
                status = ParsingOption;
                sectionStart = i;
            }
            break;
        case ParsingOption:
            if (c.isSpace()) {
                result.append(option.mid(sectionStart, i - sectionStart));
                status = SkippingSpace;
            }
            break;
        case ParsingQuotedOption:
            if ((i > sectionStart + 1) && c.isSpace()) {
                if ((option.at(i - 1) == quote) && (option.at(i - 2) != escape)) {
                    result.append(option.mid(sectionStart, i - sectionStart));
                    status = SkippingSpace;
                }
            }
            break;
        }
        i++;
    }

    if (status != SkippingSpace) {
        result.append(option.mid(sectionStart, i - sectionStart));
    }

    return result;
}

void BuildTask::run()
{
    if (this->_verbose) {
        std::cout << "start building " << _packageDir.path().toStdString() << std::endl;
    }
    auto app = QCoreApplication::instance();
    auto param = this->_param;

    PlatformDatabase database(this);
    if (!this->_database) {
        this->_database = &database;
        QString qtpathParam = this->_param->param("qtdir");
        if (qtpathParam.isEmpty()) {
            if (!database.resolveQtPath()) {
                std::cerr << "Can't find qt directory. Use --qtdir option or set QTDIR by using qtpm config subcommand." << std::endl;
                if (app) {
                    app->exit(1);
                }
            }
        } else {
            if (!database.setQtPath(qtpathParam, false)) {
                std::cerr << "Specified qt directory is wrong. <qtdir>/bin/qmake should exist." << std::endl;
                if (app) {
                    app->exit(1);
                }
            }
        }
    }
    QDir installDir = this->_database->installDirectory();
    if (!this->_destinationDir) {
        this->_destinationDir = &installDir;
    }

    if (!QpmPackage::hasQtPackageIni(this->_packageDir)) {
        std::cerr << "qtpackage.ini doesn't exist in " << this->_packageDir.path().toStdString() << std::endl;
        app->exit(1);
        return;
    }

    QScopedPointer<QpmPackage> package(new QpmPackage(this->_packageDir));
    QDir srcDir = QDir(this->_packageDir.filePath("src"));

    if (!srcDir.exists()) {
        std::cerr << "src folder doesn't exists." << std::endl;
        app->exit(1);
        return;
    }

    QString buildType;

    if (package->packageType() == "app") {
        std::cerr << "build command works for library, but this package is application" << std::endl;
        app->exit(1);
        return;
    }

    if (!param || param->param("buildType").isEmpty()) {
        buildType = package->buildType();
    } else {
        buildType = param->param("buildType");
    }

    if (buildType == "source") {
        std::cerr << "build command do nothing when --build-type is source" << std::endl;
        app->exit(1);
        return;
    }

    QString buildOptionString;

    if (!param || param->param("buildOption").isEmpty()) {
        buildOptionString = package->buildOption();
    } else {
        buildOptionString = param->param("buildOption");
    }

    bool success = false;
    if (buildType == "configure") {
        success = this->_buildByConfigure(srcDir, buildOptionString);
    } else if (buildType == "cmake") {
        success = this->_buildByCMake(package->name(), srcDir, buildOptionString);
    }
    if (success) {
        this->_copyPriFile(package.data());
        if (param && param->flag("save")) {
            if (!param->param("buildOption").isEmpty()) {
                package->setBuildOption(param->param("buildOption"));
            }
            if (!param->param("buildType").isEmpty()) {
                package->setBuildType(param->param("buildType"));
            }
        }
        app->exit(0);
    } else {
        app->exit(1);
    }
}

bool BuildTask::_buildByCMake(const QString& name, const QDir& srcDir, const QString& buildOptionString)
{
    if (this->_verbose) {
        std::cout << "  use CMake" << std::endl;
    }
    ExternalProcess cmakeProcess(true, false, this);
    QString osName = this->_database->currentPlatform();
    QString makeCommand;
    QStringList arguments;
    if (osName.startsWith("win") && osName != "win32-g++" && osName != "mingw" && osName != "cygwin") {
        arguments << "-G" << "NMake Makefiles";
        makeCommand = "nmake";
    } else {
        arguments << "-G" << "Unix Makefiles";
        makeCommand = "make";
    }
    arguments << QString("-DCMAKE_INSTALL_PREFIX:PATH=%1").arg(this->_destinationDir->path())
              << this->parseBuildOption(buildOptionString)
              << srcDir.absolutePath();
    ;
    std::cout << "cmake " << arguments.join(" ").toStdString() << std::endl;
    QDir buildDir = QDir(QDir::current().filePath("build"));
    QDir::current().mkpath("build");
    QDir buildPackageDir = buildDir.filePath(name);
    buildDir.mkdir(name);

    cmakeProcess.run("cmake", arguments, buildPackageDir);
    cmakeProcess.waitForFinished(-1);
    if (cmakeProcess.exitCode() != 0) {
        return false;
    }

    ExternalProcess makeProcess(true, false, this);
    QStringList marguments;
    std::cout << makeCommand.toStdString() << std::endl;
    makeProcess.run(makeCommand, marguments, buildPackageDir);
    makeProcess.waitForFinished(-1);
    if (makeProcess.exitCode() != 0) {
        return false;
    }

    ExternalProcess makeInstallProcess(true, false, this);
    QStringList marguments2;
    marguments2 << "install";
    std::cout << makeCommand.toStdString() << " install" << std::endl;
    makeInstallProcess.run(makeCommand, marguments2, buildPackageDir);
    makeInstallProcess.waitForFinished(-1);
    if (makeInstallProcess.exitCode() != 0) {
        return false;
    }
    return true;
}

bool BuildTask::_buildByConfigure(const QDir& srcDir, const QString& buildOptionString)
{
    if (this->_verbose) {
        std::cout << "  use configure" << std::endl;
    }
    QString makeCommand;
    ExternalProcess configureProcess(true, this);
    QStringList arguments;
#ifdef Q_OS_WIN32
    makeCommand = "nmake";
#else
    makeCommand = "make";
#endif
    arguments << QString("--prefix=%1").arg(this->_destinationDir->path())
              << this->parseBuildOption(buildOptionString);
    std::cout << "./configure " << arguments.join(" ").toStdString() << std::endl;
    configureProcess.run(srcDir.filePath("configure"), arguments, srcDir);
    configureProcess.waitForFinished(-1);
    if (configureProcess.exitCode() != 0) {
        return false;
    }

    ExternalProcess makeProcess(true, this);
    QStringList marguments;
    std::cout << makeCommand.toStdString() << std::endl;
    makeProcess.run(makeCommand, marguments, srcDir);
    makeProcess.waitForFinished(-1);
    if (makeProcess.exitCode() != 0) {
        return false;
    }

    ExternalProcess makeInstallProcess(true, this);
    QStringList marguments2;
    marguments2 << "install";
    std::cout << makeCommand.toStdString() << " install" << std::endl;
    makeInstallProcess.run(makeCommand, marguments2, srcDir);
    makeInstallProcess.waitForFinished(-1);
    if (makeInstallProcess.exitCode() != 0) {
        return false;
    }
    return true;
}

void BuildTask::_copyPriFile(QpmPackage* package)
{
    QFileInfo bestMatch;
    QFileInfo secondMatch;
    QStringList matchPlatforms;
    QString osName = this->_database->currentPlatform();
    if (this->_database) {
        matchPlatforms = this->_database->searchRelatedPlatforms(osName);
    }
    auto prifiles = this->_packageDir.entryInfoList(QStringList() << "*.pri", QDir::Files, QDir::Name);
    for (const QFileInfo& priinfo : prifiles) {
        // todo fix scoring logic
        auto baseName = priinfo.baseName();
        if (osName == baseName) {
            bestMatch = priinfo;
        } else if (matchPlatforms.contains(baseName)) {
            secondMatch = priinfo;
        }
    }

    QString priName = QString("%1.pri").arg(package->name());
    if (bestMatch.isFile()) {
        QFile::copy(bestMatch.absoluteFilePath(), this->_destinationDir->filePath(priName));
        if (this->_verbose) {
            std::cout << "copying " << bestMatch.fileName().toStdString() << " as " << priName.toStdString() << std::endl;
        }
    } else if (secondMatch.isFile()) {
        QFile::copy(secondMatch.absoluteFilePath(), this->_destinationDir->filePath(priName));
        if (this->_verbose) {
            std::cout << "copying " << secondMatch.fileName().toStdString() << " as " << priName.toStdString() << std::endl;
        }
    }
}
