#include "c3liniearization.h"

C3Liniearization::C3Liniearization()
{
}

// https://gist.github.com/ikame/1144867
C3LiniearizationResult C3Liniearization::linearize(QList<QStringList> &mroLists)
{
    C3LiniearizationResult result;

    while (true) {
        bool candidateFound = false;
        int count = mroLists.count();
        for (int i = 0; i < count; i++) {
            QStringList& mroList = mroLists[i];
            if (mroList.isEmpty()) {
                continue;
            }
            QString candidate = mroList.front();

            if (candidateFound) {
                if (result.modules.contains(candidate)) {
                    mroList.removeFirst();
                    continue;
                }
            }
            bool found = false;
            foreach (const QStringList& otherMroList, mroLists) {
                if (otherMroList.indexOf(candidate, 1) != -1) {
                    found = true;
                    break;
                }
            }
            if (found) {
                continue;
            } else {
                if (!result.modules.contains(candidate)) {
                    result.modules.append(candidate);
                }
                mroList.removeFirst();
                candidateFound = true;
            }
        }

        int sum = 0;
        foreach (const QStringList& mroList, mroLists) {
            sum += mroList.length();
        }
        if (sum == 0) {
            break;
        }
        if (!candidateFound) {
            foreach (const QStringList& mroList, mroLists) {
                foreach (const QString& module, mroList) {
                    result.errorModules.insert(module);
                }
            }
            break;
        }
    }
    return result;
}
