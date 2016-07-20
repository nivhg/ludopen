//------------------------------------------------------------------------------
/** @file       f_statistiques.cpp
 *  @brief      F_Statistiques permet de consulter les statistiques dans la partie "Administration".
 *
 *  @author     NORMAND Julien
 *  @author     STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since      01/2012
 *  @version    1.0
 *  @date       12/06/2012
 *
 *  Les statistiques disponibles sont:
 *      - Total d'adhérents inscrit.
 *      - Adhérents ayant emprunté sur une période.
 *      - Répartition des adhérents par commune.
 *      - Classement des adhérents empruntant le plus.
 *      - Classement des jeux par popularités
 *      - Nombre de jeux empruntés sur une période.
 *
 *  Fabrication   Qt Creator, projet    .pro
 *
 *  @todo       Classe terminée.
 */
//------------------------------------------------------------------------------
#include <QtWidgets>
#include <QtSql>
#include <fstream>
#include <QtDebug>

using namespace std;
//------------------------------------------------------------------------------
#include "f_statistiques.h"
#include "ui_f_statistiques.h"
//------------------------------------------------------------------------------
#include "f_plusdestatistiques.h"
//------------------------------------------------------------------------------

/**
 *  @brief Constructeur de la classe.
 *
 *  @param parent
 */
F_Statistiques::F_Statistiques(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Statistiques)
{
    this->nChampsCombo = 1;
    this->nPeriode = 1;
    this->nJourDebut = 0;
    this->nJourFin = 0;
    this->nMoisDebut = 0;
    this->nMoisFin = 0;
    this->nAnneeDebut = 0;
    this->nAnneeFin = 0;

    ui->setupUi(this);
    this->pPlusDeStatistiques = new F_PlusDeStatistiques(this);

    // Config des calendriers
    ui->DtE_Fin->setDate(QDate::currentDate());
    QDate DateDebut = QDate::currentDate();
    ui->DtE_Debut->setDate(DateDebut.addYears(-1));

    TbStatModele = new QStandardItemModel();
    ui->TbV_Stat->setModel(this->TbStatModele);
    ui->TbV_Stat->setEditTriggers(0);
    // Autorise le tri pour ce tableau
    ui->TbV_Stat->setSortingEnabled(true);

    TbStatPlusModele = new QStandardItemModel();
    ui->TbV_StatPlus->setModel(this->TbStatPlusModele);
    ui->TbV_StatPlus->setEditTriggers(0);
    //ui->TbV_StatPlus->verticalHeader()->setVisible(false);
    // Autorise le tri pour ce tableau
    ui->TbV_StatPlus->setSortingEnabled(true);

    ui->Bt_Exporter->hide();
    ui->Bt_PlusStat->hide();
}

/**
 *  @brief Destructeur de la classe.
 *
 */
F_Statistiques::~F_Statistiques()
{
    delete ui;
}

//---------------------------------------------------------------------------
// METHODEs PRIVEEs
//---------------------------------------------------------------------------
/**
 *  @brief  Permet d'afficher la classe graphique F_PlusDeStatistiques.
 *
 */
void F_Statistiques::on_Bt_PlusStat_clicked()
{
    this->pPlusDeStatistiques->show();
}

/**
 *  @brief  Permet de récupérer l'entier du combo box.
 *
 *  @param  index
 */
void F_Statistiques::on_CBx_Stat_currentIndexChanged(int index)
{
    switch (index)
    {
    case 0:
        // Pour le total d'adhérent inscrits
        this->nChampsCombo = 1;

        ui->TbV_Stat->show();
        ui->TbV_StatPlus->show();
        ui->Rbt_Jour->show();
        ui->RBt_Mois->show();
        ui->RBt_Annee->show();
        ui->label_3->show();
        ui->DtE_Debut->show();
        ui->DtE_Fin->show();
        ui->label_2->show();
        ui->label_4->show();
        break;
    case 1:
        // Pour les adhérents ayant emprunté sur une période
        this->nChampsCombo = 2;

        ui->TbV_Stat->show();
        ui->TbV_StatPlus->show();
        ui->Rbt_Jour->show();
        ui->RBt_Mois->show();
        ui->RBt_Annee->show();
        ui->label_3->show();
        ui->DtE_Debut->show();
        ui->DtE_Fin->show();
        ui->label_2->show();
        ui->label_4->show();
        break;
    case 2:
        // Pour la répartition des adhérents par commune
        this->nChampsCombo = 3;

        ui->TbV_Stat->show();
        ui->TbV_StatPlus->hide();
        ui->Rbt_Jour->hide();
        ui->RBt_Mois->hide();
        ui->RBt_Annee->hide();
        ui->label_3->hide();
        ui->DtE_Debut->hide();
        ui->DtE_Fin->show();
        ui->label_2->hide();
        ui->label_4->show();
        break;
    case 3:
        // Pour le classement des adhérents empruntant le plus
        this->nChampsCombo = 4;

        ui->TbV_Stat->show();
        ui->TbV_StatPlus->hide();
        ui->Rbt_Jour->hide();
        ui->RBt_Mois->hide();
        ui->RBt_Annee->hide();
        ui->label_3->hide();
        ui->DtE_Debut->show();
        ui->DtE_Fin->show();
        ui->label_2->show();
        ui->label_4->show();
        break;
    case 4:
        // Pour le classement des jeux par popularités
        this->nChampsCombo = 5;

        ui->TbV_Stat->show();
        ui->TbV_StatPlus->hide();
        ui->Rbt_Jour->hide();
        ui->RBt_Mois->hide();
        ui->RBt_Annee->hide();
        ui->label_3->hide();
        ui->DtE_Debut->hide();
        ui->DtE_Fin->hide();
        ui->label_2->hide();
        ui->label_4->hide();
        break;
    case 5:
        // Pour le nombre de jeux empruntés sur une période
        this->nChampsCombo = 6;

        ui->TbV_Stat->show();
        ui->TbV_StatPlus->show();
        ui->Rbt_Jour->show();
        ui->RBt_Mois->show();
        ui->RBt_Annee->show();
        ui->label_3->show();
        ui->DtE_Debut->show();
        ui->DtE_Fin->show();
        ui->label_2->show();
        ui->label_4->show();
        break;
    default:
        qDebug()<< "Erreur de combo" << endl;
    }
}

