#include "qarchive.h"
#include "archive.h"
#include "archive_entry.h"
#include <QDebug>
#include <QFile>
#include <QByteArray>

static int  copy_data(struct archive *, struct archive *);

QArchive::QArchive()
{
}

bool QArchive::extract(const QString &path, const QDir &dir)
{
    QStringList dummyList;
    return QArchive::extract(path, dir, dummyList);
}

bool QArchive::extract(const QString &path, const QDir &dir, QStringList &resultFileList)
{
    //qDebug() << "untar" << path.toStdString() << dir.path().toStdString();
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int r;
    int flags = ARCHIVE_EXTRACT_TIME;
    a = archive_read_new();
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QByteArray blob = file.readAll();

    /*
     * Note: archive_write_disk_set_standard_lookup() is useful
     * here, but it requires library routines that can add 500k or
     * more to a static executable.
     */
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    /*
     * On my system, enabling other archive formats adds 20k-30k
     * each.  Enabling gzip decompression adds about 20k.
     * Enabling bzip2 is more expensive because the libbz2 library
     * isn't very well factored.
     */
    if ((r = archive_read_open_memory(a, reinterpret_cast<void*>(blob.data()), blob.size()))) {
        qDebug() << "archive_read_open_memory():" << archive_error_string(a);
        return false;
    }
    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r != ARCHIVE_OK) {
            qDebug() << "archive_read_next_header():" << archive_error_string(a);
            return false;
        }
        const char* path = archive_entry_pathname(entry);
        QString newRelPath(path);
        resultFileList << newRelPath;
        QString newPath = dir.filePath(newRelPath);
        archive_entry_set_pathname(entry, newPath.toLocal8Bit().constData());
        r = archive_write_header(ext, entry);
        if (r != ARCHIVE_OK) {
            qDebug() << "archive_write_header():" << archive_error_string(ext);
        } else {
            copy_data(a, ext);
            r = archive_write_finish_entry(ext);
            if (r != ARCHIVE_OK) {
                qDebug() << "archive_write_finish_entry():" << archive_error_string(ext);
                return false;
            }
        }
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    return true;
}

static int copy_data(struct archive *ar, struct archive *aw) {
    int r;
    const void *buff;
    size_t size;
#if ARCHIVE_VERSION >= 3000000
    int64_t offset;
#else
    off_t offset;
#endif

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r != ARCHIVE_OK)
            return (r);
        r = archive_write_data_block(aw, buff, size, offset);
        if (r != ARCHIVE_OK) {
            qDebug() << "archive_write_data_block():" << archive_error_string(aw);
            return (r);
        }
    }
}
