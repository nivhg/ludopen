#include "fonctions_globale.h"
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QSqlError>

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

int ExecuterScriptSql(const QString & nomFichier)
{
    QFile fichier(nomFichier);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;

    QTextStream entree(&fichier);
    QString sql = entree.readAll();
    QStringList requetesSQL;
    requetesSQL= sql.split(';', QString::SkipEmptyParts);
    int nombreSucces = 0;

    foreach(const QString& requete, requetesSQL)
    {
        if (requete.trimmed() != "")
        {
            QSqlQuery query;
            if (query.exec(requete))
            nombreSucces++;
            else
            qDebug() << "Echec:" << requete << "\nRaison:" << query.lastError();
        }
    }
    return nombreSucces;
}

QString getLastExecutedQuery(const QSqlQuery& query)
{
 QString str = query.lastQuery();
 QMapIterator<QString, QVariant> it(query.boundValues());
 while (it.hasNext())
 {
  it.next();
  str.replace(it.key(),""""+it.value().toString()+"""");
 }
 return str;
}

void resizeColumnsToContents( QStandardItemModel *Modele,QTreeView *TreeView )
{
    for(int i=0;i<Modele->columnCount();i++)
    {
        TreeView->resizeColumnToContents(i);
        TreeView->setColumnWidth(i,TreeView->columnWidth(i)-15);
    }
}
