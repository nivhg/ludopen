//-------------------------------------------------------------------------------
/** @file         f_imprimerfichecompletejeu.cpp
 *  @brief        Permet d'imprimer une fiche complète du jeu
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/06/2012
 *  @version      0.1
 *  @date         20/04/2014  William  Message d'erreur sir pas d'imprimante installée
 *
 *  Permet d'imprimer une fiche complète sur le jeu.
 *
 *  Fabrication   QtCreator
 *
 *
 *
 */
//-------------------------------------------------------------------------------
// En-têtes standards  ----------------------------------------------------------
/*
#include <QtSql>
#include <QPrintDialog>
#include <QPrinter>
*/
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QPrinterInfo>

// En-tête propre à l'application -----------------------------------------------
#include "f_imprimerfichecompletejeu.h"
#include "ui_f_imprimerfichecompletejeu.h"


/**
 * @brief Constructeur de la classe f_imprimerfichecompletejeu
 *
 * @param parent
 */
F_ImprimerFicheCompleteJeu::F_ImprimerFicheCompleteJeu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_ImprimerFicheCompleteJeu)
{
    ui->setupUi(this);
}

/**
 * @brief Destructeur de la classe f_imprimerfichecompletejeu
 *
 */
F_ImprimerFicheCompleteJeu::~F_ImprimerFicheCompleteJeu()
{
    delete ui;
}
/**
 * @brief Méthode qui donne un aperçu des différentes informations à imprimer
 *
 * @param CodeJeu
 */
