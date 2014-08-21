//------------------------------------------------------------------------------
/** @file       f_popupclesttem.cpp
 *  @brief      F_PopUpCLESTTEM permet d'avoir une pop-up en fonction de du int nCLESTTEM.
 *
 *  @author     NORMAND Julien
 *  @author     STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since      07/2012
 *  @version    1.0
 *  @date       01/08/2012
 *
 *  Le int nCLESSTEM prend pour valeur un nombre, le nombre fait modifier les labels de la fenêtre donc :
 *      -Si 0, il s'agit d'un Etat Jeu.
 *      -Si 1, il s'agit d'un Statut Jeu.
 *      -Si 2, il s'agit d'un Emplacement Jeu.
 *      -Si 3, il s'agit d'une Classification Jeu.
 *      -Si 4, il s'agit d'un Mot clé Jeu.
 *      -Si 5, il s'agit d'un Type Membre.
 *      -Si 6, il s'agit d'un Titre Membre.
 *      -Si 7, il s'agit d'un Type Emprunt.
 *      -Si 8, il s'agit d'un Lieu Ludothèque.
 *      -Si 9, il s'agit d'un Paiement Membre.
 *      -Si 10, il s'agit d'une activité d'un membre.
 *
 *  Fabrication   Qt Creator, projet    .pro
 *
 *  @todo       Classe presque terminé, manque la liaison avec le F_Mainwindow.
 *
 *  @bug        Aucun.
 */
//------------------------------------------------------------------------------

// En-têtes standards (ATTENTION : garder toujours le meme ordre) --------------
#include "f_popupclesttem.h"
#include "ui_f_popupclesttem.h"
//------------------------------------------------------------------------------

// En-tête propre à l'objet ----------------------------------------------------
#include <QtDebug>
using namespace std;
//------------------------------------------------------------------------------

/**
 *  @brief Constructeur de la classe.
 *
 *  @param nCLESTTEM
 *  @param parent
 */
F_PopUpCLESTTEM::F_PopUpCLESTTEM(int nCLESTTEM, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_PopUpCLESTTEM)
{
    ui->setupUi(this);
    this->nCLESTTEM = nCLESTTEM;
}



/**
 *  @brief Destructeur de la classe.
 *
 */
F_PopUpCLESTTEM::~F_PopUpCLESTTEM()
{
    delete ui;
}

/**
 *  @brief Ouvrir la fenêtre en mode "ajouter".
 *
 */
void F_PopUpCLESTTEM::Ajouter()
{
    this->bCLESTTEM = true;

    switch(this->nCLESTTEM)
    {
        case 0: // Etat Jeu.
            ui->Lb_TitreFenetre->setText("Création d'un Etat");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case 1: // Statut Jeu.
            ui->Lb_TitreFenetre->setText("Création d'un Statut");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case 2: // Emplacement Jeu.
            ui->Lb_TitreFenetre->setText("Création d'un Emplacement");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case 3: // Classification Jeu.
            ui->Lb_TitreFenetre->setText("Création d'une Classification");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Classification :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();
            // Connecte l'évenement textEdited à la fonction toUpper
            connect(ui->LE_CLESTTEM, SIGNAL(textEdited(const QString &)), SLOT(toUpper(const QString &)));
        break;

        case 4: // Mot clé Jeu.
            ui->Lb_TitreFenetre->setText("Création d'un Mot-Clé");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
            // Connecte l'évenement textEdited à la fonction toUpper
            connect(ui->LE_CLESTTEM, SIGNAL(textEdited(const QString &)), SLOT(toUpper(const QString &)));
        break;

        case 5: // Type Membre.
            ui->Lb_TitreFenetre->setText("Création d'un Type Membre");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case 6: // Titre Membre.
            ui->Lb_TitreFenetre->setText("Création d'un Titre Membre");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Emprunt :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();
        break;

        case 7: // Type Emprunt.
            ui->Lb_TitreFenetre->setText("Création d'un Type Emprunt");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Durée :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();
        break;

        case 8: // Lieux Ludothèque.
            ui->Lb_TitreFenetre->setText("Création d'un Lieu");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case 9: // Paiement Membre.
            ui->Lb_TitreFenetre->setText("Création d'un Paiement");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case 10: // Activité d'un membre.
            ui->Lb_TitreFenetre->setText("Création d'une activité");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        default: // Erreur création fenêtre.
            qDebug()<< "Erreur lors de la création du F_PopUpSTELTECM." << endl;
        break;
    }

    this->show();
}

