// F_AdministrerMembres.h	0.1	26/04/2012  Padiou Nicolas
#ifndef F_ADMINISTRERMEMBRES_H
#define F_ADMINISTRERMEMBRES_H


// En-ttes standards necessaires dans ce fichier en-tete seulement ------------
#include "f_recherchemembres.h"
#include "f_ajoutertitretype.h"
// En-ttes propres  l'application necessaires dans ce fichier en-tete --------
#include <QWidget>
#include "f_ajoutertitretype.h"

namespace Ui {
class F_AdministrerMembres;
}

/** @brief Permet l'administration des membres(consultation, ajout, modification et suppression)
 *
 *  Permet la consultation d'un membre.
 *  Permet la suppression d'un membre.
 *  Permet la modification d'un membre.
 *  Permet la suppression d'un membre.
 */
class F_AdministrerMembres : public QWidget
{
    Q_OBJECT
    
public:
    //! Initialisation de la classe
    explicit F_AdministrerMembres(F_RechercheMembres * pRechercheMembres, QWidget *parent = 0);

    //! Destructeur
    ~F_AdministrerMembres();

// METHODEs -----------------------------------------------------------------

    //! Recupere l'emplacement d'un titre dans un vecteur en fonction de son id
    int EmplacementTitreVector (unsigned int nIdTitre) ;
    //! Recupere l'emplacement d'un type dans un vecteur en fonction de son id
    int EmplacementTypeVector(unsigned int nIdType) ;

    //! Permet d'ajouter un membre
    bool AjouterMembre () ;

    //! Permet de modifier un membre
    bool ModifierMembre ( unsigned int nIdMembre ) ;

    //! Met à jour la liste des Titres depuis la base de données
    void MaJTitre () ;

    //! Met à jour la liste des Types depuis la base de données
    void MaJType () ;

    //! Affiche les jeux empruntés dans le tableau TbW_Emprunt
    void AfficherJeuxEmpruntes (unsigned int nIdMembre) ;

    //! Affiche les informations d'un membre
    void AfficherMembre ( unsigned int nIdMembre ) ;

    //! Permet de supprimer un membre
    bool SupprimerMembre(int IdMembre) ;


    
private slots:
    void on_commandLinkButton_3_clicked();

private:
    Ui::F_AdministrerMembres *ui; ///< Créer par Qt

    F_RechercheMembres * pRechercheMembres ;///< Pointe sur l'objet F_RechercheMembre

    QVector<Type> VectorType ;               //! Vecteur contenant la liste des types (id, nom)
    QVector<Titre> VectorTitre ;             //! Vecteur contenant la liste des titres (id, nom)

    F_AjouterTitreType * pAjouterType ;      //! Pointeur sur la classe F_AjouterTitreType avec comme paramètre (0)
    F_AjouterTitreType * pAjouterTitre ;     //! Pointeur sur la classe F_AjouterTitreType avec comme paramètre (1)

    unsigned int nIdMembreSelectionne ;      //! Permet de savoir quelle membre est sélectionné
};

//------------------------------------------------------------------------------
#endif // F_ADMINISTRERMEMBRES_H
//------------------------------------------------------------------------------
