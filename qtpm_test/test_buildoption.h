#ifndef TEST_BUILDOPTION_H
#define TEST_BUILDOPTION_H

#include <QObject>

class BuildOptionTest : public QObject
{
    Q_OBJECT
public:
    explicit BuildOptionTest(QObject *parent = 0);

private Q_SLOTS:
    void parse_simple_options();
    void parse_quoted_options();
};

#endif // TEST_BUILDOPTION_H
