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
#ifndef PURCHASEEDITOR_H
#define PURCHASEEDITOR_H

#include <KDialog>
#include <KDateTime>
#include <QDate>
#include <QtGui>
#include <QPixmap>
#include <QtSql>
#include "../../src/structs.h"
#include "../../src/misc.h"
#include "ui_purchaseeditor.h"

enum rType {estatusNormal=899, estatusMod=999};

class MibitTip;

class PurchaseEditorUI : public QFrame, public Ui::purchaseEditor
{
  Q_OBJECT
  public:
    PurchaseEditorUI( QWidget *parent=0 );
};

class PurchaseEditor : public KDialog
{
  Q_OBJECT
  public:
    PurchaseEditor( QWidget *parent=0);
    ~PurchaseEditor();
    QString getDonor()     { return ui->editDonor->getCode(); };
    QString getNote() { return ui->editNote->toPlainText(); };
    QDate   getDate() { return ui->editDateTime->dateTime().date(); };
    QTime   getTime() { return ui->editDateTime->dateTime().time(); };
    bool   getPurchased() { return ui->chPurchased->isChecked(); };
    double  getPurchaseQty();
    double  getQtyOnDb()     { return qtyOnDb; }
    QPixmap getPhoto()       { return pix; };
    QByteArray getPhotoBA()  { return Misc::pixmap2ByteArray(new QPixmap(pix)); };
    QHash<QString, ProductInfo> getHash()    { return productsHash; };
    double  getTotalBuy()    { return totalBuy; };
    double  getItemCount()   { return itemCount; };
    int loggedUserId;
    void    setDb(QSqlDatabase database, QSqlRelationalTableModel *model, int user);
    void    setPurchaseQty(double q)   {ui->editQty->setText(QString::number(q)); };
    
    void    disableCode()              { ui->p->disableCode(); };
    void    enableCode()               { ui->p->enableCode(); };
    void    resetEdits();
    void    doPurchase();
private slots:
    void    addItemToList();
    void    setupTable();
    void    deleteSelectedItem();
    void    insertProduct(ProductInfo info);
protected slots:
  private:
    PurchaseEditorUI *ui;
    QSqlDatabase db;
    QPixmap pix;
    rType status;
    double qtyOnDb;
    bool productExists;
    double totalBuy;
    double itemCount;
    QString gelem;
    QHash<QString, ProductInfo> productsHash;
    QString lastCode;

    MibitTip *errorPanel;
};

#endif
