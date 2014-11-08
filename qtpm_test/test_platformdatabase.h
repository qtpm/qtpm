#ifndef TEST_PLATFORMDATABASE_H
#define TEST_PLATFORMDATABASE_H

#include <QObject>

class PlatformDatabase;

class PlatformDatabaseTest : public QObject
{
    Q_OBJECT
public:
    explicit PlatformDatabaseTest(QObject *parent = 0);

private Q_SLOTS:
    void init();
    void currentPlatformTest();
    void searchRelatedPlatformTest();
    void cleanup();

private:
    PlatformDatabase* _platform;
};

#endif // TEST_PLATFORMDATABASE_H
