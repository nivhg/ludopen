//-------------------------------------------------------------------------------
/** @file         f_postit.cpp
 *  @brief        Permet de consulter la liste de tout les jeux
 *
 *  @author       Florian MARY
 *  @author       STS IRIS, Lycée Nicolas APPERT, ORVAULT (FRANCE)
 *  @since        07/07/2012
 *  @version      0.1
 *  @date         15/12/2012
 *
 *  Permet de consulter la liste de tout les jeux ainsi que de faire un filtre par critčres
 *
 *  Fabrication   QtCreator
 *  @todo        Corriger le bug des accents dans les post it
 *
 *
 */
//-------------------------------------------------------------------------------

#include "f_postit.h"
#include "ui_f_postit.h"

#include <QtSql>

F_POSTIT::F_POSTIT(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::F_POSTIT)
{
    ui->setupUi(this);
    QSqlQuery RemplirPostIt ;
    RemplirPostIt.exec("SELECT PostIt1, PostIt2, PostIt3, PostIt4, PostIt5, PostIt6, PostIt7, PostIt8 FROM postit") ;
    RemplirPostIt.next() ;

    ui->TxE_PostIt1->setText(RemplirPostIt.value(0).toString());
    ui->TxE_PostIt2->setText(RemplirPostIt.value(1).toString());
    ui->TxE_PostIt3->setText(RemplirPostIt.value(2).toString());
    ui->TxE_PostIt4->setText(RemplirPostIt.value(3).toString());
    ui->TxE_PostIt5->setText(RemplirPostIt.value(4).toString());
    ui->TxE_PostIt6->setText(RemplirPostIt.value(5).toString());
    ui->TxE_PostIt7->setText(RemplirPostIt.value(6).toString());
    ui->TxE_PostIt8->setText(RemplirPostIt.value(7).toString());

/*
    qDebug()<<"F_POSTIT::F_POSTIT=========1========="<<ui->TxE_PostIt1->toPlainText();
    qDebug()<<"F_POSTIT::F_POSTIT=========2========="<<ui->TxE_PostIt2->toPlainText();
    qDebug()<<"F_POSTIT::F_POSTIT=========3========="<<ui->TxE_PostIt3->toPlainText();
    qDebug()<<"F_POSTIT::F_POSTIT=========4========="<<ui->TxE_PostIt4->toPlainText();
    qDebug()<<"F_POSTIT::F_POSTIT=========5========="<<ui->TxE_PostIt5->toPlainText();
    qDebug()<<"F_POSTIT::F_POSTIT=========6========="<<ui->TxE_PostIt6->toPlainText();
    qDebug()<<"F_POSTIT::F_POSTIT=========7========="<<ui->TxE_PostIt7->toPlainText();
    qDebug()<<"F_POSTIT::F_POSTIT=========8========="<<ui->TxE_PostIt8->toPlainText();
*/

    ui->Bt_Annuler1->setDisabled(true);
    ui->Bt_Annuler2->setDisabled(true);
    ui->Bt_Annuler3->setDisabled(true);
    ui->Bt_Annuler4->setDisabled(true);
    ui->Bt_Annuler5->setDisabled(true);
    ui->Bt_Annuler6->setDisabled(true);
    ui->Bt_Annuler7->setDisabled(true);
    ui->Bt_Annuler8->setDisabled(true);

    ui->Bt_Valider1->setDisabled(true);
    ui->Bt_Valider2->setDisabled(true);
    ui->Bt_Valider3->setDisabled(true);
    ui->Bt_Valider4->setDisabled(true);
    ui->Bt_Valider5->setDisabled(true);
    ui->Bt_Valider6->setDisabled(true);
    ui->Bt_Valider7->setDisabled(true);
    ui->Bt_Valider8->setDisabled(true);
/*
    ui->Bt_Annuler2->setVisible(false);
    ui->Bt_Valider2->setVisible(false);
    ui->Lb_PostIt2->setVisible(false);
    ui->TxE_PostIt2->setVisible(false);

    ui->Bt_Annuler3->setVisible(false);
    ui->Bt_Valider3->setVisible(false);
    ui->Lb_PostIt3->setVisible(false);
    ui->TxE_PostIt3->setVisible(false);

    ui->Bt_Annuler4->setVisible(false);
    ui->Bt_Valider4->setVisible(false);
    ui->Lb_PostIt4->setVisible(false);
    ui->TxE_PostIt4->setVisible(false);

    ui->Bt_Annuler5->setVisible(false);
    ui->Bt_Valider5->setVisible(false);
    ui->Lb_PostIt5->setVisible(false);
    ui->TxE_PostIt5->setVisible(false);

    ui->Bt_Annuler6->setVisible(false);
    ui->Bt_Valider6->setVisible(false);
    ui->Lb_PostIt6->setVisible(false);
    ui->TxE_PostIt6->setVisible(false);

    ui->Bt_Annuler7->setVisible(false);
    ui->Bt_Valider7->setVisible(false);
    ui->Lb_PostIt7->setVisible(false);
    ui->TxE_PostIt7->setVisible(false);

    ui->Bt_Annuler8->setVisible(false);
    ui->Bt_Valider8->setVisible(false);
    ui->Lb_PostIt8->setVisible(false);
    ui->TxE_PostIt8->setVisible(false);
*/
    ui->Bt_AjouterPostIt->setVisible(false);
}

