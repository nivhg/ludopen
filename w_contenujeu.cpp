#include "w_contenujeu.h"
#include "ui_w_contenujeu.h"

W_ContenuJeu::W_ContenuJeu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::W_ContenuJeu)
{
    ui->setupUi(this);
    QMenu *menu = new QMenu;
    QAction *actionGroupe=menu->addAction("un groupe de pièces");
    actionGroupe->setData("groupe");
    QAction *actionPiece=menu->addAction("une pièce");
    actionPiece->setData("piece");
    actionPieceDsGroupe=menu->addAction("une pièce dans ce groupe");
    actionPieceDsGroupe->setData("pieceDsGroupe");
    actionPieceDsGroupe->setEnabled(false);
    QAction *actionSaisie=menu->addAction("un ensemble d'élements à saisir");
    actionSaisie->setData("saisie");
    ui->TB_Ajouter->setMenu(menu);
    ui->TB_Ajouter->setPopupMode(QToolButton::InstantPopup);
    connect(actionGroupe,SIGNAL(triggered()),this,SLOT(on_menuAjouterPiece_triggered()));
    connect(actionPiece,SIGNAL(triggered()),this,SLOT(on_menuAjouterPiece_triggered()));
    connect(actionPieceDsGroupe,SIGNAL(triggered()),this,SLOT(on_menuAjouterPiece_triggered()));
    connect(actionSaisie,SIGNAL(triggered()),this,SLOT(on_menuAjouterPiece_triggered()));

    //Associe le modèle au TableView
    ModeleContenu=new QStandardItemModel();
    ui->Tv_Contenu->setModel(this->ModeleContenu);
    DelegateContenu=new SpinBoxDelegate(2, 1, true,this);
    ui->Tv_Contenu->setColumnWidth(0,140);
    ui->Tv_Contenu->setColumnWidth(1,40);
    ui->Tv_Contenu->setItemDelegate(DelegateContenu);

    connect(ModeleContenu,SIGNAL(itemChanged(QStandardItem *)),this,SLOT(on_Tv_Contenu_itemChanged(QStandardItem *)));

    QItemSelectionModel *selectionModel = ui->Tv_Contenu->selectionModel();

    connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)), this,
            SLOT(SelectionChanged_Tv_Contenu(const QItemSelection&,const QItemSelection&)));
    ui->TB_Valider->setVisible(false);
    ui->TB_Annuler->setVisible(false);
    ui->Te_Contenu->setVisible(false);
}

W_ContenuJeu::~W_ContenuJeu()
{
    delete ui;
}

void W_ContenuJeu::Definir_CodeJeu(QString CodeJeu)
{
    this->CodeJeu=CodeJeu;
}

void W_ContenuJeu::Definir_Main(F_MainWindow *Main)
{
    this->main=Main;
}

void W_ContenuJeu::Definir_Mode(int mode)
{
    this->Mode=mode;
    if(Mode==MODE_CONTENU_ET_MANQUANT)
    {
        ui->Lb_Contenu->setVisible(false);
        ui->W_Boutons->setVisible(false);
    }
}

/*void W_ContenuJeu::initialize(QDesignerFormEditorInterface *core )
{
    if (initialized)
        return;

    initialized = true;
}

bool W_ContenuJeu::isInitialized() const
{
    return initialized;
}

QWidget *W_ContenuJeu::createWidget(QWidget *parent)
{
    return new W_ContenuJeu(parent);
}

QString W_ContenuJeu::name() const
{
    return "AnalogClock";
}

QString W_ContenuJeu::group() const
{
    return "Display Widgets [Examples]";
}

QIcon W_ContenuJeu::icon() const
{
    return QIcon();
}

QString W_ContenuJeu::toolTip() const
{
    return "";
}

QString W_ContenuJeu::whatsThis() const
{
    return "";
}

bool W_ContenuJeu::isContainer() const
{
    return false;
}

QString W_ContenuJeu::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"AnalogClock\" name=\"analogClock\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>100</width>\n"
           "    <height>100</height>\n"
           "   </rect>\n"
           "  </property>\n"
           "  <property name=\"toolTip\" >\n"
           "   <string>The current time</string>\n"
           "  </property>\n"
           "  <property name=\"whatsThis\" >\n"
           "   <string>The analog clock widget displays the current time.</string>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>\n";
}

QString W_ContenuJeu::includeFile() const
{
    return "analogclock.h";
}
*/
//Q_EXPORT_PLUGIN2(customwidgetplugin, AnalogClockPlugin);

