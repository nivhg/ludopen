//d_image.h	0.1	<Vincent VICTORIN>
#ifndef LB_IMAGE_H
#define LB_IMAGE_H

#include <QLabel>

class lb_image : public QLabel
{
    Q_OBJECT
public:

    //! Constructeur
    explicit lb_image( QWidget * parent = 0 );
    /** @brief Affiche et redimensionne l'image en fonction de size
     */
    void AfficherImage(QSize size);
    /** @brief Affiche l'image précédente
     */
    QString AfficherImagePrecedente();
    /** @brief Affiche l'image suivante
     */
    QString AfficherImageSuivante();

    /** @brief Charge les images d'un jeu
     */
    QString ChargerImage(QSize size,QString code_jeu);

signals:
    /** @brief Signal gérant le clic de souris
     */
    void clicked();

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
    QString sCheminImage[100];

    //! Numéro de l'image en cours d'affichage
    int iImage;

    //! Nombre d'image
    int iNbImage;

};

#endif // LB_IMAGE_H
