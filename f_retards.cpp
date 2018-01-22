//------------------------------------------------------------------------------
/** @file         F_Retards.cpp
 *  @brief        Permet de faire le lien entre l'id d'un membre et l'adresse d'un objet Courriel
 *
 *  @author       Padiou Nicolas
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        06/2012
 *  @version      0.1
 *  @date         13/06/2012
 *
 *  @bug          26/04/2014 <William> <Ajout de champ dans le tableau des retards>
 *
 *L'utilisateur peut envoyer un mail à  un membre s'il a n'a pas rendu le jeu à  la date prévu.
 *Il peut définir le mail type qui sera envoyé.
 *
 */
// En-tête propre à  l'objet ----------------------------------------------------

#include "Courriel.h"
#include "f_retards.h"
#include "ui_f_retards.h"
#include "fonctions_globale.h"
#include "f_preferences.h"

// En-têtes standards ----------------------------------------------------------
#include <QStandardItemModel>
#include <QtSql>


/**
 * @brief Permet d'initialiser les différents champs et tableau.
 *
 * @param parent
 */
F_Retards::F_Retards(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_Retards)
{
   ui->setupUi(this);

   //Mise à  jour de la liste
   this->MaJListe() ;

   this->NbEmailAEnvoyer = 0 ;
   this->pCourriel=0;

   // Permettre le tri des colonnes
   ui->TbW_Retards->setSortingEnabled(true);

   ui->TbW_Retards->setEditTriggers( QAbstractItemView::SelectedClicked ) ;

   // Pas d'édition possible dans les cases
   ui->TbW_Retards->setEditTriggers( 0 ) ;

   QStandardItemModel *Model = new QStandardItemModel(2, 1);

   // Liste des champs de la combobox d'export
   QString* sChamps = new QString[12] { "", "", "Nom", "Prénom", "Nb Email envoyé", "Jours de retard", "Amende",
           "Ville", "Email", "Tél. Fixe", "Tél. Mobile", "Jeux en retard" };
   // Ajout des checkboxes dans la combobox d'export
   int i=0;
   for (int r = 0; r < 12; ++r)
       {
           if(sChamps[r]!="")
           {
               QStandardItem* item;
               item = new QStandardItem(sChamps[r]);

               item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
               item->setData(Qt::Unchecked, Qt::CheckStateRole);
               item->setCheckState(Qt::Checked);

               Model->setItem(i++, 0, item);
               this->Items.push_back(item);
           }
           // Création des caractéristiques du tableau : Nombre de colonnes, Nom des colonnes
           // 0 IdMembre, 1 case à cocher, 2 Nom, 3 Prénom, 4 Nb Email envoyé, 5 Nb de Jours de retard, 6 Amende, 7 Ville, 8 Email, 9 Telephone, 10 Mobile, 11 liste des jeux en retard
           modeleTabRetard.setHorizontalHeaderItem(  r, new QStandardItem( (sChamps[r] ) )) ;
   }

   ui->CBx_Exporter->setModel(Model);
   // Nécessaire pour l'affichage des combobox sous Linux (Bug ?)
   ui->CBx_Exporter->setStyle(new QCommonStyle);
   ui->CBx_Exporter->setStyleSheet("border-style: outset;");
   ui->CBx_Exporter->setCurrentText("Champs à exporter");

   // associer le tableau et le modèle d'affichage
   ui->TbW_Retards->setModel( &modeleTabRetard ) ;

   // Cacher le message d'erreur par défaut
   ui->Lb_Erreur->setText("");

   //Recupération du corps de l'email et des paramètre de connection
    QString ServeurSMTP =  F_Preferences::ObtenirValeur("AdresseServeurSMTP");
    int PortSMTP = F_Preferences::ObtenirValeur("PortSMTP").toInt();
    if(ServeurSMTP=="" || PortSMTP==0)
    {
       QMessageBox::critical(this, "Erreur de paramétrage",
            "Serveur d'envoi de courriel (SMTP) mal paramétré.\n"
            "Vérifier le paramétrage dans les Préférences->Réseaux et relancer l'application.");
       ui->gridLayout_3->setEnabled(false);
       return;
    }
    QString Email=F_Preferences::ObtenirValeur("Email");
    if(Email.indexOf("@")==-1)
    {
       QMessageBox::critical(this, "Erreur de paramétrage",
            "Courriel d'envoi de courriel mal formé ou vide.\n"
            "Vérifier le paramétrage dans les Préférences->Informations et relancer l'application.");
       ui->gridLayout_3->setEnabled(false);
       return;
    }
    ui->LE_Sujet->setText( F_Preferences::ObtenirValeur("SujetEmail")) ;
    ui->TE_Corps->setPlainText( F_Preferences::ObtenirValeur("CorpsEmail")) ;

    ui->Bt_Enregistrer->setDisabled( true ) ;
    ui->Bt_Annuler->setDisabled( true ) ;

    //On crée l'objet Courriel qui permettra l'envoi des emails
    pCourriel = new Courriel( ServeurSMTP , PortSMTP , &this->ListeEMailAEnvoyer ) ;

    //Connecter les bons signaux pour être prévenu ici des étapes et problèmes d'envoi d'email
    connect( pCourriel, SIGNAL( Signal_Fermer_Thread_EMail( ) ), this, SLOT( slot_FermerThreadCourriel( ) ) ) ;
    connect( pCourriel, SIGNAL( SignalMailEnvoyer( uint ) ), this, SLOT( slot_ConfirmerMailEnvoyer( uint ) ) ) ;
    connect( pCourriel, SIGNAL( Signal_Erreur_EMail( QString ) ), this, SLOT( slot_AfficherErreurMail( QString ) ) ) ;
}