F_POSTIT::~F_POSTIT()
{
    delete ui;
}

void F_POSTIT::on_TxE_PostIt1_textChanged()
{
  ui->Bt_Annuler1->setEnabled(true);
  ui->Bt_Valider1->setEnabled(true);
}

void F_POSTIT::on_Bt_Valider1_clicked()
{
    QSqlQuery RequeteValiderPostIt1;

//prépare le requęte de mise ŕ jour
    RequeteValiderPostIt1.prepare("UPDATE postit SET PostIt1=:LePostIt1");

//Entre les valeurs de la requęte
    RequeteValiderPostIt1.bindValue(":LePostIt1",ui->TxE_PostIt1->toPlainText());

 //Execut la requęte
    if (!RequeteValiderPostIt1.exec())
    {
        qDebug() << "f_postit::on_TxE_PostIt1_textChanged()" << RequeteValiderPostIt1.lastQuery() ;
    }
    ui->Bt_Annuler1->setEnabled(false);
    ui->Bt_Valider1->setEnabled(false);
}

void F_POSTIT::on_Bt_Annuler1_clicked()
{
    QSqlQuery RequeteAnnulerPostIt1;

    RequeteAnnulerPostIt1.exec("SELECT PostIt1 FROM postit");
    RequeteAnnulerPostIt1.next();

//Récupčre les remarques dans la base de données et les affiches
    QString PostIt1 = (RequeteAnnulerPostIt1.value(0).toString());
    ui->TxE_PostIt1->setText(PostIt1);

//Grise les boutons de modification du contenu
    ui->Bt_Annuler1->setEnabled(false);
    ui->Bt_Valider1->setEnabled(false);
}

void F_POSTIT::on_TxE_PostIt2_textChanged()
{
    ui->Bt_Annuler2->setEnabled(true);
    ui->Bt_Valider2->setEnabled(true);
}

void F_POSTIT::on_Bt_Valider2_clicked()
{
    QSqlQuery RequeteValiderPostIt2;

//prépare le requęte de mise à jour
    RequeteValiderPostIt2.prepare("UPDATE postit SET PostIt2=:LePostIt2");

//Entre les valeurs de la requęte
    RequeteValiderPostIt2.bindValue(":LePostIt2",ui->TxE_PostIt2->toPlainText());

 //Execut la requęte
    if (!RequeteValiderPostIt2.exec())
    {
        qDebug() << "f_postit::on_TxE_PostIt2_textChanged()" << RequeteValiderPostIt2.lastQuery() ;
    }
    ui->Bt_Annuler2->setEnabled(false);
    ui->Bt_Valider2->setEnabled(false);
}

void F_POSTIT::on_Bt_Annuler2_clicked()
{
    QSqlQuery RequeteAnnulerPostIt2;

    RequeteAnnulerPostIt2.exec("SELECT PostIt2 FROM postit");
    RequeteAnnulerPostIt2.next();

//Récupčre les remarques dans la base de données et les affiches
    QString PostIt2 = (RequeteAnnulerPostIt2.value(0).toString());
    ui->TxE_PostIt2->setText(PostIt2);

//Grise les boutons de modification du contenu
    ui->Bt_Annuler2->setEnabled(false);
    ui->Bt_Valider2->setEnabled(false);
}

