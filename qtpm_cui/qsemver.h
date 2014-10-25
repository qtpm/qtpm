#ifndef QSEMVER_H
#define QSEMVER_H

#include <QStringList>

class QSemVer
{
private:
    QSemVer();

public:
    static QStringList filter(const QString& range, const QStringList &semvers, bool reverse=false);
    static QStringList filter(const QStringList& ranges, const QStringList &semvers, bool reverse=false);
};

#endif // QSEMVER_H
