#ifndef TESTUTIL_H
#define TESTUTIL_H

#include <QString>
#include <QDir>

class TestUtil
{
public:
    TestUtil();
    ~TestUtil();

    QDir makeTestDir(bool skipRemove=false);
    void makeFile(const QString filename, const QString content);
    QString readFile(const QString& filename);

private:
    QString _testDir;
    bool _skipRemove;
};

#endif // TESTUTIL_H
