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

QString GetRandomString()
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
   const int randomStringLength = 8; // assuming you want random strings of 12 characters

   qsrand(static_cast<quint64>(QTime::currentTime().msecsSinceStartOfDay()));

   QString randomString;
   for(int i=0; i<randomStringLength; ++i)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}

QString removeAccents(QString s) {
    QString diacriticLetters = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
    QStringList noDiacriticLetters;
    noDiacriticLetters << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"<<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"<<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"<<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";

    QString output = "";
    for (int i = 0; i < s.length(); i++) {
        QChar c = s[i];
        int dIndex = diacriticLetters.indexOf(c);
        if (dIndex < 0) {
            output.append(c);
        } else {
            QString replacement = noDiacriticLetters[dIndex];
            output.append(replacement);
        }
    }

    return output;
}
