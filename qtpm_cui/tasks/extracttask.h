#ifndef EXTRACTTASK_H
#define EXTRACTTASK_H

#include <QString>
#include <QDir>

class ExtractTask
{
public:
    ExtractTask();
    virtual ~ExtractTask();

    bool run(const QString& path, const QDir& destDir, QString& resultingDirName);
};

#endif // EXTRACTTASK_H
