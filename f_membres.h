#ifndef F_MEMBRES_H
#define F_MEMBRES_H

#include "f_historiquejeux.h"
#include "f_ajoutercoticarte.h"
#include "f_popupclesttem.h"
#include "f_panier.h"
#include "TitreType.h"

#include <QtSql>
#include <QtWidgets>
#include <QStandardItem>
#include <QtDebug>
#include <QVector>

using namespace std;

#define MODE_UTILISATEUR 0
#define MODE_ADMIN 1
#define MODE_MEMBRE_ASSOCIE 2
#define MODE_NON_ADHERENT 3

// Declaration des TYPEs UTILISATEURs ------------------------------------------

/** @struct Membre
 *  @brief Structure permettant la sauvegarde de quelques informations sur un membre
 *
 *  int + QString x 4
 */
typedef struct
{
    unsigned int id ; ///< ID du membre
    QString sNom ; ///< Nom du membre
    QString sPrenom ; ///< Prenom du membre
    QString sVille ; ///< Ville du membre
    QString sCodeMembre ; ///< Code personnel du membre
} Membre ;

namespace Ui {
class F_Membres;
}

/** @brief Permet de gérer gérer les membres
 *
 *  Il est possible d'y ajouter et de modifier un membre.
 *  On peut consulter les abonnements en cour ainsi que les emprunts en cours et l'historique des emprunts d'un membre.
 */
class F_Membres : public QWidget
{
    Q_OBJECT
    
public:
    // CONSTRUCTEURs et DESTRUCTEURs
    /** Initialisation de la classe
     *  @param  pRMembres : point sur un F_Membres, bAdmin : Indique si c'est une fenêtre administration
     *  @test   Voir la procédure dans le fichier associé.
     */
    explicit F_Membres(int iMode = MODE_UTILISATEUR, QWidget *parent = 0,
                       int nIdCollectivite=0,F_Panier *pPanier=0) ;
    ~F_Membres() ;


// METHODEs -----------------------------------------------------------------

    Ui::F_Membres * ui;

    //! Recupere l'emplacement d'un titre dans un vecteur en fonction de son id
    int RecupererEmplacementTitreVecteur (unsigned int nIdTitre) ;
    //! Recupere l'emplacement d'un type dans un vecteur en fonction de son id
    int RecupererEmplacementTypeVecteur (unsigned int nIdType) ;
    //! Recupere l'emplacement d'un domaine d'email dans un vecteur en fonction de son id
    int RecupererEmplacementDomaineEmailVecteur( unsigned int nIdDomaineEmail );

    //! Permet d'ajouter un membre
    bool AjouterMembre () ;

    //! Permet de modifier un membre
    bool ModifierMembre ( unsigned int nIdMembre ) ;

    //! Met à  jour la liste des Titres depuis la base de données
    void MaJTitre () ;

    //! Met à  jour la liste des Types depuis la base de données
    void MaJType () ;

    //! Met à  jour la liste des domaine d'email depuis la base de données
    void MaJDomaineEmail ();

    //! Affiche les jeux empruntés dans le tableau TbW_Emprunt
    void AfficherJeuxEmpruntes (unsigned int nIdMembre) ;

    //! Affiche les informations d'un membre
    void AfficherMembre ( unsigned int nIdMembre ) ;

    //! Affiche le membre précédement selectionné par défaut
    void AfficherMembre() ;

    //! Permet de supprimer un membre
    bool SupprimerMembre(int IdMembre) ;

    //! Verrouille ou deverouille les champs contenant les informations personnelles
    void VerrouillerInfosPerso ( bool bVerrouille ) ;

    //! Efface le contenu de tous les champs
    void EffacerTousLesChamps () ;

    //! Affiche ou cache les boutons PB_AjouterMembre et PB_ModifierMembre
    void AfficherAjouterModifierMembre ( bool bAffiche ) ;

    //! Affiche ou cache les boutons PB_Valider et PB_Annuler
    void AfficherValiderAnnuler ( bool bAffiche ) ;

    //! Verrouille ou deverouille l'emplacement des emprunts
    void VerrouillerJeux (bool bVerrouille ) ;

    //! Verrouille ou deverrouille l'emplacement des abonnements
    void VerrouillerAbonnements ( bool bVerrouille ) ;

    //! Verrouille ou deverrouille l'emplacement des activités
    void VerrouillerActivite ( bool bVerrouille ) ;

