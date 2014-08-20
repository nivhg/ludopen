// f_jeux.h    0.1     <2012-01-01>       <Florian MARY>
#ifndef F_JEUX_H
#define F_JEUX_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QtWidgets>
#include <QStandardItemModel>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "lb_image.h"
#include "f_declarerintervention.h"
#include "f_detailsjeux.h"
#include "f_reservation.h"

using namespace std;

namespace Ui {
    class F_Jeux;
}


/** @brief Classe qui permet de consulter une fiche de jeu
 *
 * Cette classe permet de consulter une fiche de jeu,
 * elle contient toutes les informations sur le jeu.
 *
 */
class F_Jeux : public QWidget
{
    Q_OBJECT

public:
    // CONSTRUCTEUR ET DESTRUCTEUR -------------------------
    explicit F_Jeux(QWidget *parent = 0);
    ~F_Jeux(); 
    
    //METHODE----------------------------------------------------
    /** @brief Permet d'actualiser le jeux
     */
    void ActualiserJeux() ;
    /** @brief Permet de récuperer le code du jeu en consultation
     */
    QString get_JeuEnConsultation() ;
    /** @brief Permet de fixer le code du jeu en consultation
     */
    void set_JeuEnConsultation(QString CodeJeuChoisi);

    /** @brief Affiche les information sur un jeu
     */
    void AfficherJeu();

    /** @brief Affiche tous les jeux dans le tableau
     */
    void AfficherJeux();

private slots:
    // METHODES---------------------------------------------------

    /** @brief Appeler lorsque l'on clic sur le bouton Declarer intervention
     *
     */
    void on_Bt_DeclarerIntervention_clicked();

    /** @brief Appeler lorsque l'on modifier le contenu du champ remarques
     *
     */
    void on_TxE_remarques_textChanged();

    /** @brief Appeler lorsque l'on valide la nouvelle remarque
     *
     */
    void on_Bt_ValiderRemarques_clicked();

    /** @brief Appeler lorsque l'on annule la nouvelle remarque
     *
     */
    void on_Bt_AnnulerRemarques_clicked();

    /** @brief Appeler lorsque l'on modifie le contenu du champ description
     *
     */
    void on_TxE_description_textChanged();

    /** @brief Appeler lorsque l'on valide la nouvelle description
     *
     */
    void on_Bt_ValiderDescription_clicked();

    /** @brief Appeler lorsque l'on modifie le contenu du champ contenu
     *
     */
    void on_TxE_contenu_textChanged();

    /** @brief Appeler lorsque l'on valide le nouveau contenu
     *
     */
    void on_Bt_ValiderContenu_clicked();

    /** @brief Appeler lorsque l'on annuler le nouveau contenu
     *
     */
    void on_Bt_AnnulerContenu_clicked();

    /** @brief Appeler lorsque l'on annule la nouvelle description
     *
     */
    void on_Bt_AnnulerDescription_clicked();

    /** @brief Appeler lorsque l'on clic sur le bouton Valider
     *
     */
    void on_Bt_Reserver_clicked();

    /** @brief Appeler lorsque l'on clic sur le bouton Détails
     *
     */
    void on_Bt_details_clicked();

    /** @brief Appeler lorsque l'on modifie le contenu de la case de recherche par nom
     *
     */
    void on_Le_recherchenom_textChanged(const QString &arg1);

    /** @brief Appeler lorsque l'on clic sur le bouton règle du jeu
     *
     */
    void on_Bt_regle_clicked();
    
    /** @brief Appeler lorsque l'on clic sur une case du tableau
     *
     */
    void on_TbV_NomJeux_clicked(const QModelIndex &index);    

    void on_Le_nom_textChanged(const QString &arg1);

    void on_Bt_Gauche_clicked();

    void on_Bt_Droite_clicked();

    void on_Lb_Image_clicked();

private:
    //ATTRIBUTS--------------------------------------------------
    Ui::F_Jeux *ui;
    
    //! Pointeur vers la fenêtre réservation
    F_Reservation * pReservation;
    
    //! Pointeur vers la fenêtre détails jeux
    F_DetailsJeux * pDetailsJeux;
    
    //! Pointeur vers la fenêtre déclarer intervention
    F_DeclarerIntervention * pDeclarerIntervention;
    
    //! Code du jeu actif sur la fenêtre
    QString JeuEnConsultation ;

    //! Model du TableView des jeux
    QStandardItemModel * ModelJeu;

    //! Label Image
    Lb_Image* lb_image;

    //! Stocke l'ID du jeu sélectionné
    QString nIdJeuSelectionne;

    //! Stocke les chemins des fichiers de règles
    QStringList sCheminFichier;
};

//-----------------------------------------------------------------
#endif // F_JEUX_H
//-----------------------------------------------------------------
