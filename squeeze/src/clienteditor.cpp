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

    QRegExp regexpC("[0-9]{1,13}");
    QRegExpValidator * validator = new QRegExpValidator(regexpC, this);
    ui->editClientPoints->setValidator(validator);
    ui->editClientDiscount->setValidator((new QDoubleValidator(0.00, 100.000, 3,ui->editClientDiscount)));
    ui->editMonthlyPoints->setValidator((new QDoubleValidator(0.00, 1000.000, 3,ui->editMonthlyPoints)));
//    ui->editClientCode->setValidator(validator);

    ui->editClientCode->setEmptyMessage(i18n("Enter a 6, 12, or 13 digits Bar Code."));
    ui->editClientName->setEmptyMessage(i18n("Enter client full name"));
    ui->editClientPhone->setEmptyMessage(i18n("Phone number"));
    ui->editClientCell->setEmptyMessage(i18n("Cell phone number"));
    ui->editClientPoints->setEmptyMessage(i18n("Accumulated points"));
    ui->editClientDiscount->setEmptyMessage(i18n("Personal discount"));

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

    limitsModel=new QSqlTableModel();
    ui->clientTagEditor->setDb(db);
    ui->editParentClient->setDb(db,"clients");
}

ClientEditor::~ClientEditor()
{
    delete ui;
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
}

// Controlla che il codice parent inserito esista. Se esiste, colora la label di verde
// Carica il nome e la foto, imposta hasParent=true
bool ClientEditor::validateParent(QString code)
{
    if (hasChild) {
        updateChildren();
        return false;
    }
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
    ui->editMonthlyPoints->setEnabled(false);
    pal.setColor(QPalette::Text,col);
    ui->editParentClient->setPalette(pal);
    return r;
}
void ClientEditor::updateChildren()
{
    QStringList codes;
    QStringList names;
    QTableWidgetItem codeItem;
    QTableWidgetItem nameItem;
    QString code;
    QString name;
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    myDb->getChildrenClientsList(ui->editClientCode->text(), codes, names);
    delete myDb;
    ui->childrenTable->setRowCount(codes.count());
    hasChild=false;
    if (codes.count()==0) {
        ui->editParentClient->setEnabled(true);
        return;
    }

    for (int i=0 ; i<codes.count(); ++i ) {
        QTableWidgetItem *codeItem = new QTableWidgetItem(codes.at(i));
        QTableWidgetItem *nameItem = new QTableWidgetItem(names.at(i));
        ui->childrenTable->setItem(i,0,codeItem);
        ui->childrenTable->setItem(i,1,nameItem);
    }
    hasChild=true;
    hasParent=false;
    ui->editParentClient->setName("");
    ui->parentClientLabel->setText(tr("Disabled"));
    ui->editParentClient->setEnabled(false);
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


void ClientEditor::loadLimits(ClientInfo info)
{
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    limitsModel->setTable("limits");
    ui->clientLimitsList->setModel(limitsModel);
    ui->clientLimitsList->setColumnHidden(0,true);
    QString f;
    f=QString("clientId=%1").arg(info.id);
    limitsModel->setFilter(f);
    limitsModel->select();
}

void ClientEditor::setClientInfo(ClientInfo info)
{
    setCode(info.code);
    setId(info.id);
    setName(info.name);
    setParentClient(info.parentClient);
    setAddress(info.address);
    setPhone(info.phone);
    setCell(info.cell);
    setPoints(info.points);
    setMonthly(info.monthly);
    setDiscount(info.discount);
    setSinceDate(info.since);
    setExpiryDate(info.expiry);
    QPixmap photo;
    photo.loadFromData(info.photo);
    setPhoto(photo);
    ui->clientTagEditor->setTags(info.tags);
    loadLimits(info);
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
    info.address  = getAddress();
    info.phone    = getPhone();
    info.cell     = getCell();
    info.points   = getPoints();
    info.monthly   = getMonthly();
    info.discount = getDiscount();
    info.since    = getSinceDate();
    info.expiry    = getExpiryDate();
    QPixmap photo=getPhoto();
    info.photo = Misc::pixmap2ByteArray(new QPixmap(photo));
    info.parentClient=getParentClient();
    info.tags=ui->clientTagEditor->getTags();
    return info;
}

void ClientEditor::commitClientInfo()
{
    ClientInfo cInfo = getClientInfo();
    qDebug()<<"commitClientInfo: "<<cInfo.id<<cInfo.code<<cInfo.name<<cInfo.parentClient<<cInfo.monthly;
    if (!db.isOpen()) db.open();
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    bool result=myDb->updateClient(cInfo);
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
