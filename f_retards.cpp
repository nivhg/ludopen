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
 *L'utilisateur peut envoyer un mail à  un membre s'il a n'a pas rendu le jeu à  la date prévu.
 *Il peut définir le mail type qui sera envoyé.
 *
 */
// En-tÃªte propre à  l'objet ----------------------------------------------------

#include "Courriel.h"
#include "f_retards.h"
#include "ui_f_retards.h"

// En-tÃªtes standards ----------------------------------------------------------
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
    QSqlQuery Requete ;

    ui->setupUi(this);

    //Mise à  jour de la liste
    this->MaJListe() ;

    this->nNombreEmailEnvoye = 0 ;

    ui->BP_Mail->setValue( 0 ) ;

    //Recupération de l'objet et du corps du mail
    if( Requete.exec( "SELECT SujetEmail, CorpsEmail FROM preferences WHERE IdPreferences=1") )
    {
        Requete.next() ;
        ui->LE_Sujet->setText( Requete.record().value( 0 ).toString() ) ;
        ui->TE_Corps->setPlainText( Requete.record().value( 1 ).toString() ) ;

        ui->PB_Enregistrer->setDisabled( true ) ;
        ui->PB_Annuler->setDisabled( true ) ;
    }
    else
    {
        qDebug() << " Erreur :F_Retards::on_PB_Annuler_clicked() : " << Requete.lastError().text() ;
    }
}

/**
 * @brief Permet de détruire les objets dynamique
 *
 */
