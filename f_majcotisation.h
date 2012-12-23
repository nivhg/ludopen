#ifndef F_MAJCOTISATION_H
#define F_MAJCOTISATION_H

#include <QWidget>

namespace Ui {
    class F_MajCotisation;
}

class F_MajCotisation : public QWidget
{
    Q_OBJECT

public:
    explicit F_MajCotisation(QWidget *parent = 0);
    ~F_MajCotisation();

private:
    Ui::F_MajCotisation *ui;
};

#endif // F_MAJCOTISATION_H
