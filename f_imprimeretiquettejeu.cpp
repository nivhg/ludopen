//---------------------------------------------------
/** @file f_imprimeretiquettejeu.cpp
  * @brief Classe graphique permettant d'imprimer une étiquette de jeu
  *
  * @author Florian MARY
  * @author STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
  * @since 01/05/2012
  * @version 0.1
  * @date 15/08/2012
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
    RequeteEtiquetteJeu.prepare("SELECT Nom, NumeroTel, Adresse, CodePostal, Ville, NomJeu, CodeJeu, ContenuJeu FROM preferences, jeux WHERE CodeJeu =:CodeDuJeu") ;
    RequeteEtiquetteJeu.bindValue(":CodeDuJeu", CodeJeu);
    if(!RequeteEtiquetteJeu.exec())
    {
        qDebug() << "F_Jeux::ImprimerEtiquetteJeu()";
    }
    RequeteEtiquetteJeu.next() ;
    
    QString InfoLudo = RequeteEtiquetteJeu.value(0).toString() + "\n" + RequeteEtiquetteJeu.value(2).toString() + "\n" + RequeteEtiquetteJeu.value(3).toString()+ "  " + RequeteEtiquetteJeu.value(4).toString() + "\n" + RequeteEtiquetteJeu.value(1).toString() +"\n" ;
    QString NomJeu = RequeteEtiquetteJeu.value(5).toString() ;
    QString CodeDuJeu = RequeteEtiquetteJeu.value(6).toString() ;
    QString ContenuJeu = RequeteEtiquetteJeu.value(7).toString() ;
    
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
    QPrinter * Imprimante = new QPrinter(QPrinter::HighResolution);
    Imprimante->setPaperSize (QPrinter::A4);
    Imprimante->setOrientation(QPrinter::Landscape);
    Imprimante->setFullPage(true);

    QPrintDialog printDialog(Imprimante, this);
    if ( printDialog.exec() == 1)
    {
        QTextBrowser * editor = new QTextBrowser;

        //Création des formats de textes
        QTextCharFormat TailleTexte;
        TailleTexte.setFontPointSize(10);

        //Creation des QString
        editor->setCurrentCharFormat(TailleTexte);

        //Mise en place dans la feuille
        editor->append(this->ui->Lb_InfoLudo->text());
        editor->append( "<b>" + ui->Le_CodeJeu->text()+ " - " + ui->Lb_NomJeu->text()+ "</b>");
        editor->append(ui->TxE_Contenu->toPlainText() + "\n");
        editor->setAlignment(Qt::AlignCenter);
        editor->setCurrentCharFormat(TailleTexte);
        editor->setAlignment(Qt::AlignCenter);
        editor->print(Imprimante);
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

