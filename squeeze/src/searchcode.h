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
#ifndef SEARCHCODE_H
#define SEARCHCODE_H

#include <KDialog>
#include <QtGui>
#include "../../dataAccess/azahar.h"
#include "ui_searchcode.h"
#include <QtSql>


class SearchCodeUI : public QFrame, public Ui::searchCode
{
    Q_OBJECT
public:
    SearchCodeUI( QWidget *parent=0 );
};

class SearchCode : public QWidget
{
    Q_OBJECT
public:
    SearchCode(  QWidget *parent=0 );
    ~SearchCode();
    QSqlDatabase db;
    QString table;
    qulonglong id;
    QHash<int,BasicInfo> entries;
    bool setCode(QString code);
    bool setName(QString name);
    void setCustomLayout(int direction);
    QString getCode() { return ui->editCode->text(); };
    QString getName() { return ui->comboName->currentText(); };
    bool setId(qulonglong clientId);
    qulonglong getId();
    void setDb(QSqlDatabase db, QString targetTable);

    // Overloaded function calls for filling specific info structs
    bool getInfo(ClientInfo &info);
    bool getInfo(ProductInfo &info);
    bool getInfo(BasicInfo &info);
    bool getInfo(DonorInfo &info);


private:
    SearchCodeUI *ui;

    bool validate();


signals:
    void select();
    void selectCode(QString code);
    void selectName(QString code);

private slots:
    void updatedCode();
    void updatedName(int idx);

};

#endif
