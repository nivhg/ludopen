#include "w_historiquemaintenance.h"
#include "ui_w_historiquemaintenance.h"

W_HistoriqueMaintenance::W_HistoriqueMaintenance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_HistoriqueMaintenance)
{
    ui->setupUi(this);
    DelegateDetect=new DetectDelegate(this);
    ui->Tw_HistoriqueMaintenance->setItemDelegate(DelegateDetect);
    connect(DelegateDetect,SIGNAL(editingStartedSignal()),this,SLOT(editingStartedHistorique()));
}

W_HistoriqueMaintenance::~W_HistoriqueMaintenance()
{
    delete ui;
}

void W_HistoriqueMaintenance::Definir_CodeJeu(QString CodeJeu)
{
    this->CodeJeu=CodeJeu;
}

void W_HistoriqueMaintenance::ActualiserHistoriqueMaintenance()
{
    ui->Tw_HistoriqueMaintenance->clear();
    ui->Tw_HistoriqueMaintenance->setColumnCount(6);
    QStringList m_TableHeader;
    m_TableHeader<<"Date"<<"Membre"<<"Pièce";
    ui->Tw_HistoriqueMaintenance->setHorizontalHeaderLabels(m_TableHeader);

    QTableWidgetItem *Item=new QTableWidgetItem("Nb manquant");
    Item->setBackground(QColor(0, 255, 0));
    ui->Tw_HistoriqueMaintenance->setHorizontalHeaderItem(3,Item);
    Item=new QTableWidgetItem("Abimée?");
    Item->setBackground(QColor(0, 255, 0));
    ui->Tw_HistoriqueMaintenance->setHorizontalHeaderItem(4,Item);
    Item=new QTableWidgetItem("Remarque");
    Item->setBackground(QColor(0, 255, 0));
    ui->Tw_HistoriqueMaintenance->setHorizontalHeaderItem(5,Item);

    QSqlQuery Requete;
    Requete.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu");
    Requete.bindValue(":CodeJeu",CodeJeu);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    Requete.next();
    int IdJeux=ObtenirValeurParNom(Requete,"IdJeux").toInt();

    Requete.prepare("SELECT * FROM piecesmanquantes as pm LEFT JOIN pieces as p ON p.IdPieces=IdPieces_Pieces "
                    "LEFT JOIN jeux as j ON p.IdJeuxOuIdPieces=IdJeux LEFT JOIN membres as m ON IdMembre_Membres=m.IdMembre "
                    "WHERE j.IdJeux=:IdJeux Or (PieceGroupe=2 AND IdJeuxOuIdPieces IN "
                    "(SELECT IdPieces FROM pieces WHERE IdJeuxOuIdPieces=:IdJeux))");
    Requete.bindValue(":IdJeux",IdJeux);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    int i=0;
    while(Requete.next())
    {
        ui->Tw_HistoriqueMaintenance->blockSignals(true);
        ui->Tw_HistoriqueMaintenance->setRowCount(i+1);
        QTableWidgetItem *Item=new QTableWidgetItem(ObtenirValeurParNom(Requete,"DatePiecesManquantes").toDate().toString("dd-MM-yy"));
        Item->setData(Qt::UserRole,ObtenirValeurParNom(Requete,"IdPiecesManquantes").toInt());

        Item->setFlags(Item->flags() & ~Qt::ItemIsEditable);
        ui->Tw_HistoriqueMaintenance->setItem(i, 0, Item);
        Item=new QTableWidgetItem(ObtenirValeurParNom(Requete,"Prenom").toString());
        Item->setFlags(Item->flags() & ~Qt::ItemIsEditable);
        ui->Tw_HistoriqueMaintenance->setItem(i, 1, Item);
        Item=new QTableWidgetItem(ObtenirValeurParNom(Requete,"DescriptionPieces").toString());
        Item->setFlags(Item->flags() & ~Qt::ItemIsEditable);
        ui->Tw_HistoriqueMaintenance->setItem(i, 2, Item);
        ui->Tw_HistoriqueMaintenance->setItem(i, 3, new QTableWidgetItem(ObtenirValeurParNom(Requete,"NombrePiecesManquantes").toString()));
        Item = new QTableWidgetItem();
        Item->setFlags(Item->flags() | Qt::ItemIsUserCheckable );
        Item->setFlags(Item->flags() & ~Qt::ItemIsEditable);
        if(ObtenirValeurParNom(Requete,"Abimee").toBool())
        {
            Item->setCheckState(Qt::Checked);
        }
        else
        {
            Item->setCheckState(Qt::Unchecked);
        }

        ui->Tw_HistoriqueMaintenance->setItem(i, 4, Item);
        ui->Tw_HistoriqueMaintenance->setItem(i, 5, new QTableWidgetItem(ObtenirValeurParNom(Requete,"RemarquePiecesManquantes").toString()));
        ui->Tw_HistoriqueMaintenance->blockSignals(false);
        i++;
    }
    ui->Tw_HistoriqueMaintenance->resizeColumnsToContents();
    ui->Tw_HistoriqueMaintenance->setColumnWidth(5,300);
    ui->Bt_SupprimerEvenement->setEnabled(false);
}

void W_HistoriqueMaintenance::on_Bt_SupprimerEvenement_clicked()
{
    if(ui->Tw_HistoriqueMaintenance->selectedItems().count()==0)
    {
        return;
    }
    QSqlQuery Requete;
    Requete.prepare("DELETE FROM piecesmanquantes WHERE IdPiecesManquantes=:IdPiecesManquantes");
    Requete.bindValue(":IdPiecesManquantes",ui->Tw_HistoriqueMaintenance->item(ui->Tw_HistoriqueMaintenance
                                             ->selectedItems().at(0)->row(),0)->data(Qt::UserRole).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    ActualiserHistoriqueMaintenance();
}

void W_HistoriqueMaintenance::editingStartedHistorique()
{
    if(ui->Tw_HistoriqueMaintenance->selectedItems().count()>0)
    {
        ui->Bt_SupprimerEvenement->setEnabled(true);
    }
}

void W_HistoriqueMaintenance::on_Tw_HistoriqueMaintenance_itemChanged(QTableWidgetItem *item)
{
    QSqlQuery Requete;
    QString Champs;
    QString Valeur;
    // Si on est sur la 2° colonne, on mets à jour la description sinon c'est le nombre
    switch(item->column())
    {
        case 3:
            Champs="NombrePiecesManquantes";
            Valeur=item->text();
            break;
        case 4:
            Champs="Abimee";
            Valeur=QString::number(item->checkState()==Qt::Checked?1:0);
            break;
        case 5:
            Champs="RemarquePiecesManquantes";
            Valeur=item->text();
            break;
    }
    Requete.prepare("UPDATE piecesmanquantes SET "+Champs+"=:Valeur WHERE IdPiecesManquantes=:IdPiecesManquantes");
    Requete.bindValue(":Valeur",Valeur);
    Requete.bindValue(":IdPiecesManquantes",ui->Tw_HistoriqueMaintenance->item(item->row(),0)->data(Qt::UserRole).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    emit(Signal_ActualiserContenu());
}

void W_HistoriqueMaintenance::Vider()
{
    ui->Tw_HistoriqueMaintenance->clear();
    ui->Tw_HistoriqueMaintenance->setColumnCount(0);
}
