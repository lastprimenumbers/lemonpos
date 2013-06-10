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
#ifndef CLIENTEDITOR_H
#define CLIENTEDITOR_H

#include <KDialog>
#include <QtGui>
#include "../../dataAccess/azahar.h"
#include "../../squeeze/src/ui_editclient_widget.h"
#include <QtSql>

class ClientEditorUI : public QFrame, public Ui::clientEditor
{
  Q_OBJECT
  public:
    ClientEditorUI( QWidget *parent=0 );
};

class ClientEditor : public KDialog
{
  Q_OBJECT
  public:
    ClientEditor( QSqlDatabase db, QWidget *parent=0 );
    ~ClientEditor();
    QSqlDatabase db;
    bool hasParent;
    bool hasChild;
    ClientInfo parentClientInfo;
    void setCode(QString code) { ui->editClientCode->setText(code); };
    void setName(QString name) { ui->editClientName->setText(name); };
    void setAddress(QString address) { ui->editClientAddress->setText(address); } ;
    void setPhone(QString phone) { ui->editClientPhone->setText(phone); };
//    void setParentClient(QString code) { ui->editParentClient->setText(code); };
    void setParentClient(QString);
    void setMonthly(double p) { ui->editMonthlyPoints->setText(QString::number(p)); };
    void setPhoto(QPixmap photo) { ui->labelClientPhoto->setPixmap(photo); pix = photo; };
    void setId(long int id) { clientId = id; };
    void setSinceDate(QDate date) { ui->sinceDatePicker->setDate(date); }
    void setExpiryDate(QDate date) { ui->expiryDatePicker->setDate(date); }
//    void setTags(QStringList tags);

    QString getCode(){ return ui->editClientCode->text();};
    QString getName(){ return ui->editClientName->text();};
    QString getParentClient(){ return ui->editParentClient->getCode();};
    QString getAddress(){ return ui->editClientAddress->toPlainText();};
    QString getPhone(){ return ui->editClientPhone->text();};
    double getMonthly() {return ui->editMonthlyPoints->text().toDouble(); }
    QPixmap getPhoto(){ return pix;};
    QDate   getSinceDate() { return ui->sinceDatePicker->date(); }
    QDate   getExpiryDate() { return ui->expiryDatePicker->date(); }
//    QStringList getTags();

    void setClientInfo(ClientInfo info);
    void setClientInfo(QString code);
    ClientInfo getClientInfo();
    void commitClientInfo();
    void loadLimits(ClientInfo info);
    QSqlTableModel *limitsModel;


  private slots:
    void createLimit();
    void changePhoto(bool del=false);
    void openCamera();
    void checkName();
    void checkNameDelayed();
    void updateChildren();
    bool validateParent(QString code);
    void viewParentClient();
    void viewChildClient(int row,int col);

  private:
    ClientEditorUI *ui;
    long int clientId;
    QPixmap pix;
};

#endif
