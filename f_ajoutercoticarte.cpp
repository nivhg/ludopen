//------------------------------------------------------------------------------
/** @file         f_ajoutercoticarte.cpp
 *  @brief        Permet de modfier ou d'ajouter  un abonnement à  un membre
 *
 *  @author       PADIOU Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT ( FRANCE )
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         05/03/2013
 *  @author       William SOREL
 */
//------------------------------------------------------------------------------
#include "f_ajoutercoticarte.h"
#include "ui_f_ajoutercoticarte.h"

#include <QtWidgets>
#include <QtSql>
#include <QtDebug>

//======================================================================================================
/** Initialisation de la classe. Permet de remplir les combobox avec les abonnements.
  *
 */
F_AjouterCotiCarte::F_AjouterCotiCarte(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_AjouterCotiCarte)
{
    ui->setupUi(this);

    this->pPaiement=new F_Paiement;
    this->pPaiement->setWindowModality( Qt::ApplicationModal ) ;


    this->MaJListeAbonnements() ;
    // VV : Le displayformat n'est pas correct pourtant OK au niveau de Qt Designer
    ui->DtE_DateSouscription->setDisplayFormat("dd/MM/yyyy");

    //Remplir le ComboBox des modes de paiements
    QSqlQuery RequeteMode;
    RequeteMode.exec("SELECT NomPaiement,IdModePaiement FROM modepaiement ORDER BY IdModePaiement");
}
//======================================================================================================
/** Détruit l'objet ui
 */
F_AjouterCotiCarte::~F_AjouterCotiCarte()
{
    delete ui;
}
//======================================================================================================
/** Permet de modifier un abonnement
 *  @param   int nIDAbonnement
  *
 */