    //! Efface tous ce qui n'est pas un chiffre et ajoute un espace tous les 2 caractéres dans un string
    QString ModifierSyntaxeNumTelephone ( QString sNum) ;

    //! Efface tous ce qui n'est autorisé dans une adresse email
    QString ModifierSyntaxeEmail( QString sEmail );

    //! Affiche les abonnements dans le tableview abonnement
    void AfficherAbonnements( int nIdMembre ) ;

    //! Affiche les ville dans le combobox et selectionne la ville passé en paramétre
    void AfficherVilles( QString VilleSelectionne = "" ) ;

    //! Mise à jour de la liste des membres
    bool MaJListeMembres (int iModeMAJ=0);

    //! Affichage de la liste des membres
    void AfficherListe() ;//Liste par défaut(this->VecteurMembres)
    void AfficherListe(QVector<Membre> VecteurMembres ) ;//Liste donnée

    //! Recherche soit avec le nom, soit avec le numéro ou avec les 2
    void RechercherParNomEtNumero () ;

    //! Renvoie le premier code non utilisé
    int RecupererProchainCodeNonUtilise () ;

    //! Retourne le membre Selectionné dans le TableView
    int RecupererMembreSelectionne () ;

    //! Récupère la liste des activités et la mets dans une combobox
    static void ChargerActivites(QComboBox * scombobox);

signals:
    //! Signal indiquant qu'un non-adhérent a été crée et envoie son Code
    void Signal_Non_Adherent_Cree(int iCodeMembre);
    //! Signal indiquant qu'un adhérent a été crée
    void Signal_Adherent_Cree();

public slots:

    //! Permet l'ajout d'un type
    void slot_ChoisirNouveauType () ;

    //! Permet l'ajout d'un titre
    void slot_ChoisirNouveauTitre () ;

    //! Permet l'ajout d'un domaine d'email
    void slot_ChoisirNouveauDomaineEmail () ;

    //! Recupere l'id d'un membre pour l'afficher
    void slot_AfficherMembre (unsigned int nIdMembre) ;

    //! Acualise les abonnements du membre sélectionné
    void slot_ActualiserAbonnements() ;

    //! Permet l'ajout d'une ville
    void slot_ValiderAjoutVille() ;

    //! Permet l'annulation de l'ajour d'une ville
    void slot_AnnulerAjoutVille() ;

    //! Permet l'ajout d'un membre
    void on_Bt_AjouterMembre_clicked() ;

private slots:

    //! Selectionne un membre avec un double click
    void on_TbW_Recherche_doubleClicked(const QModelIndex &index);

    //! Selectionne un membre avec un click
    void on_TbW_Recherche_clicked(const QModelIndex &index);

    //! Recherche les membres correspondant au champs à  chaque fois que le champs est modifiée
    void on_LE_Nom_textEdited(const QString &arg1);

    //! Permet d'ajout un nouveau type si "Créer type..." a été sélectionné
    void on_CBx_Type_activated(int index);

    //! Permet d'ajout un nouveau titre si "Créer titre..." a été sélectionné
    void on_CBx_Titre_activated(int index);

    //! Annule l'ajout ou la modification d'un membre
    void on_Bt_AnnulerMembre_clicked() ;

    //! Valide la modification ou l'ajout d'un membre
    void on_Bt_ValiderMembre_clicked() ;

    //! Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
    void on_Le_TelFix_textEdited(const QString &arg1) ;

    //! Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
    void on_Le_TelMobile_textEdited(const QString &arg1) ;

    //! Met en forme (du type "00 00 00 00 00")  le numéro de télephone quand il est modifié
    void on_LE_Fax_textEdited(const QString &arg1) ;

    //! Affiche l'historique des jeux empruntés
    void on_Bt_Historique_clicked() ;

    //! Affiche ou cache membre écarté
    void on_ChBx_MembreEcarte_toggled(bool checked) ;

    //! Permet de modifier un membre
    void on_Bt_ModifierMembre_clicked() ;

    //! Active ou désactive le bouton valider en fonction de lineedit nom
    void on_Le_Nom_textEdited(const QString &arg1);

    //! Validation suppression d'un membre
    void on_Bt_SupprimerMembre_clicked();

    //! Selectionne une ligne entière dans le tableau LW_Abonnememnt
    void on_LW_Abonnements_clicked(const QModelIndex &index);

