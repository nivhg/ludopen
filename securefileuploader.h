/**************************************************************************
**
** This file is part of QSsh
**
** Copyright (c) 2012 LVK
**
** Contact: andres.pagliano@lvklabs.com
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**************************************************************************/

#ifndef SECUREFILEUPLOADER_H
#define SECUREFILEUPLOADER_H

#include <QObject>
#include <QStringList>

#include "sftpchannel.h"
#include "sshconnection.h"

#define COMMANDE_TELEVERSEMENT 0
#define COMMANDE_RENOMMER 1
#define COMMANDE_SUPPRIMER 2

/// Very simple example to upload a file using FTPS
class SecureFileUploader : public QObject
{
    Q_OBJECT
public:

    //!Constructeur
    explicit SecureFileUploader(QObject *parent = 0);
    /**
     *  @brief Initialise une nouvelle connexion
     */
    void init(const QString &host="", const QString &username="", const QString &privatekeyfile="");
    /**
     *  @brief Ajoute une commande à la liste des commandes
     */
    void AjouterCommande( const int &Commande, const QString &FichierSource,
                          const QString &FichierDestination);
    /**
     *  @brief Execute les commandes ajoutées précédemment
     */
    void FaireCommandes();

signals:
    /** @brief Signal emit à la fin du (télé)chargement des images
    */
    void SignalOperationFini(bool DerniereOperation);

private slots:
    /**
     *  @brief Gestion de l'évenement de connexion
     */
    void onConnected();
    /**
     *  @brief Gestion de l'évenement d'erreur de connexion
     */
    void onConnectionError(QSsh::SshError);
    /**
     *  @brief Gestion de l'évenement de canal initialisé
     */
    void onChannelInitialized();
    /**
     *  @brief Gestion d'erreur de création du canal
     */
    void onChannelError(const QString &err);
    /**
     *  @brief Gestion de l'évenement de fin d'opération
     */
    void onOpfinished(QSsh::SftpJobId job, const QString & error = QString());

private:
    // Liste des fichiers source
    QStringList m_ListeFichierSource;
    // Liste des fichiers destination
    QStringList m_ListeFichierDestination;
    // Liste des commandes
    QList<int> m_ListeCommandes;
    // Id de la dernière commande
    int m_DernierIdCommande;
    QSsh::SftpChannel::Ptr m_canal;
    QSsh::SshConnection *m_connexion;

    void parseDestination(const QString &dest);
};

#endif // SECUREFILEUPLOADER_H
