#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>
#include <QtSql>

using namespace std;

#include "f_mainwindow.h"

int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);

    // pour que tous les Qstring de Qt utilisent par défaut l'encodage UTF8
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    F_MainWindow w;
    w.showMaximized();
    return a.exec();
}
