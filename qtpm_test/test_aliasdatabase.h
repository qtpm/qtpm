#ifndef TEST_ALIASDATABASE_H
#define TEST_ALIASDATABASE_H

#include <QObject>

class AliasDatabaseTest : public QObject
{
    Q_OBJECT
public:
    explicit AliasDatabaseTest(QObject *parent = 0);

private Q_SLOTS:
    void parse_wiki_contents_test();
    void parse_wiki_contents_with_ssh_url_test();
    void read_write_test();
    void search_test();
    void search_near_title_test();
};

#endif // TEST_ALIASDATABASE_H
