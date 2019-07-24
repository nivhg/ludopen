#ifndef W_CONTENUJEU_H
#define W_CONTENUJEU_H

#include <QWidget>
#include <QtWidgets>
#include <QDesignerFormEditorInterface>
#include <QSqlQuery>
#include <QSqlError>

#include "spinboxdelegate.h"
#include "fonctions_globale.h"
#include "f_mainwindow.h"

using namespace std;

#define PIECE_SEULE 0
#define GROUPE 1
#define PIECE_DANS_GROUPE 2

#define PIECEGROUPE Qt::UserRole
#define IDPIECES Qt::UserRole+1
#define IDJEUXOUIDPIECES Qt::UserRole+2
#define NBPIECESMANQUANTES Qt::UserRole+3

#define MODE_CONTENU 0
#define MODE_CONTENU_ET_MANQUANT 1

const QString MSG_AIDE_MANQUANT="Il suffit de cliquer sur la ligne correspondante à la pièce manquante ou abimée dans la colonne \"Manquant (abimé)\" du contenu du jeu.\n"
"Il est possible de rajouter une remarque dans l'historique de maintenance et de préciser qu'il s'agit d'une pièce abimée."
"Les pièces abimées seront retirées de la colonne \"Manquant (abimé)\" (elles sont encore présente dans le jeu, mais sont abimées).";

namespace Ui {
class W_ContenuJeu;
}


class W_ContenuJeu : public QWidget
{
    Q_OBJECT

public:
    explicit W_ContenuJeu(QWidget *parent = 0);
    ~W_ContenuJeu();
    void Definir_CodeJeu(QString CodeJeu);
    void Definir_Mode(int mode);
    void Definir_Main(F_MainWindow *Main);
    void ActiverBoutonsContenu(bool Etat);
    void Vider();

    //! modèle du TreeView du contenu des jeux
    QStandardItemModel * ModeleContenu;

/*    Ui::W_ContenuJeu *get_ui() const;
    QAction *get_actionPieceDsGroupe() const;
    bool isContainer() const;
       bool isInitialized() const;
       QIcon icon() const;
       QString domXml() const;
       QString group() const;
       QString includeFile() const;
       QString name() const;
       QString toolTip() const;
       QString whatsThis() const;
       QWidget *createWidget(QWidget *parent);
       void initialize(QDesignerFormEditorInterface *core);*/
public slots:
       void ActualiserContenu(int IdPieces=0);
       void on_TB_Valider_clicked();

private slots:
       void on_menuAjouterPiece_triggered();

       void on_Tv_Contenu_itemChanged(QStandardItem *item);

       void SelectionChanged_Tv_Contenu(const QItemSelection&,const QItemSelection&);

       void on_TB_Haut_clicked();

       void InverserElement(QModelIndex ElementChoisi,int LigneDestination);
       int MettreAjourElement(QModelIndex ElementChoisi,int LigneDestination,int IdJeux,int PieceGroupe);
       void SupprimerElement(QModelIndex ElementChoisi);
       void SupprimerPiece(int IdPieces);

       void on_TB_Bas_clicked();

       void on_TB_Supprimer_clicked();

       void on_TB_SacItem_clicked();

       QStringList NettoyerEtCouperLigne(QString Ligne);
       int InsererPiece(QString NombrePieces,int OrdrePieces,QString DescriptionPieces,int PieceGroupe,int IdJeuxOuIdPieces);

       void on_TB_Annuler_clicked();

       bool on_Tv_Contenu_editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
       void Signal_ActualiserHistoriqueMaintenance();

private:
    bool initialized;

    //! Choix du menu ajouter pour créer une pièce dans un groupe
    QAction *actionPieceDsGroupe;

    SpinBoxDelegate *DelegateContenu;

    Ui::W_ContenuJeu *ui;

    QString CodeJeu;

    int Mode;

    F_MainWindow *main;
};

#endif // W_CONTENUJEU_H
