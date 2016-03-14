#include <QApplication>
#include <QTranslator>
#include <QLabel>

QString greeting();

int main(int argc,char** argv){
    QApplication app(argc,argv);
    QTranslator translator;
    if (translator.load(QLocale::system(), "workbench8", "_", ":/translations")) {
        app.installTranslator(&translator);
    }

    QLabel* l = new QLabel(greeting());
    l->show();

    return app.exec();
}