/**
 * @brief Permet de détruire les objets dynamique
 *
 */
F_Retards::~F_Retards()
{
   delete ui;
   if ( pCourriel)
   delete pCourriel ;
}

/**
 * @brief Fait une mise à  jour de la liste des membres qui ont au moins un jeu en retard
 *
 */
void F_Retards::MaJListe()
{
   int i ( 0 ) ;
   QSqlQuery RequeteMembre ;
   QSqlQuery RequeteJeux ;
   QSqlQuery RequeteNbJoursRetardToleres ;

   QStandardItem * item ;     // pour remplir le tableau
   QString sListeJeux ;
   QString RequeteAvecDate ;  //Le bind ne marche pas bien avec les dates qui doivent entre '...' => donc passer par une string

   QDate DateDeRetourPrevue ;
   QDate DateDeRetourToleree;
   DateDeRetourToleree=DateDeRetourToleree.currentDate();

  // Calculer la date de retour avec la tolérance du nombre de jours
  DateDeRetourToleree = DateDeRetourToleree.addDays( -(F_Preferences::ObtenirValeur("JourRetard").toInt()) ) ;
  //qDebug()<<"F_Retards::MaJListe()=> DateDeRetourToleree=" << DateDeRetourToleree << "Nb jour retard de la BDD=" << RequeteNbJoursRetardToleres.value(0).toInt();

   RequeteAvecDate="SELECT IdMembre,membres.Nom,Prenom,CodeMembre,membres.Email,membres.Ville,"
           "membres.Telephone,Mobile,NbrEMailsEnvoyes FROM emprunts,membres "
           "WHERE IdMembre=Membres_IdMembre AND DateRetourPrevu<='" +
           DateDeRetourToleree.toString("yyyy-MM-dd")+"' AND DateRetour IS NULL GROUP BY IdMembre" ;

   //Requete récupérant la liste des membres ayant des jeux en retards et des jeux correspondants
   if( !RequeteMembre.exec( RequeteAvecDate ) )
   {
      qDebug() << "F_Retards::MaJListe() : RequeteMembre :" << RequeteMembre.lastQuery() ;
   }
   else
   {
      //Création des caractéristiques du tableau : Nombre de lignes
      modeleTabRetard.setRowCount( RequeteMembre.size() ) ;

      //Remplissage du tableau avec les informations de la table emprunts, jeux et membres
      while( RequeteMembre.next() )
      {
         RequeteAvecDate= "SELECT DateRetourPrevu,NomJeu FROM emprunts,jeux WHERE Membres_IdMembre="+RequeteMembre.record().value(0).toString()
                         +" AND IdJeux=Jeux_IdJeux AND DateRetourPrevu<='"+DateDeRetourToleree.toString("yyyy-MM-dd")
                         +"' AND DateRetour IS NULL" ;

         //On récupère la liste des jeux d'un membre pour les mettre dans la même case du tableau
         if( !RequeteJeux.exec(RequeteAvecDate) )
         {
            qDebug() << "F_Retards::MaJListe() : RequeteJeux :" << RequeteJeux.lastQuery() ;
         }
         else
         {
             sListeJeux.clear() ;
             RequeteJeux.next() ;
             DateDeRetourPrevue = RequeteJeux.record().value( 0 ).toDate() ;
             sListeJeux = RequeteJeux.record().value( 1 ).toString() ;
             while( RequeteJeux.next() )
             {
                 sListeJeux += ", " + RequeteJeux.record().value( 1 ).toString() ;
             }
         }

         //Permet d'ajouter une checkbox à la ligne si un email est diponible pour le membre
         if ( ! RequeteMembre.record().value( 4 ).toString().isEmpty() )
         {
            item = new QStandardItem() ;
            item->setCheckable( true ) ;
            item->setCheckState( Qt::Unchecked);
            modeleTabRetard.setItem( i, 1, item ) ;
         }

         //On remplit le tableau. Ordre des colonnes :
         // 0 IdMembre, 1 case à cocher, 2 Nom, 3 Prénom, 4 Nb Email envoyé, 5 Nb de Jours de retard, 6 Amende, 7 Ville, 8 Email, 9 Telephone, 10 Mobile, 11 liste des jeux en retard
         // SELECT 0 IdMembre, 1 Nom, 2 Prenom, 3 CodeMembre, 4 Email, 5 Ville, 6 Telephone, 7 Mobile, 8 NbrEMailsEnvoyes
         modeleTabRetard.setItem( i, 0, new QStandardItem( RequeteMembre.record().value( 0 ).toString() ) ) ; // IdMembre
         modeleTabRetard.setItem( i, 2, new QStandardItem( RequeteMembre.record().value( 1 ).toString() ) ) ; // Nom
         modeleTabRetard.setItem( i, 3, new QStandardItem( RequeteMembre.record().value( 2 ).toString() ) ) ; // Prénom
         modeleTabRetard.setItem( i, 4, new QStandardItem( RequeteMembre.record().value( 8 ).toString() ) ) ; // Nb Emails envoyés
         // CALCULS du nombre de jours de retard
         qint64 nNbJoursDeRetard = DateDeRetourPrevue.daysTo( QDate::currentDate() ) ;
         QString sNbJoursDeRetard ;
         sNbJoursDeRetard.setNum(nNbJoursDeRetard) ;
         modeleTabRetard.setItem( i, 5, new QStandardItem( sNbJoursDeRetard ) ) ;   // Nb de jours de retard
         // TODO Calcul de l'amende
         float Amende = 0.0 ;
         //modeleTabRetard.setItem( i, 6, new QStandardItem( sNbJoursDeRetard ) ) ;   // Amende
         modeleTabRetard.setItem( i, 7, new QStandardItem( RequeteMembre.record().value( 5 ).toString() ) ) ; // Ville
         modeleTabRetard.setItem( i, 8, new QStandardItem( RequeteMembre.record().value( 4 ).toString() ) ) ; // Email
         modeleTabRetard.setItem( i, 9, new QStandardItem( RequeteMembre.record().value( 6 ).toString() ) ) ; // Tél. Fixe
         modeleTabRetard.setItem( i, 10, new QStandardItem( RequeteMembre.record().value( 7 ).toString() ) ) ; // Tél. Mobile
         modeleTabRetard.setItem( i, 11, new QStandardItem( sListeJeux ) ) ;       // Jeux en retard

         i++ ;
      }
      // Ajuster la largeur des colonnes au contenu
      // 0 IdMembre, 1 case à cocher, 2 Nom, 3 Prénom, 4 Nb Email envoyé, 5 Nb de Jours de retard, 6 Amende, 7 Ville, 8 Email, 9 Telephone, 10 Mobile, 11 liste des jeux en retard
      ui->TbW_Retards->setColumnWidth(  0,   0 ) ;  // IdMembre à cacher
      ui->TbW_Retards->setColumnWidth(  1,  22 ) ;  // case à cocher
      ui->TbW_Retards->resizeColumnToContents(2) ;  // Nom
      ui->TbW_Retards->resizeColumnToContents(3) ;  // Prenom
      //ui->TbW_Retards->setColumnWidth(  4,  22 ) ;  // Nb Email envoyé
      ui->TbW_Retards->resizeColumnToContents(4) ;  // Nb Email envoyé
      ui->TbW_Retards->resizeColumnToContents(5) ;  // Nb de Jours de retard
      ui->TbW_Retards->resizeColumnToContents(6) ;  // Amende
      ui->TbW_Retards->resizeColumnToContents(7) ;  // Ville
      ui->TbW_Retards->resizeColumnToContents(8) ;  // Email
      ui->TbW_Retards->resizeColumnToContents(9) ;  // Telephone
      ui->TbW_Retards->resizeColumnToContents(10) ; // Mobile
      ui->TbW_Retards->resizeColumnToContents(11);  // liste des jeux en retard

      // Affiche le nombre de membres ayant un retard
      ui->Lb_Resultat->setNum(this->modeleTabRetard.rowCount());
   }
}

