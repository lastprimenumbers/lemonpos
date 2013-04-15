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
    connect( ui->editCode, SIGNAL(returnPressed()), SLOT(validate()));

}

SearchCode::~SearchCode()
{
    delete ui;
}

void SearchCode::setDb(QSqlDatabase parentDb, QString targetTable){
    db=parentDb;
    table=targetTable;
    BasicInfo info;
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    entries = myDb->getBasicHash(table);
    //Set by default the 'general' client.
    QHashIterator<int, BasicInfo> i(entries);
    while (i.hasNext()) {
        i.next();
        info = i.value();
        ui->comboName->addItem(info.name, info.id);
    }
    delete myDb;
}

void SearchCode::setName(QString name) {
    int idx = ui->comboName->findText(name,Qt::MatchCaseSensitive);
    if (idx>-1) {
        ui->comboName->setCurrentIndex(idx);
    } else {
        return;
    }
    return;
}

void SearchCode::setCode(QString name) {
    return;
}

qulonglong SearchCode::getId() {
    return -1;
}

bool validate() {
    // controlla validità name
    // emette selectedName(QString name)
    // selectedCode(QString code)
    // selectedId(qulonglong id)
    return true;
}


#include "searchcode.moc"
