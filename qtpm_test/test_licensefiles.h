#ifndef TST_QPM_LICENSEFILES_H
#define TST_QPM_LICENSEFILES_H

#include <QObject>

class LicenseFilesTest : public QObject
{
    Q_OBJECT

public:
    LicenseFilesTest() {
    }

private Q_SLOTS:
    void licenseListTest();
    void licensePathTest();
    void licenseDisplayNameTest();
    void licenseFindTest();
    void searchLicenseByAliasTest();
};

#endif // TST_QPM_LICENSEFILES_H