/**
 * @brief Permet de supprimer un objet dynamique de type Courriel une fois qu'il fait son travail
 *
 * @param pCourriel
 */
void F_Retards::slot_FermerThreadCourriel( )
{   
   //On déverrouille la liste et ses boutons
   DesactiverChamps(false);

   //On vérifie que le thread est en route puis on arrête le thread d'envoi d'email.
   if( this->pCourriel->isRunning() )
   {
      this->pCourriel->terminate();
      this->pCourriel->wait();
   }
   else
   {
      qDebug() << "F_Retards::slot_FermerThreadCourriel : thread Courrier pas running" ;
   }
}

/**
 * @brief Permet de supprimer un objet dynamique de type Courriel une fois que le mail est envoyé et met à  jour la base données pour dire que le mail a bien été envoyé
 *
 * @param pCourriel
 */
void F_Retards::slot_ConfirmerMailEnvoyer( uint IDMembre  )
{
    //On vérifie que le pointeur n'est pas null puis on supprime l'objet et on met à  jour la base de données
    //pour dire que le mail a bien été envoyé
    if( pCourriel != NULL)
    {
    /*
        QSqlQuery RequeteMail ;

        RequeteMail.prepare("UPDATE emprunts SET MailEnvoye"
                            "WHERE Membres_IdMembre=:IdMembre AND DateRetourPrevu<CURRENT_DATE AND DateRetour IS NULL ") ;
        RequeteMail.bindValue( ":IdMembre", this->RetrouverIdMembre( pCourriel ) ) ;

        if ( !RequeteMail.exec() )
        {
            qDebug() << "Erreur RequeteMail : F_Retards::on_Bt_Enregistrer_clicked() : " << RequeteMail.lastQuery() ;
        }
    */
       this->NbEmailsDejaEnvoyes++ ;

       //On parcourt le tableau des retards et on vire la ligne correspondant de celui à qui a été envoyé un email
       for (register int i (0) ; i < ui->TbW_Retards->model()->rowCount() ; i++ )
       {
          //Si la checkbox est cochée, on enlève la ligne dans le tableau des retard pour cette personne qui a reçu un email
          if ( ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,1), Qt::CheckStateRole).toBool() )
          {
             if ( ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,0) ).toUInt() == IDMembre  )
             {
                ui->TbW_Retards->model()->removeRow(i) ;
             }
          }
       }
    }
    else
    {
       qDebug() << "Erreur : F_Retards::slot_ConfirmerMailEnvoyer : Pointeur retourné est NULL" ;
    }

    // Affiche le nombre de membres ayant un retard et devant recevoir un email
    ui->Lb_Resultat->setNum(this->modeleTabRetard.rowCount());
}