    //! Selectionne une ligne entière dans e tableau LW_Empruntes
    void on_LW_JeuxEmpruntes_clicked(const QModelIndex &index);

    //! Permet l'ajout d'un abonnement
    void on_Bt_AjouterAbonnement_clicked();

    //! Permet la modifiaction d'un abonnement
    void on_Bt_ModifierAbonnement_clicked();

    //! Permet de supprimer un abonnement
    void on_Bt_SupprimerAbonnement_clicked();

    //! Change de la couleur et met en gras membre  écarter quand le checkbox et check
    void on_ChBx_MembreEcarte_stateChanged(int arg1);

    //! Permet l'ajout de Ajouter une ville dans le combox Ville
    void on_CBx_Ville_currentIndexChanged(const QString &arg1);

    void on_LW_Abonnements_doubleClicked(const QModelIndex &index);

    void toUpper(const QString &text);

    void AfficherActivites( unsigned int nIdMembre );

    void on_Bt_AjouterActivite_clicked();

    void on_ChBx_Activites_clicked();

    void on_Bt_SupprimerActivite_clicked();

    void on_Bt_Associe_clicked();

    void on_Bt_AJouterAssocie_clicked();

    void on_Bt_SupprimerAssocie_clicked();

    void AjouterAssocie(QList <QStandardItem *> ListStandardItem);

    void on_Bt_ValiderAssocies_clicked();

    void on_CBx_Titre_currentIndexChanged(int index);

    void on_LE_Email_textEdited(const QString &arg1);

    void on_CBx_DomaineEmail_currentIndexChanged(int index);

    void on_CBx_Filtre_currentIndexChanged(int index);

private:

    // ATTRIBUTs ----------------------------------------------------------------

    QVector<Membre> VecteurMembres ;///< Vecteur contenant la liste des membres
    QVector<Membre> VecteurRechercheMembres ; ///< Vecteur contenant la liste des membres correspondant à  la recherche
    QStandardItemModel ModeleRechercheMembre;
    QStandardItemModel ModeleAssocies;

    F_HistoriqueJeux *   pHistoriqueJeux ;   //! Pointeur sur la classe F_HistoriqueJeux
    F_AjouterCotiCarte * pAjouterCotiCarte ; //! Pointeur sur la classe F_AjouterCoticarte

    F_PopUpCLESTTEM * pTypeAjMod ;      //! Pointeur sur la classe F_AjouterTitreType avec comme paramètre (5)
    F_PopUpCLESTTEM * pTitreAjMod ;     //! Pointeur sur la classe F_AjouterTitreType avec comme paramètre (6)
    F_PopUpCLESTTEM * pDomaineEmailAjMod;   //! Pointeur sur la classe F_PopUpCLESTTEM avec comme paramètre (12)

    QVector<Type>    VectorType ;            //! Vecteur contenant la liste des types (id, nom)
    QVector<Titre>   VectorTitre ;           //! Vecteur contenant la liste des titres (id, nom)
    QVector<Type>    VectorDomaineEmail;     //! Vecteur contenant la liste des domaine d'email(id, nom)
    QVector<QString> VecteurVille ;          //! Vecteur contenant la liste des villes
    QVector<int>     VecteurAbonnements ;    //! Vecteur contenant la liste des des Id des abonnements présent dans le tableau

    unsigned int nIdMembreSelectionne ;      //! Permet de savoir quelle membre est sélectionné
    unsigned int nIdCollectivite;

    int iMode ;                            //! Indique les droits d'adminstration (vrai = admin, faux = simple bénévole)

    /////////////Fenêtre d'ajout d'une ville///////////////
    QDialog     * oFenetreAjoutVille ;       //! Fenêtre pour l'ajout d'une ville
    QLineEdit   * LE_AjoutVille ;            //! LigneEdit permettant d'indiquer la ville
    QPushButton * Bt_ValiderVille ;          //! Bouton Valider pour la fenêtre d'ajout d'une ville
    QPushButton * Bt_AnnuerVille ;           //! Bouton Annuler pour la fenêtre d'ajout d'une ville
    QLabel * Lb_NomVille ;                   //! Label pour la fenêtre d'ajout d'une ville
    F_Membres * pAssocies;
    F_Panier *pPanier;
};

#endif // F_MEMBRES_H
