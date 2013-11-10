#ifndef PRODUCTVIEW_H
#define PRODUCTVIEW_H

#include <QWidget>
#include <KDateTime>
#include <QDate>
#include <QtGui>
#include <QPixmap>
#include <QtSql>
#include <QtCore>
#include "../../src/structs.h"
#include "../../src/misc.h"

#include "ui_productView.h"


//, public Ui::productView
class ProductViewUI : public QFrame, public Ui::productView
{
    Q_OBJECT
    public:
      ProductViewUI( QWidget *parent=0 );
};

class ProductView : public QWidget
{
    Q_OBJECT
public:
    ProductView(QWidget *parent = 0 );
    ~ProductView();
    ProductViewUI *ui;
    QSqlDatabase db;
    void    setDb(QSqlDatabase database);
    void    setNewProduct(bool newProduct);

    // Routing functions
    void    setModel(QSqlRelationalTableModel *model) { ui->edit->setModel(model);};
    void    setCode(QString c);
    void    setAlphacode(QString c)    { ui->edit->setAlphacode(c); };
    void    disableCode()              { ui->edit->disableCode(); };
    void    enableCode()               { ui->edit->enableCode();};
    void    setStockQtyReadOnly(bool enabled)      { ui->edit->setStockQtyReadOnly(enabled);};
    void    disableStockCorrection()   { ui->edit->disableStockCorrection();};

    ProductInfo getProductInfo()    {return  ui->edit->getProductInfo();};
    bool        isCorrectingStock() {return  ui->edit->isCorrectingStock();};
    double  getOldStock()           {return  ui->edit->getOldStock(); };
    double  getGRoupStockMax()      {return  ui->edit->getGRoupStockMax(); };
    double  getGroupPriceDrop()     {return  ui->edit->getGroupPriceDrop(); };
    QString getReason()             {return  ui->edit->getReason(); };
    double  getStockQty()           {return  ui->edit->getStockQty();};
    int result()                    {return  ui->edit->result() ;};
    QString getCode()     { return ui->edit->getCode(); };
};

#endif // PRODUCTVIEW_H
