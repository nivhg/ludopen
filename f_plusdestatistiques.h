#ifndef F_PLUSDESTATISTIQUES_H
#define F_PLUSDESTATISTIQUES_H

#include <QMainWindow>

namespace Ui {
    class F_PlusDeStatistiques;
}

class F_PlusDeStatistiques : public QMainWindow
{
    Q_OBJECT

public:
    explicit F_PlusDeStatistiques(QWidget *parent = 0);
    ~F_PlusDeStatistiques();

private:
    Ui::F_PlusDeStatistiques *ui;
};

#endif // F_PLUSDESTATISTIQUES_H
