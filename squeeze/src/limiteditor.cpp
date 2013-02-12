#include "limiteditor.h"
#include "../../dataAccess/azahar.h"




limiteditor::limiteditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::limiteditor)
{
    ui->setupUi(this);
}

limiteditor::~limiteditor()
{
    delete ui;
}

#include "limiteditor.moc"
