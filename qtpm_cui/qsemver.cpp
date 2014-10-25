#include "qsemver.h"
#include "semver/semver.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <QList>


struct CompareSemver
{
    bool operator()(const Spec* a, const Spec* b) const
    {
        return *a < *b;
    }
};

struct CompareSemverReverse
{
    bool operator()(const Spec* a, const Spec* b) const
    {
        return *b < *a;
    }
};

QSemVer::QSemVer()
{
}

QStringList QSemVer::filter(const QString &rangePattern, const QStringList& specStrings, bool reverse)
{
    QList<Spec*> specs;
    auto range = new Range(rangePattern.toLatin1().constData());

    foreach(const QString& specString, specStrings) {
        auto spec = new Spec(specString.toLatin1().constData());
        if (spec->invalid()) {
            delete spec;
            continue;
        }
        if (!range->compare(spec)) {
            delete spec;
            continue;
        }
        specs.append(spec);
    }

    QStringList result;
    if (!specs.isEmpty()) {
        if (reverse) {
            qSort(specs.begin(), specs.end(), CompareSemverReverse());
        } else {
            qSort(specs.begin(), specs.end(), CompareSemver());
        }
        for (int i = 0; i < specs.length(); i++) {
            result << QString(specs[i]->toString().c_str());
        }
    }
    delete range;
    qDeleteAll(specs);
    return result;
}

QStringList QSemVer::filter(const QStringList &rangeStrings, const QStringList &specStrings, bool reverse)
{
    QList<Spec*> specs;
    QList<Range*> ranges;
    foreach(const QString& rangePattern, rangeStrings) {
        auto range = new Range(rangePattern.toLatin1().constData());
        if (range->invalid()) {
            delete range;
            continue;
        }
        ranges.append(range);
    }

    foreach(const QString& specString, specStrings) {
        auto spec = new Spec(specString.toLatin1().constData());
        if (spec->invalid()) {
            delete spec;
            continue;
        }
        foreach (auto range, ranges) {
            if (!range->compare(spec)) {
                delete spec;
                spec = nullptr;
                break;
            }
        }
        if (spec) {
            specs.append(spec);
        }
    }

    QStringList result;
    if (!specs.isEmpty()) {
        if (reverse) {
            qSort(specs.begin(), specs.end(), CompareSemverReverse());
        } else {
            qSort(specs.begin(), specs.end(), CompareSemver());
        }
        for (int i = 0; i < specs.length(); i++) {
            result << QString(specs[i]->toString().c_str());
        }
    }
    qDeleteAll(ranges);
    qDeleteAll(specs);
    return result;
}
