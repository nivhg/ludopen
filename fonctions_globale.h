#ifndef FONCTIONS_GLOBALE_H
#define FONCTIONS_GLOBALE_H

#include <QVariant>
#include <QSqlQuery>
#include <QSqlRecord>

QVariant ObtenirValeurParNom(QSqlQuery requete,QString NomChamps);
bool EstCeURL(QString chaine);
bool EstCeNomFichierContient(QString CheminFichier,QString achercher);

#endif // FONCTIONS_GLOBALE_H
