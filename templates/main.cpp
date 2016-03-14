#include <QApplication>
#include <QTranslator>
#include <QLabel>

int main(int argc,char** argv){
    QApplication app(argc,argv);
    QTranslator translator;
    if (translator.load(QLocale::system(), "[[.TargetSmall]]", "_", ":/translations")) {
        app.installTranslator(&translator);
    }

    QLabel* l = new QLabel("hello world");
    l->show();

    return app.exec();
}