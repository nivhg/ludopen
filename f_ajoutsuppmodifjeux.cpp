//-------------------------------------------------------------------------------
/** @file         f_ajoutsuppmodifjeux.cpp
 *  @brief        Permet d'ajouter, supprimer ou modifier un jeux
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         03/02/2013 Nicolas
 *  Il permet à partir de l'onglet administration de supprimer un jeu, d'en ajouter un ou encore de le modifier.
 *
 *  Fabrication   QtCreator
 *  @date         27/03/2014 William Remise en forme de l'IHM
 *  @todo         Choisir les 3 images à mettre dans la BDD
 *
 */
//-------------------------------------------------------------------------------

// En-têtes standards  --------------
#include <qfiledialog.h>
#include <QtSql>
#include <QtWidgets>
#include <QMessageBox>

// En-tête propre à l'application ----------------------------------------------
#include "f_ajoutsuppmodifjeux.h"
#include "ui_f_ajoutsuppmodifjeux.h"
#include "f_ajoutsuppmodiffournisseursediteurs.h"
#include "f_preferences.h"

//###################################################################
/**
 * @brief Constructeur de la classe f_ajoutsuppmodifjeux
 *
 * @param parent
 */
F_AjoutSuppModifJeux::F_AjoutSuppModifJeux(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_AjoutSuppModifJeux)
{
    ///////////////////////////////////////
    //////// Constructeur ////////////////
    /////////////////////////////////////
    ui->setupUi(this);

    ui->TabW_Jeu->setCurrentIndex(2);
    ui->TabW_Jeu->setCurrentIndex(0);
    Requete=0;
    // Création des fenêtres de création de nouveaux éléments
    this->pEtatAjMod = new F_PopUpCLESTTEM(this);
    this->pStatutAjMod = new F_PopUpCLESTTEM(this);
    this->pEmplacementAjMod = new F_PopUpCLESTTEM(this);
    this->pClassificationAjMod = new F_PopUpCLESTTEM(this);
    this->pMotCleAjMod = new F_PopUpCLESTTEM(this);
    this->pAuteurAjMod = new F_PopUpCLESTTEM(this);
    this->pAjoutFournisseur=0;
    this->pAjoutEditeur=0;
    //Création de l'objet QLabel pour l'affichage des images
    lb_image = new Lb_Image(this,ui->Lb_ImageName);
    this->pImage = new D_Image(this,lb_image);

    //Création d'un modèle pour le TableView des jeux
    this->ModelJeu = new QStandardItemModel() ;
    //Associe le modèle au TableView
    ui->TbV_Recherche->setModel(this->ModelJeu);
    // Mise en lecture seule
    ui->TbV_Recherche->setEditTriggers(0);
    // Initialise la table view avec tous les jeux
    //AfficherJeux();
    //Supprime le numéro des lignes
    ui->TbV_Recherche->verticalHeader()->setVisible(false);

    ui->LE_CodeClassification->setReadOnly(true);
    //Affiche la date du jour
    QDate DateDefaut ;
    DateDefaut = DateDefaut.currentDate() ;
    ui->DtE_Achat->setDate(DateDefaut) ;

    lb_image->setAlignment(Qt::AlignCenter);
    ui->gridLayout_11->addWidget(lb_image,0,1);

    //////////////////////////////////////////////
    //////////// Remplir les COMBO BOX //////////
    ////////////////////////////////////////////
    
    ////////////////////////////////////
    //////// Remplir EtatJeu ///////////
    ////////////////////////////////////
    connect(this->pEtatAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderEtat()));
    this->ActualiserCBX_Etat();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pEtatAjMod->hide();

    /////////////////////////////////////////
    /////////Remplir StatutJeu//////////////
    ///////////////////////////////////////
    connect(this->pStatutAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderStatut()));
    this->ActualiserCBx_Statut();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pStatutAjMod->hide();

    ////////////////////////////////////////
    ////////// Fournisseurs ///////////////
    //////////////////////////////////////
    this->ActualiserCBx_Fournisseur() ;
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé

    /////////////////////////////////////////
    /////////// Editeurs ///////////////////
    ///////////////////////////////////////
    this->ActualiserCBx_Editeur() ;

    ////////////////////////////////////////////
    ///////// Classification //////////////////
    //////////////////////////////////////////
    connect(this->pClassificationAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderClassification()));
    this->ActualiserCBx_Classification();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pClassificationAjMod->hide();

    ////////////////////////////////////////////
    ///////// Emplacement /////////////////////
    //////////////////////////////////////////
    connect(this->pEmplacementAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderEmplacement()));
    this->ActualiserCBx_Emplacement();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pEmplacementAjMod->hide();
    
    ////////////////////////////////////////////
    /////////// Mot Clés //////////////////////
    ////////////////////////////////////////
    connect(this->pMotCleAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderMotCle()));
    this->ActualiserCBx_MotCle();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pMotCleAjMod->hide();

    ////////////////////////////////////////////
    /////////// Auteurs //////////////////////
    ////////////////////////////////////////
    connect(this->pAuteurAjMod, SIGNAL(SignalValider()), this, SLOT(slot_ValiderAuteur()));
    this->ActualiserCBx_Auteur();
    //Cache la fenêtre lors du démarrage : après que le CBx ait été actualisé
    this->pAuteurAjMod->hide();

    //Grise les boutons valider/annuler/supprimer
    ActiveBoutons(false);
    ui->Bt_Supprimer->setEnabled(false);

    // Connecte l'évenement textEdited à la fonction toUpper
    //connect(SearchBox, SIGNAL(textEdited(const QString &)), SLOT(toUpper(const QString &)));

    AfficherJeux() ;

    ui->TbW_LiensJeux->setColumnCount(3);
    ui->TbW_LiensJeux->setHorizontalHeaderItem(0,new QTableWidgetItem("Nom du jeu"));
    ui->TbW_LiensJeux->setHorizontalHeaderItem(1,new QTableWidgetItem("Descriptif du jeu"));
    ui->TbW_LiensJeux->setHorizontalHeaderItem(2,new QTableWidgetItem("Type de lien"));
    ////////////////////////////////////////////////////////////////////
    ///// Bloque tout et connecte les événements à cacherboutons ///////
    ////////////////////////////////////////////////////////////////////
    QList<QWidget*> QWidgetlist = this->findChildren<QWidget *>();
    QString className;
    QStringList classList;
    classList<<"QTextEdit"<<"QLineEdit"<<"QComboBox"<<"QSpinBox"<<"QRadioButton"<<"QDoubleSpinBox"
            <<"QDateEdit";
    // Parcours la liste de widget ci-dessus et
    // connecte les évenements associés à la fonction cacherboutons
    foreach(QWidget *w, QWidgetlist)
    {
        className = w->metaObject()->className();
        bool classFound = false;
        foreach(QString classN, classList)
        {
            if( classN == className )
            {
                classFound = true;
                break;
            }
        }
        if(!classFound||w->objectName() == "qt_spinbox_lineedit"||w->objectName() == "LE_RechercheNom")
        {
            continue;
        }
        w->setEnabled(false);
        if(className == "QComboBox" )
        {
            connect(w, SIGNAL(activated(int)), this, SLOT(CacherBoutons()));
        }
        else if(className == "QRadioButton")
        {
            connect(w, SIGNAL(clicked(bool)), this, SLOT(CacherBoutons()));
        }
        else if(className == "QTextEdit")
        {
            connect(w, SIGNAL(textChanged()), this, SLOT(CacherBoutons()));
        }
        else if(className == "QDateEdit")
        {
            connect(w, SIGNAL(dateChanged(QDate)), this, SLOT(CacherBoutons()));
        }
        else if(className == "QSpinBox" or className == "QDoubleSpinBox")
        {
            connect(w, SIGNAL(valueChanged(QString)), this, SLOT(CacherBoutons()));
        }
        else
        {
            connect(w, SIGNAL(textEdited(QString)), this, SLOT(CacherBoutons()));
        }
    }
    connect( this->lb_image, SIGNAL( SignalChargementFini() ), this, SLOT( slot_ActiverClicImage() ) ) ;
    // Faire défiler le tableau des jeux avec les flèches du clavier
    connect(ui->TbV_Recherche->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_Recherche_clicked(QModelIndex)));
    F_MainWindow *main=dynamic_cast <F_MainWindow *>(parent);
    ui->W_Contenu->Initialisation(MODE_CONTENU,main,"Hors départ ou retour du jeu");
    connect(ui->W_Historique,SIGNAL(Signal_ActualiserContenu()),ui->W_Contenu,SLOT(ActualiserContenu()));
    connect(ui->W_Contenu,SIGNAL(Signal_ActualiserHistoriqueMaintenance()),ui->W_Historique,SLOT(ActualiserHistoriqueMaintenance()));

    SearchJeu = new SearchBox(this,MODE_HTTP_API);
    SearchJeu->show();
    ui->horizontalLayout->addWidget(SearchJeu);

    connect(SearchJeu,SIGNAL(SignalSuggestionFini(QString)),this,SLOT(slot_HTTP_API_JeuChoisi(QString)));
    connect(SearchJeu,SIGNAL(currentTextChanged(QString)),this,SLOT(CacherBoutons()));
    SearchJeu->setEnabled(false);
}
//###################################################################
/**
 * @brief Destructeur de la classe f_ajoutsuppmodifjeux
 *
 */
