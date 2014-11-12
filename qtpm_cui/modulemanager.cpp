#include "modulemanager.h"
#include "qpmpackage.h"
#include "aliasdatabase.h"
#include "tasks/extracttask.h"
#include "tasks/externalprocess.h"
#include <QFileInfo>
#include <memory>
#include <QDebug>
#include <iostream>

ModuleManager::ModuleManager() : _verbose(false)
{
    // for unittest
}

ModuleManager::ModuleManager(const QDir &installDir)
    : _verbose(false), _installDir(installDir)
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
    if (this->_verbose) {
        std::cout << "processing " << moduleIdentifier.toStdString() << std::endl;
    }
    QRegExp branchPattern("([^#@]+)(#.+)");
    QRegExp versionPattern("([^#@]+)(@.+)");
    QFileInfo info(moduleIdentifier);
    if (info.exists()) {
        if (info.isDir()) {
            this->_prepareLocalDirModule(dependentName, info);
        } else if (this->supportedSuffixes().contains(info.completeSuffix())) {
            this->_prepareLocalArchiveFileModule(dependentName, moduleIdentifier, destDir);
        } else {
            QString error("Module error: local module should be a directory or archive file that has one extension of %1: %2");
            this->_errorMessages.append(error.arg(this->supportedSuffixes().join(", "), moduleIdentifier));
        }
    } else {
        if (this->_verbose) {
            std::cout << "    module is git package" << std::endl;
        }
        if (branchPattern.indexIn(moduleIdentifier) != -1) {
            this->_prepareBranchGitModule(dependentName, branchPattern, destDir);
        } else if (versionPattern.indexIn(moduleIdentifier) != -1) {
            this->_prepareBranchGitModule(dependentName, versionPattern, destDir);
        } else {
            this->_prepareDefaultGitModule(dependentName, moduleIdentifier, destDir);
        }
    }
}

void ModuleManager::_addInstalledModule(const QString &moduleName, const QString &version)
{
    auto module = new Module(moduleName);
    module->setStatus(Module::Installed);
    module->setFinalVersion(version);
    this->_modules.insert(moduleName, module);
}

void ModuleManager::_addLocalModuleDependent(const QString& dependentName, const QString &shortName, const QString &localPath, Module::ModuleStatus status, QpmPackage *package)
{
    Module* module = this->_modules.value(shortName, nullptr);
    if (module) {
        if (!Module::isLocal(module->status())) {
            module->setStatus(status);
            module->setLongPath(localPath);
            this->_nextTargets.insert(shortName);
            if (package) {
                module->setPackage(package);
            }
            this->_addAlias(dependentName, shortName, localPath, status);
        } else if (module->longPath() != localPath) {
            this->_errorModules.insert(module);
        }
    } else {
        module = new Module(shortName);
        module->setLongPath(localPath);
        module->setStatus(status);
        this->_modules.insert(shortName, module);
        this->_nextTargets.insert(shortName);
        module->addDependent(dependentName, shortName);
        if (package) {
            module->setPackage(package);
        }
        this->_addAlias(dependentName, shortName, localPath, status);
    }
}

