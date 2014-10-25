#ifndef BUILDTASK_H
#define BUILDTASK_H

#include <QStringList>
#include <QObject>
#include <QDir>

class ParameterParser;

class BuildTask : public QObject
{
    Q_OBJECT
public:
    explicit BuildTask(const QDir& package, const QDir& destination, ParameterParser *param, QObject *parent = 0);
    QStringList parseBuildOption(const QString& option);
signals:

public slots:
    void run();

protected:
    QDir _packageDir;
    QDir _destinationDir;
    ParameterParser* _param;

    bool _buildByCMake(const QDir& srcDir, const QString& buildOptionString);
    bool _buildByConfigure(const QDir& srcDir, const QString& buildOptionString);
};

#endif // BUILDTASK_H