F_AjoutSuppModifJeux::~F_AjoutSuppModifJeux()
{
    if(Requete!=0) delete Requete;
    delete ui;
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderClassification()
{
    this->ActualiserCBx_Classification();
    ui->CBx_Classification->setCurrentIndex(ui->CBx_Classification->count() - 2);
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderEmplacement()
{
    this->ActualiserCBx_Emplacement();
    ui->CBx_Emplacement->setCurrentIndex(ui->CBx_Emplacement->count() - 2);
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderEtat()
{
    this->ActualiserCBX_Etat();
    ui->CBx_Etat->setCurrentIndex(ui->CBx_Etat->count() - 2);
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderMotCle()
{
    this->ActualiserCBx_MotCle();
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderAuteur()
{
    this->ActualiserCBx_Auteur();
}
//###################################################################
void F_AjoutSuppModifJeux::slot_ValiderStatut()
{
    this->ActualiserCBx_Statut();
    ui->CBx_Statut->setCurrentIndex(ui->CBx_Statut->count() - 2);
}

//###################################################################
/**
 * @brief Méthode qui met a à jour le tableau en fonction de ce que l'on rentre dans la case de recherche par nom
 *
 * @param arg1
 */
void F_AjoutSuppModifJeux::on_LE_RechercheNom_textChanged(const QString &arg1)
{
    qDebug() << "F_Jeux::on_LE_RechercheNom_textChanged" << "Texte : " << arg1;
    if(ui->Bt_Valider2->isEnabled() && ui->Bt_Annuler2->isEnabled())
    {
        QMessageBox::information(this, "Pré-requis pour passer à un autre jeu !",
                 "Attention, avant d'effectuer une nouvelle recherche vous devez valider ou annuler vos modifications sur le jeu en cours !", "OK") ;
        return;
    }
    unsigned int NumeroLigne (0);
    const int CodeColonne=0;
    const int CodeNomJeu=1;
    int ColonneRecherche;
    QString NomJeu=removeAccents(arg1);
    QString TexteCellule;
    bool numeric;
    NomJeu.toInt(&numeric);
    int firstrow=-1;
    // Déconnexion du défilement du tableau des jeux avec les flêches du clavier
    disconnect(ui->TbV_Recherche->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_Recherche_clicked(QModelIndex)));
    //Tant qu'il y a des jeux dans la table jeux,
    for(int i=0;i<this->ModelJeu->rowCount();i++)
    {
        // Si le texte saisie est un nombre
        if( numeric )
        {
            ColonneRecherche=CodeColonne;
        }
        else
        {
            ColonneRecherche=CodeNomJeu;
        }

        TexteCellule = removeAccents(this->ModelJeu->item(i,ColonneRecherche)->text());
        if(TexteCellule.indexOf(
                    NomJeu,0,Qt::CaseInsensitive ) != string::npos )
        {
            ui->TbV_Recherche->showRow(i);
            if(firstrow==-1)
            {
                firstrow=i;
                ui->TbV_Recherche->selectRow(firstrow);
                qDebug() << "F_Jeux::on_Le_recherchenom_textChanged" << "Selecting " << firstrow;
            }
        }
        else
        {
            ui->TbV_Recherche->hideRow(i);
        }
    }
    // Reconnexion du défilement du tableau des jeux avec les flèches du clavier
    connect(ui->TbV_Recherche->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(on_TbV_Recherche_clicked(QModelIndex)));
    AfficherJeu(this->ModelJeu->index(firstrow, 0).data().toString());
}

void F_AjoutSuppModifJeux::AfficherJeux()
{

    ///////////////////////////////////////////////////
    ///////////////////// Recherche par nom //////////
    /////////////////////////////////////////////////
    unsigned int NumeroLigne (0);

    QSqlQuery RequeteRechercheJeu;
    NumeroLigne=0;

    RequeteRechercheJeu.prepare("SELECT CodeJeu,NomJeu,TypeJeux_Classification FROM jeux");
    RequeteRechercheJeu.exec();

    //On vide le modèle
    this->ModelJeu->clear();
    //Indique le nombres de colones puis leurs noms
    this->ModelJeu->setColumnCount(3);
    this->ModelJeu->setHorizontalHeaderItem(0, new QStandardItem("Code"));
    this->ModelJeu->setHorizontalHeaderItem(1, new QStandardItem("Nom"));
    this->ModelJeu->setHorizontalHeaderItem(2, new QStandardItem("Classification"));
    //impose une taille aux colonnes
    ui->TbV_Recherche->setColumnWidth(0, 45);
    ui->TbV_Recherche->setColumnWidth(1,300);
    ui->TbV_Recherche->setColumnWidth(2, 50);

    int ElementSelectionne=0;
    QString CodeJeu;
    //Tant qu'il y a des jeux dans la table jeux,
    while(RequeteRechercheJeu.next())
    {
        CodeJeu=ObtenirValeurParNom(RequeteRechercheJeu,"CodeJeu").toString();
        //on ajoute une nouvelle ligne du table view
        this->ModelJeu->setItem(NumeroLigne, 0, new QStandardItem(CodeJeu));
        this->ModelJeu->setItem(NumeroLigne, 1, new QStandardItem(ObtenirValeurParNom(RequeteRechercheJeu,"NomJeu").toString() ));
        this->ModelJeu->setItem(NumeroLigne, 2, new QStandardItem(ObtenirValeurParNom(RequeteRechercheJeu,"TypeJeux_Classification").toString() ));
        if(CodeJeu==sCodeJeuSelectionne)
        {
            ElementSelectionne=NumeroLigne;
        }
        NumeroLigne++;
    }
    ui->TbV_Recherche->selectRow(ElementSelectionne);
    ui->TbV_Recherche->scrollTo(ui->TbV_Recherche->selectionModel()->selectedIndexes()[0]);
}
//###################################################################
/**
 * @brief Méthode qui récupère le code du jeu sur lequel on a sélectionné dans le tableau
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_TbV_Recherche_selectionChanged(const QModelIndex &index)
{
  /*  AfficherJeu();
    // Griser Valider et Annuler/ Autoriser Ajout et suppression
    ui->Bt_Valider2->setEnabled(false);
    ui->Bt_Annuler2->setEnabled(false);
    ui->Bt_Supprimer->setEnabled(true);
    ui->Bt_Ajouter->setEnabled(true);*/
}

//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un emplacement
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Emplacement_activated(int index)
{
    //Si l'élément sélectionné est le dernier : ajouter un emplacement
    if(index == ui->CBx_Emplacement->count() -1)
    {
        this->pEmplacementAjMod->Ajouter(2);
        //this->ActualiserCBx_Emplacement();
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un fournisseur
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Fournisseur_activated(int index)
{
    //Si l'élément séléctionné est le dernier : afficher la fenêtre pour ajouter un fournisseur
    if(index == ui->CBx_Fournisseur->count() -1)
    {
        if(!this->pAjoutFournisseur)
        {
            this->pAjoutFournisseur= new D_AjoutSuppModifFournisseursEditeurs(this,MODE_FOURNISSEUR);
        }
        this->pAjoutFournisseur->show();
    }
}

//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un éditeur
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Editeur_activated(int index)
{
    //Si l'élément séléctionné est le dernier : afficher la fenêtre pour ajouter un éditeur
    if(index == ui->CBx_Editeur->count() -1)
    {
        if(!this->pAjoutEditeur)
        {
            this->pAjoutEditeur= new D_AjoutSuppModifFournisseursEditeurs(this,MODE_EDITEUR);
        }
        this->pAjoutEditeur->show();
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un statut
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Statut_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un statut
    if(index == ui->CBx_Statut->count() -1)
    {
        this->pStatutAjMod->Ajouter(1);
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'une classification
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Classification_activated(int index)
{
   // Récupère le code de classification et l'affiche dans le champs LE_CodeClassification
   if(index != 0)
   {
      //Récupère le code placé devant les : dans le CBx
      QString CodeClass = ui->CBx_Classification->currentText().left(ui->CBx_Classification->currentText().indexOf(":")) ;
      ui->LE_CodeClassification->setText(CodeClass);
   }
   else
   {
      ui->LE_CodeClassification->clear();
   }

   //Si l'élément sélectionné est le dernier, on veut ajouter un code de classification
   if(index == ui->CBx_Classification->count() -1)
   {
      this->pClassificationAjMod->Ajouter(3);
   }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un état
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_Etat_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un emplacement
    if(index == ui->CBx_Etat->count() -1)
    {
        this->pEtatAjMod->Ajouter(0);
    }
}
//###################################################################
/**
 * @brief Méthode qui lance la fenêtre d'ajout d'un mot clé (1)
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_CBx_MotCle_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un mot clé
    if(index == ui->CBx_MotCle->count() -1)
    {
        this->pMotCleAjMod->Ajouter(POPUP_MOT_CLE);
    }
}

//###################################################################
/**
 * @brief Méthode qui ajoute ou modifie un jeux selon le mode dans lequel on se trouve
 *
 */
void F_AjoutSuppModifJeux::on_Bt_Valider2_clicked()
{
   // Si pas de nom de jeu ou pas de code de jeu
   if (SearchJeu->currentText() == "" || ui->LE_Code->text() == "" )
   {
       QMessageBox::warning(this, "Pré-requis pour l'ajout",
                    "Vous devez saisir au minimum le nom du jeu et son code avant de valider !", "OK") ;
       return ;
   }

   // Si l'âge mini est > à l'âge maxi
   if ( ui->SBx_AgeMin->value() > ui->SBx_AgeMax->value() )
   {
       QMessageBox::warning(this, "Erreur de saisie !", "L'âge minimum ne peut pas être plus grand que l'âge maxi !", "OK") ;
       return ;
   }

   // Si le nombre de joueur mini est > au nombre de joueur maxi
   if ( ui->SBx_JoueursMin->value() > ui->SBx_JoueursMax ->value() )
   {
       QMessageBox::warning(this, "Erreur de saisie !", "Le nombre de joueur minimu ne pas être plus grand que le nombre de joueur maximun !", "OK") ;
       return ;
   }
   QSqlQuery RequeteVerificationCode;
   RequeteVerificationCode.prepare("SELECT IdJeux,CodeJeu FROM jeux WHERE CodeJeu=:CodeJeu");
   RequeteVerificationCode.bindValue(":CodeJeu",this->ui->LE_Code->text());
   if(!RequeteVerificationCode.exec())
   {
      qDebug()<<getLastExecutedQuery(RequeteVerificationCode)<<RequeteVerificationCode.lastError();
   }
   if(RequeteVerificationCode.next())
   {
       // Si c'est un nouveau jeu ou si c'est un jeu existant avec un code différent, on affiche une erreur et on sort
       if((!AjoutOuModif&&sCodeJeuSelectionne!=this->ui->LE_Code->text())||AjoutOuModif)
       {
           QMessageBox::critical(this, "Code Jeu déjà utilisé", "Le code jeu indiqué est déjà utilisé par un autre jeu. Merci de choisir un autre code.", "OK") ;
           return ;
       }
   }
   int IdJeux=ObtenirValeurParNom(RequeteVerificationCode,"IdJeux").toInt();


    // CREATION D'UN NOUVEAU JEU : vaut true en cas de création sinon false en cas de mise à jour
    if(AjoutOuModif == true)
    {
        // Création du nouveau jeu
        QSqlQuery RequeteAjouterJeu ;

        if((QMessageBox::information(this, "Confirmation de l'ajout","Voulez-vous vraiment créer ce nouveau jeu "+
                                     SearchJeu->currentText()+" ?","Oui", "Non")) == 0 )
        {
           //////////////////////////////////////////////////////////////////////////////////////////
           // Requete pour faire le lien avec typejeux et récupérer le'id de la classication
           QSqlQuery RequeteClassification;
           RequeteClassification.prepare("SELECT Classification FROM typejeux WHERE Classification=:CodeClassification");
           RequeteClassification.bindValue(":CodeClassification",ui->LE_CodeClassification->text());
           if(!RequeteClassification.exec())
           {
              qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteClassification "<<RequeteClassification.lastQuery() ;
           }
           RequeteClassification.next();
           /////////////////////////////////////////////////////////////////////////////////////////
           // Requete pour faire le lien avec statutjeux et récupérer le'id du statut
           QSqlQuery RequeteStatut ;
           RequeteStatut.prepare("SELECT IdStatutJeux FROM statutjeux WHERE StatutJeu=:StatutDuJeu") ;
           RequeteStatut.bindValue(":StatutDuJeu", ui->CBx_Statut->currentText());
           if(!RequeteStatut.exec())
           {
              qDebug() << "F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked() : RequeteStatut" << RequeteStatut.lastQuery() ;
           }
           RequeteStatut.next() ;
           //////////////////////////////////////////////////////////////////////////////////////////
           // Requete pour faire le lien avec etatjeux et récupérer l'id de l'état
           QSqlQuery RequeteEtat ;
           RequeteEtat.prepare("SELECT IdEtatsJeu FROM etatsjeu WHERE Etat=:EtatsDuJeu") ;
           RequeteEtat.bindValue(":EtatsDuJeu", ui->CBx_Etat->currentText());
           if(!RequeteEtat.exec())
           {
              qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteEtat " << RequeteEtat.lastQuery() ;
           }
           RequeteEtat.next() ;
           ///////////////////////////////////////////////////////////////////////////////////////////
           // Requete pour faire le lien avec emplacement et récupérer l'id de l'emplacement
           QSqlQuery RequeteEmplacement ;
           RequeteEmplacement.prepare("SELECT IdEmplacement FROM emplacement WHERE Nom=:DeEmplacement") ;
           RequeteEmplacement.bindValue(":DeEmplacement", ui->CBx_Emplacement->currentText());
           if(!RequeteEmplacement.exec())
           {
              qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteEmplacement " << RequeteEmplacement.lastQuery() ;
           }
           RequeteEmplacement.next() ;
           ///////////////////////////////////////////////////////////////////////////////////////////
           // Requete pour faire le lien avec fournisseurs/editeur et récupérer l'id du fournisseur
           QSqlQuery RequeteFournisseur ;
           RequeteFournisseur.prepare("SELECT IdFournisseur FROM fournisseursediteur "
                                      "WHERE NomFournisseur=:DuFournisseur AND Fournisseur=1") ;
           RequeteFournisseur.bindValue(":DuFournisseur", ui->CBx_Fournisseur->currentText());
           if(!RequeteFournisseur.exec())
           {
              qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteFournisseur " << RequeteFournisseur.lastQuery() ;

           }
           RequeteFournisseur.next() ;
           //////////////////////////////////////////////////////////////////////////////////////////
           // Requete pour faire le lien avec fournisseursediteur et récupérer le'id de l'éditeur
           QSqlQuery RequeteEditeur ;
           RequeteEditeur.prepare("SELECT IdFournisseur FROM fournisseursediteur "
                                  "WHERE NomFournisseur=:LeEditeur AND Editeur=1") ;
           RequeteEditeur.bindValue(":LeEditeur", ui->CBx_Editeur->currentText());
           qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked() :LeEditeur =" << ui->CBx_Editeur->currentText();
           if(!RequeteEditeur.exec())
           {
              qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()RequeteEditeur " << RequeteEditeur.lastQuery() ;

           }
           RequeteEditeur.next() ;

           RequeteAjouterJeu.prepare("INSERT INTO jeux(NomJeu,NomCreateurJeu,DateAchat,PrixAchat,PrixLoc,NbrJoueurMin,"
                                     "NbrJoueurMax,AgeMin,AgeMax,Remarque,"
                                     "DescriptionJeu,CodeJeu,EtatInitial,"
                                     "TypeJeux_Classification,StatutJeux_IdStatutJeux,EtatsJeu_IdEtatsJeu,"
                                     "Emplacement_IdEmplacement,Fournisseurs_IdFournisseur,Editeur_IdEditeur,SiteWeb1,"
                                     "DureeMin,DureeMax,Note) "
                                     "VALUES (:NomDuJeu,:NomDuCreateurJeu,:DateDeAchat,:PrixDeAchat,:PrixDeLoc,"
                                     ":NbrDeJoueurMin,:NbrDeJoueurMax,:LeAgeMin,:LeAgeMax,:LaRemarque,"
                                     ":DescriptionDuJeu,:CodeDuJeu,"
                                     ":LeEtatInitial,:LeTypeJeux_Classification,"
                                     ":StatutDuJeux_IdStatutJeux,:EtatsDuJeu_IdEtatsJeu,:Emplacement_IdDeEmplacement,"
                                     ":LeFournisseurs_IdFournisseur,:LeEditeur_IdEditeur,:LeSiteWeb1,:DureeMin,"
                                     ":DureeMax,:Note)" ) ;
           RequeteAjouterJeu.bindValue(":NomDuJeu", this->SearchJeu->currentText());
           RequeteAjouterJeu.bindValue(":DateDeAchat", this->ui->DtE_Achat->date());
           RequeteAjouterJeu.bindValue(":PrixDeAchat", this->ui->SBx_PrixAchat->value() );
           RequeteAjouterJeu.bindValue(":PrixDeLoc", this->ui->SBx_PrixLocation->value());
           RequeteAjouterJeu.bindValue(":NbrDeJoueurMin", this->ui->SBx_JoueursMin->value());
           RequeteAjouterJeu.bindValue(":NbrDeJoueurMax", this->ui->SBx_JoueursMax->value());
           RequeteAjouterJeu.bindValue(":LeAgeMin", this->ui->SBx_AgeMin->value());
           RequeteAjouterJeu.bindValue(":LeAgeMax", this->ui->SBx_AgeMax->value());
           RequeteAjouterJeu.bindValue(":Remarque", this->ui->TxE_Remarque->toPlainText());
           RequeteAjouterJeu.bindValue(":DescriptionDuJeu", this->ui->TxE_Description->toPlainText());
           RequeteAjouterJeu.bindValue(":CodeDuJeu", this->ui->LE_Code->text());
           RequeteAjouterJeu.bindValue(":LeTypeJeux_Classification", RequeteClassification.value(0).toString());
           RequeteAjouterJeu.bindValue(":StatutDuJeux_IdStatutJeux", RequeteStatut.value(0).toString() );
           RequeteAjouterJeu.bindValue(":EtatsDuJeu_IdEtatsJeu", RequeteEtat.value(0).toString() );
           RequeteAjouterJeu.bindValue(":Emplacement_IdDeEmplacement", RequeteEmplacement.value(0));
           RequeteAjouterJeu.bindValue(":LeFournisseurs_IdFournisseur", RequeteFournisseur.value(0).toString());
           RequeteAjouterJeu.bindValue(":LeEditeur_IdEditeur", RequeteEditeur.value(0).toString() );
           RequeteAjouterJeu.bindValue(":LeSiteWeb1", this->ui->LE_SiteWeb1->text() );
           RequeteAjouterJeu.bindValue(":DureeMin", this->ui->SBx_DureeMin->value() );
           RequeteAjouterJeu.bindValue(":DureeMax", this->ui->SBx_DureeMax->value() );
           RequeteAjouterJeu.bindValue(":Note", this->ui->SBx_Note->value() );

           if(ui->RBt_Neuf->isChecked())
           {
              RequeteAjouterJeu.bindValue(":LeEtatInitial", "Neuf");
           }
           else
           {
              RequeteAjouterJeu.bindValue(":LeEtatInitial", "Occasion");
           }

           if(!RequeteAjouterJeu.exec())
           {
              qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked()-"<<RequeteAjouterJeu.lastQuery()
                       <<"Erreur : "<<RequeteAjouterJeu.lastError();
           }
           QSqlQuery Requete;
           for(int i=0;i<ui->Lw_MotsCles->count();i++)
           {
               //prépare le requête d'ajout des mots-clés
               Requete.prepare("INSERT INTO motsclesassoc(Jeux_IdJeux,Motscles_IdMotCle) VALUES (:Jeux_IdJeux,:Motscles_IdMotCle)");
               //Entre les valeurs de la requête
               Requete.bindValue(":Jeux_IdJeux", RequeteAjouterJeu.lastInsertId());
               Requete.bindValue(":Motscles_IdMotCle", ui->Lw_MotsCles->item(i)->data(Qt::UserRole));
               //Exécute la requête
               if (!Requete.exec())
               {
                   qDebug() << getLastExecutedQuery(Requete) << Requete.lastError();
               }
           }

           //prépare le requête d'ajout des auteurs
           for(int i=0;i<ui->Lw_Auteurs->count();i++)
           {
               //prépare le requête d'ajout des mots-clés
               Requete.prepare("INSERT INTO auteursassoc(Jeux_IdJeux,Auteurs_IdAuteur) VALUES (:Jeux_IdJeux,:Auteurs_IdAuteur)");
               //Entre les valeurs de la requête
               Requete.bindValue(":Jeux_IdJeux", RequeteAjouterJeu.lastInsertId());
               Requete.bindValue(":Auteurs_IdAuteur", ui->Lw_Auteurs->item(i)->data(Qt::UserRole));
               //Exécute la requête
               if (!Requete.exec())
               {
                   qDebug() << getLastExecutedQuery(Requete);
               }
           }

        }
        // Vide tous les champs après l'ajout
        VideChamps();

        // Vider la saisie de la recherche du nom
        ui->LE_RechercheNom->setText("");

        QDate DateDefaut ;
        DateDefaut = DateDefaut.currentDate() ;
        ui->DtE_Achat->setDate(DateDefaut);

        // Autoriser la création d'un nouveau jeux
        ui->Bt_Ajouter->setEnabled(true);

        ActiveChamps(false);
    }
    else  // MISE A JOUR D'UN JEU EXISTANT
    {
         ////////////// MAJ du Nbr de joueurs /////////------------------------------------------------------------
         QSqlQuery RequeteModifNbrJoueurs ;
         //prépare le requête de mise à jour
         RequeteModifNbrJoueurs.prepare("UPDATE jeux SET CodeJeu=:NouveauCodeDuJeu,NbrJoueurMin=:NbrDeJoueurMin,NbrJoueurMax=:NbrDeJoueurMax "
                                        "WHERE CodeJeu=:CodeDuJeu");
         //Entre les valeurs de la requête
         RequeteModifNbrJoueurs.bindValue(":CodeDuJeu",this->sCodeJeuSelectionne);
         RequeteModifNbrJoueurs.bindValue(":NouveauCodeDuJeu",ui->LE_Code->text());
         RequeteModifNbrJoueurs.bindValue(":NbrDeJoueurMin", ui->SBx_JoueursMin->value());
         RequeteModifNbrJoueurs.bindValue(":NbrDeJoueurMax", ui->SBx_JoueursMax->value());
         //Exécutee la requête
         if (!RequeteModifNbrJoueurs.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajNbrJoueur" << RequeteModifNbrJoueurs.lastQuery() ;
         }
         //////////////// MAJ de l'âge //////////////////-------------------------------------------------------------------
         QSqlQuery RequeteModifAge ;
         //prépare le requête de mise à jour
         RequeteModifAge.prepare("UPDATE jeux SET AgeMin=:LAgeMin,AgeMax=:LAgeMax WHERE CodeJeu=:CodeDuJeu");
         //Entre les valeurs de la requête
         RequeteModifAge.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifAge.bindValue(":LAgeMin", ui->SBx_AgeMin->value());
         RequeteModifAge.bindValue(":LAgeMax", ui->SBx_AgeMax->value());
         //Exécute la requête
         if (!RequeteModifAge.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajAge" << RequeteModifAge.lastQuery() ;
         }
         //////////////// MAJ des prix ////////////////// ---------------------------------------------------------------
         QSqlQuery RequeteModifPrix ;
         //prépare le requête de mise à jour
         RequeteModifPrix.prepare("UPDATE jeux SET PrixAchat=:LePrixAchat,PrixLoc=:LePrixLoc "
                                  "WHERE CodeJeu=:CodeDuJeu");
         //Entre les valeurs de la requête
         RequeteModifPrix.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifPrix.bindValue(":LePrixAchat", ui->SBx_PrixAchat->value());
         RequeteModifPrix.bindValue(":LePrixLoc", ui->SBx_PrixLocation->value());
         //Exécute la requête
         if (!RequeteModifPrix.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajPrix" << RequeteModifPrix.lastQuery() ;
         }
         ////////////// MAJ de la date achat //////////----------------------------------------------------------
         QSqlQuery RequeteModifDateAchat ;
         //prépare le requête de mise à jour
         RequeteModifDateAchat.prepare("UPDATE jeux SET DateAchat=:DateDeAchat WHERE CodeJeu=:CodeDuJeu");
         //Entre les valeurs de la requête
         RequeteModifDateAchat.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifDateAchat.bindValue(":DateDeAchat", ui->DtE_Achat->date());
         //Exécute la requête
         if (!RequeteModifDateAchat.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajDateAchat" << RequeteModifDateAchat.lastQuery() ;
         }
         ///////// MAJ des Textes Edit (description,remarque du jeu ///////----------------------------
         QSqlQuery RequeteModifTxE ;
         //prépare le requête de mise à jour
         RequeteModifTxE.prepare("UPDATE jeux SET DescriptionJeu=:DescriptionDuJeu,Remarque=:Remarque,SiteWeb1=:LeSiteWeb1, "
                                 "DureeMin=:DureeMin, DureeMax=:DureeMax, Note=:Note WHERE CodeJeu=:CodeDuJeu");
         //Entre les valeurs de la requête
         RequeteModifTxE.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifTxE.bindValue(":DescriptionDuJeu", ui->TxE_Description->toPlainText());
         RequeteModifTxE.bindValue(":Remarque", ui->TxE_Remarque->toPlainText());
         RequeteModifTxE.bindValue(":LeSiteWeb1", ui->LE_SiteWeb1->text());
         RequeteModifTxE.bindValue(":DureeMin", ui->SBx_DureeMin->value());
         RequeteModifTxE.bindValue(":DureeMax", ui->SBx_DureeMax->value());
         RequeteModifTxE.bindValue(":Note", ui->SBx_Note->value());
         //Exécute la requête
         if (!RequeteModifTxE.exec())
         {
             qDebug() << getLastExecutedQuery(RequeteModifTxE) << RequeteModifTxE.lastError() ;
         }
         ///////////////// MAJ EtatInitial  //////////////------------------------------------------------------------
         QSqlQuery RequeteModifEtatInitial ;
         //prépare le requête de mise à jour
         RequeteModifEtatInitial.prepare("UPDATE jeux SET EtatInitial=:LeEtatInitial WHERE CodeJeu=:CodeDuJeu");
         RequeteModifEtatInitial.bindValue(":CodeDuJeu",ui->LE_Code->text());
         //Si le RadioBouton Neuf est coché
         if(ui->RBt_Neuf->isChecked())
         {
             RequeteModifEtatInitial.bindValue(":LeEtatInitial", "Neuf");
         }
         else
         {
             RequeteModifEtatInitial.bindValue(":LeEtatInitial", "Occasion");
         }
         //Exécutee la requête
         if (!RequeteModifEtatInitial.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajEtatInitial" << RequeteModifEtatInitial.lastQuery() ;
         }
         ////////// MAJ du nom //////////---------------------------------------------------------
         QSqlQuery RequeteModifNom ;
         //prépare le requête de mise à jour
         RequeteModifNom.prepare("UPDATE jeux SET NomJeu=:NomDuJeu WHERE CodeJeu=:CodeDuJeu");
         //Entre les valeurs de la requête
         RequeteModifNom.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifNom.bindValue(":NomDuJeu", SearchJeu->currentText());
         //Exécute la requête
         if (!RequeteModifNom.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajNom" << RequeteModifNom.lastQuery() ;
         }
         ////////// MAJ classification ////////////-------------------------------------------------------------------------
         QSqlQuery RequeteModifClassification ;
         //prépare le requête de mise à jour
         RequeteModifClassification.prepare("UPDATE jeux SET TypeJeux_Classification=:CodeClassification "
                                            "WHERE CodeJeu=:CodeDuJeu");
         //Entre les valeurs de la requête
         RequeteModifClassification.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifClassification.bindValue(":CodeClassification", ui->LE_CodeClassification->text());
         //Exécute la requête
         if (!RequeteModifClassification.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajClassification" << RequeteModifClassification.lastQuery() ;
         }
         ////////// MAJ Etat //////////////----------------------------------------------------------------
         QSqlQuery RequeteModifEtat ;
         //prépare le requête de mise à jour
         RequeteModifEtat.prepare("UPDATE jeux,etatsjeu SET EtatsJeu_IdEtatsJeu=IdEtatsJeu "
                                  "WHERE CodeJeu=:CodeDuJeu AND Etat=:EtatDuJeu");
         //Entre les valeurs de la requête
         RequeteModifEtat.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifEtat.bindValue(":EtatDuJeu", ui->CBx_Etat->currentText());
         //Exécute la requête
         if (!RequeteModifEtat.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajEtat" << RequeteModifEtat.lastQuery() ;
         }

         ////////// MAJ Statut ////////-----------------------------------------------------------------------
         QSqlQuery RequeteModifStatut ;
         //prépare le requête de mise à jour
         RequeteModifStatut.prepare("UPDATE jeux,statutjeux SET StatutJeux_IdStatutJeux=IdStatutJeux "
                                    "WHERE CodeJeu=:CodeDuJeu AND StatutJeu=:StatutDuJeu");
         //Entre les valeurs de la requête
         RequeteModifStatut.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifStatut.bindValue(":StatutDuJeu", ui->CBx_Statut->currentText());
         //Exécute la requête
         if (!RequeteModifStatut.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajStatut" << RequeteModifStatut.lastQuery() ;
         }
         ////////// MAJ Editeur //////---------------------------------------------------------------------
         QSqlQuery RequeteModifEditeur ;
         //prépare le requête de mise à jour
         RequeteModifEditeur.prepare("UPDATE jeux,fournisseursediteur SET Editeur_IdEditeur=IdFournisseur "
                                     "WHERE CodeJeu=:CodeDuJeu AND NomFournisseur=:NomDuEditeur");
         //Entre les valeurs de la requête
         RequeteModifEditeur.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifEditeur.bindValue(":NomDuEditeur", ui->CBx_Editeur->currentText());
         //Exécute la requête
         if (!RequeteModifEditeur.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajEditeur" << RequeteModifEditeur.lastQuery() ;
         }
         ////////// MAJ Emplacement ///////////----------------------------------------------------------------
         QSqlQuery RequeteModifEmplacement ;
         //prépare le requête de mise à jour
         RequeteModifEmplacement.prepare("UPDATE jeux,emplacement SET Emplacement_IdEmplacement=IdEmplacement "
                                         "WHERE CodeJeu=:CodeDuJeu AND NomEmplacement=:NomEmplacement");
         //Entre les valeurs de la requête
         RequeteModifEmplacement.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifEmplacement.bindValue(":NomEmplacement", ui->CBx_Emplacement->currentText());
         //Exécute la requête
         if (!RequeteModifEmplacement.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajEmplacement" << RequeteModifEmplacement.lastQuery() ;
         }
         ////////// MAJ Fournisseur ///////////----------------------------------------------------------------
         QSqlQuery RequeteModifFournisseur ;
         //prépare le requête de mise à jour
         RequeteModifFournisseur.prepare("UPDATE jeux,fournisseursediteur SET Fournisseurs_IdFournisseur=IdFournisseur "
                                         "WHERE CodeJeu=:CodeDuJeu AND NomFournisseur=:NomDuFournisseur");
         //Entre les valeurs de la requête
         RequeteModifFournisseur.bindValue(":CodeDuJeu",ui->LE_Code->text());
         RequeteModifFournisseur.bindValue(":NomDuFournisseur", ui->CBx_Fournisseur->currentText());
         //Exécute la requête
         if (!RequeteModifFournisseur.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajFournisseur" << RequeteModifFournisseur.lastQuery() ;
         }
         ///////// MAJ Auteur ///////////////--------------------------------------------------------
         QSqlQuery Requete;
         //prépare le requête de suppression des mots clés existants
         Requete.prepare("DELETE FROM auteursassoc WHERE Jeux_IdJeux=:Jeux_IdJeux");
         //Entre les valeurs de la requête
         Requete.bindValue(":Jeux_IdJeux", IdJeux);

         //Exécute la requête
         if (!Requete.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajMotCle1" << getLastExecutedQuery(Requete);
         }

         //prépare le requête d'ajout des auteurs
         for(int i=0;i<ui->Lw_Auteurs->count();i++)
         {
             //prépare le requête d'ajout des auteurs
             Requete.prepare("INSERT INTO auteursassoc(Jeux_IdJeux,Auteurs_IdAuteur) VALUES (:Jeux_IdJeux,:Auteurs_IdAuteur)");
             //Entre les valeurs de la requête
             Requete.bindValue(":Jeux_IdJeux", IdJeux);
             Requete.bindValue(":Auteurs_IdAuteur", ui->Lw_Auteurs->item(i)->data(Qt::UserRole));
             //Exécute la requête
             if (!Requete.exec())
             {
                 qDebug() << getLastExecutedQuery(Requete);
             }
         }

         ///////////////  MAJ MotCle /////////////----------------------------------------------------------------------------------
         //prépare le requête de suppression des mots clés existants
         Requete.prepare("DELETE FROM motclesassoc WHERE Jeux_IdJeux=:Jeux_IdJeux");
         //Entre les valeurs de la requête
         Requete.bindValue(":Jeux_IdJeux", IdJeux);

         //Exécute la requête
         if (!Requete.exec())
         {
             qDebug() << "F_AjoutSuppModifJeux::on_Bt_Valider_clicked() : MajMotCle1" << getLastExecutedQuery(Requete);
         }

         //prépare le requête d'ajout des mots-clés
         for(int i=0;i<ui->Lw_MotsCles->count();i++)
         {
             //prépare le requête d'ajout des mots-clés
             Requete.prepare("INSERT INTO motclesassoc(Jeux_IdJeux,Motscles_IdMotCle) VALUES (:Jeux_IdJeux,:Motscles_IdMotCle)");
             //Entre les valeurs de la requête
             Requete.bindValue(":Jeux_IdJeux", IdJeux);
             Requete.bindValue(":Motscles_IdMotCle", ui->Lw_MotsCles->item(i)->data(Qt::UserRole));
             //Exécute la requête
             if (!Requete.exec())
             {
                 qDebug() << getLastExecutedQuery(Requete);
             }
         }

        //Griser les boutons de modification du contenu
        ActiveBoutons(false);
        ui->Bt_Supprimer->setEnabled(true);
        ui->Bt_Ajouter->setEnabled(true);

        // Vider la saisie de la recherche du nom
        //ui->LE_RechercheNom->setText("");

        // Raffraichir la liste des jeux
        //this->on_LE_RechercheNom_textChanged(ui->LE_RechercheNom->text()) ;

        // Réactualiser les combobox qui ont peu avoir de nouvelles entrées
        this->ActualiserCBx_Classification();
        this->ActualiserCBx_MotCle();
        this->ActualiserCBx_Editeur();
        this->ActualiserCBx_Emplacement();
        this->ActualiserCBX_Etat();
        this->ActualiserCBx_Fournisseur();
        this->ActualiserCBx_Statut();

        // Rafraichir l'affichage du jeu
    }
    // Mettre à jour la liste des jeux
    AfficherJeux();
    //ui->LE_RechercheNom->clear();
    // Tout griser
    // Plus de modif en cours donc :
    // griser les boutons Annuler et Valider
    ActiveBoutons(false);
}

/**
 * @brief Active ou désactive les boutons valider et annuler
 *
 * @param etat : true pour activer et false pour désactiver
 */
void F_AjoutSuppModifJeux::ActiveBoutons(bool etat)
{
    ui->Bt_Valider2->setEnabled(etat);
    ui->Bt_Annuler2->setEnabled(etat);
    ui->TbV_Recherche->setEnabled(~etat);
}

/**
 * @brief Active ou désactive les champs d'un jeu
 *
 * @param etat : true pour activer et false pour désactiver
 */
void F_AjoutSuppModifJeux::ActiveChamps(bool etat)
{
    ui->CBx_Classification->setEnabled(etat);
    ui->CBx_Editeur->setEnabled(etat);
    ui->CBx_Emplacement->setEnabled(etat);
    ui->CBx_Etat->setEnabled(etat);
    ui->CBx_Fournisseur->setEnabled(etat);
    ui->CBx_MotCle->setEnabled(etat);
    ui->Lw_MotsCles->setEnabled(etat);
    ui->CBx_Auteur->setEnabled(etat);
    ui->Lw_Auteurs->setEnabled(etat);
    ui->Bt_AjouterMotCle->setEnabled(etat);
    ui->Bt_AjouterAuteur->setEnabled(etat);
    ui->CBx_Statut->setEnabled(etat);
    ui->LE_Code->setEnabled(etat);
    ui->LE_CodeClassification->setEnabled(etat);
    ui->LE_Id_BGG->setEnabled(etat);
    ui->LE_Version_BGG->setEnabled(etat);
    ui->SBx_Note->setEnabled(etat);
    ui->LE_Slug_TricTrac->setEnabled(etat);
    SearchJeu->setEnabled(etat);
    ui->LE_SiteWeb1->setEnabled(etat);
    ui->SBx_DureeMin->setEnabled(etat);
    ui->SBx_DureeMax->setEnabled(etat);
    ui->SBx_Note->setEnabled(etat);
    ui->DtE_Achat->setEnabled(etat);
    ui->SBx_AgeMax->setEnabled(etat);
    ui->SBx_AgeMin->setEnabled(etat);
    ui->SBx_JoueursMax->setEnabled(etat);
    ui->SBx_JoueursMin->setEnabled(etat);
    ui->SBx_PrixAchat->setEnabled(etat);
    ui->SBx_PrixLocation->setEnabled(etat);
    ui->W_Contenu->setEnabled(etat);
    ui->W_Historique->setEnabled(etat);
    ui->TxE_Description->setEnabled(etat);
    ui->TxE_Remarque->setEnabled(etat);
    ui->RBt_Neuf->setEnabled(etat);
    ui->RBt_Occasion->setEnabled(etat);
}

/**
 * @brief Vide les champs d'un jeu
 *
 */
void F_AjoutSuppModifJeux::VideChamps()
{
    ui->CBx_Classification->setCurrentIndex(0);
    ui->CBx_Editeur->setCurrentIndex(0);
    ui->CBx_Emplacement->setCurrentIndex(0);
    ui->CBx_Etat->setCurrentIndex(0);
    ui->CBx_Fournisseur->setCurrentIndex(0);
    ui->CBx_Statut->setCurrentIndex(0);
    ui->CBx_MotCle->setCurrentIndex(0);
    ui->Lw_MotsCles->clear();
    ui->LE_Code->clear();
    ui->LE_CodeClassification->clear();
    ui->CBx_Auteur->setCurrentIndex(0);
    ui->Lw_Auteurs->clear();
    SearchJeu->clear();
    SearchJeu->setCurrentText("");
    ui->LE_SiteWeb1->setText("http://");
    ui->SBx_DureeMin->setValue(0);
    ui->SBx_DureeMax->setValue(0);
    ui->SBx_Note->setValue(0);
    ui->SBx_AgeMax->setValue(99);
    ui->SBx_AgeMin->setValue(1);
    ui->SBx_JoueursMax->setValue(1);
    ui->SBx_JoueursMin->setValue(1);
    ui->SBx_PrixAchat->setValue(0);
    ui->SBx_PrixLocation->setValue(0);
    ui->W_Contenu->Vider();
    ui->W_Contenu->ActiverBoutonsContenu(false);
    ui->TxE_Description->clear();
    ui->TxE_Remarque->clear();
    ui->W_Historique->Vider();
    ui->RBt_Neuf->setChecked(true);
}

/**
 * @brief Bloque les signaux pour éviter que la modification de ces champs active les boutons de validation et d'annulation
 *
 * @param etat : true pour bloquer et false pour débloquer
 */
void F_AjoutSuppModifJeux::BloquerSignalsChamps(bool etat)
{
    qDebug()<<"F_AjoutSuppModifJeux::BloquerSignalsChamps:"<<etat;
    ui->CBx_Classification->blockSignals(etat);
    ui->CBx_Editeur->blockSignals(etat);
    ui->CBx_Emplacement->blockSignals(etat);
    ui->CBx_Etat->blockSignals(etat);
    ui->CBx_Fournisseur->blockSignals(etat);
    ui->CBx_MotCle->blockSignals(etat);
    ui->Lw_MotsCles->blockSignals(etat);
    ui->CBx_Statut->blockSignals(etat);
    ui->LE_Code->blockSignals(etat);
    ui->LE_CodeClassification->blockSignals(etat);
    ui->CBx_Auteur->blockSignals(etat);
    ui->Lw_Auteurs->blockSignals(etat);
    SearchJeu->blockSignals(etat);
    ui->LE_SiteWeb1->blockSignals(etat);
    ui->SBx_DureeMin->blockSignals(etat);
    ui->SBx_DureeMax->blockSignals(etat);
    ui->SBx_Note->blockSignals(etat);
    ui->DtE_Achat->blockSignals(etat);
    ui->SBx_AgeMax->blockSignals(etat);
    ui->SBx_AgeMin->blockSignals(etat);
    ui->SBx_JoueursMax->blockSignals(etat);
    ui->SBx_JoueursMin->blockSignals(etat);
    ui->SBx_PrixAchat->blockSignals(etat);
    ui->SBx_PrixLocation->blockSignals(etat);
    //ui->W_Contenu->blockSignals(etat);
    ui->TxE_Description->blockSignals(etat);
    ui->TxE_Remarque->blockSignals(etat);
    //ui->W_Historique->blockSignals(etat);
    ui->RBt_Neuf->blockSignals(etat);
    ui->RBt_Occasion->blockSignals(etat);
}
//###################################################################
/**
 * @brief Méthode qui annuler ce que l'on est en train de faire
 *
 */
void F_AjoutSuppModifJeux::on_Bt_Annuler2_clicked()
{
    // Si on est en mode édition
    if(!AjoutOuModif)
    {
        //////////////////////////////////////////////////
        /////// Annulation modif Age ////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerAge;


        RequeteAnnulerAge.prepare("SELECT AgeMin,AgeMax FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerAge.bindValue(":CodeDuJeu",this->sCodeJeuSelectionne);


        //Exécute la requête
        if (!RequeteAnnulerAge.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerAge" << RequeteAnnulerAge.lastQuery() ;
        }

        RequeteAnnulerAge.next();

        //Récupère les informations dans la base de données et les affiches
        unsigned int  AgeMin = (RequeteAnnulerAge.value(0).toInt());
        unsigned int  AgeMax = (RequeteAnnulerAge.value(1).toInt()) ;
        ui->SBx_AgeMin->setValue(AgeMin);
        ui->SBx_AgeMax->setValue(AgeMax);

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation nbr de joueurs ///////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerNbrJoueurs;

        RequeteAnnulerNbrJoueurs.prepare("SELECT NbrJoueurMin,NbrJoueurMax FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerNbrJoueurs.bindValue(":CodeDuJeu",this->sCodeJeuSelectionne);

        //Exécute la requête
        if (!RequeteAnnulerNbrJoueurs.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerNbrJoueurs" << RequeteAnnulerNbrJoueurs.lastQuery() ;
        }

        RequeteAnnulerNbrJoueurs.next();

        //Récupère les informations dans la base de données et les affiches
        unsigned int  NbrJoueurMin = (RequeteAnnulerNbrJoueurs.value(0).toInt());
        unsigned int  NbrJoueurMax = (RequeteAnnulerNbrJoueurs.value(1).toInt()) ;
        ui->SBx_JoueursMin->setValue(NbrJoueurMin);
        ui->SBx_JoueursMax->setValue(NbrJoueurMax);

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation modif prix ///////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerPrix;


        RequeteAnnulerPrix.prepare("SELECT PrixAchat,PrixLoc FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerPrix.bindValue(":CodeDuJeu",this->sCodeJeuSelectionne);


        //Exécute la requête
        if (!RequeteAnnulerPrix.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerPrix" << RequeteAnnulerPrix.lastQuery() ;
        }

        RequeteAnnulerPrix.next();

        //Récupère les informations dans la base de données et les affiches
        float  PrixAchat = (RequeteAnnulerPrix.value(0).toFloat());
        unsigned int  PrixLoc = (RequeteAnnulerPrix.value(1).toInt()) ;
        ui->SBx_PrixAchat->setValue(PrixAchat);
        ui->SBx_PrixLocation->setValue(PrixLoc);

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation date achat ///////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerDateAchat;

        RequeteAnnulerDateAchat.prepare("SELECT DateAchat FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerDateAchat.bindValue(":CodeDuJeu",this->sCodeJeuSelectionne);

        //Exécute la requête
        if (!RequeteAnnulerDateAchat.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerDateAchat" << RequeteAnnulerDateAchat.lastQuery() ;
        }

        RequeteAnnulerDateAchat.next();

        //Récupère les informations dans la base de données et les affiches
        QDate DateAchat = (RequeteAnnulerDateAchat.value(0).toDate());
        ui->DtE_Achat->setDate(DateAchat) ;

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation du créateur //////////////////
        ////////////////////////////////////////////////
        ActualiserLw_Auteurs();

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation des TxE //////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerTxE;

        RequeteAnnulerTxE.prepare("SELECT Remarque,DescriptionJeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerTxE.bindValue(":CodeDuJeu",this->sCodeJeuSelectionne);

        //Exécute la requête
        if (!RequeteAnnulerTxE.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerTxE" << RequeteAnnulerTxE.lastQuery() ;
        }

        RequeteAnnulerTxE.next();

        //Récupère les informations dans la base de données et les affiches
        ui->W_Contenu->ActualiserContenu();
        ui->TxE_Description->setText(ObtenirValeurParNom(RequeteAnnulerTxE,"DescriptionJeu").toString());
        ui->TxE_Remarque->setText(ObtenirValeurParNom(RequeteAnnulerTxE,"Remarque").toString());

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation du nom  //////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerNomJeu;

        RequeteAnnulerNomJeu.prepare("SELECT NomJeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerNomJeu.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécutee la requête
        if (!RequeteAnnulerNomJeu.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerNomJeu" << RequeteAnnulerNomJeu.lastQuery() ;
        }

        RequeteAnnulerNomJeu.next();

        //Récupère les informations dans la base de données et les affiches
        QString NomJeu = (RequeteAnnulerNomJeu.value(0).toString());
        SearchJeu->setCurrentText(NomJeu);

        //Grise le bouton annuler
        ActiveBoutons(false);

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de l'etat initial ////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerEtatInitial;

        RequeteAnnulerEtatInitial.prepare("SELECT EtatInitial FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteAnnulerEtatInitial.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerEtatInitial.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerEtatInitial" << RequeteAnnulerEtatInitial.lastQuery() ;
        }

        RequeteAnnulerEtatInitial.next();

        //Récupère les informations dans la base de données et les affiches
        QString EtatInitial = (RequeteAnnulerEtatInitial.value(0).toString());
        if(EtatInitial == "Neuf")
        {
            ui->RBt_Neuf->setChecked(true);
        }
        else
        {
            ui->RBt_Occasion->setChecked(true);
        }

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de la classification /////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerClassification;

        RequeteAnnulerClassification.prepare("SELECT TypeJeux_Classification,TypeJeux,Classification "
                                             "FROM jeux,typejeux "
                                             "WHERE CodeJeu=:CodeDuJeu AND Classification=TypeJeux_Classification");
        RequeteAnnulerClassification.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerClassification.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerClassification" << RequeteAnnulerClassification.lastQuery() ;
        }

        RequeteAnnulerClassification.next();

        //Récupère les informations dans la base de données et les affiches
        QString CodeClassification = (RequeteAnnulerClassification.value(0).toString());
        QString NomClassification = (RequeteAnnulerClassification.value(1).toString()) ;
        QString RemplissageCBx = CodeClassification + ": " + NomClassification ;
        ui->LE_CodeClassification->setText(CodeClassification);
        ui->CBx_Classification->setItemText(ui->CBx_Classification->currentIndex(), RemplissageCBx);


        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de l'état ////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerEtat;

        RequeteAnnulerEtat.prepare("SELECT EtatsJeu_IdEtatsJeu,IdEtatsJeu,Etat "
                                   "FROM jeux,etatsjeu WHERE CodeJeu=:CodeDuJeu AND IdEtatsJeu=EtatsJeu_IdEtatsJeu");
        RequeteAnnulerEtat.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerEtat.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerEtat" << RequeteAnnulerEtat.lastQuery() ;
        }

        RequeteAnnulerEtat.next();

        //Se place sur l'index précédent
        ui->CBx_Etat->setCurrentIndex(TableauPositionIndex[0]);

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation du statut ////////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerStatut;

        RequeteAnnulerStatut.prepare("SELECT StatutJeux_IdStatutJeux,IdStatutJeux,StatutJeu "
                                     "FROM jeux,statutjeux WHERE CodeJeu=:CodeDuJeu AND IdStatutJeux=StatutJeux_IdStatutJeux");
        RequeteAnnulerStatut.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerStatut.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerStatut" << RequeteAnnulerStatut.lastQuery() ;
        }

        RequeteAnnulerStatut.next();

        //Se place sur l'index précédent
        ui->CBx_Statut->setCurrentIndex(TableauPositionIndex[1]);

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de l'éditeur /////////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerEditeur;

        RequeteAnnulerEditeur.prepare("SELECT Editeur_IdEditeur,IdFournisseur,NomFournisseur,Editeur "
                                      "FROM jeux,fournisseursediteur "
                                      "WHERE CodeJeu=:CodeDuJeu AND IdFournisseur=Editeur_IdEditeur AND Editeur=1");
        RequeteAnnulerEditeur.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerEditeur.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerEditeur" << RequeteAnnulerEditeur.lastQuery() ;
        }

        RequeteAnnulerEditeur.next();

        //Se place sur l'index précédent
        ui->CBx_Editeur->setCurrentIndex(TableauPositionIndex[2]);

        //------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation du fournisseur ///////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerFournisseur;

        RequeteAnnulerFournisseur.prepare("SELECT Fournisseurs_IdFournisseur,IdFournisseur,NomFournisseur,Fournisseur "
                                          "FROM jeux,fournisseursediteur "
                                          "WHERE CodeJeu=:CodeDuJeu "
                                          "AND IdFournisseur=Fournisseurs_IdFournisseur AND Fournisseur=1");
        RequeteAnnulerFournisseur.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerFournisseur.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerFournisseur" << RequeteAnnulerFournisseur.lastQuery() ;
        }

        RequeteAnnulerFournisseur.next();

        //Se place sur l'index précédent
        ui->CBx_Fournisseur->setCurrentIndex(TableauPositionIndex[3]);

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation de l'emplacement /////////////
        ////////////////////////////////////////////////
        QSqlQuery RequeteAnnulerEmplacement;

        RequeteAnnulerEmplacement.prepare("SELECT Emplacement_IdEmplacement,IdEmplacement,NomEmplacement "
                                          "FROM jeux,emplacement "
                                          "WHERE CodeJeu=:CodeDuJeu AND IdEmplacement=Emplacement_IdEmplacement");
        RequeteAnnulerEmplacement.bindValue(":CodeDuJeu",ui->LE_Code->text());

        //Exécute la requête
        if (!RequeteAnnulerEmplacement.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Annuler_clicked() : AnnulerEmplacement" << RequeteAnnulerEmplacement.lastQuery() ;
        }

        RequeteAnnulerEmplacement.next();

        //Se place sur l'index précédent
        ui->CBx_Emplacement->setCurrentIndex(TableauPositionIndex[4]);

        //--------------------------------------------------------------------------------------------------------------------------
        //////////////////////////////////////////////////
        /////// Annulation MotCle //////////////////////
        ////////////////////////////////////////////////

        ActualiserLw_MotCles();

    }
    else
    {
        ActiveChamps(true);
        ui->LE_Code->clear();
    }
    //Grise le bouton annuler
    ActiveBoutons(false);
    // Rendre utilisable la création et la suppression d'un jeu en cas
    // d'annulation du jeu courant
    ui->Bt_Supprimer->setEnabled(true);
    ui->Bt_Ajouter->setEnabled(true);
}

//###################################################################
/**
 * @brief Méthode qui permet de vider les champs ainsi que de passer en mode ajout (modif booléen)
 *
 */
void F_AjoutSuppModifJeux::on_Bt_Ajouter_clicked()
{
    // Rendre utilisable (cliquable) les widgets de l'IHM
    ActiveChamps(true);

    // Vide tous les champs avant l'ajout
    VideChamps();
    QLocale English(QLocale::English);
    ui->SBx_PrixLocation->setValue(English.toDouble(F_Preferences::ObtenirValeur("UniteLocation")));

    // Provisoire :
    ui->CBx_Classification->setItemText(0, "Sélectionner une classification");
    ui->CBx_Classification->setCurrentIndex(0);

    //Provisoire :
    ui->CBx_Editeur->setItemText(0, "Sélectionner un éditeur");
    ui->CBx_Editeur->setCurrentIndex(0);

    ui->CBx_Emplacement->setCurrentIndex(0);
    ui->CBx_Etat->setCurrentIndex(0);

    //Provisoire :
    ui->CBx_Fournisseur->setItemText(0, "Sélectionner un fournisseur");
    ui->CBx_Fournisseur->setCurrentIndex(0);

    // Par défaut, le jeu est empruntable
    ui->CBx_Statut->setCurrentIndex(1);

    ui->LE_RechercheNom->clear();

    // Affichage de la date d'achat à la date du jour
    QDate DateDefaut ;
    DateDefaut = DateDefaut.currentDate() ;
    ui->DtE_Achat->setDate(DateDefaut);

    QSqlQuery RequetePremierCodeLibre ;
    //Sélectionne le premier CodeJeu qui est libre (supérieur au plus grand)
    RequetePremierCodeLibre.exec("SELECT MIN(CodeJeu+1) FROM jeux WHERE (CodeJeu+1) NOT IN (SELECT CodeJeu FROM jeux)") ;
    RequetePremierCodeLibre.next() ;
    sCodeJeuSelectionne = RequetePremierCodeLibre.value(0).toString() ;
    ui->LE_Code->setText(sCodeJeuSelectionne);
    // On passe en mode Création d'un nouveau jeu, pas en modif d'un jeu
    AjoutOuModif = true ;
    // Autoriser les boutons pour valider ou annuler
    ActiveBoutons(true);
    // Curseur dans le chanmps de saisie du nom du jeu
    SearchJeu->setFocus();
}
//###################################################################
/**
 * @brief Méthode qui permet de supprimer un jeu de la BDD
 *
 */
void F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked()
{
    if(!ChargementImageFini)
    {
        QMessageBox::information(this, "Erreur", "Merci d'attendre la fin de chargement des photos avant supprimer un jeu.") ;
        return;
    }

    QSqlQuery RequeteJeuEmprunte ;
    //Si aucun jeu n'a été sélectionné alors un message d'erreur s'affiche
    if(SearchJeu->currentText() == "" && ui->LE_Code->text() == "")
    {
        QMessageBox::information(this, "Erreur de suppression", "Aucun jeu n'a été sélectionné, la suppression n'est donc pas possible", "Sortir") ;
    }
    else
    {   // Vérifier si le jeu n'est pas emprunté actuellement
        if( RequeteJeuEmprunte.exec( "SELECT IdJeux FROM jeux WHERE StatutJeux_IdStatutJeux=3 AND CodeJeu=" + ui->LE_Code->text() ) )
        {
            if( RequeteJeuEmprunte.next() )
            {
                QMessageBox::information(this, "Erreur de suppression", "Impossible de supprimer un jeu emprunté.", "Sortir") ;
            }
            else
            {
                //Test permettant de savoir quel bouton a été appuyé, entre dans cette méthode si le bouton oui a été cliqué
                if((QMessageBox::information(this, "Confirmation suppression","Voulez-vous vraiment supprimer le jeu "+
                                             SearchJeu->currentText()+"?","Oui", "Non")) == 0)
                {
                    if(EstCeURL(F_Preferences::ObtenirValeur("CheminPhotosJeux")))
                    {
                        // Récupère le chemin de la clé privée de la machine
                        SecureFileUploader * uploader = new SecureFileUploader(this);

                        uploader->EffacerCommandes();
                        uploader->init(F_Preferences::ObtenirValeur("AdresseServeur"),
                                       F_Preferences::ObtenirValeur("LoginServeur"),
                                       F_Preferences::ObtenirValeur("CheminClePrivee"));
                        foreach(QString nom, lb_image->ObtenirsNomImage())
                        {
                            uploader->AjouterCommande(COMMANDE_SUPPRIMER,
                                F_Preferences::ObtenirValeur("CheminPhotosServeur") + "/" + nom,"");
                        }
                        uploader->FaireCommandes();
                    }
                    else
                    {
                        foreach(QString nom, lb_image->ObtenirsNomImage())
                        {
                            QString NomFichierSource=F_Preferences::ObtenirValeur("CheminPhotosJeux") + "/" + nom;
                            QFile FichierSource(NomFichierSource);
                            FichierSource.remove();
                        }
                    }

                    // Sélection de l'Id correspondant au code //
                    QSqlQuery RequeteSelectionJeux ;
                    RequeteSelectionJeux.prepare("SELECT IdJeux, CodeJeu FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
                    RequeteSelectionJeux.bindValue(":CodeDuJeu", ui->LE_Code->text());
                    RequeteSelectionJeux.exec() ;
                    RequeteSelectionJeux.next() ;
                    int IdJeux=ObtenirValeurParNom(RequeteSelectionJeux,"IdJeux").toInt();

                    ///// Suppr Historique Interventions //////
                    QSqlQuery RequeteSuppHistInter ;
                    RequeteSuppHistInter.prepare("DELETE FROM intervientionsjeu WHERE Jeux_IdJeux=:LeJeux_IdJeux") ;
                    RequeteSuppHistInter.bindValue(":LeJeux_IdJeux", IdJeux );
                    if(!RequeteSuppHistInter.exec())
                    {
                        qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked() : RequeteSuppHistInter" << RequeteSuppHistInter.lastQuery() ;
                    }
                    RequeteSuppHistInter.next() ;

                    ///// Suppr Historique Emprunt ////////////
                    QSqlQuery RequeteSuppHistEmprunt ;
                    RequeteSuppHistEmprunt.prepare("DELETE FROM emprunts WHERE Jeux_IdJeux=:Jeux_IdDuJeux") ;
                    RequeteSuppHistEmprunt.bindValue(":Jeux_IdDuJeux", IdJeux);
                    if(!RequeteSuppHistEmprunt.exec())
                    {
                        qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked()-RequeteSuppHistEmprunt" << RequeteSuppHistEmprunt.lastQuery() ;

                    }
                    RequeteSuppHistEmprunt.next() ;

                    ///// Suppr Historique Résa ///////////////
                    QSqlQuery RequeteSuppHistReservation ;
                    RequeteSuppHistReservation.prepare("DELETE FROM reservation WHERE Jeux_IdJeux=:Jeux_IdLeJeux");
                    RequeteSuppHistReservation.bindValue(":Jeux_IdLeJeux", IdJeux);
                    if(!RequeteSuppHistReservation.exec())
                    {
                        qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked() : RequeteSuppHistReservation" << RequeteSuppHistReservation.lastQuery() ;

                    }

                    ///// Suppression des mots-clés du jeu ///////////////
                    QSqlQuery RequeteSuppMotsCles;
                    RequeteSuppMotsCles.prepare("DELETE FROM motsclesassoc WHERE Jeux_IdJeux=:Jeux_IdLeJeux");
                    RequeteSuppMotsCles.bindValue(":Jeux_IdLeJeux", IdJeux);
                    if(!RequeteSuppMotsCles.exec())
                    {
                        qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked() : RequeteSuppHistReservation"
                                 << getLastExecutedQuery(RequeteSuppMotsCles) ;

                    }

                    //Création de la requête SQL
                    QSqlQuery RequeteSuppressionJeux ;
                    //Préparation de la requête SQL
                    RequeteSuppressionJeux.prepare("DELETE FROM jeux WHERE CodeJeu=:CodeDuJeu") ;
                    //Copie la valeur récuper sur l'interface dans la variable SQL
                    RequeteSuppressionJeux.bindValue(":CodeDuJeu", ui->LE_Code->text() );
                    //Éxécutre la requête, mais affiche une erreur si il y en a une
                    if (!RequeteSuppressionJeux.exec())
                    {
                        //Affiche la dernière erreur
                        qDebug()<<"F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked()-Suppr jeu"<< RequeteSuppressionJeux.lastQuery();
                    }
                    RequeteSuppressionJeux.next() ;

                    //Vider tous les champs après la suppression du jeu
                    VideChamps();

                    //QDate DateDefaut ;
                    //DateDefaut = DateDefaut.currentDate() ;
                    ui->DtE_Achat->setDate(QDate::currentDate());
                    // Bloquer les boutons Valider/Annuler et Supprimer
                    ActiveBoutons(false);
                    ui->Bt_Supprimer->setEnabled(false);
                    // Autoriser la création d'un nouveau jeu
                    ui->Bt_Ajouter->setEnabled(true);
                    // Remettre à jour la liste des jeux puisqu'un jeu a été enlevé de la liste
                    ui->LE_RechercheNom->clear();
                    AfficherJeux();
                }
            }
        }
        else
        {
            qDebug() << "F_AjoutSuppModifJeux::on_Bt_Supprimer_clicked() : RequetejeuxEmprunte : " << RequeteJeuEmprunte.lastQuery() ;
        }
    }
}
//###################################################################
/**
 * @brief Méthode qui recherche le jeu correspondant au code entrée, et affiche les informations
 *
 */
void F_AjoutSuppModifJeux::AfficherJeu(QString CodeJeu)
{
    this->sCodeJeuSelectionne=CodeJeu;
    ui->Bt_AjouterLienJeux->setEnabled(true);
    ui->W_Contenu->Definir_CodeJeu(sCodeJeuSelectionne);
    ui->W_Historique->Definir_CodeJeu(sCodeJeuSelectionne);

    qDebug() << "F_AjoutSuppModifJeux::AfficherJeu() ++++++++++++++++++++++++++++++++++++" ;
    // Débloque tous les champs de saisie en vue d'une modification possible
    ActiveChamps(true);
    ChargementImageFini=false;
    BloquerSignalsChamps(true);

    // Vérifie si un jeu n'est pas en cours de modification
    // Evite de perdre des modif avant de passer à un autre jeu
    if(ui->Bt_Valider2->isEnabled() && ui->Bt_Annuler2->isEnabled())
    {
        QMessageBox::information(this, "Pré-requis pour passer à un autre jeu !", "Attention, avant d'effectuer une nouvelle recherche vous devez valider ou annuler vos modifications sur le jeu en cours !", "OK") ;
    }
    else
    {
        //////////// Recherche par code //////////////////
        QSqlQuery RequeteRechercheJeu;

        RequeteRechercheJeu.prepare("SELECT * FROM jeux WHERE CodeJeu=:CodeDuJeu");
        RequeteRechercheJeu.bindValue(":CodeDuJeu",this->sCodeJeuSelectionne);
        if(!RequeteRechercheJeu.exec())
        {
            qDebug() << " F_AjoutSuppModifJeux::AfficherJeu() : Erreur RequeteRechercheJeu="<<RequeteRechercheJeu.lastQuery() ;
        }
        RequeteRechercheJeu.next();
        //-------------------------------------------------------------------------------------------------
        QString CodeJeu = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("CodeJeu")).toString() ;
        ui->LE_Code->setText(CodeJeu);
        //-------------------------------------------------------------------------------------------------
        QString NomJeu = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("NomJeu")).toString() ;
        SearchJeu->setCurrentText(NomJeu);
        //ui->LE_Nom->setCursorPosition(0) ;
        //-------------------------------------------------------------------------------------------------
        QString SiteWeb1Jeu = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("SiteWeb1")).toString() ;
        ui->LE_SiteWeb1->setText(SiteWeb1Jeu);
        //-------------------------------------------------------------------------------------------------
        ui->SBx_DureeMin->setValue(ObtenirValeurParNom(RequeteRechercheJeu,"DureeMin").toInt());
        ui->SBx_DureeMax->setValue(ObtenirValeurParNom(RequeteRechercheJeu,"DureeMax").toInt());
        ui->SBx_Note->setValue(ObtenirValeurParNom(RequeteRechercheJeu,"Note").toInt());
        //-------------------------------------------------------------------------------------------------
        ui->CBx_Auteur->setCurrentIndex(0);
        ActualiserLw_Auteurs();
        //-------------------------------------------------------------------------------------------------
        QDate DateAchat = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("DateAchat")).toDate() ;
        ui->DtE_Achat->setDate(DateAchat);
        //-------------------------------------------------------------------------------------------------
        double PrixAchat = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("PrixAchat")).toDouble() ;
        ui->SBx_PrixAchat->setValue(PrixAchat);
        //-------------------------------------------------------------------------------------------------
        unsigned int PrixLoc = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("PrixLoc")).toInt() ;
        ui->SBx_PrixLocation->setValue(PrixLoc);
        //-------------------------------------------------------------------------------------------------
        unsigned int NbrJoueurMin = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("NbrJoueurMin")).toInt() ;
        ui->SBx_JoueursMin->setValue(NbrJoueurMin);
        //-------------------------------------------------------------------------------------------------
        unsigned int NbrJoueurMax = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("NbrJoueurMax")).toInt() ;
        ui->SBx_JoueursMax->setValue(NbrJoueurMax);
        //-------------------------------------------------------------------------------------------------
        unsigned int AgeMin = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("AgeMin")).toInt() ;
        ui->SBx_AgeMin->setValue(AgeMin);
        //-------------------------------------------------------------------------------------------------
        unsigned int AgeMax = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("AgeMax")).toInt() ;
        ui->SBx_AgeMax->setValue(AgeMax) ;
        //-------------------------------------------------------------------------------------------------
        int CurrentIndex=ui->TabW_Jeu->currentIndex();
        ui->TabW_Jeu->setCurrentIndex(2);
        ui->TabW_Jeu->setCurrentIndex(CurrentIndex);
        ui->W_Contenu->ActualiserContenu();
        //-------------------------------------------------------------------------------------------------
        ui->TxE_Description->setText(ObtenirValeurParNom(RequeteRechercheJeu,"DescriptionJeu").toString());
        //-------------------------------------------------------------------------------------------------
        ui->TxE_Remarque->setText(ObtenirValeurParNom(RequeteRechercheJeu,"Remarque").toString());
        //--------Remplissage CBx Classification-----------------------------------------------------------------------------------
        QString CodeClassification = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("TypeJeux_Classification")).toString() ;
        ui->LE_CodeClassification->setText(CodeClassification);

        QSqlQuery RequeteNomClassification ;
        RequeteNomClassification.prepare("SELECT TypeJeux,Classification FROM typejeux WHERE Classification =:CodeDeClassification") ;
        RequeteNomClassification.bindValue(":CodeDeClassification", ui->LE_CodeClassification->text());
        if(!RequeteNomClassification.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::AfficherJeu() : RequeteNomClassification="<<RequeteNomClassification.lastQuery() ;
        }
        RequeteNomClassification.next();

        QString NomClassification = RequeteNomClassification.value(0).toString() ;
        QString NumClassification = RequeteNomClassification.value(1).toString() ;
        QString AffichageCBxClassification = NumClassification + ": " + NomClassification ;

        ui->CBx_Classification->setItemText(0, AffichageCBxClassification);

        //--------------Remplissage CBx Statut----------------------------------------------------------------------------
        QSqlQuery RequeteStatut ;
        QString IdStatut = RequeteRechercheJeu.value(RequeteRechercheJeu.record().
                                                     indexOf("StatutJeux_IdStatutJeux")).toString() ;
        ui->CBx_Statut->setCurrentIndex(ui->CBx_Statut->findData(IdStatut)+1);

        TableauPositionIndex[1] = ui->CBx_Statut->currentIndex() ;

        //-------------------Remplissage CBx Etat-------------------------------------------------------------
        QSqlQuery RequeteEtat ;
        QString IdEtat = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("EtatsJeu_IdEtatsJeu")).toString() ;

        RequeteEtat.prepare("SELECT Etat,IdEtatsJeu FROM etatsjeu WHERE IdEtatsJeu =:IdEtatDuJeu") ;
        RequeteEtat.bindValue(":IdEtatDuJeu", IdEtat);
        if(!RequeteEtat.exec())
        {
            qDebug() << "F_AjoutSuppModifJeux::AfficherJeu() : RequeteEtat="<<RequeteEtat.lastQuery() ;
        }
        RequeteEtat.next() ;
        ui->CBx_Etat->setCurrentIndex(RequeteEtat.value(1).toInt());
        TableauPositionIndex[0] = ui->CBx_Etat->currentIndex() ;
        //-----------------Remplissage CBx Editeur---------------------------------------------------------------
        QSqlQuery RequeteEditeur ;
        QString IdEditeur = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("Editeur_IdEditeur")).toString() ;
        if(IdEditeur != 0)
        {
            RequeteEditeur.prepare("SELECT NomFournisseur,IdFournisseur,Editeur "
                                   "FROM fournisseursediteur "
                                   "WHERE IdFournisseur =:IdDuEditeur ORDER BY NomFournisseur") ;
            RequeteEditeur.bindValue(":IdDuEditeur", IdEditeur);
            if(!RequeteEditeur.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_OK_clicked() : RequeteEditeur="<<RequeteEditeur.lastQuery() ;
            }
            RequeteEditeur.next() ;
            // Affiche dans le combobox l'éditeur pour ce jeu
            QString NomEditeur = RequeteEditeur.value(0).toString() ;
            ui->CBx_Editeur->setItemText(0, NomEditeur);
            //ui->CBx_Editeur->setCurrentIndex(RequeteEditeur.value(1).toInt());
            TableauPositionIndex[2] = ui->CBx_Editeur->currentIndex() ;
        }
        //-----------------Remplissage CBx Fournisseur---------------------------------------------------------
        QSqlQuery RequeteFournisseur ;
        QString idFournisseur = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("Fournisseurs_IdFournisseur")).toString() ;
        if(idFournisseur != 0)
        {
            RequeteFournisseur.prepare("SELECT NomFournisseur,IdFournisseur,Fournisseur "
                                       "FROM fournisseursediteur "
                                       "WHERE IdFournisseur=:IdDuFournisseur ORDER BY NomFournisseur") ;
            RequeteFournisseur.bindValue(":IdDuFournisseur", idFournisseur);
            if(!RequeteFournisseur.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_OK_clicked() : RequeteFournisseur="<<RequeteFournisseur.lastQuery() ;
            }
            RequeteFournisseur.next() ;

            // Affiche dans le combobox le Fournisseur pour ce jeu
            QString NomFournisseur = RequeteFournisseur.value(0).toString() ;
            ui->CBx_Fournisseur->setItemText(0, NomFournisseur);

            //ui->CBx_Fournisseur->setCurrentIndex(RequeteFournisseur.value(1).toInt());
            TableauPositionIndex[3] = ui->CBx_Fournisseur->currentIndex() ;
            //qDebug() << "F_AjoutSuppModifJeux::on_Bt_OK_clicked()-La case 3 du tableau est rempli avec :" << TableauPositionIndex[3] ;
        }
        //-----------------Remplissage CBx Emplacement -----------------------------------------------------
        QSqlQuery RequeteEmplacement ;
        QString IdEmplacement = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("Emplacement_IdEmplacement")).toString() ;
        if(IdEmplacement != 0)
        {
            RequeteEmplacement.prepare("SELECT IdEmplacement,NomEmplacement FROM emplacement WHERE IdEmplacement =:IdDeEmplacement") ;
            RequeteEmplacement.bindValue(":IdDeEmplacement", IdEmplacement);
            if(!RequeteEmplacement.exec())
            {
                qDebug() << "F_AjoutSuppModifJeux::on_Bt_OK_clicked() : RequeteEmplacement="<<RequeteEmplacement.lastQuery() ;
            }
            RequeteEmplacement.next() ;
            // Affiche dans le combobox l'emplacement pour ce jeu
            QString NomEmplacement = RequeteEmplacement.value(1).toString() ;
            ui->CBx_Emplacement->setItemText(0, NomEmplacement);

            //ui->CBx_Emplacement->setCurrentIndex(RequeteEmplacement.value(0).toInt());
            TableauPositionIndex[4] = ui->CBx_Emplacement->currentIndex() ;
        }
        ui->CBx_MotCle->setCurrentIndex(0);
        ActualiserLw_MotCles();
        //--------------Cocher EtatInitial-----------------------------------------------------------------
        QString EtatInitial = RequeteRechercheJeu.value(RequeteRechercheJeu.record().indexOf("EtatInitial")).toString() ;

        if(EtatInitial == "Neuf")
        {
            ui->RBt_Neuf->setChecked(true);
        }
        else
        {
            if(EtatInitial == "Occasion")
            {
                ui->RBt_Occasion->setChecked(true);
            }
            else
            {
                ui->RBt_Neuf->setChecked(false);
                ui->RBt_Occasion->setChecked(false);
            }
        }
        //----------Affichage photo----------------------------------------------------------------------------
        lb_image->setCursor(Qt::WaitCursor);
        disconnect( lb_image, SIGNAL( SignalClic() ), this, SLOT( on_Lb_Image_clicked() ) );
        lb_image->ChargerImage(QSize(200,160),this->sCodeJeuSelectionne,Qt::WaitCursor);
        // Grise les boutons valider et annuler
        // mais autoriser la création d'un nouveau jeu ou la suppression du jeu courant
        ActiveBoutons(false);
        ui->Bt_Supprimer->setEnabled(true);
        ui->Bt_Ajouter->setEnabled(true);
        ui->W_Contenu->ActiverBoutonsContenu(true);
        AjoutOuModif = false ;
    }
    BloquerSignalsChamps(false);
    qDebug() << "F_AjoutSuppModifJeux::on_Bt_OK_clicked() ------------------------------------";
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Editeur après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Editeur()
{
    ui->CBx_Editeur->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Editeur /////////
    //////////////////////////////////////////

    QSqlQuery RequeteEditeur ;

    RequeteEditeur.exec("SELECT DISTINCT IdFournisseur, NomFournisseur "
                        "FROM fournisseursediteur "
                        "WHERE Editeur = 1 ORDER BY NomFournisseur") ;

    ui->CBx_Editeur->addItem("Sélectionner un éditeur");

    int i=1;
    while(RequeteEditeur.next())
    {
        QString NomEditeur = ObtenirValeurParNom(RequeteEditeur,"NomFournisseur").toString();
        QString IdFournisseur = ObtenirValeurParNom(RequeteEditeur,"IdFournisseur").toString();
        ui->CBx_Editeur->addItem(NomEditeur);
        // Ajoute son ID dans la partie DATA
        ui->CBx_Editeur->setItemData(i++,IdFournisseur,Qt::UserRole);
    }
    ui->CBx_Editeur->addItem("Ajouter un éditeur");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Fournisseur après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Fournisseur()
{
    ui->CBx_Fournisseur->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Fournisseur /////
    //////////////////////////////////////////

    QSqlQuery RequeteFournisseur ;

    RequeteFournisseur.exec("SELECT DISTINCT NomFournisseur,Fournisseur "
                            "FROM fournisseursediteur "
                            "WHERE Fournisseur = 1 ORDER BY NomFournisseur") ;

    ui->CBx_Fournisseur->addItem("Sélectionner un fournisseur");
    while(RequeteFournisseur.next())
    {

        QString NomFournisseur = (RequeteFournisseur.value(0).toString()) ;
        ui->CBx_Fournisseur->addItem(NomFournisseur);
    }
    ui->CBx_Fournisseur->addItem("Ajouter un fournisseur");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Statut après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Statut()
{
    ui->CBx_Statut->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Emplacement /////
    //////////////////////////////////////////

    QSqlQuery RequeteStatut ;

    RequeteStatut.exec("SELECT IdStatutJeux,StatutJeu FROM statutjeux") ;
    ui->CBx_Statut->addItem("Sélectionner un statut");
    int i=0;
    while(RequeteStatut.next())
    {
        QString NomStatut = RequeteStatut.value(1).toString();
        ui->CBx_Statut->addItem(NomStatut);
        ui->CBx_Statut->setItemData(i++,RequeteStatut.value(0).toString());
    }

    ui->CBx_Statut->addItem("Ajouter un statut");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Etat après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBX_Etat()
{
    ui->CBx_Etat->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Emplacement /////
    //////////////////////////////////////////

    QSqlQuery RequeteEtat ;

    RequeteEtat.exec("SELECT DISTINCT Etat FROM etatsjeu") ;
    ui->CBx_Etat->addItem("Sélectionner un état");

    while(RequeteEtat.next())
    {
        QString NomEtat = (RequeteEtat.value(0).toString()) ;
        ui->CBx_Etat->addItem(NomEtat);
    }
    ui->CBx_Etat->addItem("Ajouter un état");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Classification après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Classification()
{
    ui->CBx_Classification->clear();

    QSqlQuery RequeteClassification ;

    RequeteClassification.exec("SELECT DISTINCT TypeJeux,Classification FROM typejeux ORDER BY Classification ASC") ;

    ui->CBx_Classification->addItem("Sélectionner une classification");

    while(RequeteClassification.next())
    {

        QString NomClassification = (RequeteClassification.value(0).toString()) ;
        QString CodeClassification = (RequeteClassification.value(1).toString()) ;
        QString AffichageCBx = CodeClassification + ":" + NomClassification ;
        ui->CBx_Classification->addItem(AffichageCBx);
    }
    ui->CBx_Classification->addItem("Ajouter une classification");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx emplacement après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Emplacement()
{
    ui->CBx_Emplacement->clear();

    QSqlQuery RequeteEmplacement ;

    RequeteEmplacement.exec("SELECT DISTINCT NomEmplacement FROM emplacement") ;
    ui->CBx_Emplacement->addItem("Sélectionner un emplacement");
    while(RequeteEmplacement.next())
    {

        QString NomEmplacement = (RequeteEmplacement.value(0).toString()) ;
        ui->CBx_Emplacement->addItem(NomEmplacement);
    }
    ui->CBx_Emplacement->addItem("Ajouter un emplacement");
}
//###################################################################
/**
 * @brief Méthode qui actualise le CBx_MotCle après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_MotCle()
{
    ui->CBx_MotCle->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_MotCles /////////
    //////////////////////////////////////////

    QSqlQuery RequeteMotCle ;

    RequeteMotCle.exec("SELECT DISTINCT MotCle,Id_MotCle FROM motscles ORDER BY MotCle") ;

    ui->CBx_MotCle->addItem("Choisir le mot clé");

    int i=1;
    while(RequeteMotCle.next())
    {

        QString MotCle = ObtenirValeurParNom(RequeteMotCle,"MotCle").toString();
        QString IdMotCle = ObtenirValeurParNom(RequeteMotCle,"Id_MotCle").toString();
        ui->CBx_MotCle->addItem(MotCle);
        // Ajoute son ID dans la partie DATA
        ui->CBx_MotCle->setItemData(i++,IdMotCle,Qt::UserRole);
    }
    ui->CBx_MotCle->addItem("Ajouter un nouveau mot clé");
}

//###################################################################
/**
 * @brief Méthode qui actualise le CBx_Auteur après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserCBx_Auteur()
{
    ui->CBx_Auteur->clear();
    ////////////////////////////////////////////
    ///////// Remplissage CBx_Auteur /////////
    //////////////////////////////////////////

    QSqlQuery Requete;

    Requete.exec("SELECT DISTINCT NomAuteur,IdAuteur FROM auteurs ORDER BY NomAuteur") ;

    ui->CBx_Auteur->addItem("Choisir l'auteur");

    int i=1;
    while(Requete.next())
    {
        QString Auteur = ObtenirValeurParNom(Requete,"NomAuteur").toString();
        QString IdAuteur = ObtenirValeurParNom(Requete,"IdAuteur").toString();
        ui->CBx_Auteur->addItem(Auteur);
        // Ajoute son ID dans la partie DATA
        ui->CBx_Auteur->setItemData(i++,IdAuteur,Qt::UserRole);
    }
    ui->CBx_Auteur->addItem("Ajouter un nouvel auteur");
}

//###################################################################
/**
 * @brief Méthode qui actualise le Lw_MotCle après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserLw_MotCles()
{
    ui->Lw_MotsCles->clear();

    QSqlQuery RequeteMotCle ;

    RequeteMotCle.prepare("SELECT DISTINCT MotCle,Id_MotCle FROM motscles LEFT JOIN motsclesassoc ON Motscles_IdMotCle=Id_MotCle "
                       "LEFT JOIN jeux ON Jeux_IdJeux=IdJeux WHERE CodeJeu=:CodeJeu ORDER BY MotCle") ;
    RequeteMotCle.bindValue(":CodeJeu",sCodeJeuSelectionne);
    if(!RequeteMotCle.exec())
    {
        qDebug()<< getLastExecutedQuery(RequeteMotCle)<<RequeteMotCle.lastError();
        return;
    }

    int i=0;
    while(RequeteMotCle.next())
    {

        QString MotCle = ObtenirValeurParNom(RequeteMotCle,"MotCle").toString();
        QString IdMotCle = ObtenirValeurParNom(RequeteMotCle,"Id_MotCle").toString();
        QListWidgetItem *Item = new QListWidgetItem(MotCle);
        // Ajoute son ID dans la partie DATA
        Item->setData(Qt::UserRole,IdMotCle);
        ui->Lw_MotsCles->insertItem(ui->Lw_MotsCles->count()-1,Item);
    }
}

//###################################################################
/**
 * @brief Méthode qui actualise le Lw_MotCle après un ajout
 *
 */
void F_AjoutSuppModifJeux::ActualiserLw_Auteurs()
{
    ui->Lw_Auteurs->clear();

    QSqlQuery Requete;

    Requete.prepare("SELECT DISTINCT NomAuteur,IdAuteur FROM auteurs LEFT JOIN auteursassoc ON "
                    "Auteurs_IdAuteur=IdAuteur LEFT JOIN jeux ON Jeux_IdJeux=IdJeux WHERE "
                    "CodeJeu=:CodeJeu ORDER BY NomAuteur") ;
    Requete.bindValue(":CodeJeu",sCodeJeuSelectionne);
    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
        return;
    }

    int i=0;
    while(Requete.next())
    {

        QString NomAuteur = ObtenirValeurParNom(Requete,"NomAuteur").toString();
        QString IdAuteur = ObtenirValeurParNom(Requete,"IdAuteur").toString();
        QListWidgetItem *Item = new QListWidgetItem(NomAuteur);
        // Ajoute son ID dans la partie DATA
        Item->setData(Qt::UserRole,IdAuteur);
        ui->Lw_Auteurs->insertItem(ui->Lw_Auteurs->count()-1,Item);
    }

}
//###################################################################
/**
 *  Grise les boutons Supprimer et ajouter et active les boutons Valider et Annuler
 */
void F_AjoutSuppModifJeux::CacherBoutons()
{
    ActiveBoutons(true);
    ui->Bt_Supprimer->setEnabled(false);
    ui->Bt_Ajouter->setEnabled(false);
    ui->TbV_Recherche->setEnabled(false);
}

void F_AjoutSuppModifJeux::on_Bt_Gauche_clicked()
{
    lb_image->AfficherImagePrecedente();
}

void F_AjoutSuppModifJeux::on_Bt_Droite_clicked()
{
    lb_image->AfficherImageSuivante();
}

void F_AjoutSuppModifJeux::on_Lb_Image_clicked()
{
    // Bloque les signaux pour éviter qu'il soit appelé dans la QDialog
    lb_image->blockSignals(true);
    // Afficher l'image et passe à D_Image la liste des fichiers images temporaires
    this->pImage->AfficherImage(this->sCodeJeuSelectionne,lb_image->ObtenirsCheminImage());
    // Mets la fenêtre au max
    this->pImage->showMaximized();
    this->pImage->exec();
    //delete this->pImage;
    this->pImage = new D_Image(this,lb_image);
    // Remet lb_image dans la fenêtre principale Jeux
    ui->gridLayout_11->addWidget(lb_image,0,1);
    // Débloque les signaux
    lb_image->blockSignals(false);
}


/**
 *  @brief Mets en majuscule le texte saisi dans un QLineEdit
 *
 *  @param text
 */

void F_AjoutSuppModifJeux::toUpper(const QString &text)
{
    QLineEdit *le = qobject_cast<QLineEdit *>(sender());
    if (!le)
    return;
    int curPos=le->cursorPosition();
    le->setText(text.toUpper());
    le->setCursorPosition(curPos);
}

/**
 * @brief Méthode qui récupère le code du jeu sur lequel on a cliqué dans le tableau
 *
 * @param index
 */
void F_AjoutSuppModifJeux::on_TbV_Recherche_clicked(const QModelIndex &index)
{
    if(ui->Bt_Valider2->isEnabled() && ui->Bt_Annuler2->isEnabled())
    {
        QMessageBox::information(this, "Pré-requis pour passer à un autre jeu !",
                 "Attention, avant d'effectuer une nouvelle recherche vous devez valider ou annuler vos modifications sur le jeu en cours !", "OK") ;
        return;
    }
//    ModelJeu->item(index.row(),0)->setFlags(QAbstractItemModel::flags() | Qt::ItemIsSelectable);
//    ui->TbV_Recherche->selectionModel()->setCurrentIndex(this->ModelJeu->index(index.row(), 0),QItemSelectionModel::Current);
//    ModelJeu->item(index.row(),0)->setSelectable(true);
    AfficherJeu(this->ModelJeu->index(index.row(), 0).data().toString());
    ActualiserLienJeux();
}

void F_AjoutSuppModifJeux::slot_ActiverClicImage()
{
    //Gestion de l'évenement MousePress
    connect( lb_image, SIGNAL( SignalClic() ), this, SLOT( on_Lb_Image_clicked() ) );
    lb_image->setCursor(Qt::CrossCursor);
    ChargementImageFini=true;
}

void F_AjoutSuppModifJeux::on_Bt_AjouterLienJeux_clicked()
{
    D_LienJeux dLienJeux(this,sCodeJeuSelectionne);
    dLienJeux.exec() ;
    ActualiserLienJeux();
}

void F_AjoutSuppModifJeux::ActualiserLienJeux()
{
    ui->TbW_LiensJeux->clearContents();
    ui->TbW_LiensJeux->setRowCount(0);
    int i=0;
    QSqlQuery Requete;
    Requete.prepare("SELECT IdLienJeuxJeux,NomJeu,IF(Jeux_IdJeuxSource=(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu),DescriptifDestination,DescriptifSource) "
                    "as Descriptif,NomLien FROM lienjeuxjeux LEFT JOIN jeux ON IdJeux="
                    "IF(Jeux_IdJeuxSource=(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu),Jeux_IdJeuxDestination,Jeux_IdJeuxSource)  "
                    "LEFT JOIN typelien ON TypeLien_IdTypeLien=IdTypeLien WHERE Jeux_IdJeuxSource="
                    "(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu) OR Jeux_IdJeuxDestination="
                    "(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu)");
    Requete.bindValue(":CodeJeu",sCodeJeuSelectionne);

    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    else
    {
        while(Requete.next())
        {
            ui->TbW_LiensJeux->setRowCount(i+1);
            QTableWidgetItem *item=new QTableWidgetItem(ObtenirValeurParNom(Requete,"NomJeu").toString());
            item->setData(Qt::UserRole,ObtenirValeurParNom(Requete,"IdLienJeuxJeux").toInt());
            ui->TbW_LiensJeux->setItem(i,0,item);
            ui->TbW_LiensJeux->setItem(i,1,new QTableWidgetItem(ObtenirValeurParNom(Requete,"Descriptif").toString()));
            ui->TbW_LiensJeux->setItem(i++,2,new QTableWidgetItem(ObtenirValeurParNom(Requete,"NomLien").toString()));
        }
    }
}

void F_AjoutSuppModifJeux::on_TbW_LiensJeux_clicked(const QModelIndex &index)
{
    ui->Bt_SupprimerLienJeux->setEnabled(true);
}

void F_AjoutSuppModifJeux::on_Bt_SupprimerLienJeux_clicked()
{
    QSqlQuery Requete;
    Requete.prepare("DELETE FROM lienjeuxjeux WHERE IdLienJeuxJeux=:IdLienJeuxJeux");
    Requete.bindValue(":IdLienJeuxJeux",ui->TbW_LiensJeux->currentItem()->data(Qt::UserRole));
    Requete.exec();
    ActualiserLienJeux();
    ui->Bt_Supprimer->setEnabled(false);
}

/**
 * @brief Méthode qui récupère le code du jeu en cours de consultation
 *
 * @return QString
 */
QString F_AjoutSuppModifJeux::get_JeuEnConsultation()
{
    return this->sCodeJeuSelectionne ;
}

void F_AjoutSuppModifJeux::on_Bt_Aide_PiecesManquantes_clicked()
{
    QMessageBox::information(this,"Description des boutons","- Bouton d'ajout (+) :\n"
                            "Permet de rajouter des pièces simples, des groupes de pièces et des pièces dans un groupe.\n"
                            "Pour ce dernier type de pièce, il est nécessaire d’avoir sélectionné préalablement le groupe où rajouter la pièce.\n"
                            "- Bouton suppression (croix rouge) :\n"
                            "Ce bouton permet de supprimer l’élément sélectionné. S’il s’agit d’un groupe, une confirmation est demandée avant la suppression\n"
                            "- Bouton de déplacement (flèches bleues haut et base) :\n"
                            "Ces boutons permettent de déplacer une ligne en bas ou en haut.\n"
                            "Si l’élément qui se trouve à destination du déplacement est un groupe, la ligne est intégrée dans le groupe.\n"
                            "- Bouton de conversion (sac avec double flèche suivi d'un pièce) :\n"
                            "Ce bouton permet de convertir une pièce simple en groupe ou un groupe en pièce simple.\n"
                            "- Bouton de découpage (ciseaux) :"
                            "Ce bouton permet de découper une ligne qui contient plusieurs pièces.\n"
                            "Par exemple, une ligne qui contiendrait \"sac, 3 chemises, 2 pantalons\"\n"
                            "serait transformés en 3 lignes : sac, chemises, pantalons.","OK");
}

void F_AjoutSuppModifJeux::on_Bt_ValeurOrigine_clicked()
{
    QSqlQuery Requete;

    Requete.prepare("SELECT ContenuJeu FROM jeux WHERE CodeJeu=:CodeDuJeu");
    Requete.bindValue(":CodeDuJeu",this->sCodeJeuSelectionne);

    //Exécute la requête
    if (!Requete.exec())
    {
        qDebug() << Requete.lastQuery() ;
    }

    Requete.next();
    QMessageBox::information(this,"Valeur d'origine du champs Contenu",ObtenirValeurParNom(Requete,"ContenuJeu").toString());
}

void F_AjoutSuppModifJeux::slot_HTTP_API_JeuChoisi(QString IdBGG)
{
    this->IdBGG=IdBGG;
    Requete=new Http_xml_api();
    Requete->LancerTelechargement("https://api.geekdo.com/xmlapi2/thing?versions=1&stats=1&id="+IdBGG,MODE_BGG_VER);
    connect(Requete,SIGNAL(SignalTelechargementFiniBGGVersion(QVector<QVector<QVector<QString>>>)),this,
            SLOT(slot_AfficherChoixVersion(QVector<QVector<QVector<QString>>>)));
}

void F_AjoutSuppModifJeux::slot_AfficherChoixVersion(QVector<QVector<QVector<QString>>> Matrice)
{
    this->Matrice=Matrice;
    // Si il n'y a pas de versions dans le jeu choisi, on appelle directement la fonction slot_VersionBGGChoisi()
    if(Matrice[0][BGG_VERSION][0]=="")
    {
        slot_VersionBGGChoisi("");
        return;
    }
    D_Image *d_image = new D_Image(this);
    connect(d_image,SIGNAL(SignalVersionBGGChoisi(QString)),this,SLOT(slot_VersionBGGChoisi(QString)));
    //on_Bt_Ajouter_clicked();
    d_image->AfficherImageBGG(ui->LE_Code->text(),Matrice);
    d_image->exec();
    delete d_image;
}

void F_AjoutSuppModifJeux::slot_VersionBGGChoisi(QString IdVersionBGG)
{
    ui->LE_Id_BGG->setText(IdBGG);
    ui->LE_Version_BGG->setText(IdVersionBGG);
    if(IdVersionBGG=="")
    {
        RemplirChampsMatrice(0);
        return;
    }
    lb_image->setCursor(Qt::WaitCursor);
    disconnect( lb_image, SIGNAL( SignalClic() ), this, SLOT( on_Lb_Image_clicked() ) );
    lb_image->ChargerImage(QSize(200,160),ui->LE_Code->text(),Qt::WaitCursor);
    for(int i=0;i<Matrice.size();i++)
    {
        // On cherche le jeu sélectionné par l'utilisateur
        if(Matrice[i][BGG_VERSION][0]==IdVersionBGG)
        {
            RemplirChampsMatrice(i);
        }
    }
}

void F_AjoutSuppModifJeux::RemplirChampsMatrice(int i)
{
    for(int j=0;j<Matrice[i][BGG_MOTSCLES].size();j++)
    {
        int index=ui->CBx_MotCle->findData(Matrice[i][BGG_MOTSCLES][j],Qt::UserRole,Qt::MatchCaseSensitive);
        ui->CBx_MotCle->setCurrentIndex(index);
        on_Bt_AjouterMotCle_clicked();
    }
    ui->CBx_MotCle->setCurrentIndex(0);

    QSqlQuery Requete;
    for(int j=0;j<Matrice[i][BGG_AUTEURS_ID].size();j++)
    {
        int index=ui->CBx_Auteur->findData(Matrice[i][BGG_AUTEURS_ID][j],Qt::UserRole,Qt::MatchCaseSensitive);
        // Si l'auteur est introuvable, on l'ajoute
        if(index==-1)
        {
            Requete.prepare("INSERT INTO auteurs(IdAuteur,NomAuteur) VALUES (:IdAuteur,:NomAuteur)");
            Requete.bindValue(":IdAuteur",Matrice[i][BGG_AUTEURS_ID][j]);
            Requete.bindValue(":NomAuteur",Matrice[i][BGG_AUTEURS_TXT][j]);

            //Exécute la requête
            if (!Requete.exec())
            {
                qDebug() << Requete.lastQuery() ;
            }
            ActualiserCBx_Auteur();
            index=ui->CBx_Auteur->findData(Matrice[i][BGG_AUTEURS_ID][j],Qt::UserRole,Qt::MatchCaseSensitive);
        }
        ui->CBx_Auteur->setCurrentIndex(index);
        on_Bt_AjouterAuteur_clicked();
    }
    ui->CBx_Auteur->setCurrentIndex(0);

    int index=ui->CBx_Editeur->findData(Matrice[i][BGG_EDITEUR_ID][0],Qt::UserRole,Qt::MatchCaseSensitive);

    // Si l'editeur est introuvable, on l'ajoute
    if(index==-1)
    {
        Requete.prepare("INSERT INTO fournisseursediteur(IdFournisseur,NomFournisseur,Editeur) VALUES (:IdFournisseur,:NomFournisseur,1)");
        Requete.bindValue(":IdFournisseur",Matrice[i][BGG_EDITEUR_ID][0]);
        Requete.bindValue(":NomFournisseur",Matrice[i][BGG_EDITEUR_TXT][0]);

        //Exécute la requête
        if (!Requete.exec())
        {
            qDebug() << getLastExecutedQuery(Requete)<<Requete.lastError();
        }
        ActualiserCBx_Editeur();
        index=ui->CBx_Editeur->findData(Matrice[i][BGG_EDITEUR_ID][0],Qt::UserRole,Qt::MatchCaseSensitive);
    }
    ui->CBx_Editeur->setCurrentIndex(index);

    ui->SBx_AgeMin->setValue(Matrice[i][BGG_MINAGE][0].toInt());
    ui->SBx_JoueursMin->setValue(Matrice[i][BGG_MINJOUEURS][0].toInt());
    ui->SBx_JoueursMax->setValue(Matrice[i][BGG_MAXJOUEURS][0].toInt());
    ui->SBx_DureeMin->setValue(Matrice[i][BGG_MINDUREE][0].toInt());
    ui->SBx_DureeMax->setValue(Matrice[i][BGG_MAXDUREE][0].toInt());
    qDebug()<<Matrice[i][BGG_NOTE][0];
    ui->SBx_Note->setValue(Matrice[i][BGG_NOTE][0].toDouble());
    QTextDocument text;
    text.setHtml(Matrice[i][BGG_DESCRIPTION][0]);
    // Si le texte est déjà rempli, on ne l'écrase pas
    if(ui->TxE_Description->toPlainText().trimmed()!="")
    {
        return;
    }
    this->Requete=new Http_xml_api();
    this->Requete->LancerTelechargement("http://translate.google.fr/translate_a/single?client=gtx&sl=en&tl=fr&dt=t&q="+text.toPlainText(),
                                        MODE_GT_JSON);
    connect(this->Requete,SIGNAL(SignalTelechargementFiniGT(QString)),this,SLOT(slot_Traduction_finie(QString)));

}

void F_AjoutSuppModifJeux::slot_Traduction_finie(QString TexteTraduit)
{
    ui->TxE_Description->setText(TexteTraduit);
}

void F_AjoutSuppModifJeux::on_Bt_AjouterMotCle_clicked()
{
    QListWidgetItem *Item = new QListWidgetItem(ui->CBx_MotCle->currentText());
    // Ajoute son ID dans la partie DATA
    Item->setData(Qt::UserRole,ui->CBx_MotCle->currentData());
    ui->Lw_MotsCles->insertItem(ui->Lw_MotsCles->count()-1,Item);
}

void F_AjoutSuppModifJeux::on_Bt_SupprimerMotCle_clicked()
{
    delete ui->Lw_MotsCles->takeItem(ui->Lw_MotsCles->currentRow());
    ui->Bt_SupprimerMotCle->setEnabled(false);
}

void F_AjoutSuppModifJeux::on_Lw_MotsCles_clicked(const QModelIndex &index)
{
    ui->Bt_SupprimerMotCle->setEnabled(true);
}

void F_AjoutSuppModifJeux::on_Bt_AjouterAuteur_clicked()
{
    QListWidgetItem *Item = new QListWidgetItem(ui->CBx_Auteur->currentText());
    // Ajoute son ID dans la partie DATA
    Item->setData(Qt::UserRole,ui->CBx_Auteur->currentData());
    ui->Lw_Auteurs->insertItem(ui->Lw_Auteurs->count()-1,Item);
}

void F_AjoutSuppModifJeux::on_Bt_SupprimerAuteur_clicked()
{
    delete ui->Lw_Auteurs->takeItem(ui->Lw_Auteurs->currentRow());
    ui->Bt_SupprimerAuteur->setEnabled(false);
}

void F_AjoutSuppModifJeux::on_Lw_Auteurs_clicked(const QModelIndex &index)
{
    ui->Bt_SupprimerAuteur->setEnabled(true);
}

void F_AjoutSuppModifJeux::on_CBx_Auteur_activated(int index)
{
    //Si l'élément séléctionné est le dernier : ajouter un mot clé
    if(index == ui->CBx_Auteur->count() -1)
    {
        this->pAuteurAjMod->Ajouter(POPUP_AUTEUR);
    }
}
