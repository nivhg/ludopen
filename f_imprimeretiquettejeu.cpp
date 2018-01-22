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
void F_ImprimerEtiquetteJeu::ImprimerEtiquetteJeu(QString CodeJeu)
{
    
    this->show();
    
    QSqlQuery RequeteEtiquetteJeu ;
    RequeteEtiquetteJeu.prepare("SELECT NomJeu, CodeJeu, ContenuJeu FROM jeux WHERE CodeJeu =:CodeDuJeu") ;
    RequeteEtiquetteJeu.bindValue(":CodeDuJeu", CodeJeu);
    if(!RequeteEtiquetteJeu.exec())
    {
        qDebug() << "F_Jeux::ImprimerEtiquetteJeu()";
    }
    RequeteEtiquetteJeu.next() ;
    
    QString InfoLudo = F_Preferences::ObtenirValeur("Nom") + "\n" + F_Preferences::ObtenirValeur("Adresse")
            + "\n" + F_Preferences::ObtenirValeur("CodePostal") + "  " + F_Preferences::ObtenirValeur("Ville")
            + "\n" + F_Preferences::ObtenirValeur("NumeroTel") +"\n" ;
    QString NomJeu = RequeteEtiquetteJeu.value(0).toString();
    QString CodeDuJeu = RequeteEtiquetteJeu.value(1).toString() ;
    QString ContenuJeu = RequeteEtiquetteJeu.value(2).toString() ;
    
    ui->Lb_NomJeu->setText(NomJeu);
    ui->Lb_InfoLudo->setText(InfoLudo);
    ui->Le_CodeJeu->setText(CodeDuJeu);
    ui->TxE_Contenu->setText(ContenuJeu);
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
