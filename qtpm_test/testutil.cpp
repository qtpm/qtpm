#include "testutil.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <stdexcept>

TestUtil::TestUtil()
{
}

TestUtil::~TestUtil()
{
    if (!this->_testDir.isEmpty() and !this->_skipRemove) {
        QDir tempdir(this->_testDir);
        tempdir.removeRecursively();
    }
}

QDir TestUtil::makeTestDir(bool skipRemove)
{
    this->_skipRemove = skipRemove;
    QDir temp = QDir::temp();
    QString dirname = QString::number(QDateTime::currentMSecsSinceEpoch());
    temp.mkdir(dirname);
    this->_testDir = temp.filePath(dirname);
    if (skipRemove) {
        qDebug() << "test dir:" << this->_testDir;
    }
    return QDir(this->_testDir);
}

void TestUtil::makeFile(const QString filename, const QString content)
{
    if (this->_testDir.isEmpty()) {
        throw new std::logic_error("temp dir is not initialized");
    }
    QDir tempdir(this->_testDir);
    QString filepath = tempdir.filePath(filename);
    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw new std::logic_error("file creation error");
    }
    QTextStream stream(&file);
    stream << content << endl;
    file.close();
}
