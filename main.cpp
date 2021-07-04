#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>
#include <QtSql>

using namespace std;

#include "f_mainwindow.h"
#include <qapplication.h>
#include <stdio.h>
#include <stdlib.h>

/*#include <ods/ods>
#include <ods/err.hpp>
#include <ods/inst/SvgDesc.hpp>
#include <ods/inst/SvgTitle.hpp>
#include <ods/inst/NumberBooleanStyle.hpp>
#include <ods/inst/NumberDay.hpp>
#include <ods/inst/NumberHours.hpp>
#include <ods/inst/NumberMinutes.hpp>
#include <ods/inst/NumberMonth.hpp>
#include <ods/inst/NumberSeconds.hpp>
#include <ods/inst/NumberText.hpp>
#include <ods/inst/NumberYear.hpp>*/

// Fonction de redirection des messages de déboggage dans un fichier
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "MESSAGE (%s:%u %s): %s\n", context.file, context.line, context.function, localMsg.constData());
    fflush(stderr);
    Q_UNUSED(context);

    QString dt = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    QString txt = QString("[%1] ").arg(dt);

    switch (type)
    {
      case QtDebugMsg:
         txt += QString("{Debug} \t\t %1").arg(msg);
         break;
      case QtWarningMsg:
         txt += QString("{Warning} \t %1").arg(msg);
         break;
      case QtCriticalMsg:
         txt += QString("{Critical} \t %1").arg(msg);
         break;
      case QtFatalMsg:
         txt += QString("{Fatal} \t\t %1").arg(msg);
         abort();
         break;
    }

    QFile outFile("Ludopen.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream textStream(&outFile);
    textStream << txt << endl;
}

int main(int argc, char *argv[])
{   
    // Handler pour rediriger les messages de déboggage dans un fichier
    qInstallMessageHandler(myMessageOutput);
    QApplication a(argc, argv);
    // pour que tous les Qstring de Qt utilisent par défaut l'encodage UTF8
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    F_MainWindow w;
    w.showMaximized();
    a.exec();
}
