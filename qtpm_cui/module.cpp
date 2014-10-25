#include "module.h"
#include "qsemver.h"
#include <QDebug>

Module::Module(const QString& name) :
    _name(name), _package(nullptr), _status(Module::NewInstall), _fix(false), _downloaded(false)
{
}

Module::~Module()
{

}

bool Module::findValidVersion()
{
    this->_fix = true;
    auto validVersions = QSemVer::filter(this->_dependents.values(), this->_availableVersions, true);
    if (!validVersions.isEmpty()) {
        this->_finalVersion = validVersions.front();
    }
    return this->valid();
}

bool Module::addDependent(const QString &name, const QString &range)
{
    this->_dependents.insert(name, range);
    if (this->_fix and this->valid()) {
        auto versions = QStringList() << this->_finalVersion;
        return !QSemVer::filter(range, versions, true).isEmpty();
    }
    return true;
}

QString Module::name() const
{
    return _name;
}

QStringList Module::availableVersions() const
{
    return _availableVersions;
}

void Module::setAvailableVersions(const QStringList &availableVersions)
{
    _availableVersions = availableVersions;
}

QString Module::finalVersion() const
{
    return _finalVersion;
}

void Module::setFinalVersion(const QString &version)
{
    this->_finalVersion = version;
    this->_fix = true;
}
bool Module::valid() const
{
    return !this->_finalVersion.isEmpty();
}
bool Module::downloaded() const
{
    return _downloaded;
}

void Module::setDownloaded(bool dowonloaded)
{
    _downloaded = dowonloaded;
}
bool Module::fix() const
{
    return _fix;
}

void Module::setFix()
{
    this->_fix = true;
}
Module::ModuleStatus Module::status() const
{
    return _status;
}

void Module::setStatus(const ModuleStatus &status)
{
    if (this->_status == Module::Installed) {
        this->_fix = false;
        this->_finalVersion = "";
    }
    this->_status = status;
}

QString Module::longPath() const
{
    return _longPath;
}

void Module::setLongPath(const QString &longPath)
{
    _longPath = longPath;
}

QpmPackage *Module::package() const
{
    return _package;
}

void Module::setPackage(QpmPackage *package)
{
    _package = package;
}


QMap<QString, QString> Module::dependents() const
{
    return _dependents;
}
