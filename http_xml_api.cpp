#include "http_xml_api.h"

/**
 *  @brief Lance le processus de téléchargement
 *
 *  @param sCheminImagePref : Chemin ou URL des fichiers à télécharger
 *  @param code_jeu : Code du jeu correspondant aux fichiers à télécharger
 *  @param ListeExtension : Liste des extentions des fichiers à traiter
 */
void Http_xml_api::LancerTelechargement( QString URL, int Mode)
{
    this->Mode=Mode;
    this->ValeursRetour=ValeursRetour;
    this->HeureLancement=QDateTime::currentMSecsSinceEpoch();
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QNetworkReply *newreply= manager.get(QNetworkRequest(QUrl(URL)));
    QObject::connect(newreply, SIGNAL(finished()), this, SLOT(SlotTelechargementFini()));
    connect(newreply, SIGNAL(sslErrors(QList<QSslError>)),
                SLOT(sslErrors(QList<QSslError>)));
}

void Http_xml_api::SlotTelechargementFini()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if(reply == NULL) return;
    int err=reply->error();
    switch (reply->error())
    {
        case QNetworkReply::NoError:
            // Pas d'erreur
            QDomDocument xmlBOM;
            QString Reponse=reply->readAll();
            xmlBOM.setContent(Reponse);
            // Extract the root markup
            QDomElement root=xmlBOM.documentElement();

            // Get the first child of the root (Markup COMPONENT is expected)
            QDomElement Component=root.firstChild().toElement();

            switch(Mode)
            {
                case MODE_GT_JSON:
                {
                    QJsonDocument doc = QJsonDocument::fromJson(Reponse.toUtf8());
                    QJsonArray array;
                    if(!doc.isNull())
                    {
                        if(doc.isArray())
                        {
                            array = doc.array();
                        }
                        else
                        {
                            qDebug() << "Document is not an object" << endl;
                            return;
                        }
                    }
                    else
                    {
                        qDebug() << "Invalid JSON...\n" << endl;
                        return;
                    }
                    QJsonArray Array2;
                    Array2=array[0].toArray();
                    QString Retour;
                    for(int i=0;i<Array2.count();i++)
                    {
                        Retour+=Array2[i].toArray()[0].toString()+"\n";
                    }
                    emit SignalTelechargementFiniGT(Retour);
                    break;
                }
                case MODE_BGG:
                {
                    QVector<QVector<QString>> *MatriceBGG=new QVector<QVector<QString>>();
                    // Loop while there is a child
                    while(!Component.isNull())
                    {
                        // Check if the child tag name is COMPONENT
                        if (Component.tagName()=="item")
                        {
                            // Get the first child of the component
                            QDomElement Child=Component.firstChild().toElement();
                            // Read each child of the component node
                            while (!Child.isNull())
                            {
                                // Read Name and value
                                if (Child.tagName()=="name")
                                {
                                    QVector<QString> *Liste=new QVector<QString>();
                                    Liste->append(Child.attribute("value","No name"));
                                    Liste->append(Component.attribute("id","No id"));
                                    MatriceBGG->append(*Liste);
                                }

                                // Next child
                                Child = Child.nextSibling().toElement();
                            }
                        }
                        // Next component
                        Component = Component.nextSibling().toElement();
                    }
                    emit SignalTelechargementFiniBGG(*MatriceBGG,HeureLancement);
                    delete MatriceBGG;
                    break;
                }
                case MODE_BGG_VER:
                {
                    QVector<QVector<QVector<QString>>> *Matrice=new QVector<QVector<QVector<QString>>>();
                    bool LigneAjoutee=false;
                    QVector<QVector<QString>> Liste;
                    Liste.resize(BGG_TAILLE+BGG_TAILLE_MULTICHAMPS);

                    // Loop while there is a child
                    while(!Component.isNull())
                    {
                        if (Component.tagName()=="item")
                        {
                            // Get the first child of the component
                            QDomElement Child=Component.firstChild().toElement();
                            // Read each child of the component node
                            while (!Child.isNull())
                            {
                                if (Child.tagName()=="statistics")
                                {
                                    // Get the first child of the component
                                    QDomElement Child2=Child.firstChild().toElement();
                                    // Read each child of the component node
                                    while (!Child2.isNull())
                                    {
                                        // Read Name and value
                                        if (Child2.tagName()=="ratings")
                                        {
                                            // Get the first child of the component
                                            QDomElement Child3=Child2.firstChild().toElement();
                                            while (!Child3.isNull())
                                            {
                                                if (Child3.tagName()=="average")
                                                {
                                                    Liste[BGG_NOTE].resize(1);
                                                    Liste[BGG_NOTE].replace(0,Child3.attribute("value","No idvalue"));
                                                    break;
                                                }
                                                // Next child
                                                Child3 = Child3.nextSibling().toElement();
                                            }
                                        }
                                        // Next child
                                        Child2 = Child2.nextSibling().toElement();
                                    }
                                }
                                // Next child
                                Child = Child.nextSibling().toElement();
                            }
                        }
                        // Next component
                        Component = Component.nextSibling().toElement();
                    }

                    Component=root.firstChild().toElement();
                    // Loop while there is a child
                    while(!Component.isNull())
                    {
                        if (Component.tagName()=="item")
                        {
                            // Get the first child of the component
                            QDomElement Child=Component.firstChild().toElement();
                            // Read each child of the component node
                            while (!Child.isNull())
                            {
                                // On exclue BGG_MOTCLES du resize
                                for(int i=0;i<BGG_TAILLE;i++)
                                {
                                    if(Liste[i].size()==0) Liste[i].resize(1);
                                }
                                // Read Name and value
                                if (Child.tagName()=="link")
                                {
                                    if(Child.attribute("type","No type")=="boardgamecategory"||
                                        Child.attribute("type","No type")=="boardgamemechanic")
                                    {
                                        Liste[BGG_MOTSCLES].append(Child.attribute("id","No id"));
                                    }
                                    else if(Child.attribute("type","No type")=="boardgamedesigner")
                                    {
                                        Liste[BGG_AUTEURS_ID].append(Child.attribute("id","No id"));
                                        Liste[BGG_AUTEURS_TXT].append(Child.attribute("value","No idvalue"));
                                    }
                                    else if(Child.attribute("type","No type")=="boardgamepublisher")
                                    {
                                        Liste[BGG_EDITEUR_ID].replace(0,Child.attribute("id","No id"));
                                        Liste[BGG_EDITEUR_TXT].replace(0,Child.attribute("value","No value"));
                                    }
                                }
                                if(Child.tagName()=="minplayers")
                                {
                                    Liste[BGG_MINJOUEURS].replace(0,Child.attribute("value","No value"));
                                }
                                if(Child.tagName()=="maxplayers")
                                {
                                    Liste[BGG_MAXJOUEURS].replace(0,Child.attribute("value","No value"));
                                }
                                if(Child.tagName()=="minplaytime")
                                {
                                    Liste[BGG_MINDUREE].replace(0,Child.attribute("value","No value"));
                                }
                                if(Child.tagName()=="maxplaytime")
                                {
                                    Liste[BGG_MAXDUREE].replace(0,Child.attribute("value","No value"));
                                }
                                if(Child.tagName()=="minage")
                                {
                                    Liste[BGG_MINAGE].replace(0,Child.attribute("value","No value"));
                                }
                                if(Child.tagName()=="description")
                                {
                                    Liste[BGG_DESCRIPTION].replace(0,Child.firstChild().toText().data());
                                }

                                // Read Name and value
                                if (Child.tagName()=="versions")
                                {
                                    // Get the first child of the component
                                    QDomElement Child2=Child.firstChild().toElement();
                                    // Read each child of the component node
                                    while (!Child2.isNull())
                                    {
                                        // Read Name and value
                                        if (Child2.tagName()=="item")
                                        {
                                            // Get the first child of the component
                                            QDomElement Child3=Child2.firstChild().toElement();
                                            // Récupération de l'ID de la version BGG
                                            Liste[BGG_VERSION].replace(0,Child2.attribute("id","No id"));
                                            // Read each child of the component node
                                            while (!Child3.isNull())
                                            {
                                                if (Child3.tagName()=="link")
                                                {
                                                    if(Child3.attribute("type","No type")=="language")
                                                    {
                                                        Liste[BGG_LANGUE].replace(0,Child3.attribute("id","No id"));
                                                    }
                                                    else if(Child3.attribute("type","No type")=="boardgamepublisher")
                                                    {
                                                        Liste[BGG_EDITEUR_ID].replace(0,Child3.attribute("id","No id"));
                                                        Liste[BGG_EDITEUR_TXT].replace(0,Child3.attribute("value","No value"));
                                                    }

                                                }
                                                if (Child3.tagName()=="name"&&Child3.attribute("type","No type")=="primary")
                                                {
                                                    Liste[BGG_NOM].replace(0,Child3.attribute("value","No value"));
                                                }
                                                if (Child3.tagName()=="thumbnail")
                                                {
                                                    Liste[BGG_APERCU].replace(0,Child3.firstChild().toText().data());
                                                }
                                                if (Child3.tagName()=="image")
                                                {
                                                    Liste[BGG_IMAGE].replace(0,Child3.firstChild().toText().data());
                                                }
                                                // Next child
                                                Child3 = Child3.nextSibling().toElement();
                                            }
                                            if (Liste != QVector<QVector<QString>>())
                                            {
                                                Matrice->append(Liste);
                                                LigneAjoutee=true;
                                                //delete Liste;
                                            }
                                        }

                                        // Next child
                                        Child2 = Child2.nextSibling().toElement();
                                    }
                                }

                                // Next child
                                Child = Child.nextSibling().toElement();
                            }
                        }
                        // Next component
                        Component = Component.nextSibling().toElement();
                    }
                    if(!LigneAjoutee)
                    {
                        Matrice->append(Liste);
                    }
                    emit SignalTelechargementFiniBGGVersion(*Matrice);
                    delete Matrice;
                    break;
                }
            }
            delete reply;
            break;
    }
}

void Http_xml_api::sslErrors(const QList<QSslError> &sslErrors)
{
    for (const QSslError &error : sslErrors)
    {
        qDebug()<<"SSL error: "+error.errorString();
    }
}
