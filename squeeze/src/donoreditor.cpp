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

#include "donoreditor.h"
#include "../../mibitWidgets/mibitlineedit.h"
#include "../../src/misc.h"
#include "../../dataAccess/azahar.h"

DonorEditorUI::DonorEditorUI( QWidget *parent )
: QFrame( parent )
{
    setupUi( this );
}

DonorEditor::DonorEditor( QSqlDatabase parentDb, QWidget *parent )
: KDialog( parent )
{
    ui = new DonorEditorUI( this );
    setMainWidget( ui );
    setCaption( i18n("Client Editor") );
    setButtons( KDialog::Ok|KDialog::Cancel );
    setDefaultButton(KDialog::NoDefault); //disable default button (return Pressed)
    enableButton(KDialog::Ok, false);

    connect( ui->btnChangeClientPhoto   , SIGNAL( clicked() ), this, SLOT( changePhoto() ) );
    connect( ui->editClientName, SIGNAL(textEdited(const QString &)),this, SLOT( checkNameDelayed()) );
    connect(ui->editClientCode, SIGNAL(returnPressed()),ui->editClientName, SLOT(setFocus()) );
    connect(ui->editClientCode, SIGNAL(editingFinished()),this, SLOT( checkNameDelayed() )); //both returnPressed and lost focus fires this signal. But only fired if validator is accepted.

//    QRegExp regexpC("[0-9]{1,13}");
//    QRegExpValidator * validator = new QRegExpValidator(regexpC, this);
//    ui->editClientCode->setValidator(validator);
    ui->editClientCode->setEmptyMessage(i18n("Enter a 6, 12, or 13 digits Bar Code."));
    ui->editClientName->setEmptyMessage(i18n("Enter client full name"));
    ui->editClientPhone->setEmptyMessage(i18n("Phone number"));
    ui->nameRefDonor->setEmptyMessage("");
    ui->surnameRefDonor->setEmptyMessage("");
    ui->emailDonor->setEmptyMessage("");
    ui->emailRefDonor->setEmptyMessage("");
    ui->phoneRefDonor->setEmptyMessage("");
    ui->notesRefDonor->setEmptyMessage("");

    //since date picker
    ui->sinceDatePicker->setDate(QDate::currentDate());
    
    QTimer::singleShot(750, this, SLOT(checkName()));
    ui->editClientCode->setFocus();
    db=parentDb;
    QStringList hl;
    hl<<tr("Code")<<tr("Name");

    ui->stats->setDb(db);

}

DonorEditor::~DonorEditor()
{
    delete ui;
}

void DonorEditor::changePhoto()
{
  QString fname = KFileDialog::getOpenFileName();
  if (!fname.isEmpty()) {
    QPixmap p = QPixmap(fname);
    setPhoto(p);
  }
}


void DonorEditor::checkNameDelayed()
{
    QTimer::singleShot(750, this, SLOT(checkName()));
}

void DonorEditor::checkName()
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


void DonorEditor::setClientInfo(DonorInfo info)
{
    setCode(info.code);
    setId(info.id);
    setName(info.name);
    setAddress(info.address);
    setPhone(info.phone);
    setSinceDate(info.since);
    setEmail(info.emailDonor);
    setNameRef(info.nameRefDonor);
    setSurnameRef(info.surnameRefDonor);
    setEmailRef(info.emailRefDonor);
    setPhoneRef(info.phoneRefDonor);
    setNotes(info.notesRefDonor);

    QPixmap photo;
    photo.loadFromData(info.photo);
    setPhoto(photo);
    ui->stats->setStats(info.stats);
}
//Overloaded: imposta le informazioni basandosi sul codice!
void DonorEditor::setClientInfo(QString code)
{
    Azahar *myDb=new Azahar;
    myDb->setDatabase(db);
    DonorInfo info=myDb->getDonorInfo(code);
    setClientInfo(info);
}

DonorInfo DonorEditor::getClientInfo()
{
    DonorInfo info;
    info.id       = clientId;
    info.code     = getCode();
    info.name     = getName();
    info.address  = getAddress();
    info.phone    = getPhone();
    info.since    = getSinceDate();
    QPixmap photo=getPhoto();
    info.photo = Misc::pixmap2ByteArray(new QPixmap(photo));
    info.emailDonor = getEmail();
    info.nameRefDonor = getNameRef();
    info.surnameRefDonor = getSurnameRef();
    info.emailRefDonor = getEmailRef();
    info.phoneRefDonor = getPhoneRef();
    info.notesRefDonor = getNotes();

    return info;
}

void DonorEditor::commitClientInfo()
{
    DonorInfo cInfo = getClientInfo();
    qDebug()<<"commitClientInfo: "<<cInfo.id<<cInfo.code;
    if (!db.isOpen()) db.open();
    Azahar *myDb = new Azahar;//
    myDb->setDatabase(db);
    bool result=myDb->updateDonor(cInfo);
    db.commit();
    delete myDb;
}


#include "donoreditor.moc"
