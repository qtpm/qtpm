#ifndef INITLIBTASK_H
#define INITLIBTASK_H

#include <QObject>

class ParameterParser;

class InitLibTask : public QObject
{
    Q_OBJECT
public:
    explicit InitLibTask(ParameterParser *param, QObject *parent = 0);

signals:

public slots:
    void run();

private:
    ParameterParser* _param;
};

#endif // INITLIBTASK_H
