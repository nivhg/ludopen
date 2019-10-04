#ifndef HTTP_XML_API_H
#define HTTP_XML_API_H
#include<QtCore>
#include<QtNetwork>
#include<QtWidgets>
#include<QFile>
#include<QDebug>
#include<QObject>
#include<QtXml>

#define MODE_BGG 0
#define MODE_BGG_VER 1
#define MODE_GT_JSON 2

#define BGG_VERSION 0
#define BGG_NOM 1
#define BGG_LANGUE 2
#define BGG_APERCU 3
#define BGG_IMAGE 4
#define BGG_MINJOUEURS 5
#define BGG_MAXJOUEURS 6
#define BGG_MINDUREE 7
#define BGG_MAXDUREE 8
#define BGG_MINAGE 9
#define BGG_EDITEUR_ID 10
#define BGG_EDITEUR_TXT 11
#define BGG_DESCRIPTION 12
#define BGG_NOTE 13
#define BGG_TAILLE 14

#define BGG_AUTEURS_ID 14
#define BGG_AUTEURS_TXT 15
#define BGG_MOTSCLES 16
// Nombre de champs qui contiennent plusieurs valeurs
#define BGG_TAILLE_MULTICHAMPS 3



using namespace std;

class Http_xml_api : public QObject
{
    Q_OBJECT
    QNetworkAccessManager manager;
public:
    void LancerTelechargement( QString URL, int Mode=MODE_BGG);

signals:
    void SignalTelechargementFiniBGG(QVector<QVector<QString>>,qint64);
    void SignalTelechargementFiniBGGVersion(QVector<QVector<QVector<QString>>>);
    void SignalTelechargementFiniGT(QString);

public slots:
    void SlotTelechargementFini();
    void sslErrors(const QList<QSslError> &errors);

private:
    QStringList ValeursRetour;
    qint64 HeureLancement;
    int Mode;
};

#endif // HTTP_XML_API_H
