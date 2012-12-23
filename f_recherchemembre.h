#ifndef F_RECHERCHEMEMBRE_H
#define F_RECHERCHEMEMBRE_H

#include <QWidget>

namespace Ui {
class F_RechercheMembre;
}

class F_RechercheMembre : public QWidget
{
    Q_OBJECT
    
public:
    explicit F_RechercheMembre(QWidget *parent = 0);
    ~F_RechercheMembre();
    
private:
    Ui::F_RechercheMembre *ui;
};

#endif // F_RECHERCHEMEMBRE_H
