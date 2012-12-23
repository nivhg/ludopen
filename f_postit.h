#ifndef F_POSTIT_H
#define F_POSTIT_H

#include <QWidget>

namespace Ui {
class F_POSTIT;
}

class F_POSTIT : public QWidget
{
    Q_OBJECT
    
public:
    explicit F_POSTIT(QWidget *parent = 0);
    ~F_POSTIT();
    
private slots:
    void on_TxE_PostIt1_textChanged();

    void on_Bt_Valider1_clicked();

    void on_Bt_Annuler1_clicked();

    void on_TxE_PostIt2_textChanged();

    void on_Bt_Valider2_clicked();

    void on_Bt_Annuler2_clicked();

    void on_TxE_PostIt3_textChanged();

    void on_Bt_Valider3_clicked();

    void on_Bt_Annuler3_clicked();

    void on_TxE_PostIt4_textChanged();

    void on_Bt_Valider4_clicked();

    void on_Bt_Annuler4_clicked();

    void on_TxE_PostIt5_textChanged();

    void on_Bt_Valider5_clicked();

    void on_Bt_Annuler5_clicked();

    void on_TxE_PostIt6_textChanged();

    void on_Bt_Valider6_clicked();

    void on_Bt_Annuler6_clicked();

    void on_TxE_PostIt7_textChanged();

    void on_Bt_Valider7_clicked();

    void on_Bt_Annuler7_clicked();

    void on_TxE_PostIt8_textChanged();

    void on_Bt_Valider8_clicked();

    void on_Bt_Annuler8_clicked();


    //void on_Bt_AjouterPostIt_clicked();


private:
    Ui::F_POSTIT *ui;
};

#endif // F_POSTIT_H
