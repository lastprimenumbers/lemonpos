#include "productinfo.h"
#include "ui_productinfo.h"

#include <KLocale>
#include <KMessageBox>
#include <KFileDialog>
#include <kiconloader.h>
#include <kstandarddirs.h>

#include <QByteArray>
#include <QRegExpValidator>

#include "../../dataAccess/azahar.h"
#include "../../mibitWidgets/mibittip.h"

productView::productView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::productView)
{
    ui->setupUi(this);
}

productView::setDb(QSqlDatabase database) {
    db=database;
    ui->edit->setDb(db);
    ui->trans->setDb(db);
    ui->stats->setDb(db);
}

productView::~productView()
{
    delete ui;
}
