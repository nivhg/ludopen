#include "d_ajoutelementpanier.h"
#include "ui_d_ajoutelementpanier.h"

D_AjoutElementPanier::D_AjoutElementPanier(QWidget *parent,int IdDuMembre) :
  QDialog(parent),
  ui(new Ui::D_AjoutElementPanier)
{
    ui->setupUi(this);
    QSqlQuery Requete;

    if(!Requete.exec("SELECT * FROM prereglagespanier"))
    {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
            return;
    }
    ui->CBx_ChoixPrerempli->addItem("");

    while(Requete.next())
    {
        ui->CBx_ChoixPrerempli->addItem(ObtenirValeurParNom(Requete,"Description").toString(),ObtenirValeurParNom(Requete,"IdPrereglagesPanier").toInt());
        for(int i=0;i<Requete.record().count();i++)
        {
            ListePrereglagesPanier[ObtenirValeurParNom(Requete,"IdPrereglagesPanier").toInt()][Requete.record().fieldName(i)]=Requete.value(i);
        }
    }

    if(!Requete.exec("SELECT * FROM typeventilation"))
    {
            qDebug()<<getLastExecutedQuery(Requete)<<Requete.lastError();
            return;
    }

    ui->CBx_TypeVentilation->addItem("");

    while(Requete.next())
    {
        ui->CBx_TypeVentilation->addItem(ObtenirValeurParNom(Requete,"TypeVentilation").toString(),ObtenirValeurParNom(Requete,"IdTypeVentilation"));
    }


    this->IdDuMembre=IdDuMembre;
    // Si on ne connait pas le membre, on affiche le controle de recherche d'un membre
    // et on désactive les autres contrôles tant que le membre n'a pas été choisi
    if(IdDuMembre==0)
    {
        MaJListeMembres();

        SearchMembre = new SearchBox(this);
        SearchMembre->MAJResults(this->VecteurMembres,3);
        SearchMembre->show();

        ui->horizontalLayout->addWidget(SearchMembre);
        connect(SearchMembre,SIGNAL(SignalSuggestionFini(QString)),this,SLOT(SearchMembre_MembreTrouve()));
        ActiverControles(false);
    }
    // Si on connait le membre, on cache le contrôle
    else
    {
        SearchMembre=0;
        ui->Lb_Membre->setVisible(false);
    }
}

D_AjoutElementPanier::~D_AjoutElementPanier()
{
    delete ui;
}

/**   Mise  jour de la liste des membres
 *  @pre    Accès à  la base de données
 *  @retval bool
 *  @return True si tous c'est passé correctement et false si il y a une erreur
 *  @test   Voir la procédure dans le fichier associé.
 */
bool D_AjoutElementPanier::MaJListeMembres()
{
    QSqlQuery query ;
    QVector<QString> Membres ;
    bool bRetourner = true ;

    //Vidage du vecteur
    this->VecteurMembres.clear() ;

    QString requeteSQL;
    requeteSQL="SELECT Nom, Prenom, Email,Ville,IdMembre,CodeMembre FROM membres ORDER BY Nom ASC";

    //Execute une requète sql qui retourne la liste des membres
    //Si la requète est correcte -> Remplissage du veteur VecteurMembres avec le résultat de la requète et on retourne vrai.
    if(query.exec(requeteSQL))
    {
        while(query.next())
        {
            for(int i=0;i<query.record().count();i++)
            {
                Membres.append(query.value(i).toString()) ;
            }
            this->VecteurMembres.push_back(Membres) ;
            Membres.clear();
        }
    }
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        qDebug()<< "F_Emprunt::MaJListeMembres() : Erreur de connexion avec la base de donnée !" << endl ;
        bRetourner = false ;
    }
    return bRetourner ;
}

void D_AjoutElementPanier::ActiverControles(bool etat)
{
    ui->Gb_Elements->setEnabled(etat);
    ui->CBx_ChoixPrerempli->setEnabled(etat);
}

void D_AjoutElementPanier::SearchMembre_MembreTrouve()
{
    ActiverControles(true);
    for(int i=0;i<VecteurMembres.count();i++)
    {
        QVector<QString> Membre;
        Membre=VecteurMembres[i];
        if(Membre[Membre.count()-1]==SearchMembre->currentText())
        {
            IdDuMembre=Membre[Membre.count()-2].toInt();
            break;
        }
    }
}