void F_ImprimerFicheCompleteJeu::ImprimerFicheJeuComplete(QString CodeJeu)
{
    this->show() ;

    QSqlQuery RequeteFicheJeuComplete ;
    RequeteFicheJeuComplete.prepare("SELECT NomJeu, CodeJeu, NomCreateurJeu, DateAchat, PrixAchat, PrixLoc, NbrJoueurMin, NbrJoueurMax, AgeMin, AgeMax, Remarque, ContenuJeu, Caution, DescriptionJeu, EtatInitial, TypeJeux_Classification, Emplacement_IdEmplacement, EtatsJeu_IdEtatsJeu, StatutJeux_IdStatutJeux, Fournisseurs_IdFournisseur, Editeur_IdEditeur, CheminPhotoJeu FROM jeux WHERE CodeJeu =:CodeDuJeu") ;
    RequeteFicheJeuComplete.bindValue(":CodeDuJeu", CodeJeu) ;
    if(!RequeteFicheJeuComplete.exec())
    {
        qDebug() << "F_ImprimerFicheCompleteJeu::ImprimerFicheJeuComplete(QString CodeJeu) : " << RequeteFicheJeuComplete.lastQuery() ;
    }
    RequeteFicheJeuComplete.next() ;

    QString NomJeu = RequeteFicheJeuComplete.value(0).toString() ;
    ui->LE_Nom->setText(NomJeu) ;
    ui->LE_Nom->setReadOnly(true);
    
    QString Code = RequeteFicheJeuComplete.value(1).toString() ;
    ui->LE_Code->setText(Code);
    ui->LE_Code->setReadOnly(true);
    
    //QString NomCreateurJeu = RequeteFicheJeuComplete.value(2).toString();
    if(RequeteFicheJeuComplete.value(2).toString() == "")
    {
        ui->LE_Createur->setText("Information non renseigné");
    }
    else
    {
        ui->LE_Createur->setText(RequeteFicheJeuComplete.value(2).toString());
    }
    ui->LE_Createur->setReadOnly(true);
    
    QString DateAchat = RequeteFicheJeuComplete.value(3).toString() ;
    ui->LE_DateAchat->setText(DateAchat);
    ui->LE_DateAchat->setReadOnly(true);
    
    QString PrixAchat = RequeteFicheJeuComplete.value(4).toString() ;
    ui->LE_PrixAchat->setText(PrixAchat);
    ui->LE_PrixAchat->setReadOnly(true);
    
    QString PrixLoc = RequeteFicheJeuComplete.value(5).toString() ;
    ui->LE_PrixLoc->setText(PrixLoc);
    ui->LE_PrixLoc->setReadOnly(true);
    
    QString NbrJoueurMin = RequeteFicheJeuComplete.value(6).toString() ;
    ui->LE_JoueurMin->setText(NbrJoueurMin);
    ui->LE_JoueurMin->setReadOnly(true);
    
    QString NbrJoueurMax = RequeteFicheJeuComplete.value(7).toString() ;
    ui->LE_JoueurMax->setText(NbrJoueurMax);
    ui->LE_JoueurMax->setReadOnly(true);
    
    QString AgeMin = RequeteFicheJeuComplete.value(8).toString() ;
    ui->LE_AgeMin->setText(AgeMin);
    ui->LE_AgeMin->setReadOnly(true);
    
    QString AgeMax = RequeteFicheJeuComplete.value(9).toString() ;
    ui->LE_AgeMax->setText(AgeMax);
    ui->LE_AgeMax->setReadOnly(true);
    
    QString Remarques = RequeteFicheJeuComplete.value(10).toString() ;
    ui->TxE_Remarques->setText(Remarques);
    ui->TxE_Remarques->setReadOnly(true);
    
    QString Contenu = RequeteFicheJeuComplete.value(11).toString() ;
    ui->TxE_Contenu->setText(Contenu);
    ui->TxE_Contenu->setReadOnly(true);
    
    QString Caution = RequeteFicheJeuComplete.value(12).toString() ;
    ui->LE_Caution->setText(Caution);
    ui->LE_Caution->setReadOnly(true);
    
    QString Description = RequeteFicheJeuComplete.value(13).toString() ;
    ui->TxE_Description->setText(Description);
    ui->TxE_Description->setReadOnly(true);
    
    //QString EtatInitial = RequeteFicheJeuComplete.value(14).toString() ;
    if(RequeteFicheJeuComplete.value(14).toString() == "")
    {
        ui->LE_EtatInitial->setText("Information non renseigné");
    }
    else
    {
        ui->LE_EtatInitial->setText(RequeteFicheJeuComplete.value(14).toString());
    }
    ui->LE_EtatInitial->setReadOnly(true);
    


    QSqlQuery RequeteClassification ;
    QString Classification = RequeteFicheJeuComplete.value(15).toString() ;
    RequeteClassification.prepare("SELECT TypeJeux, Classification FROM typejeux WHERE Classification =:NumClassification") ;
    RequeteClassification.bindValue(":NumClassification", Classification);
    RequeteClassification.exec() ;
    RequeteClassification.next() ;
    if(RequeteClassification.value(1).toString() == "" || RequeteClassification.value(1).toString() == 0)
    {
        ui->LE_Classification->setText("Information non renseigné");
    }
    else
    {
        QString NomClassification = RequeteClassification.value(0).toString() ;
        QString AffichageClassification =  Classification + ": " + NomClassification ;
        ui->LE_Classification->setText(AffichageClassification);
    }
    ui->LE_Classification->setReadOnly(true);
    


    QSqlQuery RequeteEmplacement ;
    QString Emplacement = RequeteFicheJeuComplete.value(16).toString() ;
    RequeteEmplacement.prepare("SELECT IdEmplacement, Nom FROM emplacement WHERE IdEmplacement =:idDeEmplacement") ;
    RequeteEmplacement.bindValue(":idDeEmplacement", Emplacement);
    RequeteEmplacement.exec() ;
    RequeteEmplacement.next() ;
    if(RequeteEmplacement.value(1).toString() == "" || RequeteEmplacement.value(1).toString() == 0)
    {
        ui->LE_Emplacement->setText("Information non renseigné");
    }
    else
    {
         ui->LE_Emplacement->setText(RequeteEmplacement.value(1).toString());
    }
    ui->LE_Emplacement->setReadOnly(true);
    

    QSqlQuery RequeteEtat ;
    QString Etat = RequeteFicheJeuComplete.value(17).toString() ;
    RequeteEtat.prepare("SELECT IdEtatsJeu, Etat FROM etatsjeu WHERE IdEtatsJeu =:IdEtatsDuJeu") ;
    RequeteEtat.bindValue(":IdEtatsDuJeu", Etat);
    RequeteEtat.exec() ;
    RequeteEtat.next() ;
    if(RequeteEtat.value(1).toString() == "" || RequeteEtat.value(1).toString() == 0)
    {
        ui->LE_Etat->setText("Information non renseigné");
    }
    else
    {
        ui->LE_Etat->setText(RequeteEtat.value(1).toString());
    }
    ui->LE_Etat->setReadOnly(true);
    

    QSqlQuery RequeteStatut ;
    QString Statut = RequeteFicheJeuComplete.value(18).toString() ;
    RequeteStatut.prepare("SELECT IdStatutJeux, StatutJeu FROM statutjeux WHERE IdStatutJeux =:IdStatutDuJeux") ;
    RequeteStatut.bindValue(":IdStatutDuJeux", Statut);
    RequeteStatut.exec() ;
    RequeteStatut.next() ;
    if(RequeteStatut.value(1).toString() == "" || RequeteStatut.value(1).toString() == 0)
    {
        ui->LE_Statut->setText("Information non renseigné");
    }
    else
    {
        ui->LE_Statut->setText(RequeteStatut.value(1).toString());
    }
    ui->LE_Statut->setReadOnly(true);

    
    QSqlQuery RequeteFournisseur ;
    QString Fournisseur = RequeteFicheJeuComplete.value(19).toString() ;
    RequeteFournisseur.prepare("SELECT IdFournisseur, NomFournisseur, Fournisseur FROM fournisseursediteur WHERE IdFournisseur =:IdDuFournisseur AND Fournisseur = 1") ;
    RequeteFournisseur.bindValue(":IdDuFournisseur", Fournisseur);
    RequeteFournisseur.exec() ;
    RequeteFournisseur.next() ;
    if(RequeteFournisseur.value(1).toString() == "" || RequeteFournisseur.value(1).toString() == 0)
    {
        ui->LE_Fournisseur->setText("Information non renseigné");
    }
    else
    {
        ui->LE_Fournisseur->setText(RequeteFournisseur.value(1).toString());
    }
    ui->LE_Fournisseur->setReadOnly(true);
    
    QSqlQuery RequeteEditeur ;
    QString Editeur = RequeteFicheJeuComplete.value(20).toString() ;
    RequeteEditeur.prepare("SELECT IdFournisseur, NomFournisseur, Editeur FROM fournisseursediteur WHERE IdFournisseur =:IdDuFournisseur AND Editeur = 1") ;
    RequeteEditeur.bindValue(":IdDuFournisseur", Editeur);
    RequeteEditeur.exec() ;
    RequeteEditeur.next() ;
    if(RequeteEditeur.value(1).toString() == "" || RequeteEditeur.value(1).toString() == 0)
    {
        ui->LE_Editeur->setText("Information non renseigné");
    }
    else
    {
        ui->LE_Editeur->setText(RequeteEditeur.value(1).toString());
    }
    ui->LE_Editeur->setReadOnly(true);

    /*
    QSqlQuery RequeteImage ;
    QString CheminPhotosJeux ;
    
    RequeteImage.prepare("SELECT CheminPhotoJeu, CodeJeu FROM jeux WHERE CodeJeu = :CodeDuJeu") ;
    RequeteImage.bindValue(":CodeDuJeu", ui->LE_Code->text());
    RequeteImage.exec() ;
    RequeteImage.next() ;
    */

    ////////////////////////////////////////////////////////
    ////////////// Photo //////////////////////////////////
    //////////////////////////////////////////////////////

    QPixmap Image;
    QString TypeImage;

    QString sCheminImage=QApplication::applicationDirPath() + "/photos/" + ui->LE_Code->text() ;

    if( QFile::exists( sCheminImage + ".jpg" ) )
    {
        sCheminImage = sCheminImage + ".jpg" ;
        TypeImage="JPG";
    }
    else
    if( QFile::exists( sCheminImage + ".jpeg" ) )
    {
        sCheminImage = sCheminImage + ".jpeg" ;
        TypeImage="JPEG";
    }
    else
    if( QFile::exists( sCheminImage + ".png" ) )
    {
        sCheminImage = sCheminImage + ".png" ;
        TypeImage="PNG";
    }
    else
    if( QFile::exists( sCheminImage + ".bmp" ) )
    {
        sCheminImage = sCheminImage + ".bmp" ;
        TypeImage="BMP";
    }
    else
    if( QFile::exists( sCheminImage + ".gif" ) )
    {
        sCheminImage = sCheminImage + ".gif" ;
        TypeImage="GIF";
    }

    QDir CheminFichierImage( sCheminImage );
    // si le chemin est faux ou l'image n'existe pas, efface l'image d'avant automatiquement
    if ( Image.load(CheminFichierImage.absolutePath(),TypeImage.toLocal8Bit().data()) )
    {
       ui->Lb_Photo->setPixmap( Image ) ;
       //Met l'image à l'échelle du cadre
       ui->Lb_Photo->setScaledContents( true ) ;
       qDebug()<< "F_Jeux::on_Bt_ok_clicked() =>  sCheminImage=" <<  CheminFichierImage.absolutePath() << "TypeImage" << TypeImage.toLocal8Bit().data();
    }
    else
    {
       ui->Lb_Photo->setText("Photo n°1\nindisponible");
    }
}

