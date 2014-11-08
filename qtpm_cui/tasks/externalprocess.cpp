#include "externalprocess.h"
#include <iostream>
#include <QDebug>


ExternalProcess::ExternalProcess(bool dumpStdoutAndError, bool storeStdout, QObject *parent) :
    QObject(parent), _process(nullptr), _dumpStdoutAndError(dumpStdoutAndError), _storeStdout(storeStdout)
{
}

ExternalProcess::~ExternalProcess()
{
    this->_process->deleteLater();
}

void ExternalProcess::run(const QString &command, const QStringList &arguments)
{
    QDir current = QDir::current();
    this->run(command, arguments, current);
}

void ExternalProcess::run(const QString &command, const QStringList &arguments, const QDir &workdir)
{
    this->_process = new QProcess(this);
    this->_process->setProcessChannelMode(QProcess::MergedChannels);
    this->_process->setWorkingDirectory(workdir.absolutePath());
    connect(this->_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(_error(QProcess::ProcessError)));
    connect(this->_process, SIGNAL(readyReadStandardError()), this, SLOT(_readyReadStandardError()));
    connect(this->_process, SIGNAL(readyReadStandardOutput()), this, SLOT(_readyReadStandardOutput()));
    connect(this->_process, SIGNAL(finished(int)), this, SLOT(_finished(int)));
    this->_process->start(command, arguments);
}

int ExternalProcess::waitForFinished(int msec)
{
    return this->_process->waitForFinished(msec);
}

int ExternalProcess::exitCode() const
{
    return this->_process->exitCode();
}

QString ExternalProcess::stdoutResult() const
{
    return this->_stdout.join("");
}

void ExternalProcess::_readyReadStandardError()
{
    QString str = this->_process->readAllStandardError();
    emit onStderr(str);
    if (this->_dumpStdoutAndError) {
        std::cerr << str.toStdString();
    }
}

void ExternalProcess::_readyReadStandardOutput()
{
    QString str = this->_process->readAllStandardOutput();
    emit onStdout(str);
    if (this->_dumpStdoutAndError) {
        std::cout << str.toStdString();
    }
    if (this->_storeStdout) {
        this->_stdout.append(str);
    }
}

void ExternalProcess::_finished(int exitCode)
{
    emit finished(exitCode);
}

void ExternalProcess::_error(QProcess::ProcessError error)
{
    qDebug() << error;
}
