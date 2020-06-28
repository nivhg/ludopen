// F_Retards	0.1	06/2012	Padiou Nicolas
// @bug   2014-04-20  <William>  <Remise aux normes des noms d'IHM>

#ifndef F_RETARDS_H
#define F_RETARDS_H

// En-têtes propres à l'application necessaires dans ce fichier en-tête --------
#include <QtWidgets>
#include "Courriel.h"

namespace Ui {
    class F_Retards;
}

/** @struct IdMembreCourriel
 *  @brief Permet de faire le lien entre l'id d'un membre et l'adresse d'un objet Courriel
 */
typedef struct
{
    uint IdMembre ; ///< Correspond à l'id d'un membre
    Courriel * pCourriel ; ///< Correspond à l'adresse d'un objet Courriel
} IdMembreCourriel ;

/**
 * @brief Permet la gestion de l'envoie de mail lors de retard
 *
 *L'utilisateur peut envoyer un mail à un membre s'il a n'a pas rendu le jeu à la date prévu.
 *Il peut définir le mail type qui sera envoyé.
 */
class F_Retards : public QWidget
{
    Q_OBJECT

public:
    // METHODEs -----------------------------------------------------------------

    /**
     * @brief Permet d'initialiser les différents champs et tableau.
     *
     * @param parent
     */
    explicit F_Retards(QWidget *parent = 0);
    /**
     * @brief Permet de détruire les objets dynamique
     *
     */
    ~F_Retards();

    /**
     * @brief Fait une mise à jour de la liste des membres qui ont au moins un jeu en retard
     */
    void MaJListe () ;
    /**
     * @brief Permet de désactiver les champs de l'onglet
     */
    void DesactiverChamps(bool Etat);


signals:
    //! Signal indiquand qu'un double clic a eu lieu dans la liste des membres en retard
    void Signal_DoubleClic_ListeMembres( uint IdMembre) ;

public slots :
    /**
     * @brief Permet de supprimer un objet dynamique de type Courriel une fois qu'il fait son travail
     *
     * @param pCourriel
     */
    void slot_FermerThreadCourriel() ;

    /**
     * @brief Permet de supprimer un objet dynamique de type Courriel une fois que le mail est envoyé
     *
     * @param pCourriel
     */
    void slot_ConfirmerMailEnvoyer( uint IDMembre ) ;

    /**
     * @brief Afficher les erreurs qu'il peut y avoir lors de l'envoie d'un mail
     *
     * @param sErreur
     */
    void slot_AfficherErreurMail( QString sErreur ) ;


private slots :
    /**
     * @brief Permet de selectionner toutes les lignes de la liste
     *
     */
    void on_Bt_Selectionner_clicked();

    /**
     * @brief Permet l'envoie des mails vers les membres selectionner dans la liste
     *
     */
    void on_Bt_Envoyer_clicked();

    /**
     * @brief  Permet de deselectionner toutes les lignes de la liste
     *
     */
    void on_Bt_Deselectionner_clicked();

    /**
     * @brief Permet d'enregistrer le mail type
     *
     */
    void on_Bt_Enregistrer_clicked();

    /**
     * @brief Permet d'annuler les edites sur le mail type
     *
     */
    void on_Bt_Annuler_clicked();

    /**
     * @brief Déverrouille les boutons enregistrer et annuler quand le PlainTextEdit est edité
     *
     */
    void on_TE_Corps_textChanged();

    /**
     * @brief Déverrouille les boutons enregistrer et annuler quand le LineEdit est edité
     *
     * @param arg1
     */
    void on_LE_Sujet_textChanged(const QString &arg1);

    void on_TbW_Retards_doubleClicked(const QModelIndex &index);

    void on_Bt_Exporter_clicked();

private:
    Ui::F_Retards *ui;

    /// Contient le nombre de thread lancé pour l'envoi des emails
    uint NbEmailAEnvoyer ;
    /// Contient le nombre d'emails en cour d'envoi
    uint NbEmailsDejaEnvoyes ;
    /// Modèle pour l'affichage dans le tableau des retards
    QStandardItemModel modeleTabRetard ;
    /// vecteur qui contient la liste des emails à envoyer
    QList <EMail> ListeEMailAEnvoyer ;
    /// le thread qui gère l'envoie des emails
    Courriel * pCourriel ;
    std::vector<QStandardItem*> Items;      //! Liste des checkboxes dans la combobox d'export
    QThread *t;
};

#endif // F_RETARDS_H
