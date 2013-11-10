#include "statistics.h"
#include "ui_statistics.h"
#include <KLocale>
#include <KMessageBox>
#include <KFileDialog>

#include <QByteArray>

#include "../../mibitWidgets/mibitlineedit.h"
#include "../../src/misc.h"
#include "../../dataAccess/azahar.h"


statistics::statistics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::statistics)
{
    ui->setupUi(this);

    // STATS
    connect(ui->statDo,SIGNAL(clicked()),this,SLOT(calc()));
    ui->statEnd->setDate(QDate::currentDate());
}

void statistics::setStats(Statistics &parentStats) {
    stats=parentStats;
}
void statistics::setDb(QSqlDatabase parentDb) {
    db=parentDb;
}


void statistics::calc(){
    //!Update statistical tables
    stats.start=ui->statStart->date();
    stats.end=ui->statEnd->date();
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    stats=myDb->getStatistics(stats);
    QHash<QString,int> cats=myDb->getCategoriesHash();
    delete myDb;
    ui->statTotal->setText(QString::number(stats.total));

    // Fill the category table
    QList<int> ck=stats.categories.keys();
    ui->categoryTable->setRowCount(ck.count());
    ui->categoryTable->setColumnCount(3);

    for (int i=0; i<ck.count(); i++) {
        ui->categoryTable->setItem(i,0,new QTableWidgetItem(QString::number(ck[i])));
        ui->categoryTable->setItem(i,1,new QTableWidgetItem(cats.key(ck[i])));
        ui->categoryTable->setItem(i,2,new QTableWidgetItem(QString::number(stats.categories[ck[i]])));
    }

    // Fill the product table
    QList<QString> pk=stats.products.keys();
    qDebug()<<"Mapped products"<<pk.count();
    ui->productTable->setRowCount(pk.count());
    ui->productTable->setColumnCount(3);
    for (int i=0; i<pk.count(); i++) {
        qDebug()<<"Appending product"<<pk[i]<<stats.products[pk[i]];
        ui->productTable->setItem(i,0,new QTableWidgetItem(pk[i]));
        ui->productTable->setItem(i,1,new QTableWidgetItem(stats.items[pk[i]].name));
        ui->productTable->setItem(i,2,new QTableWidgetItem(QString::number(stats.products[pk[i]])));
    }

}


statistics::~statistics()
{
    delete ui;
}
