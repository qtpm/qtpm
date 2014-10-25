#ifndef QARCHIVE_H
#define QARCHIVE_H

#include <QByteArray>
#include <QDir>
#include <QMap>

class QArchive
{
public:
    QArchive();

    static bool extract(const QString& path, const QDir& dir);
    static bool extract(const QString& path, const QDir& dir, QStringList& resultFileList);
};

#endif // QARCHIVE_H
