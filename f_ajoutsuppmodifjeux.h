//f_ajoutsuppmodifjeux.h	0.1	<Mary Florian>
#ifndef F_AJOUTSUPPMODIFJEUX_H
#define F_AJOUTSUPPMODIFJEUX_H

class F_AjoutSuppModifJeux;

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QtWidgets>
#include <QStandardItemModel>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "d_ajoutsuppmodiffournisseursediteurs.h"
#include "f_popupclesttem.h"
#include "lb_image.h"
#include "d_image.h"

using namespace std;

class D_AjoutSuppModifFournisseursEditeurs;

namespace Ui {
    class F_AjoutSuppModifJeux;
}

/** @brief Classe qui permet de gérer les jeux
 *
 *  Cette classe permet de gérer les jeux, c'est à dire
 *  que depuis la fenêtre administration, et le widget Jeux,
 *  il est possible d'ajouter, de supprimer ou de modifier une fiche de jeux.
 *
 */
class F_AjoutSuppModifJeux : public QWidget
{
    Q_OBJECT

public:
    explicit F_AjoutSuppModifJeux(QWidget *parent = 0);
    ~F_AjoutSuppModifJeux();

    // METHODEs -----------------------------------------------------------------
    /** @brief Permet d'actualiser le Cbx_Emplacement lorsqu'on ajout à été fait
     */
    void ActualiserCBx_Emplacement() ;
    /** @brief Permet d'actualiser le Cbx_Fournisseur lorsqu'un ajout a été fait
     */
    void ActualiserCBx_Fournisseur() ;
    /** @brief Permet d'actualiser le Cbx_Editeur lorsqu'un ajout a été fait
     */
    void ActualiserCBx_Editeur() ;
    /** @brief Permet d'actualiser le Cbx_Statut lorsqu'un ajout a été fait
     */
    void ActualiserCBx_Statut() ;
    /** @brief Permet d'actualiser le Cbx_Etat lorsqu'un ajout a été fait
     */
    void ActualiserCBX_Etat() ;
    /** @brief Permet d'actualiser le Cbx_Classification lorsqu'un ajout a été fait
     */
    void ActualiserCBx_Classification() ;
    /** @brief Permet d'actualiser le Cbx_MotCle lorsquuon ajout a été fait
     */
    void ActualiserCBx_MotCle() ;

    /** @brief Permet d'ajouter un image pour un jeu
     */
    void AjouterImage(QString sCheminImage , QString sCodeJeux ) ;
    
    /** @brief Affiche tous les jeux dans le tableau
     */
    void AfficherJeux();

    /** @brief Affiche les informations sur un jeu
     */
    void AfficherJeu();

    /** @brief Vide les champs d'un jeu
     */
    void VideChamps();

    /** @brief Active ou désactive les champs d'un jeu
     */
    void ActiveChamps(bool etat);

    /** @brief Active ou désactive les boutons valider et annuler
     */
    void ActiveBoutons(bool etat);

    /** @brief Bloque les signaux pour éviter que la modification de ces champs active les boutons de validation et d'annulation
    */
    void BloquerSignalsChamps(bool etat);


public slots:
    void slot_ValiderClassification();
    void slot_ValiderEtat();
    void slot_ValiderEmplacement();
    void slot_ValiderMotCle();
    void slot_ValiderStatut();
    void slot_ActiverClicImage();

private slots:
    // METHODEs -----------------------------------------------------------------
    
    /** @brief Appelé lorsque l'on modifier le contenu de la case de recherche par nom
     */
    void on_LE_RechercheNom_textChanged(const QString &arg1);
    
    /** @brief Appelé lorsque l'on clic sur le bouton Ajouter
     */
    void on_Bt_Ajouter_clicked();
    
    /** @brief Appelé lorsque l'on clic sur le bouton Supprimer
     */
    void on_Bt_Supprimer_clicked();
                
    /** @brief Appelé lorsque le contenu du Cbx_Classification change
     */
    void on_CBx_Classification_activated(int index);