void F_POSTIT::on_TxE_PostIt3_textChanged()
{
    ui->Bt_Annuler3->setEnabled(true);
    ui->Bt_Valider3->setEnabled(true);
}

void F_POSTIT::on_Bt_Valider3_clicked()
{
    QSqlQuery RequeteValiderPostIt3;

//prépare le requęte de mise ŕ jour
    RequeteValiderPostIt3.prepare("UPDATE postit SET PostIt3=:LePostIt3");

//Entre les valeurs de la requęte
    RequeteValiderPostIt3.bindValue(":LePostIt3",ui->TxE_PostIt3->toPlainText());

 //Execut la requęte
    if (!RequeteValiderPostIt3.exec())
    {
        qDebug() << "f_postit::on_TxE_PostIt3_textChanged()" << RequeteValiderPostIt3.lastQuery() ;
    }
    ui->Bt_Annuler3->setEnabled(false);
    ui->Bt_Valider3->setEnabled(false);
}

void F_POSTIT::on_Bt_Annuler3_clicked()
{
    QSqlQuery RequeteAnnulerPostIt3;

    RequeteAnnulerPostIt3.exec("SELECT PostIt3 FROM postit");
    RequeteAnnulerPostIt3.next();

//Récupčre les remarques dans la base de données et les affiches
    QString PostIt3 = (RequeteAnnulerPostIt3.value(0).toString());
    ui->TxE_PostIt3->setText(PostIt3);

//Grise les boutons de modification du contenu
    ui->Bt_Annuler3->setEnabled(false);
    ui->Bt_Valider3->setEnabled(false);
}


void F_POSTIT::on_TxE_PostIt4_textChanged()
{
    ui->Bt_Annuler4->setEnabled(true);
    ui->Bt_Valider4->setEnabled(true);
}


void F_POSTIT::on_Bt_Valider4_clicked()
{
    QSqlQuery RequeteValiderPostIt4;

//prépare le requęte de mise ŕ jour
    RequeteValiderPostIt4.prepare("UPDATE postit SET PostIt4=:LePostIt4");

//Entre les valeurs de la requęte
    RequeteValiderPostIt4.bindValue(":LePostIt4",ui->TxE_PostIt4->toPlainText());

 //Execut la requęte
    if (!RequeteValiderPostIt4.exec())
    {
        qDebug() << "f_postit::on_TxE_PostIt4_textChanged()" << RequeteValiderPostIt4.lastQuery() ;
    }
    ui->Bt_Annuler4->setEnabled(false);
    ui->Bt_Valider4->setEnabled(false);
}

void F_POSTIT::on_Bt_Annuler4_clicked()
{
    QSqlQuery RequeteAnnulerPostIt4;

    RequeteAnnulerPostIt4.exec("SELECT PostIt4 FROM postit");
    RequeteAnnulerPostIt4.next();

//Récupčre les remarques dans la base de données et les affiches
    QString PostIt4 = (RequeteAnnulerPostIt4.value(0).toString());
    ui->TxE_PostIt4->setText(PostIt4);

//Grise les boutons de modification du contenu
    ui->Bt_Annuler4->setEnabled(false);
    ui->Bt_Valider4->setEnabled(false);
}

void F_POSTIT::on_TxE_PostIt5_textChanged()
{
    ui->Bt_Annuler5->setEnabled(true);
    ui->Bt_Valider5->setEnabled(true);
}

void F_POSTIT::on_Bt_Valider5_clicked()
{
    QSqlQuery RequeteValiderPostIt5;

//prépare le requête de mise à jour
    RequeteValiderPostIt5.prepare("UPDATE postit SET PostIt5=:LePostIt5");

//Entre les valeurs de la requęte
    RequeteValiderPostIt5.bindValue(":LePostIt5",ui->TxE_PostIt5->toPlainText());

 //Execut la requęte
    if (!RequeteValiderPostIt5.exec())
    {
        qDebug() << "f_postit::on_TxE_PostIt5_textChanged()" << RequeteValiderPostIt5.lastQuery() ;
    }
    ui->Bt_Annuler5->setEnabled(false);
    ui->Bt_Valider5->setEnabled(false);
}