/**
 *  @brief  Permet d'afficher les DateTime Edit sous forme "dd/MM/yyyy".
 *
 */
void F_Statistiques::on_Rbt_Jour_clicked()
{
    this->nPeriode = 1;
    ui->DtE_Debut->setDisplayFormat("dd/MM/yyyy");
    ui->DtE_Fin->setDisplayFormat("dd/MM/yyyy");
}

/**
 *  @brief  Permet d'afficher les DateTime Edit sous forme "MM/yyyy".
 *
 */
void F_Statistiques::on_RBt_Mois_clicked()
{
    this->nPeriode = 2;
    ui->DtE_Debut->setDate(QDate(ui->DtE_Debut->date().year(),ui->DtE_Debut->date().month(),1));
    ui->DtE_Debut->setDisplayFormat("MM/yyyy");
    ui->DtE_Fin->setDate(QDate(ui->DtE_Fin->date().year(),ui->DtE_Fin->date().month(),1));
    ui->DtE_Fin->setDisplayFormat("MM/yyyy");
}

/**
 *  @brief  Permet d'afficher les DateTime Edit sous forme "yyyy".
 *
 */
void F_Statistiques::on_RBt_Annee_clicked()
{
    this->nPeriode = 3;
    ui->DtE_Debut->setDate(QDate(ui->DtE_Debut->date().year(),1,1));
    ui->DtE_Debut->setDisplayFormat("yyyy");
    ui->DtE_Fin->setDate(QDate(ui->DtE_Fin->date().year(),1,1));
    ui->DtE_Fin->setDisplayFormat("yyyy");

}

