/**************************************************************************
 *   Copyright © 2007-2011 by Miguel Chavez Gamboa                         *
 *   miguel@lemonpos.org                                                   *
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
#include <kiconloader.h>
#include <kstandarddirs.h>

#include <QByteArray>
#include <QRegExpValidator>

#include "../../dataAccess/azahar.h"
#include "../../mibitWidgets/mibittip.h"
#include "purchaseeditor.h"


PurchaseEditorUI::PurchaseEditorUI( QWidget *parent )
: QFrame( parent )
{
    setupUi( this );
}

PurchaseEditor::PurchaseEditor( QWidget *parent)
: KDialog( parent )
{
    ui = new PurchaseEditorUI( this );
    setMainWidget( ui );
    setCaption( i18n("Purchase") );
    setButtons( KDialog::Ok|KDialog::Cancel );
    setDefaultButton(KDialog::None);

    ui->editDonor->setCustomLayout(1);

    ui->btnAddItem->setDefault(true);

    connect( ui->btnAddItem, SIGNAL( clicked() ), this, SLOT( addItemToList() ) );

    connect(ui->btnRemoveItem, SIGNAL( clicked() ), SLOT( deleteSelectedItem() ) );

    ui->chIsAGroup->setDisabled(true);

    
    QString path = KStandardDirs::locate("appdata", "styles/");
    path = path+"tip.svg";
    errorPanel = new MibitTip(this, ui->widgetPurchase, path, DesktopIcon("dialog-warning",32) );
    

    lastCode = "";
    status = estatusNormal;
    productExists = false;
    productsHash.clear();
    resetEdits();
    totalBuy = 0.0;
    itemCount = 0.0;
    ui->editDateTime->setDateTime(QDateTime::currentDateTime());
    QTimer::singleShot(500, this, SLOT(setupTable()));
}

void PurchaseEditor::setDb(QSqlDatabase database, QSqlRelationalTableModel *model)
{
  db = database;
  if (!db.isOpen()) db.open();
  ui->p->setDb(db);
  ui->editDonor->setDb(db,"donors");

  ui->p->setModel(model);
}

void PurchaseEditor::setupTable() {
  QSize tableSize = ui->tableView->size();
  ui->tableView->horizontalHeader()->resizeSection(0, (tableSize.width()/7));
  ui->tableView->horizontalHeader()->resizeSection(1, (tableSize.width()/7)*3);
  ui->tableView->horizontalHeader()->resizeSection(2,(tableSize.width()/7)*1.4);
  ui->tableView->horizontalHeader()->resizeSection(3,(tableSize.width()/7)*1.4);
}


void PurchaseEditor::addItemToList()
{
    qDebug()<<"DATE:"<<getDate().toString()<<ui->editDateTime->dateTime().date().toString();
    ProductInfo pInfo;
    Azahar *myDb = new Azahar;
    myDb->setDatabase(db);
    bool ok=false;

    if (ui->p->ui->editCode->text().isEmpty()) ui->p->ui->editCode->setFocus();
    else if (ui->p->ui->editDesc->text().isEmpty()) ui->p->ui->editDesc->setFocus();
    else if (ui->p->ui->editFinalPrice->text().isEmpty()) ui->p->ui->editFinalPrice->setFocus();
    else if (ui->editQty->text().isEmpty() || ui->editQty->text()=="0") ui->editQty->setFocus();
    else if ((ui->p->ui->editFinalPrice->text().isEmpty()) || ui->p->ui->editFinalPrice->text().toDouble() < 0.0 ) ui->p->ui->editFinalPrice->setFocus();
    //  else if (ui->p->ui->groupBoxedItem->isChecked() && (ui->p->ui->editItemsPerBox->text().isEmpty() || ui->p->ui->editItemsPerBox->text()=="0"))  ui->p->ui->editItemsPerBox->setFocus();
    //  else if (ui->p->ui->groupBoxedItem->isChecked() && (ui->p->ui->editPricePerBox->text().isEmpty() || ui->p->ui->editPricePerBox->text()=="0")) ui->p->ui->editPricePerBox->setFocus();
    else ok = true;
    // Exit here
    if (!ok) {return;}

    // in-db definition
    ProductInfo info = myDb->getProductInfo(  ui->p->getCode() );
    productExists=(info.code!="0");
    //if is an Unlimited stock product, do not allow to add to the purchase.
    if (info.hasUnlimitedStock)  {
        errorPanel->showTip(i18n("<b>Unlimited Stock Products cannot be purchased.</b>"), 10000);
        return;
    }

    // in-widget definition
    info=ui->p->getProductInfo();

    info.validDiscount = productExists; //used to check if product is already on db.
    if (info.isAGroup) {
        //FIXME!
        // get each product fo the group/pack
        QStringList list = gelem.split(",");
        for (int i=0; i<list.count(); ++i) {
            QStringList tmp = list.at(i).split("/");
            if (tmp.count() == 2) { //ok 2 fields
                qulonglong  code  = tmp.at(0).toULongLong();
                pInfo = myDb->getProductInfo(QString::number(code));
                pInfo.purchaseQty = getPurchaseQty();
                pInfo.validDiscount = true; // all grouped products exists
                insertProduct(pInfo); ///inserting each product of the group
            } // correct fields
        }//for each element
    } else {
        info.purchaseQty=getPurchaseQty();
        insertProduct(info);
    }
    resetEdits();
    ui->p->ui->editCode->setFocus();
}

void PurchaseEditor::insertProduct(ProductInfo info)
{
    //When a product is already on list, increment qty.
    bool existed = false;
    if (info.code=="0") {
        qDebug()<<"INVALID PRODUCT";
        return;
    }
    if (productsHash.contains(info.code)) {
        qDebug()<<"Item "<<info.code<<" already on hash";
        info = productsHash.take(info.code); //re get it from hash
        double finalStock = info.purchaseQty + info.stockqty;
        double suggested;
        if (-info.stockqty == 0)
            suggested = 1;
        else
            suggested = -info.stockqty;
        qDebug()<<"Purchase Qty:"<<info.purchaseQty<<" product stock:"<<info.stockqty<<" final Stock:"<<finalStock;
        if (finalStock < 0) {
            //this cannot be saved, negative stock is not allowed in database.
            qDebug()<<"Cannot be negative stock!, suggested purchase:"<<suggested;
            //launch a message
            errorPanel->showTip(i18n("<b>Stock cannot go negative.</b> The <i>minimum</i> purchase can be <b>%1</b>", suggested), 10000);
            setPurchaseQty(suggested);
            ui->editQty->setFocus();
            return;
        }
        info.purchaseQty += getPurchaseQty();
        itemCount += getPurchaseQty();
        totalBuy = totalBuy + info.cost*getPurchaseQty();
        existed = true;
    } else {
        double finalStock = info.purchaseQty + info.stockqty;
        double suggested;
        if (-info.stockqty == 0)
            suggested = 1;
        else
            suggested = -info.stockqty;
        qDebug()<<"Purchase Qty:"<<info.purchaseQty<<" product stock:"<<info.stockqty<<" final Stock:"<<finalStock;
        if (finalStock < 0) {
            //this cannot be saved, negative stock is not allowed in database.
            qDebug()<<"Cannot be negative stock!, suggested purchase:"<<suggested;
            //launch a message
            errorPanel->showTip(i18n("<b>Stock cannot go negative.</b> The <i>minimum</i> purchase can be <b>%1</b>", suggested), 10000);
            setPurchaseQty(suggested);
            ui->editQty->setFocus();
            return;
        }
        itemCount += info.purchaseQty;
        totalBuy = totalBuy + info.cost*info.purchaseQty;
    }
    //its ok to reset edits now...
    resetEdits();
    
    double finalCount = info.purchaseQty + info.stockqty;
    info.groupElementsStr=""; //grouped products cannot be a group.
    //insert item to productsHash
    productsHash.insert(info.code, info);
    //insert item to ListView
    if (!existed) {
        int rowCount = ui->tableView->rowCount();
        ui->tableView->insertRow(rowCount);
        ui->tableView->setItem(rowCount, 0, new QTableWidgetItem(info.code));
        ui->tableView->setItem(rowCount, 1, new QTableWidgetItem(info.desc));
        ui->tableView->setItem(rowCount, 2, new QTableWidgetItem(QString::number(info.purchaseQty)));
        ui->tableView->setItem(rowCount, 3, new QTableWidgetItem(QString::number(finalCount)));
    } else {
        //simply update the groupView with the new qty
        for (int ri=0; ri<ui->tableView->rowCount(); ++ri)
        {
            QTableWidgetItem * item = ui->tableView->item(ri, 0);
            QString name = item->data(Qt::DisplayRole).toString();
            if (name == info.code) {
                //update
                QTableWidgetItem *itemQ = ui->tableView->item(ri, 2);
                itemQ->setData(Qt::EditRole, QVariant(QString::number(info.purchaseQty)));
                itemQ = ui->tableView->item(ri, 3);
                itemQ->setData(Qt::EditRole, QVariant(QString::number(finalCount)));
                itemQ = ui->tableView->item(ri, 1);
                itemQ->setData(Qt::EditRole, QVariant(info.desc));
                continue; //HEY PURIST, WHEN I GOT SOME TIME I WILL CLEAN IT
            }
        }
    }
    
    ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    ui->tableView->resizeRowsToContents();
    //totalBuy = totalBuy + info.cost*info.purchaseQty;
    //itemCount = itemCount + info.purchaseQty;

    //update info on group caption
    ui->groupBox->setTitle( i18n("Items in this purchase [ %1  items,  %2 ]",itemCount,
                                 KGlobal::locale()->formatMoney(totalBuy, QString(), 2)
                                 ) );
    
    qDebug()<<"totalBuy until now:"<<totalBuy<<" itemCount:"<<itemCount<<"info.cost:"<<info.cost<<"info.purchaseQty:"<<info.purchaseQty;

}

void PurchaseEditor::deleteSelectedItem() //added on dec 3, 2009
{
  if (ui->tableView->currentRow()!=-1) {
    int row = ui->tableView->currentRow();
    QTableWidgetItem *item = ui->tableView->item(row, colCode);
    QString code = item->data(Qt::DisplayRole).toString();
    if (productsHash.contains(code)) {
      //delete it from hash and from view
      ProductInfo info = productsHash.take(code);
      ui->tableView->removeRow(row);
      //update qty and $ of the purchase
      totalBuy  -= (info.cost*info.purchaseQty);
      itemCount -= info.purchaseQty;
      ui->groupBox->setTitle( i18n("Items in this purchase [ %1  items,  %2 ]",itemCount,
                                   KGlobal::locale()->formatMoney(totalBuy, QString(), 2)
                                   ) );
    }
  }
}

void PurchaseEditor::resetEdits()
{
  ui->p->ui->editCode->setText("");
  ui->p->ui->editDesc->setText("");
  ui->p->ui->editFinalPrice->setText("");
  qtyOnDb = 0;
  pix = QPixmap(0,0); //null pixmap.
  ui->p->ui->labelPhoto->setText(i18n("No Photo"));
//  ui->p->ui->editItemsPerBox->setText("");
//  ui->p->ui->editPricePerBox->setText("");
  ui->editQty->setText("");
  ui->p->ui->chIsAGroup->setChecked(false);
  gelem = "";
//  Notify current productsHash to the producteditor
  ui->p->setProductsHash(productsHash);
}

double PurchaseEditor::getPurchaseQty()
{
//  if (ui->p->ui->groupBoxedItem->isChecked()) {
//    if ( ui->p->ui->editItemsPerBox->text().isEmpty() ) return ui->editQty->text().toDouble();
//    else return (ui->editQty->text().toDouble())*(ui->p->ui->editItemsPerBox->text().toDouble());
//  }
//  else return ui->editQty->text().toDouble();
  return ui->editQty->text().toDouble();
}


PurchaseEditor::~PurchaseEditor()
{
    delete ui;
}


#include "purchaseeditor.moc"
