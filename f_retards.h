// F_Retards	0.1	06/2012	Padiou Nicolas

#ifndef F_RETARDS_H
#define F_RETARDS_H

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include <QWidget>
#include "Courriel.h"

namespace Ui {
    class F_Retards;
}

/** @struct IdMembreCourriel
 *  @brief Permet de faire le lien entre l'id d'un membre et l'adresse d'un objet Courriel
 */
typedef struct{
    int IdMembre ; ///< Correspond à l'id d'un membre
    Courriel * pCourriel ; ///< Correspond à l'adresse d'un objet Courriel
}IdMembreCourriel ;

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
     *
     */
    void MaJListe () ;

    /**
     * @brief Fait le lien entre l'id d'un membre et l'addresse de l'objet de type Courriel.
     *
     * @param pCourriel
     * @return int
     */
    int RetrouverIdMembre( Courriel * pCourriel ) ;


public slots :
    /**
     * @brief Permet de supprimer un objet dynamique de type Courriel une fois qu'il fait son travail
     *
     * @param pCourriel
     */
    void on_FermerThreadCourriel( Courriel * pCourriel ) ;

    /**
     * @brief Permet de supprimer un objet dynamique de type Courriel une fois que le mail est envoyé
     *
     * @param pCourriel
     */
    void on_ConfirmerMailEnvoyer( Courriel * pCourriel ) ;

    /**
     * @brief Afficher les erreurs qu'il peut y avoir lors de l'envoie d'un mail
     *
     * @param sErreur
     */
    void on_AfficherErreur( QString sErreur ) ;

private slots:
    /**
     * @brief Permet de selectionner toutes les lignes de la liste
     *
     */
    void on_PB_Selectionner_clicked();

    /**
     * @brief Permet l'envoie des mails vers les membres selectionner dans la liste
     *
     */
    void on_PB_Envoyer_clicked();

    /**
     * @brief  Permet de deselectionner toutes les lignes de la liste
     *
     */
    void on_PB_Deselectionner_clicked();

    /**
     * @brief Permet d'enregistrer le mail type
     *
     */
    void on_PB_Enregistrer_clicked();

    /**
     * @brief Permet d'annuler les edites sur le mail type
     *
     */
    void on_PB_Annuler_clicked();

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

private:
    Ui::F_Retards *ui;
    QVector<int> VecteurIdMembre ; ///< Vecteur contenant l'id des membres. Les id des membres sont dans le même ordre quand dans le tableau
    QVector<IdMembreCourriel> VecteurIdMembreCourriel ; ///< Vecteur fesant le lien entre l'id des membres et l'adresse d'un objet dynamique Courriel
    int nNombreEmailEnvoye ; ///< Contient le nombre de thread lancer pour l'envoie des mails
    int nNombreEmailEnCourDEnvoie ; ///< Contient le nombre d'email en cour d'envoie
};

#endif // F_RETARDS_H