void F_POSTIT::on_Bt_Annuler5_clicked()
{
    QSqlQuery RequeteAnnulerPostIt5;

    RequeteAnnulerPostIt5.exec("SELECT PostIt5 FROM postit");
    RequeteAnnulerPostIt5.next();

//Récupčre les remarques dans la base de données et les affiches
    QString PostIt5 = (RequeteAnnulerPostIt5.value(0).toString());
    ui->TxE_PostIt5->setText(PostIt5);

//Grise les boutons de modification du contenu
    ui->Bt_Annuler5->setEnabled(false);
    ui->Bt_Valider5->setEnabled(false);
}


void F_POSTIT::on_TxE_PostIt6_textChanged()
{
    ui->Bt_Annuler6->setEnabled(true);
    ui->Bt_Valider6->setEnabled(true);
}


void F_POSTIT::on_Bt_Valider6_clicked()
{
    QSqlQuery RequeteValiderPostIt6;

//prépare le requęte de mise ŕ jour
    RequeteValiderPostIt6.prepare("UPDATE postit SET PostIt6=:LePostIt6");

//Entre les valeurs de la requęte
    RequeteValiderPostIt6.bindValue(":LePostIt6",ui->TxE_PostIt6->toPlainText());

 //Execut la requęte
    if (!RequeteValiderPostIt6.exec())
    {
        qDebug() << "f_postit::on_TxE_PostIt6_textChanged()" << RequeteValiderPostIt6.lastQuery() ;
    }
    ui->Bt_Annuler6->setEnabled(false);
    ui->Bt_Valider6->setEnabled(false);
}


void F_POSTIT::on_Bt_Annuler6_clicked()
{
    QSqlQuery RequeteAnnulerPostIt6;

    RequeteAnnulerPostIt6.exec("SELECT PostIt6 FROM postit");
    RequeteAnnulerPostIt6.next();

//Récupčre les remarques dans la base de données et les affiches
    QString PostIt6 = (RequeteAnnulerPostIt6.value(0).toString());
    ui->TxE_PostIt6->setText(PostIt6);

//Grise les boutons de modification du contenu
    ui->Bt_Annuler6->setEnabled(false);
    ui->Bt_Valider6->setEnabled(false);
}


void F_POSTIT::on_TxE_PostIt7_textChanged()
{
    ui->Bt_Annuler7->setEnabled(true);
    ui->Bt_Valider7->setEnabled(true);
}

void F_POSTIT::on_Bt_Valider7_clicked()
{
    QSqlQuery RequeteValiderPostIt7;

//prépare le requęte de mise ŕ jour
    RequeteValiderPostIt7.prepare("UPDATE postit SET PostIt7=:LePostIt7");

//Entre les valeurs de la requęte
    RequeteValiderPostIt7.bindValue(":LePostIt7",ui->TxE_PostIt7->toPlainText());

 //Execut la requęte
    if (!RequeteValiderPostIt7.exec())
    {
        qDebug() << "f_postit::on_TxE_PostIt7_textChanged()" << RequeteValiderPostIt7.lastQuery() ;
    }
    ui->Bt_Annuler7->setEnabled(false);
    ui->Bt_Valider7->setEnabled(false);
}

void F_POSTIT::on_Bt_Annuler7_clicked()
{
    QSqlQuery RequeteAnnulerPostIt7;

    RequeteAnnulerPostIt7.exec("SELECT PostIt7 FROM postit");
    RequeteAnnulerPostIt7.next();

//Récupčre les remarques dans la base de données et les affiches
    QString PostIt7 = (RequeteAnnulerPostIt7.value(0).toString());
    ui->TxE_PostIt7->setText(PostIt7);

//Grise les boutons de modification du contenu
    ui->Bt_Annuler7->setEnabled(false);
    ui->Bt_Valider7->setEnabled(false);
}

void F_POSTIT::on_TxE_PostIt8_textChanged()
{
    ui->Bt_Annuler8->setEnabled(true);
    ui->Bt_Valider8->setEnabled(true);
}


