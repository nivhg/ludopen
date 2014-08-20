#include "fonctions_globale.h"
#include <QFileInfo>

QVariant ObtenirValeurParNom(QSqlQuery requete,QString NomChamps)
{
    return requete.value(requete.record().indexOf(NomChamps));
}


bool EstCeURL(QString chaine)
{
    return chaine.indexOf("http://",0,Qt::CaseInsensitive) != -1;
}

bool EstCeNomFichierContient(QString CheminFichier,QString achercher)
{
    QFileInfo InfoFichier(CheminFichier);
    return InfoFichier.fileName().indexOf(achercher)!=-1;
}

