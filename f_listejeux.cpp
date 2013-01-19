//-------------------------------------------------------------------------------
/** @file         f_listejeux.cpp
 *  @brief        Permet de consulter la liste de tout les jeux
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         19/01/2013
 *  @author       William SOREL
 *
 *  Permet de consulter la liste de tout les jeux ainsi que de faire un filtre par crit?res
 *
 *  Fabrication   QtCreator
 *  @todo         Tout reprendre car ça utilise 100% du processeur
 *  @bug          Tout reprendre car ça utilise 100% du processeur
 *
 */
//-------------------------------------------------------------------------------
#include <QtSql>
#include <QFileDialog>

// EN-TETES UNIQUEMENT UTILISÉS DANS CE FICHIER ---------------------------------
#include "f_listejeux.h"
#include "ui_f_listejeux.h"


/**
 * @brief Constructeur de la classe f_listejeux
 *
 * @param parent
 */
F_ListeJeux::F_ListeJeux(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_ListeJeux)
{
    ui->setupUi(this);
    
    //////////////////Création table view //////////////////

    //Création d'un modèle pour le TableView des jeux
    this->ModelJeu = new QStandardItemModel() ;
    //Associe le modèle au TableView
    ui->TbV_Recherche->setModel(this->ModelJeu);
    // Mise en lecture seule
    ui->TbV_Recherche->setEditTriggers(0);
    // Initialise la table view avec tous les jeux
    //on_LE_Nom_textChanged("") ;
    //Affiche les noms des colonnes dans le tableau
    ui->TbV_Recherche->horizontalHeader()->setVisible(true);
    // Permettre le tri des colonnes
    ui->TbV_Recherche->setSortingEnabled(true);
    //Indique le nom des colones
    this->ModelJeu->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModelJeu->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
    this->ModelJeu->setHorizontalHeaderItem(2, new QStandardItem("Classification"));
    this->ModelJeu->setHorizontalHeaderItem(3, new QStandardItem("Nbr Joueurs Min"));
    this->ModelJeu->setHorizontalHeaderItem(4, new QStandardItem("Nbr Joueurs Max"));
    this->ModelJeu->setHorizontalHeaderItem(5, new QStandardItem("Age Min"));
    this->ModelJeu->setHorizontalHeaderItem(6, new QStandardItem("Age Max"));
    this->ModelJeu->setHorizontalHeaderItem(7, new QStandardItem("Date Acquisition"));
    //impose une taille aux colones
    ui->TbV_Recherche->setColumnWidth(0,50);
    ui->TbV_Recherche->setColumnWidth(1,200);
    ui->TbV_Recherche->setColumnWidth(2,100);
    ui->TbV_Recherche->setColumnWidth(3,120);
    ui->TbV_Recherche->setColumnWidth(4,120);
    ui->TbV_Recherche->setColumnWidth(5,80);
    ui->TbV_Recherche->setColumnWidth(6,80);
    ui->TbV_Recherche->setColumnWidth(7,120);

    //Rempli tous les crit?res combobox
    this->RAZCriteres();
}

/**
 * @brief Destructeur de la classe f_listejeux
 *
 */
F_ListeJeux::~F_ListeJeux()
{
    delete ui;
}

/**
 * @brief Méthode qui modifie le contenu du tableau en fonction de ce que l'on rentre dans la barre de recherche par nom
 *
 * @param arg1
 */

