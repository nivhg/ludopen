//d_image.h	0.1	<Vincent VICTORIN>
#ifndef LB_IMAGE_H
#define LB_IMAGE_H

#include <QLabel>
#include<QtNetwork>
#include "acces_fichier_http.h"

class Lb_Image : public QLabel
{
    Q_OBJECT
public:

    //! Constructeur
    explicit Lb_Image( QWidget * parent = 0, QLabel * Lb_NomImage=0 );

    //! Destructeur
    ~Lb_Image();

    /** @brief Affiche et redimensionne l'image en fonction de size
     */
    void AfficherImage(QSize size);
    /** @brief Affiche l'image précédente
     */
    void AfficherImagePrecedente();
    /** @brief Affiche l'image suivante
     */
    void AfficherImageSuivante();

    /** @brief Charge les images d'un jeu
     */
    void ChargerImage(QSize size,QString code_jeu,QCursor curseur);

    /** @brief Récupère les chemins des fichiers téléchargés
     */
    QStringList ObtenirsCheminImage();

    /** @brief Récupère les noms des fichiers téléchargés
     */
    QStringList ObtenirsNomImage();

    /** @brief Définit les chemins des fichiers téléchargés
     */
    void DefinirsCheminImage(QStringList sCheminImage);

    /** @brief Effacer les fichiers temporaires
     */
    void EffacerFichiers();

    /** @brief Définit iImage
     */
    void DefiniriImage(int iImage);

signals:
    /** @brief Signal gérant le clic de souris
     */
    void SignalClic();

    /** @brief Signal emit à la fin du (télé)chargement des images
    */
    void SignalChargementFini();

protected:
    /** @brief Gestion de l'évenement mousePress
     */
    void mousePressEvent ( QMouseEvent * event ) ;
    /** @brief Gestion de l'évenement resize
     */
    void resizeEvent ( QResizeEvent * event );
private:
    //! Chemin ou URL contenant les images défini dans les préférences
    QString sCheminImagePref;

    //! Chemin vers les fichiers images du jeu en cours
    QStringList sCheminImage;

    //! Nom des fichiers images du jeu en cours
    QStringList sNomImage;

    //! Numéro de l'image en cours d'affichage
    int iImage;

    //! Booléen pour indiquer que le téléchargement des images est en cours
    bool ChargementEnCours;

    QCursor SauvegardeCurseur;
    QLabel * Lb_NomImage;

    AccesFichierParHTTP * acces;
private slots:
    void SlotFichierTelecharger(QString Fichier);
    void SlotTelechargementsFini();
};

#endif // LB_IMAGE_H
