#include "f_panier.h"
#include "ui_f_panier.h"

F_Panier::F_Panier(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::F_Panier)
{
    ui->setupUi(this);

    for(int i=1;i<13;i++)
    {
        QString mois=QDate::longMonthName(i);
        mois[0]=mois[0].toUpper();
        ui->CBx_Mois->addItem(mois,i);
    }
    ui->CBx_Mois->setCurrentIndex(QDate::currentDate().month()-1);

    ui->CBx_Annee->addItem(QString::number(QDate::currentDate().year()-1));
    ui->CBx_Annee->addItem(QString::number(QDate::currentDate().year()));
    ui->CBx_Annee->addItem(QString::number(QDate::currentDate().year()+1));
    ui->CBx_Annee->setCurrentIndex(1);

    ui->SBx_Total->setSuffix("€");
    ui->TbV_Panier->clearSpans() ;
    ui->TbV_Panier->setModel(&ModelePanier) ;
    ui->TbV_Panier->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TbV_Panier->setSortingEnabled(true);
    ModelePanier.setColumnCount(2);
    ModelePanier.setRowCount(0);
    ModelePanier.setHorizontalHeaderItem(0, new QStandardItem("Description"));
    ui->TbV_Panier->setColumnWidth(0,400);
    ModelePanier.setHorizontalHeaderItem(1, new QStandardItem("Prix en €"));

    ui->GB_Collectivite->setVisible(false);

    //Associe le modèle au TableView
    ui->TW_Paiements->setColumnCount(7);
    QStringList Liste;
    Liste<<"Nom du membre"<<"N° facture"<<"Date"<<"Mode de paiement"<<"Total"<<"Date de paiement"<<"Remarque";
    ui->TW_Paiements->setHeaderLabels(Liste);
    ui->TW_Paiements->setColumnWidth(0,200);

    this->pBanqueAjMod = new F_PopUpCLESTTEM();
    this->pBanqueAjMod->setWindowModality(Qt::ApplicationModal);

    //Initialise le tableau des Autres moyens de paiement
    HInfosMembre["IdMembre"]=0;
    //Remplir le ComboBox des modes de paiements
    QSqlQuery RequeteMode;
    RequeteMode.exec("SELECT NomPaiement,IdModePaiement FROM modepaiement ORDER BY IdModePaiement");
    ui->CBx_ModePaiement->addItem("");
    while (RequeteMode.next())
    {
      ui->CBx_ModePaiement->addItem(RequeteMode.value(0).toString(),RequeteMode.value(1).toInt());
    }
    ControleChequeVisible(false);
    ActualiserPaiements();
}

F_Panier::~F_Panier()
{
    delete ui;
}

bool F_Panier::slot_AjouterAuPanier(QString Titre,uint nIDMembre,double Prix,int IdVentilation,QString Table,QList<QSqlQuery *> *Requetes)
{
    if(HInfosMembre["IdMembre"]==0)
    {
        HInfosMembre["IdMembre"]=nIDMembre;
    }
    else if(HInfosMembre["IdMembre"]!=nIDMembre)
    {
        QMessageBox::critical(this, "Membre différent", "Il n'est pas possible d'ajouter l'élement en cours au panier, \n"
                              "car il s'agit d'un membre différent. Valider le panier avant de rajouter de nouveaux élements.");
        return false;
    }
    QSqlQuery RequeteMembre;
    RequeteMembre.prepare("SELECT * FROM membres WHERE IdMembre=:IdMembre");
    RequeteMembre.bindValue(":IdMembre",HInfosMembre["IdMembre"]);
    RequeteMembre.exec();
    RequeteMembre.next();
    for(int i=0;i<RequeteMembre.record().count();i++)
    {
        HInfosMembre[RequeteMembre.record().fieldName(i)]=RequeteMembre.value(i);
    }
    ui->GB_Collectivite->setVisible(HInfosMembre["TitreMembre_IdTitreMembre"]==FILTRE_COLLECTIVITES);
    ui->Le_NomMembre->setText(HInfosMembre["Nom"].toString()+" "+HInfosMembre["Prenom"].toString());
    infosPaiements MesinfosPaiements;
    MesinfosPaiements.Requetes=Requetes;
    MesinfosPaiements.IdVentilation=IdVentilation;
    MesinfosPaiements.Table=Table;
    lInfosPaiements.append(MesinfosPaiements);
    QList<QStandardItem *> *Liste=new QList<QStandardItem *>();
    Liste->append(new QStandardItem(Titre));
    Liste->append(new QStandardItem(QString::number(Prix)));
    ModelePanier.appendRow(*Liste);
    ui->SBx_Total->setValue(ui->SBx_Total->value()+Prix);
    CalculerPrix();
    ControlePanierVide(true);
    emit(SignalMiseAJourNbItemsPanier(ModelePanier.rowCount()));
    return true;
}

