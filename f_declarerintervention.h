//f_declarerintervention.h	0.1	<Mary Florian>
#ifndef F_DECLARERINTERVENTION_H
#define F_DECLARERINTERVENTION_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QWidget>

namespace Ui {
    class F_DeclarerIntervention;
}

/** @brief Classe qui permet de déclarer une intervention sur un jeu
 *
 * Cette classe permet de déclarer une intervention sur le jeu en consultation
 *
 */
class F_DeclarerIntervention : public QWidget
{
    Q_OBJECT

public:
    explicit F_DeclarerIntervention(QWidget *parent = 0);
    ~F_DeclarerIntervention();

    // METHODEs -----------------------------------------------------------------
    /** @brief Permet de définir le jeu en consultation et récupère en argument le code de ce jeu
     *
     */
    void set_JeuEnConsultation(QString CodeJeu) ;
private slots:

    // METHODEs -----------------------------------------------------------------
    /** @brief Appelé lorsque l'on clic sur le bouton Valider l'intervention
     *
     */
    void on_Bt_ValiderIntervention_clicked();

    /** @brief Appelé lorsque l'on clic sur le bouton Annuler l'intervention
     *
     */
    void on_Bt_AnnulerIntervention_clicked();
    


private:
    // ATTRIBUTs ----------------------------------------------------------------
    Ui::F_DeclarerIntervention *ui;
    /** @brief Récupère le code du jeu en consultation
     *
     */
    QString JeuEnConsultation ;
};

//------------------------------------------------------------------------------
#endif // F_DECLARERINTERVENTION_H
//------------------------------------------------------------------------------
