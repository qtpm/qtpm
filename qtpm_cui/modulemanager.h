#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QMultiMap>
#include <QSet>
#include <QDir>
#include "semver/semver.h"
#include "module.h"

class ModuleManagerTest;

class ModuleManager
{
public:
    ModuleManager();
    ModuleManager(const QDir& installDir);
    ~ModuleManager();

    bool refresh();

    /**
     * @brief Adds requestedModules;
     * @param module
     */
    void addModuleDependent(const QString &dependentName, const QString &moduleIdentifier, const QDir& destDir);
    void setAvailableVersions(const QString& moduleName, const QStringList& availableVersions);
    bool isInstalled(const QString& name) const;
    void checkAliasConflict();

    QList<Module *> shift();
    QList<Module *> versionErrorModules() const;
    const QStringList& supportedSuffixes() const;
    const QStringList& errorMessages() const;
    const QStringList& warningMessages() const;

    bool verbose() const;
    void setVerbose(bool verbose);

private:
    bool _verbose;
    QDir _installDir;
    QDir _repositoryDir;
    QSet<QString> _nextTargets;
    QMap<QString, Module*> _modules;
    QSet<Module*> _errorModules;
    // short name -> long name
    QMap<QString, QString> _aliases;
    QMap<QString, QString> _reverseAliases;
    // short name -> status => long name -> dependent
    QMap<QString, QMap<Module::ModuleStatus, QMap<QString, QStringList> > > _aliasLog;
    QStringList _errorMessages;
    QStringList _warningMessages;

    void _addInstalledModule(const QString& moduleName, const QString& version);
    void _addLocalModuleDependent(const QString& dependentName, const QString& moduleName, const QString& localPath, Module::ModuleStatus status, QpmPackage* package=nullptr);
    void _addRemoteModuleDependent(const QString& dependentName, const QString& requiredModuleName, const QString& range, QpmPackage* package=nullptr);
    bool _addAlias(const QString& dependentName, const QString& shortName, const QString& longName, Module::ModuleStatus status);
    void _addModule(Module* module);

    friend class ModuleManagerTest;
};

#endif // MODULE_MANAGER_H
