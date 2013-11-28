#include "transactions.h"
#include "ui_transactions.h"

#include <KLocale>
#include <KMessageBox>
#include <KFileDialog>

#include <QByteArray>

#include "../../mibitWidgets/mibitlineedit.h"
#include "../../src/misc.h"
#include "../../dataAccess/azahar.h"

transactions::transactions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::transactions)
{
    ui->setupUi(this);
    transModel=new QSqlRelationalTableModel();
    ticketModel=new QSqlRelationalTableModel();
    connect(ui->transView, SIGNAL(clicked(const QModelIndex &)), SLOT(ticketViewOnSelected(const QModelIndex &)));
}

void transactions::setDb(QSqlDatabase parentDb){
    db=parentDb;
    transModel->setTable("transactions");
    ui->transView->setModel(transModel);
    ui->transView->setColumnHidden(2,true);
    for (int i=6; i<=25; ++i) {
        ui->transView->setColumnHidden(i,true);
    }
//     Empty list
    transModel->setFilter(QString("clientid=-1"));
    transModel->select();
    ticketModel->setTable("transactionitems");
    ui->ticketView->setModel(ticketModel);
    ui->ticketView->setColumnHidden(0,true);
    ui->ticketView->setColumnHidden(1,true);
    ui->ticketView->setColumnHidden(4,true);
    ui->ticketView->setColumnHidden(5,true);
    ui->ticketView->setColumnHidden(6,true);
    for (int i=11; i<=16; ++i) {
        ui->ticketView->setColumnHidden(i,true);
    }

}


void transactions::setStats(Statistics &stats) {
    //! Set client/donor statistics.
    Azahar *myDb=new Azahar;
    QStringList instat=myDb->getInStatements(stats);
    delete myDb;
    QString f;

    // Select only SELL transaction (type=1) with corresponding client id.
    if (stats.type.contains(1)) {
        f=QString("( clientid IN (%1) )").arg(instat[0]);
        // Model connecting clientid column to client code.
//        transModel->setRelation(1, QSqlRelation("clients", "id", "code"));

    }
    else if (stats.type.contains(2) or stats.type.contains(7)) {
        f=QString("( donor IN (%1) )").arg(instat[1]);
        // Model connecting clientid with donor name
//        transModel->setRelation(24, QSqlRelation("donors", "code", "name"));
        ui->transView->setColumnHidden(24,false);
        ui->transView->setColumnHidden(1,true);
    }

    transModel->setFilter(f);
    transModel->select();
}

void transactions::setProduct(QString &code) {
    // View purchase/donation transactions regarding a product
    qDebug()<<"Setting product code"<<code;
    ui->transView->setColumnHidden(24,false);
    ui->transView->setColumnHidden(1,true);
    transModel->setRelation(24, QSqlRelation("donors", "code", "name"));
//    transModel->setRelation(1, QSqlRelation("clients", "id", "code"));
    transModel->setFilter(QString("itemslist REGEXP '%1/' AND type IN (2,7)").arg(code));
    transModel->select();
    productCode=code;
}

void transactions::ticketViewOnSelected(const QModelIndex & index) {
        //getting data from model...
        const QAbstractItemModel *model = index.model();
        int row = index.row();
        int fid=transModel->fieldIndex("id");
        QModelIndex indx = model->index(row, fid);
        QString tid=model->data(indx, Qt::DisplayRole).toString();
        if (productCode=="") {
            ticketModel->setFilter(QString("transaction_id=%1").arg(tid));
        } else {
            ticketModel->setFilter(QString("transaction_id=%1 AND product_id=%2").arg(tid,productCode));
        }
        ticketModel->select();
}

transactions::~transactions()
{
    delete ui;
}
