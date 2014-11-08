#ifndef PROJECTFILETASK_H
#define PROJECTFILETASK_H

#include <QDir>
#include <QObject>
#include <QMap>
#include <QStringList>

class ProjectFileTask : public QObject
{
    Q_OBJECT
public:
    explicit ProjectFileTask(const QDir& current, QObject *parent = 0);

    bool hasEnv(const QString& envName) const;
    bool hasLib(const QString& envName, const QString& libName) const;
    QString dump() const;
    void write() const;
signals:

public slots:

private:
    QDir _current;
    QMap<QString, QStringList> _envs;
};

#endif // PROJECTFILETASK_H
