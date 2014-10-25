#ifndef INITAPPTASK_H
#define INITAPPTASK_H

#include <QObject>

class ParameterParser;

class InitAppTask : public QObject
{
    Q_OBJECT
public:
    explicit InitAppTask(ParameterParser *param, QObject *parent = 0);

signals:

public slots:
    void run();

private:
    ParameterParser* _param;
};

#endif // INITAPPTASK_H
