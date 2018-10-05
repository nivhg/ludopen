// f_poupcode.h	1.0	<06/2012>	<NORMAND Julien>
#ifndef D_POPUPCODE_H
#define D_POPUPCODE_H

// En-têtes standards necessaires dans ce fichier en-tête seulement ------------
#include <QDialog>
#include <QtDebug>
#include <QtWidgets>
#include <QtSql>
//using namespace std ;

#include "f_preferences.h"

// En-têtes propres à  l'application necessaires dans ce fichier en-tête --------

namespace Ui {
class D_PopUpCode;
}

/**
 *  @brief  D_PopUpCode permet la gestion de la partie "Administration" en ayant un code.
 *
 */
class D_PopUpCode : public QDialog
{
    Q_OBJECT
    
public:
    // METHODEs ----------------------------------------------------------------
    // CONSTRUCTEURs et DESTRUCTEURs
    /**
     *  @brief  Constructeur de la classe.
     *
     *  @param  parent
     */
    explicit D_PopUpCode(QWidget *parent = 0);

    /**
     *  @brief  Destructeur de la classe.
     *
     */
    ~D_PopUpCode();

signals:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief
     *
     */
    void SignalOnglet();
    void SignalMembreIdentifier(uint iIdMembre);

private slots:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Permet de valider ou non le code d'accès administrateur.
     *
     */
    void on_Bt_OK_clicked();

    /**
     *  @brief Ferme la fenêtre D_PopUpCode et ré-affiche le premier onglet.
     *
     */
    void on_Bt_Annuler_clicked();

    /**
     *  @brief  Verrouille le bouton Bt_OK si le champs est vide.
     *
     *  @param  arg1
     */
    void on_LE_Code_textEdited(const QString &arg1);

    void on_LE_Code_returnPressed();

    void on_CBx_Benevole_currentIndexChanged(int index);

private:
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::D_PopUpCode *ui;
    void closeEvent(QCloseEvent *event);
    bool enableClose;
};

//------------------------------------------------------------------------------
#endif // D_PopUpCode_H
//------------------------------------------------------------------------------