/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT" qui montre tous les adhérents inscrits.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentInscrit()
{
    QSqlQuery Statistique;
    int nNombreLigne (0);
    QDateTime DateFin;

    if(ui->RBt_Mois->isChecked())
    {
        DateFin = ui->DtE_Fin->dateTime().addMonths(1);
    }
    else if(ui->RBt_Annee->isChecked())
    {
        DateFin = ui->DtE_Fin->dateTime().addYears(1);
    }
    else
    {
        DateFin = ui->DtE_Fin->dateTime();
    }
    QString test=DateFin.toString("dd-MM-yyyy");
    Statistique.prepare("SELECT DISTINCT Nom,Prenom,CodeMembre FROM membres,abonnements WHERE "
                    "DateSouscription >= :DateDebut AND DateSouscription <=:DateFin AND Ecarte=0 AND "
                    "Prestations_IdPrestation IS NOT NULL AND Membres_IdMembre = IdMembre AND "
                    "Prestations_IdPrestation != 5 ORDER BY Nom");
    Statistique.bindValue(":DateDebut", ui->DtE_Debut->dateTime());
    Statistique.bindValue(":DateFin", DateFin);
    Statistique.exec();
    ui->LE_SQL->setText(Statistique.lastQuery());
    while(Statistique.next())
    {
        this->TbStatModele->setItem(nNombreLigne, 0, new QStandardItem(Statistique.value(0).toString()));
        this->TbStatModele->setItem(nNombreLigne, 1, new QStandardItem(Statistique.value(1).toString()));
        this->TbStatModele->setItem(nNombreLigne, 2, new QStandardItem(Statistique.value(2).toString()));
        nNombreLigne++;
    }
    ui->Lb_Resultat->setNum( nNombreLigne ) ;
}
/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir les adhérents inscrits en fonction des jours, du mois et de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentInscritJour()
{
    QSqlQuery Statistique;
    QString sDateFin;
    QString sDateDebut;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    sDateDebut = QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
    sDateFin = QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisFin) + "/" + QString::number(this->nAnneeFin);

    while(sDateDebut.compare(sDateFin) != 0)
    {
        Statistique.prepare("SELECT DISTINCT Nom,Prenom,CodeMembre FROM membres,abonnements WHERE "
                            "DAY(DateSouscription)=:DateJour AND MONTH(DateSouscription)=:DateMois AND "
                            "YEAR(DateSouscription)=:DateAnnee AND Ecarte=0 AND "
                            "Prestations_IdPrestation IS NOT NULL AND Membres_IdMembre = IdMembre ORDER BY Nom");
        Statistique.bindValue(":DateJour", this->nJourDebut);
        Statistique.bindValue(":DateMois", this->nMoisDebut);
        Statistique.bindValue(":DateAnnee", this->nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sDateDebut =  QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sDateDebut));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nJourDebut = this->nJourDebut + 1
                ;
        if(this->nJourDebut == 31)
        {
            this->nJourDebut = 1;
            this->nMoisDebut = this->nMoisDebut + 1;

            if(this->nMoisDebut == 13)
            {
                this->nMoisDebut = 1;
                this->nAnneeDebut = this->nAnneeDebut + 1;
            }
        }
        nDetail = 0;
    }
}
/**
 *  @brief Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir les adhérents inscrits en fonction du mois et de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentInscritMois()
{
    QSqlQuery Statistique;
    QString sDateFin;
    QString sDateDebut;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    sDateDebut = QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
    sDateFin = QString::number(this->nMoisFin) + "/" + QString::number(this->nAnneeFin);

    while(sDateDebut.compare(sDateFin) != 0)
    {
        Statistique.prepare("SELECT DISTINCT Nom,Prenom,CodeMembre FROM membres,abonnements WHERE "
                    "MONTH(DateSouscription)=:DateMois AND "
                    "YEAR(DateSouscription)=:DateAnnee AND Ecarte=0 AND "
                    "Prestations_IdPrestation IS NOT NULL AND Membres_IdMembre = IdMembre ORDER BY Nom");
        Statistique.bindValue(":DateMois", this->nMoisDebut);
        Statistique.bindValue(":DateAnnee", this->nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sDateDebut = QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sDateDebut));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nMoisDebut = this->nMoisDebut + 1;
        if(this->nMoisDebut == 13)
        {
            this->nMoisDebut = 1;
            this->nAnneeDebut = this->nAnneeDebut + 1;
        }
        nDetail = 0;
    }
}
/**
 *  @brief Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir les adhérents inscrits en fonction de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentInscritAnnee()
{
    QSqlQuery Statistique;
    QString sAnnee;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    while(this->nAnneeDebut <= this->nAnneeFin)
    {
        Statistique.prepare("SELECT DISTINCT Nom,Prenom,CodeMembre FROM membres,abonnements WHERE "
                    "YEAR(DateSouscription)=:DateAnnee AND Ecarte=0 AND "
                    "Prestations_IdPrestation IS NOT NULL AND Membres_IdMembre = IdMembre ORDER BY Nom");
        Statistique.bindValue(":DateAnnee", this->nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sAnnee = QString::number(this->nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sAnnee));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nAnneeDebut = this->nAnneeDebut + 1;
        nDetail = 0;
    }
}

/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT" qui montre tous les adhérents ayant empruntés.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentEmprunt()
{
    QSqlQuery Statistique;
    QSqlQuery StatistiquePlus;
    int nNombreLigne (0);

    Statistique.prepare("SELECT Membres_IdMembre FROM emprunts WHERE DateEmprunt>=:DateDebut AND DateEmprunt<=:DateFin GROUP BY Membres_IdMembre");
    Statistique.bindValue(":DateDebut", ui->DtE_Debut->dateTime());
    Statistique.bindValue(":DateFin", ui->DtE_Fin->dateTime());
    Statistique.exec();
    while(Statistique.next())
    {
        StatistiquePlus.prepare("SELECT Nom, Prenom FROM membres WHERE IdMembre=:IdMembre");
        StatistiquePlus.bindValue(":IdMembre", Statistique.value(0).toString());
        StatistiquePlus.exec();
        ui->LE_SQL->setText(StatistiquePlus.lastQuery());
        while(StatistiquePlus.next())
        {
            this->TbStatModele->setItem(nNombreLigne, 1, new QStandardItem(StatistiquePlus.value(0).toString()));
            this->TbStatModele->setItem(nNombreLigne, 2, new QStandardItem(StatistiquePlus.value(1).toString()));
        }

        this->TbStatModele->setItem(nNombreLigne, 0, new QStandardItem(Statistique.value(0).toString()));
        nNombreLigne = nNombreLigne + 1;
    }
    ui->Lb_Resultat->setNum( nNombreLigne ) ;
}
/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir le nombre d'adhérents empruntant en fonction des jours, du mois et de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentEmpruntJour()
{
    QSqlQuery Statistique;
    QString sDateFin;
    QString sDateDebut;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    sDateDebut = QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
    sDateFin = QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisFin) + "/" + QString::number(this->nAnneeFin);

    while(sDateDebut.compare(sDateFin) != 0)
    {
        Statistique.prepare("SELECT * FROM emprunts WHERE DAY(DateEmprunt)=:DateJour AND MONTH(DateEmprunt)=:DateMois AND YEAR(DateEmprunt)=:DateAnnee");
        Statistique.bindValue(":DateJour", this->nJourDebut);
        Statistique.bindValue(":DateMois", this->nMoisDebut);
        Statistique.bindValue(":DateAnnee", this->nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sDateDebut =  QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sDateDebut));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nJourDebut = this->nJourDebut + 1;

        if(this->nJourDebut == 31)
        {
            this->nJourDebut = 1;
            this->nMoisDebut = this->nMoisDebut + 1;

            if(this->nMoisDebut == 13)
            {
                this->nMoisDebut = 1;
                this->nAnneeDebut = this->nAnneeDebut + 1;
            }
        }

        nDetail = 0;
    }
}
/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir le nombre d'adhérents empruntant en fonction du mois et de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentEmpruntMois()
{
    QSqlQuery Statistique;
    QString sDateFin;
    QString sDateDebut;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    sDateDebut = QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
    sDateFin = QString::number(this->nMoisFin) + "/" + QString::number(this->nAnneeFin);

    while(sDateDebut.compare(sDateFin) != 0)
    {
        Statistique.prepare("SELECT * FROM emprunts WHERE MONTH(DateEmprunt)=:DateMois AND YEAR(DateEmprunt)=:DateAnnee");
        Statistique.bindValue(":DateMois", this->nMoisDebut);
        Statistique.bindValue(":DateAnnee", this->nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sDateDebut = QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sDateDebut));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nMoisDebut = this->nMoisDebut + 1;

        if(this->nMoisDebut == 13)
        {
            this->nMoisDebut = 1;
            this->nAnneeDebut = this->nAnneeDebut + 1;
        }

        nDetail = 0;
    }
}

/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir le nombre d'adhérents empruntant en fonction de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentEmpruntAnnee()
{
    QSqlQuery Statistique;
    QString sAnnee;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    while(this->nAnneeDebut <= this->nAnneeFin)
    {
        Statistique.prepare("SELECT * FROM emprunts WHERE YEAR(DateEmprunt)=:DateEmprunt");
        Statistique.bindValue(":DateEmprunt", this->nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sAnnee = QString::number(nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sAnnee));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nAnneeDebut = this->nAnneeDebut + 1;
        nDetail = 0;
    }
}

/**
 *  @brief  @brief Fonction qui exécute la requête SQL "SELECT" qui montrent le nombre d'adhérent vivant dans les villes.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentCommune()
{
    QSqlQuery Statistique;
    int nNombreLigne (0),i(0);

    Statistique.prepare("SELECT Ville,count(Ville) FROM membres,abonnements WHERE "
                        "DateExpiration >= :DateFin AND Ecarte=0 AND "
                        "Prestations_IdPrestation IS NOT NULL AND Membres_IdMembre = IdMembre "
                        "AND Prestations_IdPrestation != 5 GROUP BY Ville");
    Statistique.bindValue(":DateFin", ui->DtE_Fin->dateTime());
    Statistique.exec();
    ui->LE_SQL->setText(Statistique.lastQuery());
    while(Statistique.next())
    {
        this->TbStatModele->setItem(i, 0, new QStandardItem(Statistique.value(0).toString()));
        this->TbStatModele->setItem(i++, 1, new QStandardItem(Statistique.value(1).toString()));
        nNombreLigne = nNombreLigne + Statistique.value(1).toInt();
    }
    ui->Lb_Resultat->setNum( nNombreLigne ) ;
}

/**
 *  @brief Fonction qui exécute la requête SQL "SELECT" qui montre le nombre de fois que les adhérents ont emprunté.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteAdherentPlus()
{
    QSqlQuery Statistique;
    QSqlQuery StatistiquePlus;
    QSqlQuery StatistiquePlusPlus;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    Statistique.prepare("SELECT Membres_IdMembre FROM emprunts WHERE DateEmprunt>=:DateDebut AND DateEmprunt<=:DateFin GROUP BY Membres_IdMembre");
    Statistique.bindValue(":DateDebut", ui->DtE_Debut->dateTime());
    Statistique.bindValue(":DateFin", ui->DtE_Fin->dateTime());
    Statistique.exec();
    ui->LE_SQL->setText(Statistique.lastQuery());
    while(Statistique.next())
    {
        StatistiquePlus.prepare("SELECT * FROM emprunts WHERE DateEmprunt>=:DateDebut AND DateEmprunt<=:DateFin AND Membres_IdMembre=:Membres");
        StatistiquePlus.bindValue(":DateDebut", ui->DtE_Debut->dateTime());
        StatistiquePlus.bindValue(":DateFin", ui->DtE_Fin->dateTime());
        StatistiquePlus.bindValue(":Membres", Statistique.value(0).toString());
        StatistiquePlus.exec();
        while(StatistiquePlus.next())
        {
            nDetail = nDetail + 1;
        }

        StatistiquePlusPlus.prepare("SELECT Nom, Prenom FROM membres WHERE IdMembre=:IdMembre");
        StatistiquePlusPlus.bindValue(":IdMembre", Statistique.value(0).toString());
        StatistiquePlusPlus.exec();
        while(StatistiquePlusPlus.next())
        {
            this->TbStatModele->setItem(nNombreLigne, 1, new QStandardItem(StatistiquePlusPlus.value(0).toString()));
            this->TbStatModele->setItem(nNombreLigne, 2, new QStandardItem(StatistiquePlusPlus.value(1).toString()));
        }

        sDetail = QString::number(nDetail);
        this->TbStatModele->setItem(nNombreLigne, 0, new QStandardItem(Statistique.value(0).toString()));
        this->TbStatModele->setItem(nNombreLigne, 3, new QStandardItem(sDetail));
        nNombreLigne = nNombreLigne + 1;
        nDetail = 0;
    }
    ui->Lb_Resultat->setNum( nNombreLigne ) ;
}

/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT" qui montre le nombre de fois que les jeux ont été empruntés.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteJeuxPopularite()
{
    QSqlQuery Statistique;
    QSqlQuery StatistiquePlus;
    QSqlQuery StatistiquePlusPlus;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    Statistique.prepare("SELECT Jeux_IdJeux FROM emprunts GROUP BY Jeux_IdJeux");
    Statistique.exec();
    ui->LE_SQL->setText(Statistique.lastQuery());
    while(Statistique.next())
    {
        StatistiquePlus.prepare("SELECT * FROM emprunts WHERE Jeux_IdJeux=:Jeux");
        StatistiquePlus.bindValue(":Jeux", Statistique.value(0).toString());
        StatistiquePlus.exec();
        while(StatistiquePlus.next())
        {
            nDetail = nDetail + 1;
        }

        StatistiquePlusPlus.prepare("SELECT NomJeu FROM jeux WHERE IdJeux=:IdJeux");
        StatistiquePlusPlus.bindValue(":IdJeux", Statistique.value(0).toString());
        StatistiquePlusPlus.exec();
        while(StatistiquePlusPlus.next())
        {
            this->TbStatModele->setItem(nNombreLigne, 1, new QStandardItem(StatistiquePlusPlus.value(0).toString()));
        }

        this->TbStatModele->setItem(nNombreLigne, 0, new QStandardItem(Statistique.value(0).toString()));
        QStandardItem *item = new QStandardItem();
        item->setData(QVariant(nDetail), Qt::DisplayRole);
        this->TbStatModele->setItem(nNombreLigne, 2, item);
        nNombreLigne = nNombreLigne + 1;
        nDetail = 0;
    }
    ui->Lb_Resultat->setNum( nNombreLigne ) ;
    this->TbStatModele->sort(2,Qt::DescendingOrder);
}

/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT" qui montre tous les jeux empruntés.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteJeuxEmprunt()
{
    QSqlQuery Statistique;
    QSqlQuery StatistiquePlus;
    QSqlQuery StatistiquePlusPlus;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    Statistique.prepare("SELECT Jeux_IdJeux,CodeJeu FROM emprunts,jeux WHERE IdJeux=Jeux_IdJeux AND DateEmprunt>=:DateDebut AND DateEmprunt<=:DateFin GROUP BY Jeux_IdJeux");
    Statistique.bindValue(":DateDebut", ui->DtE_Debut->dateTime());
    Statistique.bindValue(":DateFin", ui->DtE_Fin->dateTime());
    Statistique.exec();
    ui->LE_SQL->setText(Statistique.lastQuery());
    while(Statistique.next())
    {
        StatistiquePlus.prepare("SELECT * FROM emprunts WHERE DateEmprunt>=:DateDebut AND DateEmprunt<=:DateFin AND Jeux_IdJeux=:Jeux");
        StatistiquePlus.bindValue(":DateDebut", ui->DtE_Debut->dateTime());
        StatistiquePlus.bindValue(":DateFin", ui->DtE_Fin->dateTime());
        StatistiquePlus.bindValue(":Jeux", Statistique.value(0).toString());
        StatistiquePlus.exec();
        while(StatistiquePlus.next())
        {
            nDetail = nDetail + 1;
        }

        StatistiquePlusPlus.prepare("SELECT NomJeu FROM jeux WHERE IdJeux=:IdJeux");
        StatistiquePlusPlus.bindValue(":IdJeux", Statistique.value(0).toString());
        StatistiquePlusPlus.exec();
        while(StatistiquePlusPlus.next())
        {
            this->TbStatModele->setItem(nNombreLigne, 1, new QStandardItem(StatistiquePlusPlus.value(0).toString()));
        }

        sDetail = QString::number(nDetail);
        this->TbStatModele->setItem(nNombreLigne, 0, new QStandardItem(Statistique.value(1).toString()));
        this->TbStatModele->setItem(nNombreLigne, 2, new QStandardItem(sDetail));
        nNombreLigne = nNombreLigne + 1;
        nDetail = 0;
    }
    ui->Lb_Resultat->setNum( nNombreLigne ) ;
}

/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir les jeux empruntés en fonction des jours, du mois et de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteJeuxEmpruntJour()
{
    QSqlQuery Statistique;
    QString sDateFin;
    QString sDateDebut;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    sDateDebut = QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
    sDateFin = QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisFin) + "/" + QString::number(this->nAnneeFin);

    while(sDateDebut.compare(sDateFin) != 0)
    {
        Statistique.prepare("SELECT * FROM emprunts WHERE DAY(DateEmprunt)=:DateJour AND MONTH(DateEmprunt)=:DateMois AND YEAR(DateEmprunt)=:DateAnnee");
        Statistique.bindValue(":DateJour", this->nJourDebut);
        Statistique.bindValue(":DateMois", this->nMoisDebut);
        Statistique.bindValue(":DateAnnee", this->nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sDateDebut =  QString::number(this->nJourDebut) + "/" + QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sDateDebut));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nJourDebut = this->nJourDebut + 1;

        if(this->nJourDebut == 31)
        {
            this->nJourDebut = 1;
            this->nMoisDebut = this->nMoisDebut + 1;

            if(this->nMoisDebut == 13)
            {
                this->nMoisDebut = 1;
                this->nAnneeDebut = this->nAnneeDebut + 1;
            }
        }
        nDetail = 0;
    }
}

/**
 *  @brief Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir les jeux empruntés en fonction du mois et de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteJeuxEmpruntMois()
{
    QSqlQuery Statistique;
    QString sDateFin;
    QString sDateDebut;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    sDateDebut = QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
    sDateFin = QString::number(this->nMoisFin) + "/" + QString::number(this->nAnneeFin);

    while(sDateDebut.compare(sDateFin) != 0)
    {
        Statistique.prepare("SELECT * FROM emprunts WHERE MONTH(DateEmprunt)=:DateMois AND YEAR(DateEmprunt)=:DateAnnee");
        Statistique.bindValue(":DateMois", nMoisDebut);
        Statistique.bindValue(":DateAnnee", nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sDateDebut = QString::number(this->nMoisDebut) + "/" + QString::number(this->nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sDateDebut));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nMoisDebut = this->nMoisDebut + 1;

        if(this->nMoisDebut == 13)
        {
            this->nMoisDebut = 1;
            this->nAnneeDebut = this->nAnneeDebut + 1;
        }

        nDetail = 0;
    }
}

/**
 *  @brief  Fonction qui exécute la requête SQL "SELECT".
 *  La requête permet de voir les jeux empruntés en fonction de l'année dans le tableau de droite.
 *
 *  @pre    Etre connecté à la BDD.
 */
