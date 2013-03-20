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

limiteditor::~limiteditor()
{
    delete ui;
}

#include "limiteditor.moc"
