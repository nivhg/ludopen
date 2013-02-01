//f_ajoutsuppmodifjeux.h	0.1	<Mary Florian>
#ifndef F_AJOUTSUPPMODIFJEUX_H
#define F_AJOUTSUPPMODIFJEUX_H

class F_AjoutSuppModifJeux;

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QWidget>
#include <QStandardItemModel>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "f_ajouterfournisseur.h"
#include "f_ajouterediteur.h"
#include "f_popupclesttem.h"

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
    
public slots:
    void slot_ValiderClassification();
    void slot_ValiderEtat();
    void slot_ValiderEmplacement();
    void slot_ValiderMotCle();
    void slot_ValiderStatut();

private slots:
    // METHODEs -----------------------------------------------------------------

    /** @brief Appelé lorsque l'on clic sur le bouton OK
     */
    void on_Bt_OK_clicked();
    
    /** @brief Appelé lorsque l'on modifier le contenu de la case de recherche par nom
     */
    void on_LE_RechercheNom_textChanged(const QString &arg1);
    
    /** @brief Appelé lorsque l'on clic sur la touche Entrée du calicer
     */
    void on_LE_RechercheCode_returnPressed();
    
    /** @brief Appelé lorsque l'on modifier le contenu de la case de recherche par code
     */
    void on_LE_RechercheCode_textChanged(const QString &arg1);
    
    /** @brief Appelé lorsque l'on clic sur le bouton Ajouter
     */
    void on_Bt_Ajouter_clicked();
    
    /** @brief Appelé lorsque l'on clic sur le bouton Supprimer
     */
    void on_Bt_Supprimer_clicked();
            
    /** @brief Appelé lorsque le contenu de la case Nom change
     */
    void on_LE_Nom_textChanged(const QString &arg1);
    
    /** @brief Appelé lorsque le contenu du Cbx_Classification change
     */
    void on_CBx_Classification_activated(int index);
    
    /** @brief Appelé lorsque le contenu du SBx_JoueurMin change
     */
    void on_SBx_JoueursMin_valueChanged(int arg1);
    
    /** @brief Appelé lorsque le contenu du SBx_JoueurMax change
     */
    void on_SBx_JoueursMax_valueChanged(int arg1);
    
    /** @brief Appelé lorsque le contenu du SBx_AgeMin change
     */
    void on_SBx_AgeMin_valueChanged(int arg1);
    
    /** @brief Appelé lorsque le contenu du SBx_AgeMax change
     */
    void on_SBx_AgeMax_valueChanged(int arg1);
    
    /** @brief Appelé lorsque le contenu du Cbx_Etat change
     */
    void on_CBx_Etat_activated(int index);
    
    /** @brief Appelé lorsque le contenu du Cbx_Statut change
     *
     */
    void on_CBx_Statut_activated(int index);
    
    /** @brief Appelé lorsque le contenu du Cbx_Editeur change
     *
     */
    void on_CBx_Editeur_activated(int index);
    
    /** @brief Appelé lorsque le contenu du Cbx_Fournisseur change
     */
    void on_CBx_Fournisseur_activated(int index);
    
    /** @brief Appelé lorsque le contenu du LE_Createur change
     */
    void on_LE_Createur_textChanged(const QString &arg1);
    
    /** @brief Appelé lorsque le contenu du DtE_Achat change
     */
    void on_DtE_Achat_dateChanged(const QDate &date);
    
    /** @brief Appelé lorsque le contenu du SBx_PrixAchat change
     */
    void on_SBx_PrixAchat_valueChanged(double arg1);
    
    /** @brief Appelé lorsque le contenu du SBx_PrixLocation change
     */
    void on_SBx_PrixLocation_valueChanged(int arg1);
    
    /** @brief Appelé lorsque le contenu du SBx_Caution change
     */
    void on_SBx_Caution_valueChanged(int arg1);
    
    /** @brief Appelé lorsque le contenu du TxE_Remarques change
     */
    void on_TxE_Remarques_textChanged();
    
    /** @brief Appelé lorsque le contenu du TxE_Description change
     *
     */
    void on_TxE_Description_textChanged();
    
    /** @brief Appelé lorsque l'on clic sur une case du tableau
     */
    void on_TbV_Recherche_clicked(const QModelIndex &index);
    
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
    
    /** @brief Appelé lorsque le contenu du TxE_Contenu change
     */
    void on_TxE_Contenu_textChanged();
    
    /** @brief Appelé lorsque l'état du RBt_Neuf change
     */
    void on_RBt_Neuf_toggled(bool checked);
    
    /** @brief Appelé lorsque l'état du RBt_Occasion change
     */
    void on_RBt_Occasion_toggled(bool checked);
    
    void on_Bt_Renseigner_clicked();

    void on_Bt_ChargerImage_clicked();

    /** @brief Appelé lorsque le contenu du Cbx_Emplacement change
     */
    void on_CBx_Emplacement_activated(int index);

private:
    // ATTRIBUTs ----------------------------------------------------------------

    Ui::F_AjoutSuppModifJeux *ui;
    
    /** @brief Contient le code du jeu en cour de consultation
     *
     */
    //Code du jeu actif sur la fênentre
    QString JeuEnConsultation ;
    
    //! Model du TableView des membres
    QStandardItemModel * ModelJeu;
    
    /** @brief Pointeur qui fati le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pEmplacementAjMod;

    /** @brief Pointeur qui fai le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pStatutAjMod;

    /** @brief Pointeur qui fai le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pEtatAjMod;

    /** @brief Pointeur qui fai le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pClassificationAjMod;
    
    /** @brief Pointeur qui fai le lien entre la classe F_PopUpCLESTTEM et celle-ci
     */
    F_PopUpCLESTTEM * pMotCleAjMod;

    /** @brief Pointeur qui fai le lien entre la classe F_AjouterFournisseur et celle-ci
     */
    F_AjouterFournisseur * pAjoutFournisseur;

    /** @brief Pointeur qui fai le lien entre la classe F_AjouterEditeur et celle-ci
     */
    F_AjouterEditeur * pAjoutEditeur;

    /** @brief Pour gérer le jeu de cache cache des boutons Valider/Annuler/Ajouter/Supprimer
     */
    void CacherBoutons() ;
    
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

    // A voir si c'est utile ???
    QString CheminPhoto ;

};

//------------------------------------------------------------------------------
#endif // F_AJOUTSUPPMODIFJEUX_H
//------------------------------------------------------------------------------