void W_ContenuJeu::on_menuAjouterPiece_triggered()
{
    QSqlQuery Requete;
    QAction *menu = qobject_cast<QAction *>(sender());
    QList<QStandardItem *> Liste;
    QStandardItem *Item=new QStandardItem();
    QStandardItem *Item2=new QStandardItem("");
    Item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
    Requete.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu");
    Requete.bindValue(":CodeJeu", CodeJeu);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    Requete.next();
    int IdJeux=ObtenirValeurParNom(Requete,"IdJeux").toInt();
    if(menu->data().toString()=="groupe")
    {
        Requete.prepare("INSERT INTO pieces(OrdrePieces,NombrePieces,DescriptionPieces,PieceGroupe,IdJeuxOuIdPieces) VALUES "
                        "(:OrdrePieces,NULL,NULL,1,(SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu))");
        Requete.bindValue(":OrdrePieces", ModeleContenu->rowCount());
        Requete.bindValue(":CodeJeu", CodeJeu);
        if(!Requete.exec())
        {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
        }

        Item->setData(GROUPE,PIECEGROUPE);
        Item->setData(IdJeux,IDJEUXOUIDPIECES);
        Item2->setIcon(QIcon(":/Sac.svg"));
        Liste<<Item<<Item2;
        ModeleContenu->appendRow(Liste);
        actionPieceDsGroupe->setEnabled(true);
    }
    else if(menu->data().toString()=="saisie")
    {
        ui->TB_Valider->setVisible(true);
        ui->TB_Annuler->setVisible(true);
        ui->Tv_Contenu->setVisible(false);
        ui->Te_Contenu->setVisible(true);
        ui->Te_Contenu->setEnabled(true);
        QMessageBox::information(this,"Recommandations","Merci de suivre les recommandations suivantes afin que LudOpen reconnaisse correctement la saisie :"
               "\n- Ne pas mettre plusieurs éléments de jeux sur la même ligne"
               "\n- Les éléments d'un groupe doivent commencer par une tabulation (touche avec la double flèche à gauche du A");
    }
    else
    {
        Requete.prepare("INSERT INTO pieces(OrdrePieces,NombrePieces,DescriptionPieces,PieceGroupe,IdJeuxOuIdPieces) VALUES "
                        "(:OrdrePieces,NULL,NULL,:PieceGroupe,:IdGroupeOuJeu)");

        // Si l'item en cours est un groupe (n'a pas de parent et que son UserRole vaut 1) et que l'utilisateur a choisi le menu d'ajout dans un groupe
        if(ui->Tv_Contenu->currentIndex().parent()==QModelIndex()&&ModeleContenu->index(ui->Tv_Contenu->currentIndex().row(),0).data(PIECEGROUPE)==1
                &&menu->data().toString()=="pieceDsGroupe")
        {
            int IdGroupe=ModeleContenu->itemFromIndex(ui->Tv_Contenu->currentIndex())->data(IDPIECES).toInt();
            Item->setData(IdGroupe,IDJEUXOUIDPIECES);
            Liste<<Item<<Item2;
            qDebug()<<ui->Tv_Contenu->currentIndex();
            qDebug()<<ModeleContenu->itemFromIndex(ui->Tv_Contenu->currentIndex());
            ModeleContenu->itemFromIndex(ui->Tv_Contenu->currentIndex())->appendRow(Liste);
            ui->Tv_Contenu->expand(ui->Tv_Contenu->currentIndex());
            Item->setData(PIECE_DANS_GROUPE,PIECEGROUPE);
            Requete.bindValue(":PieceGroupe",PIECE_DANS_GROUPE);
            Requete.bindValue(":IdGroupeOuJeu", ModeleContenu->index(ui->Tv_Contenu->currentIndex().row(),0).data(IDPIECES));
            Requete.bindValue(":OrdrePieces", ModeleContenu->indexFromItem(Item).row());
            Item2->setIcon(QIcon(":/ItemDansSac.svg"));
        }
        else
        {
            Item->setData(IdJeux,IDJEUXOUIDPIECES);
            Item->setData(PIECE_SEULE,PIECEGROUPE);
            Requete.bindValue(":OrdrePieces", ModeleContenu->rowCount());
            Requete.bindValue(":PieceGroupe",PIECE_SEULE);
            Requete.bindValue(":IdGroupeOuJeu", IdJeux);
            Liste<<Item<<Item2;
            ModeleContenu->appendRow(Liste);
            Item2->setIcon(QIcon(":/Item.svg"));
        }
        if(!Requete.exec())
        {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
        }
    }
    Item->setData(Requete.lastInsertId(),IDPIECES);
    ui->Tv_Contenu->setCurrentIndex(ModeleContenu->indexFromItem(Item));
    ui->Tv_Contenu->edit(ui->Tv_Contenu->currentIndex());
}

