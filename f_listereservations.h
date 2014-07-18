#ifndef F_LISTERESERVATIONS_H
#define F_LISTERESERVATIONS_H

// En-ttes propres  l'application necessaires dans ce fichier en-tete --------
#include <QtSql/QtSql>  //Nécessaire pour la base de données

#include <QtWidgets>
#include <QtWidgets\QTableView>
#include <QtWidgets\QPushButton>
#include <QtWidgets\QLabel>
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
    int EmplacementStatutDuJeuVector( unsigned int nIdStatutDuJeu ) ;

    //! Met à jour la liste des StatutDuJeus depuis la base de données
    void MiseAJourStatutJeu () ;

    //! Met à jour la liste des Lieuxs depuis la base de données
    void MaJLieux () ;

    bool AffichageListe () ;

signals:
    void Signal_DoubleClic_ListeMembres( unsigned int nIdMembre ) ;
    
private slots:
    void on_Bt_RAZ_clicked();

    void on_Bt_Deselection_clicked();

    void on_ChBx_LieuxDeReservation_clicked();

    void on_CBx_LieuxDeReservation_activated(int index);

    void on_ChBx_LieuxDeRetrait_clicked();

    void on_ChBx_StatutJeu_clicked();

    void on_CBx_StatutJeu_currentIndexChanged(int index);

    void on_ChBx_NbreRetard_clicked();

    void on_CBx_Retards_currentIndexChanged(int index);

    void on_SBx_NbreRetard_valueChanged(int arg1);

    //void on_ChBx_Retard_clicked();

    void on_ChBx_DateReservation_clicked();

    void on_DtE_DateResa_Debut_dateChanged(const QDate &date);

    void on_DtE_DateResa_Fin_dateChanged(const QDate &date);

    void on_ChBx_DateEmpruntPrevue_clicked();

    void on_DtE_DateEmpruntPrevue_Debut_dateChanged(const QDate &date);

    void on_DtE_DateEmpruntPrevue_Fin_dateChanged(const QDate &date);

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
    QVector<Type> VectorLieux ;                 //! Vecteur contenant la liste des Lieux (id, nom)
    QVector<StatutDuJeu> VectorStatutDuJeu ;    //! Vecteur contenant la liste des titres (id, nom)
    QVector<int> VecteurListeReservations ;     //! Vecteur conteant la liste des id des réservation
};

#endif // F_LISTERESERVATIONS_H
