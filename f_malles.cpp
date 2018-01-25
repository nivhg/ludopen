#include "f_malles.h"
#include "ui_f_malles.h"

F_Malles::F_Malles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::F_Malles)
{
    ui->setupUi(this);
    this->TbV_CalendrierMalles=new TableViewToolTipModifier(this);
    this->TbV_CalendrierMalles->setSelectionMode(QAbstractItemView::SingleSelection);
    this->TbV_CalendrierMalles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->gL_CalendrierMalles->addWidget(this->TbV_CalendrierMalles,0,0);
    this->ModeleMalle = new QStandardItemModel() ;
    connect(this->TbV_CalendrierMalles, SIGNAL( mouseClickItem(QModelIndex)), this, SLOT(on_TbV_CalendrierMalles_clicked(QModelIndex)));

    //Affiche le nom de la fenêtre
    this->setWindowTitle("Calendrier des grands jeux");
/*    setMouseTracking(true);
    ui->TbV_CalendrierMalles->installEventFilter(this);
    ui->TbV_CalendrierMalles->setMouseTracking(true);
    ui->TbV_CalendrierMalles->viewport()->setMouseTracking(true);*/

}

F_Malles::~F_Malles()
{
    delete ui;
}

//------------------------------------------------------------------------------
/** Permet d'afficher les informations du payement
 *  @pre    Connexion à la base de données
 *  @param  Somme : Pris à payer en nombre de crédits , CodeMembre : code du membre qui emprunte
 *  @retval Valeurs de retour
 */
