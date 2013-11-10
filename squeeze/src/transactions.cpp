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
    transModel=new QSqlTableModel();
    ticketModel=new QSqlTableModel();
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
    ui->ticketView->setColumnHidden(5,true);
    ui->ticketView->setColumnHidden(6,true);
    for (int i=11; i<=16; ++i) {
        ui->ticketView->setColumnHidden(i,true);
    }
}

void transactions::setStats(Statistics &stats) {
    Azahar *myDb=new Azahar;
    QStringList instat=myDb->getInStatements(stats);
    transModel->setFilter("");
    transModel->setFilter(QString("( clientid IN (%1) or  donor IN (%2) )").arg(instat[0],instat[1]));
    transModel->select();
}

void transactions::setProduct(QString &code) {
    qDebug()<<"Setting product code"<<code;
    transModel->setFilter("");
    transModel->setFilter(QString("productCode=%1").arg(code));
    transModel->select();
}

void transactions::ticketViewOnSelected(const QModelIndex & index) {
        //getting data from model...
        const QAbstractItemModel *model = index.model();
        int row = index.row();
        int fid=transModel->fieldIndex("id");
        QModelIndex indx = model->index(row, fid);
        qulonglong tid=model->data(indx, Qt::DisplayRole).toULongLong();
        ticketModel->setFilter(QString("transaction_id=%1").arg(tid));
        ticketModel->select();
}

transactions::~transactions()
{
    delete ui;
}