void F_AjouterCotiCarte::ModifierAbonnement( int nIDAbonnement )
{
    QSqlQuery RequeteAbonnement ;
    QSqlQuery RequeteCarte ;
    QSqlQuery RequetePrestation ;
    QString sNombre ;

    this->nIDAbonnement = nIDAbonnement;
    this->bNouvelAbo = false;

    ui->LE_Nom->setReadOnly( true ) ;
    ui->LE_DureeValidite->setReadOnly( true ) ;
    ui->LE_Prix->setReadOnly( true ) ;

    ui->DtE_DateSouscription->setEnabled( false ) ;

    ui->Lb_DateExpiration->show() ;
    ui->DtE_Expiration->show() ;

    ui->CBx_ChoixAbonnement->hide();

    ui->LE_CreditsDisponibles->setReadOnly( true ) ;
    ui->LE_CreditsRestants->setReadOnly( false ) ;

    ui->Bt_Valider->setText("Valider");

    //Vérification carte ou prestation
    RequeteAbonnement.prepare("SELECT Prestations_IdPrestation,CartesPrepayees_IdCarte,DateExpiration,CreditRestant,DateSouscription "
                              "FROM abonnements WHERE IdAbonnements=:IdAbonnement AND Supprimer=0");
    RequeteAbonnement.bindValue(":IdAbonnement", this->nIDAbonnement);
    if( RequeteAbonnement.exec() )
    {
        RequeteAbonnement.next();
    }
    else
    {
        qDebug()<< "F_AjouterCotiCarte::ModifierAbonnement : Vérification carte ou prestation : " <<RequeteAbonnement.lastQuery();
    }

    // Afficher la fenêtre d'ajout/Modif d'abonnement
    this->show();

    // Si c'est une carte pré-payées
    if(RequeteAbonnement.record().value( 1 ).toInt() != 0 )
    {
        // Adapter l'affichage pour une carte pré-payés
        ui->Lb_CreditsRestants->show();
        ui->LE_CreditsRestants->show();
        ui->Lb_CreditsDisponibles->show();
        ui->LE_CreditsDisponibles->show();
        this->setWindowTitle("Modifier cette carte pré-payée de ce membre...");

        RequeteCarte.prepare("SELECT NomCarte,DureeValidite,Prix,CreditDisponible FROM cartesprepayees WHERE IdCarte=:IdCarte");
        RequeteCarte.bindValue(":IdCarte", RequeteAbonnement.record().value( 1 ).toInt());
        if( RequeteCarte.exec() )
        {
            RequeteCarte.next();

            ui->LE_CreditsDisponibles->setEnabled(true);
            ui->LE_CreditsRestants->setEnabled(true);

            ui->LE_Nom->setText( RequeteCarte.record().value( 0 ).toString() ) ;
            ui->LE_Nom->setCursorPosition( 0 ) ;

            sNombre.setNum( RequeteCarte.record().value( 1 ).toInt() ) ;
            ui->LE_DureeValidite->setText( sNombre ) ;

            sNombre.setNum( RequeteCarte.record().value( 2 ).toInt() ) ;
            ui->LE_Prix->setText( sNombre ) ;

            sNombre.setNum( RequeteCarte.record().value( 3 ).toInt() ) ;
            ui->LE_CreditsDisponibles->setText( sNombre ) ;

            sNombre.setNum( RequeteAbonnement.record().value( 3 ).toInt() ) ;
            ui->LE_CreditsRestants->setText( sNombre ) ;

            ui->DtE_DateSouscription->setDate( RequeteAbonnement.record().value( 4 ).toDate() ) ;
            ui->DtE_Expiration->setDate( RequeteAbonnement.record().value( 2 ).toDate() ) ;
        }
        else
        {
            qDebug()<< "F_AjouterCotiCarte::ModifierAbonnement : Carte : " << RequeteCarte.lastQuery() << endl ;
        }
    }
    // Sinon c'est un abonnement
    else
    {
        // Adapter l'affichage pour une prestation
        ui->Lb_CreditsRestants->hide();
        ui->LE_CreditsRestants->hide();
        ui->Lb_CreditsDisponibles->hide();
        ui->LE_CreditsDisponibles->hide();
        this->setWindowTitle("Modifier l'adhésion de ce membre...");

        RequetePrestation.prepare("SELECT NomPrestation,DureeValidite,Prix FROM prestations WHERE IdPrestation=:IdPrestation");
        RequetePrestation.bindValue(":IdPrestation", RequeteAbonnement.record().value( 0 ).toInt());

        if( RequetePrestation.exec() )
        {
            RequetePrestation.next();
            ui->LE_CreditsDisponibles->setEnabled(false);
            ui->LE_CreditsRestants->setEnabled(false);
            ui->LE_CreditsDisponibles->clear();
            ui->LE_CreditsRestants->clear();

            ui->LE_Nom->setText( RequetePrestation.record().value( 0 ).toString() ) ;
            ui->LE_Nom->setCursorPosition( 0 ) ;

            sNombre.setNum( RequetePrestation.record().value( 1 ).toInt() ) ;
            ui->LE_DureeValidite->setText( sNombre ) ;

            sNombre.setNum( RequetePrestation.record().value( 2 ).toInt() ) ;
            ui->LE_Prix->setText( sNombre ) ;

            ui->DtE_DateSouscription->setDate( RequeteAbonnement.record().value( 4 ).toDate() ) ;
            ui->DtE_Expiration->setDate( RequeteAbonnement.record().value( 2 ).toDate() ) ;
        }
        else
        {
            qDebug()<< "F_AjouterCotiCarte::ModifierAbonnement : prestation : " << RequetePrestation.lastQuery() << endl ;
        }
    }
}
//======================================================================================================
/** Permet d'ajouter un abonnement à  un membre
 *  @param  int nIDMembre
  *
 */
void F_AjouterCotiCarte::AjouterAbonnement(int nIDMembre)
{
    this->nIDMembre = nIDMembre;
    this->bNouvelAbo = true;

    this->setWindowTitle("Choisir un abonnement pour ce membre...");

    ui->LE_Nom->hide() ;
    ui->Lb_NomPrestation->hide();

    // Prix et durée modifiable si on veut
    ui->LE_DureeValidite->setReadOnly( false ) ;
    ui->LE_Prix->setReadOnly( false ) ;

    ui->LE_CreditsDisponibles->setReadOnly( false ) ;
    ui->LE_CreditsDisponibles->show();
    ui->Lb_CreditsDisponibles->show();

    ui->CBx_ChoixAbonnement->setEnabled( true ) ;
    ui->CBx_ChoixAbonnement->show() ;

    ui->DtE_DateSouscription->setDate( QDate::currentDate() ) ;
    ui->DtE_DateSouscription->setEnabled( true ) ;

    // Cacher la date d'expiration
    ui->Lb_DateExpiration->hide();
    ui->DtE_Expiration->hide();

    // Pas de crédit restant puisque nouvelle carte
    ui->Lb_CreditsRestants->hide();
    ui->LE_CreditsRestants->hide();

    ui->Bt_Valider->setText("Paiement");

    this->show();
}
//======================================================================================================
void F_AjouterCotiCarte::MaJListeAbonnements()
{
   //qDebug()<< "F_AjouterCotiCarte::MaJListeAbonnements ";

    QSqlQuery Requete;

    ui->CBx_ChoixAbonnement->clear() ;
    if ( Requete.exec("SELECT NomCarte FROM cartesprepayees"))
    {
       while(Requete.next())
       {
           ui->CBx_ChoixAbonnement->addItem(Requete.value(0).toString());
           //qDebug()<< "F_AjouterCotiCarte::MaJListeAbonnements : Carte=" << Requete.value(0).toString();
       }
    }
    else
    {
       qDebug()<< "F_AjouterCotiCarte::MaJListeAbonnements : Requete Carte pré-payée : " << Requete.lastQuery() ;
    }

    if ( Requete.exec("SELECT NomPrestation FROM prestations") )
    {
       while(Requete.next())
       {
           ui->CBx_ChoixAbonnement->addItem(Requete.value(0).toString());
       }
    }
    else
    {
       qDebug()<< "F_AjouterCotiCarte::MaJListeAbonnements : Requete prestations: " << Requete.lastQuery() ;
    }
    //qDebug()<< "F_AjouterCotiCarte::MaJListeAbonnements : OK";
}
//======================================================================================================
/** Permet l'affichage d'un abonnement
 *  @param  const QString &arg1
  *
 */
