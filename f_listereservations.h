#ifndef F_LISTERESERVATIONS_H
#define F_LISTERESERVATIONS_H

// En-ttes propres  l'application necessaires dans ce fichier en-tete --------
#include <QtSql/QtSql>  //Nécessaire pour la base de données

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QLabel>
#include <QStandardItem>
#include "TitreType.h"

namespace Ui {
class F_ListeReservations;
}

class F_ListeReservations : public QWidget
{
    Q_OBJECT
    
public:
    explicit F_ListeReservations(QWidget *parent = 0) ;
    ~F_ListeReservations() ;

    void TousSelectionner( bool bSelection) ;

    //! Recupere l'emplacement d'un titre dans un vecteur en fonction de son id
    int EmplacementLieuxVector( unsigned int nIdLieux ) ;

    //! Recupere l'emplacement d'un type dans un vecteur en fonction de son id
    int EmplacementTitreVector( unsigned int nIdTitre ) ;

    //! Met à jour la liste des Titres depuis la base de données
    void MaJTitre () ;

    //! Met à jour la liste des Lieuxs depuis la base de données
    void MaJLieux () ;

    bool AffichageListe () ;

signals:
    void SignalSelectionMembre( unsigned int nIdMembre ) ;
    
private slots:
    void on_Bt_RAZ_clicked();

    void on_Bt_Deselection_clicked();

    void on_ChBx_Lieux_clicked();

    void on_CBx_Lieux_activated(int index);

    void on_ChBx_Nom_clicked();

    void on_LE_Nom_textEdited(const QString &arg1);

    void on_ChBx_Prenom_clicked();

    void on_LE_Prenom_textEdited(const QString &arg1);

    void on_ChBx_Titre_clicked();

    void on_CBx_Titre_currentIndexChanged(int index);

    void on_ChBx_NbreRetard_clicked();

    void on_CBx_Retards_currentIndexChanged(int index);

    void on_SBx_NbreRetard_valueChanged(int arg1);

    void on_ChBx_Retard_clicked();

    void on_ChBx_DateReservation_clicked();

    void on_DtE_DateResa_Debut_dateChanged(const QDate &date);

    void on_DtE_DateResa_Fin_dateChanged(const QDate &date);

    void on_ChBx_DateNaissance_clicked();

    void on_DtE_DN_Debut_dateChanged(const QDate &date);

    void on_DtE_DN_Fin_dateChanged(const QDate &date);

    void on_ChBx_Cotisation_clicked();

    void on_CBx_Cotisation_currentIndexChanged(int index);

    void on_Bt_SupprimerListe_clicked();

    void on_Bt_Exporter_clicked();

    void on_TbW_ListeReservations_clicked(const QModelIndex &index);

    void on_TbW_ListeReservations_doubleClicked(const QModelIndex &index);

private:
    Ui::F_ListeReservations *ui;
    // Modèle pour l'affichage et le tri du tableau des réservations
    QStandardItemModel ModeleReservations ;
    QVector<Type> VectorLieux ;                  //! Vecteur contenant la liste des Lieux (id, nom)
    QVector<Titre> VectorTitre ;                //! Vecteur contenant la liste des titres (id, nom)
    QVector<int> VecteurListeReservations ;     //! Vecteur conteant la liste des id des membres  
};

#endif // F_LISTERESERVATIONS_H