#include "licensefiles.h"
#include "NLTemplate.h"
#include <sstream>
#include <QDebug>
#include <QFile>
#include <QDateTime>

class LoaderQFile : public NL::Template::Loader {
public:
    NL::Template::Loader::Result load( const std::string & name )
    {
        QFile infile(QString::fromStdString(name));
        if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            //qDebug() << __FILE__ << __LINE__;
            return { false, nullptr, "Could not open file " + name };
        }
        QTextStream instream(&infile);
        QString result = instream.readAll();
        return { true, result.toStdString() };
    }
};


LicenseFiles::LicenseFiles()
{
    this->_licenseNames.insert("apache-v2.0", "Apache License v2.0");
    this->_licenseNames.insert("artistic-v2.0", "The Artistic License v2.0");
    this->_licenseNames.insert("bsd-2", "Simplified BSD License (BSD 2-clause)");
    this->_licenseNames.insert("bsd-3", "Modified BSD License (BSD 3-clause)");
    this->_licenseNames.insert("epl-v1.0", "Eclipse Public License (EPL) v1.0");
    this->_licenseNames.insert("gnu-agpl-v3.0", "GNU Affero General Public License (AGPL) v3.0");
    this->_licenseNames.insert("gnu-fdl-v1.3", "GNU Free Documentation License (FDL) v1.3");
    this->_licenseNames.insert("gnu-gpl-v1.0", "GNU General Public License (GPL) v1.0");
    this->_licenseNames.insert("gnu-gpl-v2.0", "GNU General Public License (GPL) v2.0");
    this->_licenseNames.insert("gnu-gpl-v3.0", "GNU General Public License (GPL) v3.0");
    this->_licenseNames.insert("gnu-lgpl-v2.1", "GNU Lesser General Public License (LGPL) v2.1");
    this->_licenseNames.insert("gnu-lgpl-v3.0", "GNU Lesser General Public License (LGPL) v3.0");
    this->_licenseNames.insert("mit", "The MIT License (MIT)");
    this->_licenseNames.insert("mpl-v2.0", "Mozilla Public License (MPL) v2.0");
    this->_licenseNames.insert("unlicense", "The Unlicense (Public Domain)");

    this->_aliases.insert("apache2", "apache-v2.0");
    this->_aliases.insert("perl", "artistic-v2.0");
    this->_aliases.insert("eclipse", "epl-v1.0");
    this->_aliases.insert("mozilla", "mpl-v2.0");
    this->_aliases.insert("gpl", "gnu-gpl-v3.0");
    this->_aliases.insert("gpl2", "gnu-gpl-v2.0");
    this->_aliases.insert("gpl3", "gnu-gpl-v3.0");
    this->_aliases.insert("lgpl", "gnu-lgpl-v3.0");
    this->_aliases.insert("lgpl2", "gnu-lgpl-v2.1");
    this->_aliases.insert("lgpl3", "gnu-lgpl-v3.0");
    this->_aliases.insert("bsd", "bsd-3");
    this->_aliases.insert("x", "mit");
    this->_aliases.insert("public domain", "unlicense");
}

LicenseFiles::~LicenseFiles()
{

}

QStringList LicenseFiles::licenseKeys() const
{
    return this->_licenseNames.keys();
}

QString LicenseFiles::licensePath() const
{
    if (this->_licenseNames.contains(this->_license)) {
        return QString(":/rest-licenses/%1.rst").arg(this->_license);
    } else {
        return QString();
    }
}

QString LicenseFiles::findKey(const QString &displayName) const
{
    auto keys = this->_licenseNames.keys(displayName);
    if (keys.isEmpty()) {
        return QString();
    }
    return keys.first();
}

QString LicenseFiles::displayName() const
{
    return this->_licenseNames.value(this->_license, QString());
}

bool LicenseFiles::findLicense(const QString &searchKey)
{
    auto lowerKey = searchKey.toLower();
    foreach (const QString& key, this->_aliases.keys()) {
        if (key.contains(lowerKey)) {
            this->_license = this->_aliases[key];
            return true;
        }
    }

    foreach (const QString& key, this->_licenseNames.keys()) {
        if (key.contains(lowerKey)) {
            return true;
        }
    }

    return false;
}

QString LicenseFiles::writeLicense(const QDir &dir, const QString &author, const QString &name) const
{
    LoaderQFile loader;
    NL::Template::Template t(loader);
    t.load(this->licensePath().toStdString());
    t.set("author", author.toStdString());
    t.set("organization", name.toStdString());
    t.set("year", QString::number(QDateTime::currentDateTime().date().year()).toStdString());
    std::stringstream out;
    t.render(out);
    QFile outfile(dir.filePath("LICENSE.rst"));
    if (outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream outstream(&outfile);
        outstream << QString::fromStdString(out.str());
    }

}
QString LicenseFiles::license() const
{
    return _license;
}

void LicenseFiles::setLicense(const QString &license)
{
    _license = license;
}

