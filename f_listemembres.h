#ifndef F_LISTEMEMBRES_H
#define F_LISTEMEMBRES_H

// En-ttes propres  l'application necessaires dans ce fichier en-tete --------
#include <QtSql/QtSql>  //Nécessaire pour la base de données

#include <QtWidgets>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QStandardItem>
#include "TitreType.h"

namespace Ui {
class F_ListeMembres;
}

class F_ListeMembres : public QWidget
{
    Q_OBJECT
    
public:
    explicit F_ListeMembres(bool bAdmin = false, QWidget *parent = 0) ;
    ~F_ListeMembres() ;

    void DecocherTout( bool bSelection) ;

    //! Recupere l'emplacement d'un titre dans un vecteur en fonction de son id
    int EmplacementTypeVector( unsigned int nIdType ) ;

    //! Recupere l'emplacement d'un type dans un vecteur en fonction de son id
    int EmplacementTitreVector( unsigned int nIdTitre ) ;

    //! Met à jour la liste des Titres depuis la base de données
    void MaJTitre () ;

    //! Met à jour la liste des Types depuis la base de données
    void MaJType () ;

    //! Affiche le résultat du filtrage
    bool AffichageListe () ;

    //! Efface tous ce qui n'est pas un chiffre et ajoute un espace tous les 2 caractéres dans un string
    QString ModifierSyntaxeNumTelephone ( QString sNum) ;

signals:
    void Signal_DoubleClic_ListeMembres( unsigned int nIdMembre ) ;

private slots:
    void on_Bt_RAZ_clicked();

    void on_Bt_ToutDecocher_clicked();

    void on_ChBx_Type_clicked();

    void on_CBx_Type_currentIndexChanged(int index);

    void on_ChBx_Nom_clicked();

    void on_LE_Nom_textEdited(const QString &arg1);

    void on_ChBx_Prenom_clicked();

    void on_LE_Prenom_textEdited(const QString &arg1);

    void on_ChBx_Ville_clicked();

    void on_LE_Ville_textEdited(const QString &arg1);

    void on_ChBx_CodePostal_clicked();

    void on_LE_CodePostal_textEdited(const QString &arg1);

    void on_ChBx_Titre_clicked();

    void on_CBx_Titre_currentIndexChanged(int index);

    void on_ChBx_NbreRetard_clicked();

    void on_CBx_Retards_currentIndexChanged(int index);

    void on_SBx_NbreRetard_valueChanged(int arg1);

    //void on_ChBx_Retard_clicked();

    void on_ChBx_AncienMembre_clicked();

    void on_ChBx_Abonnements_clicked();

    void on_ChBx_DateInscription_clicked();

    void on_DtE_DI_Debut_dateChanged(const QDate &date);

    void on_DtE_DI_Fin_dateChanged(const QDate &date);

    void on_ChBx_DateNaissance_clicked();

    void on_DtE_DN_Debut_dateChanged(const QDate &date);

    void on_DtE_DN_Fin_dateChanged(const QDate &date);

    void on_ChBx_Cotisation_clicked();

    void on_CBx_Cotisation_currentIndexChanged(int index);

    void on_Bt_SelectionListe_clicked();

    void on_Bt_DeselectionListe_clicked();

    void on_Bt_SupprimerListe_clicked();

    void on_TbW_ListeMembre_clicked(const QModelIndex &index);

    void on_TbW_ListeMembre_doubleClicked(const QModelIndex &index);

    void on_Bt_Exporter_clicked();

    void on_ChBx_Activite_clicked();

    void on_CBx_Activites_currentIndexChanged(int index);

    void on_ChBx_Retard_clicked();

private:
    Ui::F_ListeMembres *ui;

    bool bAdmin ;

    QVector<Type> VectorType ;               //! Vecteur contenant la liste des types (id, nom)
    QVector<Titre> VectorTitre ;             //! Vecteur contenant la liste des titres (id, nom)

    QStandardItemModel ModeleMembres ;

    QDialog * pFenetreAbonnements ;                     //! Fenêtre
    QTableView * pListeAbonnements ;        //! l
    QLabel * pLb_Abonnements ;                   //! LigneEdit permettant
    QPushButton * pBt_FermerAbonnements ;          //! Bouton
    std::vector<QStandardItem*> Items;      //! Liste des checkboxes dans la combobox d'export
};

#endif // F_LISTEMEMBRES_H
