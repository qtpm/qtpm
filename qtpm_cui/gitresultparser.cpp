#include "gitresultparser.h"
#include "semver/semver.h"
#include <QDebug>

GitResultParser::GitResultParser()
{
}

GitRefs GitResultParser::parseReferenceList(QByteArray input)
{
    GitRefs result;
    QByteArray headHash;
    QMap<QByteArray, QString> hash2ref;
    foreach (const QByteArray& line, input.split('\n')) {
        QList<QByteArray> fragments = line.split('\t');
        //qDebug() << fragments;
        if (fragments.length() != 2) {
            continue;
        }
        QString referenceName(fragments[1]);
        if (referenceName == "HEAD") {
            headHash = fragments[0];
        } else {
            hash2ref.insert(fragments[0], referenceName);
            if (referenceName.startsWith("refs/tags/v")) {
                Spec* semver = new Spec(referenceName.mid(11).toLatin1().constData());
                if (semver->invalid()) {
                    result.tags.insert(referenceName.mid(10), fragments[0]);
                } else {
                    result.versions.insert(referenceName.mid(11), fragments[0]);
                }
                delete semver;
            } else if (referenceName.startsWith("refs/tags/")) {
                result.tags.insert(referenceName.mid(10), fragments[0]);
            } else if (referenceName.startsWith("refs/heads/")) {
                result.branches.insert(referenceName.mid(11), fragments[0]);
            }
        }
    }
    QString headReferenceName = hash2ref.value(headHash);
    if (result.versions.contains(headReferenceName.mid(11))) {
        result.head = headReferenceName.mid(11);
        result.isHeadVersion = true;
    } else if (result.branches.contains(headReferenceName.mid(11))) {
        result.head = headReferenceName.mid(11);
        result.isHeadBranch = true;
    } else {
        result.head = headReferenceName.mid(10);
        result.isHeadTag = true;
    }
    return result;
}
