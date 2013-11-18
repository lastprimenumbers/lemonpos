#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include <QWidget>
#include <QtGui>
#include "../../dataAccess/azahar.h"
#include <QtSql>

namespace Ui {
class transactions;
}

class transactions : public QWidget
{
    Q_OBJECT
    
public:
    explicit transactions(QWidget *parent = 0);
    ~transactions();
    QList<int> transaction_types;
    QSqlRelationalTableModel *transModel;
    QSqlRelationalTableModel *ticketModel;
    QSqlDatabase db;
    void setDb(QSqlDatabase parentDb);
    void setStats(Statistics &stats);
    void setProduct(QString &code);
    QString productCode;
    
private:
    Ui::transactions *ui;

public slots:
    void    ticketViewOnSelected(const QModelIndex & index);
};

#endif // TRANSACTIONS_H
