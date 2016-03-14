#include <QString>
#include <QObject>

QString greeting() {
    return QObject::tr("hello, Linux user");
}