void W_ContenuJeu::ActualiserContenu(int IdPieces)
{
    QSqlQuery Requete;
    Requete.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu");
    Requete.bindValue(":CodeJeu",CodeJeu);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    Requete.next();
    int IdJeux=ObtenirValeurParNom(Requete,"IdJeux").toInt();

    Requete.prepare("SELECT *,IF(PieceGroupe=2,"
                    "(SELECT CONCAT(OrdrePieces,'.',p.OrdrePieces+1) FROM pieces WHERE p.IdJeuxOuIdPieces=IdPieces),OrdrePieces) as OrdreGroupePieces,"
                    "(SELECT SUM(NombrePiecesManquantes) FROM piecesmanquantes WHERE IdPieces=IdPieces_Pieces AND Abimee=0) as NombrePiecesManquantes "
                    "FROM pieces as p WHERE IdJeuxOuIdPieces=:IdJeux Or (PieceGroupe=2 AND IdJeuxOuIdPieces IN "
                    "(SELECT IdPieces FROM pieces WHERE IdJeuxOuIdPieces=:IdJeux)) ORDER By OrdreGroupePieces");
    Requete.bindValue(":IdJeux",IdJeux);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    ui->Tv_Contenu->blockSignals(true);
    ModeleContenu->clear();
    //Initialise les colones du TableView des nouveaux emprunts
    int ColNombre=1;
    this->ModeleContenu->setColumnCount(3);
    this->ModeleContenu->setHorizontalHeaderItem(0, new QStandardItem("Libellé"));
    this->ModeleContenu->setHorizontalHeaderItem(1, new QStandardItem("Nombre"));
    QStandardItem *Item=new QStandardItem("Manquant (abimé)");
    Item->setBackground(QColor(0, 255, 0));
    this->ModeleContenu->setHorizontalHeaderItem(2, Item);
    int DernierIdGroupe=0;
    QStandardItem *DernierGroupe;
    while(Requete.next())
    {
        int PieceGroupe=ObtenirValeurParNom(Requete,"PieceGroupe").toInt();
        QList<QStandardItem *>liste;
        QStandardItem *Item,*Item2;
        Item=new QStandardItem(ObtenirValeurParNom(Requete,"DescriptionPieces").toString());
        Item->setData(PieceGroupe,PIECEGROUPE);
        Item->setData(ObtenirValeurParNom(Requete,"IdPieces").toInt(),IDPIECES);
        Item->setData(ObtenirValeurParNom(Requete,"IdJeuxOuIdPieces").toInt(),IDJEUXOUIDPIECES);
        Item2=new QStandardItem(ObtenirValeurParNom(Requete,"NombrePiecesManquantes").toInt());
        Item2->setData(ObtenirValeurParNom(Requete,"NombrePiecesManquantes").toInt(),NBPIECESMANQUANTES);
        Item2->setData(ObtenirValeurParNom(Requete,"NombrePiecesManquantes").toInt(),Qt::DisplayRole);
        liste<<Item<<new QStandardItem(ObtenirValeurParNom(Requete,"NombrePieces").toInt())<<Item2;
        // S'il s'agit d'un groupe
        if(PieceGroupe==1)
        {
            Item2->setIcon(QIcon(":/Sac.svg"));
            DernierIdGroupe=ObtenirValeurParNom(Requete,"IdPieces").toInt();
            DernierGroupe=Item;
        }
        else
        {
            // Si IdJeuxOuIdPieces est égale à l'id du dernier groupe vu, il s'agit d'une pièce d'un groupe
            if(ObtenirValeurParNom(Requete,"IdJeuxOuIdPieces").toInt()==DernierIdGroupe)
            {
                ModeleContenu->blockSignals(true);
                DernierGroupe->appendRow(liste);
                if(Requete.value("NombrePieces").isNull())
                {
                    DernierGroupe->child(DernierGroupe->rowCount()-1,ColNombre)->setData(QVariant(QVariant::Int),Qt::DisplayRole);
                }
                else
                {
                    DernierGroupe->child(DernierGroupe->rowCount()-1,ColNombre)->setData(ObtenirValeurParNom(Requete,"NombrePieces").toInt(),Qt::DisplayRole);
                }
                ui->Tv_Contenu->expand(ModeleContenu->indexFromItem(DernierGroupe));
                Item2->setIcon(QIcon(":/ItemDansSac.svg"));
                ModeleContenu->blockSignals(false);
                if(Item->data(IDPIECES).toInt()==IdPieces)
                {
                    ui->Tv_Contenu->setCurrentIndex(ModeleContenu->indexFromItem(Item));
                    ui->Tv_Contenu->edit(ui->Tv_Contenu->currentIndex());
                }
                continue;
            }
            else
            {
                Item2->setIcon(QIcon(":/Item.svg"));
            }
        }
        ModeleContenu->blockSignals(true);
        ModeleContenu->appendRow(liste);
        QModelIndex index=ModeleContenu->indexFromItem(Item);
        if(Item->data(IDPIECES).toInt()==IdPieces)
        {
            ui->Tv_Contenu->setCurrentIndex(index);
            ui->Tv_Contenu->edit(ui->Tv_Contenu->currentIndex());
        }
        if(Requete.value("NombrePieces").isNull())
        {
            ModeleContenu->itemFromIndex(index.sibling(index.row(),ColNombre))->setData(QVariant(QVariant::Int),Qt::DisplayRole);
        }
        else
        {
            ModeleContenu->itemFromIndex(index.sibling(index.row(),ColNombre))->setData(ObtenirValeurParNom(Requete,"NombrePieces").toInt(),Qt::DisplayRole);
        }
        ModeleContenu->blockSignals(false);
    }
    ActiverBoutonsContenu(true);
    ui->Tv_Contenu->setColumnWidth(0,370);
    ui->Tv_Contenu->setColumnWidth(1,70);
    ui->Tv_Contenu->setColumnWidth(2,60);
    emit(Signal_ActualiserHistoriqueMaintenance());
    ui->Tv_Contenu->blockSignals(false);
}

