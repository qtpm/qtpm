#ifndef BUILDTASK_H
#define BUILDTASK_H

#include <QStringList>
#include <QObject>
#include <QDir>

class QpmPackage;
class ParameterParser;
class PlatformDatabase;

class BuildTask : public QObject
{
    Q_OBJECT
public:
    explicit BuildTask(const QDir& package, bool verbose, ParameterParser *param = 0, PlatformDatabase* database = 0, QDir* destination = 0, QObject *parent = 0);
    QStringList parseBuildOption(const QString& option);
signals:

public slots:
    void run();

protected:
    QDir _packageDir;
    QDir *_destinationDir;
    ParameterParser* _param;
    bool _verbose;
    PlatformDatabase* _database;

    bool _buildByCMake(const QString& name, const QDir& srcDir, const QString& buildOptionString);
    bool _buildByConfigure(const QDir& srcDir, const QString& buildOptionString);
    void _copyPriFile(QpmPackage *package);
};

#endif // BUILDTASK_H
