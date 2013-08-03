// f_listejeux.h    0.1     <2012-01-01>       <Florian MARY>
#ifndef F_LISTEJEUX_H
#define F_LISTEJEUX_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QtWidgets>
#include <QStandardItemModel>

namespace Ui {
    class F_ListeJeux;
}

/** @brief Classe qui permet de consulter les jeux
 *
 * Cette classe permet de consulter les jeux,
 * en effectuant des filtres selon certains critères (prix, age ...).
 *
 */
class F_ListeJeux : public QWidget
{
    Q_OBJECT

public:
    explicit F_ListeJeux(QWidget *parent = 0);
    ~F_ListeJeux();

private slots:
    //METHODE----------------------------------------------------
    /** @brief Appeler lorsque l'on modifie le contenu de la case de recherche par nom
     *
     */
    void on_LE_Nom_textChanged(const QString &arg1);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_JoueursMax
     *
     */
    void on_CBx_JoueursMax_activated(int index);
    
    /** @brief Appeler lorsque l'on clic sur le bouton Remise à zéro
     *
     */
    void on_bt_RAZ_clicked();
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_JoueursMin
     *
     */
    void on_CBx_JoueursMin_activated(int index);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_AgeMin
     *
     */
    void on_CBx_AgeMin_activated(int index);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_AgeMax
     *
     */
    void on_CBx_AgeMax_activated(int index);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_DateAcquisition
     *
     */
    void on_CBx_DateAcquisition_activated(int index);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_Etat
     *
     */
    void on_CBX_Etat_activated(int index);
    
    /** @brief Permet de récupérer le contenu de l'index
     *
     */
    void RecupererContenuIndex() ;

    /** @brief Appeler lorsque l'on modifie le contenu du CBx_PrixLoc
     *
     */
    void on_CBx_PrixLoc_activated(int index);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_Statut
     *
     */
    void on_CBx_Statut_activated(int index);
    
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_Classification
     *
     */
    void on_CBx_Classification_activated(int index);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_MotCle1
     *
     */
    void on_CBx_MotCle1_activated(int index);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_MotCle2
     *
     */
    void on_CBx_MotCle2_activated(int index);
    
    /** @brief Appeler lorsque l'on modifie le contenu du CBx_MotCle3
     *
     */
    void on_CBx_MotCle3_activated(int index);
    
    void on_Bt_ExporterListe_clicked();

    /** @brief Double clic dans la liste des jeux
     */
    void on_TbV_Recherche_doubleClicked(const QModelIndex &index);

    /** @brief Remise ? zéro des crit?res
     */
    void RAZCriteres();

signals:
    //! Signal indiquand qu'un double clic a eu lieu dans la liste des jeux
    void Signal_DoubleClic_ListeJeux( QString CodeJeu) ;

private:
    //ATTRIBUTS--------------------------------------------------

    Ui::F_ListeJeux *ui;
    
    //! Model du TableView de la liste des jeux
    QStandardItemModel * ModelJeu;
    
};

//-----------------------------------------------------------------
#endif // F_LISTEJEUX_H
//-----------------------------------------------------------------