void F_Statistiques::EffectuerRequeteJeuxEmpruntAnnee()
{
    QSqlQuery Statistique;
    QString sAnnee;
    QString sDetail;
    int nNombreLigne (0);
    int nDetail (0);

    while(this->nAnneeDebut <= this->nAnneeFin)
    {
        Statistique.prepare("SELECT * FROM emprunts WHERE YEAR(DateEmprunt)=:DateEmprunt");
        Statistique.bindValue(":DateEmprunt", this->nAnneeDebut);
        Statistique.exec();
        ui->LE_SQL->setText(Statistique.lastQuery());
        while(Statistique.next())
        {
            nDetail = nDetail + 1;
        }

        sAnnee = QString::number(this->nAnneeDebut);
        sDetail = QString::number(nDetail);

        if(nDetail != 0)
        {
            this->TbStatPlusModele->setItem(nNombreLigne, 0, new QStandardItem(sAnnee));
            this->TbStatPlusModele->setItem(nNombreLigne, 1, new QStandardItem(sDetail));
            nNombreLigne = nNombreLigne +1;
        }

        this->nAnneeDebut = this->nAnneeDebut + 1;
        nDetail = 0;
    }
}

/**
 *  @brief Permet d'afficher la statistique sélectionnée avec les options voulues.
 *
 */
