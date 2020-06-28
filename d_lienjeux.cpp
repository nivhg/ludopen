#include "d_lienjeux.h"
#include "ui_d_lienjeux.h"

D_LienJeux::D_LienJeux(QWidget *parent,QString CodeJeu) :
    QDialog(parent),
    ui(new Ui::D_LienJeux)
{
    ui->setupUi(this);

    this->CodeJeu=CodeJeu;
    SearchJeux = new SearchBox(this);
    QVector<QString> Jeux ;
    QVector<QVector <QString> > VecteurJeux;

    QSqlQuery Requete;
    Requete.prepare("SELECT DISTINCT NomJeu,CodeJeu FROM jeux as j  ORDER BY NomJeu");

    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    else
    {
        //Execute une requète sql qui retourne la liste des jeux
        //Si la requète est correcte -> Remplissage du veteur VecteurJeux
        // avec le résultat de la requête et on retourne vrai.
        while(Requete.next())
        {
            for(int i=0;i<Requete.record().count();i++)
            {
                Jeux.append(Requete.value(i).toString()) ;
            }
            VecteurJeux.push_back(Jeux) ;
            Jeux.clear();
        }
    }
    SearchJeux->MAJResults(VecteurJeux,2);
    SearchJeux->show();
    ui->HLay_Jeux->addWidget(SearchJeux);

    Requete.prepare("SELECT * FROM typelien ORDER BY IdTypeLien");

    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    else
    {
        while(Requete.next())
        {
            ui->Cbx_TypeLien->addItem(ObtenirValeurParNom(Requete,"NomLien").toString(),ObtenirValeurParNom(Requete,"IdTypeLien").toInt());
        }
    }
}

D_LienJeux::~D_LienJeux()
{
    delete ui;
}

void D_LienJeux::on_Bt_Annuler_clicked()
{
    this->close();
}

void D_LienJeux::on_Bt_OK_clicked()
{
    QSqlQuery Requete;
    Requete.prepare("INSERT INTO lienjeuxjeux (Jeux_IdJeuxSource,Jeux_IdJeuxDestination,TypeLien_IdTypeLien) VALUES ("
                    "(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeuSource),(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeuDestination),"
                    ":TypeLien_IdTypeLien)");
    if(ui->Cbx_DescriptifJeu->currentData()=="Source")
    {
        Requete.bindValue(":CodeJeuSource",SearchJeux->currentText());
        Requete.bindValue(":CodeJeuDestination",this->CodeJeu);
    }
    else
    {
        Requete.bindValue(":CodeJeuSource",this->CodeJeu);
        Requete.bindValue(":CodeJeuDestination",SearchJeux->currentText());
    }
    Requete.bindValue(":TypeLien_IdTypeLien",ui->Cbx_TypeLien->currentData().toInt());
    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    this->close();
}

void D_LienJeux::on_Cbx_TypeLien_currentIndexChanged(int index)
{
    QSqlQuery Requete;
    Requete.prepare("SELECT * FROM typelien WHERE IdTypeLien=:IdTypeLien ORDER BY NomLien");
    Requete.bindValue(":IdTypeLien",ui->Cbx_TypeLien->currentData());

    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete)<<Requete.lastError();
        return;
    }
    Requete.next();
    ui->Cbx_DescriptifJeu->clear();
    ui->Cbx_DescriptifJeu->addItem(ObtenirValeurParNom(Requete,"DescriptifSource").toString(),"Source");
    ui->Cbx_DescriptifJeu->addItem(ObtenirValeurParNom(Requete,"DescriptifDestination").toString(),"Destination");
}
