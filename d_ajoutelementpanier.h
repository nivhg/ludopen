//f_panier.h 0.1	<Vincent VICTORIN>
#ifndef D_AJOUTELEMENTPANIER_H
#define D_AJOUTELEMENTPANIER_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QScriptEngine>

#include "fonctions_globale.h"
#include "searchbox.h"
#include "f_panier.h"

namespace Ui {
    class D_AjoutElementPanier;
}

/** @brief Classe qui permet de gérer le panier
 *
 * Cette classe permet de gérer le panier
 *
 */
class D_AjoutElementPanier : public QDialog
{
    Q_OBJECT
public:
    explicit D_AjoutElementPanier(QWidget *parent = nullptr,int IdDuMembre=0);
    ~D_AjoutElementPanier();

signals:
    //! Signal emit lors de l'ajout au panier d'un abonnement
    bool Signal_AjouterAuPanier(QString Titre,uint nIDMembre,double Prix,int IdVentilation,QString Table,QList<QSqlQuery *> *requete);

public slots:

private slots:
    void on_Bt_Ajouter_clicked();

    void on_Le_Description_textChanged(const QString &arg1);

    void on_Bt_Annuler_clicked();

    void SearchMembre_MembreTrouve();

    void on_CBx_ChoixPrerempli_currentIndexChanged(int index);

    void on_SBx_Champs1_valueChanged(int arg1);

    void on_SBx_Champs2_valueChanged(int arg1);


private:
    //! Liste des membres
    QVector<QVector <QString> > VecteurMembres;
    // ATTRIBUTs ----------------------------------------------------------------
    ///< Pointeur de la classe graphique.
    Ui::D_AjoutElementPanier *ui;
    //! LineEdit SearchBox pour la recherche de membre
    SearchBox *SearchMembre;
    uint IdDuMembre;
    QHash<int, QHash<QString, QVariant>> ListePrereglagesPanier;
    bool MaJListeMembres();
    void ActiverControles(bool etat=false);
    void VerifChampsRempli();
    void CalculerMontant();
};

#endif