void W_ContenuJeu::on_Tv_Contenu_itemChanged(QStandardItem *item)
{
    QModelIndex index=ModeleContenu->indexFromItem(item).sibling(item->row(),0);
    QSqlQuery Requete;
    if(Mode==MODE_CONTENU)
    {
        QString Champs;
        // Si on est sur la 2° colonne, on mets à jour la description sinon c'est le nombre
        if(ModeleContenu->indexFromItem(item).column()==1)
        {
            Champs="DescriptionPieces";
        }
        else
        {
            Champs="NombrePieces";
        }
        Requete.prepare("UPDATE pieces SET "+Champs+"=:Valeur WHERE IdPieces=:IdPieces");
        Requete.bindValue(":Valeur",item->text());
        Requete.bindValue(":IdPieces",index.data(IDPIECES).toInt());
        if(!Requete.exec())
        {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
        }
    }
    else
    {
        Requete.prepare("INSERT INTO piecesmanquantes(DatePiecesManquantes,IdMembre_Membres,"
                        "IdPieces_Pieces,NombrePiecesManquantes,RemarquePiecesManquantes) VALUES (:DatePiecesManquantes,"
                        ":IdMembre_Membres,:IdPieces_Pieces,:NombrePiecesManquantes,:RemarquePiecesManquantes)");
        // Si on est sur la 1° colonne, on mets à jour la description sinon c'est le nombre
        Requete.bindValue(":DatePiecesManquantes",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        Requete.bindValue(":IdMembre_Membres",main->RecupereIdBenevole());
        Requete.bindValue(":IdPieces_Pieces",index.data(Qt::UserRole+1));
        QModelIndex index2=ModeleContenu->indexFromItem(item).sibling(item->row(),item->column());
        int NbPiecesManquantes=item->text().toInt()-index2.data(Qt::UserRole+3).toInt();
        Requete.bindValue(":NombrePiecesManquantes",NbPiecesManquantes);
        if(NbPiecesManquantes<0)
        {
            Requete.bindValue(":RemarquePiecesManquantes","Pièce(s) rendue");
        }
        else
        {
            Requete.bindValue(":RemarquePiecesManquantes","Au départ du jeu");
        }
        if(!Requete.exec())
        {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
        }
        emit(Signal_ActualiserHistoriqueMaintenance());
    }
}

void W_ContenuJeu::SelectionChanged_Tv_Contenu(const QItemSelection&selected,const QItemSelection&deselected)
{
    // On active les boutons contenu si un élement a été sélectionné, sinon on les désactive
    ActiverBoutonsContenu(true);

    // Si l'élement choisi n'a pas de parent et qu'il s'agit d'un groupe
    if(ui->Tv_Contenu->currentIndex().parent()==QModelIndex()&&
            ModeleContenu->index(ui->Tv_Contenu->currentIndex().row(),0).data(PIECEGROUPE)==GROUPE)
    {
        actionPieceDsGroupe->setEnabled(true);
    }
    else
    {
        actionPieceDsGroupe->setEnabled(false);
    }
}

void W_ContenuJeu::InverserElement(QModelIndex ElementChoisi,int LigneDestination)
{
    QSqlQuery Requete;
    Requete.prepare("UPDATE pieces SET OrdrePieces=:OrdrePieces WHERE IdPieces=:IdPieces");
    Requete.bindValue(":OrdrePieces",LigneDestination);
    Requete.bindValue(":IdPieces",ElementChoisi.data(IDPIECES).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    QModelIndex ElementDestination=ElementChoisi.sibling(LigneDestination,0);
    Requete.prepare("UPDATE pieces SET OrdrePieces=:OrdrePieces WHERE IdPieces=:IdPieces");
    Requete.bindValue(":OrdrePieces",ElementChoisi.row());
    Requete.bindValue(":IdPieces",ElementDestination.data(IDPIECES).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
}

void W_ContenuJeu::ActiverBoutonsContenu(bool Etat)
{
    ui->TB_Ajouter->setEnabled(Etat);
    if(Etat)
    {
        ui->TB_Supprimer->setEnabled(ui->Tv_Contenu->currentIndex()!=QModelIndex());
        QModelIndex ElementChoisi=ui->Tv_Contenu->currentIndex().sibling(ui->Tv_Contenu->currentIndex().row(),0);
        ui->TB_Haut->setEnabled(ElementChoisi.data(PIECEGROUPE).toInt()==PIECE_DANS_GROUPE||ElementChoisi.row()>0);
        ui->TB_SacItem->setEnabled(ElementChoisi.data(PIECEGROUPE).toInt()!=PIECE_DANS_GROUPE);
        ui->TB_Bas->setEnabled(ElementChoisi!=QModelIndex()&&ElementChoisi.row()<ModeleContenu->rowCount()-1);
    }
    else
    {
        ui->TB_Supprimer->setEnabled(Etat);
        ui->TB_Haut->setEnabled(false);
        ui->TB_Bas->setEnabled(false);
    }
}

void W_ContenuJeu::on_TB_Haut_clicked()
{
    QModelIndex ElementChoisi=ui->Tv_Contenu->currentIndex().sibling(ui->Tv_Contenu->currentIndex().row(),0);
    int LigneElement=ElementChoisi.row();
    QModelIndex ElementPrecedent=ElementChoisi.sibling(ElementChoisi.row()-1,0);
    int IdPieces=0;
    // S'il s'agit de la première pièce d'un groupe qu'on déplace en haut, on le sort du groupe et on le place en haut
    if(ElementChoisi.data(PIECEGROUPE)==PIECE_DANS_GROUPE&&ElementChoisi.row()==0)
    {
            IdPieces=MettreAjourElement(ElementChoisi,ElementChoisi.parent().row(),ElementChoisi.parent().data(IDJEUXOUIDPIECES).toInt(),PIECE_SEULE);
            // On décale vers le bas l'ordre des élements suivants
            QSqlQuery Requete;
            Requete.prepare("UPDATE pieces SET OrdrePieces=OrdrePieces-1 WHERE "
                            "IdJeuxOuIdPieces=:IdJeuxOuIdPieces AND PieceGroupe=2");
            Requete.bindValue(":IdJeuxOuIdPieces",ElementChoisi.parent().data(IDPIECES).toInt());
            if(!Requete.exec())
            {
                qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
            }
    }
    // S'il s'agit d'un item qu'on déplace en haut et que la pièce précédente est un groupe, on la mets dans le groupe
    else if(ElementChoisi.data(PIECEGROUPE)==PIECE_SEULE&&ElementPrecedent.data(PIECEGROUPE).toInt()==GROUPE)
    {
            IdPieces=MettreAjourElement(ElementChoisi,ModeleContenu->rowCount(ElementPrecedent),ElementPrecedent.data(IDPIECES).toInt(),PIECE_DANS_GROUPE);
    }
    else
    {
        InverserElement(ElementChoisi,LigneElement-1);
        IdPieces=ElementChoisi.data(IDPIECES).toInt();
    }
    ActualiserContenu(IdPieces);
}

void W_ContenuJeu::on_TB_Bas_clicked()
{
    QModelIndex ElementChoisi=ui->Tv_Contenu->currentIndex().sibling(ui->Tv_Contenu->currentIndex().row(),0);
    QModelIndex ElementSuivant=ElementChoisi.sibling(ElementChoisi.row()+1,0);
    int LigneElement=ElementChoisi.row();
    int IdPieces=0;
    // S'il s'agit de la dernière pièce d'un groupe qu'on déplace en bas, on le sort du groupe et on le place en bas
    if(ElementChoisi.data(PIECEGROUPE)==PIECE_DANS_GROUPE&&ElementChoisi.row()==ModeleContenu->rowCount(ElementChoisi.parent())-1)
    {
            IdPieces=MettreAjourElement(ElementChoisi,ElementChoisi.parent().row()+1,ElementChoisi.parent().data(IDJEUXOUIDPIECES).toInt(),PIECE_SEULE);
    }
    // S'il s'agit d'un item qu'on déplace en bas et que la pièce suivante est un groupe, on la mets dans le groupe
    else if(ElementChoisi.data(PIECEGROUPE)==PIECE_SEULE&&ElementSuivant.data(PIECEGROUPE).toInt()==GROUPE)
    {
            IdPieces=MettreAjourElement(ElementChoisi,ModeleContenu->rowCount(ElementSuivant),ElementSuivant.data(IDPIECES).toInt(),PIECE_DANS_GROUPE);
            // On décale vers le bas l'ordre des élements suivants
            QSqlQuery Requete;
            Requete.prepare("UPDATE pieces SET OrdrePieces=OrdrePieces-1 WHERE "
                            "IdJeuxOuIdPieces=:IdJeuxOuIdPieces AND OrdrePieces>:OrdrePieces");
            Requete.bindValue(":OrdrePieces",ElementChoisi.row());
            Requete.bindValue(":IdJeuxOuIdPieces",ElementChoisi.data(IDJEUXOUIDPIECES).toInt());
            if(!Requete.exec())
            {
                qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
            }
    }
    else
    {
        InverserElement(ElementChoisi,LigneElement+1);
        IdPieces=ElementChoisi.data(IDPIECES).toInt();
    }
    ActualiserContenu(IdPieces);
}

void W_ContenuJeu::SupprimerElement(QModelIndex ElementChoisi)
{
    QSqlQuery Requete;
    Requete.prepare("DELETE FROM pieces WHERE IdPieces=:IdPieces") ;
    Requete.bindValue(":IdPieces", ElementChoisi.data(IDPIECES).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
}

int W_ContenuJeu::MettreAjourElement(QModelIndex ElementChoisi,int LigneDestination,int IdJeux,int PieceGroupe)
{
    QSqlQuery Requete;
    // On décale les élements suivants pour pouvoir insérer le nouvel élément
    if(PieceGroupe!=PIECE_DANS_GROUPE)
    {
        Requete.prepare("UPDATE pieces SET OrdrePieces=OrdrePieces+1 WHERE "
                        "IdJeuxOuIdPieces=:IdJeuxOuIdPieces AND OrdrePieces>=:OrdrePieces");
        Requete.bindValue(":OrdrePieces",LigneDestination);
        Requete.bindValue(":IdJeuxOuIdPieces",IdJeux);
        if(!Requete.exec())
        {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
        }
    }
    Requete.prepare("UPDATE pieces SET OrdrePieces=:OrdrePieces, NombrePieces=:NombrePieces,DescriptionPieces=:DescriptionPieces,"
                    "PieceGroupe=:PieceGroupe,IdJeuxOuIdPieces=:IdJeuxOuIdPieces WHERE IdPieces=:IdPieces");
    Requete.bindValue(":OrdrePieces",LigneDestination);
    Requete.bindValue(":NombrePieces",ElementChoisi.data(Qt::DisplayRole));
    Requete.bindValue(":DescriptionPieces",ElementChoisi.sibling(ElementChoisi.row(),1).data(Qt::DisplayRole));
    Requete.bindValue(":PieceGroupe",PieceGroupe);
    Requete.bindValue(":IdJeuxOuIdPieces",IdJeux);
    Requete.bindValue(":IdPieces",ElementChoisi.data(IDPIECES).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    return ElementChoisi.data(IDPIECES).toInt();
}

/*
void W_ContenuJeu::on_TB_Droite_clicked()
{
    QModelIndex ElementChoisi=ui->Tv_Contenu->currentIndex();
    int FrereHaut=ElementChoisi.sibling(ElementChoisi.row()-1,0).data(PIECEGROUPE).toInt();
    int FrereBas=ElementChoisi.sibling(ElementChoisi.row()+1,0).data(PIECEGROUPE).toInt();
    // Si le frère haut est un groupe, on ajoute la pièce à ce groupe, sinon il faut utiliser le frère bas
    QModelIndex Frere;
    if(FrereHaut==GROUPE)
    {
        Frere=ElementChoisi.sibling(ElementChoisi.row()-1,0);
    }
    else
    {
        Frere=ElementChoisi.sibling(ElementChoisi.row()+1,0);
    }
    QSqlQuery Requete;
    Requete.prepare("UPDATE pieces SET OrdrePieces=:OrdrePieces,IdJeuxOuIdPieces=:IdJeuxOuIdPieces,PieceGroupe=:PieceGroupe WHERE IdPieces=:IdPieces");
    Requete.bindValue(":OrdrePieces",ModeleContenu->itemFromIndex(Frere)->rowCount());
    Requete.bindValue(":IdJeuxOuIdPieces",Frere.data(IDPIECES).toInt());
    Requete.bindValue(":PieceGroupe",2);
    Requete.bindValue(":IdPieces",ElementChoisi.sibling(ElementChoisi.row(),0).data(IDPIECES).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }

    // S'il s'agit du premier élément, il n'y a rien à faire
    ActualiserContenu();
}

void W_ContenuJeu::on_TB_Gauche_clicked()
{
    QModelIndex ElementChoisi=ui->Tv_Contenu->currentIndex();

    QModelIndex Pere=ElementChoisi.parent();
    QSqlQuery Requete;
    Requete.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu");
    Requete.bindValue(":CodeJeu",CodeJeu);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    Requete.next();
    int IdJeux=ObtenirValeurParNom(Requete,"IdJeux").toInt();

    for(int i=Pere.row()+1;i<ModeleContenu->rowCount();i++)
    {
        Requete.prepare("UPDATE pieces SET OrdrePieces=:OrdrePieces WHERE IdPieces=:IdPieces");
        Requete.bindValue(":OrdrePieces",i+1);
        Requete.bindValue(":IdPieces",ModeleContenu->item(i,0)->data(IDPIECES).toInt());
        if(!Requete.exec())
        {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
        }
    }
    Requete.prepare("UPDATE pieces SET OrdrePieces=:OrdrePieces,IdJeuxOuIdPieces=:IdJeuxOuIdPieces,PieceGroupe=:PieceGroupe WHERE IdPieces=:IdPieces");
    Requete.bindValue(":OrdrePieces",Pere.row()+1);
    Requete.bindValue(":IdJeuxOuIdPieces",IdJeux);
    Requete.bindValue(":PieceGroupe",0);
    Requete.bindValue(":IdPieces",ElementChoisi.sibling(ElementChoisi.row(),0).data(IDPIECES).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }

    // S'il s'agit du premier élément, il n'y a rien à faire
    ActualiserContenu();
}*/

void W_ContenuJeu::Vider()
{
    ModeleContenu->clear();
}

void W_ContenuJeu::on_TB_Supprimer_clicked()
{
    QModelIndex ElementChoisi=ui->Tv_Contenu->currentIndex().sibling(ui->Tv_Contenu->currentIndex().row(),0);

    int IdPieces;
    if(ElementChoisi.data(PIECEGROUPE).toInt()==GROUPE&&QMessageBox::question(this, "Confirmation",
            "Êtes-vous sûr de vouloir supprimer ce groupe ainsi que toutes les pièces qu'il contient ?", "Oui", "Non") != 0)
    {
        return;
    }

    QSqlQuery Requete;
    SupprimerPiece(ElementChoisi.sibling(ElementChoisi.row(),0).data(IDPIECES).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    for(int i=ElementChoisi.row()+1;i<ModeleContenu->rowCount();i++)
    {
        Requete.prepare("UPDATE pieces SET OrdrePieces=:OrdrePieces WHERE IdPieces=:IdPieces");
        Requete.bindValue(":OrdrePieces",i-1);
        Requete.bindValue(":IdPieces",ModeleContenu->item(i,0)->data(IDPIECES).toInt());
        if(!Requete.exec())
        {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
        }
    }
    if(ElementChoisi.data(PIECEGROUPE).toInt()==GROUPE)
    {
        int Fin=ModeleContenu->rowCount(ElementChoisi)-1;
        for(int i=Fin;i>=0;i--)
        {
            SupprimerPiece(ElementChoisi.child(i,0).data(IDPIECES).toInt());
        }
    }
    ActualiserContenu();
}

void W_ContenuJeu::SupprimerPiece(int IdPieces)
{
    QSqlQuery Requete;
    Requete.prepare("DELETE FROM pieces WHERE IdPieces=:IdPieces") ;
    Requete.bindValue(":IdPieces",IdPieces);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    Requete.prepare("DELETE FROM piecesmanquantes WHERE IdPieces_Pieces=:IdPieces") ;
    Requete.bindValue(":IdPieces",IdPieces);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
}
void W_ContenuJeu::on_TB_SacItem_clicked()
{
    QModelIndex ElementChoisi=ui->Tv_Contenu->currentIndex().sibling(ui->Tv_Contenu->currentIndex().row(),0);
    qDebug()<<ElementChoisi.data(PIECEGROUPE).toInt()<<ModeleContenu->rowCount(ElementChoisi);
    if(ElementChoisi.data(PIECEGROUPE).toInt()==GROUPE&&ModeleContenu->rowCount(ElementChoisi)>0)
    {
        QMessageBox::information(this,"Pas géré","Il n'est pas possible de convertir un groupe rempli de pièce en pièce."
                                 "\nMerci de le vider avant.");
        return;
    }
    int Destination;
    if(ElementChoisi.data(PIECEGROUPE).toInt()==PIECE_SEULE)
    {
        Destination=GROUPE;
    }
    else
    {
        Destination=PIECE_SEULE;
    }
    QSqlQuery Requete;
    Requete.prepare("UPDATE pieces SET PieceGroupe=:PieceGroupe WHERE IdPieces=:IdPieces");
    Requete.bindValue(":PieceGroupe",Destination);
    Requete.bindValue(":IdPieces",ElementChoisi.data(IDPIECES).toInt());
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    ActualiserContenu(ElementChoisi.data(IDPIECES).toInt());
}

void W_ContenuJeu::on_TB_Valider_clicked()
{
    QStringList lignes = ui->Te_Contenu->toPlainText().split("\n");
    bool GroupeTrouve=false;
    int IdGroupeTrouve=0;
    int i=0;
    QSqlQuery Requete;
    Requete.prepare("SELECT IdJeux FROM jeux WHERE CodeJeu=:CodeJeu");
    Requete.bindValue(":CodeJeu", CodeJeu);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    Requete.next();
    int IdJeux=ObtenirValeurParNom(Requete,"IdJeux").toInt();

    Requete.prepare("SELECT MAX(OrdrePieces) as MaxOrdre FROM pieces WHERE IdJeuxOuIdPieces=:IdJeux AND PieceGroupe!=2");
    Requete.bindValue(":IdJeux", IdJeux);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    Requete.next();
    int MaxOrdre=ObtenirValeurParNom(Requete,"MaxOrdre").toInt();

    foreach (QString ligne, lignes)
    {
        if(i==0)
        {
            i++;
            continue;
        }
        if(ligne.trimmed()=="") continue;
        QString LignePrecedente=lignes.at(i-1);
        QStringList Piece=NettoyerEtCouperLigne(LignePrecedente);
        // Si le premier caractère de la ligne est une tabulation, et qu'aucun groupe n'a été trouvé, on crée le groupe à partir de la ligne précédente
        if(!GroupeTrouve&&ligne.at(0).toAscii()==9)
        {
            GroupeTrouve=true;
            IdGroupeTrouve=InsererPiece(Piece[0],MaxOrdre+i,Piece[1],1,IdJeux);
        }
        else
        {   // Si un groupe a été trouvé
            if(GroupeTrouve)
            {
                InsererPiece(Piece[0],MaxOrdre+i,Piece[1],2,IdGroupeTrouve);

                // Si il n'y a plus de tabulation, c'est que le groupe est fini
                if(ligne.at(0).toAscii()!=9)
                {
                    GroupeTrouve=false;
                    IdGroupeTrouve=0;
                }
            }
            else
            {
                InsererPiece(Piece[0],MaxOrdre+i,Piece[1],0,IdJeux);
            }
        }
        i++;
    }
    // On crée la dernière pièce
    QString LignePrecedente=lignes.at(i-1);
    on_TB_Annuler_clicked();
    if(LignePrecedente=="") return;
    QStringList Piece=NettoyerEtCouperLigne(LignePrecedente);
    // Si le premier caractère de la ligne est une tabulation, et qu'aucun groupe n'a été trouvé, on crée le groupe à partir de la ligne précédente
    // Si un groupe a été trouvé
    if(GroupeTrouve)
    {
        InsererPiece(Piece[0],MaxOrdre+i,Piece[1],2,IdGroupeTrouve);
    }
    else
    {
        InsererPiece(Piece[0],MaxOrdre+i,Piece[1],0,IdJeux);
    }
    ActualiserContenu();
}

int W_ContenuJeu::InsererPiece(QString NombrePieces,int OrdrePieces,QString DescriptionPieces,int PieceGroupe,int IdJeuxOuIdPieces)
{
    QSqlQuery Requete;
    Requete.prepare("INSERT INTO pieces(OrdrePieces,NombrePieces,DescriptionPieces,PieceGroupe,IdJeuxOuIdPieces) VALUES "
                    "(:OrdrePieces,:NombrePieces,"
                    ":DescriptionPieces,:PieceGroupe,:IdJeuxOuIdPieces)");
    if(NombrePieces=="")
    {
        Requete.bindValue(":NombrePieces", QVariant(QVariant::Int));
    }
    else
    {
        Requete.bindValue(":NombrePieces", NombrePieces);
    }
    Requete.bindValue(":OrdrePieces", OrdrePieces);
    Requete.bindValue(":DescriptionPieces", DescriptionPieces);
    Requete.bindValue(":PieceGroupe", PieceGroupe);
    Requete.bindValue(":IdJeuxOuIdPieces", IdJeuxOuIdPieces);
    if(!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    return Requete.lastInsertId().toInt();
}
QStringList W_ContenuJeu::NettoyerEtCouperLigne(QString Ligne)
{

    QRegularExpression re("^( |\\t|-|\\()*(\\d*)\\s*(.*)$");
    QRegularExpressionMatch match = re.match(Ligne);
    QStringList Liste;
    if (match.hasMatch()) {
        Liste<<match.captured(2).trimmed();
        Liste<<match.captured(3).trimmed();
    }
    return Liste;
}

void W_ContenuJeu::on_TB_Annuler_clicked()
{
    ui->TB_Valider->setVisible(false);
    ui->TB_Annuler->setVisible(false);
    ui->Tv_Contenu->setVisible(true);
    ui->Te_Contenu->setVisible(false);
    ui->Te_Contenu->clear();
}

bool W_ContenuJeu::on_Tv_Contenu_editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    uint iIdBenevole=main->RecupereIdBenevole();
    if(iIdBenevole==0)
    {
        QMessageBox::critical(this,"Aucun utilisateur choisir",
                              "Aucun utilisateur n'a été choisi. Merci d'utiliser le menu \"Utilisateur en cours\" pour sélectionner votre nom.");
        return true;
    }
    return false;
}
