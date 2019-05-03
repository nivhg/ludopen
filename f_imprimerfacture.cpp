// En-tête propre à l'application ----------------------------------------------
#include "f_imprimerfacture.h".h"
#include "ui_f_imprimerfacture.h"

F_ImprimerFacture::F_ImprimerFacture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_ImprimerFacture)
{
    ui->setupUi(this);
    this->setWindowTitle("Aperçu avant impression");
}

F_ImprimerFacture::~F_ImprimerFacture()
{
    delete ui;
}

/**
 * @brief Méthode qui affiche les différentes informations à imprimer
 *
 * @param CodeJeu
 */
void F_ImprimerFacture::ImprimerFacture(uint nIdPaiement, bool EstCeFacture,QHash<QString, QVariant> HPaiement)
{
    this->EstCeFacture=EstCeFacture;
    QFile file(":/Modele_facture_devis.html");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString contenu=file.readAll();
    contenu.replace("#Nom#",F_Preferences::ObtenirValeur("Nom"),Qt::CaseInsensitive);
    contenu.replace("#Adresse#",F_Preferences::ObtenirValeur("Adresse"),Qt::CaseInsensitive);
    contenu.replace("#CodePostal#",F_Preferences::ObtenirValeur("CodePostal"),Qt::CaseInsensitive);
    contenu.replace("#VilleLudo#",F_Preferences::ObtenirValeur("Ville"),Qt::CaseInsensitive);
    contenu.replace("#NumeroTel#",F_Preferences::ObtenirValeur("NumeroTel"),Qt::CaseInsensitive);
    contenu.replace("#PiedPageFacture#",F_Preferences::ObtenirValeur("PiedPageFacture"),Qt::CaseInsensitive);

    QSqlQuery RequetePaiement;
    if(EstCeFacture)
    {
        contenu.replace("#Titre#","FACTURE",Qt::CaseInsensitive);
        RequetePaiement.prepare("SELECT *,p.Montant as PaieMontant, pa.Montant as Montants,p.Remarque as RemarquePaiement FROM paiements as p LEFT JOIN membres as m ON m.IdMembre=p.Membres_IdMembre "
                                "LEFT JOIN paiementsachats as pa ON pa.Paiements_IdPaiement=p.IdPaiement LEFT JOIN modepaiement as mo ON "
                                "mo.IdModePaiement=ModePaiement_IdModePaiement WHERE IdPaiement=:IdPaiement");
        RequetePaiement.bindValue(":IdPaiement",nIdPaiement);

        if (!RequetePaiement.exec())
        {
            qDebug()<<getLastExecutedQuery(RequetePaiement)<<RequetePaiement.lastError();
            return;
        }

        //QHash<QString, QVariant> HPaiement;
        RequetePaiement.next();
        for(int i=0;i<RequetePaiement.record().count();i++)
        {
            HPaiement[RequetePaiement.record().fieldName(i)]=RequetePaiement.value(i);
        }
        HPaiement["Montant"]=QString::number(HPaiement["PaieMontant"].toDouble());
    }
    else
    {
        contenu.replace("#Titre#","DEVIS",Qt::CaseInsensitive);
    }

    contenu.replace("#NomMembre#",HPaiement["Nom"].toString(),Qt::CaseInsensitive);
    contenu.replace("#PrenomMembre#",HPaiement["Prenom"].toString(),Qt::CaseInsensitive);
    contenu.replace("#Rue#",HPaiement["Rue"].toString(),Qt::CaseInsensitive);
    contenu.replace("#CP#",HPaiement["CP"].toString(),Qt::CaseInsensitive);
    contenu.replace("#Ville#",HPaiement["Ville"].toString(),Qt::CaseInsensitive);
    QString Tel=HPaiement["Telephone"].toString();
    QString Mobile=HPaiement["Mobile"].toString();
    QString Tels;
    if(Tel!="" && Mobile !="")
    {
        Tels=Tel+"/"+Mobile;
    }
    else if(Tel!="")
    {
        Tels=Tel;
    }
    else
    {
        Tels=Mobile;
    }
    contenu.replace("#Telephones#",Tels,Qt::CaseInsensitive);
    contenu.replace("#NumFacture#",HPaiement["NumeroFacture"].toString(),Qt::CaseInsensitive);
    contenu.replace("#Date#",HPaiement["DatePaiement"].toDate().toString("dd/MM/yyyy"),Qt::CaseInsensitive);
    contenu.replace("#Remarque#",HPaiement["RemarquePaiement"].toString(),Qt::CaseInsensitive);
    contenu.replace("#Total#",HPaiement["Montant"].toString(),Qt::CaseInsensitive);


    QString DatePaye=HPaiement["DatePaye"].toDate().toString("dd/MM/yyyy");
    qDebug()<<DatePaye;
    if(DatePaye=="")
    {
        DatePaye="Attente paiement";
    }
    contenu.replace("#DatePaye#",DatePaye,Qt::CaseInsensitive);
    contenu.replace("#ModePaiement#",HPaiement["NomPaiement"].toString(),Qt::CaseInsensitive);

    QString Designations;
    QString Montants;
    if(EstCeFacture)
    {
        do
        {
            Designations+="<p>"+ObtenirValeurParNom(RequetePaiement,"Designation").toString()+"</p>";
            Montants+="<p align='center'>"+ObtenirValeurParNom(RequetePaiement,"Montants").toString()+" €</p>";
        }
        while(RequetePaiement.next());
    }
    else
    {
        Designations=HPaiement["Designations"].toString();
        Montants=HPaiement["Montants"].toString();
    }

    contenu.replace("#Designations#",Designations,Qt::CaseInsensitive);
    contenu.replace("#Montants#",Montants,Qt::CaseInsensitive);

    ui->TxE_Contenu->setText("<table width='100%'><tr><td>"+contenu+"</td><td>"+contenu+"</td></tr></table>");
    file.close();
}

/**
 * @brief Méthode qui lance la fenêtre d'impression et qui fait la mise en page
 *
 */
void F_ImprimerFacture::on_Bt_Imprimer_clicked()
{
    QPrinterInfo InformationsImprimantes ;
    if (InformationsImprimantes.defaultPrinter().isNull() )
    {
       QMessageBox::information(this,"Impossible d'imprimer !","Il n'y a pas d'imprimante reconnue sur votre ordinateur !\n\n"
                                "1°) Soit il n'y a pas d'imprimante installée sur ce poste.\n"
                                "2°) Soit elle n'est pas allumée.\n"
                                "3°) Soit elle n'est pas branchée.\n\n"
                                "Contactez votre administrateur préféré.","OK");
    }
    else
    {
       QPrinter Imprimante(QPrinter::ScreenResolution);
       Imprimante.setPaperSize (QPrinter::A4);
       Imprimante.setOrientation(QPrinter::Landscape);
       Imprimante.setFullPage(true);

       QPrintDialog printDialog(&Imprimante, this);
       // Si pas d'imprimante connectée, la fenêtre ne peut s'afficher
       if ( printDialog.exec() == QDialog::Accepted )
       {
           //Création des formats de textes
/*           QTextCharFormat TailleTexte;
           TailleTexte.setFontPointSize(10);

           ui->TxE_Contenu->setCurrentCharFormat(TailleTexte);*/
           ui->TxE_Contenu->print(&Imprimante);
       }
    }
}


/**
 * @brief Méthode qui permet de quitter la fenêtre d'impression
 *
 */
void F_ImprimerFacture::on_Bt_Quitter_clicked()
{
    this->hide();
}
