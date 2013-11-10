#include <KLocale>
#include <KMessageBox>
#include <QtSql>
#include "../../dataAccess/azahar.h"

#include "productView.h"
#include "ui_productView.h"


ProductViewUI::ProductViewUI( QWidget *parent )
: QFrame( parent )
{
    setupUi( this );
}

ProductView::ProductView(QWidget *parent)
: QWidget(parent)
{
    ui = new ProductViewUI( this );
    ui->setupUi(this);
    setWindowTitle( i18n("Product Editor") );
}

void ProductView::setDb(QSqlDatabase database) {
    db=database;
    ui->edit->setDb(database);
    ui->trans->setDb(database);
    ui->stats->setDb(database);
}

void ProductView::setNewProduct(bool newProduct) {
    ui->edit->setNewProduct(newProduct);
    ui->trans->setEnabled(!newProduct);
    ui->stats->setEnabled(!newProduct);
}

void ProductView::setCode(QString c){
    ui->edit->setCode(c);
    ui->trans->setProduct(c);
}

ProductView::~ProductView()
{
    delete ui;
}

#include "productView.moc"
