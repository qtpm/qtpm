#include "initlibtask.h"
#include <QCoreApplication>
#include <QDebug>
#include "qpmpackage.h"
#include "parameterparser.h"
#include "licensefiles.h"
#include <iostream>

InitLibTask::InitLibTask(ParameterParser *param, QObject *parent) :
    QObject(parent), _param(param)
{
}

void InitLibTask::run()
{
    auto app = QCoreApplication::instance();
    QDir currentDir = QDir::current();
    auto param = this->_param;
    bool initialRun = !QpmPackage::hasQtPackageIni(currentDir);

    // Error check;
    bool error = false;
    if (initialRun and param->param("name").isEmpty()) {
        std::cerr << "New package needs --name option to initialize." << std::endl;
        error = true;
    }
    QString type = param->param("type");
    if (!type.isEmpty() && type != "bundle" && type != "dev") {
        std::cerr << "--type option can accept only 'bundle' or 'dev', but "
                  << type.toStdString() << " is passed." << std::endl;
        error = true;
    }
    QString buildType = param->param("buildType");
    if (!buildType.isEmpty() && buildType != "configure"
            && buildType != "source" && buildType != "cmake") {
        std::cerr << "--build option can accept only 'source' or 'configure', 'cmake', but "
                  << type.toStdString() << " is passed." << std::endl;
        error = true;
    }

    auto package = new QpmPackage(currentDir);
    QStringList authors = package->authors() + param->dumpPeoples("authors");

    if (!param->param("license").isEmpty() && authors.isEmpty()) {
        std::cerr << "--license option needs at least one author." << std::endl;
        error = true;
    }

    if (error) {
        if (initialRun) {
            delete package;
            QpmPackage::remove(currentDir);
        }
        app->exit(1);
        return;
    }

    // Start processing

    if (!param->param("name").isEmpty()) {
        package->setName(param->param("name"));
    }

    if (package->version().isEmpty() && param->param("version").isEmpty()) {
        package->setVersion("1.0.0");
    } else if (!param->param("version").isEmpty()) {
        package->setVersion(param->param("version"));
    }

    if (initialRun && type.isEmpty()) {
        package->setLibType("bundle");
    } else if (!type.isEmpty()) {
        package->setLibType(type);
    }

    if (initialRun && buildType.isEmpty()) {
        package->setBuildType("source");
    } else if (!buildType.isEmpty()) {
        package->setBuildType(buildType);
    }

    package->setPackageType("lib");

    if (!param->param("description").isEmpty()) {
        package->setDescription(param->param("description"));
    }
    if (!param->param("homepage").isEmpty()) {
        package->setUrl(param->param("repository"));
    }
    if (!param->param("repository").isEmpty()) {
        package->setRepository(param->param("repository"));
    }
    if (!param->param("bugs").isEmpty()) {
        package->setBugs(param->param("bugs"));
    }
    if (!param->param("buildOption").isEmpty()) {
        package->setBuildOption(param->param("buildOption"));
    }

    package->setAuthors(authors);
    QStringList committers = package->committers() + param->dumpPeoples("committers");
    package->setCommitters(committers);
    QStringList contributors = package->contributors() + param->dumpPeoples("contributors");
    package->setContributors(contributors);

    if (!param->param("license").isEmpty()) {
        LicenseFiles licenses;
        if (licenses.findLicense(param->param("license"))) {
            licenses.writeLicense(currentDir, authors[0], package->name());
            package->setLicense(licenses.displayName());
        } else {
            package->setLicense(param->param("license"));
        }
    }

    delete package;
    app->exit(0);
}