bool F_Panier::slot_VerifMembrePanier(uint IdDuMembre)
{
    return HInfosMembre["IdMembre"]!=IdDuMembre;
}

void F_Panier::ControlePanierVide(bool etat)
{
    if(!etat)
    {
        ui->Le_NomMembre->setText("");
        ui->GB_Collectivite->setVisible(false);
        ui->CBx_ModePaiement->setCurrentIndex(0);
        ui->SBx_Total->setValue(0);
        HInfosMembre.clear();
        HInfosMembre["IdMembre"]=0;
        lInfosPaiements.clear();
        ControleChequeVisible(false);
        emit(SignalMiseAJourNbItemsPanier(0));
        ModelePanier.removeRows(0,ModelePanier.rowCount());
        ui->SB_NumFacture->setValue(0);
        ui->SB_AnneeFacture->setValue(0);
    }
    ui->Bt_OK->setEnabled(false);
    // Si on active les controles et qu'un mode de paiement a été choisi, on active le bouton Valider
    if(etat && ui->CBx_ModePaiement->currentIndex()>0)
    {
        ui->Bt_OK->setEnabled(etat);
    }
    ui->CBx_ModePaiement->setEnabled(etat);
    ui->Bt_Effacer->setEnabled(etat);
}

void F_Panier::on_Bt_OK_clicked()
{
    if(HInfosMembre["TitreMembre_IdTitreMembre"]==FILTRE_COLLECTIVITES)
    {
        if(ui->SB_NumFacture->value()==0 || ui->SB_AnneeFacture->value()==0)
        {
            QMessageBox::critical(this, "Numéro de facture manquante", "Merci de saisir le numéro de facture correspondant.");
            return;
        }
    }
    int IdCheque=NULL;
    if(ui->CBx_ModePaiement->currentData().toInt()==PAIEMENT_CHEQUE && !ui->CB_PaiementDiffere->isChecked())
    {
        if(!ui->CB_PaiementDiffere->isChecked())
        {
            QString NomChamps;
            if(ui->Le_NumCheque->text().trimmed()=="")
            {
                NomChamps="Le numéro du chèque";
            }
            if(ui->CBx_Banque->currentIndex()==0)
            {
                NomChamps="La banque du chèque";
            }
            if(ui->Le_Emetteur->text().trimmed()=="")
            {
                NomChamps="L'émetteur du chèque";
            }
            if(NomChamps!="")
            {
                QMessageBox::critical(this, "Information(s) sur le chèque manquante", NomChamps+" est vide. Merci de saisir le champs manquant.");
                return;
            }
        }

      QSqlQuery RequeteAjoutCheque;
      RequeteAjoutCheque.prepare("INSERT INTO cheques (Banques_IdBanque,NomEmetteur,Numero,Montant,DateCheque) "
                                   "VALUES (:Banques_IdBanque,:NomEmetteur,:Numero,:Montant,:DateCheque)");
      RequeteAjoutCheque.bindValue(":Numero",ui->Le_NumCheque->text());
      RequeteAjoutCheque.bindValue(":Banques_IdBanque",ui->CBx_Banque->currentData().toInt());
      RequeteAjoutCheque.bindValue(":NomEmetteur",ui->Le_Emetteur->text());
      RequeteAjoutCheque.bindValue(":DateCheque",ui->DtE_DateCheque->date().toString("yyyy-MM-dd"));
      RequeteAjoutCheque.bindValue(":Montant",ui->SBx_Total->value());

      if(!RequeteAjoutCheque.exec())
      {
          qDebug() << getLastExecutedQuery( RequeteAjoutCheque) << RequeteAjoutCheque.lastError();
          return;
      }
      IdCheque=RequeteAjoutCheque.lastInsertId().toInt();
    }

    QSqlQuery RequetePaiement;
    RequetePaiement.prepare("INSERT INTO paiements (DatePaiement,Membres_IdMembre,Montant,ModePaiement_IdModePaiement,Cheques_IdCheque,NumeroFacture,DatePaye,Remarque) "
                               "VALUES (:DatePaiement,:Membres_IdMembre,:Montant,:IdModePaiement,:IdCheque,:NumeroFacture,:DatePaye,:Remarque)");
    RequetePaiement.bindValue(":DatePaiement",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    RequetePaiement.bindValue(":Membres_IdMembre",HInfosMembre["IdMembre"]);
    RequetePaiement.bindValue(":Montant",ui->SBx_Total->value());
    RequetePaiement.bindValue(":IdModePaiement",ui->CBx_ModePaiement->currentData());
    RequetePaiement.bindValue(":IdCheque",IdCheque);
    QString NumeroFacture="";
    if(ui->SB_NumFacture->value()!=0 || ui->SB_AnneeFacture->value()!=0)
    {
        NumeroFacture=QString(QString::number(ui->SB_NumFacture->value())+"/"+QString::number(ui->SB_AnneeFacture->value()));
    }
    RequetePaiement.bindValue(":NumeroFacture",NumeroFacture);
    if(ui->CB_PaiementDiffere->isChecked())
    {
        RequetePaiement.bindValue(":DatePaye",QVariant(QVariant::Date));
    }
    else
    {
        RequetePaiement.bindValue(":DatePaye",QDate::currentDate());
    }
    RequetePaiement.bindValue(":Remarque",ui->LE_Remarque->text());

    if(!RequetePaiement.exec())
    {
        qDebug()<<getLastExecutedQuery(RequetePaiement)<<RequetePaiement.lastError();
        return;
    }
    nIDPaiement=RequetePaiement.lastInsertId().toInt();

    int IdTable;
    // On ajoute tous les achats du panier dans la table paiementsachats
    for(int i=0;i<ModelePanier.rowCount();i++)
    {
        int IdMalle=0;
        QList<QSqlQuery *> *requetes=lInfosPaiements.at(i).Requetes;
        if(requetes!=NULL)
        {
            for(int j=0;j<requetes->count();j++)
            {
                if(lInfosPaiements.at(i).IdVentilation==VENTILATION_MALLES)
                {
                    requetes->at(j)->bindValue(":Malles_IdMalle",IdMalle);
                }
                if( !requetes->at(j)->exec() )
                {
                    qDebug()<< "Panier : " << ModelePanier.item(i,COLITEM_DESCRIPTION)->text() << getLastExecutedQuery(*(requetes->at(j))) << requetes->at(j)->lastError();
                }
                if(j==0)
                {
                    IdTable=requetes->at(j)->lastInsertId().toInt();
                    if(lInfosPaiements.at(i).IdVentilation==VENTILATION_MALLES)
                    {
                        IdMalle=IdTable;
                    }
                }
            }
        }
        QSqlQuery RequeteAjoutPaiement;

        RequeteAjoutPaiement.prepare("INSERT INTO paiementsachats (Designation,Montant,TypeVentilation_IdTypeVentilation,NomTable,IdTable,Paiements_IdPaiement) "
                                   "VALUES (:Designation,:Montant,:IdTypeVentilation,:NomTable,:IdTable,:Paiements_IdPaiement)");
        RequeteAjoutPaiement.bindValue(":Designation",ModelePanier.item(i,COLITEM_DESCRIPTION)->text());
        RequeteAjoutPaiement.bindValue(":Montant",ModelePanier.item(i,COLITEM_PRIX)->text().toDouble());
        RequeteAjoutPaiement.bindValue(":IdTypeVentilation",lInfosPaiements.at(i).IdVentilation);
        RequeteAjoutPaiement.bindValue(":NomTable",lInfosPaiements.at(i).Table);
        RequeteAjoutPaiement.bindValue(":IdTable",IdTable);
        RequeteAjoutPaiement.bindValue(":Paiements_IdPaiement",nIDPaiement);

        if(!RequeteAjoutPaiement.exec())
        {
            qDebug()<<getLastExecutedQuery(RequeteAjoutPaiement)<<RequeteAjoutPaiement.lastError();
            return;
        }
    }

    ControlePanierVide(false);
    ui->rB_Les_deux->setChecked(true);
    ActualiserPaiements();
    on_TW_Paiements_itemClicked(new QTreeWidgetItem(),0);
    // Pour mette à jour le calendrier des malles
    emit(Signal_Nouvelle_Malle());
}

void F_Panier::MaJBanques()
{
    ui->CBx_Banque->clear();
    QSqlQuery RequeteBanque;

    // Récupère toutes les banques
    RequeteBanque.prepare("SELECT IdBanque, CONCAT(Acronyme,\" - \",NomBanque) as NomBanque FROM banques ORDER BY NomBanque" ) ;

    //Exectution de la requête
    if( !RequeteBanque.exec() )
    {
        qDebug() << getLastExecutedQuery(RequeteBanque)<< RequeteBanque.lastError();
    }
    else
    {
        QString Banque;
        int IdBanque;
        int i = 1;
        ui->CBx_Banque->addItem("");
        while( RequeteBanque.next() )
        {
            // Récupère la valeurs des champs
            Banque = ObtenirValeurParNom(RequeteBanque,"NomBanque").toString();
            IdBanque = ObtenirValeurParNom(RequeteBanque,"IdBanque").toInt();
            // Ajoute une activité et son ID dans la partie DATA
            ui->CBx_Banque->addItem(Banque);
            ui->CBx_Banque->setItemData(i,IdBanque,Qt::UserRole);
            i++;
        }
        ui->CBx_Banque->addItem("Ajouter une banque...");
    }
}

void F_Panier::ActualiserPaiements()
{
    char Filtre;
    if(ui->rB_Collectivites->isChecked())
    {
        Filtre=FILTRE_COLLECTIVITES;
    }
    else if(ui->rB_Familles->isChecked())
    {
        Filtre=FILTRE_FAMILLES;
    }
    else
    {
        Filtre=FILTRE_LES_DEUX;
    }
    QString requete="SELECT * FROM paiements as p LEFT JOIN membres as m ON m.IdMembre=p.Membres_IdMembre "
                    "LEFT JOIN modepaiement as mo ON mo.IdModePaiement=p.ModePaiement_IdModePaiement WHERE MONTH(p.DatePaiement)="+
                    ui->CBx_Mois->currentData().toString()+" AND YEAR(p.DatePaiement)="+ui->CBx_Annee->currentText();
    if(Filtre!=FILTRE_LES_DEUX)
    {
        requete+=" AND m.TitreMembre_IdTitreMembre="+QString::number(Filtre);
    }
    //requete+=" ORDER BY DatePaiement";
    QSqlQuery RequetePaiements;
    RequetePaiements.prepare(requete);
    if (!RequetePaiements.exec())
    {
        qDebug()<<getLastExecutedQuery(RequetePaiements)<<RequetePaiements.lastError();
    }

    // On efface le treeview
    ui->TW_Paiements->clear();
    int cptLine=0;
    while(RequetePaiements.next())
    {
        QString detailsPaiement;
        QStringList List;

        List<<ObtenirValeurParNom(RequetePaiements,"Nom").toString()+" "+ObtenirValeurParNom(RequetePaiements,"Prenom").toString()
           <<ObtenirValeurParNom(RequetePaiements,"NumeroFacture").toString()
         <<ObtenirValeurParNom(RequetePaiements,"DatePaiement").toDate().toString("dd/MM/yyyy")
        <<ObtenirValeurParNom(RequetePaiements,"NomPaiement").toString()
        <<ObtenirValeurParNom(RequetePaiements,"Montant").toString()
        <<ObtenirValeurParNom(RequetePaiements,"DatePaye").toDate().toString("dd/MM/yyyy")
        <<ObtenirValeurParNom(RequetePaiements,"Remarque").toString();
        QTreeWidgetItem *Racine=new QTreeWidgetItem(List);
        Racine->setFlags(Racine->flags() | Qt::ItemIsEditable);
        Racine->setData(0,Qt::UserRole,ObtenirValeurParNom(RequetePaiements,"IdPaiement").toInt());
        ui->TW_Paiements->addTopLevelItem(Racine);

        QSqlQuery RequetePaiementsAchats;
        RequetePaiementsAchats.prepare("SELECT * FROM paiementsachats as p LEFT JOIN typeventilation as t ON t.IdTypeVentilation=p.TypeVentilation_IdTypeVentilation "
                                       "WHERE Paiements_IdPaiement=:IdPaiement");
        RequetePaiementsAchats.bindValue(":IdPaiement",ObtenirValeurParNom(RequetePaiements,"IdPaiement").toInt());

        if (!RequetePaiementsAchats.exec())
        {
            qDebug()<<getLastExecutedQuery(RequetePaiementsAchats)<<RequetePaiementsAchats.lastError();
        }
        else
        {
            QTreeWidgetItem *AchatsItem=new QTreeWidgetItem();
            AchatsItem->setText(0,"Détails achats");
            Racine->addChild(AchatsItem);
            int j=0;
            while(RequetePaiementsAchats.next())
            {
                detailsPaiement=ObtenirValeurParNom(RequetePaiementsAchats,"Designation").toString()+" ("+
                        ObtenirValeurParNom(RequetePaiementsAchats,"TypeVentilation").toString()+") : "+
                                                ObtenirValeurParNom(RequetePaiementsAchats,"Montant").toString()+"€ ";
                QTreeWidgetItem *Item=new QTreeWidgetItem();
                Item->setFirstColumnSpanned(true);
                Item->setText(0,detailsPaiement);
                AchatsItem->addChild(Item);
                Item->setFirstColumnSpanned(true);
            }
        }
        int IdCheque=ObtenirValeurParNom(RequetePaiements,"Cheques_IdCheque").toInt();
        if(IdCheque!=0)
        {
            QSqlQuery RequeteCheque;
            RequeteCheque.prepare("SELECT * FROM cheques as c LEFT JOIN banques as b ON Banques_IdBanque=IdBanque WHERE IdCheque=:IdCheque");
            RequeteCheque.bindValue(":IdCheque",IdCheque);

            if (!RequeteCheque.exec())
            {
                qDebug()<<getLastExecutedQuery(RequeteCheque)<<RequeteCheque.lastError();
            }
            else
            {
                QTreeWidgetItem *ChequeItem=new QTreeWidgetItem();
                ChequeItem->setText(0,"Détails chèque");
                Racine->addChild(ChequeItem);
                while(RequeteCheque.next())
                {
                    QString Remise=ObtenirValeurParNom(RequeteCheque,"NumeroRemise").toString();
                    if(Remise=="0")
                    {
                        Remise="Non mis en banque";
                    }
                    else
                    {
                        Remise="Remise n°"+Remise;
                    }
                    detailsPaiement=ObtenirValeurParNom(RequeteCheque,"NomEmetteur").toString()+" (N°"+ObtenirValeurParNom(RequeteCheque,"Numero").toString()+" "+
                            ObtenirValeurParNom(RequeteCheque,"Acronyme").toString()+") - "+Remise;
                    QTreeWidgetItem *Item=new QTreeWidgetItem();
                    Item->setText(0,detailsPaiement);
                    ChequeItem->addChild(Item);
                    Item->setFirstColumnSpanned(true);
                }
            }
        }
        cptLine++;
    }
    ui->TW_Paiements->sortItems(2,Qt::AscendingOrder);
    on_TW_Paiements_itemClicked(new QTreeWidgetItem(),0);
}

void F_Panier::on_CBx_ModePaiement_currentIndexChanged(int index)
{
    ui->Bt_OK->setEnabled(false);
    // Si l'utilisateur a choisi la première ligne qui est vide, on quitte la fonction
    if(index==0)
    {
        ControleChequeVisible(false);
        return;
    }
    // Si le panier n'est pas vide, on active le bouton de validation
    if(ModelePanier.rowCount()!=0)
    {
        ui->Bt_OK->setEnabled(true);
    }
    // Si on a choisi le paiement par chèque, on affiche les champs correspondants
    bool ModeCheque=ui->CBx_ModePaiement->currentData().toInt()==PAIEMENT_CHEQUE;
    ControleChequeVisible(ModeCheque);
    if(ModeCheque)
    {
        ui->Le_NumCheque->setText("");
        ui->Le_Emetteur->setText(HInfosMembre["Nom"].toString());
        ui->CBx_Banque->setCurrentIndex(0);
        ui->DtE_DateCheque->setDate(QDate::currentDate());
        ui->CB_PaiementDiffere->setChecked(false);
    }
    else if(ui->CBx_ModePaiement->currentData().toInt()==PAIEMENT_VIREMENT)
    {
        ui->CB_PaiementDiffere->setChecked(true);
    }
    else
    {
        ui->CB_PaiementDiffere->setChecked(false);
    }
    MaJBanques();
}

void F_Panier::on_CBx_Banque_currentIndexChanged(int index)
{
    if( (ui->CBx_Banque->count()-1) == index && index >0 )
    {
        // S'il ne rajoute pas de de banque, on sort de la fonction
        if(this->pBanqueAjMod->Ajouter(13)==0)
        {
            return;
        }
        ui->CBx_Banque->blockSignals(true);
        ui->CBx_Banque->clear();
        MaJBanques();
        ui->CBx_Banque->blockSignals(false);
    }
}

void F_Panier::on_Bt_SupprimerItemPanier_clicked()
{
    //Supprime la ligne sélectionnée après avoir vérifier qu'il y ait bien un item de choisi dans le tableau
   ModelePanier.removeRow(ui->TbV_Panier->currentIndex().row());

   //Grise le bouton de suppression
   ui->Bt_SupprimerItemPanier->setEnabled(false);
   CalculerPrix();
   if(ModelePanier.rowCount()==0)
   {
       ControlePanierVide(false);
   }
   emit(SignalMiseAJourNbItemsPanier(ModelePanier.rowCount()));
}

void F_Panier::on_TbV_Panier_clicked(const QModelIndex &index)
{
    // Si un élément a été sélectionné
    if(ModelePanier.item(index.row(),index.column())!=0)
    {
        ui->Bt_SupprimerItemPanier->setEnabled(true);
    }
}

/**
 * @brief Calcul du prix à payer qui diminue à chaque fois qu'un moyen de paiement est utilisé.
 * @pre
 * @param
 * @retval
 */
void F_Panier::CalculerPrix()
{
    double Total=0;
    for (register unsigned int i=0; i < ModelePanier.rowCount() ; i++)
    {
        QLocale English(QLocale::English);
        Total+=English.toDouble(ModelePanier.item(i,COLITEM_PRIX)->text());
    }

    ui->SBx_Total->setValue(Total);
    if(Total==0)
    {
        ui->Bt_OK->setEnabled(false);
        ui->CBx_ModePaiement->setEnabled(false);
    }
}

void F_Panier::on_rB_Les_deux_clicked(bool checked)
{
    ActualiserPaiements();
}

void F_Panier::on_rB_Collectivites_clicked(bool checked)
{
    ActualiserPaiements();
}

void F_Panier::on_rB_Familles_clicked(bool checked)
{
    ActualiserPaiements();
}

void F_Panier::on_Bt_Effacer_clicked()
{
    ui->rB_Les_deux->setChecked(true);
    ActualiserPaiements();
    ControlePanierVide(false);
}

void F_Panier::on_CB_PaiementDiffere_toggled(bool checked)
{
    // On masque
    ControleChequeVisible(!checked&&ui->CBx_ModePaiement->currentData().toInt()==PAIEMENT_CHEQUE);
}

void F_Panier::ControleChequeVisible(bool etat)
{
    ui->Lb_NumCheque->setVisible(etat);
    ui->Lb_Banque->setVisible(etat);
    ui->Lb_Emetteur->setVisible(etat);
    ui->Lb_DateCheque->setVisible(etat);

    ui->Le_NumCheque->setVisible(etat);
    ui->CBx_Banque->setVisible(etat);
    ui->Le_Emetteur->setVisible(etat);
    ui->DtE_DateCheque->setVisible(etat);
}

void F_Panier::on_Bt_ImprimerDevis_clicked()
{
    QHash<QString, QVariant> HPaiement;
    HPaiement=HInfosMembre;
    HPaiement["Remarque"]=ui->LE_Remarque->text();
    HPaiement["Montant"]=ui->SBx_Total->value();
    HPaiement["DatePaiement"]=QDate::currentDate();
    // On ajoute tous les achats du panier dans la table paiementsachats
    QString Designations,Montants;
    for(int i=0;i<ModelePanier.rowCount();i++)
    {
        Designations+="<p>"+ModelePanier.item(i,COLITEM_DESCRIPTION)->text()+"</p>";
        Montants+="<p align='center'>"+ModelePanier.item(i,COLITEM_PRIX)->text()+" €</p>";
    }
    HPaiement["Designations"]=Designations;
    HPaiement["Montants"]=Montants;

    F_ImprimerFacture F_ImprimerFacture;
    F_ImprimerFacture.ImprimerFacture(nIDPaiement,false,HPaiement);
    F_ImprimerFacture.exec() ;
}

void F_Panier::on_Bt_ImprimerFacture_clicked()
{
    int Idpaiement=ui->TW_Paiements->currentItem()->data(0,Qt::UserRole).toInt();
    F_ImprimerFacture F_ImprimerFacture;
    F_ImprimerFacture.ImprimerFacture(Idpaiement,true);
    F_ImprimerFacture.exec() ;
}

void F_Panier::on_CBx_Mois_currentIndexChanged(int index)
{
    ActualiserPaiements();
}

void F_Panier::on_Bt_AjoutAnimation_clicked()
{
    qDebug()<<HInfosMembre["IdMembre"].toInt();
    D_AjoutElementPanier dAjoutElementPanier(this,HInfosMembre["IdMembre"].toInt());
    connect( &dAjoutElementPanier, SIGNAL( Signal_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *) ), this,
             SLOT( slot_AjouterAuPanier(QString,uint,double,int,QString,QList<QSqlQuery *> *)) ) ;
    dAjoutElementPanier.exec() ;
}

void F_Panier::on_TW_Paiements_itemClicked(QTreeWidgetItem *item, int column)
{
    if(column==COLPAIE_REMARQUE)
    {
        ui->TW_Paiements->editItem(item, column);
    }
    int Idpaiement=item->data(0,Qt::UserRole).toInt();
    ui->Bt_ImprimerFacture->setEnabled(Idpaiement!=0);
}

void F_Panier::on_TW_Paiements_itemChanged(QTreeWidgetItem *item, int column)
{
    int Idpaiement=ui->TW_Paiements->currentItem()->data(0,Qt::UserRole).toInt();

    QSqlQuery Requete;
    Requete.prepare("UPDATE paiements SET Remarque=:Remarque WHERE IdPaiement=:IdPaiement");
    Requete.bindValue(":Remarque", item->text(COLPAIE_REMARQUE));
    Requete.bindValue(":IdPaiement", Idpaiement);
    if(!Requete.exec())
    {
        qDebug()<< getLastExecutedQuery(Requete) << Requete.lastError();
    }
}

void F_Panier::on_CBx_Annee_currentIndexChanged(int index)
{
    ActualiserPaiements();
}
