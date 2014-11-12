#include "qpmpackage.h"
#include <QDebug>

static const char* filename = "qtpackage.ini";

QpmPackage::QpmPackage(const QDir& dir, QObject *parent) :
    QObject(parent)
{
    //qDebug() << dir.path();
    auto filepath = dir.filePath(filename);
    this->_setting = new QSettings(filepath, QSettings::IniFormat);
    this->_init();
}

QpmPackage::~QpmPackage()
{
    this->_setting->sync();
    delete this->_setting;
}

QpmPackage *QpmPackage::search(const QDir &dir)
{
    QDir nonConstDir(dir);
    do {
        if (nonConstDir.exists(filename)) {
            return new QpmPackage(nonConstDir);
        }
    } while (nonConstDir.cdUp());
    return nullptr;
}

QString QpmPackage::filePath() const
{
    return this->_setting->fileName();
}

QString QpmPackage::packageType() const
{
    return this->_setting->value("package/type").toString();
}

void QpmPackage::setPackageType(const QString &type)
{
    this->_setting->setValue("package/type", type);
}

QString QpmPackage::name() const
{
    return this->_setting->value("package/name").toString();
}

void QpmPackage::setName(const QString &title)
{
    this->_setting->setValue("package/name", title);
}

QString QpmPackage::version() const
{
    return this->_setting->value("package/version").toString();
}

void QpmPackage::setVersion(const QString &version)
{
    this->_setting->setValue("package/version", version);
}

QString QpmPackage::libType() const
{
    return this->_setting->value("package/libtype").toString();
}

void QpmPackage::setLibType(const QString &type)
{
    this->_setting->setValue("package/libtype", type);
}

QString QpmPackage::buildType() const
{
    return this->_setting->value("build/type").toString();
}

void QpmPackage::setBuildType(const QString &type)
{
    this->_setting->setValue("build/type", type);
}

QString QpmPackage::buildOption() const
{
    return this->_setting->value("build/option").toString();
}

void QpmPackage::setBuildOption(const QString &option)
{
    this->_setting->setValue("build/option", option);
}

QString QpmPackage::description() const
{
    return this->_setting->value("package/description").toString();
}

void QpmPackage::setDescription(const QString &description)
{
    this->_setting->setValue("package/description", description);
}

QStringList QpmPackage::authors() const
{
    return this->_persons("authors");
}

void QpmPackage::setAuthors(const QStringList &authors)
{
    this->_setPersons("authors", authors);
}

QStringList QpmPackage::committers() const
{
    return this->_persons("committers");
}

void QpmPackage::setCommitters(const QStringList &committers)
{
    this->_setPersons("committers", committers);
}

QStringList QpmPackage::contributors() const
{
    return this->_persons("contributors");
}

void QpmPackage::setContributors(const QStringList &contributors)
{
    this->_setPersons("contributors", contributors);
}

QString QpmPackage::repository() const
{
    return this->_setting->value("package/repository").toString();
}

void QpmPackage::setRepository(const QString &repository)
{
    this->_setting->setValue("package/repository", repository);
}
QString QpmPackage::bugs() const
{
    return this->_setting->value("package/bugs").toString();
}

void QpmPackage::setBugs(const QString &bugs)
{
    this->_setting->setValue("package/bugs", bugs);
}
QString QpmPackage::url() const
{
    return this->_setting->value("package/url").toString();
}

void QpmPackage::setUrl(const QString &url)
{
    this->_setting->setValue("package/url", url);
}
QString QpmPackage::license() const
{
    return this->_setting->value("package/license").toString();
}

void QpmPackage::setLicense(const QString &license)
{
    this->_setting->setValue("package/license", license);
}

bool QpmPackage::addBundlePackage(const QString &packageName, const QString& version)
{
    if (this->hasDevelopmentPackage(packageName)) {
        return false;
    }
    this->_setting->beginGroup("bundle-package");
    this->_setting->setValue(packageName, version);
    this->_setting->endGroup();
    return true;
}

bool QpmPackage::updateBundlePackage(const QString &packageName, const QString &version)
{
    bool result;
    this->_setting->beginGroup("bundle-package");
    if (this->_setting->contains(packageName)) {
         this->_setting->setValue(packageName, version);
        result = true;
    } else {
        result = false;
    }
    this->_setting->endGroup();
    return result;
}

bool QpmPackage::hasBundlePackage(const QString &packageName) const
{
    this->_setting->beginGroup("bundle-package");
    bool result = this->_setting->contains(packageName);
    this->_setting->endGroup();
    return result;
}

QMap<QString, QString> QpmPackage::bundlePackages() const
{
    QMap<QString, QString> result;

    this->_setting->beginGroup("bundle-package");
    foreach(const QString& key, this->_setting->childKeys()) {
        result.insert(key, this->_setting->value(key).toString());
    }
    this->_setting->endGroup();

    return result;
}

bool QpmPackage::addDevelopmentPackage(const QString &packageName, const QString& version)
{
    if (this->hasBundlePackage(packageName)) {
        return false;
    }
    this->_setting->beginGroup("development-package");
    this->_setting->setValue(packageName, version);
    this->_setting->endGroup();
    return true;
}

bool QpmPackage::updateDevelopmentPackage(const QString &packageName, const QString &version)
{
    bool result;
    this->_setting->beginGroup("development-package");
    if (this->_setting->contains(packageName)) {
         this->_setting->setValue(packageName, version);
        result = true;
    } else {
        result = false;
    }
    this->_setting->endGroup();
    return result;
}

bool QpmPackage::hasDevelopmentPackage(const QString &packageName) const
{
    this->_setting->beginGroup("development-package");
    bool result = this->_setting->contains(packageName);
    this->_setting->endGroup();
    return result;
}

QMap<QString, QString> QpmPackage::developmentPackages() const
{
    QMap<QString, QString> result;

    this->_setting->beginGroup("development-package");
    foreach(const QString& key, this->_setting->childKeys()) {
        result.insert(key, this->_setting->value(key).toString());
    }
    this->_setting->endGroup();

    return result;
}

QString QpmPackage::packageVersion(const QString &packageName) const
{
    this->_setting->beginGroup("bundle-package");
    if (this->_setting->contains(packageName)) {
        auto result = this->_setting->value(packageName).toString();
        this->_setting->endGroup();
        return result;
    }
    this->_setting->endGroup();

    this->_setting->beginGroup("development-package");
    if (this->_setting->contains(packageName)) {
        auto result = this->_setting->value(packageName).toString();
        this->_setting->endGroup();
        return result;
    }
    this->_setting->endGroup();

    return "";
}

bool QpmPackage::hasQtPackageIni(const QDir &dir)
{
    return dir.exists(filename);
}

void QpmPackage::remove(QDir &dir)
{
    dir.remove(filename);
}

void QpmPackage::_init()
{
}

void QpmPackage::_setPersons(const QString &type, const QStringList &persons)
{
    this->_setting->beginWriteArray(type);
    for (int i = 0; i < persons.length(); i++) {
        this->_setting->setArrayIndex(i);
        this->_setting->setValue("name", persons[i]);
    }
    this->_setting->endArray();
}

QStringList QpmPackage::_persons(const QString &type) const
{
    QStringList result;
    int count = this->_setting->beginReadArray(type);
    for (int i = 0; i < count; i++) {
        this->_setting->setArrayIndex(i);
        result.append(this->_setting->value("name").toString());
    }
    this->_setting->endArray();
    return result;
}
