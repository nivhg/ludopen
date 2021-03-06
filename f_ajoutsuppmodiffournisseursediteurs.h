//f_ajoutsuppmodiffournisseursediteurs.h	0.1	<Mary Florian>
#ifndef F_AJOUTSUPPMODIFFOURNISSEURSEDITEURS_H
#define F_AJOUTSUPPMODIFFOURNISSEURSEDITEURS_H

#define MODE_INDEFINI 0
#define MODE_FOURNISSEUR 1
#define MODE_EDITEUR 2
#define MODE_EDITEUR_FOURNISSEUR 3

#include "f_ajoutsuppmodifjeux.h"

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QSqlQuery>
#include <QtWidgets>
#include <QStandardItemModel>

#include "f_popupclesttem.h"


namespace Ui {
    class F_AjoutSuppModifFournisseursEditeurs;
}

/** @brief Classe qui permet de gérer les fournisseurs ainsi que les éditeurs
 *
 *  Cette classe permet de gérer les fournisseurs ainsi que les éditeurs, c'est à dire
 *  que depuis la fenêtre administration, et le widget Fournisseurs-Editeur,
 *  il est possible de consulter, d'ajouter, de supprimer ou de modifier une fiche de fournisseur ou d'éditeur.
 *
 */
class F_AjoutSuppModifFournisseursEditeurs : public QWidget
{
    Q_OBJECT

public:
    explicit F_AjoutSuppModifFournisseursEditeurs(QWidget *parent = 0, char Mode=MODE_INDEFINI);
    ~F_AjoutSuppModifFournisseursEditeurs();
    /**
     *  @brief Cache la partie recherche de la fenêtre et active les champs fournisseur/editeur
     */
    void AjoutFournisseursEditeursSeulement();
    void ActualiserModele(QSqlQuery requete);

private slots:
    // METHODEs -----------------------------------------------------------------
    /** @brief Appelé lorsque l'on modifier le contenu de la case de recherche
     *
     */
    void on_LE_Recherche_textChanged(const QString &arg1);

    /** @brief Appelé lorsque l'on clic sur une case du tableau contenant les fournisseurs ou éditeurs
     *
     */
    void on_TbV_Recherche_clicked(const QModelIndex &index);

    /** @brief Appelé lorsque l'on clic sur le bouton OK
     *
     */
    void on_Bt_Ok_clicked();
    /** @brief Appelé lorsque l'on clic sur le bouton Ajouter
     *
     */
    void on_Bt_Ajouter_clicked();
    /** @brief Appelé lorsque l'on clic sur le bouton Supprimer
     *
     */
    void on_Bt_Supprimer_clicked();
    /** @brief Appelé lorsque l'on clic sur le bouton Valider
     *
     */
    void on_Bt_Valider_clicked();
    /** @brief Appelé lorsque l'on clic sur le bouton Annuler
     *
     */
    void on_Bt_Annuler_clicked();
    /** @brief Appelé lorsque l'on clic sur le radio bouton Fournisseur pour ne faire une recherche que de fournisseur
     *
     */
    void on_Rb_Fournisseur_clicked();
    /** @brief Appelé lorsque l'on clic sur le  radio bouton Editeur pour ne faire une recherche que d'éditeur
     *
     */
    void on_Rb_Editeur_clicked();
    /** @brief Appelé lorsque l'on clic sur le  radio bouton Les deux pour faire une recherche dans les fournisseurs
     *         ainsi que dans les éditeurs
     *
     */
    void on_Rb_Les2_clicked();
    /** @brief Appelé lorsque l'on clic sur la touche entrée du clavier
     *
     */
    void on_LE_Recherche_returnPressed();
    /** @brief Méthode permettant de vider tout les champs d'informations
     *
     */
    void ViderChamps() ;
    /** @brief Appelé lorsque l'on modifier le champ Nom
     *
     */
    void on_LE_Nom_textChanged(const QString &arg1);

    /**
     *  @brief Mets en majuscule le texte saisi dans un QLineEdit
     */
    void toUpper(const QString &text);

private:
    Ui::F_AjoutSuppModifFournisseursEditeurs *ui;

    // ATTRIBUTs ----------------------------------------------------------------

    //! Model du TableView des fournisseurs
    QStandardItemModel * ModelFournisseur;

    QWidget * Parent;

    /** @brief QString pour récuper le nom du fournisseur sur lequel on a cliqué dans le tableau
     *
     */
    QString Selection ;

    /** @brief Int pour stocket l'ID du fournisseur sur lequel on a cliqué dans le tableau
     */
    int IdSelection ;

    /** @brief Booléen pour savoir si l'on se trouve en mode ajout ou en mode modification
     *
     */
    /******************
      * true = ajout
      * false = modif
    ******************/
    bool AjoutOuModif ;

    // Definit le mode de la classe : editeur, fournisseur ou les 2
    char Mode ;

    ///< Pointeur qui contient l'adresse de la fenêtre graphique F_PopUpCLESTTEM.
    F_PopUpCLESTTEM * pPopUpCLESTTEM;

    ///< Creer un modele du tableau pour le table View de la fenêtre de pop-up
    QStandardItemModel * TbCLESTTEM;

    ///< Creer un modele du tableau pour le table View.
    QTableView * TbV_CLESTTEM;
};

//------------------------------------------------------------------------------
#endif // F_AJOUTSUPPMODIFFOURNISSEURSEDITEURS_H
//------------------------------------------------------------------------------