    /** @brief Appelé lorsque le contenu du Cbx_Etat change
     */
    void on_CBx_Etat_activated(int index);

    /** @brief Appelé lorsque le contenu du Cbx_Statut change
     *
     */
    void on_CBx_Statut_activated(int index);

    /** @brief Appelé lorsque le contenu du Cbx_Editeur change
     */
    void on_CBx_Editeur_activated(int index);

    /** @brief Appelé lorsque le contenu du Cbx_Fournisseur change
     */
    void on_CBx_Fournisseur_activated(int index);

    /** @brief Appelé lorsque l'on sélectionne sur une case du tableau
     */
    void on_TbV_Recherche_selectionChanged(const QModelIndex &index);
    
    /** @brief Appelé lorsque l'on clic sur le bouton Valider
     */
    void on_Bt_Valider_clicked();

    /** @brief Appelé lorsque l'on clic sur le bouton Annuler
     */
    void on_Bt_Annuler_clicked();

    /** @brief Appelé lorsque le contenu du CBx_MotCle1 change
     */
    void on_CBx_MotCle1_activated(int index);
    
    /** @brief Appelé lorsque le contenu du CBx_MotCle2 change
     */
    void on_CBx_MotCle2_activated(int index);
    
    /** @brief Appelé lorsque le contenu du CBx_MotCle3 change
     */
    void on_CBx_MotCle3_activated(int index);

    /** @brief Appelé lorsque le contenu du Cbx_Emplacement change
     */
    void on_CBx_Emplacement_activated(int index);

    void on_Bt_Gauche_clicked();

    void on_Bt_Droite_clicked();

    void on_Lb_Image_clicked();

    void toUpper(const QString &text);

    void on_TbV_Recherche_clicked(const QModelIndex &index);

    /** @brief Pour gérer le jeu de cache cache des boutons Valider/Annuler/Ajouter/Supprimer
     */
    void CacherBoutons() ;

private:
    // ATTRIBUTs ----------------------------------------------------------------

    Ui::F_AjoutSuppModifJeux *ui;
    
    /** @brief Contient le code du jeu en cours de consultation
     *
     */
    //Code du jeu actif sur la fenêtre
    QString nIdJeuSelectionne ;
    
    //! Model du TableView des jeux
    QStandardItemModel * ModelJeu;
    
    /** @brief Pointeur qui fait le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pEmplacementAjMod;

    /** @brief Pointeur qui fait le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pStatutAjMod;

    /** @brief Pointeur qui fait le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pEtatAjMod;

    /** @brief Pointeur qui fait le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pClassificationAjMod;
    
    /** @brief Pointeur qui fait le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pMotCleAjMod;

    /** @brief Pointeur qui fait le lien entre la classe D_AjoutSuppModifFournisseursEditeurs coté fournisseur et celle-ci
     */
    D_AjoutSuppModifFournisseursEditeurs * pAjoutFournisseur;

    /** @brief Pointeur qui fait le lien entre la classe D_AjoutSuppModifFournisseursEditeurs coté éditeur et celle-ci
     */
    D_AjoutSuppModifFournisseursEditeurs * pAjoutEditeur;
    
    /** @brief Pointeur qui fait le lien entre la classe D_Image et celle-ci
     */
    D_Image * pImage;

    /** @brief Tableau permettant de retrouver l'index sélectionné
     * 0 : Etat
     * 1 : Statut
     * 2 : Editeur
     * 3 : Fournisseur
     * 4 : Emplacement
     * 5 : MotCle1
     * 6 : MotCle2
     * 7 : MotCle3
     */
    unsigned int TableauPositionIndex [8] ;
    
    /** @brief Booleen permettant de savoir si l'on est en ajout ou en modification d'un jeu
     * true : ajout
     * false : modificatiion
     */
    bool AjoutOuModif ;

    //! Label Image
    Lb_Image* lb_image;
};

//------------------------------------------------------------------------------
#endif // F_AJOUTSUPPMODIFJEUX_H
//------------------------------------------------------------------------------
