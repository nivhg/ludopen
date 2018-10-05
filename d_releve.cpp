//------------------------------------------------------------------------------
/** @file       D_Releve.cpp
 *  @brief      D_Releve permet de saisir les relevés de caisse
 *
 *  @author     VICTORIN Vincent
 *  @since      10/2018
 *  @version    1.0
 *  @date       01/10/2018
 *
 *  Fabrication Qt Creator, projet  .pro
 *
 *  @todo       Classe terminée.
 */

#include "d_releve.h"
#include "ui_d_releve.h"

D_Releve::D_Releve(QWidget *parent,uint iIdBenevole) :
    QDialog(parent),
    ui(new Ui::D_Releve)
{
    ui->setupUi(this);

    this->iIdBenevole=iIdBenevole;
    QDateTime DateActuelle;
    DateActuelle=DateActuelle.currentDateTime();
    ui->DtE_Releve->setDateTime(DateActuelle);

    ui->TbV_Releves->clearSpans() ;
    ui->TbV_Releves->setModel(&ModeleReleves) ;
    //ui->TbV_Releves->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TbV_Releves->setSortingEnabled(true);
    ModeleReleves.setColumnCount(4);
    ModeleReleves.setRowCount(0);
    ModeleReleves.setHorizontalHeaderItem(0, new QStandardItem("Date & Heure"));
    ModeleReleves.setHorizontalHeaderItem(1, new QStandardItem("Montant"));
    ModeleReleves.setHorizontalHeaderItem(2, new QStandardItem("ou différence"));
    ModeleReleves.setHorizontalHeaderItem(3, new QStandardItem("Nom"));
    ModeleReleves.setHorizontalHeaderItem(4, new QStandardItem("Remarque"));

    MajReleves();

    ui->TbV_Releves->resizeColumnsToContents();
    connect(&this->ModeleReleves, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(TbV_Releves_ItemChanged(QStandardItem *)));
}

D_Releve::~D_Releve()
{
    delete ui;
}

void D_Releve::MajReleves()
{
    QSqlQuery RequeteReleves;
    //Enregistrement d'un nouveau membre dans la base de données
    RequeteReleves.prepare( "SELECT IdReleveCaisse,DateHeureReleve,Montant,Difference,Concat(m.Prenom,' ',m.Nom) as Nom,r.Remarque FROM relevescaisse as r LEFT JOIN membres as m ON "
                            "r.Membres_IdMembre=m.IdMembre WHERE DATEDIFF(NOW(),DateHeureReleve)<30 ORDER BY DateHeureReleve") ;
    //Exectution de la requête
    if( !RequeteReleves.exec() )
    {
        qDebug()<< getLastExecutedQuery(RequeteReleves) << RequeteReleves.lastError();
    }
    else
    {
        int i=0;
        while(RequeteReleves.next())
        {
            QStandardItem *item=new QStandardItem(ObtenirValeurParNom(RequeteReleves,"DateHeureReleve").toDateTime().toString("dd/MM hh:mm"));
            item->setData(ObtenirValeurParNom(RequeteReleves,"IdReleveCaisse").toInt());
            item->setEditable(false);
            ModeleReleves.setItem(i,0,item);
            ModeleReleves.setItem(i,1,new QStandardItem(QString::number(ObtenirValeurParNom(RequeteReleves,"Montant").toDouble(),'f',2)));
            ModeleReleves.setItem(i,2,new QStandardItem(QString::number(ObtenirValeurParNom(RequeteReleves,"Difference").toDouble(),'f',2)));
            item=new QStandardItem(ObtenirValeurParNom(RequeteReleves,"Nom").toString());
            item->setEditable(false);
            ModeleReleves.setItem(i,3,item);
            ModeleReleves.setItem(i,4,new QStandardItem(ObtenirValeurParNom(RequeteReleves,"Remarque").toString()));
            i++;
        }
    }
}