/**
 * @brief Méthode qui lance la fenêtre d'impression et qui fait la mise en page des différentes informations
 *
 */
void F_ImprimerFicheCompleteJeu::on_Bt_Imprimer_clicked()
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
      if ( printDialog.exec() == QDialog::Accepted)
      {
        QTextBrowser FeuilleAImprimer ;

        //Création des formats de textes
        QTextCharFormat TailleTexte;
        TailleTexte.setFontPointSize(12);

        //Creation des QString
        FeuilleAImprimer.setCurrentCharFormat(TailleTexte);

        //Mise en place dans la feuille
        FeuilleAImprimer.append(this->ui->Lb_Nom->text() + ": " + "<b>" + this->ui->LE_Nom->text() + "</b>") ;

        FeuilleAImprimer.append(this->ui->Lb_Code->text() + ": " +"<b>" + this->ui->LE_Code->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_Classification->text() + ": " + "<b>" + this->ui->LE_Classification->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_AgeMin->text() + ": " + "<b>" + this->ui->LE_AgeMin->text() + "</b>" + " - " + this->ui->Lb_AgeMax->text() + ": " + "<b>" + this->ui->LE_AgeMax->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_JoueurMin->text() + ": " + "<b>" + this->ui->LE_JoueurMin->text() + "</b>" + " - " + this->ui->Lb_JoueurMax->text() + ": " + "<b>" + this->ui->LE_JoueurMax->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_Createur->text() + ": " + "<b>" + this->ui->LE_Createur->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_Editeur->text() + ": " + "<b>" + this->ui->LE_Editeur->text() + "</b>");

        FeuilleAImprimer.append(this->ui->Lb_Fournisseur->text() + ": " + "<b>" + this->ui->LE_Fournisseur->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_Statut->text() + ": " + "<b>" + this->ui->LE_Statut->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_Etat->text() + ": " + "<b>" + this->ui->LE_Etat->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_EtatInitial->text() + ": " + "<b>" + this->ui->LE_EtatInitial->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_DateAchat->text() + ": " + "<b>" + this->ui->LE_DateAchat->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_PrixAchat->text() + ": " + "<b>" + this->ui->LE_PrixAchat->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_PrixLoc->text() + ": " + "<b>" + this->ui->LE_PrixLoc->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_Caution->text() + ": " + "<b>" + this->ui->LE_Caution->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_Emplacement->text() + ": " + "<b>" + this->ui->LE_Emplacement->text() + "</b>" + "\n");

        FeuilleAImprimer.append(this->ui->Lb_Contenu->text() + ": " + "\n");
        FeuilleAImprimer.append("<b>" + this->ui->TxE_Contenu->toPlainText() + "</b" + "\n");

        FeuilleAImprimer.setAlignment(Qt::AlignCenter);
        FeuilleAImprimer.setCurrentCharFormat(TailleTexte);
        FeuilleAImprimer.setAlignment(Qt::AlignCenter);

        FeuilleAImprimer.print(&Imprimante);
      }
   }
}
/**
 * @brief Méthode qui quitte la fenêtre d'aperçu des informations
 *
 */
void F_ImprimerFicheCompleteJeu::on_Bt_Quitter_clicked()
{
    this->hide() ;
}