void F_Statistiques::on_Bt_AfficherStatistique_clicked()
{
    int nNombreLigne (0);
    int nNombreLigne2 (0);

    ui->Bt_Exporter->show();

    this->TbStatModele->clear();
    this->TbStatPlusModele->clear();

    this->nJourDebut = ui->DtE_Debut->date().day();
    this->nJourFin = ui->DtE_Fin->date().day();
    this->nMoisDebut = ui->DtE_Debut->date().month();
    this->nMoisFin = ui->DtE_Fin->date().month();
    this->nAnneeDebut = ui->DtE_Debut->date().year();
    this->nAnneeFin = ui->DtE_Fin->date().year();

    switch (this->nChampsCombo)
    {
    case 1:
        this->TbStatModele->setColumnCount(3);
        this->TbStatModele->setRowCount(nNombreLigne);
        this->TbStatModele->setHorizontalHeaderItem(0, new QStandardItem("Nom"));
        this->TbStatModele->setHorizontalHeaderItem(1, new QStandardItem("Prénom"));
        this->TbStatModele->setHorizontalHeaderItem(2, new QStandardItem("Code"));
        ui->TbV_Stat->setColumnWidth(0, 150);
        ui->TbV_Stat->setColumnWidth(1, 150);
        ui->TbV_Stat->setColumnWidth(2, 50);

        this->TbStatPlusModele->setColumnCount(2);
        this->TbStatPlusModele->setRowCount(nNombreLigne2);
        this->TbStatPlusModele->setHorizontalHeaderItem(0, new QStandardItem("Date"));
        this->TbStatPlusModele->setHorizontalHeaderItem(1, new QStandardItem("Total"));
        ui->TbV_StatPlus->setColumnWidth(0, 150);
        ui->TbV_StatPlus->setColumnWidth(1, 100);

        this->EffectuerRequeteAdherentInscrit();
        switch(this->nPeriode)
        {
        case 1:
            this->EffectuerRequeteAdherentInscritJour();
            break;
        case 2:
            this->EffectuerRequeteAdherentInscritMois();
            break;
        case 3:
            this->EffectuerRequeteAdherentInscritAnnee();
            break;
        default:
            qDebug()<< "F_Statistiques::on_Bt_AfficherStatistique_clicked()=Erreur dans la valeur nPeriode" << endl;
            break;
        }
        break;

    case 2:
        this->TbStatModele->setColumnCount(3);
        this->TbStatModele->setRowCount(nNombreLigne);
        this->TbStatModele->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->TbStatModele->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->TbStatModele->setHorizontalHeaderItem(2, new QStandardItem("Prénom"));
        ui->TbV_Stat->setColumnWidth(0, 50);
        ui->TbV_Stat->setColumnWidth(1, 150);
        ui->TbV_Stat->setColumnWidth(2, 150);

        this->TbStatPlusModele->setColumnCount(2);
        this->TbStatPlusModele->setRowCount(nNombreLigne2);
        this->TbStatPlusModele->setHorizontalHeaderItem(0, new QStandardItem("Date"));
        this->TbStatPlusModele->setHorizontalHeaderItem(1, new QStandardItem("Total"));
        ui->TbV_StatPlus->setColumnWidth(0, 150);
        ui->TbV_StatPlus->setColumnWidth(1, 100);

        this->EffectuerRequeteAdherentEmprunt();
        switch(this->nPeriode)
        {
        case 1:
            this->EffectuerRequeteAdherentEmpruntJour();
            break;
        case 2:
            this->EffectuerRequeteAdherentEmpruntMois();
            break;
        case 3:
            this->EffectuerRequeteAdherentEmpruntAnnee();
            break;
        default:
            qDebug()<< "F_Statistiques::on_Bt_AfficherStatistique_clicked()=Erreur dans la valeur nPeriode" << endl;
            break;
        }
        break;

    case 3:
        this->TbStatModele->setColumnCount(2);
        this->TbStatModele->setRowCount(nNombreLigne);
        this->TbStatModele->setHorizontalHeaderItem(0, new QStandardItem("Ville"));
        this->TbStatModele->setHorizontalHeaderItem(1, new QStandardItem("Nombres d'adherent"));
        ui->TbV_Stat->setColumnWidth(0, 200);
        ui->TbV_Stat->setColumnWidth(1, 150);

        this->EffectuerRequeteAdherentCommune();
        break;

    case 4:
        this->TbStatModele->setColumnCount(4);
        this->TbStatModele->setRowCount(nNombreLigne);
        this->TbStatModele->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->TbStatModele->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
        this->TbStatModele->setHorizontalHeaderItem(2, new QStandardItem("Prenom"));
        this->TbStatModele->setHorizontalHeaderItem(3, new QStandardItem("Emprunts"));
        ui->TbV_Stat->setColumnWidth(0, 50);
        ui->TbV_Stat->setColumnWidth(1, 150);
        ui->TbV_Stat->setColumnWidth(2, 150);
        ui->TbV_Stat->setColumnWidth(3, 100);

        this->EffectuerRequeteAdherentPlus();
        break;

    case 5:
        this->TbStatModele->setColumnCount(3);
        this->TbStatModele->setRowCount(nNombreLigne);
        this->TbStatModele->setHorizontalHeaderItem(0, new QStandardItem("Code Jeu"));
        this->TbStatModele->setHorizontalHeaderItem(1, new QStandardItem("Nom du Jeu"));
        this->TbStatModele->setHorizontalHeaderItem(2, new QStandardItem("Emprunts"));
        ui->TbV_Stat->setColumnWidth(0, 100);
        ui->TbV_Stat->setColumnWidth(1, 200);
        ui->TbV_Stat->setColumnWidth(2, 100);

        this->EffectuerRequeteJeuxPopularite();
        break;

    case 6:
        this->TbStatModele->setColumnCount(2);
        this->TbStatModele->setRowCount(nNombreLigne);
        this->TbStatModele->setHorizontalHeaderItem(0, new QStandardItem("Code"));
        this->TbStatModele->setHorizontalHeaderItem(1, new QStandardItem("Nom Jeu"));
        this->TbStatModele->setHorizontalHeaderItem(2, new QStandardItem("Emprunts"));
        ui->TbV_Stat->setColumnWidth(0, 100);
        ui->TbV_Stat->setColumnWidth(1, 200);
        ui->TbV_Stat->setColumnWidth(2, 100);

        this->TbStatPlusModele->setColumnCount(2);
        this->TbStatPlusModele->setRowCount(nNombreLigne2);
        this->TbStatPlusModele->setHorizontalHeaderItem(0, new QStandardItem("Date"));
        this->TbStatPlusModele->setHorizontalHeaderItem(1, new QStandardItem("Total"));
        ui->TbV_StatPlus->setColumnWidth(0, 150);
        ui->TbV_StatPlus->setColumnWidth(1, 100);

        this->EffectuerRequeteJeuxEmprunt();
        switch(this->nPeriode)
        {
        case 1:
            this->EffectuerRequeteJeuxEmpruntJour();
            break;
        case 2:
            this->EffectuerRequeteJeuxEmpruntMois();
            break;
        case 3:
            this->EffectuerRequeteJeuxEmpruntAnnee();
            break;
        default:
            qDebug()<< "F_Statistiques::on_Bt_AfficherStatistique_clicked()=Erreur dans la valeur nPeriode" << endl;
            break;
        }
        break;
    default:
        qDebug()<< "F_Statistiques::on_Bt_AfficherStatistique_clicked()=Erreur dans la valeur nChampsCombo" << endl;
        break;
    }
}

