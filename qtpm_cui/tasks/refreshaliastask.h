#ifndef REFRESHALIASTASK_H
#define REFRESHALIASTASK_H

#include <QObject>

class ParameterParser;

class RefreshAliasTask : public QObject
{
    Q_OBJECT
public:
    explicit RefreshAliasTask(ParameterParser *param, QObject *parent = 0);

signals:

public slots:
    bool run();

private:
    ParameterParser* _param;
};

#endif // REFRESHALIASTASK_H
