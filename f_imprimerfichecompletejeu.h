//f_imprimerfichecompletejeu.h	0.1	<Mary Florian>
#ifndef F_IMPRIMERFICHECOMPLETEJEU_H
#define F_IMPRIMERFICHECOMPLETEJEU_H

class  F_ImprimerFicheCompleteJeu ;

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QDialog>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "f_mainwindow.h"

namespace Ui {
    class F_ImprimerFicheCompleteJeu;
}

/** @brief Classe qui permet d'imprimer une fiche de jeu
 *
 * Cette classe permet d'imprimer une fiche de jeu,
 * elle contient toutes les informations sur le jeu.
 *
 */
class F_ImprimerFicheCompleteJeu : public QDialog
{
    Q_OBJECT

public:
    explicit F_ImprimerFicheCompleteJeu(QWidget *parent = 0);
    ~F_ImprimerFicheCompleteJeu();
    // METHODEs -----------------------------------------------------------------
    /** @brief Permet d'imprimer la fiche du jeu, récupère en argument le code du jeu en consultation
     *
     */
    void ImprimerFicheJeuComplete(QString CodeJeu) ;

private slots:
    // METHODEs -----------------------------------------------------------------
    /** @brief Appeler lorsque l'on clic sur le bouton Imprimer
     *
     */
    void on_Bt_Imprimer_clicked();

    /** @brief Appeler lorsque l'on clic sur le bouton Quitter
     *
     */
    void on_Bt_Quitter_clicked();

private:
    // ATTRIBUTs ----------------------------------------------------------------
    Ui::F_ImprimerFicheCompleteJeu *ui;
};

//------------------------------------------------------------------------------
#endif // F_IMPRIMERFICHECOMPLETEJEU_H
//------------------------------------------------------------------------------
