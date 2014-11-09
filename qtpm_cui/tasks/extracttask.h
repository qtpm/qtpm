#ifndef EXTRACTTASK_H
#define EXTRACTTASK_H

#include <QString>
#include <QDir>

class ExtractTask
{
public:
    ExtractTask();
    virtual ~ExtractTask();

    bool run(const QString& path, const QDir& destDir, QString& resultingDirName, bool verbose=false);
};

#endif // EXTRACTTASK_H
