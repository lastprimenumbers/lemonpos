/***************************************************************************
 *   Copyright (C) 2007-2009 by Miguel Chavez Gamboa                       *
 *   miguel.chavez.gamboa@gmail.com                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *

 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/
#include <KLocale>
#include <KMessageBox>
#include <KFileDialog>

#include <QByteArray>

#include "clienteditor.h"
#include "../../mibitWidgets/mibitlineedit.h"
#include "../../src/misc.h"
#include "../../dataAccess/azahar.h"

#include "limiteditor.h"
#include "searchcode.h"

ClientEditorUI::ClientEditorUI( QWidget *parent )
: QFrame( parent )
{
    setupUi( this );
}


ClientEditor::ClientEditor( QSqlDatabase parentDb, QWidget *parent )
: KDialog( parent )
{
    ui = new ClientEditorUI( this );
    hasParent=false;
    inserting=false;
    setMainWidget( ui );
    setCaption( i18n("Client Editor") );
    setButtons( KDialog::Ok|KDialog::Cancel );
    setDefaultButton(KDialog::NoDefault); //disable default button (return Pressed)
    enableButton(KDialog::Ok, false);

    connect( ui->btnChangeClientPhoto   , SIGNAL( clicked() ), this, SLOT( changePhoto() ) );
    connect( ui->btnOpenCamera   , SIGNAL( clicked() ), this, SLOT( openCamera() ) );
    connect( ui->editClientName, SIGNAL(textEdited(const QString &)),this, SLOT( checkNameDelayed()) );
    connect(ui->editClientCode, SIGNAL(returnPressed()),ui->editClientName, SLOT(setFocus()) );
    connect(ui->editClientCode, SIGNAL(editingFinished()),this, SLOT( checkNameDelayed() )); //both returnPressed and lost focus fires this signal. But only fired if validator is accepted.

    // STATS
    connect(ui->statDo,SIGNAL(clicked()),this,SLOT(statistics()));
    ui->statEnd->setDate(QDate::currentDate());

    QRegExp regexpC("[0-9]{1,13}");
    QRegExpValidator * validator = new QRegExpValidator(regexpC, this);
    ui->editMonthlyPoints->setValidator((new QDoubleValidator(0.00, 1000.000, 3,ui->editMonthlyPoints)));
//    ui->editClientCode->setValidator(validator);

    ui->editClientCode->setEmptyMessage(i18n("Enter a 6, 12, or 13 digits Bar Code."));
    ui->editClientName->setEmptyMessage(i18n("Enter client full name"));
    ui->editClientSurname->setEmptyMessage(i18n("Surname"));
    ui->editClientEmail->setEmptyMessage(i18n("email@address"));
    ui->editClientNation->setEmptyMessage("");
    ui->editClientPhone->setEmptyMessage(i18n("Phone number"));

    ui->editParentClient->setCustomLayout(0);

    // Limits
    connect(ui->addLimitButton, SIGNAL(clicked()), SLOT( createLimit() ));

    //since date picker
    ui->sinceDatePicker->setDate(QDate::currentDate());
    
    QTimer::singleShot(750, this, SLOT(checkName()));
    ui->editClientCode->setFocus();
    db=parentDb;
    ui->childrenTable->setColumnCount(2);
    QStringList hl;
    hl<<tr("Code")<<tr("Name");
    ui->childrenTable->setHorizontalHeaderLabels( hl);
    connect(ui->editClientCode, SIGNAL(textChanged(QString)), SLOT(updateChildren()));
    connect(ui->editParentClient, SIGNAL(selectCode(QString)), SLOT(validateParent(QString)));
    connect(ui->viewClientButton, SIGNAL(clicked()), SLOT(viewParentClient()));
    connect(ui->childrenTable, SIGNAL(cellDoubleClicked(int,int)), SLOT(viewChildClient(int,int)));
    connect(ui->changeDebit, SIGNAL(clicked()), SLOT(changeDebit()));
    ui->editClientCode->setInputMask(">XXXXXXXXXXXXXXXXxxx");
    limitsModel=new QSqlTableModel();
    transModel=new QSqlTableModel();
    ticketModel=new QSqlTableModel();
    ui->clientTagEditor->setDb(db);
    ui->editParentClient->setDb(db,"clients");

    connect(ui->transView, SIGNAL(clicked(const QModelIndex &)), SLOT(ticketViewOnSelected(const QModelIndex &)));
}



ClientEditor::~ClientEditor()
{
    delete ui;
}

void ClientEditor::statistics(){
//Update statistical tables
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    myDb->getFamilyStatistics(family,ui->statStart->date(),ui->statEnd->date());
    ui->statTotal->setText(QString::number(family.total));

    // Fill the category table
    QList<int> ck=family.categories.keys();
    ui->categoryTable->setRowCount(ck.count());
    ui->categoryTable->setColumnCount(3);
    QHash<QString,int> cats=myDb->getCategoriesHash();
    for (int i=0; i<ck.count(); i++) {
        ui->categoryTable->setItem(i,0,new QTableWidgetItem(QString::number(ck[i])));
        ui->categoryTable->setItem(i,1,new QTableWidgetItem(cats.key(ck[i])));
        ui->categoryTable->setItem(i,2,new QTableWidgetItem(QString::number(family.categories[ck[i]])));
    }

    // Fill the product table
    QList<QString> pk=family.products.keys();
    qDebug()<<"Mapped products"<<pk.count();
    ui->productTable->setRowCount(pk.count());
    ui->productTable->setColumnCount(3);
    for (int i=0; i<pk.count(); i++) {
        qDebug()<<"Appending product"<<pk[i]<<family.products[pk[i]];
        ui->productTable->setItem(i,0,new QTableWidgetItem(pk[i]));
        ui->productTable->setItem(i,1,new QTableWidgetItem(family.items[pk[i]].name));
        ui->productTable->setItem(i,2,new QTableWidgetItem(QString::number(family.products[pk[i]])));
    }
}


void ClientEditor::changeDebit()
{
    QMessageBox::StandardButton sb=QMessageBox::question(this,"Modificare Credito?","Confermare la modifica del credito",QMessageBox::Ok|QMessageBox::Cancel);
    if (sb==QMessageBox::Cancel) {
        return;
    }
    qDebug()<<"Change debit";
    QString n=ui->newDebit->text();
    double ndf=n.toFloat();
    qDebug()<<"Change debit"<<ndf<<parentClientInfo.id;
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    CreditInfo cr= myDb->queryCreditInfoForClient(parentClientInfo.id);
    cr.total=ndf;
    if (cr.clientId==0) {
        myDb->insertCredit(cr);
    } else {
        myDb->updateCredit(cr);
    }
    delete myDb;
    loadLimits(parentClientInfo);
}

void ClientEditor::ticketViewOnSelected(const QModelIndex & index) {
        //getting data from model...
        const QAbstractItemModel *model = index.model();
        int row = index.row();
        int fid=transModel->fieldIndex("id");
        QModelIndex indx = model->index(row, fid);
        qulonglong tid=model->data(indx, Qt::DisplayRole).toULongLong();
        ticketModel->setFilter(QString("transaction_id=%1").arg(tid));
        ticketModel->select();
}

void ClientEditor::changePhoto(bool del)
{
  QDir img=QDir::home();
  img.cd("Immagini/Webcam");
  QString fname = QFileDialog::getOpenFileName(this,"Seleziona Fotografia",img.path());
  if (!fname.isEmpty()) {
    QPixmap p = QPixmap(fname);
    setPhoto(p);
    if (del==true) {
        QFile file(fname);
        file.remove();
    }
  }
}


void ClientEditor::openCamera()
{
  QProcess cam;
  cam.start("cheese");
  cam.waitForFinished();
  QDir img=QDir::home();
  img.cd("Immagini/Webcam");
  img.setSorting(QDir::Time);
  QStringList imgs =img.entryList();
  QString fname=img.filePath(imgs.at(1));
  qDebug()<<"Camera "<<fname;
  if (!fname.isEmpty()) {
      qDebug()<<"Name OK";
    QPixmap p = QPixmap(fname);
    qDebug()<<"Pixmap";
    setPhoto(p);
    qDebug()<<"Removing"<<fname;
    QFile file(fname);
    file.remove();
  }
}


void ClientEditor::checkNameDelayed()
{
    QTimer::singleShot(750, this, SLOT(checkName()));
}

void ClientEditor::checkName()
{
  if (ui->editClientCode->hasFocus()) {
      enableButtonOk(false);
  }
  else {
      if (ui->editClientName->text().isEmpty())
        enableButtonOk(false);
      else
        enableButtonOk(true);
  }
}

void ClientEditor::setParentClient(QString code)
{

    if (code.count()>0) {
        qDebug()<<"setParentClient "<<code;
        validateParent(code);
        ui->editParentClient->setCode(code);

    }
    else {
        qDebug()<<"Empty parent client"<<code;
    }
    updateChildren();
}

// Controlla che il codice parent inserito esista. Se esiste, colora la label di verde
// Carica il nome e la foto, imposta hasParent=true
bool ClientEditor::validateParent(QString code)
{
    QPixmap photo;
    QPalette pal=this->palette();
    QColor col;
    bool r;
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    ClientInfo info=myDb->getClientInfo(code);
    delete myDb;
    parentClientInfo=info;
    qDebug()<<"validateParent: code , "<<code<<" id:"<<info.id;
    if (info.id == 0) {
        qDebug()<<"Not found!";
        r=false;
        if (code.count()>0) {
            col.setNamedColor("red");
        }
        else {
            col.setNamedColor("black");
        }
    }
    else {
        r=true;
        col.setNamedColor("green");
        photo.loadFromData(info.photo);
    }
    ui->parentClientLabel->setText(info.name);
    ui->parentClientPhoto->setPixmap(photo);
    hasParent=r;

    pal.setColor(QPalette::Text,col);
    ui->editParentClient->setPalette(pal);
    return r;
}
void ClientEditor::updateChildren()
{
    ClientInfo info=getClientInfo();
    if (info.code=="") {
        return;
    }
    info.parentClient=getParentClient();
    qDebug()<<"updateChildren"<<info.code<<info.parentClient;
    // Retrieve from db
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    family=myDb->getFamily(info);
    delete myDb;
    // Populate family table
    ui->childrenTable->setRowCount(family.members.count());
    hasChild=true;
    hasParent=false;
    if (family.members.count()==1) {
        ui->editParentClient->setEnabled(true);
        hasChild=false;
        hasParent=false;
        ui->editClientMsgsusp->setReadOnly(true);
    } else if (family.members.at(0).code==info.code) {
        hasChild=true;
        hasParent=false;
        ui->editParentClient->setName("");
        ui->parentClientLabel->setText(tr("Disabled"));
    } else {
        hasChild=false;
        hasParent=true;
    }
    // Enable or Disable parent client controls
    ui->lastCreditResetPicker->setEnabled(!hasParent);
    ui->nextCreditResetPicker->setEnabled(!hasParent);
    ui->editMonthlyPoints->setEnabled(!hasParent);
    ui->sinceDatePicker->setEnabled(!hasParent);
    ui->expiryDatePicker->setEnabled(!hasParent);
    ui->beginsuspPicker->setEnabled(!hasParent);
    ui->endsuspPicker->setEnabled(!hasParent);
    ui->editClientMsgsusp->setReadOnly(hasParent);

    for (int i=0 ; i<family.members.count(); ++i ) {
        ClientInfo fc=family.members.at(i);
        QTableWidgetItem *codeItem = new QTableWidgetItem(fc.code);
        QTableWidgetItem *nameItem = new QTableWidgetItem(fc.name+" "+fc.surname);
        if (fc.code==info.parentClient || (info.parentClient.count()==0 && i==0)) {
            codeItem->setBackgroundColor(QColor("red"));
            nameItem->setBackgroundColor(QColor("red"));
        }
        ui->childrenTable->setItem(i,0,codeItem);
        ui->childrenTable->setItem(i,1,nameItem);
    }
}

void ClientEditor::viewParentClient()
{
    if (parentClientInfo.id>0) {
    ClientEditor *parentClientDlg = new ClientEditor(db,this);
    parentClientDlg->setClientInfo(parentClientInfo);
    if (parentClientDlg->exec() ) {
        parentClientDlg->commitClientInfo();
    }
    }
}

void ClientEditor::viewChildClient(int row, int col)
{
    QTableWidgetItem *child=ui->childrenTable->item(row,0);
    QString code=child->text();
    ClientEditor *childClientDlg = new ClientEditor(db,this);
    childClientDlg->setClientInfo(code);
    if (childClientDlg->exec() ) {
        childClientDlg->commitClientInfo();
    }

}

void ClientEditor::setLastCreditReset(QDate date) {
    ui->lastCreditResetPicker->setDate(date);
    ui->nextCreditResetPicker->setDate(date.addDays(30));
    parentClientInfo.lastCreditReset=date;
}

void ClientEditor::loadLimits(ClientInfo info)
{
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    limitsModel->setTable("limits");
    ui->clientLimitsList->setModel(limitsModel);
    ui->clientLimitsList->setColumnHidden(0,true);
    limitsModel->setFilter(QString("clientId=%1").arg(info.id));
    limitsModel->select();

    CreditInfo credit=myDb->getCreditInfoForClient(parentClientInfo.id,false);
    ui->editDebit->setText(QString::number(credit.total));
    ui->editCredit->setText(QString::number(parentClientInfo.monthly-credit.total));
    setLastCreditReset(credit.lastCreditReset);
    info.lastCreditReset=credit.lastCreditReset;
    qDebug()<<"loadLimits: Last Credit Reset"<<credit.lastCreditReset;
    delete myDb;

    transModel->setTable("transactions");
    ui->transView->setModel(transModel);
//    ui->transView->setColumnHidden(1,true);

    ui->transView->setColumnHidden(2,true);
    for (int i=6; i<=25; ++i) {
        ui->transView->setColumnHidden(i,true);
    }
    transModel->setFilter(QString("clientid IN (%1)").arg(myDb->getFamilyInStatement(family)));
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

void ClientEditor::setClientInfo(ClientInfo info)
{
    setCode(info.code);
    setId(info.id);
    setName(info.name);
    setSurname(info.surname);
    setEmail(info.email);
    setNation(info.nation);
    setBirthDate(info.birthDate);
    parentClientInfo=info;
    setParentClient(info.parentClient);
    if (parentClientInfo.id>0) {
        loadLimits(parentClientInfo);
    } else {
        loadLimits(info);
    }
    setAddress(info.address);
    setPhone(info.phone);
    setMonthly(info.monthly);
    setSinceDate(info.since);
    setExpiryDate(info.expiry);
    setBeginsuspDate(info.beginsusp);
    setEndsuspDate(info.endsusp);
    setMsgsusp(info.msgsusp);
    setNotes(info.notes);

    QPixmap photo;
    photo.loadFromData(info.photo);
    setPhoto(photo);
    qDebug()<<"clientEditor setting tags"<<info.code<<info.tags;
    ui->clientTagEditor->setTags(info.tags);
    ui->statStart->setDate(info.lastCreditReset);
}
//Overloaded: imposta le informazioni basandosi sul codice!
void ClientEditor::setClientInfo(QString code)
{
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    ClientInfo info=myDb->getClientInfo(code);
    setClientInfo(info);
}

ClientInfo ClientEditor::getClientInfo()
{
    ClientInfo info;
    info.id       = clientId;
    info.code     = getCode();
    info.name     = getName();
    info.surname  = getSurname();
    info.email    = getEmail();
    info.nation   = getNation();
    info.address  = getAddress();
    info.phone    = getPhone();
    info.monthly   = getMonthly();
    info.since    = getSinceDate();
    info.expiry    = getExpiryDate();
    info.beginsusp = getBeginsuspDate();
    info.endsusp = getEndsuspDate();
    info.msgsusp = getMsgsusp();
    info.birthDate = getBirthDate();
    info.notes   = getNotes();
    if (inserting) {
        info.lastCreditReset=info.since;
    } else {
        info.lastCreditReset=getLastCreditReset();
    }
    QPixmap photo=getPhoto();
    info.photo = Misc::pixmap2ByteArray(new QPixmap(photo));
    info.parentClient=getParentClient();
    info.tags=ui->clientTagEditor->getTags();
    //TODO: complete!
    return info;
}

void ClientEditor::commitClientInfo()
{
    ClientInfo cInfo = getClientInfo();
    qDebug()<<"commitClientInfo: "<<cInfo.id<<cInfo.code<<cInfo.name<<cInfo.surname<<cInfo.birthDate<<cInfo.monthly;
    if (!db.isOpen()) db.open();
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    bool result=myDb->updateClient(cInfo);
    qDebug()<<"Client updated:"<<result<<myDb->lastError();
    myDb->setClientTags(cInfo);
    db.commit();
    delete myDb;
}

void ClientEditor::createLimit()
{
    limiteditor *limed = new limiteditor();
    limed->show();
}

#include "clienteditor.moc"