void D_AjoutElementPanier::on_Bt_Ajouter_clicked()
{
    emit(Signal_AjouterAuPanier(ui->Le_Description->text(),IdDuMembre,ui->SBx_Montant->value(),ui->CBx_TypeVentilation->currentData().toInt(),"",nullptr));
    this->close();
}

void D_AjoutElementPanier::on_Le_Description_textChanged(const QString &arg1)
{
    VerifChampsRempli();
}

void D_AjoutElementPanier::VerifChampsRempli()
{
    if(ui->Le_Description->text()!="")
    {
        if(SearchMembre!=0 && SearchMembre->currentText()!="")
        {
            ui->Bt_Ajouter->setEnabled(true);
        }
        else if(SearchMembre==0)
        {
            ui->Bt_Ajouter->setEnabled(true);
        }
    }
    else
    {
        ui->Bt_Ajouter->setEnabled(false);
    }
}

void D_AjoutElementPanier::on_Bt_Annuler_clicked()
{
    this->close();
}

void D_AjoutElementPanier::on_CBx_ChoixPrerempli_currentIndexChanged(int index)
{
    for(int i=0;i<ui->CBx_TypeVentilation->count();i++)
    {
        if(ui->CBx_TypeVentilation->itemData(i).toInt()==ListePrereglagesPanier[ui->CBx_ChoixPrerempli->currentData().toInt()]
                ["TypeVentilation_IdTypeVentilation"])
        {
            ui->CBx_TypeVentilation->setCurrentIndex(i);
            break;
        }
    }

    ui->Le_Description->setText(ListePrereglagesPanier[ui->CBx_ChoixPrerempli->currentData().toInt()]["Description"].toString());

    if(ListePrereglagesPanier[ui->CBx_ChoixPrerempli->currentData().toInt()]["DescriptionChamps"].toString()!="")
    {
        ui->Lb_Champs1->setVisible(true);
        ui->SBx_Champs1->setVisible(true);
        ui->SBx_Champs1->setValue(0);
        ui->Lb_Champs1->setText(ListePrereglagesPanier[ui->CBx_ChoixPrerempli->currentData().toInt()]["DescriptionChamps"].toString());
        if(ListePrereglagesPanier[ui->CBx_ChoixPrerempli->currentData().toInt()]["DescriptionChamps2"].toString()!="")
        {
            ui->Lb_Champs2->setVisible(true);
            ui->SBx_Champs2->setVisible(true);
            ui->SBx_Champs2->setValue(0);
            ui->Lb_Champs2->setText(ListePrereglagesPanier[ui->CBx_ChoixPrerempli->currentData().toInt()]["DescriptionChamps2"].toString());
        }
        else
        {
            ui->Lb_Champs2->setVisible(false);
            ui->SBx_Champs2->setVisible(false);
        }
    }
    else
    {
        ui->Lb_Champs1->setVisible(false);
        ui->SBx_Champs1->setVisible(false);
        ui->Lb_Champs2->setVisible(false);
        ui->SBx_Champs2->setVisible(false);
    }
    CalculerMontant();
}

void D_AjoutElementPanier::on_SBx_Champs1_valueChanged(int arg1)
{
    CalculerMontant();
}

void D_AjoutElementPanier::on_SBx_Champs2_valueChanged(int arg1)
{
    CalculerMontant();
}

void D_AjoutElementPanier::CalculerMontant()
{
    QString CalculMontant=ListePrereglagesPanier[ui->CBx_ChoixPrerempli->currentData().toInt()]["CalculMontant"].toString();
    CalculMontant.replace("#Champs1#",QString::number(ui->SBx_Champs1->value()),Qt::CaseInsensitive);
    CalculMontant.replace("#Champs2#",QString::number(ui->SBx_Champs2->value()),Qt::CaseInsensitive);
    QScriptEngine expression;
    double valeur=expression.evaluate(CalculMontant).toNumber();
    // Test si la valeur vaut NaN, on la met à 0
    if(valeur!=valeur)
    {
        valeur=0;
    }
    ui->SBx_Montant->setValue(valeur);
}
