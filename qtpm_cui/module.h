#ifndef MODULE_H
#define MODULE_H

#include <QString>
#include <QStringList>
#include <QSet>
#include <QMap>
#include "semver/semver.h"

class QpmPackage;

class Module {
public:
    enum ModuleStatus {
        Installed,
        NewInstall,
        LocalFileModule,
        LocalDirModule,
        RemoteModule,
        IndexedModule
    };

    Module(const QString& name);
    ~Module();

    bool findValidVersion();
    bool addDependent(const QString& name, const QString& range);

    QString name() const;
    QMap<QString, QString> dependents() const;
    QStringList availableVersions() const;
    void setAvailableVersions(const QStringList &availableVersions);
    QString finalVersion() const;
    void setFinalVersion(const QString& version);
    bool valid() const;
    bool downloaded() const;
    void setDownloaded(bool downloaded);

    bool fix() const;
    void setFix();
    QpmPackage *package() const;
    void setPackage(QpmPackage *package);

    ModuleStatus status() const;
    void setStatus(const ModuleStatus &status);

    QString longPath() const;
    void setLongPath(const QString &longPath);

    static bool isLocal(Module::ModuleStatus status);

private:
    QString _name;
    QpmPackage* _package;
    QString _longPath;
    QMap<QString, QString> _dependents;
    QStringList _availableVersions;
    QString _finalVersion;
    ModuleStatus _status;
    bool _fix;
    bool _downloaded;
};
#endif // MODULE_H
