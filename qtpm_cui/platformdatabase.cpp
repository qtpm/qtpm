#include "platformdatabase.h"
#include "tasks/externalprocess.h"
#include <QDebug>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QSettings>

static const QString InstallDirTemplate = "deps.%1";

PlatformDatabase::PlatformDatabase(QObject *parent) :
    QObject(parent)
{
}

bool PlatformDatabase::resolveQtPath()
{
    QSettings setting(this->_settingPath(), QSettings::IniFormat);
    if (setting.contains("QTDIR")) {
        auto dir = QDir(setting.value("QTDIR").toString());
        if (this->isQtPathValid(dir)) {
            this->_qtpath = dir;
            this->_detectMethod = "(read from config)";
            return true;
        }
    }
    auto envs = QProcessEnvironment::systemEnvironment();
    if (envs.contains("QTDIR")) {
        auto dir = QDir(envs.value("QTDIR"));
        if (this->isQtPathValid(dir)) {
            this->_qtpath = dir;
            this->_detectMethod = "(detect by environment variable)";
            return true;
        }
    }
    QString qtdirPath;
    if (QDir::home().exists("Qt")) {
        qtdirPath = QDir::home().filePath("Qt");
    } else {
        QDir dir("C:\\Qt");
        if (dir.exists()) {
            qtdirPath = dir.path();
        }
    }
    QStringList ngPrefixes;
    // mobile oses can't build program by themselves.
    ngPrefixes << "android" << "blackberry" << "ios" << "maemo" << "wince" << "winphone" << "winrt";
    if (!qtdirPath.isEmpty()) {
        QDir qtDir(qtdirPath);
        auto ventries = qtDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
        while (!ventries.isEmpty()) {
            // version folder
            QFileInfo vinfo = ventries.last();
            QDir versionDir(vinfo.absoluteFilePath());
            auto pentries = versionDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
            while (!pentries.isEmpty()) {
                QFileInfo pinfo = pentries.last();
                QDir platformDir(pinfo.absoluteFilePath());
                if (this->isQtPathValid(platformDir)) {
                    QString basename = pinfo.baseName();
                    bool ng = false;
                    for (const QString& ngPrefix : ngPrefixes) {
                        if (basename.startsWith(ngPrefix)) {
                            ng = true;
                            break;
                        }
                    }
                    if (!ng) {
                        this->_qtpath = platformDir;
                        this->_detectMethod = "(autodetect)";
                        return true;
                    }
                }
                pentries.removeLast();
            }
            ventries.removeLast();

        }
    }
    return false;
}

bool PlatformDatabase::setQtPath(const QString &path, bool save)
{
    QDir dir(path);
    if (this->isQtPathValid(dir)) {
        this->_qtpath = dir;
        this->_detectMethod = "(set manually)";
        if (save) {
            QSettings setting(this->_settingPath(), QSettings::IniFormat);
            setting.setValue("QTDIR", dir.path());
            setting.sync();
        }
        return true;
    }
    return false;
}

bool PlatformDatabase::isQtPathValid(const QDir &dir) const
{
    return dir.exists("bin/qmake") || dir.exists("bin/qmake.exe");
}

QString PlatformDatabase::currentPlatform()
{
    if (this->_currentPlatform.isEmpty()) {
        this->_currentPlatform = this->_queryQMake(this->_qtpath.path(), "QMAKE_XSPEC");
    }
    return this->_currentPlatform;
}

QString PlatformDatabase::qtPath()
{
    return this->_qtpath.path();
}

QStringList PlatformDatabase::searchRelatedPlatforms(const QString &platform)
{
    if (this->_relatedPlatforms.contains(platform)) {
        return this->_relatedPlatforms[platform];
    }
    QStringList nextSpecs;
    QStringList platforms;

    QString platformStr = platform;
    if (platform.isEmpty()) {
        platformStr = this->_queryQMake(this->_qtpath.path(), "QMAKE_SPEC");
    }
    if (this->_qtpath.exists(QString("mkspecs/%1/qmake.conf").arg(platformStr))) {
        nextSpecs << this->_qtpath.filePath(QString("mkspecs/%1/qmake.conf").arg(platformStr));
    }

    while (!nextSpecs.isEmpty()) {
        QStringList newNextSpecs;
        for (auto nextSpec : nextSpecs) {
            this->_readQMakeConf(nextSpec, newNextSpecs, platforms);
        }
        nextSpecs = newNextSpecs;
    }

    this->_relatedPlatforms[platformStr] = platforms;
    return platforms;
}

QString PlatformDatabase::detectMethod() const
{
    return _detectMethod;
}

QDir PlatformDatabase::installDirectory()
{
    QDir currentDir = QDir::current();
    QString installDirName = InstallDirTemplate.arg(this->currentPlatform());
    if (!currentDir.exists(installDirName)) {
        currentDir.mkpath(installDirName);
    }
    return QDir(currentDir.filePath(installDirName));
}

void PlatformDatabase::_readQMakeConf(const QString &filepath, QStringList &nextSpecs, QStringList &platforms) const
{
    QFileInfo info(filepath);
    QDir dir(info.path());

    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw new std::logic_error("file creation error");
    }
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
       QString line = stream.readLine();
       if (line.startsWith("include(")) {
            line.chop(1);
            QFileInfo info(dir.filePath(line.mid(8)));
            nextSpecs << info.absoluteFilePath();
       } else if (line.startsWith("QMAKE_PLATFORM")) {
           for (QString platform : line.split("=")[1].split(" ")) {
               platform = platform.trimmed();
               if (!platform.isEmpty()) {
                   platforms << platform;
               }

           }
       }
    }
    file.close();
}

QString PlatformDatabase::_queryQMake(const QDir &dir, const QString &query) const
{
    ExternalProcess qmake(false, true);
    QString qmakePath;
    if (dir.exists("bin/qmake")) {
        qmakePath = dir.filePath("bin/qmake");
    } else {
        qmakePath = dir.filePath("bin/qmake.exe");
    }
    qmake.run(qmakePath, QStringList() << "-query" << query, QDir::home());
    qmake.waitForFinished(-1);
    return qmake.stdoutResult().split("\n")[0];
}

QString PlatformDatabase::_settingPath() const
{
    QDir home = QDir::home();
    if (!home.exists(".qtpm")) {
        home.mkdir(".qtpm");
    }
    QDir qtpmDir = QDir(home.filePath(".qtpm"));
    return qtpmDir.filePath("qtpm_config.ini");
}