void F_AjouterCotiCarte::on_CBx_ChoixAbonnement_currentIndexChanged(const QString &arg1)
{
    QSqlQuery Requete;

    this->sCombo = arg1;

    Requete.prepare("SELECT NomCarte,DureeValidite,Prix,CreditDisponible FROM cartesprepayees WHERE NomCarte=:NomCarte");
    Requete.bindValue(":NomCarte", this->sCombo);
    Requete.exec();
    Requete.next();
    if (Requete.isValid()!=true)
    {
        Requete.prepare("SELECT NomPrestation,DureeValidite,Prix FROM prestations WHERE NomPrestation=:NomPrestation");
        Requete.bindValue(":NomPrestation", this->sCombo);
        Requete.exec();
        Requete.next();
        if(Requete.isValid()==true)
        {
            ui->LE_CreditsDisponibles->setEnabled(false);
        }
    }
    else
    {
        ui->LE_CreditsDisponibles->setEnabled(true);
    }

    QString NomCarte = Requete.value(0).toString();
    QString Duree = Requete.value(1).toString();
    QString Prix = Requete.value(2).toString();
    QString Credit = Requete.value(3).toString();

    ui->LE_Nom->setText(NomCarte);
    ui->LE_DureeValidite->setText(Duree);
    ui->LE_Prix->setText(Prix);
    ui->LE_CreditsDisponibles->setText(Credit);
}

//======================================================================================================
/** Efface les champs et ferme la fenêtre
  *
 */
