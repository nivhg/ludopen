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

#include "securefileuploader.h"

#include <QtDebug>
#include <QFileInfo>
#include <QEventLoop>

SecureFileUploader::SecureFileUploader(QObject *parent) :
    QObject(parent), m_connexion(0)
{
}

/**
 *  @brief Initialise une nouvelle connexion
 *
 *  @param host : nom ou adresse IP de l'hôte à se connecter
 *  @param username : nom de l'utilisateur avec lequel se connecter
 *  @param privatekeyfile : Chemin du fichier contenant la clé privée
 */
void SecureFileUploader::init(const QString &host, const QString &username, const QString &privatekeyfile)
{
    QSsh::SshConnectionParameters params;
    params.host = host;
    params.userName = username;
    params.privateKeyFile = privatekeyfile;
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationByKey;
    params.timeout = 30;
    params.port = 22;

    m_connexion = new QSsh::SshConnection(params, this); // TODO free this pointer!
}

/**
 *  @brief EffacerCommandes : Efface la liste des commandes et fichiers source et destination
 */
void SecureFileUploader::EffacerCommandes()
{
    m_ListeCommandes.clear();
    m_ListeFichierDestination.clear();
    m_ListeFichierSource.clear();
}

/**
 *  @brief Ajoute une commande à la liste des commandes
 *
 *  @param commande : Type de commande à faire
 *  @param FichierSource : Chemin du fichier source
 *  @param FichierDestination : Chemin du fichier de destination
 */
void SecureFileUploader::AjouterCommande( const int &Commande,
                                 const QString &FichierSource, const QString &FichierDestination)
{
    m_ListeCommandes<<Commande;
    m_ListeFichierSource<<FichierSource;
    m_ListeFichierDestination<<FichierDestination;
}

/**
 *  @brief Execute les commandes ajoutées précédemment
 */
void SecureFileUploader::FaireCommandes()
{
    if(m_ListeCommandes.count()==0)
    {
        qDebug() << "SecureFileUploader::FaireCommandes(): Aucune commande passée, abandon.";
        emit SignalOperationFini(true);
        return;
    }
    qDebug() << "SecureUploader: Connexion";

    connect(m_connexion, SIGNAL(connected()), SLOT(onConnected()));
    connect(m_connexion, SIGNAL(error(QSsh::SshError)), SLOT(onConnectionError(QSsh::SshError)));

    m_connexion->connectToHost();
}

/**
 *  @brief Gestion de l'évenement de connexion
 */
void SecureFileUploader::onConnected()
{
    qDebug() << "SecureUploader: Connecté";
    qDebug() << "SecureUploader: Création du canal SFTP...";

    m_canal = m_connexion->createSftpChannel();

    if (m_canal) {
        connect(m_canal.data(), SIGNAL(initialized()),
                SLOT(onChannelInitialized()));
        connect(m_canal.data(), SIGNAL(initializationFailed(QString)),
                SLOT(onChannelError(QString)));
        connect(m_canal.data(), SIGNAL(finished(QSsh::SftpJobId, QString)),
                SLOT(onOpfinished(QSsh::SftpJobId, QString)));

        m_canal->initialize();

    } else {
        qDebug() << "SecureUploader: Erreur canal nul. State :" << m_connexion->state();;
    }
}

/**
 *  @brief Gestion de l'évenement d'erreur de connexion
 */
void SecureFileUploader::onConnectionError(QSsh::SshError err)
{
    qDebug() << "SecureUploader: Erreur de connection N°" << err<<" : "<<m_connexion->errorString();
}

/**
 *  @brief Gestion de l'évenement de canal initialisé
 */
void SecureFileUploader::onChannelInitialized()
{
    qDebug() << "SecureUploader: Canal initialisé";

    QSsh::SftpJobId job;
    // Parcours toutes les commandes ajoutées
    for(int i=0;i<m_ListeCommandes.count();i++)
    {
        // Execute la commande associée
        switch(m_ListeCommandes[i])
        {
            case COMMANDE_TELEVERSEMENT:
                qDebug() << "SecureUploader: Téléversement de " << m_ListeFichierSource[i]
                            << "vers " << m_ListeFichierDestination[i];
                job = m_canal->uploadFile(m_ListeFichierSource[i], m_ListeFichierDestination[i],QSsh::SftpOverwriteExisting);
                break;
            case COMMANDE_RENOMMER:
                qDebug() << "SecureUploader: Renommage de " << m_ListeFichierSource[i]
                            << "vers " << m_ListeFichierDestination[i];
                job = m_canal->renameFileOrDirectory(m_ListeFichierSource[i], m_ListeFichierDestination[i]);
                break;
            case COMMANDE_SUPPRIMER:
                qDebug() << "SecureUploader: Suppression de " << m_ListeFichierSource[i];
                job = m_canal->removeFile(m_ListeFichierSource[i]);
                break;
        }
        if (job != QSsh::SftpInvalidJob)
        {
            qDebug() << "SecureUploader: Demarrage de la commande #" << job;
        }
        else
        {
            qDebug() << "SecureUploader: Commande invalide";
        }
        if(i==m_ListeCommandes.count()-1)
        {
            m_DernierIdCommande=job;
        }
    }
}

/**
 *  @brief Gestion d'erreur de création du canal
 */
void SecureFileUploader::onChannelError(const QString &err)
{
    qDebug() << "SecureUploader: Erreur N°" << err<<" : "<<m_connexion->errorString();
}

/**
 *  @brief Gestion de l'évenement de fin d'opération
 */
void SecureFileUploader::onOpfinished(QSsh::SftpJobId job, const QString &err)
{
    qDebug() << "SecureUploader: Commande fini #" << job << ":" << (err.isEmpty() ? "OK" : err);
    // Emets un signal à destination de d_Image et indique si il s'agit de la dernière commande
    emit SignalOperationFini(m_DernierIdCommande==job);
}
