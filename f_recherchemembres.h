// MaClasse.h	0.1	01/2012 PADIOU Nicolas

#ifndef F_RECHERCHEMEMBRES_H
#define F_RECHERCHEMEMBRES_H

// En-ttes standards (ATTENTION : garder toujours le meme ordre) --------------
#include <QtSql/QtSql>

// En-ttes standards necessaires dans ce fichier en-tete seulement ------------
#include <QWidget>
#include <QStandardItem>
#include <iostream>
#include <QVector>

using namespace std ;

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
class F_RechercheMembres;
}


/** @brief  Permet la sélection et la recherche de membre via une liste.
 * Permet la sélection d'un membre avec la saisie de son code membre.
 *
 *  Description dtaille de MaClasse. Description dtaille de MaClasse.
 *  Description dtaille de MaClasse. Description dtaille de MaClasse.
 *  <p>
 *  Autre bloc de description dtaille de MaClasse. Autre bloc de description dtaille de MaClasse.
 *  Autre bloc de description dtaille de MaClasse. Autre bloc de description dtaille de MaClasse.
 */
class F_RechercheMembres : public QWidget
{
    Q_OBJECT
    
public:

    //CONSTRUCTEUR ET DESTRCTEUR

    //! Initialisation des variables et du trableau TbV_ListeMembre
    explicit F_RechercheMembres(QWidget *parent = 0);

    //! Suppression du contenu des variables dynamiques
    ~F_RechercheMembres();


    //METHODES-------------------------------------------------------------------

    //! Mise ŕ jour de la liste des membres
    bool MaJListeMembres ();

    //! Affichage de la liste des membres
    void AfficherListe() ;//Liste par défaut(this->VecteurMembres)
    void AfficherListe(QVector<Membre> VecteurMembres ) ;//Liste donnée

    //! Recherche soit avec le nom, soit avec le numéro ou avec les 2
    void RechercherParNomEtNumero () ;

    //! Renvoie le premier code non utilisé
    int RecupererProchainCodeNonUtilise () ;

    //! Retourne le membre Selectionné dans le TableView
    int RecupererMembreSelectionne () ;

signals:
   void SignalRenvoieIdMembre (unsigned int) ;

private slots:

    //! Selectionne un membre avec un double click
    void on_TbW_Recherche_doubleClicked(const QModelIndex &index);

    //! Selectionne un membre avec un click
    void on_TbW_Recherche_clicked(const QModelIndex &index);

    //! Recherche les membres correspondant au champs à  chaque fois que le champs est modifiée
    void on_LE_Nom_textEdited(const QString &arg1);

    //! Non utilisé
    void on_LE_Nom_textChanged(const QString &arg1);

    //! Recherche les membres correspondant au champs à  chaque fois que le champs est modifiée
    void on_LE_Code_textEdited(const QString &arg1);

    //! Envoie un signal avec l'id du membre sélectionner avec le code membre quand ok appuyé
    void on_PB_Ok_clicked();

    //! Envoie un signal avec l'id du membre sélectionner avec le code membre quand la toucher entrée appuyée
    void on_LE_Code_returnPressed();

    //! Non utilisé
    void on_TbW_Recherche_pressed(const QModelIndex &index);

    //! Non utilisé
    void on_TbW_Recherche_activated(const QModelIndex &index);

    //! Non utilisé
    void on_TbW_Recherche_entered(const QModelIndex &index);

private:
    // ATTRIBUTs ----------------------------------------------------------------
    Ui::F_RechercheMembres *ui;
    QVector<Membre> VecteurMembres ;///< Vecteur contenant la liste des membres
    QVector<Membre> VecteurRechercheMembres ; ///< Vecteur contenant la liste des membres correspondant à  la recherche
};

#endif // F_RECHERCHEMEMBRE_H