F_Retards::~F_Retards()
{
    delete ui;
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
    QStandardItemModel * modele ;
    QStandardItem * item ;
    QString sListeJeux ;

    //Initialisation de l'objet permettant l'organisation du tableau
    modele = new QStandardItemModel() ;

    ui->TbW_Retards->setModel( modele ) ;
    ui->TbW_Retards->setEditTriggers( QAbstractItemView::SelectedClicked ) ;
    ui->TbW_Retards->setEditTriggers( 0 ) ;


    //Requete recuperant la liste des membres ayant des jeux en retards et des jeux correspondants
    if( RequeteMembre.exec( "SELECT IdMembre, Nom, Prenom, CodeMembre, Email FROM emprunts, membres "
                            "WHERE IdMembre=Membres_IdMembre AND DateRetourPrevu<CURRENT_DATE AND DateRetour IS NULL AND MailEnvoye!=1 GROUP BY IdMembre" ) )
    {
        /*Cration des caractristiques du tableau : -Nombre de colonnes
                                                   -Nom des colonnes
                                                   -Nombre de lignes*/

        //On vide le vecteur recuperant l'id des membres
        this->VecteurIdMembre.clear() ;

        //Mise en place du tableau
        modele->setColumnCount( 6 ) ;
        modele->setRowCount( RequeteMembre.size() ) ;
        modele->setHorizontalHeaderItem( 0, new QStandardItem( "" ) ) ;
        modele->setHorizontalHeaderItem( 1, new QStandardItem( "Nom" ) ) ;
        modele->setHorizontalHeaderItem( 2, new QStandardItem( "Prenom" ) ) ;
        modele->setHorizontalHeaderItem( 3, new QStandardItem( "Code" ) ) ;
        modele->setHorizontalHeaderItem( 4, new QStandardItem( "Mail" ) ) ;
        modele->setHorizontalHeaderItem( 5, new QStandardItem( "Jeux en retard" ) ) ;

        ui->TbW_Retards->setColumnWidth( 0, 22 ) ;
        ui->TbW_Retards->setColumnWidth( 3, 55 ) ;
        ui->TbW_Retards->setColumnWidth( 4, 200 ) ;
        ui->TbW_Retards->setColumnWidth( 5, 400 ) ;


        //Remplissage du tableau avec les informations de la table emprunts, jeux et membres
        while( RequeteMembre.next() )
        {
            RequeteJeux.prepare( "SELECT  DateRetourPrevu, NomJeu FROM emprunts, jeux "
                                 "WHERE Membres_IdMembre=:IdMembre AND IdJeux=Jeux_IdJeux AND DateRetourPrevu<CURRENT_DATE AND DateRetour IS NULL " ) ;
            RequeteJeux.bindValue( ":IdMembre", RequeteMembre.record().value(0).toInt() ) ;

            this->VecteurIdMembre.append( RequeteMembre.record().value(0).toInt() );

            //On recupere la liste des jeux d'un membre pour les mettre dans la mÃªme case du tableau
            if( RequeteJeux.exec() )
            {
                sListeJeux.clear() ;
                RequeteJeux.next() ;
                sListeJeux = RequeteJeux.record().value( 1 ).toString() ;
                while( RequeteJeux.next() )
                {
                    sListeJeux += ", " + RequeteJeux.record().value( 1 ).toString() ;
                }
            }
            else
            {
                qDebug() << "F_Retards::MaJListe() : RequeteJeux :" << RequeteJeux.lastError().text() ;
            }

            //Permet d'ajout un checkbox à  la 1ére case de la ligne
            item = new QStandardItem() ;
            item->setCheckable( true ) ;
            item->setCheckState( Qt::Unchecked);
            modele->setItem( i, 0, item ) ;

            //On remplie le tableau
            modele->setItem( i, 1, new QStandardItem( RequeteMembre.record().value( 1 ).toString() ) ) ;
            modele->setItem( i, 2, new QStandardItem( RequeteMembre.record().value( 2 ).toString() ) ) ;
            modele->setItem( i, 3, new QStandardItem( RequeteMembre.record().value( 3 ).toString() ) ) ;
            modele->setItem( i, 4, new QStandardItem( RequeteMembre.record().value( 4 ).toString() ) ) ;
            modele->setItem( i, 5, new QStandardItem( sListeJeux ) ) ;
            i++ ;
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug() << "F_Retards::MaJListe() : RequeteMembre :" << RequeteMembre.lastError().text() ;
    }
}

/**
 * @brief Fait le lien entre l'id d'un membre et l'addresse de l'objet de type Courriel.
 *
 * @param pCourriel
 * @return int
 */
int F_Retards::RetrouverIdMembre(Courriel * pCourriel)
{
    bool bBoucle ( true ) ;
    int nBoucle ( 0 ) ;
    int nRetourne ( 0 ) ;

    //Recherche quand l'adresse de l'objet correspond à  une case du vecteur
    while( this->VecteurIdMembreCourriel.size() > nBoucle && bBoucle == true )
    {
        if( this->VecteurIdMembreCourriel.at( nBoucle ).pCourriel == pCourriel )
        {
            nRetourne = this->VecteurIdMembreCourriel.at( nBoucle ).IdMembre ;
            bBoucle = false ;
        }
        nBoucle ++ ;
    }

    //renvoie l'id correspondant à  la case du vecteur
    return nRetourne ;
}


/**
 * @brief Permet de supprimer un objet dynamique de type Courriel une fois qu'il fait son travail
 *
 * @param pCourriel
 */
void F_Retards::on_FermerThreadCourriel( Courriel * pCourriel )
{
    //On vérifie que le pointeur n'est pas null puis on supprime l'objet.
    if( pCourriel != NULL)
    {
        delete pCourriel ;
    }
    else
    {
        qDebug() << "Erreur : F_Retards::on_FermerThreadCourriel : Pointeur retourné est NULL" ;
    }

    //On deverrouille la liste et ses boutons si tous les objets on été détruit
    this->nNombreEmailEnCourDEnvoie++ ;

    ui->BP_Mail->setValue(  this->nNombreEmailEnCourDEnvoie / this->nNombreEmailEnvoye *100 ) ;
    if( this->nNombreEmailEnCourDEnvoie == this->nNombreEmailEnvoye )
    {
        ui->TbW_Retards->setEnabled( true ) ;
        ui->PB_Envoyer->setEnabled( true ) ;
        ui->PB_Selectionner->setEnabled( true ) ;
        ui->PB_Deselectionner->setEnabled( true ) ;
    }
}

/**
 * @brief Permet de supprimer un objet dynamique de type Courriel une fois que le mail est envoyé et met à  jour la base données pour dire que le mail a bien été envoyé
 *
 * @param pCourriel
 */
void F_Retards::on_ConfirmerMailEnvoyer( Courriel * pCourriel )
{
    //On vérifie que le pointeur n'est pas null puis on supprime l'objet et on met à  jour la base de donnée pour dire que le mail a bien été envoyé
    if( pCourriel != NULL)
    {
        QSqlQuery RequeteMail ;

        RequeteMail.prepare("UPDATE emprunts SET MailEnvoye=true "
                            "WHERE Membres_IdMembre=:IdMembre AND DateRetourPrevu<CURRENT_DATE AND DateRetour IS NULL ") ;
        RequeteMail.bindValue( ":IdMembre", this->RetrouverIdMembre( pCourriel ) ) ;

        if ( !RequeteMail.exec() )
        {
            qDebug() << "Erreur RequeteMail : F_Retards::on_PB_Enregistrer_clicked() : " << RequeteMail.lastError().text() ;
        }


        delete pCourriel ;
    }
    else
    {
        qDebug() << "Erreur : F_Retards::on_ConfirmerMailEnvoyer : Pointeur retourné est NULL" ;
    }


    this->MaJListe() ;
    //On deverrouille la liste et ses boutons si tous les objets on été détruit
    this->nNombreEmailEnCourDEnvoie++ ;
    ui->BP_Mail->setValue(  this->nNombreEmailEnCourDEnvoie / this->nNombreEmailEnvoye *100 ) ;

    if( this->nNombreEmailEnCourDEnvoie == this->nNombreEmailEnvoye )
    {
        ui->TbW_Retards->setEnabled( true ) ;
        ui->PB_Envoyer->setEnabled( true ) ;
        ui->PB_Selectionner->setEnabled( true ) ;
        ui->PB_Deselectionner->setEnabled( true ) ;
    }
}

/**
 * @brief Afficher les erreurs qu'il peut y avoir lors de l'envoie d'un mail
 *
 * @param sErreur
 */
void F_Retards::on_AfficherErreur( QString sErreur )
{
    //Affiche l'erreur en paramètre dans le label prévu à  cette effet
    ui->Lb_Erreur->setText( sErreur ) ;
}

/**
 * @brief Permet de selectionner toutes les lignes de la liste
 *
 */
void F_Retards::on_PB_Selectionner_clicked()
{
    //Permet mettre toute les checkbox en état "Check"
    for (register int i (0) ; i < ui->TbW_Retards->model()->rowCount() ; i++ )
    {
        ui->TbW_Retards->model()->setData( ui->TbW_Retards->model()->index(i ,0), Qt::Checked, Qt::CheckStateRole) ;
    }
}

/**
 * @brief Permet l'envoie des mails vers les membres selectionner dans la liste
 *
 */
void F_Retards::on_PB_Envoyer_clicked()
{
    QSqlQuery RequeteConnexion ;
    Courriel * pCourriel ;
    IdMembreCourriel oIdMembreCourriel ;
    QString sSujetEmail ;
    QString sCorpsEmail ;

    bool bSelection ( false ) ;

    this->nNombreEmailEnvoye = 0 ;
    this->nNombreEmailEnCourDEnvoie = 0 ;

    ui->Lb_Erreur->clear() ;

    ui->BP_Mail->setValue( 0 ) ;

    // On recupere l'email de l'expediteur ,l'adresse du serveur SMPT, le port, le sujet du mail et le corps du mail dans la base de données
    if ( RequeteConnexion.exec( "SELECT Email, AdresseServeurSMTP, PortSMTP, SujetEmail, CorpsEmail FROM preferences WHERE IdPreferences=1" ) )
    {
        RequeteConnexion.next() ;
    }
    else
    {
        qDebug() << "F_Retards : on_PB_Enregistrer_clicked() : " << RequeteConnexion.lastError().text() ;
    }

    //On vide le vecteur fesant un lien entre les ids des membes et les adresses des objets
    this->VecteurIdMembreCourriel.clear() ;

    //On parcour la liste
    for (register int i (0) ; i < ui->TbW_Retards->model()->columnCount() ; i++ )
    {
        //Si le checkbox est "check" on prépare le mail et on l'envoie
        if(ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,0), Qt::CheckStateRole).toBool() )
        {
            bSelection = true ;
            //On met l'objet et le corps du mail recuperé dans la base de données dans des QString
            sSujetEmail = RequeteConnexion.record().value( 3 ).toString() ;
            sCorpsEmail = RequeteConnexion.record().value( 4 ).toString() ;

            //On remplace les sigles par les données définies.
            sSujetEmail = sSujetEmail.replace("%nom", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,1 ) ).toString()) ;
            sCorpsEmail = sCorpsEmail.replace("%nom", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,1 ) ).toString()) ;

            sSujetEmail = sSujetEmail.replace("%prenom", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,2 ) ).toString()) ;
            sCorpsEmail = sCorpsEmail.replace("%prenom", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,2 ) ).toString()) ;

            sSujetEmail = sSujetEmail.replace("%jeux", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,5 ) ).toString()) ;
            sCorpsEmail = sCorpsEmail.replace("%jeux", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,5 ) ).toString()) ;

            sSujetEmail = sSujetEmail.replace("%destinataire", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,4 ) ).toString()) ;
            sCorpsEmail = sCorpsEmail.replace("%destinataire", ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,4 ) ).toString()) ;

            sSujetEmail = sSujetEmail.replace("%expediteur", RequeteConnexion.record().value(0).toString() ) ;
            sCorpsEmail = sCorpsEmail.replace("%expediteur", RequeteConnexion.record().value(0).toString() ) ;

            sSujetEmail = sSujetEmail.replace("%date", QDate::currentDate().toString( "dd/MM/yyyy" ) ) ;
            sCorpsEmail = sCorpsEmail.replace("%date", QDate::currentDate().toString( "dd/MM/yyyy" ) ) ;

            //On créer l'objet Courriel qui permettra l'envoie du mail
            pCourriel = new Courriel( RequeteConnexion.record().value(1).toString(), RequeteConnexion.record().value(2).toInt() , RequeteConnexion.record().value(0).toString() ,ui->TbW_Retards->model()->data( ui->TbW_Retards->model()->index(i ,4)).toString() , sSujetEmail, sCorpsEmail ) ;

            //Connect
            connect( pCourriel, SIGNAL( SignalFermerThread( Courriel * ) ), this, SLOT( on_FermerThreadCourriel( Courriel * ) ) ) ;
            connect( pCourriel, SIGNAL( SignalMailEnvoyer( Courriel * ) ), this, SLOT( on_ConfirmerMailEnvoyer( Courriel * ) ) ) ;
            connect( pCourriel, SIGNAL( SignalErreur( QString ) ), this, SLOT( on_AfficherErreur( QString ) ) ) ;

            //On remplie les vecteur avec l'id du membre et l'adresse de l'objet
            oIdMembreCourriel.IdMembre = this->VecteurIdMembre.at( i ) ;
            oIdMembreCourriel.pCourriel = pCourriel ;
            this->VecteurIdMembreCourriel.append( oIdMembreCourriel ) ;

            //On l'incrémente pour connaitre le nombre de mail envoyé
            this->nNombreEmailEnvoye++ ;
        }
    }
    //On met à  jour la liste
    if ( bSelection == true )
    {
        this->MaJListe() ;
        ui->TbW_Retards->setDisabled( true ) ;
        ui->PB_Envoyer->setDisabled( true ) ;
        ui->PB_Selectionner->setDisabled( true ) ;
        ui->PB_Deselectionner->setDisabled( true ) ;
    }

}