/**
 * @brief Afficher les erreurs qu'il peut y avoir lors de l'envoie d'un mail
 *
 * @param sErreur
 */
void F_Retards::slot_AfficherErreurMail( QString sErreur )
{
    //Affiche l'erreur en paramètre dans le label prévu à  cette effet
    ui->Lb_Erreur->setText( sErreur ) ;
}


/**
 * @brief Permet l'envoie des mails vers les membres selectionner dans la liste
 *
 */
void F_Retards::on_Bt_Envoyer_clicked()
{
    QSqlQuery RequeteConnexion ;
    EMail EMailProchainAEnvoyer ;
    QString sSujetEmail ;
    QString sCorpsEmail ;

    bool bSelection ( false ) ;

    // Remettre à zéro le nombre d'email à envoyer, sert d'index pour le vecteur ListeEMailAEnvoyer
    this->NbEmailAEnvoyer = 0 ;
    this->NbEmailsDejaEnvoyes = 0 ;

    // effacer les messages d'erreur précédent
    ui->Lb_Erreur->clear() ;

    //On vide le vecteur faisant un lien entre les ids des membres et les adresses des threads d'envoi d'email
    this->ListeEMailAEnvoyer.clear() ;


    //On parcourt la liste pour savoir quel membre a été coché
    for (register int i (0) ; i < ui->TbW_Retards->model()->rowCount() ; i++ )
    {
        //Si le checkbox est "check" on prépare le mail et on l'envoie
        if(ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,1), Qt::CheckStateRole).toBool() )
        {
            bSelection = true ;
            //On met l'objet et le corps du mail recuperé dans la base de données dans des QString
            sSujetEmail = F_Preferences::ObtenirValeur("SujetEmail") ;
            sCorpsEmail = F_Preferences::ObtenirValeur("CorpsEmail") ;

            //On remplace les sigles par les données définies.
            // 0 IdMembre, 1 case à cocher, 2 Nom, 3 Prénom, 4 Nb Email envoyé, 5 Nb de Jours de retard, 6 Amende, 7 Ville, 8 Email, 9 Telephone, 10 Mobile, 11 liste des jeux en retard
            sSujetEmail = sSujetEmail.replace("%nom", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,2 ) ).toString()) ;
            sCorpsEmail = sCorpsEmail.replace("%nom", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,2 ) ).toString()) ;

            sSujetEmail = sSujetEmail.replace("%prenom", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,3 ) ).toString()) ;
            sCorpsEmail = sCorpsEmail.replace("%prenom", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,3 ) ).toString()) ;

            sSujetEmail = sSujetEmail.replace("%jeux", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,11 ) ).toString()) ;
            sCorpsEmail = sCorpsEmail.replace("%jeux", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,11 ) ).toString()) ;

            sSujetEmail = sSujetEmail.replace("%destinataire", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,8 ) ).toString()) ;
            sCorpsEmail = sCorpsEmail.replace("%destinataire", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,8 ) ).toString()) ;

            sSujetEmail = sSujetEmail.replace("%expediteur", F_Preferences::ObtenirValeur("Email") ) ;
            sCorpsEmail = sCorpsEmail.replace("%expediteur", F_Preferences::ObtenirValeur("Email") ) ;

            sSujetEmail = sSujetEmail.replace("%date", QDate::currentDate().toString( "dd/MM/yyyy" ) ) ;
            sCorpsEmail = sCorpsEmail.replace("%date", QDate::currentDate().toString( "dd/MM/yyyy" ) ) ;

            //On remplie le vecteur des EMails avec les données de ce membre actuel
            EMailProchainAEnvoyer.sCorps = sCorpsEmail ;
            EMailProchainAEnvoyer.sSujet = sSujetEmail ;
            EMailProchainAEnvoyer.sFrom = F_Preferences::ObtenirValeur("Email");
            EMailProchainAEnvoyer.sTo = ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,8)).toString() ;
            EMailProchainAEnvoyer.IDMembre =  ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,0 )).toUInt() ;

            this->ListeEMailAEnvoyer.append( EMailProchainAEnvoyer ) ;

            //On l'incrémente pour connaitre le nombre d'emails envoyés
            this->NbEmailAEnvoyer++ ;            
        }
    }

    // s'il y a des emails à envoyer
    if ( NbEmailAEnvoyer > 0)
    {
        pCourriel->start();
    }

    //On met à jour la liste
    if ( bSelection )
    {
        //this->MaJListe() ;
        DesactiverChamps(true);
    }
}

