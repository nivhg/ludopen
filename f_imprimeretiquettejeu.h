//f_imprimeretiquettejeu.h	0.1	<Mary Florian>
#ifndef F_IMPRIMERETIQUETTEJEU_H
#define F_IMPRIMERETIQUETTEJEU_H

class F_ImprimerEtiquetteJeu ;

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QDialog>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include <f_mainwindow.h>

namespace Ui {
    class F_ImprimerEtiquetteJeu;
}

/** @brief Classe qui permet d'imprimer une étiquette de jeu
 *
 * Cette classe permet d'imprimer une étiquette de jeu,
 * elle contient le informations sur la ludothèque, le nom et le code du jeu.
 *
 */
class F_ImprimerEtiquetteJeu : public QDialog
{
    Q_OBJECT

public:
    explicit F_ImprimerEtiquetteJeu(QWidget *parent = 0);
    ~F_ImprimerEtiquetteJeu();

    // METHODEs -----------------------------------------------------------------
    /** @brief Permet d'imprimer l'étiquette du jeu, récupère en argument le code du jeu en consultation
     *
     */
    void ImprimerEtiquetteJeu(QString CodeJeu) ;

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
    Ui::F_ImprimerEtiquetteJeu *ui;
};

//------------------------------------------------------------------------------
#endif // F_IMPRIMERETIQUETTEJEU_H
//------------------------------------------------------------------------------




