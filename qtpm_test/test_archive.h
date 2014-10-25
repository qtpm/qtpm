#ifndef TEST_UNTAR_H
#define TEST_UNTAR_H

#include <QObject>

class ArchiveTest : public QObject
{
    Q_OBJECT
public:
    explicit ArchiveTest(QObject *parent = 0);

private Q_SLOTS:
    void extract_tar();
    void extract_tar_gz();
    void extract_tar_bz2();
    void extract_tar_xz();
    void extract_tar_lzma();
    void extract_zip();
};

#endif // TEST_UNTAR_H