void D_Releve::CalculTotal()
{
    double total;
    total=ui->SBx_UnCent->value()*0.01+ui->SBx_DeuxCents->value()*0.02+ui->SBx_CinqCents->value()*0.05+ui->SBx_DixCents->value()*0.1+ui->SBx_VingtCents->value()*0.2+
            ui->SBx_CinquanteCents->value()*0.5+ui->SBx_UnEuro->value()+ui->SBx_DeuxEuros->value()*2+ui->SBx_CinqEuros->value()*5+ui->SBx_DixEuros->value()*10+
            +ui->SBx_VingtEuros->value()*20+ui->SBx_CinquanteEuros->value()*50+ui->SBx_CentEuros->value()*100;
    ui->SBx_Total->setValue(total);
    VerifChampsSaisis();
}

void D_Releve::on_SBx_UnCent_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_DeuxCents_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_CinqCents_valueChanged(const QString &arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_DixCents_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_VingtCents_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_CinquanteCents_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_UnEuro_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_DeuxEuros_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_CinqEuros_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_DixEuros_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_VingtEuros_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_CinquanteEuros_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_CentEuros_valueChanged(int arg1)
{
    CalculTotal();
}

void D_Releve::on_SBx_Total_valueChanged(double arg1)
{
    VerifChampsSaisis();
    ui->SBx_Difference->setValue(0);

}

void D_Releve::on_SBx_Difference_valueChanged(double arg1)
{
    VerifChampsSaisis();
    ui->SBx_Total->setValue(0);
}

void D_Releve::VerifChampsSaisis()
{
    if(ui->SBx_Total->value()!=0 || ui->SBx_Difference->value()!=0)
    {
        ui->Bt_Valider->setEnabled(true);
    }
}

void D_Releve::on_Bt_Valider_clicked()
{
    QSqlQuery Requete;
    // Requête qui permet de retrouver le montant des recettes de la permanence en cours. On cherche déjà l'heure de début de la permanence, puis on calcule le
    // montant des recettes à partir de cette heure de début moins 1H. Si il est 1h de plus que l'heure de fin de la permanence, la requête ne renvoie rien

    Requete.prepare("SELECT SUM(pa.Montant) as Total,r.Montant as MontantDebut FROM (SELECT @d:='2018-09-26 18:30:00') d,permanences as pe LEFT JOIN paiements as pa ON "
                    "DATE(pa.DatePaiement)=DATE(@d) AND SUBTIME(pa.DatePaiement,SUBTIME(pe.HeureDebut,'01:00')) > 0 LEFT JOIN relevescaisse as r ON "
                    "DATE(r.DateHeureReleve)=DATE(@d) AND SUBTIME(r.DateHeureReleve,SUBTIME(pe.HeureDebut,'01:00')) > 0 WHERE JourPermanence=WEEKDAY(@d) AND "
                    "SUBTIME(ADDTIME(HeureFin,'01:00'),TIME(@d)) > 0 AND SUBTIME(TIME(@d),SUBTIME(HeureDebut,'01:00')) > 0 GROUP BY r.Montant,r.DateHeureReleve "
                    "ORDER BY r.DateHeureReleve ASC LIMIT 1");
    //Exectution de la requête
    if( !Requete.exec() )
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError() ;
        return;
    }
    Requete.next();
    double total=ObtenirValeurParNom(Requete,"Total").toDouble();
    double DiffDebutFin=ui->SBx_Total->value()-ObtenirValeurParNom(Requete,"MontantDebut").toDouble();
    if(ui->SBx_Difference->value() != 0 && total!=ui->SBx_Difference->value())
    {
        if(QMessageBox::question(this, "Confirmation", "Il y a eu "+QString::number(total)+
                " euros de recette pendant cette permanence. Êtes-vous sûr que la différence n'est que de "+
                QString::number(ui->SBx_Difference->value())+" euros ?", "Oui", "Non") != 0)
        {
            return;
        }
    }
    if(ui->SBx_Total->value() != 0 && DiffDebutFin != total)
    {
        if(QMessageBox::question(this, "Confirmation", "Il y a eu "+QString::number(total)+
                " euros de recette pendant cette permanence. Êtes-vous sûr que le montant de la caisse est de "+QString::number(ui->SBx_Total->value())+
                " euros ?", "Oui", "Non") != 0)
        {
            return;
        }
    }

    // Récupère toutes les activités
    Requete.prepare("INSERT INTO relevescaisse "
        "(Membres_IdMembre,DateHeureReleve,Montant,Difference,Remarque,UnCent,DeuxCents,CinqCents,DixCents,VingtCents,CinquanteCents,"
        "UnEuro,DeuxEuros,CinqEuros,DixEuros,VingtEuros,CinquanteEuros,CentEuros) VALUES "
        "(:Membres_IdMembre,:DateHeureReleve,:Montant,:Difference,:Remarque,:UnCent,:DeuxCents,:CinqCents,:DixCents,:VingtCents,:CinquanteCents,"
        ":UnEuro,:DeuxEuros,:CinqEuros,:DixEuros,:VingtEuros,:CinquanteEuros,:CentEuros)") ;
    Requete.bindValue(":Membres_IdMembre", this->iIdBenevole);
    Requete.bindValue(":DateHeureReleve", ui->DtE_Releve->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    Requete.bindValue(":Montant", QString::number(ui->SBx_Total->value(), 'f', ui->SBx_Total->decimals()));
    Requete.bindValue(":Difference", ui->SBx_Difference->value());
    Requete.bindValue(":Remarque", ui->Le_Remarques->text());
    Requete.bindValue(":UnCent", ui->SBx_UnCent->value());
    Requete.bindValue(":DeuxCents", ui->SBx_DeuxCents->value());
    Requete.bindValue(":CinqCents", ui->SBx_CinqCents->value());
    Requete.bindValue(":DixCents", ui->SBx_DixCents->value());
    Requete.bindValue(":VingtCents", ui->SBx_VingtCents->value());
    Requete.bindValue(":CinquanteCents", ui->SBx_CinquanteCents->value());
    Requete.bindValue(":UnEuro", ui->SBx_UnEuro->value());
    Requete.bindValue(":DeuxEuros", ui->SBx_DeuxEuros->value());
    Requete.bindValue(":CinqEuros", ui->SBx_CinqEuros->value());
    Requete.bindValue(":DixEuros", ui->SBx_DixEuros->value());
    Requete.bindValue(":VingtEuros", ui->SBx_VingtEuros->value());
    Requete.bindValue(":CinquanteEuros", ui->SBx_CinquanteEuros->value());
    Requete.bindValue(":CentEuros", ui->SBx_CentEuros->value());

    //Exectution de la requête
    if( !Requete.exec() )
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError() ;
        return;
    }
    MajReleves();
    emit(SignalReleveFini());
}

void D_Releve::TbV_Releves_ItemChanged(QStandardItem *item)
{
    QString NomChamps;
    switch(item->column())
    {
        // Montant
        case 1:
            NomChamps="Montant";
            break;

        // Différence
        case 2:
            NomChamps="Difference";
            break;
        // Remarque
        case 4:
            NomChamps="Remarque";
            break;

        default:
        return;
    }
    QSqlQuery Requete;

    Requete.prepare("UPDATE relevescaisse SET "+NomChamps+"=:"+NomChamps+" WHERE IdReleveCaisse=:IdReleveCaisse");
    Requete.bindValue(":"+NomChamps, item->text());
    qDebug()<<ModeleReleves.item(item->row(),0)->data();
    Requete.bindValue(":IdReleveCaisse", ModeleReleves.item(item->row(),0)->data().toInt());
    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete) << Requete.lastError();
    }
}

void D_Releve::on_Bt_PlusTard_clicked()
{
    emit(SignalPlusTard());
}

void D_Releve::ChangementModeSaisie(bool DebutFin)
{
    ui->SBx_Difference->setEnabled(DebutFin);
}
