#ifndef F_HISTORIQUEJEUX_H
#define F_HISTORIQUEJEUX_H

// En-ttes standards necessaires dans ce fichier en-tete seulement ------------
#include <QWidget>
#include <iostream>
#include <QtSql/QtSql>
#include <QStandardItemModel>

namespace Ui {
    class F_HistoriqueJeux;
}

/** @brief Permet l'affichage des jeux empruntes puis rendu
 *
 */

class F_HistoriqueJeux : public QWidget
{
    Q_OBJECT

public:
    // CONSTRUCTEURs et DESTRUCTEURs
    /** Initialisation de la classe
     *  @test   Voir la procédure dans le fichier associĂŠ.
     */
    explicit F_HistoriqueJeux(QWidget *parent = 0);
    ~F_HistoriqueJeux();

    // METHODEs -----------------------------------------------------------------
    //! Permet l'affichage l'historique des emprunts du membre selectionne
    void AfficherHistorique ( unsigned int nIdMembre ) ;

private slots:
    //! Permet de quitter la fenêtre
    void on_PB_Quitter_clicked();

private:
    Ui::F_HistoriqueJeux *ui;
};

#endif // F_HISTORIQUEJEUX_H