void F_POSTIT::on_Bt_Valider8_clicked()
{
    QSqlQuery RequeteValiderPostIt8;

//prépare le requęte de mise ŕ jour
    RequeteValiderPostIt8.prepare("UPDATE postit SET PostIt8=:LePostIt8");

//Entre les valeurs de la requęte
    RequeteValiderPostIt8.bindValue(":LePostIt8",ui->TxE_PostIt8->toPlainText());

 //Execut la requęte
    if (!RequeteValiderPostIt8.exec())
    {
        qDebug() << "f_postit::on_TxE_PostIt8_textChanged()" << RequeteValiderPostIt8.lastQuery() ;
    }
    ui->Bt_Annuler8->setEnabled(false);
    ui->Bt_Valider8->setEnabled(false);
}

void F_POSTIT::on_Bt_Annuler8_clicked()
{
    QSqlQuery RequeteAnnulerPostIt8;

    RequeteAnnulerPostIt8.exec("SELECT PostIt8 FROM postit");
    RequeteAnnulerPostIt8.next();

//Récupčre les remarques dans la base de données et les affiches
    QString PostIt8 = (RequeteAnnulerPostIt8.value(0).toString());
    ui->TxE_PostIt8->setText(PostIt8);

//Grise les boutons de modification du contenu
    ui->Bt_Annuler8->setEnabled(false);
    ui->Bt_Valider8->setEnabled(false);
}
/*
void F_POSTIT::on_Bt_AjouterPostIt_clicked()
{
    if(ui->Lb_PostIt7->isVisible())
    {
        ui->Bt_Annuler8->setVisible(true);
        ui->Bt_Valider8->setVisible(true);
        ui->Bt_Annuler8->setDisabled(true);
        ui->Bt_Valider8->setDisabled(true);
        ui->Lb_PostIt8->setVisible(true);
        ui->TxE_PostIt8->setVisible(true);
    }
    if(ui->Lb_PostIt6->isVisible())
    {
        ui->Bt_Annuler7->setVisible(true);
        ui->Bt_Valider7->setVisible(true);
        ui->Bt_Annuler7->setDisabled(true);
        ui->Bt_Valider7->setDisabled(true);
        ui->Lb_PostIt7->setVisible(true);
        ui->TxE_PostIt7->setVisible(true);
    }
    if(ui->Lb_PostIt5->isVisible())
    {
        ui->Bt_Annuler6->setVisible(true);
        ui->Bt_Valider6->setVisible(true);
        ui->Bt_Annuler6->setDisabled(true);
        ui->Bt_Valider6->setDisabled(true);
        ui->Lb_PostIt6->setVisible(true);
        ui->TxE_PostIt6->setVisible(true);
    }
    if(ui->Lb_PostIt4->isVisible())
    {
        ui->Bt_Annuler5->setVisible(true);
        ui->Bt_Valider5->setVisible(true);
        ui->Bt_Annuler5->setDisabled(true);
        ui->Bt_Valider5->setDisabled(true);
        ui->Lb_PostIt5->setVisible(true);
        ui->TxE_PostIt5->setVisible(true);
    }
    if(ui->Lb_PostIt3->isVisible())
    {
        ui->Bt_Annuler4->setVisible(true);
        ui->Bt_Valider4->setVisible(true);
        ui->Bt_Annuler4->setDisabled(true);
        ui->Bt_Valider4->setDisabled(true);
        ui->Lb_PostIt4->setVisible(true);
        ui->TxE_PostIt4->setVisible(true);
    }
    if(ui->Lb_PostIt2->isVisible())
    {
        ui->Bt_Annuler3->setVisible(true);
        ui->Bt_Valider3->setVisible(true);
        ui->Bt_Annuler3->setDisabled(true);
        ui->Bt_Valider3->setDisabled(true);
        ui->Lb_PostIt3->setVisible(true);
        ui->TxE_PostIt3->setVisible(true);
    }
    if(ui->Lb_PostIt2->isHidden())
    {
        ui->Bt_Annuler2->setVisible(true);
        ui->Bt_Valider2->setVisible(true);
        ui->Lb_PostIt2->setVisible(true);
        ui->TxE_PostIt2->setVisible(true);
        ui->Bt_Annuler2->setDisabled(true);
        ui->Bt_Valider2->setDisabled(true);
    }
}
*/

