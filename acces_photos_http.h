#ifndef ACCESPHOTOSPARHTTP_H
#define ACCESPHOTOSPARHTTP_H

#include<QtCore>
#include<QtNetwork>
#include<QtWidgets>

class AccesPhotosParHTTP:public QNetworkAccessManager
{
  Q_OBJECT
public:
    bool FileOrURLExists(QString* filename, QString url, QString ext, QString* TypeImage)
    {
        // On suppose que l'argument ext est toujours mis en minuscule,
        // et on rappele la fonction avec ext en majuscule
        if(ext!=ext.toUpper()) {
            if(FileOrURLExists(filename,url,ext.toUpper(),TypeImage))
            {
                return true;
            }
            ext=ext.toLower();
        }
        url+="."+ext;
        *TypeImage=ext.toUpper();
        // Si le chemin des images est une URL
        if( url.indexOf("http://",0,Qt::CaseInsensitive) != -1)
        {
            QNetworkAccessManager m_NetworkMngr;
            QNetworkReply *reply= m_NetworkMngr.get(QNetworkRequest(url));
            QEventLoop loop;
            QObject::connect(reply, SIGNAL(finished()),&loop, SLOT(quit()));
            loop.exec();

            QTemporaryFile tempfile;
            tempfile.open();
            tempfile.setAutoRemove(false);
            tempfile.write(reply->readAll());
            tempfile.close();
            *filename=tempfile.fileName();

            switch (reply->error())
              {
                case QNetworkReply::NoError:
                  // No error
                  delete reply;
                  return true;
                case QNetworkReply::ContentNotFoundError:
                  // 404 Not found
                  /*int httpStatus = reply->attribute(
                    QNetworkRequest::HttpStatusCodeAttribute).toInt();
                  QByteArray httpStatusMessage = reply->attribute(
                    QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();*/
                  delete reply;
                  tempfile.remove();
                  return false;
                  break;
                }
        }
        else
        {
            return QFile::exists(url);
        }

    }

};

#endif // ACCESPHOTOSPARHTTP_H