void F_ListeJeux::on_LE_Nom_textChanged(const QString &arg1)
{
    int NumeroLigne (0);
    QString NomJeu=arg1;

    if(NomJeu.size()>= 2)
    {
        NomJeu="%"+NomJeu+"%";
        QSqlQuery RequeteRechercheJeu;
        NumeroLigne=0;

        RequeteRechercheJeu.prepare("SELECT CodeJeu, NomJeu, TypeJeux_Classification, NbrJoueurMin, NbrJoueurMax, AgeMin, AgeMax, DateAchat FROM jeux WHERE NomJeu LIKE (:NomJeu)");
        RequeteRechercheJeu.bindValue(":NomJeu",NomJeu);
        RequeteRechercheJeu.exec();

        //On vide le modèle
        this->ModelJeu->removeRows(0,this->ModelJeu->rowCount());

        //Tant qu'il y a des jeux dans la table jeux,
        while(RequeteRechercheJeu.next())
        {
            //on ajoute une nouvelle ligne du table view
            this->ModelJeu->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheJeu.value(0).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 1, new QStandardItem(RequeteRechercheJeu.value(1).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 2, new QStandardItem(RequeteRechercheJeu.value(2).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 3, new QStandardItem(RequeteRechercheJeu.value(3).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 4, new QStandardItem(RequeteRechercheJeu.value(4).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 5, new QStandardItem(RequeteRechercheJeu.value(5).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 6, new QStandardItem(RequeteRechercheJeu.value(6).toString() ));
            this->ModelJeu->setItem(NumeroLigne, 7, new QStandardItem(RequeteRechercheJeu.value(7).toString() ));
            NumeroLigne++;
         }
        ui->Lb_Resultat->setNum( NumeroLigne ) ;
    }

    //RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu des index
 *
 */
void F_ListeJeux::RecupererContenuIndex()
{
    QString FiltreJeux ;
    QString ListeJeux ;
    bool PremierCritere (false) ;
    QSqlQuery RequeteFiltreJeux ;
    FiltreJeux = "SELECT DateAchat, PrixLoc, MotCle1, MotCle2, MotCle3, NbrJoueurMin, NbrJoueurMax, AgeMin, AgeMax, EtatsJeu_IdEtatsJeu, StatutJeux_IdStatutJeux, TypeJeux_Classification, NomJeu, CodeJeu FROM jeux, etatsjeu, motscles, statutjeux, typejeux " ;

    if(ui->CBx_DateAcquisition->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " DateAchat = " ;
        ListeJeux += " \""+ ui->CBx_DateAcquisition->currentText()+" \"" ;
        PremierCritere = true ;
    }
    if(ui->CBx_PrixLoc->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " PrixLoc = " ;
        ListeJeux += ui->CBx_PrixLoc->currentText() ;
        PremierCritere = true ;
    }
    if(ui->CBx_JoueursMin->currentIndex() != 0)
    {
        qDebug("joueur nim !=0");
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " NbrJoueurMin >= " ;
        ListeJeux += ui->CBx_JoueursMin->currentText() ;
        PremierCritere = true ;
    }
    if(ui->CBx_JoueursMax->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " NbrJoueurMax <= " ;
        ListeJeux += ui->CBx_JoueursMax->currentText() ;
        PremierCritere = true ;
    }
    if(ui->CBx_AgeMin->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " AgeMin >= " ;
        ListeJeux += ui->CBx_AgeMin->currentText() ;
        PremierCritere = true ;
    }
    if(ui->CBx_AgeMax->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " AgeMax <= " ;
        ListeJeux += ui->CBx_AgeMax->currentText() ;
        PremierCritere = true ;
    }
    if(ui->CBX_Etat->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " EtatsJeu_IdEtatsJeu = IdEtatsJeu AND Etat = " ;
        ListeJeux += " \""+ui->CBX_Etat->currentText()+" \"" ;
        PremierCritere = true ;
    }
    
    if(ui->CBx_MotCle1->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " MotCle1 = Id_MotCle AND MotCle =" ;
        ListeJeux += " \""+ui->CBx_MotCle1->currentText()+" \"" ;
        
        ListeJeux += " OR MotCle2 = Id_MotCle AND MotCle =" ;
        ListeJeux += " \""+ui->CBx_MotCle1->currentText()+" \"" ;
        
        ListeJeux += " OR MotCle3 = Id_MotCle AND MotCle =" ;
        ListeJeux += " \""+ui->CBx_MotCle1->currentText()+" \"" ;
        
        PremierCritere = true ;
    }
    if(ui->CBx_MotCle2->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " MotCle2 = Id_MotCle AND MotCle = " ;
        ListeJeux += " \""+ui->CBx_MotCle2->currentText()+" \"" ;
        
        ListeJeux += " OR MotCle1 = Id_MotCle AND MotCle = " ;
        ListeJeux += " \""+ui->CBx_MotCle2->currentText()+" \"" ;
        
        ListeJeux += " OR MotCle3 = Id_MotCle AND MotCle = " ;
        ListeJeux += " \""+ui->CBx_MotCle2->currentText()+" \"" ;
        
        PremierCritere = true ;
    }
    if(ui->CBx_MotCle3->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " MotCle3 = Id_MotCle AND MotCle = " ;
        ListeJeux += " \""+ui->CBx_MotCle3->currentText()+" \"" ;
        
        ListeJeux += " OR MotCle2 = Id_MotCle AND MotCle = " ;
        ListeJeux += " \""+ui->CBx_MotCle3->currentText()+" \"" ;
        
        ListeJeux += " OR MotCle1 = Id_MotCle AND MotCle = " ;
        ListeJeux += " \""+ui->CBx_MotCle3->currentText()+" \"" ;
        PremierCritere = true ;
    }
    
    if(ui->CBx_Statut->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " StatutJeux_IdStatutJeux = IdStatutJeux AND StatutJeu = " ;
        ListeJeux += " \""+ui->CBx_Statut->currentText()+" \"" ;
        PremierCritere = true ;
    }
    if(ui->CBx_Classification->currentIndex() != 0)
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " TypeJeux_Classification = " ;
        ListeJeux += " \""+ui->CBx_Classification->currentText().left(ui->CBx_Classification->currentText().indexOf(":")) +" \"" ;
        PremierCritere = true ;
    }
    if(ui->LE_Nom->text() != "")
    {
        if(PremierCritere == true)
        {
            ListeJeux += " AND " ;
        }
        ListeJeux += " NomJeu = " ;
        ListeJeux += " \"% "+ui->LE_Nom->text()+" % \"" ;
        PremierCritere = true ;
    }

    if(PremierCritere == true)
    {
        FiltreJeux += "WHERE " ;
    }
    
    ListeJeux += " GROUP BY NomJeu " ;
    FiltreJeux += ListeJeux ;
    qDebug() << "F_ListeJeux::RecupererContenuIndex()- Requete finale =" << FiltreJeux ;
    if(!RequeteFiltreJeux.exec(FiltreJeux))
    {
        qDebug() << "F_ListeJeux::RecupererContenuIndex()" << RequeteFiltreJeux.lastError() ;
    }
    
    
    int NumeroLigne (0);

    //On vide le modèle
    this->ModelJeu->removeRows(0,this->ModelJeu->rowCount());

    //Tant qu'il y a des jeux dans la table jeux,
    while(RequeteFiltreJeux.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModelJeu->setItem(NumeroLigne, 0, new QStandardItem(RequeteFiltreJeux.value(13).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 1, new QStandardItem(RequeteFiltreJeux.value(12).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 2, new QStandardItem(RequeteFiltreJeux.value(11).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 3, new QStandardItem(RequeteFiltreJeux.value(5).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 4, new QStandardItem(RequeteFiltreJeux.value(6).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 5, new QStandardItem(RequeteFiltreJeux.value(7).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 6, new QStandardItem(RequeteFiltreJeux.value(8).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 7, new QStandardItem(RequeteFiltreJeux.value(0).toString() ));
        NumeroLigne++;
     }
     ui->Lb_Resultat->setNum( NumeroLigne ) ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_DateAcquisition
 *
 * @param index
 */
void F_ListeJeux::on_CBx_DateAcquisition_activated(int index)
{
    RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_Etat
 *
 * @param index
 */
void F_ListeJeux::on_CBX_Etat_activated(int index)
{
    RecupererContenuIndex() ;
}
/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_AgeMax
 *
 * @param index
 */
void F_ListeJeux::on_CBx_AgeMax_activated(int index)
{
    RecupererContenuIndex() ;
}
/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_JoueursMax
 *
 * @param index
 */
void F_ListeJeux::on_CBx_JoueursMax_activated(int index)
{
    RecupererContenuIndex() ;
}
/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_JoueursMin
 *
 * @param index
 */
void F_ListeJeux::on_CBx_JoueursMin_activated(int index)
{
    RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_AgeMin
 *
 * @param index
 */
void F_ListeJeux::on_CBx_AgeMin_activated(int index)
{
    RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_PrixLoc
 *
 * @param index
 */
void F_ListeJeux::on_CBx_PrixLoc_activated(int index)
{
    RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_Statut
 *
 * @param index
 */
void F_ListeJeux::on_CBx_Statut_activated(int index)
{
    RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_Classification
 *
 * @param index
 */
void F_ListeJeux::on_CBx_Classification_activated(int index)
{
    RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_MotCle1
 *
 * @param index
 */
void F_ListeJeux::on_CBx_MotCle1_activated(int index)
{
    RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_MotCle2
 *
 * @param index
 */
void F_ListeJeux::on_CBx_MotCle2_activated(int index)
{
    RecupererContenuIndex() ;
}

/**
 * @brief Méthode qui récupère le contenu de l'index lors de la  modification du contenu du CBx_MotCle3
 *
 * @param index
 */
void F_ListeJeux::on_CBx_MotCle3_activated(int index)
{
    RecupererContenuIndex() ;
}

void F_ListeJeux::on_Bt_ExporterListe_clicked()
{
    int nNombreColonne (0);
    int nNombreLigne (0);

    QString sCaractere;
    QString nomFichier = QFileDialog::getSaveFileName(this, "Enregistrer sous ...", "ListeJeux.csv");
    QFile fichier(nomFichier);
    QTextStream ecrire (&fichier);
    fichier.open(QIODevice::WriteOnly);

    ecrire << "Code,Nom,Classification,JoueursMin,JoueursMax,AgeMin,AgeMax,DateAchat\r\n" ;
    for(nNombreLigne = 0; nNombreLigne<ui->TbV_Recherche->model()->rowCount(); nNombreLigne++)
    {
        for(nNombreColonne = 0 ; nNombreColonne<ui->TbV_Recherche->model()->columnCount(); nNombreColonne++)
        {
            sCaractere = ui->TbV_Recherche->model()->data( ui->TbV_Recherche->model()->index( nNombreLigne, nNombreColonne ) ).toString() ;
            // On rejete les valeurs ? caract?re unique et on le remplace par un champs vide
            sCaractere.replace(" ", "\ ") ;
            if(sCaractere == "-" || sCaractere == "_" || sCaractere == ".")
            {
                sCaractere = "";
            }
            ecrire << "\"" << sCaractere << "\",";
        }
        ecrire << "\r\n";
        nNombreColonne = 0;
    }
}

/**
 * @brief Méthode qui remet tous les critères à zéro et donc ré-affiche la tableau au complet
 *
 */
void F_ListeJeux::on_bt_RAZ_clicked()
{
    this->RAZCriteres();
    // Effacer le nom du jeu choisi
    ui->LE_Nom->clear();
}

/**
 * Quand double clic sur un jeu, afficher l'onglet jeu avec le jeu concerné pour avoir le détail
 * envoi un signal qui sera reçu par la MainWindow
 * @param index Ligne choisie dans le modle associé au tableau TbV_Recherche
 */
void F_ListeJeux::on_TbV_Recherche_doubleClicked(const QModelIndex &index)
{
     emit( this->Signal_DoubleClic_ListeJeux( this->ModelJeu->index(index.row(), 0).data().toString())) ;
}

/** Remise à zéro des critères
 */
void F_ListeJeux::RAZCriteres()
{
    qDebug()<<"F_ListeJeux::RAZCriteres() - Début";
    ///////////////////////////////////////////////
    ////////// VIDER LES COMBO BOX /////////////
    /////////////////////////////////////////////
    ui->CBx_AgeMin->clear();
    ui->CBx_AgeMax->clear();
    ui->CBx_Classification->clear();
    ui->CBx_DateAcquisition->clear();
    ui->CBX_Etat->clear();
    ui->CBx_JoueursMax->clear();
    ui->CBx_JoueursMin->clear();
    ui->CBx_MotCle1->clear();
    ui->CBx_MotCle2->clear();
    ui->CBx_MotCle3->clear();
    ui->CBx_PrixLoc->clear();
    ui->CBx_Statut->clear();

    // Mettre le titre pour chaque liste déroulante
    ui->CBx_AgeMin->addItem("Age mini");
    ui->CBx_AgeMax->addItem("Age maxi");
    ui->CBx_Classification->addItem("Classification");
    ui->CBx_DateAcquisition->addItem("Date d'achat");
    ui->CBX_Etat->addItem("Etat");
    ui->CBx_JoueursMax->addItem("Nb joueur mini");
    ui->CBx_JoueursMin->addItem("Nb joueur maxi");
    ui->CBx_MotCle1->addItem("Mot clé 1");
    ui->CBx_MotCle2->addItem("Mot clé 2");
    ui->CBx_MotCle3->addItem("Mot clé 3");
    ui->CBx_PrixLoc->addItem("Prix location");
    ui->CBx_Statut->addItem("Statut");

    ////////////////////////////////////
    //////// Remplir EtatJeu ///////////
    ////////////////////////////////////

    QSqlQuery RequeteEtatJeu ;

    RequeteEtatJeu.exec("SELECT `Etat` FROM `etatsjeu`");

    //Tant qu'il y a différents états dans la table etatsjeu,
   while(RequeteEtatJeu.next())
   {
        //on entre un nouveau Item au ComboBox avec le nom de l'état
        QString EtatJeu=(RequeteEtatJeu.value(0).toString()) ;
        ui->CBX_Etat->addItem(EtatJeu);
   }
   /////////////////////////////////////////
   /////////////////////////////////////////
   /////////Remplir StatutJeu//////////////
   ///////////////////////////////////////
   QSqlQuery RequeteStatutJeu ;

  RequeteStatutJeu.exec("SELECT `StatutJeu` FROM `statutjeux`");

//Tant qu'il y a différents états dans la table jeux,
  while(RequeteStatutJeu.next())
  {
      //on entre un nouveau Item au ComboBox avec le nom du statut
      QString StatutJeu=(RequeteStatutJeu.value(0).toString()) ;
      ui->CBx_Statut->addItem(StatutJeu);
  }

  ////////////////////////////////////////
  ///////////////////////////////////////
  ///////// Age Maximum ///////////////
  ///////////////////////////////////

    QSqlQuery RequeteAgeMax ;

    RequeteAgeMax.exec("SELECT `AgeMax` FROM `jeux` GROUP BY `AgeMax` ") ;

    //Tant qu'il y a différents ages dans la table jeux,
    while(RequeteAgeMax.next())
    {
        //on entre un nouveau Item au ComboBox avec l'age maximum
        QString AgeMax = (RequeteAgeMax.value(0).toString()) ;
        ui->CBx_AgeMax->addItem(AgeMax);
    }

    /////////////////////////////////////////
    ////////////////////////////////////////
    /////////Age Minimum //////////////////
    //////////////////////////////////////

    QSqlQuery RequeteAgeMin ;

    RequeteAgeMin.exec("SELECT `AgeMin` FROM `jeux` GROUP BY `AgeMin` ") ;

    //Tant qu'il y a différents ages dans la table jeux,
    while(RequeteAgeMin.next())
    {
        //on entre un nouveau Item au ComboBox avec l'age minimum
        QString AgeMin = (RequeteAgeMin.value(0).toString()) ;
        ui->CBx_AgeMin->addItem(AgeMin);
    }
    /////////////////////////////////////////
    ////////////////////////////////////////
    ////////// NbreJoueurs Min ////////////
    //////////////////////////////////////

    QSqlQuery RequeteNbrJoueurMin ;

    RequeteNbrJoueurMin.exec("SELECT `NbrJoueurMin` FROM `jeux` GROUP BY `NbrJoueurMin`") ;

    //Tant qu'il y a différents nombres de joueurs mininmum dans la table jeux,

    while(RequeteNbrJoueurMin.next())
    {
        //on entre un nouveau Item au ComboBox avec le nombre de joueurs min
        QString NbrJoueurMin = (RequeteNbrJoueurMin.value(0).toString()) ;
        ui->CBx_JoueursMin->addItem(NbrJoueurMin);
    }

    /////////////////////////////////////////
    ////////////////////////////////////////
    ////////// NbreJoueurs Max ////////////
    //////////////////////////////////////

    QSqlQuery RequeteNbrJoueurMax ;

    RequeteNbrJoueurMax.exec("SELECT `NbrJoueurMax` FROM `jeux` GROUP BY `NbrJoueurMax`") ;

    //Tant qu'il y a différents nombres de joueurs maximum dans la table jeux,

    while(RequeteNbrJoueurMax.next())
    {
        //on entre un nouveau Item au ComboBox avec le nombre de joueurs min
        QString NbrJoueurMax = (RequeteNbrJoueurMax.value(0).toString()) ;
        ui->CBx_JoueursMax->addItem(NbrJoueurMax);
    }

    ////////////////////////////////////////////
    ///////// Classification //////////////////
    //////////////////////////////////////////

    QSqlQuery RequeteClassification ;

    RequeteClassification.exec("SELECT TypeJeux, Classification FROM typejeux ORDER BY Classification ASC") ;

    while(RequeteClassification.next())
    {

        QString NomClassification = (RequeteClassification.value(0).toString()) ;
        QString CodeClassification = (RequeteClassification.value(1).toString()) ;
        QString AffichageCBx = CodeClassification + ": " + NomClassification ;
        ui->CBx_Classification->addItem(AffichageCBx);
    }

    /////////////////////////////////////
    /////// Date Acquisition ///////////
    ///////////////////////////////////

    QSqlQuery RequeteDateAcquisition ;

    RequeteDateAcquisition.exec("SELECT DateAchat FROM jeux GROUP BY DateAchat") ;

    while(RequeteDateAcquisition.next())
    {
        QString DateAcquisition = (RequeteDateAcquisition.value(0).toString()) ;
        ui->CBx_DateAcquisition->addItem(DateAcquisition);
    }

    ////////////////////////////////////
    ///////// Mots Clés ///////////////
    //////////////////////////////////

    QSqlQuery RequeteMotsCle ;

    RequeteMotsCle.exec("SELECT MotCle FROM motscles GROUP BY MotCle") ;

    while(RequeteMotsCle.next())
    {
        QString MotCle1 = (RequeteMotsCle.value(0).toString()) ;
        QString MotCle2 = (RequeteMotsCle.value(0).toString()) ;
        QString MotCle3 = (RequeteMotsCle.value(0).toString()) ;
        ui->CBx_MotCle1->addItem(MotCle1);
        ui->CBx_MotCle2->addItem(MotCle2);
        ui->CBx_MotCle3->addItem(MotCle3);
    }

    ////////////////////////////////////
    ///////// Prix ////////////////////
    //////////////////////////////////
    QSqlQuery RequetePrixLoc ;

    RequetePrixLoc.exec("SELECT PrixLoc FROM jeux GROUP BY PrixLoc") ;
    while(RequetePrixLoc.next())
    {
        QString PrixLoc = (RequetePrixLoc.value(0).toString()) ;
        ui->CBx_PrixLoc->addItem(PrixLoc);
    }
    ///////////////////////////////////////////////////
    //     REMPLIR LE TABLEAU AVEC TOUS LES JEUX     //
    ///////////////////////////////////////////////////
    QSqlQuery RequeteRechercheJeu;
    int NumeroLigne =0;

    RequeteRechercheJeu.exec("SELECT CodeJeu, NomJeu, TypeJeux_Classification, NbrJoueurMin, NbrJoueurMax, AgeMin, AgeMax, DateAchat FROM jeux ORDER BY NomJeu");

    //On vide le modèle
    this->ModelJeu->removeRows(0,this->ModelJeu->rowCount());

    //Tant qu'il y a des jeux dans la table jeux,
    while(RequeteRechercheJeu.next())
    {
        //on ajoute une nouvelle ligne du table view
        this->ModelJeu->setItem(NumeroLigne, 0, new QStandardItem(RequeteRechercheJeu.value(0).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 1, new QStandardItem(RequeteRechercheJeu.value(1).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 2, new QStandardItem(RequeteRechercheJeu.value(2).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 3, new QStandardItem(RequeteRechercheJeu.value(3).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 4, new QStandardItem(RequeteRechercheJeu.value(4).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 5, new QStandardItem(RequeteRechercheJeu.value(5).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 6, new QStandardItem(RequeteRechercheJeu.value(6).toString() ));
        this->ModelJeu->setItem(NumeroLigne, 7, new QStandardItem(RequeteRechercheJeu.value(7).toString() ));
        NumeroLigne++;
    }

    ui->Lb_Resultat->setNum( NumeroLigne ) ;
    qDebug()<<"F_ListeJeux::RAZCriteres() - Fin";
}
