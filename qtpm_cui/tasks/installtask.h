#ifndef INSTALLTASK_H
#define INSTALLTASK_H

#include <QObject>

class ParameterParser;

class InstallTask : public QObject
{
    Q_OBJECT
public:
    explicit InstallTask(ParameterParser *param, QObject *parent = 0);

signals:

public slots:
    void run();

private:
    ParameterParser* _param;
};

#endif // INSTALLTASK_H