/**
 * @brief Permet de désactiver les champs de l'onglet
 *
 */
void F_Retards::DesactiverChamps(bool Etat)
{
    ui->TbW_Retards->setDisabled( Etat ) ;
    ui->Bt_Envoyer->setDisabled( Etat ) ;
    ui->Bt_Selectionner->setDisabled( Etat ) ;
    ui->Bt_Deselectionner->setDisabled( Etat ) ;
}

/**
 * @brief Permet de sélectionner toutes les lignes de la liste
 *
 */
void F_Retards::on_Bt_Selectionner_clicked()
{
   //Permet de cocher toutes les checkbox
   for (register int i (0) ; i < ui->TbW_Retards->model()->rowCount() ; i++ )
   {
      //0 IdMembre, 1 case à cocher, 2 CodeMembre, 3 Nom, 4 Prenom, 5 Ville, 6 Email, 7 Telephone, 8 Mobile, 9 DatePrévue, 10 liste des jeux en retard
      //Permet d'ajouter une checkbox à la ligne si un email est diponible pour le membre
      if ( ! ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,8)).toString().isEmpty() )
       {
        ui->TbW_Retards->model()->setData( ui->TbW_Retards->model()->index(i ,1), Qt::Checked, Qt::CheckStateRole) ;
       }
   }
}

