//acces_fichier_http.h	0.1	<Vincent VICTORIN>
#ifndef ACCESFICHIERPARHTTP_H
#define ACCESFICHIERPARHTTP_H

// En-têtes standards necessaires dans ce fichier en-tete seulement ------------
#include<QtCore>
#include<QtNetwork>
#include<QtWidgets>
#include<QFile>
#include<QDebug>

/**
 *  @brief AccesPhotosParHTTP permet la gestion des images stockées sur un serveur HTTP
 */
class AccesFichierParHTTP: public QNetworkAccessManager
{
  Q_OBJECT
public:
    /**
     *  @brief Constructeur de la classe.
     */
    AccesFichierParHTTP( QWidget * parent = 0 );
    /**
     *  @brief Destructeur de la classe.
     */
    ~AccesFichierParHTTP();
    /**
     *  @brief Lance le processus de téléchargement
     */
    void LancerTelechargements( QString sCheminImagePref,QString code_jeu="",QStringList ListeExtensionOuURL=QStringList());
    /**
     *  @brief Annule le processus de téléchargement
     */
    void AnnulerTelechargements();
    /**
     *  @brief Téléchargement synchrone de fichiers
     */
    void TelechargementSynchrone(QString URL,QString CheminDestination);

public slots:
    /**
     *  @brief Traitement des téléchargements : slot de fin de téléchargement et
     *         lances le prochain téléchargement
     */
    void SlotTelechargementFini();
private:
    /**
     *  @brief Passe au prochain fichier à télécharger
     */
    void PasserFichierSuivant(bool FichierTrouver);
    QNetworkAccessManager *manager;
    QStringList ListeExtensionOuURL;
    int IndexExtension;
    QString sCheminImagePref;
    QString NomFichier;
    QString CodeJeu;
    int NumeroFichier;
    bool PremierFichierTrouver;
    bool TelechargementAnnuler;
signals:
    void SignalFichierTelecharger(QString);
    void SignalTelechargementsFini();
    void SignalAnnulerTelechargement();
};

#endif // ACCESFICHIERPARHTTP_H
