#include "limiteditor.h"
#include "../../dataAccess/azahar.h"




limiteditor::limiteditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::limit_editor)
{
    ui->setupUi(this);
}

void limiteditor::setDb(QSqlDatabase parentDb)
{
    db = parentDb;
    if (!db.isOpen()) {
        db.open();
    }
    qDebug()<<'setDb'<<db.lastError();
    ui->clientTagWidget->setDb(db);
    ui->clientTagWidget->setTags(QStringList());
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    QHash<QString, int> hash;
    hash = myDb->getCategoriesHash();
    QHashIterator<QString, int> i(hash);
    while (i.hasNext()) {
      i.next();
      ui->comboProductCat->addItem(i.key(), i.value());
    }
}

void limiteditor::addLimit()

{
    Limit lim;
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    if (ui->radioAllClients->isChecked()) {
        lim.clientId = 0;
        lim.clientTag = QString("*");

    }else if (ui->radioSingleClient->isChecked()) {
        ClientInfo info;
        info = myDb->getClientInfo(ui->editClientCode->text());
        lim.clientId = info.id;


    }else if (ui->radioTagClient->isChecked()) {
        QStringList tags;
        tags = ui->clientTagWidget->getTags();
        if (tags.count()==0) {
            return;
            }

        lim.clientTag = tags.at (0);

    }
    if (ui->radioProductCat->isChecked()){
        int i;
        i = ui->comboProductCat->currentIndex();

        lim.productCat = ui->comboProductCat->itemData(i).toInt();
    }else if (ui->radioProductCode->isChecked()) {

        lim.productCode = ui->editProductCode->text();


    }
    lim.limit = ui->inputLimit->value();

}

limiteditor::~limiteditor()
{
    delete ui;
}

#include "limiteditor.moc"
