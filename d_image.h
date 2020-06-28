//d_image.h	0.1	<Vincent VICTORIN>
#ifndef D_IMAGE_H
#define D_IMAGE_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include <QDialog>

// En-têtes propres à l'application necessaires dans ce fichier en-tete --------
#include "lb_image.h"
#include "securefileuploader.h"
#include "http_xml_api.h"

#define OP_DEPLACEMENT_GAUCHE 0
#define OP_DEPLACEMENT_DROITE 1
#define OP_AJOUT 2
#define OP_SUPPRESSION 3
#define OP_DEFINIR_LUDOPEN 4
#define OP_DEFINIR_WEB 5
#define OP_DEFINIR_DEUX 6

#define EMPLACEMENT_SERVEUR 0
#define EMPLACEMENT_TEMP 1
#define EMPLACEMENT_LOCAL 2

// Déclaration de la classe utile dans les appels récursifs des fichiers .h
class D_Image;

namespace Ui {
class D_Image;
}

class D_Image : public QDialog
{
    Q_OBJECT

public:
    //! Constructeur
    explicit D_Image(QWidget *parent = 0, Lb_Image* lb_image=0);
    //! Destructeur
    ~D_Image();

    /**
     *  @brief Affiche une bordure sur le label passé en paramètre
     */
    void ChangerSelection(QLabel * pLabel);
    /**
     *  @brief Initialise l'affichage des images
     */
    void AfficherImage(QString sCodeJeu,QStringList sCheminImage);
    /**
     *  @brief Recharge les images sur le disque et les affiche
     */
    void RechargerImages();
    /**
     *  @brief Inverse l'image sélectionné avec l'image indiquée
     *          par iInversionDecalage (index par rapport à l'image sélectionnée)
     */
    void Inverser(int emplacement);
    /**
     *  @brief Ajouter une image de jeu
     */
    void Ajouter(int emplacement);
    /**
     *  @brief Suppression de l'image sélectionné et décalage des images suivantes
     */
    void Suppression(int emplacement);
    /**
     *  @brief Mets l'image sélectionnée par défaut sur Ludopen
     */
    void DefinirLudopen(int emplacement);
    /**
     *  @brief Mets par défaut l'image sélectionnée pour le site Web
     */
    void DefinirWeb(int emplacement);
    /**
     *  @brief Mets l'image sélectionnée par défaut sur Ludopen et le site Web
     */
    void DefinirDeux(int emplacement);
    /**
     *  @brief Décalager à gauche par rapport à l'image sélectionnée
     */
    void DecalerAGauche(int emplacement);
    /**
     *  @brief Montre ou cache les labels indiquants les images par défaut
     *          sur Ludopen, le site Web ou les 2
     */
    void MontrerLudopenWebDeux(bool Montrer);

    /**
     *  @brief Affiche les images du site BGG passé en paramètre
     */
    void AfficherImageBGG(QString sCodeJeu,QVector<QVector<QVector<QString>>> Matrice);
public slots:
    void SlotTelechargementsFini();
    void SlotTelechargementImageFinale();
private slots:
    /**
     *  @brief Gestion du click de Lb_Image
     */
    void on_Lb_Image_clicked();
    /**
     *  @brief Gestion du bouton "Ajouter"
     */
    void on_Bt_Ajouter_clicked();
    /**
     *  @brief Gestion du clic du bouton "Déplacer à gauche"
     */
    void on_Bt_DeplacerGauche_clicked();
    /**
     *  @brief Gestion du clic du bouton "Déplacer à droite"
     */
    void on_Bt_DeplacerDroite_clicked();
    /**
     *  @brief Slot qui est exécuté à chaque fois qu'une opération sur
     *      le serveur est terminée
     */
    void slot_OperationFini(bool DerniereOperation);
    /**
     *  @brief Gestion du clic du bouton "Gauche"
     */
    void on_Bt_Gauche_clicked();
    /**
     *  @brief Gestion du clic du bouton "Droite"
     */
    void on_Bt_Droite_clicked();
    /**
     *  @brief Gestion du clic du bouton Sauvegarder
     */
    void on_Bt_Sauvegarder_clicked();
    /**
     *  @brief Gestion du clic du bouton Supprimer
     */
    void on_Bt_Supprimer_clicked();
    /**
     *  @brief Gestion du clic du bouton d'une image par défaut sur Ludopen
     */
    void on_Bt_Defaut_Ludopen_clicked();
    /**
     *  @brief Gestion du clic du bouton pour mettre l'image sélectionnée
     *          par défaut sur le site Web
     */
    void on_Bt_Defaut_Web_clicked();
    /**
     *  @brief Gestion du clic du bouton pour mettre l'image sélectionnée
     *      par défaut sur ludopen et le site Web
     */
    void on_Bt_Defaut_Deux_clicked();
    /**
     *  @brief Slot exécuté lorsque la fenêtre se ferme
     */
    void onCloseDialog();

    void on_Cb_FiltreLangue_currentIndexChanged(int index);

    void on_Bt_Annuler_clicked();

    void on_Bt_Valider_clicked();

signals:
    void dialogIsClosing();
    void SignalVersionBGGChoisi(QString IdVersionBGG);

protected:
    /**
     *  @brief Gestion de l'évenement de fermeture
     */
    void closeEvent(QCloseEvent *event);

private:
    Ui::D_Image *ui;
    QStringList sCheminImage,sURLImage,sNomVersion,sIdVersion;
    Lb_Image * lb_image;
    Lb_Image * lb_image2;
    Lb_Image * lb_image3;
    Lb_Image * lb_img;
    QString sCodeJeu;
    int iLbImageSelectionnee;
    QStringList sNomImagesAjoutees;
    SecureFileUploader * uploader;
    // Indique le numéro de lecture des images au cas où l'on serait sur une autre page (les images s'affichent 3 par 3)
    int iDecalage;
    // Paramètre de décalage par rapport à une autre image dans le cas d'un changement d'état (site web par défaut, ludopen par défaut...)
    int iInversionDecalage;
    int iOperationServeur;
    QVector<QVector<QVector<QString>>> MatriceBGG;
    AccesFichierParHTTP * acces;
    bool ImagesBGG;

    void RecupererImagesBGG();
    void AjoutImageLocalOuServeur();
    void EffacerItemsLayout(QGridLayout *layout, int row, int column, bool deleteWidgets);
    void EffacerWidgetsEnfants(QLayoutItem *item);
};

#endif // D_IMAGE_H
