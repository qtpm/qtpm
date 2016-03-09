#include "package2.h"
#include <package1/package1.h>

Package2::Package2(QObject *parent) : QObject(parent)
{
}

QString Package2::relay() const {
    Package1 package1;
    return package1.greeting();
}

QString Package2::greeting() const {
    return "good morning from package2";
}

#include "moc_package2.cpp"
