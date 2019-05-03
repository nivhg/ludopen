#ifndef D_RESAMISDECOTE_H
#define D_RESAMISDECOTE_H

#include <QDialog>
#include "f_preferences.h"
#include "Courriel.h"
#include "fonctions_globale.h"
#include <QAbstractSocket>

#define STATUTJEUX_DISPONIBLE 1
#define STATUTJEUX_ENRESERVATION 7

namespace Ui {
class D_ResaMisDeCote;
}

class D_ResaMisDeCote : public QDialog
{
    Q_OBJECT
public:
    explicit D_ResaMisDeCote(QWidget *parent = nullptr,QString CodeJeu="",QString NomJeu="",int IdMembre=0,int IdJeu=0,QString Email="");
    ~D_ResaMisDeCote();

signals:

public slots:

private slots:
    /**
     * @brief Permet de supprimer un objet dynamique de type Courriel une fois qu'il fait son travail
     *
     * @param pCourriel
     */
    void slot_FermerThreadCourriel() ;

    /**
     * @brief Permet de supprimer un objet dynamique de type Courriel une fois que le mail est envoyé
     *
     * @param pCourriel
     */
    void slot_ConfirmerMailEnvoyer( uint IDMembre ) ;

    /**
     * @brief Afficher les erreurs qu'il peut y avoir lors de l'envoie d'un mail
     *
     * @param sErreur
     */
    void slot_AfficherErreurMail( QString sErreur ) ;
    void on_Bt_NonMisDeCote_clicked();
    void on_Bt_MisDeCote_clicked();

    void on_Bt_Valider_clicked();

private:
    Ui::D_ResaMisDeCote *ui;
    void closeEvent(QCloseEvent *event);
    bool enableClose;
    Courriel *pCourriel;
    /// vecteur qui contient la liste des emails à envoyer
    QList <EMail> ListeEMailAEnvoyer ;
    int IdMembre=0;
    int IdJeu=0;
    QString Email;
    QString NomJeu;
    QString CodeJeu;
    QString ServeurSMTP;
    int PortSMTP;
};

#endif // D_RESAMISDECOTE_H
