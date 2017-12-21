#include "majeur.h"
#include "acces_fichier_http.h"
#include "fonctions_globale.h"
#include "f_preferences.h"
#include <QtNetwork>

MAJeur::MAJeur(QWidget *parent)
{
// Si mode debug, on sort
#ifndef QT_NO_DEBUG
// return;
#endif
    sURLMAJeur= F_Preferences::ObtenirValeur("URLMAJeur");
    QString CheminDestination=QDir::tempPath()+"/derniereVersion.ini";
    AccesFichierParHTTP manager;
    manager.TelechargementSynchrone(sURLMAJeur + "/derniereVersion.ini",CheminDestination);
    QSettings DerniereVersionINI(CheminDestination, QSettings::IniFormat);
    double VersionMAJeur=DerniereVersionINI.value("Version/derniereVersion", "derniereVersion").toDouble();
    double VersionLudopen=QString::fromLocal8Bit(VER).toDouble();
    double VersionBDD = F_Preferences::ObtenirValeur("Version").toDouble();
    qDebug()<<"VersionMAJeur :"<<VersionMAJeur;
    qDebug()<<"VersionLudopen :"<<VersionLudopen;
    qDebug()<<"VersionBDD :"<<VersionBDD;

    int DiffVersion=(VersionMAJeur-VersionLudopen)*100;
    // Si on a la même version que la BDD, la mise à jour n'est pas nécessaire,
    // et si il y a une nouvelle version,
    // on pose la question à l'utilisateur de savoir si il veut la mise à jour de suite
    int index=QCoreApplication::arguments().indexOf(QRegExp("-MAJ"));
    if(VersionBDD==VersionLudopen && VersionLudopen<VersionMAJeur &&
            QCoreApplication::arguments().indexOf(QRegExp("-MAJ")) == -1)
    {
        QString Message;
        if(DiffVersion>1)
        {
            Message="Il y a "+ QString::number(DiffVersion)+ " mises à jour. Souhaitez-vous les installer maintenant ?";
        }
        else
        {
            Message="Souhaitez-vous mettre à jour Ludopen maintenant ?";
        }
        // Si l'utilisateur réponds non, on sort de la fonction, et on continue l'exécution de Ludopen
        if(QMessageBox::question(parent, "Mise à jour", Message, "Oui", "Non") != 0)
        {
           return;
        }
    }

    QString TypeOS;
    // OS is not Windows
    #ifdef Q_OS_WIN32
        TypeOS="windows";
    #else
        TypeOS="linux";
    #endif
    if(VersionLudopen<VersionMAJeur)
    {
        QString CheminMAJeurIni=QDir::tempPath()+"/MAJeur.ini";
        // On calcule l'URL de la prochaine version pour récupérer MAJeur.ini
        QString URL=sURLMAJeur + QString::number(VersionLudopen + 0.01,'f',2) + "/" + TypeOS + "/MAJeur.ini";
        manager.TelechargementSynchrone(URL,CheminMAJeurIni);
        QSettings MAJeurINI(CheminMAJeurIni, QSettings::IniFormat);
        MAJeurINI.beginGroup("Fichiers");
        // On filtre les chaines qui n'ont pas #SCRIPT suivi d'un nombre
        QStringList cles = MAJeurINI.childKeys().filter(QRegExp("^(?!#SCRIPT[0-9]+)"));
        foreach (QString cle, cles)
        {
            URL=sURLMAJeur + "/" + QString::number(VersionLudopen + 0.01,'f',2)+ "/" + TypeOS + "/" + cle;
            CheminDestination=qApp->applicationDirPath() + QDir::separator () + cle;
            manager.TelechargementSynchrone(URL,CheminDestination);
        }
        cles = MAJeurINI.childKeys().filter(QRegExp("^#SCRIPT[0-9]+"));
        QProcess *process=new QProcess();
        process->setWorkingDirectory(qApp->applicationDirPath());
        foreach (QString cle, cles)
        {
            QString valeur=MAJeurINI.value(cle).toString();
            URL=sURLMAJeur + QString::number(VersionLudopen + 0.01,'f',2)+ "/" + TypeOS + "/" + valeur;
            CheminDestination=qApp->applicationDirPath() + QDir::separator () + valeur;
            manager.TelechargementSynchrone(URL,CheminDestination);
            QString CheminBash="";
            if(TypeOS=="windows")
            {
                CheminBash="bash\\";
            }
            process->start(CheminBash + "bash " + CheminDestination);
            if(!process->waitForFinished())
            {
                qDebug()<<"MAJeur: "<<process->errorString()<<" "<<process->error();
            }
            else
            {
                qDebug()<<"MAJeur:process->readAllStandardOutput: "<<process->readAllStandardOutput();
                qDebug()<<"MAJeur:process->readAllStandardError: "<<process->readAllStandardError();
            }
        }
        if(TypeOS=="windows")
        {
            URL=sURLMAJeur + "/" + QString::number(VersionLudopen + 0.01,'f',2)+ "/" + TypeOS + "/LudOpen.exe";
            CheminDestination=QDir::tempPath() + QDir::separator () + "LudOpen.exe";
            manager.TelechargementSynchrone(URL,CheminDestination);
            process->startDetached("bash\\bash.exe",QStringList("relancerLudopen.sh"));
            //qApp->quit();
        }
        else
        {
            URL=sURLMAJeur + "/" + QString::number(VersionLudopen + 0.01,'f',2)+ "/" + TypeOS + "/LudOpen";
            CheminDestination=qApp->applicationDirPath() + "/LudOpen";
            manager.TelechargementSynchrone(URL,CheminDestination);
            process->start("chmod +x " + CheminDestination);
            process->waitForFinished();
            process->startDetached(qApp->applicationDirPath() + "/LudOpen",QStringList("-MAJ"));
            process->waitForStarted();
        }
        exit(0);
        return;
    }
    if(VersionBDD < VersionMAJeur)
    {
        double VersionEnCours;
        VersionEnCours=VersionBDD;
        do
        {
            VersionEnCours+=0.01;
            QString URL=sURLMAJeur + QString::number(VersionEnCours,'f',2) + "/" + TypeOS + "/BDD.sql";
            CheminDestination=QDir::tempPath()+"/BDD.sql";
            manager.TelechargementSynchrone(URL,CheminDestination);
            ExecuterScriptSql(CheminDestination);
        } while(VersionEnCours < VersionMAJeur);

        QSqlQuery Requete ;

        Requete.prepare( "UPDATE preferences SET Valeur=? WHERE NomChamps=?" ) ;
        QVariantList valeurs;
        valeurs << VersionMAJeur;
        Requete.addBindValue(valeurs);

        QVariantList nomChamps;
        nomChamps << "Version";
        Requete.addBindValue(nomChamps);

        if( ! Requete.execBatch() )
        {
            qDebug() << " Erreur : MAJeur::MAJeur() : " << Requete.lastQuery() << Requete.lastError();
        }

    }

}

