#ifndef EXTERNALPROCESS_H
#define EXTERNALPROCESS_H

#include <QObject>
#include <QProcess>
#include <QDir>

class ExternalProcess : public QObject
{
    Q_OBJECT
public:
    explicit ExternalProcess(bool dumpStdoutAndError, QObject *parent = 0);
    ~ExternalProcess();

    void run(const QString& command, const QStringList& arguments);
    void run(const QString& command, const QStringList& arguments, const QDir& workdir);
    int waitForFinished(int msec=30000);
    int exitCode() const;

signals:
    void finished(int exitCode);
    void onStdout(QString text);
    void onStderr(QString text);

protected slots:
    void _readyReadStandardError();
    void _readyReadStandardOutput();
    void _finished(int exitCode);
    void _error(QProcess::ProcessError error);

protected:
    QProcess* _process;
    bool _dumpStdoutAndError;
};

#endif // EXTERNALPROCESS_H
