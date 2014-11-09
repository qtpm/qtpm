#ifndef CLEANTASK_H
#define CLEANTASK_H

#include <QObject>
#include <QDir>

class CleanTask : public QObject
{
    Q_OBJECT
public:
    explicit CleanTask(const QDir& dir, QObject *parent = 0);

signals:

public slots:
    void run();

private:
    QDir _dir;
};

#endif // CLEANTASK_H
