//---------------------------------------------------
/** @file f_imprimeretiquettejeu.cpp
  * @brief Classe graphique permettant d'imprimer une étiquette de jeu
  *
  * @author Florian MARY
  * @author STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
  * @since 01/05/2012
  * @version 0.1
  * @date 20/04/2014  William
  *
  * Cette classe permet d'imprimer une étiquette de jeu avec les informations sur la ludothèque ainsi que le nom et le code du jeu
  *
  * Fabrication QtCreator, LudOpen
  *
  *
*/
//-------------------------------------------------------
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QPrinterInfo>

// En-tête propre à l'application ----------------------------------------------
#include "f_imprimeretiquettejeu.h"
#include "ui_f_imprimeretiquettejeu.h"


/**
 * @brief Constructeur de la classe f_imprimeretiquettejeu
 *
 * @param parent
 */
F_ImprimerEtiquetteJeu::F_ImprimerEtiquetteJeu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_ImprimerEtiquetteJeu)
{
    ui->setupUi(this);
}

/**
 * @brief Desctructeur de la classe f_imprimeretiquettejeu
 *
 */
F_ImprimerEtiquetteJeu::~F_ImprimerEtiquetteJeu()
{
    delete ui;
}
/**
 * @brief Méthode qui affiche les différentes informations à imprimer
 *
 * @param CodeJeu
 */
QString F_ImprimerEtiquetteJeu::ImprimerEtiquetteJeu(QString CodeJeu)
{
    
    this->show();
    
    QSqlQuery Requete;
    Requete.prepare("SELECT NomJeu,IdJeux FROM jeux WHERE CodeJeu=:CodeJeu");
    Requete.bindValue(":CodeJeu",CodeJeu);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    Requete.next();
    int IdJeux=ObtenirValeurParNom(Requete,"IdJeux").toInt();
    QString NomJeu=ObtenirValeurParNom(Requete,"NomJeu").toString();

    Requete.prepare("SELECT *,IF(PieceGroupe=2,"
                    "(SELECT CONCAT(OrdrePieces,'.',p.OrdrePieces+1) FROM pieces WHERE p.IdJeuxOuIdPieces=IdPieces),OrdrePieces)"
                    "as OrdreGroupePieces,(SELECT SUM(NombrePiecesManquantes) FROM piecesmanquantes WHERE "
                    "IdPieces=IdPieces_Pieces AND Abimee=0) as NombrePiecesManquantes FROM pieces as p WHERE "
                    "(IdJeuxOuIdPieces=:IdJeux AND PieceGroupe!=2) OR (PieceGroupe=2 AND IdJeuxOuIdPieces IN (SELECT IdPieces FROM pieces WHERE "
                    "IdJeuxOuIdPieces=:IdJeux)) ORDER By CONVERT(OrdreGroupePieces,DECIMAL(5,2))");

    Requete.bindValue(":IdJeux",IdJeux);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }

    int DernierIdGroupe=0;
    QString ContenuJeu;
    while(Requete.next())
    {
        if(ObtenirValeurParNom(Requete,"DescriptionPieces").toString().trimmed()=="")
        {
            ContenuJeu+="\n";
            continue;
        }
        int PieceGroupe=ObtenirValeurParNom(Requete,"PieceGroupe").toInt();
        QString NombrePieces="";
        if(!Requete.isNull("NombrePieces"))
        {
            NombrePieces=ObtenirValeurParNom(Requete,"NombrePieces").toString()+" ";
        }
        // S'il s'agit d'un groupe
        switch(PieceGroupe)
        {
            case PIECE_SEULE:
            case GROUPE:
            {
                ContenuJeu+="- "+NombrePieces+ObtenirValeurParNom(Requete,"DescriptionPieces").toString();
                if(PieceGroupe==PIECE_SEULE)
                {
                    ContenuJeu+="\n";
                }
                else
                {
                    ContenuJeu+=" :\n";
                }
                break;
            }
            case PIECE_DANS_GROUPE:
            {
                ContenuJeu+="\t* "+NombrePieces+ObtenirValeurParNom(Requete,"DescriptionPieces").toString()+"\n";
                break;
            }
        }
    }
    QString InfoLudo = F_Preferences::ObtenirValeur("Nom") + "\n" + F_Preferences::ObtenirValeur("Adresse")
            + "\n" + F_Preferences::ObtenirValeur("CodePostal") + "  " + F_Preferences::ObtenirValeur("Ville")
            + "\n" + F_Preferences::ObtenirValeur("NumeroTel") +"\n" ;
    
    ui->Lb_NomJeu->setText(NomJeu);
    ui->Lb_InfoLudo->setText(InfoLudo);
    ui->Le_CodeJeu->setText(CodeJeu);
    ui->TxE_Contenu->setText(ContenuJeu);
    return ContenuJeu;
}

/**
 * @brief Méthode qui lance la fenêtre d'impression et qui fait la mise en page
 *
 */
void F_ImprimerEtiquetteJeu::on_Bt_Imprimer_clicked()
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
         FeuilleAImprimer.append(this->ui->Lb_InfoLudo->text());
         FeuilleAImprimer.append( "<b>" + ui->Le_CodeJeu->text()+ " - " + ui->Lb_NomJeu->text()+ "</b>");
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
void F_ImprimerEtiquetteJeu::on_Bt_Quitter_clicked()
{
    this->hide();
}

