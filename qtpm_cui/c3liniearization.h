#ifndef C3LINIEARIZATION_H
#define C3LINIEARIZATION_H

#include <QStringList>
#include <QList>
#include <QSet>

class Module;

struct C3LiniearizationResult {
    QStringList modules;
    QSet<QString> errorModules;
    bool hasError() {
        return errorModules.count();
    }
};

class C3Liniearization
{
private:
    C3Liniearization();
public:
    static C3LiniearizationResult linearize(QList<QStringList> &mroList);
};

#endif // C3LINIEARIZATION_H
