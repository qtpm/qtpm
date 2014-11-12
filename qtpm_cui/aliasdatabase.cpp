#include "aliasdatabase.h"
#include <QRegExp>
#include <QDebug>
#include <QDir>
#include <iostream>

const QString sshPatternSrc = "git@(\\w[\\w\\.]+):(\\w[\\w\\.-]+)/(\\w[\\w\\.-]+)\\.git";
const QString httpsPatternSrc = "https?://(\\w[\\w\\.]+)/(\\w[\\w\\.-]+)/(\\w[\\w\\.-]+)(\\.git)?";

AliasDatabase::AliasDatabase(QObject *parent) :
    QObject(parent)
{
    QDir home = QDir::home();
    if (!home.exists(".qtpm")) {
        home.mkdir(".qtpm");
    }
    QDir qtpmDir = QDir(home.filePath(".qtpm"));
    this->_settings = new QSettings(qtpmDir.filePath("alias.ini"), QSettings::IniFormat);
}

AliasDatabase::AliasDatabase(const QDir& dir, QObject *parent) :
    QObject(parent)
{
    this->_settings = new QSettings(dir.filePath("alias.ini"), QSettings::IniFormat);
}

AliasDatabase::~AliasDatabase()
{
    delete this->_settings;
    this->_settings = nullptr;
}

void AliasDatabase::refresh()
{

}

void AliasDatabase::refresh(const QString &content)
{
    QRegExp aliasParser("`([^`<]+)\\s*<([^>`]+)>`_\\s+(.+)");
    this->_settings->clear();

    foreach(const QString& line, content.split("\n")) {
        int pos = aliasParser.indexIn(line);
        if (pos != -1) {
            auto name = aliasParser.cap(1).trimmed();
            auto url = aliasParser.cap(2).trimmed();
            auto description = aliasParser.cap(3).trimmed();
            this->_settings->beginGroup(name);
            this->_settings->setValue("url", url);
            this->_settings->setValue("description", description);
            this->_settings->endGroup();
        }
    }
    this->_settings->sync();
}

int AliasDatabase::count() const
{
    return this->_settings->childGroups().count();
}

bool AliasDatabase::contains(const QString &key) const
{
    return this->_settings->childGroups().contains(key);
}

DatabaseEntry AliasDatabase::find(const QString &key) const
{
    if (this->contains(key)) {
        this->_settings->beginGroup(key);
        auto url = this->_settings->value("url").toString();
        auto description = this->_settings->value("description").toString();
        this->_settings->endGroup();
        return DatabaseEntry(key, url, description);
    } else {
        return DatabaseEntry();
    }
}

QList<DatabaseEntry> AliasDatabase::search(const QString &key) const
{
    QList<DatabaseEntry> result;

    QString lowKey = key.toLower();

    foreach(const QString& name, this->_settings->childGroups()) {
        this->_settings->beginGroup(name);
        QString url = this->_settings->value("url").toString();
        QString description = this->_settings->value("description").toString();
        if (description.toLower().contains(lowKey) || url.contains(lowKey) || name.toLower().contains(lowKey)) {
            result.append(DatabaseEntry(name, url, description));
        }
        this->_settings->endGroup();
    }

    return result;
}

QList<DatabaseEntry> AliasDatabase::search_near_title(const QString &key, int distance) const
{
    QList<DatabaseEntry> result;

    QString lowKey = key.toLower();

    foreach(const QString& name, this->_settings->childGroups()) {
        QString lowName = name.toLower();
        if (this->_levenshtein_distance(lowKey, lowName) < distance) {
            this->_settings->beginGroup(name);
            QString url = this->_settings->value("url").toString();
            QString description = this->_settings->value("description").toString();
            result.append(DatabaseEntry(name, url, description));
            this->_settings->endGroup();
        }
    }

    return result;
}

bool AliasDatabase::isUrl(const QString &text) const
{
    QRegExp sshPattern(sshPatternSrc);
    QRegExp httpsPattern(httpsPatternSrc);
    if (sshPattern.indexIn(text) != -1) {
        return true;
    }
    if (httpsPattern.indexIn(text) != -1) {
        return true;
    }
    return false;
}

QString AliasDatabase::toUrl(const QString moduleIdentifier) const
{
    QStringList fragments = moduleIdentifier.split("/");
    QString url;
    if (this->isUrl(moduleIdentifier)) {
        url = moduleIdentifier;
    } else if (fragments.count() == 1) {
        if (this->contains(moduleIdentifier)) {
            url = this->find(moduleIdentifier).url();
        } else {
            std::cerr << "    This module is not in alias database." << std::endl;
        }
    } else if (fragments.count() == 3) {
        url = QString("git@%1:%2/%3.git").arg(fragments[0], fragments[1], fragments[2]);
    } else {
        std::cerr << "    Module identity is wrong it should be registered name in alias or [github.com]/[owner]/[repo] style name." << std::endl;
    }
    return url;
}

int AliasDatabase::_levenshtein_distance(const QString &str1, const QString &str2) const
{
    const int len1 = str1.size();
    const int len2 = str2.size();

    QVector<int> col(len2 + 1);
    QVector<int> prevCol(len2 + 1);

    for (int i = 0; i < prevCol.size(); i++) {
        prevCol[i] = i;
    }
    for (int i = 0; i < len1; i++) {
        col[0] = i+1;
        for (int j = 0; j < len2; j++) {
            col[j+1] = std::min( std::min(prevCol[1 + j] + 1, col[j] + 1),
                                prevCol[j] + (str1[i]==str2[j] ? 0 : 1) );
        }
        col.swap(prevCol);
    }
    return prevCol[len2];
}


DatabaseEntry::DatabaseEntry(const QString& shortName, const QString& url, const QString& description)
    : _name(shortName), _url(url), _description(description), _invalid(false)
{
    QRegExp sshPattern(sshPatternSrc);
    QRegExp httpsPattern(httpsPatternSrc);
    QStringList result;
    if (sshPattern.indexIn(url) != -1) {
        result.append(sshPattern.cap(1));
        result.append(sshPattern.cap(2));
        result.append(sshPattern.cap(3));
    } else if (httpsPattern.indexIn(url) != -1) {
        result.append(httpsPattern.cap(1));
        result.append(httpsPattern.cap(2));
        auto repoName = httpsPattern.cap(3);
        if (repoName.endsWith(".git")) {
            repoName = repoName.left(repoName.count() - 4);
        }
        result.append(repoName);
    } else {
        this->_invalid = true;
    }
    if (!this->_invalid) {
        this->_longName = result.join("/");
    }
}

DatabaseEntry::DatabaseEntry()
{
    this->_invalid = true;
}

const QString &DatabaseEntry::name() const
{
    return this->_name;
}

const QString &DatabaseEntry::longName() const
{
    return _longName;
}

const QString& DatabaseEntry::url() const
{
    return _url;
}

const QString& DatabaseEntry::description() const
{
    return _description;
}

bool DatabaseEntry::invalid() const
{
    return _invalid;
}




