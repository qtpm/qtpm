#ifndef CLEANTASK_H
#define CLEANTASK_H

#include <QObject>
#include <QDir>

class CleanTask : public QObject
{
    Q_OBJECT
public:
    explicit CleanTask(const QDir& dir, bool verbose, QObject *parent = 0);

signals:

public slots:
    void run();

private:
    QDir _dir;
    bool _verbose;
};

#endif // CLEANTASK_H
