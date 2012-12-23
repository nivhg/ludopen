//------------------------------------------------------------------------------
/** @file         f_RechercheMembres.cpp
 *  @brief        Permet la recherche d'un membre
 *
 *  @author       PADIOU Nicolas
 *  @author       STS IRIS, Lyce Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        01/01/2012
 *  @version      0.1
 *  @date         11/05/2012
 *
 *  Description dtaille du fichier f_RechercheMembres.cpp
 *
 *
 *  @todo         Fini
 *
 */
//------------------------------------------------------------------------------
// En-tte propre  l'objet ----------------------------------------------------
#include "f_recherchemembres.h"
#include "ui_f_recherchemembres.h"


using namespace std ;

/** Initialise la liste des membres et l'affiche
 *  @test   Voir la procédure dans le fichier associé.
 */
F_RechercheMembres::F_RechercheMembres(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_RechercheMembres)
{
    ui->setupUi(this);

    this->MaJListeMembres() ;
    this->VecteurRechercheMembres = this->VecteurMembres ;
    this->AfficherListe(this->VecteurRechercheMembres) ;
}

/** Détruit l'objet graphique
 *  @test   Voir la procédure dans le fichier associé.
 */
F_RechercheMembres::~F_RechercheMembres()
{
    delete ui;
}

//---------------------------------------------------------------------------
// METHODEs PUBLIQUEs
//-------------------------------------------------------------------------


/**   Mise  jour de la liste des membres
 *  @pre    Accès à  la base de données
 *  @retval bool
 *  @return True si tous c'est passé correctement et false si il y a une erreur
 *  @test   Voir la procédure dans le fichier associé.
 */
bool F_RechercheMembres::MaJListeMembres()
{
    QSqlQuery query ;
    Membre Membres ;
    bool bRetourner = true ;

    //Vidange du vecteur
    this->VecteurMembres.clear() ;

    //Execute une requète sql qui retourne la liste des membres
    //Si la requète est correcte -> Remplissage du veteur VecteurMembres avec le résultat de la requète et on retourne vrai.
    if(query.exec("SELECT IdMembre, Nom, Prenom, Ville, CodeMembre FROM membres ORDER BY Nom ASC"))
    {
        while(query.next())
        {
            Membres.id = query.value(0).toInt() ;
            Membres.sNom = query.value(1).toString() ;
            Membres.sPrenom = query.value(2).toString() ;
            Membres.sVille = query.value(3).toString() ;
            Membres.sCodeMembre = query.value(4).toString() ;
            this->VecteurMembres.push_back(Membres) ;
        }
    } 
    else //Sinon on affiche un message d'erreur et on retourne Faux
    {
        cerr << "RechercheMembre::MaJListeMembres() : Erreur de connexion avec la base de donnée !" << endl ;
        bRetourner = false ;
    }
    this->VecteurRechercheMembres = VecteurMembres ;
    ui->LE_Nom->clear() ;
    ui->LE_Code->clear() ;
    return bRetourner ;
}

/**  Affichage de la liste des membres
 *  @test   Voir la procédure dans le fichier associé.
*/
void F_RechercheMembres::AfficherListe()
{
    this->AfficherListe(this->VecteurMembres);
}

/** Affichage de la liste des membres
 *  @pre    Accés à  la base de données
 *  @param  QVector<Membre> VecteurMembre
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::AfficherListe(QVector<Membre> VecteurMembres)
{
    int i (0) ;
    QStandardItemModel * modele = new QStandardItemModel();
    ui->TbW_Recherche->setModel(modele) ;
    ui->TbW_Recherche->setEditTriggers(0);

    //Remise à  zero de la table recherche
    ui->TbW_Recherche->clearSpans() ;

    //Vérifie que la liste n'est pas vide
    //Si la variable VecteurMembres contient des membres alors on affiche la liste
    //Sinon on affiche rien
    if(VecteurMembres.empty() == false)
    {
        //Création des caractéristiques du tableau : -Nombre de colonnes
        //                                           -Nom des colonnes
        //                                           -Nombre de lignes
        modele->setColumnCount(4);
        modele->setRowCount(VecteurMembres.size());
        modele->setHorizontalHeaderItem(0, new QStandardItem("Nom"));
        modele->setHorizontalHeaderItem(1, new QStandardItem("Prenom"));
        modele->setHorizontalHeaderItem(2, new QStandardItem("Ville"));
        modele->setHorizontalHeaderItem(3, new QStandardItem("Code Membre"));

        //Remplissage du tableau avec les informations contenu dans VecteurMembres
        for(i = 0 ; i < VecteurMembres.size() ; i++)
        {
             modele->setItem(i, 0, new QStandardItem( VecteurMembres[i].sNom ));
             modele->setItem(i, 1, new QStandardItem( VecteurMembres[i].sPrenom ));
             modele->setItem(i, 2, new QStandardItem( VecteurMembres[i].sVille ));
             modele->setItem(i, 3, new QStandardItem( VecteurMembres[i].sCodeMembre));
        }
    }
}

/** Recherche soit avec le nom, soit avec le numéro ou avec les 2
 *  @pre    Accés à  la base de données
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::RechercherParNomEtNumero()
{
    int i (0) ;

    //Remize à  zéro de du Vecteur VecteurRechercheMembres
    this->VecteurRechercheMembres.clear();

    //Vérification que les champs sont bien remplis
    //S'ils ne sont pas remplis le Vecteur VecteurRechercheMembres avec le Vecteur VecteurMembre
    if(ui->LE_Code->text() == "" && ui->LE_Nom->text() == "")
    {
        this->VecteurRechercheMembres = this->VecteurMembres ;
    }
    else// Sinon on remplis le Vecteur VecteurRechercheMembre
    {
        for(i = 0 ; i < this->VecteurMembres.size() ; i++)
        {
            //On vérifie que la suite de lettres entrées dans LE_Nom correpondent aux Noms du vecteur VecteurMembre ou que la suite de chiffre entrées dans LE_Code correspondent aux CodeMembres du Vecteur VecteurMembres
            //Si le Nom, le code ou les 2 correspondent, on l'ajoute dans le vecteur VecteurRechercheMembres
            if( this->VecteurMembres[i].sCodeMembre.toUpper().toStdString().find( ui->LE_Code->text().toUpper().toStdString().c_str() ) != string::npos && this->VecteurMembres[i].sNom.toUpper().toStdString().find( ui->LE_Nom->text().toUpper().toStdString().c_str() ) != string::npos )
            {
                this->VecteurRechercheMembres.push_back(VecteurMembres[i]);
            }
        }
    }
    this->AfficherListe(this->VecteurRechercheMembres);
}

/** Selectionne un membre avec un double click
 *  @pre    Double clique sur un membre
 *  @param  const QModelIndex &index
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::on_TbW_Recherche_doubleClicked(const QModelIndex &index)
{
    emit(this->SignalRenvoieIdMembre(this->VecteurRechercheMembres[index.row()].id));
}

/** Renvoie le premier code non utilisé
 *  @test   Voir la procédure dans le fichier associé.
 */
