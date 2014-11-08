#ifndef ALIASDATABASE_H
#define ALIASDATABASE_H

#include <QObject>
#include <QDir>
#include <QSettings>

class DatabaseEntry
{
public:
    explicit DatabaseEntry(const QString& shortName, const QString& url, const QString& description);
    explicit DatabaseEntry();
    const QString& name() const;
    const QString& longName() const;
    const QString& url() const;
    const QString& description() const;
    bool invalid() const;

private:
    QString _name;
    QString _longName;
    QString _url;
    QString _description;
    bool _invalid;
};

class AliasDatabase : public QObject
{
    Q_OBJECT
public:
    explicit AliasDatabase(const QDir& dir, QObject *parent = 0);
    ~AliasDatabase();

    void read();
    void refresh();
    void refresh(const QString& content);
    int count() const;
    bool contains(const QString& key) const;
    DatabaseEntry find(const QString& key) const;
    QList<DatabaseEntry> search(const QString& key) const;
    QList<DatabaseEntry> search_near_title(const QString& key, int distance=4) const;

signals:

public slots:
private:
    QSettings* _settings;

    int _levenshtein_distance(const QString& str1, const QString& str2) const;

};

#endif // ALIASDATABASE_H
