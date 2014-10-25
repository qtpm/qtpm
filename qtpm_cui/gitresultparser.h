#ifndef GITRESULTPARSER_H
#define GITRESULTPARSER_H

#include <QMap>
#include <QString>
#include <QByteArray>

struct GitRefs {
    QMap<QString, QByteArray> versions;
    QMap<QString, QByteArray> branches;
    QMap<QString, QByteArray> tags;
    QString head;
    bool isHeadTag;
    bool isHeadBranch;
    bool isHeadVersion;
    GitRefs() : isHeadTag(false), isHeadBranch(false), isHeadVersion(false) {}
};

class GitResultParser
{
private:
    GitResultParser();

public:
    static GitRefs parseReferenceList(QByteArray input);
};

#endif // GITRESULTPARSER_H
