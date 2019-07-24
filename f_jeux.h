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
#include "spinboxdelegate.h"
#include "f_ajoutsuppmodifjeux.h"

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

    /** @brief Affiche tous les jeux dans le tableau
     */
    void AfficherJeux();

    /** @brief Actualiser la liste des liens du jeu
     */
    void ActualiserLienJeux();

    void ActualiserContenu();

public slots:
    /** @brief Affiche les information sur un jeu
     */
    void AfficherJeu(QString Jeu);

private slots:
    // METHODES---------------------------------------------------

    /** @brief Appeler lorsque l'on clic sur le bouton Declarer intervention
     *
     */
    void on_Bt_DeclarerIntervention_clicked();

    /** @brief Appeler lorsque l'on modifie le contenu du champ description
     *
     */
    void on_TxE_description_textChanged();

    /** @brief Appeler lorsque l'on valide la nouvelle description
     *
     */
    void on_Bt_ValiderDescription_clicked();

    /** @brief Appeler lorsque l'on annule la nouvelle description
     *
     */
    void on_Bt_AnnulerDescription_clicked();

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

    void SlotRegleTelecharger(QString CheminFichier);

    void on_TbW_LiensJeux_clicked(const QModelIndex &index);

signals:
    //! Signal indiquant qu'un clic a eu lieu sur le bouton réservé
    void Signal_Clic_Reserve( int IdReservation);

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

    //! Id de la réservation
    int IdReservation ;

    //! Model du TableView des jeux
    QStandardItemModel * ModelJeu;

    //! Label Image
    Lb_Image* lb_image;

    //! Stocke les chemins des fichiers de règles
    QString sCheminFichier;

    AccesFichierParHTTP * acces;

    //! modèle du TreeView du contenu des jeux
    QStandardItemModel * ModeleContenu;

    //! Choix du menu ajouter pour créer une pièce dans un groupe
    QAction *actionPieceDsGroupe;

    SpinBoxDelegate *DelegateContenu;
};

//-----------------------------------------------------------------
#endif // F_JEUX_H
//-----------------------------------------------------------------
