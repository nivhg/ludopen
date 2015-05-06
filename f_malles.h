#ifndef F_MALLES_H
#define F_MALLES_H

#include <QWidget>

namespace Ui {
class F_Malles;
}

class F_Malles : public QWidget
{
    Q_OBJECT

public:
    explicit F_Malles(QWidget *parent = 0);
    ~F_Malles();

private:
    Ui::F_Malles *ui;
};

#endif // F_MALLES_H
