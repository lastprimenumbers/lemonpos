#ifndef PRODUCTINFO_H
#define PRODUCTINFO_H

#include <QWidget>
#include <KDateTime>
#include <QDate>
#include <QtGui>
#include <QPixmap>
#include <QtSql>
#include "../../src/structs.h"
#include "../../src/misc.h"

namespace Ui {
class productView;
}

class productView : public QWidget
{
    Q_OBJECT
    
public:
    explicit productView(QWidget *parent = 0);
    ~productView();
    Ui::productView *ui;
    QSqlDatabase db;
    void    setDb(QSqlDatabase database);

    // Routing functions
    void    setModel(QSqlRelationalTableModel *model) {ui->edit->setModel(model);};
    void    setCode(QString c)      {ui->edit->setCode(c); };
    void    setAlphacode(QString c)    { ui->edit->setAlphacode(c); };
    void    disableCode()              { ui->edit->disableCode(); };
    void    enableCode()               { ui->edit->enableCode();};
    void    setStockQtyReadOnly()      { ui->edit->setStockQtyReadOnly();};
    void    disableStockCorrection()   { ui->edit->disableStockCorrection();};

    ProductInfo getProductInfo()    { ui->edit->getProductInfo();};
    bool        isCorrectingStock() { ui->edit->isCorrectingStock();};
    double  getOldStock()           { ui->edit->getOldStock(); };
    double  getGRoupStockMax()      { ui->edit->getGRoupStockMax(); };;
    double  getGroupPriceDrop()     { ui->edit->getGroupPriceDrop(); };
    QString getReason()             { ui->edit->getReason(); };
    double  getStockQty()           { ui->edit->getStockQty();};
    int result()                    { ui->edit->result() ;};

#endif // PRODUCTINFO_H
