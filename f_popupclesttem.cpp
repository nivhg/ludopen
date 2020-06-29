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
 *      -Si 11, il s'agit d'un mot-clé d'un jeu.
 *      -Si 12, il s'agit d'un domaine d'email
 *      -Si 13, il s'agit d'une banque
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

#include "w_contenujeu.h"
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
F_PopUpCLESTTEM::F_PopUpCLESTTEM(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_PopUpCLESTTEM)
{
    pContenuJeu=dynamic_cast <W_ContenuJeu *>(parent);
    ui->setupUi(this);
    this->TbV_CLESTTEM=0;
    ui->TE_CLESTTEM->setVisible(false);
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
int F_PopUpCLESTTEM::Ajouter(int nCLESTTEM)
{
    this->nCLESTTEM = nCLESTTEM;
    this->bCLESTTEM = true;
    ui->Lb_CLESTTEM->show();
    ui->LE_CLESTTEM->show();
    ui->Lb_Autres->show();
    ui->LE_Autres->show();
    ui->TbV_CLESTTEM->hide();

    switch(this->nCLESTTEM)
    {
        case POPUP_ETAT_JEU: // Etat Jeu.
            ui->Lb_TitreFenetre->setText("Création d'un Etat");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_STATUT: // Statut Jeu.
            ui->Lb_TitreFenetre->setText("Création d'un Statut");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_EMPLACEMENT: // Emplacement Jeu.
            ui->Lb_TitreFenetre->setText("Création d'un Emplacement");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_CLASSIFICATION: // Classification Jeu.
            ui->Lb_TitreFenetre->setText("Création d'une Classification");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Classification :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();
            // Connecte l'évenement textEdited à la fonction toUpper
            connect(ui->LE_CLESTTEM, SIGNAL(textEdited(const QString &)), SLOT(toUpper(const QString &)));
        break;

        case POPUP_MOT_CLE: // Mot clé Jeu.
            ui->Lb_TitreFenetre->setText("Création d'un Mot-Clé");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
            // Connecte l'évenement textEdited à la fonction toUpper
            connect(ui->LE_CLESTTEM, SIGNAL(textEdited(const QString &)), SLOT(toUpper(const QString &)));
        break;

        case POPUP_TYPE_MEMBRE: // Type Membre.
            ui->Lb_TitreFenetre->setText("Création d'un Type Membre");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_TITRE_MEMBRE: // Titre Membre.
            ui->Lb_TitreFenetre->setText("Création d'un Titre Membre");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Emprunt :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();
        break;

        case POPUP_TYPE_EMPRUNT: // Type Emprunt.
            ui->Lb_TitreFenetre->setText("Création d'un Type Emprunt");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Durée :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();
        break;

        case POPUP_LIEU: // Lieux Ludothèque.
            ui->Lb_TitreFenetre->setText("Création d'un Lieu");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_PAIEMENT: // Paiement Membre.
            ui->Lb_TitreFenetre->setText("Création d'un Paiement");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_ACTIVITE: // Activité d'un membre.
            ui->Lb_TitreFenetre->setText("Création d'une activité");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_AUTEUR: // Auteur
            ui->Lb_TitreFenetre->setText("Création d'un auteur");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_DOMAINE_EMAIL: // Domaine d'email.
            ui->Lb_TitreFenetre->setText("Création d'un domaine d'email");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
        break;

        case POPUP_BANQUE: // Banque
            ui->Lb_TitreFenetre->setText("Création d'une banque");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Acronyme :");
            ui->LE_Autres->show();
            ui->LE_Autres->clear();
        break;

        case POPUP_CONTENUJEU:
            ui->Lb_TitreFenetre->setText("Ajouter un ensemble de pièces");
            ui->Lb_Autres->show();
            ui->Lb_Autres->setText("Merci de suivre les recommandations suivantes afin que LudOpen reconnaisse correctement la saisie :"
                                   "\n- Ne pas mettre plusieurs éléments de jeux sur la même ligne"
                                   "\n- Mettre le nombre de pièces avant leur description"
                                   "\n- Les éléments d'un groupe doivent commencer par une tabulation (touche avec la double flèche à gauche du A)");
            ui->TE_CLESTTEM->show();
            ui->TE_CLESTTEM->setEnabled(true);
            ui->LE_CLESTTEM->hide();
            ui->Lb_CLESTTEM->hide();
            ui->LE_Autres->hide();
            this->setFixedSize(QSize(900,300));
            // Connecte l'évenement textEdited à la fonction toUpper
            connect(ui->Bt_Valider, SIGNAL(clicked(bool)), pContenuJeu, SLOT(slot_contenujeu_clicked()));

        break;


    default: // Erreur création fenêtre.
            qDebug()<< "Erreur lors de la création du F_PopUpSTELTECM." << endl;
        break;
    }

    ui->LE_CLESTTEM->setEnabled(true);
    ui->LE_Autres->setEnabled(true);
    return this->exec();
}

/**
 *  @brief Ouvrir la fenêtre en mode "modifier".
 *
 *  @param sCLESTTEM
 */
int F_PopUpCLESTTEM::Modifier(QString sCLESTTEM, int nCLESTTEM, QTableView * TbV_CLESTTEM)
{
    QSqlQuery Recherche;
    this->bCLESTTEM = false;
    this->nCLESTTEM = nCLESTTEM;
    ui->TbV_CLESTTEM->hide();
    int i=0;

    switch(this->nCLESTTEM)
    {
        case 0: // Etat Jeu.
            ui->Lb_TitreFenetre->setText("Modification d'un Etat");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
        break;

        case 1: // Statut Jeu.
            ui->Lb_TitreFenetre->setText("Modification d'un Statut");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
        break;

        case 2: // Emplacement Jeu.
            ui->Lb_TitreFenetre->setText("Modification d'un Emplacement");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
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

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
        break;

        case 5: // Type Membre.
            ui->Lb_TitreFenetre->setText("Modification d'un Type Membre");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
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

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
        break;

        case 9: // Paiement Membre.
            ui->Lb_TitreFenetre->setText("Modification d'un Paiement");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
        break;

        case 10: // Activité d'un membre.
            ui->Lb_TitreFenetre->setText("Modification d'une activité");
            ui->LE_CLESTTEM->setText(sCLESTTEM);
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
            this->sCLESTTEM = sCLESTTEM;
        break;

        case POPUP_AUTEUR: // Auteur
            ui->Lb_TitreFenetre->setText("Modification d'un mot-clé");
            ui->LE_CLESTTEM->setText(sCLESTTEM);
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();
            this->sCLESTTEM = sCLESTTEM;
        break;

        case 12: // Domaine d'email.
            ui->Lb_TitreFenetre->setText("Modification d'un domaine d'email");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->LE_Autres->clear();

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
        break;

        case 13: // Banque
            ui->Lb_TitreFenetre->setText("Modification d'une banque");
            ui->LE_CLESTTEM->clear();
            ui->Lb_Autres->setText("Acronyme");
            ui->Lb_Autres->show();
            ui->LE_Autres->show();
            ui->LE_Autres->clear();

            this->sCLESTTEM = sCLESTTEM;
            ui->LE_CLESTTEM->setText(sCLESTTEM);
        break;

        case 255: // Suppression d'une valeur toujours utilisée : choix d'une autre valeur à donner

            ui->Lb_TitreFenetre->setText("La valeur est toujours utilisée par des élements. \n"
                                         "Choisir la nouvelle valeur à donner pour ces élements :");
            ui->Lb_CLESTTEM->hide();
            ui->LE_CLESTTEM->hide();
            ui->Lb_Autres->hide();
            ui->LE_Autres->hide();
            ui->TbV_CLESTTEM->hide();
            ui->GL->addWidget(TbV_CLESTTEM,6,0,1,4);
            this->TbV_CLESTTEM=TbV_CLESTTEM;
            ui->LE_Autres->clear();
            ui->Bt_Valider->setEnabled(true);
            this->sCLESTTEM = sCLESTTEM;
        break;

        default: // Erreur création fenêtre.
            qDebug()<< "Erreur lors de la création du F_PopUpSTELTECM." << endl;
        break;
    }

    ui->LE_CLESTTEM->setEnabled(true);
    ui->LE_Autres->setEnabled(true);
    return this->exec();
}

/**
 *  @brief Permet de valider l'ajout ou la modification en fonction de nCLESTTEM.
 *
 */
void F_PopUpCLESTTEM::on_Bt_Valider_clicked()
{
    QSqlQuery RequeteValider;
    QString requete;
    QVariant test;
    switch(this->nCLESTTEM)
    {
        case POPUP_ETAT_JEU: // Etat Jeu.
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

        case POPUP_STATUT: // Statut Jeu.
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

        case POPUP_EMPLACEMENT: // Emplacement Jeu.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO emplacement (NomEmplacement) VALUES (:Nom)");
                RequeteValider.bindValue(":Nom", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                if(this->bCLESTTEM == false)
                {
                    RequeteValider.prepare("UPDATE emplacement SET NomEmplacement=:NomEmplacement WHERE NomEmplacement=:NomPrecedent");
                    RequeteValider.bindValue(":NomEmplacement", ui->LE_CLESTTEM->text());
                    RequeteValider.bindValue(":NomPrecedent", this->sCLESTTEM);
                    RequeteValider.exec();
                }
            }
        break;

        case POPUP_CLASSIFICATION: // Classification Jeu.
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

        case POPUP_MOT_CLE: // Mot clé Jeu.
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

        case POPUP_TYPE_MEMBRE: // Type Membre.
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

        case POPUP_TITRE_MEMBRE: // Titre Membre.
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

        case POPUP_TYPE_EMPRUNT: // Type Emprunt.
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

        case POPUP_LIEU: // Lieux Ludothèque.
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

        case POPUP_PAIEMENT: // Paiement Membre.
            if(this->bCLESTTEM == true)
            {
                RequeteValider.prepare("INSERT INTO modepaiement (NomPaiement) VALUES (:NomPaiement)");
                RequeteValider.bindValue(":NomPaiement", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                RequeteValider.prepare("UPDATE modepaiement SET NomPaiement=:NomPaiement WHERE NomPaiement=:NomPaiementPrecedent");
                RequeteValider.bindValue(":NomPaiement", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":NomPaiementPrecedent", this->sCLESTTEM);
                RequeteValider.exec();
            }
        break;

        case POPUP_ACTIVITE: // Activité Membre.
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
                requete=RequeteValider.lastQuery();
            }
        break;

        case POPUP_AUTEUR: // Auteur
            if(this->bCLESTTEM)
            {
                RequeteValider.prepare("INSERT INTO auteurs (NomAuteur) VALUES (:NomAuteur)");
                RequeteValider.bindValue(":NomAuteur", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                RequeteValider.prepare("UPDATE auteurs SET NomAuteur=:NomAuteur WHERE NomAuteur=:NomAuteurPrecedent");
                RequeteValider.bindValue(":NomAuteur", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":NomAuteurPrecedent", this->sCLESTTEM);
                RequeteValider.exec();
                requete=RequeteValider.lastQuery();
            }
        break;
        case POPUP_DOMAINE_EMAIL: // Domaine d'email.
            if(this->bCLESTTEM)
            {
                RequeteValider.prepare("INSERT INTO domaineemail (NomDomaine) VALUES (:NomDomaine)");
                RequeteValider.bindValue(":NomDomaine", ui->LE_CLESTTEM->text());
                RequeteValider.exec();
            }
            else
            {
                RequeteValider.prepare("UPDATE domaineemail SET NomDomaine=:NomDomaine WHERE NomDomaine=:NomDomainePrecedent");
                RequeteValider.bindValue(":NomDomaine", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":NomDomainePrecedent", this->sCLESTTEM);
                RequeteValider.exec();
                requete=RequeteValider.lastQuery();
            }
        break;

        case POPUP_BANQUE: // Banque
            if(this->bCLESTTEM)
            {
                RequeteValider.prepare("INSERT INTO banques (NomBanque,Acronyme) VALUES (:NomBanque,:Acronyme)");
                RequeteValider.bindValue(":NomBanque", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":Acronyme", ui->LE_Autres->text());
                RequeteValider.exec();
                qDebug()<<getLastExecutedQuery(RequeteValider)<<RequeteValider.lastError();
            }
            else
            {
                RequeteValider.prepare("UPDATE banques SET NomBanque=:NomBanque,Acronyme=:Acronyme WHERE NomBanque=:NomBanquePrecedent");
                RequeteValider.bindValue(":NomBanque", ui->LE_CLESTTEM->text());
                RequeteValider.bindValue(":NomBanquePrecedent", this->sCLESTTEM);
                RequeteValider.bindValue(":Acronyme", ui->LE_Autres->text());
                RequeteValider.exec();
                requete=RequeteValider.lastQuery();
            }
        break;

        case POPUP_CONTENUJEU:
            ui->Bt_Valider->setText(ui->TE_CLESTTEM->toPlainText());
            ui->TE_CLESTTEM->clear();
        break;

        case 255: // Suppression
        break;

        default: // Erreur création fenêtre.
            qDebug()<< "Erreur lors de la création du F_PopUpSTELTECM." << endl;
        break;
    }

    emit(this->SignalValider());
    this->done(1);
    this->close();
}

/**
 *  @brief Ferme la pop-up.
 *
 */
void F_PopUpCLESTTEM::on_Bt_Annuler_clicked()
{
    this->done(0);
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
    int curPos=le->cursorPosition();
    le->setText(text.toUpper());
    le->setCursorPosition(curPos);
}

void F_PopUpCLESTTEM::on_TE_CLESTTEM_textChanged()
{
    ui->Bt_Valider->setEnabled(true);
}
