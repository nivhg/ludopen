// En-tête propre à l'application ----------------------------------------------
#include "f_imprimermalle.h"
#include "ui_f_imprimermalle.h"

F_ImprimerMalle::F_ImprimerMalle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_ImprimerMalle)
{
    ui->setupUi(this);
}

F_ImprimerMalle::~F_ImprimerMalle()
{
    delete ui;
}

/**
 * @brief Méthode qui affiche les différentes informations à imprimer
 *
 * @param CodeJeu
 */
void F_ImprimerMalle::ImprimerMalle(int iIdMalle,bool EstCeEmprunt)
{
    this->show();

    QString Select="FROM malles as m LEFT JOIN typeemprunt ON TypeEmprunt_IdTypeEmprunt=IdTypeEmprunt LEFT JOIN membres as me ON m.Membres_IdMembre=me.IdMembre ";
    if(EstCeEmprunt)
    {
        Select="SELECT *,m.DateEmprunt as DD,m.DateRetourPrevu as DR "+Select;
        Select+="LEFT JOIN emprunts as e ON m.IdMalle=e.Malles_IdMalle";
    }
    else
    {
        Select="SELECT *,m.DatePrevuEmprunt as DD,m.DateRetourPrevu as DR "+Select;
        Select+="LEFT JOIN reservation as r ON m.IdMalle=r.Malles_IdMalle";
    }
    Select+=" LEFT JOIN jeux as j ON j.IdJeux=Jeux_IdJeux WHERE IdMalle=:IdMalle";
    QSqlQuery Requete ;
    Requete.prepare(Select);
    Requete.bindValue(":IdMalle", iIdMalle);
    if(!Requete.exec())
    {
        qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
    }
    qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
    Requete.next();
    QString InfosMembre = "Membre:\n"+ObtenirValeurParNom(Requete,"Nom").toString()+" "+ObtenirValeurParNom(Requete,"Prenom").toString()+"\n"+
            ObtenirValeurParNom(Requete,"Rue").toString()+"\n"+
            ObtenirValeurParNom(Requete,"CP").toString()+" "+ObtenirValeurParNom(Requete,"Ville").toString()+"\n"+
            ObtenirValeurParNom(Requete,"Telephone").toString()+"\n";
    QString InfosMalle="Type de malle : "+ObtenirValeurParNom(Requete,"TypeEmprunt").toString()+"\nDate de départ : "+
            ObtenirValeurParNom(Requete,"DD").toDateTime().toString("dd/MM/yyyy hh:mm")+"\nDate de retour prévu : "+
            ObtenirValeurParNom(Requete,"DR").toDateTime().toString("dd/MM/yyyy hh:mm");
    QString InfosJeux="Jeux\n";
    do
    {
        InfosJeux+="- "+ObtenirValeurParNom(Requete,"NomJeu").toString()+" ("+ObtenirValeurParNom(Requete,"CodeJeu").toString()+")\n";
    }
    while(Requete.next());

    QString InfosLudo = "Ludothèque "+F_Preferences::ObtenirValeur("Nom") + "\n" + F_Preferences::ObtenirValeur("Adresse")
            + "\n" + F_Preferences::ObtenirValeur("CodePostal") + "  " + F_Preferences::ObtenirValeur("Ville")+
            "\n" + F_Preferences::ObtenirValeur("ContactMalle");

    ui->TxE_Contenu->setText(InfosLudo+"\n\n"+InfosMembre+"\n\n"+InfosMalle+"\n\n"+InfosJeux);
}

/**
 * @brief Méthode qui lance la fenêtre d'impression et qui fait la mise en page
 *
 */
void F_ImprimerMalle::on_Bt_Imprimer_clicked()
{
    QPrinterInfo InformationsImprimantes ;
    if (InformationsImprimantes.defaultPrinter().isNull() )
    {
       QMessageBox::information(this,"Pas d'impression possible !","Il n'y a pas d'imprimante reconnue sur votre ordinateur !\n\n"
                                "1°) Soit il n'y a pas d'imprimante installée sur ce poste.\n"
                                "2°) Soit elle n'est pas allumée.\n"
                                "3°) Soit elle n'est pas branchée.\n\n"
                                "Contactez votre administrateur réseau.","Ok");
    }
    else
    {
       QPrinter Imprimante(QPrinter::HighResolution);
       Imprimante.setPaperSize (QPrinter::A4);
       Imprimante.setOrientation(QPrinter::Landscape);
       Imprimante.setFullPage(true);

       QPrintDialog printDialog(&Imprimante, this);
       // Si pas d'imprimante connectée, la fenêtre ne peut s'afficher
       if ( printDialog.exec() == QDialog::Accepted )
       {
          QTextBrowser FeuilleAImprimer ;

          //Création des formats de textes
          QTextCharFormat TailleTexte;
          TailleTexte.setFontPointSize(10);

          //Creation des QString
          FeuilleAImprimer.setCurrentCharFormat(TailleTexte);

          //Mise en place dans la feuille
          FeuilleAImprimer.append(ui->TxE_Contenu->toPlainText() + "\n");
          FeuilleAImprimer.setAlignment(Qt::AlignCenter);
          FeuilleAImprimer.setCurrentCharFormat(TailleTexte);
          FeuilleAImprimer.setAlignment(Qt::AlignCenter);
          FeuilleAImprimer.print(&Imprimante);
       }
    }
}


/**
 * @brief Méthode qui permet de quitter la fenêtre d'impression
 *
 */
void F_ImprimerMalle::on_Bt_Quitter_clicked()
{
    this->hide();
}
