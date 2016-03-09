#ifndef PACKAGE1_H
#define PACKAGE1_H

#include "package1_global.h"
#include <QObject>

class PACKAGE1SHARED_EXPORT Package1 : public QObject {
    Q_OBJECT
public:
    explicit Package1(QObject *parent = 0);
    QString  greeting() const;
signals:
public slots:
};

#endif // PACKAGE1_H

