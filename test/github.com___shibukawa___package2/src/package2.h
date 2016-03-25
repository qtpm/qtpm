#ifndef PACKAGE2_H
#define PACKAGE2_H

#include <QObject>

class Package2 : public QObject {
    Q_OBJECT
public:
    explicit Package2(QObject *parent = 0);
    QString relay() const;
    QString greeting() const;
signals:
public slots:
};

#endif // PACKAGE2_H

