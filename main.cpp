#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>
#include <QtSql>

using namespace std;

#include "f_mainwindow.h"

int main(int argc, char *argv[])
{   
    // Caches la console si aucun argument n'est passé à Ludopen
    if(argc==1)
    {
        #ifdef __WIN32__
            ShowWindow(GetConsoleWindow(), SW_HIDE);
        #endif
    }
    QApplication a(argc, argv);
    // pour que tous les Qstring de Qt utilisent par défaut l'encodage UTF8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    F_MainWindow w;
    w.showMaximized();
    return a.exec();
}