void F_Malles::AfficherCalendrier(QRect ParentGeometry=QRect())
{
    QDesktopWidget desk;
    QRect screenres = desk.screenGeometry(0);
    //this->setGeometry(QRect(screenres.x(),screenres.y(),screenres.width(),screenres.height()));
    /*this->setWindowState(this->windowState() | Qt::WindowNoState);
    this->setWindowState(this->windowState() | Qt::WindowActive);
    this->setWindowState(this->windowState() | Qt::WindowMaximized);*/
    if(ParentGeometry!=QRect())
    {
        this->setGeometry(ParentGeometry);
    }
    qDebug()<<this->geometry();
    qDebug()<<this->maximumSize();
    int i;
    QStandardItem *item;
    // Calcul le dernier jour du mois
    QDate *DateChoisie=new QDate(ui->CBx_Annee->currentText().toInt(),ui->CBx_Mois->currentIndex()+1,1);
    *DateChoisie=DateChoisie->addMonths(1);
    *DateChoisie=DateChoisie->addDays(-1);
    int iDernierJour=DateChoisie->day();
    //Création et initialisation du Model
    this->ModeleMalle->clear();
    /*Cration des caractristiques du tableau : -Nombre de colonnes
                                               -Nom des colonnes*/
    this->ModeleMalle->setColumnCount( iDernierJour ) ;

    QLocale french(QLocale::French);
    QDate *DateEnCours;
    for(i=1;i<=iDernierJour;i++)
    {
        DateEnCours=new QDate(ui->CBx_Annee->currentText().toInt(),ui->CBx_Mois->currentIndex()+1,i);
        item=new QStandardItem( french.dayName(DateEnCours->dayOfWeek()).at(0)+" "+QString::number(i) );
        // Si il s'agit du samedi ou dimanche, on les mets en rouge
        if(DateEnCours->dayOfWeek()>5)
        {
            item->setForeground(Qt::red);
        }
        this->ModeleMalle->setHorizontalHeaderItem( i-1, item) ;
    }

    QSqlQuery Requete;
    //Prépare le requête pour récupérer le nom du jeu
    Requete.prepare("SELECT IdJeux,NomJeu FROM jeux WHERE "+F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"="+
                        F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur")+" ORDER BY NomJeu");

    //Exécute la requête
    if (!Requete.exec())
    {
        qDebug()<<"F_Malles::AfficherCalendrier => RequeteJeux : "<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    i=0;
    while(Requete.next())
    {
        item=new QStandardItem( ObtenirValeurParNom(Requete,"NomJeu").toString() );
        item->setData(ObtenirValeurParNom(Requete,"IdJeux"));
        this->ModeleMalle->setVerticalHeaderItem( i++, item ) ;
    }
    this->ModeleMalle->setRowCount(i) ;
    //Prépare le requête pour récupérer les grands jeux avec le type d'emprunt (malle), la date en cours
    Requete.prepare("SELECT t.TypeEmprunt,me.Nom as NomMembre,me.Prenom as PrenomMembre, m.IdMalle, Jeux_IdJeux,r.DatePrevuEmprunt as DateEmprunt,"
                    "DAY(r.DatePrevuEmprunt) as JourEmprunt,r.DatePrevuRetour as DateRetour,0 as emprunt FROM reservation as r "
                    "LEFT JOIN jeux as j ON Jeux_IdJeux=j.IdJeux LEFT JOIN malles as m ON m.IdMalle=Malles_IdMalle "
                    "LEFT JOIN typeemprunt as t on t.IdTypeEmprunt=m.TypeEmprunt_IdTypeEmprunt "
                    "LEFT JOIN membres as me ON r.Membres_IdMembre=me.IdMembre WHERE "+
                    F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"="+
                    F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur")+" AND MONTH(r.DatePrevuEmprunt)="+
                    QString::number(ui->CBx_Mois->currentIndex()+1)+" AND YEAR(r.DatePrevuEmprunt)="+ui->CBx_Annee->currentText()+
                    " UNION ALL (SELECT t.TypeEmprunt,me.Nom as NomMembre, me.Prenom as PrenomMembre, m.IdMalle, Jeux_IdJeux,e.DateEmprunt,"
                    "DAY(e.DateEmprunt) as JourEmprunt,IFNULL(e.DateRetour,e.DateRetourPrevu) as DateRetour,1 as emprunt FROM emprunts as e "
                    "LEFT JOIN jeux as j ON Jeux_IdJeux=j.IdJeux LEFT JOIN malles as m ON m.IdMalle=Malles_IdMalle "
                    "LEFT JOIN typeemprunt as t on t.IdTypeEmprunt=e.TypeEmprunt_IdTypeEmprunt "
                    "LEFT JOIN membres as me ON e.Membres_IdMembre=me.IdMembre WHERE "+
                    F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxNomChamps")+"="+
                    F_Preferences::ObtenirValeur("FiltreJeuxSpeciauxValeur")+" AND MONTH(e.DateEmprunt)="+
                    QString::number(ui->CBx_Mois->currentIndex()+1)+" AND YEAR(e.DateEmprunt)="+ui->CBx_Annee->currentText()+") ORDER BY emprunt DESC");
    //Exécute la requête
    if (!Requete.exec())
    {
        qDebug()<<"F_Malles::AfficherCalendrier => RequeteJeux : "<< getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    qDebug()<<"F_Malles::AfficherCalendrier => RequeteJeux : "<< getLastExecutedQuery(Requete)<<Requete.lastError();
    int iCouleur=Qt::red;
    int iCptMalle=0;
    Requete.next();
    int iIdMalleCourant=ObtenirValeurParNom(Requete,"IdMalle").toInt();
    do
    {
        // Recherche de numéro du ligne du jeu
        for(i=0;i<this->ModeleMalle->rowCount();i++)
        {
            // trouvé
            if(this->ModeleMalle->verticalHeaderItem(i)!=0 &&
                    this->ModeleMalle->verticalHeaderItem(i)->data().toInt()==
                    ObtenirValeurParNom(Requete,"Jeux_IdJeux").toInt())
            {
                break;
            }
        }
        int NumLigneJeu=i;
        // Si le jeu n'a pas été trouvé, on ne traite pas et on continue
        if(this->ModeleMalle->rowCount()==i)
        {
            continue;
        }
        if(iIdMalleCourant!=ObtenirValeurParNom(Requete,"IdMalle").toInt())
        {
            iCptMalle++;
            iIdMalleCourant=ObtenirValeurParNom(Requete,"IdMalle").toInt();
        }
        QString EmprunOuResa;
        // S'il s'agit d'un emprunt, on le barre
        if(ObtenirValeurParNom(Requete,"emprunt").toBool())
        {
            EmprunOuResa="Emprunt de ";
        }
        else
        {
            EmprunOuResa="Réservation de ";
        }
        item=new QStandardItem( QString::number(iCptMalle+1)+ " ("+EmprunOuResa.at(0)+")" );
        // On affiche la couleur d'après dans l'enum Qt::GlobalColor
        item->setBackground(static_cast<Qt::GlobalColor>(iCouleur+iCptMalle));
        // Si on est arrivé à la dernière couleur, on recommence
        if(iCouleur+iCptMalle==Qt::transparent)
        {
            iCouleur=Qt::red;
        }
        int iRetourEmprunt;
        QDate DateRetourEmprunt=ObtenirValeurParNom(Requete,"DateRetour").toDate();
        if(DateRetourEmprunt.month()==(ui->CBx_Mois->currentIndex()+1))
        {
            iRetourEmprunt=DateRetourEmprunt.day();
        }
        else
        {
            iRetourEmprunt=iDernierJour;
        }
        int iPremiereColonne=ObtenirValeurParNom(Requete,"JourEmprunt").toInt()-1;
        int iDerniereColonne=iRetourEmprunt-iPremiereColonne;
        QVariantList vListe;
        vListe.append(iPremiereColonne);
        vListe.append(iDerniereColonne);
        QSqlRecord record = Requete.record();
        QHash<QString, QVariant> list;
        for(int i=0; i < record.count(); i++)
        {
            list[record.fieldName(i)] = record.value(i);
        }
        vListe.append(list);
        item->setData(vListe);
        item->setData(QString(EmprunOuResa+list["NomMembre"].toString()+" "+
                      list["PrenomMembre"].toString()+"\nType de malle: "+
                list["TypeEmprunt"].toString()+"\nDate emprunt: "+list["DateEmprunt"].toDateTime().toString("dd/MM/yyyy hh:mm")+
                "\nDate retour: "+list["DateRetour"].toDateTime().toString("dd/MM/yyyy hh:mm")),Qt::ToolTipRole);
        for(int j=0;j<iDerniereColonne;j++)
        {
            qDebug()<<NumLigneJeu<<iPremiereColonne+j;
            this->ModeleMalle->setItem(NumLigneJeu,iPremiereColonne+j,item);
            item=item->clone();
            qDebug()<<item->data(Qt::ToolTipRole);
        }
    }while(Requete.next());

    this->TbV_CalendrierMalles->setModel(this->ModeleMalle);
    this->TbV_CalendrierMalles->resizeColumnsToContents();
}

void F_Malles::on_CBx_Mois_currentIndexChanged(int index)
{
    this->AfficherCalendrier();
}

void F_Malles::on_CBx_Annee_currentIndexChanged(int index)
{
    this->AfficherCalendrier();
}

void F_Malles::on_TbV_CalendrierMalles_clicked(const QModelIndex &index)
{
    if(this->ModeleMalle->item(index.row(),index.column())!=0)
    {
        QVariantList vList=this->ModeleMalle->item(index.row(),index.column())->data().value<QVariantList> ();
        QItemSelectionModel *selModel=this->TbV_CalendrierMalles->selectionModel();
        for(int i=vList[0].toInt();i<vList[0].toInt()+vList[1].toInt();i++)
        {
            selModel->select(this->ModeleMalle->index(index.row(),i),QItemSelectionModel::Select);
        }
        this->TbV_CalendrierMalles->setSelectionModel(selModel);
        this->TbV_CalendrierMalles->setModel(this->ModeleMalle);
        QHash<QString, QVariant> list=vList[2].value<QHash<QString, QVariant> >();
        ui->Le_NomMembre->setText(list["NomMembre"].toString());
        ui->Le_NomMembre->setProperty("IdMalle",list["IdMalle"]);
        ui->Le_TypeEmprunt->setText(list["TypeEmprunt"].toString());
        ui->DtE_Depart->setDateTime(list["DateEmprunt"].toDateTime());
        ui->DtE_Retour->setDateTime(list["DateRetour"].toDateTime());
        ui->Bt_SupprimerMalle->setEnabled(true);
        if(list["emprunt"].toBool())
        {
            ui->Lb_EmpruntResa->setText("<img width=32 src="":/Valider.png"">Emprunté</img>");
            ui->Bt_EmprunterMalle->setEnabled(false);
            ui->Bt_SupprimerMalle->setEnabled(false);
        }
        else
        {
            ui->Lb_EmpruntResa->setText("<img width=32 src="":/Download.png"">Réservé</img>");
            ui->Bt_EmprunterMalle->setEnabled(true);
            ui->Bt_SupprimerMalle->setEnabled(true);
        }
    }
    else
    {
        QItemSelectionModel *selModel=this->TbV_CalendrierMalles->selectionModel();
        selModel->clear();
        this->TbV_CalendrierMalles->setSelectionModel(selModel);
        ui->Le_NomMembre->clear();
        ui->DtE_Depart->clear();
        ui->DtE_Retour->clear();
        ui->Bt_SupprimerMalle->setEnabled(false);
        ui->Lb_EmpruntResa->setText("");
    }
}

void F_Malles::on_Bt_SupprimerMalle_clicked()
{
    QSqlQuery Requete;
    Requete.prepare("DELETE FROM malles,reservation USING malles INNER JOIN reservation ON Malles_IdMalle=IdMalle WHERE IdMalle=:IdMalle");
    Requete.bindValue(":IdMalle",ui->Le_NomMembre->property("IdMalle").toInt());

    //Exécute la requête
    if (!Requete.exec())
    {
        qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
    }
    this->AfficherCalendrier();
}

void F_Malles::on_Bt_EmprunterMalle_clicked()
{
    QItemSelectionModel *selModel=this->TbV_CalendrierMalles->selectionModel();
    QVariantList vList=this->ModeleMalle->item(selModel->currentIndex().row(),selModel->currentIndex().column())
            ->data().value<QVariantList> ();
    QHash<QString, QVariant> list=vList[2].value<QHash<QString, QVariant> >();
    emit Signal_Clic_Emprunter(list["IdMalle"].toInt());
    this->close();
}

/*
void F_Malles::mouseMoveEvent(QMouseEvent* event)
{
    qDebug() << event->pos();
    QToolTip::hideText();
    QWidget::mousePressEvent(event);
}

bool F_Malles::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this->TbV_CalendrierMalles || obj == this->TbV_CalendrierMalles->viewport())
    {
        qDebug()<<event->type();
        if (event->type() == QEvent::MouseMove)
            qDebug() << "table mouse moveevent";
    }
    //return F_Malles::eventFilter(obj, event);
}
*/
