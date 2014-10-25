#ifndef INSTALLWORKER_H
#define INSTALLWORKER_H

#include <QObject>

class InstallWorker : public QObject
{
    Q_OBJECT
public:
    explicit InstallWorker(QObject *parent = 0);

signals:

public slots:

};

#endif // INSTALLWORKER_H
