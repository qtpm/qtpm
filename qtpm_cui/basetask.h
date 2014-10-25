#ifndef BASETASK_H
#define BASETASK_H

#include <QObject>

class BaseTask : public QObject
{
    Q_OBJECT
public:
    explicit BaseTask(QObject *parent = 0);

signals:
    void finished();

public slots:
    virtual void run() = 0;
};

#endif // BASETASK_H
