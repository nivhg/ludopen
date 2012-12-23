// f_poupcode.h	1.0	<06/2012>	<NORMAND Julien>
#ifndef F_POPUPCODE_H
#define F_POPUPCODE_H

// En-têtes standards necessaires dans ce fichier en-tête seulement ------------
#include <QWidget>
#include <QtSql>
#include <QtGui>

// En-têtes propres à  l'application necessaires dans ce fichier en-tête --------

namespace Ui {
class F_PopUpCode;
}

/**
 *  @brief  F_PopUpCode permet la gestion de la partie "Administration" en ayant un code.
 *
 */
class F_PopUpCode : public QWidget
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
    explicit F_PopUpCode(QWidget *parent = 0);

    /**
     *  @brief  Destructeur de la classe.
     *
     */
    ~F_PopUpCode();

signals:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief
     *
     */
    void SignalOnglet();
    
private slots:
    // METHODEs ----------------------------------------------------------------
    /**
     *  @brief Permet de valider ou non le code d'accès administrateur.
     *
     */
    void on_Bt_OK_clicked();

    /**
     *  @brief Ferme la fenêtre F_PopUpCode et ré-affiche le premier onglet.
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

private:
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::F_PopUpCode *ui;
};

//------------------------------------------------------------------------------
#endif // F_POPUPCODE_H
//------------------------------------------------------------------------------
