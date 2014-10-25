#ifndef LICENSEFILES_H
#define LICENSEFILES_H

#include <QStringList>
#include <QMap>
#include <QDir>

class LicenseFiles
{
public:
    explicit LicenseFiles();
    virtual ~LicenseFiles();
    QStringList licenseKeys() const;
    QString licensePath() const;
    QString findKey(const QString& displayName) const;
    QString displayName() const;
    bool findLicense(const QString& searchKey);
    QString writeLicense(const QDir& dir, const QString& author, const QString& name) const;
    QString license() const;
    void setLicense(const QString& license);

private:
    QMap<QString, QString> _licenseNames;
    QMap<QString, QString> _aliases;
    QString _license;
};

#endif // LICENSEFILES_H