/**
 *  @brief Ouvrir la fenêtre en mode "modifier".
 *
 *  @param sCLESTTEM
 */
void F_PopUpCLESTTEM::Modifier(QString sCLESTTEM)
{
    QSqlQuery Recherche;
    this->bCLESTTEM = false;

    switch(this->nCLESTTEM)
    {
        case 0: // Etat Jeu.
            ui->Lb_TitreFenetre->setText("Modification d'un Etat");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT Etat FROM etatsjeu WHERE Etat=:Etat");
            Recherche.bindValue(":Etat", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
        break;

        case 1: // Statut Jeu.
            ui->Lb_TitreFenetre->setText("Modification d'un Statut");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT StatutJeu FROM statutjeux WHERE StatutJeu=:StatutJeu");
            Recherche.bindValue(":StatutJeu", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
        break;

        case 2: // Emplacement Jeu.
            ui->Lb_TitreFenetre->setText("Modification d'un Emplacement");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT Nom FROM emplacement WHERE Nom=:Nom");
            Recherche.bindValue(":Nom", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
        break;

        case 3: // Classification Jeu.
            ui->Lb_TitreFenetre->setText("Modification d'une Classification");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Classification :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT TypeJeux,Classification FROM typejeux WHERE TypeJeux=:TypeJeux");
            Recherche.bindValue(":TypeJeux", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
            ui->LE_Autres->setText(Recherche.value(1).toString());
        break;

        case 4: // Mot clé Jeu.
            ui->Lb_TitreFenetre->setText("Modification d'un Mot-Clé");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT MotCle FROM motscles WHERE MotCle=:MotCle");
            Recherche.bindValue(":MotCle", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
        break;

        case 5: // Type Membre.
            ui->Lb_TitreFenetre->setText("Modification d'un Type Membre");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT TypeMembre FROM typemembres WHERE TypeMembre=:TypeMembre");
            Recherche.bindValue(":TypeMembre", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
        break;

        case 6: // Titre Membre.
            ui->Lb_TitreFenetre->setText("Modification d'un Titre");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Emprunt :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT NomTitre,NbrJeuxEmpruntables FROM titremembre WHERE NomTitre=:NomTitre");
            Recherche.bindValue(":NomTitre", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
            ui->LE_Autres->setText(Recherche.value(1).toString());
        break;

        case 7: // Type Emprunt.
            ui->Lb_TitreFenetre->setText("Modification d'un Type Emprunt");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Emprunt :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT TypeEmprunt,DureeEmprunt FROM typeemprunt WHERE TypeEmprunt=:TypeEmprunt");
            Recherche.bindValue(":TypeEmprunt", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
            ui->LE_Autres->setText(Recherche.value(1).toString());
        break;

        case 8: // Lieux Ludothèque
            ui->Lb_TitreFenetre->setText("Modification d'un Lieu");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT NomLieux FROM lieux WHERE NomLieux=:NomLieux");
            Recherche.bindValue(":NomLieux", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
        break;

        case 9: // Paiement Membre.
            ui->Lb_TitreFenetre->setText("Modification d'un Paiement");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            Recherche.prepare("SELECT NomPaiement FROM modepaiement WHERE NomPaiement=:NomPaiement");
            Recherche.bindValue(":NomPaiement", sCLESTTEM);
            Recherche.exec();
            Recherche.next();

            this->sCLESTTEM = Recherche.value(0).toString();
            ui->LE_CLESTTEM->setText(Recherche.value(0).toString());
        break;

        case 10: // Activité d'un membre.
            ui->Lb_TitreFenetre->setText("Modification d'une activité");
            ui->LE_CLESTTEM->setText(sCLESTTEM);
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        default: // Erreur création fenêtre.
            qDebug()<< "Erreur lors de la création du F_PopUpSTELTECM." << endl;
        break;
    }

    this->show();
}

/**
 *  @brief Permet de valider l'ajout ou la modification en fonction de nCLESTTEM.
 *
 */
void F_PopUpCLESTTEM::on_Bt_Valider_clicked()
{
    QSqlQuery RequeteValider;
    switch(this->nCLESTTEM)
    {
        case 0: // Etat Jeu.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO etatsjeu (Etat) VALUES (:Etat)");
                RequeteValider.bindValue(":Etat", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE etatsjeu SET Etat=:Etat WHERE Etat=:EtatPrecedent");
                    RequeteValider.bindValue(":Etat", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":EtatPrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                }
            }
        break;

        case 1: // Statut Jeu.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO statutjeux (StatutJeu) VALUES (:StatutJeu)");
                RequeteValider.bindValue(":StatutJeu", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE statutjeux SET StatutJeu=:StatutJeu WHERE StatutJeu=:StatutJeuPrecedent");
                    RequeteValider.bindValue(":StatutJeu", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":StatutJeuPrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                }
            }
        break;

        case 2: // Emplacement Jeu.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO emplacement (Nom) VALUES (:Nom)");
                RequeteValider.bindValue(":Nom", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE emplacement SET Nom=:Nom WHERE Nom=:NomPrecedent");
                    RequeteValider.bindValue(":Nom", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":NomPrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                }
            }
        break;

        case 3: // Classification Jeu.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO typejeux(TypeJeux, Classification) VALUES(:TypeJeux, :Classification)") ;
                RequeteValider.bindValue(":TypeJeux", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":Classification", ui->LE_Autres->text());
                RequeteValider.exec();
                RequeteValider.next();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    qDebug()<< this->sCLESTTEM << endl;
                    RequeteValider.prepare("UPDATE typejeux SET TypeJeux=:TypeJeux, Classification=:Classification WHERE TypeJeux=:TypeJeuxPrecedent");
                    RequeteValider.bindValue(":TypeJeux", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":Classification", ui->LE_Autres->text());
                    RequeteValider.bindValue(":TypeJeuxPrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                    RequeteValider.next();
                }
            }
        break;

        case 4: // Mot clé Jeu.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO motscles (MotCle) VALUES (:MotCle)");
                RequeteValider.bindValue(":MotCle", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
                RequeteValider.next();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE motscles SET MotCle=:MotCle WHERE MotCle=:MotClePrecedent");
                    RequeteValider.bindValue(":MotCle", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":MotClePrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                    RequeteValider.next();
                }
            }
        break;

        case 5: // Type Membre.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO typemembres (TypeMembre) VALUES (:TypeMembre)");
                RequeteValider.bindValue(":TypeMembre", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
                RequeteValider.next();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE typemembres SET TypeMembre=:TypeMembre WHERE TypeMembre=:TypeMembrePrecedent");
                    RequeteValider.bindValue(":TypeMembre", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":TypeMembrePrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                    RequeteValider.next();
                }
            }
        break;

        case 6: // Titre Membre.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO titremembre (NomTitre, NbrJeuxEmpruntables) VALUES (:NomTitre, :NbrJeuxEmpruntables)");
                RequeteValider.bindValue(":NomTitre", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":NbrJeuxEmpruntables", ui->LE_Autres->text().toInt());
                RequeteValider.exec();
                RequeteValider.next();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE titremembre SET NomTitre=:NomTitre, NbrJeuxEmpruntables=:NbrJeuxEmpruntables WHERE NomTitre=:NomTitrePrecedent");
                    RequeteValider.bindValue(":NomTitre", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":NbrJeuxEmpruntables", ui->LE_Autres->text().toInt());
                    RequeteValider.bindValue(":NomTitrePrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                    RequeteValider.next();
                }
            }
        break;

        case 7: // Type Emprunt.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO typeemprunt(TypeEmprunt, DureeEmprunt) VALUES(:TypeEmprunt, :DureeEmprunt)") ;
                RequeteValider.bindValue(":TypeEmprunt", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":DureeEmprunt", ui->LE_Autres->text().toInt());
                RequeteValider.exec();
                RequeteValider.next();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE typeemprunt SET TypeEmprunt=:TypeEmprunt, DureeEmprunt=:DureeEmprunt WHERE TypeEmprunt=:TypeEmpruntPrecedent");
                    RequeteValider.bindValue(":TypeEmprunt", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":DureeEmprunt", ui->LE_Autres->text().toInt());
                    RequeteValider.bindValue(":TypeEmpruntPrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                    RequeteValider.next();
                }
            }
        break;

        case 8: // Lieux Ludothèque.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO lieux (NomLieux) VALUES (:NomLieux)");
                RequeteValider.bindValue(":NomLieux", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE lieux SET NomLieux=:NomLieux WHERE NomLieux=:NomLieuxPrecedent");
                    RequeteValider.bindValue(":NomLieux", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":NomLieuxPrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                }
            }
        break;

        case 9: // Paiement Membre.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO modepaiement (NomPaiement) VALUES (:NomPaiement)");
                RequeteValider.bindValue(":NomLieux", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE modepaiement SET NomPaiement=:NomPaiement WHERE NomPaiement=:NomPaiementPrecedent");
                    RequeteValider.bindValue(":NomPaiement", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":NomPaiementPrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                }
            }
        break;

        case 10: // Activité Membre.
            if(this->bCLESTTEM)
            {
                RequeteValider.prepare("INSERT INTO activite (Activite) VALUES (:Activite)");
                RequeteValider.bindValue(":Activite", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                RequeteValider.prepare("UPDATE activite SET Activite=:Activite WHERE Activite=:ActivitePrecedent");
                RequeteValider.bindValue(":Activite", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":ActivitePrecedent", this->sCLESTTEM);
                RequeteValider.exec();
            }
        break;

        default: // Erreur création fenêtre.
            qDebug()<< "Erreur lors de la création du F_PopUpSTELTECM." << endl;
        break;
    }

    emit(this->SignalValider());
    this->close();
}

/**
 *  @brief Ferme la pop-up.
 *
 */
void F_PopUpCLESTTEM::on_Bt_Annuler_clicked()
{
    this->close();
}

/**
 *  @brief Verrouille ou Déverouille le bouton "Valider" quand le champs du TextEdit LE_CLESTTEM n'est pas vide.
 *
 *  @param arg1
 */
void F_PopUpCLESTTEM::on_LE_CLESTTEM_textEdited(const QString &arg1)
{
    if(arg1 == "")
    {
        ui->Bt_Valider->setEnabled(false) ;
    }
    else
    {
        ui->Bt_Valider->setEnabled(true);
    }
}

/**
 *  @brief Verrouille ou Déverouille le bouton "Valider" quand le champs du TextEdit LE_Autres n'est pas vide.
 *
 *  @param arg1
 */
void F_PopUpCLESTTEM::on_LE_Autres_textEdited(const QString &arg1)
{
    if(arg1 == "")
    {
        ui->Bt_Valider->setEnabled(false) ;
    }
    else
    {
        ui->Bt_Valider->setEnabled(true);
    }
}

/**
 *  @brief Mets en majuscule le texte saisi dans un QLineEdit
 *
 *  @param text
 */
void F_PopUpCLESTTEM::toUpper(const QString &text)
{
    QLineEdit *le = qobject_cast<QLineEdit *>(sender());
    if (!le)
    return;
    le->setText(text.toUpper());
}
