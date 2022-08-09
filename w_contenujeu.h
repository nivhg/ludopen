#ifndef W_CONTENUJEU_H
#define W_CONTENUJEU_H

#include <QWidget>
#include <QtWidgets>
//#include <QDesignerFormEditorInterface>
#include <QSqlQuery>
#include <QSqlError>

#include "spinboxdelegate.h"
#include "f_popupclesttem.h"
#include "fonctions_globale.h"
#include "f_mainwindow.h"

//class F_PopUpCLESTTEM;

using namespace std;

#define PIECE_SEULE 0
#define GROUPE 1
#define PIECE_DANS_GROUPE 2

#define PIECEGROUPE Qt::UserRole
#define IDPIECES Qt::UserRole+1
#define IDJEUXOUIDPIECES Qt::UserRole+2
#define NBPIECESMANQUANTES Qt::UserRole+3

#define MODE_CONTENU 0
#define MODE_LECTURE_SEULE 1

const QString MSG_AIDE_MANQUANT="Pour déclarer une pièce manquante ou abimée, changer la valeur de la colonne \"Manquant\" du contenu du jeu.\n"
                                "Une fois la valeur changée, une ligne apparait dans l'historique de maintenance.\n"
                                "Pour déclarer qu'elle est abimée, cocher la case appropriée.";

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
    void Initialisation(int mode,F_MainWindow *Main,QString Remarque="");
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
       void slot_contenujeu_clicked();

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

       bool on_Tv_Contenu_editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

       void on_TB_Ciseaux_clicked();

       void on_TB_Texte_clicked();

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

    QString RemarquePiecesManquantes;

    F_PopUpCLESTTEM *pPopUpCLESTTEM;
};

#endif // W_CONTENUJEU_H
