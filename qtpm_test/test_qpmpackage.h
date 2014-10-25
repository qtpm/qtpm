#ifndef TST_QPM_QPMPACKAGE_H
#define TST_QPM_QPMPACKAGE_H

#include <QObject>
class QpmPackage;
class QDir;

class QPMPackageTest : public QObject
{
    Q_OBJECT

public:
    explicit QPMPackageTest(QObject *parent = 0);

signals:

private Q_SLOTS:
    void initializeTest();
    void existanceTest1();
    void existanceTest2();
    void searchTest();
    void searchTest2();

    void addBundlePackageTest1();
    void addBundlePackageTest2();
    void updateBundlePackageTest1();
    void updateBundlePackageTest2();

    void addDevelopmentPackageTest1();
    void addDevelopmentPackageTest2();
    void updateDevelopmentPackageTest1();
    void updateDevelopmentPackageTest2();

    void duplicatePackageDetectionTest1();
    void duplicatePackageDetectionTest2();
};

#endif // TST_QPM_QPMPACKAGE_H
