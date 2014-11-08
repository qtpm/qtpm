#ifndef PLATFORMDATABASE_H
#define PLATFORMDATABASE_H

#include <QObject>
#include <QDir>
#include <QMap>

class QSettings;

class PlatformDatabase : public QObject
{
    Q_OBJECT
public:
    explicit PlatformDatabase(QObject *parent = 0);

    bool resolveQtPath();
    bool setQtPath(const QString& path, bool save);
    bool isQtPathValid(const QDir &dir) const;

    QString currentPlatform();
    QString qtPath();
    QStringList searchRelatedPlatforms(const QString& platform="");
    QString detectMethod() const;
    QDir installDirectory();

signals:

public slots:

private:
    QString _detectMethod;
    QDir _qtpath;
    QString _currentPlatform;
    QMap<QString, QStringList> _relatedPlatforms;

    void _readQMakeConf(const QString& filepath, QStringList& nextSpecs, QStringList& platforms) const;
    QString _queryQMake(const QDir &dir, const QString& query) const;
    QString _settingPath() const;
};

#endif // PLATFORMDATABASE_H
