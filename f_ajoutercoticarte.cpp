//------------------------------------------------------------------------------
/** @file         f_ajoutercoticarte.cpp
 *  @brief        Permet de modfier ou d'ajouter  un abonnement à  un membre
 *
 *  @author       PADIOU Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT ( FRANCE )
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         28/06/2012
 */
//------------------------------------------------------------------------------
// En-tte propre  l'objet ----------------------------------------------------
#include "f_ajoutercoticarte.h"
#include "ui_f_ajoutercoticarte.h"

// En-ttes standards necessaires dans ce fichier en-tete seulement ------------
#include <QtGui>
#include <QtSql>
#include <iostream>

using namespace std;


/** Initialisation de la classe. Permet de remplir les combobox avec les abonnements.
 *  @test   Voir la procédure dans le fichier associé.
 */
F_AjouterCotiCarte::F_AjouterCotiCarte(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_AjouterCotiCarte)
{
    ui->setupUi(this);

    this->MaJListeAbonnements() ;
}

/** Detruit l'objet ui
 *  @test   Voir la procédure dans le fichier associe.
 */
F_AjouterCotiCarte::~F_AjouterCotiCarte()
{
    delete ui;
}

/** Permet de modifier un abonnement
 *  @param   int nIDAbonnement
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_AjouterCotiCarte::ModifierMembre( int nIDAbonnement )
{
    QSqlQuery RequeteAbonnement ;
    QSqlQuery RequeteCarte ;
    QSqlQuery RequetePrestation ;
    QString sNombre ;


    this->nIDAbonnement = nIDAbonnement;
    this->bNouvelAbo = false;

    ui->LE_Nom->setReadOnly( true ) ;
    ui->LE_Duree->setReadOnly( true ) ;
    ui->LE_Prix->setReadOnly( true ) ;
    ui->LE_Credit->setReadOnly( true ) ;
    ui->CBx_Abo->setDisabled( true ) ;
    ui->DE_Depart->setEnabled( false ) ;

    ui->label_modif1->setVisible( true ) ;
    ui->label_modif2->setVisible( true ) ;
    ui->DtE_Expiration->setVisible( true ) ;
    ui->LE_Restant->setVisible( true ) ;

    this->show();

    //Vérification carte ou préstation
    RequeteAbonnement.prepare("SELECT Prestations_IdPrestation, CartesPrepayees_IdCarte, DateExpiration, CreditRestant FROM abonnements WHERE IdAbonnements=:IdAbonnement");
    RequeteAbonnement.bindValue(":IdAbonnemnts", this->nIDAbonnement);
    if( RequeteAbonnement.exec() )
    {
        RequeteAbonnement.next();
    }
    else
    {
        cerr << "F_AjouterCotiCarte::ModifierMembre : Vérification carte ou prestation : " <<RequeteAbonnement.lastError().text().toStdString() << endl ;
    }

    if(RequeteAbonnement.record().value( 1 ).toInt() != 0 )
    {
        RequeteCarte.prepare("SELECT NomCarte, DureeValidite, Prix, CreditDisponible FROM cartesprepayees WHERE IdCarte=:IdCarte");
        RequeteCarte.bindValue(":IdCarte", RequeteAbonnement.record().value( 1 ).toInt());
        if( RequeteCarte.exec() )
        {
            RequeteCarte.next();

            ui->LE_Credit->setEnabled(true);
            ui->LE_Restant->setEnabled(true);

            ui->LE_Nom->setText( RequeteCarte.record().value( 0 ).toString() ) ;
            ui->LE_Nom->setCursorPosition( 0 ) ;

            sNombre.setNum( RequeteCarte.record().value( 1 ).toInt() ) ;
            ui->LE_Duree->setText( sNombre ) ;

            sNombre.setNum( RequeteCarte.record().value( 2 ).toInt() ) ;
            ui->LE_Prix->setText( sNombre ) ;

            sNombre.setNum( RequeteCarte.record().value( 3 ).toInt() ) ;
            ui->LE_Credit->setText( sNombre ) ;


            sNombre.setNum( RequeteAbonnement.record().value( 3 ).toInt() ) ;
            ui->LE_Restant->setText( sNombre ) ;

            ui->DtE_Expiration->setDate( RequeteAbonnement.record().value( 2 ).toDate() ) ;
        }
        else
        {
            cerr << "F_AjouterCotiCarte::ModifierMembre : Carte : " << RequeteAbonnement.lastError().text().toStdString() << endl ;
        }
    }
    else
    {
        RequetePrestation.prepare("SELECT NomPrestation, DureeValidite, Prix FROM prestations WHERE IdPrestation=:IdPrestation");
        RequetePrestation.bindValue(":IdPrestation", RequeteAbonnement.record().value( 0 ).toInt());

        if( RequetePrestation.exec() )
        {
            RequetePrestation.next();
            ui->LE_Credit->setEnabled(false);
            ui->LE_Restant->setEnabled(false);


            ui->LE_Nom->setText( RequetePrestation.record().value( 0 ).toString() ) ;
            ui->LE_Nom->setCursorPosition( 0 ) ;

            sNombre.setNum( RequetePrestation.record().value( 1 ).toInt() ) ;
            ui->LE_Duree->setText( sNombre ) ;

            sNombre.setNum( RequetePrestation.record().value( 2 ).toInt() ) ;
            ui->LE_Prix->setText( sNombre ) ;

            sNombre.setNum( RequetePrestation.record().value( 3 ).toInt() ) ;
            ui->LE_Credit->setText( sNombre ) ;


            ui->DtE_Expiration->setDate( RequeteAbonnement.record().value( 2 ).toDate() ) ;
        }
        else
        {
            cerr << "F_AjouterCotiCarte::ModifierMembre : préstation : " << RequeteAbonnement.lastError().text().toStdString() << endl ;
        }
    }
}

/** Permet d'ajouter un abonnement à  un membre
 *  @param  int nIDMembre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_AjouterCotiCarte::AjouterMembre(int nIDMembre)
{
    this->nIDMembre = nIDMembre;
    this->bNouvelAbo = true;

    this->show();

    ui->LE_Nom->setReadOnly( false ) ;
    ui->LE_Duree->setReadOnly( false ) ;
    ui->LE_Prix->setReadOnly( false ) ;
    ui->LE_Credit->setReadOnly( false ) ;
    ui->CBx_Abo->setEnabled( true ) ;

    ui->DE_Depart->setDate( QDate::currentDate() ) ;
    ui->DE_Depart->setEnabled( true ) ;

    ui->label_modif1->hide();
    ui->label_modif2->hide();
    ui->DtE_Expiration->hide();
    ui->LE_Restant->hide();
}

void F_AjouterCotiCarte::MaJListeAbonnements()
{
    QSqlQuery query;
    QString Abo;

    ui->CBx_Abo->clear() ;
    query.exec("SELECT NomCarte FROM cartesprepayees");
    while(query.next())
    {
        Abo = (query.value(0).toString());
        ui->CBx_Abo->addItem(Abo);
    }

    query.exec("SELECT NomPrestation FROM prestations");
    while(query.next())
    {
        Abo = (query.value(0).toString());
        ui->CBx_Abo->addItem(Abo);
    }
}

/** Permet de valider l'ajout ou la modification d'un abonnement
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_AjouterCotiCarte::on_Bt_Valider_clicked()
{
    QSqlQuery query;
    int nIdPrestation (0) ;
    int nIdCarte (0) ;
    QDate DateActuelle ;

    DateActuelle = QDate::currentDate() ;


    if (this->bNouvelAbo == true)
    {
        if( ui->LE_Credit->isEnabled() )
        {
            if( !query.exec( "SELECT IdCarte FROM cartesprepayees WHERE NomCarte='" + ui->CBx_Abo->currentText() + "'"  ) )
            {
                cerr << "F_AjouterCotiCarte::on_Bt_Valider_clicked() : selection Carte : " <<query.lastError().text().toStdString() << endl ;
            }
            else
            {
                query.next() ;
                nIdCarte = query.record().value( 0 ).toInt() ;
            }


            query.prepare("INSERT INTO abonnements (CartesPrepayees_IdCarte, Membres_IdMembre, DateSouscription, DateExpiration, CreditRestant) "
                          "VALUES ( :cartesprepayees_IdCarte, :Membres_IdMembre, :DateSouscription, :DateExpiration, :CreditRestant) ");
            query.bindValue(":CartesPrepayees_IdCarte", nIdCarte);
            query.bindValue(":Membres_IdMembre", this->nIDMembre);
            query.bindValue(":DateSouscription", ui->DE_Depart->date() ) ;
            DateActuelle = ui->DE_Depart->date() ;
            query.bindValue(":DateExpiration", DateActuelle.addDays( ui->LE_Duree->text().toInt() ) ) ;
            query.bindValue(":CreditRestant", ui->LE_Credit->text().toInt() ) ;
            if( !query.exec() )
            {
                cerr << "F_AjouterCotiCarte::on_Bt_Valider_clicked() : enregistrement carte : " << query.lastError().text().toStdString() << endl ;
            }            
        }
        else
        {
            if( !query.exec( "SELECT IdPrestation FROM prestations WHERE NomPrestation='" + ui->CBx_Abo->currentText() + "'"  ) )
            {
                cerr << "F_AjouterCotiCarte::on_Bt_Valider_clicked() : selection prestation : " <<query.lastError().text().toStdString() << endl ;
            }
            else
            {
                query.next() ;
                nIdPrestation = query.record().value( 0 ).toInt() ;
            }

            query.prepare("INSERT INTO abonnements (Prestations_IdPrestation, Membres_IdMembre, DateSouscription, DateExpiration ) "
                          "VALUES (:Prestations_IdPrestation, :Membres_IdMembre, :DateSouscription, :DateExpiration ) ");
            query.bindValue(":Prestations_IdPrestation", nIdPrestation);
            query.bindValue(":Membres_IdMembre", this->nIDMembre);
            query.bindValue(":DateSouscription", ui->DE_Depart->date() ) ;
            DateActuelle = ui->DE_Depart->date() ;
            query.bindValue(":DateExpiration", DateActuelle.addDays( ui->LE_Duree->text().toInt() ) ) ;
            if( !query.exec() )
            {
                cerr << "F_AjouterCotiCarte::on_Bt_Valider_clicked() : Prestation : " <<query.lastError().text().toStdString() << endl ;
            }
            cerr << nIdPrestation << endl ;
        } 
    }

    else
    {
        if ( ui->LE_Restant->isEnabled() == true )
        {
            query.prepare("UPDATE abonnements SET DateExpiration=:DateExpiration, CreditRestant=:CreditRestant "
                          "WHERE IdAbonnements=:IdAbonnements");
            query.bindValue(":IdAbonnements", this->nIDAbonnement);
            query.bindValue( ":DateExpiration", ui->DtE_Expiration->date() ) ;
            query.bindValue(":CreditRestant", ui->LE_Restant->text().toInt() ) ;
            if( !query.exec() )
            {
                cerr << "F_AjouterCotiCarte::on_Bt_Valider_clicked() : Modifier carte : " << query.lastError().text().toStdString() << endl ;
            }
        }
        else
        {
            query.prepare("UPDATE abonnements SET DateExpiration=:DateExpiration "
                          "WHERE IdAbonnements=:IdAbonnements");
            query.bindValue(":IdAbonnements", this->nIDAbonnement);
            query.bindValue( ":DateExpiration", ui->DtE_Expiration->date() ) ;
            if( !query.exec() )
            {
                cerr << "F_AjouterCotiCarte::on_Bt_Valider_clicked() : modifier prestation : " <<query.lastError().text().toStdString() << endl ;
            }

        }

    }

    emit(this->SignalAjoutCotisationCarte() ) ;

    this->setVisible(false);

    ui->LE_Nom->clear() ;
    ui->LE_Duree->clear() ;
    ui->LE_Prix->clear() ;
    ui->LE_Credit->clear() ;

    ui->CBx_Abo->setCurrentIndex( 0 ) ;
}

/** Permet l'affichage d'un abonnement
 *  @param  const QString &arg1
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_AjouterCotiCarte::on_CBx_Abo_currentIndexChanged(const QString &arg1)
{
    QSqlQuery Requete;

    this->sCombo = arg1;

    Requete.prepare("SELECT NomCarte, DureeValidite, Prix, CreditDisponible FROM cartesprepayees WHERE NomCarte=:NomCarte");
    Requete.bindValue(":NomCarte", this->sCombo);
    Requete.exec();
    Requete.next();
    if (Requete.isValid()!=true)
    {
        Requete.prepare("SELECT NomPrestation, DureeValidite, Prix FROM prestations WHERE NomPrestation=:NomPrestation");
        Requete.bindValue(":NomPrestation", this->sCombo);
        Requete.exec();
        Requete.next();
        if(Requete.isValid()==true)
        {
            ui->LE_Credit->setEnabled(false);
        }
    }
    else
    {
        ui->LE_Credit->setEnabled(true);
    }

    QString NomCarte = Requete.value(0).toString();
    QString Duree = Requete.value(1).toString();
    QString Prix = Requete.value(2).toString();
    QString Credit = Requete.value(3).toString();

    ui->LE_Nom->setText(NomCarte);
    ui->LE_Duree->setText(Duree);
    ui->LE_Prix->setText(Prix);
    ui->LE_Credit->setText(Credit);
}

/** Verrouille le bouton valider quand rien n'est selectionne dans le combobox
 *  @param int index
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_AjouterCotiCarte::on_CBx_Abo_currentIndexChanged(int index)
{
    /*
    if (index != 0)
    {
        ui->Bt_Valider->setEnabled(true);
    }
    else
    {
        ui->Bt_Valider->setEnabled(false);
    }
    */
}

/** Efface les champs et ferme la fenêtre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_AjouterCotiCarte::on_Bt_Annuler_clicked()
{
    this->hide();
    ui->LE_Nom->clear() ;
    ui->LE_Duree->clear() ;
    ui->LE_Prix->clear() ;
    ui->LE_Credit->clear() ;

    ui->CBx_Abo->setCurrentIndex( 0 ) ;
}