void F_AjouterCotiCarte::on_Bt_Annuler_clicked()
{
    this->hide();
    ui->LE_Nom->clear() ;
    ui->LE_DureeValidite->clear() ;
    ui->LE_Prix->clear() ;
    ui->LE_CreditsDisponibles->clear() ;

    ui->CBx_ChoixAbonnement->setCurrentIndex( 0 ) ;
}
//======================================================================================================
/** Permet de valider l'ajout ou la modification d'un abonnement
  *
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
        QSqlQuery RequeteMembre;
        RequeteMembre.prepare("SELECT CodeMembre FROM membres WHERE IdMembre=:IdMembre");
        RequeteMembre.bindValue(":IdMembre",this->nIDMembre);
        RequeteMembre.exec();
        RequeteMembre.next();
        QString CodeMembre=ObtenirValeurParNom(RequeteMembre,"CodeMembre").toString();

        pPaiement->setWindowModality(Qt::ApplicationModal);
        pPaiement->AfficherPaiement(QDateTime::currentDateTime(),CodeMembre,ui->LE_CreditsDisponibles->text().toInt(),
                                    VENTILATION_ABONNEMENT,"abonnements",NULL,NULL,false);

        // S'il ne procède pas au paiement, on sort de la fonction
        if(pPaiement->exec()==0)
        {
            return;
        }
        if( ui->LE_CreditsDisponibles->isEnabled() )
        {
            if( !query.exec( "SELECT IdCarte FROM cartesprepayees WHERE NomCarte='" + ui->CBx_ChoixAbonnement->currentText() + "'"  ) )
            {
                qDebug()<< "F_AjouterCotiCarte::on_Bt_Valider_clicked() : selection Carte : " <<query.lastQuery() << endl ;
            }
            else
            {
                query.next() ;
                nIdCarte = query.record().value( 0 ).toInt() ;
            }

            query.prepare("INSERT INTO abonnements (CartesPrepayees_IdCarte,Membres_IdMembre,DateSouscription,"
                          "DateExpiration,CreditRestant) "
                          "VALUES (:CartesPrepayees_IdCarte,:Membres_IdMembre,:DateSouscription,:DateExpiration,:CreditRestant)");
            query.bindValue(":CartesPrepayees_IdCarte", nIdCarte);
            query.bindValue(":Membres_IdMembre", this->nIDMembre);
            query.bindValue(":DateSouscription", ui->DtE_DateSouscription->date() ) ;
            DateActuelle = ui->DtE_DateSouscription->date() ;
            query.bindValue(":DateExpiration", DateActuelle.addDays( ui->LE_DureeValidite->text().toInt() ) ) ;
            query.bindValue(":CreditRestant", ui->LE_CreditsDisponibles->text().toInt() ) ;
            if( !query.exec() )
            {
                qDebug()<< "F_AjouterCotiCarte::on_Bt_Valider_clicked() : enregistrement carte : " << query.lastQuery() << endl ;
            }
        }
        else
        {
            if( !query.exec( "SELECT IdPrestation FROM prestations WHERE NomPrestation='" + ui->CBx_ChoixAbonnement->currentText() + "'"  ) )
            {
                qDebug()<< "F_AjouterCotiCarte::on_Bt_Valider_clicked() : selection prestation : " <<query.lastQuery();
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
            query.bindValue(":DateSouscription", ui->DtE_DateSouscription->date() ) ;
            DateActuelle = ui->DtE_DateSouscription->date() ;
            query.bindValue(":DateExpiration", DateActuelle.addDays( ui->LE_DureeValidite->text().toInt() ) ) ;
            if( !query.exec() )
            {
                qDebug()<< "F_AjouterCotiCarte::on_Bt_Valider_clicked() : Prestation : " <<query.lastQuery();
            }
        }
        QSqlQuery UpdateQuery;
        UpdateQuery.prepare("UPDATE paiements SET IdTable=:IdTable");
        UpdateQuery.bindValue(":IdTable",query.lastInsertId().toInt());
        if(!UpdateQuery.exec())
        {
            qDebug()<< "Mise à jour paiement" << getLastExecutedQuery(UpdateQuery) << UpdateQuery.lastError();
        }

    }
    else
    {
        if ( ui->LE_CreditsRestants->isEnabled() == true )
        {
            query.prepare("UPDATE abonnements SET DateExpiration=:DateExpiration,CreditRestant=:CreditRestant "
                          "WHERE IdAbonnements=:IdAbonnements");
            query.bindValue(":IdAbonnements", this->nIDAbonnement);
            query.bindValue(":DateExpiration", ui->DtE_Expiration->date() ) ;
            query.bindValue(":CreditRestant", ui->LE_CreditsRestants->text().toInt() ) ;
            if( !query.exec() )
            {
                qDebug()<< "F_AjouterCotiCarte::on_Bt_Valider_clicked() : Modifier carte : " << query.lastQuery() ;
            }
        }
        else
        {
            query.prepare("UPDATE abonnements SET DateExpiration=:DateExpiration,DateSouscription=:DateSouscription "
                          "WHERE IdAbonnements=:IdAbonnements");
            query.bindValue( ":IdAbonnements"    , this->nIDAbonnement);
            query.bindValue( ":DateExpiration"   , ui->DtE_Expiration->date() ) ;
            query.bindValue( ":DateSouscription" , ui->DtE_DateSouscription->date() ) ;
            if( !query.exec() )
            {
                qDebug()<< "F_AjouterCotiCarte::on_Bt_Valider_clicked() : modifier prestation : " <<query.lastQuery() << endl ;
            }
        }
    }

    emit(this->SignalAjoutCotisationCarte() ) ;

    this->setVisible(false);

    ui->LE_Nom->clear() ;
    ui->LE_DureeValidite->clear() ;
    ui->LE_Prix->clear() ;
    ui->LE_CreditsDisponibles->clear() ;

    ui->CBx_ChoixAbonnement->setCurrentIndex( 0 ) ;
}
//======================================================================================================
/** Permet de prolonger d'un an un abonnement de type cotisation
  *
 */
void F_AjouterCotiCarte::on_Bt_Prolonger_clicked()
{
    ui->DtE_DateSouscription->setDate( QDate::currentDate() ) ;
    ui->DtE_Expiration->setDate( QDate::currentDate().addYears(1) ) ;
}
