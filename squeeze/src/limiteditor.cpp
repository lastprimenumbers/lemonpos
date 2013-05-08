#include "limiteditor.h"
#include "../../dataAccess/azahar.h"
#include "searchcode.h"



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
    ui->codeProduct->setDb(parentDb,"products");
    ui->codeClient->setDb(parentDb,"clients");

}

void limiteditor::addLimit()

{
    Limit lim;
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    // An empty limit
    lim.clientId=0;
    lim.clientTag=QString("*");
    lim.productCat=-1;
    lim.productCode=QString("*");
    lim.parent=-1;
    lim.limit=-1;
    lim.current=0;

// CLIENT SELECTION
    // All clients
    if (ui->radioAllClients->isChecked()) {
        lim.clientId = 0;
        lim.clientTag = QString("*");
    // A single, specific client
    }else if (ui->radioSingleClient->isChecked()) {
        ClientInfo info;
        info = myDb->getClientInfo(ui->codeClient->getCode());
        lim.clientId = info.id;
    // All clients having a tag
    }else if (ui->radioTagClient->isChecked()) {
        QStringList tags;
        tags = ui->clientTagWidget->getTags();
        if (tags.count()==0) {
            return;
            }
    // For the moment keep just the first tag. Should be generalized.
        lim.clientTag = tags.at (0);
    }

// PRODUCT SELECTION
    // A product category
    if (ui->radioProductCat->isChecked()){
        int i;
        i = ui->comboProductCat->currentIndex();
        lim.productCat = ui->comboProductCat->itemData(i).toInt();
    // A single product
    }else if (ui->radioProductCode->isChecked()) {
        lim.productCode = ui->codeProduct->getCode();
    }

    // The limit threshold
    lim.limit = ui->inputLimit->value();
    myDb->insertLimit(lim);
}

limiteditor::~limiteditor()
{
    delete ui;
}

#include "limiteditor.moc"
