#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <iostream>
#include <QStyleFactory>
#include "darkpalette.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;
    parser.process(a);
    QStringList args = parser.positionalArguments();
    qApp->setStyle(QStyleFactory::create("Universal"));


    QString ARG;
    for(const auto &i : args){
        ARG = i;
        break;
    }
    std::cout << "argv[]: " << ARG.toStdString() << "\n";
    MainWindow w(ARG);
    w.show();
    return a.exec();
}
