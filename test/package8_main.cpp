#include<QApplication>
#include<QLabel>

QString greeting();

int main(int argc,char** argv){
    QApplication app(argc,argv);

    QLabel* l = new QLabel(greeting());
    l->show();

    return app.exec();
}
