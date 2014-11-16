#include "initapptask.h"
#include <QCoreApplication>
#include <QDebug>
#include "qpmpackage.h"
#include "parameterparser.h"
#include "licensefiles.h"
#include <iostream>

InitAppTask::InitAppTask(ParameterParser *param, QObject *parent) :
    QObject(parent), _param(param)
{

}

void InitAppTask::run()
{
    auto app = QCoreApplication::instance();
    QDir currentDir = QDir::current();
    auto param = this->_param;
    bool initialRun = !QpmPackage::hasQtPackageIni(currentDir);

    // Error check;
    bool error = false;

    auto package = new QpmPackage(currentDir);
    QStringList authors = package->authors() + param->dumpPeoples("authors");

    if (!param->param("license").isEmpty() and authors.isEmpty()) {
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

    package->setPackageType("app");

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
        package->setBugs(param->param("build-option"));
    }

    package->setAuthors(authors);
    QStringList committers = package->committers() + param->dumpPeoples("committers");
    package->setCommitters(committers);
    QStringList contributors = package->contributors() + param->dumpPeoples("contributors");
    package->setContributors(contributors);

    if (!param->param("license").isEmpty()) {
        LicenseFiles licenses;
        if (licenses.findLicense(param->param("license"))) {
            QString packageName = package->name();
            QString author = authors[0];
            licenses.writeLicense(currentDir, author, packageName);
            package->setLicense(licenses.displayName());
        } else {
            package->setLicense(param->param("license"));
        }
    }
    delete package;
    app->exit(0);
}