Module* ModuleManager::_addRemoteModuleDependent(const QString &dependentName, const QString &requiredModuleName, Module::ModuleStatus status, const QString &range, QpmPackage *package)
{
    Module* module = this->_modules.value(requiredModuleName, nullptr);
    if (module) {
        if (status == Module::RemoteBranchModule) {
            if (module->isLocal()) {
                this->_warningMessages.append(
                        QString("%1 uses %2 by using git branch, but local one has higher priority. This module is ignored.")
                            .arg(dependentName, requiredModuleName));
            } else if (module->status() == Module::RemoteBranchModule && module->finalVersion() != range) {
                this->_warningMessages.append(
                        QString("%1 uses %2 of '%3' branch, but other module specify by '%4' branch already. This module is ignored.")
                            .arg(dependentName, requiredModuleName, range, module->finalVersion()));
            } else if (module->status() == Module::RemoteVersionModule && module->fix()) {
                this->_warningMessages.append(
                        QString("%1 uses %2 of '%3' branch, but other module specify by version already. This module is ignored.")
                            .arg(dependentName, requiredModuleName, range));
            } else {
                module->addDependent(dependentName, range, true);
            }
        } else if (status == Module::RemoteVersionModule) {

        }
        if (module->status() == Module::RemoteVersionModule) {
            if (status == Module::RemoteBranchModule) {
                if (!module->addDependent(dependentName, range)) {
                    module->setStatus(status);
                    this->_nextTargets.insert(requiredModuleName);
                }
            }
            else if (module->fix()) {
                if (!module->addDependent(dependentName, range)) {
                    this->_errorModules.insert(module);
                }
            } else if (!module->addDependent(dependentName, range)) {
                this->_nextTargets.insert(requiredModuleName);
            }
        } else if (module->status() == Module::Installed) {
            if (!module->addDependent(dependentName, range)) {
                module->setStatus(status);
                this->_nextTargets.insert(requiredModuleName);
            }
        }
    } else {
        module = new Module(requiredModuleName);
        this->_modules.insert(requiredModuleName, module);
        this->_nextTargets.insert(requiredModuleName);
        module->addDependent(dependentName, range, (status == Module::RemoteBranchModule));
        module->setStatus(status);
        if (package) {
            module->setPackage(package);
        }
    }
    return module;
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

bool ModuleManager::_addAlias(const QString& dependentName, const QString &shortName, const QString &longName, Module::ModuleStatus status)
{
    bool update = false;
    if (dependentName != "app") {
        if (Module::isLocal(status)) {
            auto msg = QString("%1 is used from %2, but local path is not recommended except for root application packages.").arg(longName, dependentName);
            this->_warningMessages.append(msg);
        } else if (status == Module::RemoteBranchModule) {
            auto msg = QString("%1 is used from %2, but specifing branch is not recommended except for root application packages.").arg(longName, dependentName);
            this->_warningMessages.append(msg);
        }
    }
    this->_reverseAliases[longName] = shortName;
    if (this->_aliases.contains(shortName)) {
        auto moduleStatus = this->_modules[shortName]->status();
        if (moduleStatus == Module::RemoteBranchModule && Module::isLocal(status)) { // check it
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
    }

    return update;
}

void ModuleManager::_addModule(Module *module)
{
    this->_modules.insert(module->name(), module);
}

void ModuleManager::_prepareLocalDirModule(const QString &dependentName, const QFileInfo &info)
{
    if (this->_verbose) {
        std::cout << "    module is dir" << std::endl;
    }
    auto qtpackage = new QpmPackage(QDir(info.baseName()));
    this->_addLocalModuleDependent(dependentName, qtpackage->name(), info.absoluteFilePath(), Module::LocalDirModule, qtpackage);
}

void ModuleManager::_prepareLocalArchiveFileModule(const QString &dependentName, const QFileInfo &info, const QDir &destDir)
{
    if (this->_verbose) {
        std::cout << "    module is an archive file" << std::endl;
    }
    ExtractTask task;
    QString resultDirPath;
    if (task.run(info.absoluteFilePath(), destDir, resultDirPath, this->_verbose)) {
        auto moduleDir = QDir(destDir.filePath(resultDirPath));
        auto qtpackage = new QpmPackage(moduleDir);
        this->_addLocalModuleDependent(dependentName, qtpackage->name(), info.baseName(), Module::LocalFileModule, qtpackage);
    } else {
        QString error("Module error: fail to extract archive file %1");
        this->_errorMessages.append(error.arg(info.baseName()));
    }
}

void ModuleManager::_prepareDefaultGitModule(const QString &dependentName, const QString &moduleIdentifier, const QDir& destDir)
{
    if (this->_verbose) {
        std::cout << "    module is git repository" << std::endl;
    }
    AliasDatabase database;
    auto url = database.toUrl(moduleIdentifier);
    if (!url.isEmpty()) {
        int result;
        QDir moduleDir = this->_gitCheckout(url, destDir, result);
        if (result == 0) {
            QpmPackage* package = new QpmPackage(moduleDir);
            this->_addRemoteModuleDependent(dependentName, package->name(), Module::RemoteVersionModule, QString("^") + package->version(), package);
        } else {
            std::cerr << "    git fails: " << result << std::endl;
        }
    }
}

void ModuleManager::_prepareVersionGitModule(const QString &dependentName, const QRegExp &moduleIdentifier, const QDir &destDir)
{
    if (this->_verbose) {
        std::cout << "    module is git repository" << std::endl;
    }
    AliasDatabase database;
    auto url = database.toUrl(moduleIdentifier.cap(1));
    if (!url.isEmpty()) {
        int result;
        QDir moduleDir = this->_gitCheckout(url, destDir, result, moduleIdentifier.cap(2).mid(1), true);
        if (result == 0) {
            QpmPackage* package = new QpmPackage(moduleDir);
            this->_addRemoteModuleDependent(dependentName, package->name(), Module::RemoteVersionModule, QString("^") + package->version(), package);
        } else {
            std::cerr << "    git fails: " << result << std::endl;
        }
    }
}

void ModuleManager::_prepareBranchGitModule(const QString &dependentName, const QRegExp &moduleIdentifier, const QDir &destDir)
{
    if (this->_verbose) {
        std::cout << "    module is git repository" << std::endl;
    }
    AliasDatabase database;
    auto url = database.toUrl(moduleIdentifier.cap(1));
    if (!url.isEmpty()) {
        int result;
        auto branchName = moduleIdentifier.cap(2).mid(1);
        QDir moduleDir = this->_gitCheckout(url, destDir, result, branchName);
        if (result == 0) {
            QpmPackage* package = new QpmPackage(moduleDir);
            auto module = this->_addRemoteModuleDependent(dependentName, package->name(), Module::RemoteBranchModule, QString("^") + package->version(), package);
            module->setBranch(branchName);
        } else {
            std::cerr << "    git fails: " << result << std::endl;
        }
    }
}

QDir ModuleManager::_gitCheckout(const QString &url, const QDir &destDir, int &result, const QString &reference, bool isTag)
{
    ExternalProcess process(true);
    QString dirName = url.split("/").last();
    if (dirName.isEmpty()) {
        QString urlCopy(url);
        urlCopy.chop(1);
        dirName = urlCopy.split("/").last();
    } else if (dirName.endsWith(".git")) {
        dirName.chop(4);
    }
    QStringList args;
    QDir workDir;
    QDir moduleDir(destDir);
    moduleDir.cd(dirName);
    bool exist = destDir.exists(dirName + "/.git");
    if (exist && isTag) {
        return moduleDir; // skip running
    }
    if (exist) {
        workDir = moduleDir;
        if (reference.isEmpty()) {
            args << "pull" << "--depth" << "1";
        } else {
            args << "pull" << "--depth" << "1" << "origin" << reference;
        }
    } else {
        workDir = destDir;
        if (destDir.exists(dirName)) {
            QDir dir(destDir.filePath(dirName));
            dir.removeRecursively();
        }
        if (reference.isEmpty()) {
            args << "clone" << "--depth" << "1" << url;
        } else {
            args << "clone" << "--depth" << "1" << "-b" << reference << url;
        }
    }
    if (this->_verbose) {
        std::cout << "    git " << args.join(" ").toStdString() << std::endl;
    }
    process.run("git", args, workDir);
    process.waitForFinished(-1);
    result = process.exitCode();
    return moduleDir;
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
        auto module = this->_modules[shortName];
        Module::ModuleStatus currentStatus = module->status();
        // In conflict checking logic, LocalDir and LocalFile have
        // same priority.
        if (currentStatus == Module::LocalDirModule) {
            currentStatus = Module::LocalFileModule;
        }
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

QList<Module*> ModuleManager::shift()
{
    QList<Module *> result;
    for (const QString& moduleName : this->_nextTargets) {
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

QList<Module*> ModuleManager::versionErrorModules() const
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
bool ModuleManager::verbose() const
{
    return _verbose;
}

void ModuleManager::setVerbose(bool verbose)
{
    _verbose = verbose;
}

