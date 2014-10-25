#ifndef QPMPACKAGE_H
#define QPMPACKAGE_H

#include <QObject>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QMap>

class QpmPackage : public QObject
{
    Q_OBJECT

public:
    enum PackageType {
        SourceType,
        ConfigureType
    };

    explicit QpmPackage(const QDir& dir, QObject *parent = 0);
    virtual ~QpmPackage();

    static QpmPackage *search(const QDir& dir);
    static bool hasQtPackageIni(const QDir& dir);
    static void remove(QDir &dir);

    QString filePath() const;

    QString packageType() const;
    void setPackageType(const QString &type);
    QString name() const;
    void setName(const QString &name);
    QString libType() const;
    void setLibType(const QString &type);
    QString buildType() const;
    void setBuildType(const QString &type);
    QString buildOption() const;
    void setBuildOption(const QString &option);
    QString description() const;
    void setDescription(const QString &description);
    QStringList authors() const;
    void setAuthors(const QStringList &authors);
    QStringList committers() const;
    void setCommitters(const QStringList &committers);
    QStringList contributors() const;
    void setContributors(const QStringList &contributors);
    QString repository() const;
    void setRepository(const QString &repository);
    QString bugs() const;
    void setBugs(const QString &bugs);
    QString url() const;
    void setUrl(const QString &url);
    QString license() const;
    void setLicense(const QString &license);

    bool addBundlePackage(const QString& packageName, const QString& version);
    bool updateBundlePackage(const QString& packageName, const QString& version);
    bool hasBundlePackage(const QString& packageName) const;
    QMap<QString, QString> bundlePackages() const;
    bool addDevelopmentPackage(const QString& packageName, const QString& version);
    bool updateDevelopmentPackage(const QString& packageName, const QString& version);
    bool hasDevelopmentPackage(const QString& packageName) const;
    QMap<QString, QString> developmentPackages() const;
    QString packageVersion(const QString& packageName) const;

signals:

public slots:

protected:
    QSettings* _setting;
    void _init();

    void _setPersons(const QString& type, const QStringList &_persons);
    QStringList _persons(const QString& type) const;
};

#endif // QPMPACKAGE_H