/**
 * @brief Permet de désélectionner toutes les lignes de la liste
 *
 */
void F_Retards::on_Bt_Deselectionner_clicked()
{
   //Parcourt la liste pour décocher toutes les checkbox
   for (register int i (0) ; i < ui->TbW_Retards->model()->rowCount() ; i++ )
   {
      //0 IdMembre, 1 case à cocher, 2 CodeMembre, 3 Nom, 4 Prenom, 5 Ville, 6 Email, 7 Telephone, 8 Mobile, 9 DatePrévue, 10 liste des jeux en retard
      //Permet d'enlever la checkbox à la ligne si un email est diponible pour le membre
      if ( ! ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,6)).toString().isEmpty() )
      {
         ui->TbW_Retards->model()->setData( ui->TbW_Retards->model()->index(i ,1), Qt::Unchecked, Qt::CheckStateRole) ;
      }
   }
}

/**
 * @brief Permet d'enregistrer le mail type
 *
 */
void F_Retards::on_Bt_Enregistrer_clicked()
{
    QSqlQuery Requete ;

    Requete.prepare( "UPDATE preferences SET Valeur=? WHERE NomChamps=?" ) ;
    QVariantList valeurs;
    valeurs << ui->LE_Sujet->text() << ui->TE_Corps->toPlainText();
    Requete.addBindValue(valeurs);

    QVariantList nomChamps;
    nomChamps << "SujetEmail" << "CorpsEmail";
    Requete.addBindValue(nomChamps);

    if( Requete.execBatch() )
    {
        ui->Bt_Enregistrer->setDisabled( true ) ;
        ui->Bt_Annuler->setDisabled( true ) ;
    }
    else
    {
        qDebug() << " Erreur : F_Retards::on_Bt_Enregistrer_clicked() : " << Requete.lastQuery()  << Requete.lastError();
    }
}

