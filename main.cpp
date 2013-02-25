#include <QtGui/QApplication>
#include <QtCore/QCoreApplication>
#include <QtSql>
#include <iostream>
using namespace std;

#include "f_mainwindow.h"

int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);


    QString sNomBDD;
    QString sAdresseIP;
    QString sNomUtilisateur;
    QString sMotDePasse;
    unsigned int nPort;

    // pour que tous les Qstring de Qt utilisent par défaut l'encodage UTF8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    //! Ouverture du fichier INI.
    QSettings settings("config.ini", QSettings::IniFormat);

    //! Recupération des valeurs dans le fichier INI.
    sNomBDD = settings.value("BaseDeDonnees/NomDeLaBDD", "config").toString();
    sAdresseIP = settings.value("BaseDeDonnees/AdresseIP", "config").toString();
    sNomUtilisateur = settings.value("BaseDeDonnees/NomUtilisateur", "config").toString();
    sMotDePasse = settings.value("BaseDeDonnees/MotDePasse", "config").toString();
    nPort = settings.value("BaseDeDonnees/Port", "config").toInt();

    //! Ouverture de la BDD en entrant les valeurs du fichier INI.
    db.setDatabaseName(sNomBDD);
    db.setHostName(sAdresseIP);
    db.setUserName(sNomUtilisateur);
    db.setPassword(sMotDePasse);
    db.setPort(nPort);

    db.open();
    // Test d'accès à la base de données
    if(db.isOpen() == false)
    {
        QMessageBox Err(QMessageBox::Critical,"LudOpen - Erreur d'accès aux données" ,"Impossible d'accéder à la base de données !\nPeut être que l'ordinateur gérant la base de données n'est pas allumé !\n\n"+ db.lastError().text(),QMessageBox::Close);
        Err.exec();
        cerr << "Vous etes sur l'adresse IP " << sAdresseIP.toStdString() << " sur le port " << nPort << " en tant que " << sNomUtilisateur.toStdString() << "." << endl << endl;
        cerr << endl << "La connexion à la BDD " << sNomBDD.toStdString() << " a échouée." << endl;
        return -1 ;
    }
    else
    {
        cerr << "Connexion BDD OK sur l'adresse IP " << sAdresseIP.toStdString() << " sur le port " << nPort << " en tant que " << sNomUtilisateur.toStdString() << "." << endl << endl;
        F_MainWindow w;
        cerr << endl << "Démarrage de Winludo" << endl ;
        w.showMaximized();
        return a.exec();
    }      
}