/**
 * @brief Permet de deselectionner toutes les lignes de la liste
 *
 */
void F_Retards::on_PB_Deselectionner_clicked()
{
    //Parcour la liste pour "decheck" toute toutes les checkbox
    for (register int i (0) ; i < ui->TbW_Retards->model()->rowCount() ; i++ )
    {
        ui->TbW_Retards->model()->setData( ui->TbW_Retards->model()->index(i ,0), Qt::Unchecked, Qt::CheckStateRole) ;
    }
}

/**
 * @brief Permet d'enregistrer le mail type
 *
 */
void F_Retards::on_PB_Enregistrer_clicked()
{
    QSqlQuery Requete ;

    Requete.prepare( "UPDATE preferences SET SujetEmail=:Sujet, CorpsEmail=:Corps WHERE IdPreferences=1" ) ;
    Requete.bindValue( ":Sujet", ui->LE_Sujet->text() ) ;
    Requete.bindValue( ":Corps", ui->TE_Corps->toPlainText() ) ;


    if( Requete.exec() )
    {
        ui->PB_Enregistrer->setDisabled( true ) ;
        ui->PB_Annuler->setDisabled( true ) ;
    }
    else
    {
        qDebug() << " Erreur : F_Retards::on_PB_Enregistrer_clicked() : " << Requete.lastError().text() ;
    }
}

