#ifndef CONFIGTASK_H
#define CONFIGTASK_H

#include <QObject>

class ParameterParser;
class PlatformDatabase;

class ConfigTask : public QObject
{
    Q_OBJECT
public:
    explicit ConfigTask(ParameterParser *param, QObject *parent = 0);

signals:

public slots:
    void run();

private:
    ParameterParser* _param;

    void _showQtDir(PlatformDatabase& database) const;
    void _showPlatform(PlatformDatabase& database) const;
    void _showPlatformAlias(PlatformDatabase& database) const;
};

#endif // CONFIGTASK_H