int F_RechercheMembres::RecupererProchainCodeNonUtilise ()
{
    int nCode ( 1 ) ;
    int     i ( 0 ) ;

    while( i < this->VecteurMembres.size() )
    {
        if( this->VecteurMembres[i].sCodeMembre.toInt() == nCode )
        {
            nCode ++ ;
            i = 0 ;
        }
        i++ ;
    }
    return nCode ;
}

/** Retourne le membre Selectionné dans le TableView
 *  @retval int
 *  @return Retourne la ligne de la selction
 *  @test   Voir la procédure dans le fichier associé.
 */
 int F_RechercheMembres::RecupererMembreSelectionne ()
 {
     return ui->TbW_Recherche->currentIndex().row() ;
 }


 //---------------------------------------------------------------------------
 // SLOTS PRIVEES
 //---------------------------------------------------------------------------


 /** Selectionne un membre avec un click
  *  @param  const QModelIndex &index
  *  @test   Voir la procédure dans le fichier associé.
  */
void F_RechercheMembres::on_TbW_Recherche_clicked(const QModelIndex &index)
{
    ui->TbW_Recherche->selectRow( index.row() ) ;
    emit(this->SignalRenvoieIdMembre(this->VecteurRechercheMembres[index.row()].id));
}

/** Recherche les membres correspondant au champs   chaque fois que le champs est modifiée
 *  @param  const QString &arg1
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::on_LE_Nom_textEdited(const QString &arg1)
{
    ui->LE_Code->clear() ;
    this->RechercherParNomEtNumero() ;
    ui->TbW_Recherche->selectRow( 0 ) ;
}

/** Non utilisé
 *  @param  const QString &arg1
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::on_LE_Nom_textChanged(const QString &arg1)
{

}

/** Recherche les membres correspondant au champs   chaque fois que le champs est modifiée
 *  @param  const QString &arg1
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::on_LE_Code_textEdited(const QString &arg1)
{
    ui->LE_Nom->clear() ;
}

/** Met   jour la liste des membres
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::on_MaJListeMembres()
{
    this->MaJListeMembres() ;
    this->RechercherParNomEtNumero() ;
}

/** Envoie un signal avec l'id du membre sélectionner avec le code membre quand ok appuyé
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::on_PB_Ok_clicked()
{
    int nBoucle (0) ;

    while( nBoucle < this->VecteurMembres.size()-1 && VecteurMembres[nBoucle].sCodeMembre != ui->LE_Code->text() )
    {
        nBoucle ++ ;
    }
    if( VecteurMembres[nBoucle].sCodeMembre == ui->LE_Code->text() )
    {
        emit( this->SignalRenvoieIdMembre( VecteurMembres[ nBoucle ].id ) );
    }

   ui->LE_Code->text() ;
}

/** Envoie un signal avec l'id du membre sélectionner avec le code membre quand la toucher entrée appuyée
 *  @test   Voir la procédure dans le fichier associé.
 */
void F_RechercheMembres::on_LE_Code_returnPressed()
{
    int nBoucle (0) ;

    while( nBoucle < this->VecteurMembres.size()-1 && VecteurMembres[nBoucle].sCodeMembre != ui->LE_Code->text() )
    {
        nBoucle ++ ;
    }
    if( VecteurMembres[nBoucle].sCodeMembre == ui->LE_Code->text() )
    {
        emit( this->SignalRenvoieIdMembre( VecteurMembres[ nBoucle ].id ) );
    }

   ui->LE_Code->text() ;
}

void F_RechercheMembres::on_TbW_Recherche_pressed(const QModelIndex &index)
{
}

void F_RechercheMembres::on_TbW_Recherche_activated(const QModelIndex &index)
{
}

void F_RechercheMembres::on_TbW_Recherche_entered(const QModelIndex &index)
{
}