/**
 *  @brief  Permet d'exporter les tableaux dans un fichier .csv.
 *
 */
void F_Statistiques::on_Bt_Exporter_clicked()
{
    int nNombreColonne (0);
    int nNombreLigne (0);

    QString sCaractere;
    QString nomFichier = QFileDialog::getSaveFileName(this, "Enregistrer sous ...", "Sans titre 1.csv");
    QFile fichier(nomFichier);
    QTextStream ecrire (&fichier);
    fichier.open(QIODevice::WriteOnly);

    ecrire << "Tableau Statistiques Général \r\n\r\n";
    for(nNombreLigne = 0; nNombreLigne<this->TbStatModele->rowCount(); nNombreLigne++)
    {
        for(nNombreColonne = 0; nNombreColonne<this->TbStatModele->columnCount(); nNombreColonne++)
        {
            sCaractere = this->TbStatModele->item(nNombreLigne, nNombreColonne)->text();
            // On rejete les valeurs à caractère unique et on le remplace par un champs vide
            if(sCaractere == "-" || sCaractere == "_" || sCaractere == ".")
            {
                sCaractere = "";
            }
            ecrire << "\"" << sCaractere << "\";";
        }
        ecrire << "\r\n";
        nNombreColonne = 0;
    }
    nNombreLigne = 0;

    if(this->nChampsCombo == 1 || this->nChampsCombo == 2 || this->nChampsCombo == 6)
    {
        ecrire << "\r\n\r\n Tableau Statistiques Détaillées \r\n\r\n";
        for(nNombreLigne = 0; nNombreLigne<this->TbStatPlusModele->rowCount(); nNombreLigne++)
        {
            for(nNombreColonne = 0; nNombreColonne<this->TbStatPlusModele->columnCount(); nNombreColonne++)
            {
                sCaractere = this->TbStatPlusModele->item(nNombreLigne, nNombreColonne)->text();
                // On rejete les valeurs à caractère unique et on le remplace par un champs vide
                if(sCaractere == "-" || sCaractere == "_" || sCaractere == ".")
                {
                    sCaractere = "";
                }
                ecrire << "\"" << sCaractere << "\";";
            }
            ecrire << "\r\n";
            nNombreColonne = 0;
        }
        nNombreLigne = 0;
    }
}

//---------------------------------------------------------------------------
