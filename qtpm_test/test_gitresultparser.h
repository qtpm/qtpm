#ifndef TEST_GITRESULTPARSER_H
#define TEST_GITRESULTPARSER_H

#include <QObject>

class GitResultParserTest : public QObject
{
    Q_OBJECT
public:
    explicit GitResultParserTest(QObject *parent = 0);

private Q_SLOTS:
    void parseReferenceListTest();
};

#endif // TEST_GITRESULTPARSER_H
