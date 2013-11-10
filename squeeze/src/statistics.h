#ifndef STATISTICS_H
#define STATISTICS_H

#include <QWidget>
#include <QtGui>
#include "../../dataAccess/azahar.h"
#include <QtSql>
#include "ui_statistics.h"


namespace Ui {
class statistics;
}

class statistics : public QWidget
{
    Q_OBJECT
    
public:
    explicit statistics(QWidget *parent);
    ~statistics();
    Statistics stats;
    QStringList hCat;
    QStringList hProd;
    QSqlDatabase db;
    void setStats(Statistics &parentStats);
    void setDb(QSqlDatabase db);

private:
    Ui::statistics *ui;

public slots:
    void calc();
};

#endif // STATISTICS_H
