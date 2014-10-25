#include "modulemanager.h"
#include "qpmpackage.h"
#include "tasks/extracttask.h"
#include <QFileInfo>
#include <QDebug>

ModuleManager::ModuleManager()
{
    // for unittest
}

ModuleManager::ModuleManager(const QDir &installDir) : _installDir(installDir)
{
    if (QDir::home().exists(".qtpm")) {
        QDir::home().mkdir(".qtpm");
    }
    this->_repositoryDir = QDir(QDir::home().filePath(".qtpm"));
}

ModuleManager::~ModuleManager()
{
    qDeleteAll(this->_modules.values());
}

void ModuleManager::addModuleDependent(const QString &dependentName, const QString &moduleIdentifier, const QDir& destDir)
{
    QRegExp modulePattern("([^#]+)(#[.*])?(@[.*])?");
    QFileInfo info(moduleIdentifier);
    if (info.exists()) {
        if (info.isDir()) {
            auto qtpackage = new QpmPackage(QDir(moduleIdentifier));
            this->addLocalModuleDependent(dependentName, qtpackage->name(), moduleIdentifier);
        } else if (this->supportedSuffixes().contains(info.completeSuffix())) {
            ExtractTask task;
            QString resultDirPath;
            if (task.run(dependentName, destDir, resultDirPath)) {
                auto moduleDir = QDir(destDir.filePath(resultDirPath));
                auto qtpackage = new QpmPackage(moduleDir);
                this->addLocalModuleDependent(dependentName, qtpackage->name(), moduleIdentifier, qtpackage);
            } else {
                QString error("Module error: fail to extract archive file %1");
                this->_errorMessages.append(error.arg(moduleIdentifier));
            }
        } else {
            QString error("Module error: local module should be a directory or archive file that has one extension of %1: %2");
            this->_errorMessages.append(error.arg(this->supportedSuffixes().join(", "), moduleIdentifier));
        }
    } else {

    }
}

void ModuleManager::addInstalledModule(const QString &moduleName, const QString &version)
{
    auto module = new Module(moduleName);
    module->setStatus(Module::Installed);
    module->setFinalVersion(version);
    this->_modules.insert(moduleName, module);
}

void ModuleManager::addLocalModuleDependent(const QString& dependentName, const QString &moduleName, const QString &localPath, QpmPackage *package)
{
    Module* module = this->_modules.value(moduleName, nullptr);
    if (module) {
        if (module->status() != Module::LocalModule) {
            module->setStatus(Module::LocalModule);
            module->setLongPath(localPath);
            this->_nextTargets.insert(moduleName);
            if (package) {
                module->setPackage(package);
            }
        }
    } else {
        module = new Module(moduleName);
        module->setStatus(Module::LocalModule);
        module->setLongPath(localPath);
        this->_modules.insert(moduleName, module);
        this->_nextTargets.insert(moduleName);
        module->addDependent(dependentName, "x");
        if (package) {
            module->setPackage(package);
        }
    }
}

void ModuleManager::addRemoteModuleDependent(const QString &dependentName, const QString &requiredModuleName, const QString &range, QpmPackage *package)
{
    Module* module = this->_modules.value(requiredModuleName, nullptr);
    if (module) {
        if (module->status() == Module::RemoteModule) {
            if (module->fix()) {
                if (!module->addDependent(dependentName, range)) {
                    this->_errorModules.insert(module);
                }
            } else if (!module->addDependent(dependentName, range)) {
                this->_nextTargets.insert(requiredModuleName);
            }
        } else if (module->status() == Module::Installed) {
            if (!module->addDependent(dependentName, range)) {
                module->setStatus(Module::RemoteModule);
                this->_nextTargets.insert(requiredModuleName);
            }
        }
    } else {
        module = new Module(requiredModuleName);
        this->_modules.insert(requiredModuleName, module);
        this->_nextTargets.insert(requiredModuleName);
        module->addDependent(dependentName, range);
        module->setStatus(Module::RemoteModule);
        if (package) {
            module->setPackage(package);
        }
    }
}

void ModuleManager::setAvailableVersions(const QString &moduleName, const QStringList &availableVersions)
{
    Module* module = this->_modules.value(moduleName, nullptr);
    if (!module) {
        module = new Module(moduleName);
        this->_modules.insert(moduleName, module);
    }
    module->setAvailableVersions(availableVersions);
}

bool ModuleManager::addAlias(const QString& dependentName, const QString &shortName, const QString &longName, Module::ModuleStatus status)
{
    bool update = false;
    if (dependentName != "app" and status == Module::LocalModule) {
        auto msg = QString("%1 is used from %2, but local path is not recommended except for root application packages.").arg(longName, dependentName);
        this->_warningMessages.append(msg);
    }
    this->_reverseAliases[longName] = shortName;
    if (this->_aliases.contains(shortName)) {
        if (this->_aliasTypes[shortName] == Module::RemoteModule and status == Module::LocalModule) {
            update = true;
        }
    } else {
        update = true;
        this->_aliasLog.insert(shortName, QMap<Module::ModuleStatus, QMap<QString, QStringList> >());
    }
    if (!this->_aliasLog[shortName].contains(status)) {
        this->_aliasLog[shortName].insert(status, QMap<QString, QStringList>());
    }
    if (!this->_aliasLog[shortName][status].contains(longName)) {
        this->_aliasLog[shortName][status].insert(longName, QStringList());
    }
    this->_aliasLog[shortName][status][longName].append(dependentName);

    if (update) {
        this->_aliases[shortName] = longName;
        this->_aliasTypes[shortName] = status;
    }

    return update;
}

bool ModuleManager::isInstalled(const QString &name) const
{
    if (this->_reverseAliases.contains(name)) {
        return true;
    }
    return (this->_aliases.contains(name));
}

void ModuleManager::checkAliasConflict()
{
    auto i = this->_aliasLog.constBegin();
    while (i != this->_aliasLog.constEnd()) {
        const QString& shortName = i.key();
        auto statusMap = i.value();
        Module::ModuleStatus currentStatus = this->_aliasTypes[shortName];
        const QString& currentModule = this->_aliases[shortName];
        if (statusMap[currentStatus].count() > 1) {
            QStringList modules = statusMap[currentStatus].keys();
            modules.removeOne(currentModule);
            QString msg("module '%1' is used for %2. %3 are ignored");
            this->_warningMessages.append(msg.arg(currentModule, shortName, modules.join(", ")));
        }
        ++i;
    }
}

QList<Module *> ModuleManager::shift()
{
    QList<Module *> result;
    foreach(const QString& moduleName, this->_nextTargets) {
        Module* module = this->_modules.value(moduleName);
        if (module->findValidVersion()) {
            result.append(module);
        } else {
            this->_errorModules.insert(module);
        }
    }
    this->_nextTargets.clear();
    return result;
}

QList<Module *> ModuleManager::versionErrorModules() const
{
    return _errorModules.toList();
}

const QStringList &ModuleManager::supportedSuffixes() const
{
    static QStringList suffixes;
    if (suffixes.length() == 0) {
        suffixes << "tar" << "tar.bz2" << "tbz" << "tar.gz" << "tgz" << "tar.lzma" << "tar.xz" << "zip";
    }
    return suffixes;
}
const QStringList &ModuleManager::errorMessages() const
{
    return _errorMessages;
}
const QStringList &ModuleManager::warningMessages() const
{
    return _warningMessages;
}
