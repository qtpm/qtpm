#ifndef TEST_RESULTHASH_H
#define TEST_RESULTHASH_H

#include <QObject>

class ResultHashTest : public QObject
{
    Q_OBJECT
public:
    explicit ResultHashTest(QObject *parent = 0);

private Q_SLOTS:
    void hashTest();
};

#endif // TEST_RESULTHASH_H