/**
 * @brief Permet d'annuler les edites sur le mail type
 *
 */
void F_Retards::on_Bt_Annuler_clicked()
{
    ui->LE_Sujet->setText( F_Preferences::ObtenirValeur("SujetEmail") ) ;
    ui->TE_Corps->setPlainText( F_Preferences::ObtenirValeur("CorpsEmail") ) ;

    ui->Bt_Enregistrer->setDisabled( true ) ;
    ui->Bt_Annuler->setDisabled( true ) ;
}

/**
 * @brief Déverrouille les boutons enregistrer et annuler quand le PlainTextEdit est edité
 *
 */
void F_Retards::on_TE_Corps_textChanged()
{
    ui->Bt_Enregistrer->setEnabled( true ) ;
    ui->Bt_Annuler->setEnabled( true ) ;
}

/**
 * @brief Déverrouille les boutons enregistrer et annuler quand le LineEdit est edité
 *
 * @param arg1
 */
void F_Retards::on_LE_Sujet_textChanged(const QString &arg1)
{
    ui->Bt_Enregistrer->setEnabled( true ) ;
    ui->Bt_Annuler->setEnabled( true ) ;
}

void F_Retards::on_TbW_Retards_doubleClicked(const QModelIndex &index)
{
    emit( this->Signal_DoubleClic_ListeMembres( ui->TbW_Retards->model()->data( this->modeleTabRetard.index( index.row() , 0 )).toUInt() )) ;
}

void F_Retards::on_Bt_Exporter_clicked()
{
    int nNombreColonne (0);
    int nNombreLigne (0);

    QString sCaractere;
    QString nomFichier = QFileDialog::getSaveFileName(this, "Enregistrer sous ...", "Sans titre 1.csv");
    QFile fichier(nomFichier);
    QTextStream ecrire (&fichier);
    fichier.open(QIODevice::WriteOnly);

    // Récupération des titres de colonnes et concaténation dans la variable titre si les checkboxes sont cochées
    QString titre;
    for (int i = 0; i < this->Items.size(); ++i) {
        QStandardItem * item = this->Items[i];
        if(item->checkState() == Qt::Checked)
        {
            if(titre.size()==0)
            {
                titre+=item->text();
            }
            else
            {
                titre+=";"+item->text();
            }
        }
    }

    ecrire << titre + "\r\n";

    for(nNombreLigne = 0; nNombreLigne<ui->TbW_Retards->model()->rowCount(); nNombreLigne++)
    {
        for(nNombreColonne = 2; nNombreColonne<ui->TbW_Retards->model()->columnCount(); nNombreColonne++)
        {
            QStandardItem * item = this->Items[nNombreColonne-2];
            if(item->checkState() == Qt::Checked)
            {
                titre=ui->TbW_Retards->model()->headerData(nNombreColonne,Qt::Horizontal,Qt::DisplayRole).toString();
                sCaractere = ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index( nNombreLigne, nNombreColonne ) ).toString() ;
                // On rejete les valeurs à caractère unique et on le remplace par un champs vide
                sCaractere.replace(" ", "\ ") ;
                if(sCaractere == "-" || sCaractere == "_" || sCaractere == ".")
                {
                    sCaractere = "";
                }
                ecrire << "\"" << sCaractere << "\";";
            }
        }
        ecrire << "\r\n";
        nNombreColonne = 0;
    }
}
