#include "extracttask.h"
#include "qpmpackage.h"
#include "qarchive.h"
#include <QDir>
#include <QDateTime>
#include <QScopedPointer>
#include <iostream>


ExtractTask::ExtractTask()
{
}

ExtractTask::~ExtractTask()
{
}

bool ExtractTask::run(const QString &path, const QDir &destDir, QString &resultingDirName, bool verbose)
{
    QDir temp = QDir::temp();
    QString dirname = QString::number(QDateTime::currentMSecsSinceEpoch());
    temp.mkdir(dirname);
    QDir tempDir(temp.filePath(dirname));
    QScopedPointer<QpmPackage> package(nullptr);

    QStringList resultFiles;

    if (verbose) {
        std::cout << "    extracting... " << path.toStdString() << std::endl;
    }
    bool result = QArchive::extract(path, tempDir, resultFiles);

    if (result) {
        if (resultFiles.length() == 0) {
            if (verbose) {
                std::cout << "    this is empty archive file." << std::endl;
            }
            result = false;
        } else {
            QStringList fragments = QDir::fromNativeSeparators(resultFiles[0]).split("/");
            if (fragments.length() == 0) {
                result = false;
            } else {
                QDir sourceDir(tempDir.filePath(fragments[0]));
                package.reset(new QpmPackage(sourceDir));
                if (destDir.exists(package->name())) {
                    if (!QDir(destDir.filePath(package->name())).removeRecursively()) {
                        result = false;
                        if (!verbose) {
                            std::cerr << "    extracting... " << path.toStdString() << std::endl;
                        }
                        std::cerr << "    removing existing folder failed." << std::endl;
                    }
                }
            }
            if (result) {
                QDir dir;
                resultingDirName = fragments[0];
                dir.rename(tempDir.filePath(resultingDirName), destDir.filePath(package->name()));
            }
        }
    }
    tempDir.removeRecursively();
    return result;
}