/**
 * @brief Permet d'annuler les edites sur le mail type
 *
 */
void F_Retards::on_PB_Annuler_clicked()
{
    QSqlQuery Requete ;

    if( Requete.exec( "SELECT SujetEmail, CorpsEmail FROM preferences WHERE IdPreferences=1") )
    {
        Requete.next() ;
        ui->LE_Sujet->setText( Requete.record().value( 0 ).toString() ) ;
        ui->TE_Corps->setPlainText( Requete.record().value( 1 ).toString() ) ;

        ui->PB_Enregistrer->setDisabled( true ) ;
        ui->PB_Annuler->setDisabled( true ) ;
    }
    else
    {
        qDebug() << " Erreur :F_Retards::on_PB_Annuler_clicked() : " << Requete.lastError().text() ;
    }
}

/**
 * @brief Déverrouille les boutons enregistrer et annuler quand le PlainTextEdit est edité
 *
 */
void F_Retards::on_TE_Corps_textChanged()
{
    ui->PB_Enregistrer->setEnabled( true ) ;
    ui->PB_Annuler->setEnabled( true ) ;
}

/**
 * @brief Déverrouille les boutons enregistrer et annuler quand le LineEdit est edité
 *
 * @param arg1
 */
void F_Retards::on_LE_Sujet_textChanged(const QString &arg1)
{
    ui->PB_Enregistrer->setEnabled( true ) ;
    ui->PB_Annuler->setEnabled( true ) ;
}
