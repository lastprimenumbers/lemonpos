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

#include "searchcode.h"
#include "../../mibitWidgets/mibitlineedit.h"
#include "../../src/misc.h"
#include "../../dataAccess/azahar.h"


SearchCodeUI::SearchCodeUI( QWidget *parent )
: QFrame( parent )
{
    setupUi( this );
}

SearchCode::SearchCode( QWidget *parent )
: QWidget( parent )
{
    ui = new SearchCodeUI( this );
//    setMainWidget( ui );
    //ui.comboName
    connect( ui->editCode, SIGNAL(editingFinished()), SLOT(updatedCode()));
    connect( ui->comboName, SIGNAL(currentIndexChanged(int)), SLOT(updatedName(int)));
    (ui->comboName) ->setEditable(true);
}

SearchCode::~SearchCode()
{
    delete ui;
}

void SearchCode::setCustomLayout(int direction) {
    delete ui->layout();
    if (direction==0) { // Vertical grid
        QGridLayout *lay=new QGridLayout;
        lay->addWidget(ui->lblCode,0,0);
        lay->addWidget(ui->editCode,0,1);
        lay->addWidget(ui->lblName,1,0);
        lay->addWidget(ui->comboName,1,1);
        setLayout(lay);

    } else {    // Horizontal grid
        QHBoxLayout *lay=new QHBoxLayout;
        lay->addWidget(ui->lblCode);
        lay->addWidget(ui->editCode);
        lay->addWidget(ui->lblName);
        lay->addWidget(ui->comboName);
        setLayout(lay);
    }

    return;
}


void SearchCode::setDb(QSqlDatabase parentDb, QString targetTable){
    // supported targetTables: clients, products, donors
    db=parentDb;
    table=targetTable;
    BasicInfo info;
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    entries = myDb->getBasicHash(table);
   // Fill the comboName list
    QHashIterator<int, BasicInfo> i(entries);
    ui->comboName->addItem("", -1);
    while (i.hasNext()) {
        i.next();
        info = i.value();
        ui->comboName->addItem(info.surname+" "+info.name, info.id);
        qDebug()<<"Inserted in combo"<<info.name;
    }
    delete myDb;
}
// External function to set the initial name
bool SearchCode::setName(QString name) {
    int idx = ui->comboName->findText(name,Qt::MatchCaseSensitive);
    if (idx>-1) {
        ui->comboName->setCurrentIndex(idx);
        return true;
    } else {
        return false;
    }
}

// External function to set the initial code
bool SearchCode::setCode(QString name) {
    ui->editCode->setText(name);
    updatedCode();
    return (getId() > 0);

}

// Called when a new code is entered (return pressed)
void SearchCode::updatedCode() {
    qulonglong id;
    BasicInfo info;
    id=getId();
    qDebug()<<"updated code "<<id;
    info=entries[id];
    qDebug()<<"got info "<<info.name<<info.surname<<id;
    setName(info.surname+" "+info.name);
}

// Called when a new name is selected
void SearchCode::updatedName(int idx) {
    blockSignals(true);
    int id;
    id=ui->comboName->itemData(idx).toInt();
    qDebug()<<"updated name "<<id;
    setCode(entries[id].code);
    blockSignals(false);
}


bool SearchCode::setId(qulonglong clientId) {
    BasicInfo info;
    QHashIterator<int, BasicInfo> i(entries);
    while (i.hasNext()) {
        i.next();
        info = i.value();
        if ( info.id==clientId ) {
            setCode(info.code);
            return true;
        }
    }
    return false;
}

qulonglong SearchCode::getId() {
    BasicInfo info;
    QString code;
    code = getCode();
    QHashIterator<int, BasicInfo> i(entries);
    while (i.hasNext()) {
        i.next();
        info = i.value();

        if ( info.code==code ) {
            emit select();
            emit selectCode(code);
            emit selectName(info.surname+" "+info.name);
            return info.id;
        }
    }
    return 0;
}

bool SearchCode::validate(){
    int id;
    id=getId();
    BasicInfo info=entries[id];
    QString cmpname=info.surname+" "+info.name;
    if ( cmpname==getName() and info.code==getCode() ) {
        return true;
    }
    return false;
}

bool SearchCode::getInfo(BasicInfo &info){
    int id;
    id=getId();
    info=entries[id];
    return true;
}

bool SearchCode::getInfo(ClientInfo &info){
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    info = myDb->getClientInfo(getCode());
    delete myDb;
    if (info.id==0) return false;
    return true;
}

bool SearchCode::getInfo(DonorInfo &info){
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    info = myDb->getDonorInfo(getCode());
    delete myDb;
    if (info.id==0) return false;
    return true;
}

bool SearchCode::getInfo(ProductInfo &info){
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    info = myDb->getProductInfo(getCode());
    delete myDb;
    if (info.code=="0") return false;
    return true;
}

#include "searchcode.moc"
