#ifndef CLIENTTAG_H
#define CLIENTTAG_H
#include "clienttag.h"
#include "ui_clienttag.h"
#include "../../dataAccess/azahar.h"
#include <QtGui>


clienttag::clienttag(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::clienttag)
{
//    ui = new Ui::clienttag;
    ui->setupUi(this);
    // Tags
    connect(ui->tagAddButton, SIGNAL(clicked()), SLOT( addTag() ));
    connect(ui->tagRemoveButton, SIGNAL(clicked()), SLOT( removeTag() ));
    connect(ui->tagCreateButton, SIGNAL(clicked()), SLOT( createTag() ));
}

clienttag::~clienttag()
{
    delete ui;
}

void clienttag::setDb(QSqlDatabase parentDb)
{
    db = parentDb;
    if (!db.isOpen()) {
        db.open();
    }
    qDebug()<<'setDb'<<db.lastError();
    setTags(QStringList());
}

void clienttag::setTags(QStringList tags)
{
    for (int i = 0; i<tags.count(); ++i) {
        ui->clientTagsList->addItem(tags.at(i));
    }
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    QStringList avail=myDb->getAvailableTags();
    qDebug() << "Avail" << avail;
    for (int i = 0; i<avail.count(); ++i) {
        if (tags.contains(avail.at(i))) { continue; }
        ui->availableTagsList->addItem(avail.at(i));
    }

}

QStringList clienttag::getTags()
{
    QStringList tags;
    for (int i =0; i < ui->clientTagsList->count(); ++i) {
        tags.append(ui->clientTagsList->item(i)->text());
    }
    return tags;
}

void clienttag::addTag()
{
    QString item=ui->availableTagsList->takeItem(ui->availableTagsList->currentRow())->text();
    ui->clientTagsList->addItem(item);
}

void clienttag::removeTag()
{
    QString item=ui->clientTagsList->takeItem(ui->clientTagsList->currentRow())->text();
    ui->availableTagsList->addItem(item);
}
void clienttag::createTag()
{
    QString item = QInputDialog::getText(this,"Create a new tag","Enter the new tag:");
    if (item.count()==0){
        return;
    }
    ui->clientTagsList->addItem(item);
}

#endif CLIENTTAG_H
