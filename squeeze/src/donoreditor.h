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
#ifndef DONOREDITOR_H
#define DONOREDITOR_H

#include <KDialog>
#include <QtGui>
#include "../../dataAccess/azahar.h"
#include "ui_editdonor_widget.h"
#include <QtSql>

class DonorEditorUI : public QFrame, public Ui::donorEditor
{
  Q_OBJECT
  public:
    DonorEditorUI( QWidget *parent=0 );
};

class DonorEditor : public KDialog
{
  Q_OBJECT
  public:
    DonorEditor( QSqlDatabase db, QWidget *parent=0 );
    ~DonorEditor();
    QSqlDatabase db;
    void setCode(QString code) { ui->editClientCode->setText(code); };
    void setName(QString name) { ui->editClientName->setText(name); };
    void setAddress(QString address) { ui->editClientAddress->setText(address); } ;
    void setPhone(QString phone) { ui->editClientPhone->setText(phone); };
    void setCell(QString cell) { ui->editClientCell->setText(cell); };
    void setPhoto(QPixmap photo) { ui->labelClientPhoto->setPixmap(photo); pix = photo; };
    void setId(long int id) { clientId = id; };
    void setSinceDate(QDate date) { ui->sinceDatePicker->setDate(date); }

    QString getCode(){ return ui->editClientCode->text();};
    QString getName(){ return ui->editClientName->text();};
    QString getAddress(){ return ui->editClientAddress->toPlainText();};
    QString getPhone(){ return ui->editClientPhone->text();};
    QString getCell(){ return ui->editClientCell->text();};
    QPixmap getPhoto(){ return pix;};
    QDate   getSinceDate() { return ui->sinceDatePicker->date(); }

    void setClientInfo(DonorInfo info);
    void setClientInfo(QString code);
    DonorInfo getClientInfo();
    void commitClientInfo();


  private slots:
    void changePhoto();
    void checkName();
    void checkNameDelayed();

  private:
    DonorEditorUI *ui;
    long int clientId;
    QPixmap pix;
};

#endif
