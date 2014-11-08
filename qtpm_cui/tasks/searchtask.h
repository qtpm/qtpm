#ifndef SEARCHTASK_H
#define SEARCHTASK_H

#include <QObject>

class ParameterParser;

class SearchTask : public QObject
{
    Q_OBJECT
public:
    explicit SearchTask(ParameterParser *param, QObject *parent = 0);

public slots:
    void run();

protected:
    ParameterParser *_param;
};

#endif // SEARCHTASK_H
