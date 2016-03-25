#include "package1.h"

Package1::Package1(QObject *parent) : QObject(parent)
{
}

QString Package1::greeting() const {
    return "hello from package1";
}

#include "moc_package1.cpp"
