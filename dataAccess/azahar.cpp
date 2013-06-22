/**************************************************************************
 *   Copyright © 2007-2010 by Miguel Chavez Gamboa                         *
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

#include <QWidget>
#include <QByteArray>
#include "azahar.h"
#include <klocale.h>

Azahar::Azahar(QWidget * parent): QObject(parent)
{
  errorStr = "";
  m_mainClient = "undefined";
  clientFields= QString("name, surname, address, phone, email, nation, monthly, photo, since, expiry, birthDate, code, beginsusp, endsusp, msgsusp, notes, parent").split(", ");
  donorFields=QString("name, email, address, phone, photo, since, code, refname, refsurname, refemail, refphone, notes").split(", ");
  limitFields=QString("clientCode, clientTag, productCode, productCat, limit, current, priority, parent").split(", ");
}

Azahar::~Azahar()
{
  //qDebug()<<"*** AZAHAR DESTROYED ***";
}

void Azahar::initDatabase(QString user, QString server, QString password, QString dbname)
{
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
  db = QSqlDatabase::addDatabase("QMYSQL");
  db.setHostName(server);
  db.setDatabaseName(dbname);
  db.setUserName(user);
  db.setPassword(password);

  if (!db.isOpen()) db.open();
  if (!db.isOpen()) db.open();
}

void Azahar::setDatabase(const QSqlDatabase& database)
{
  db = database;
  if (!db.isOpen()) db.open();
}

bool Azahar::isConnected()
{
  return db.isOpen();
}

void Azahar::setError(QString err)
{
  errorStr = err;
}

QString Azahar::lastError()
{
  return errorStr;
}

bool  Azahar::correctStock(QString pcode, double oldStockQty, double newStockQty, const QString &reason )
{ //each correction is an insert to the table.
  bool result1, result2;
  result1 = result2 = false;
  if (!db.isOpen()) db.open();

  //Check if the desired product is a a group. Or hasUnlimitedStock
  ProductInfo p = getProductInfo(pcode);
  if ( p.isAGroup || p.hasUnlimitedStock ) return false;

  QSqlQuery query(db);
  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();
  query.prepare("INSERT INTO stock_corrections (product_id, new_stock_qty, old_stock_qty, reason, date, time) VALUES(:pcode, :newstock, :oldstock, :reason, :date, :time); ");
  query.bindValue(":pcode", pcode);
  query.bindValue(":newstock", newStockQty);
  query.bindValue(":oldstock", oldStockQty);
  query.bindValue(":reason", reason);
  query.bindValue(":date", date.toString("yyyy-MM-dd"));
  query.bindValue(":time", time.toString("hh:mm"));
  if (!query.exec()) setError(query.lastError().text()); else result1=true;

  //modify stock
  QSqlQuery query2(db);
  query2.prepare("UPDATE products set stockqty=:newstock where code=:pcode;");
  query2.bindValue(":pcode", pcode);
  query2.bindValue(":newstock", newStockQty);
  if (!query2.exec()) setError(query2.lastError().text()); else result2=true;
  return (result1 && result2);
}

double Azahar::getProductStockQty(QString code)
{
  double result=0.0;
  if (db.isOpen()) {
    QString qry = QString("SELECT stockqty from products WHERE code=%1").arg(code);
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
    }
    if (query.size() <= 0)
      setError(i18n("Error serching product id %1, Rows affected: %2", code,query.size()));
    else {
      while (query.next()) {
        int fieldStock = query.record().indexOf("stockqty");
        int fieldIsUnlimited = query.record().indexOf("hasUnlimitedStock");
        result = query.value(fieldStock).toDouble(); //return stock
        if ( query.value(fieldIsUnlimited).toBool() )
            result = 999999; //just make sure we do not return 0 for unlimited stock items.
      }
    }
  }
  return result;
}

qulonglong Azahar::getProductOfferCode(QString code)
{
  qulonglong result=0;
  if (db.isOpen()) {
    QString qry = QString("SELECT id,product_id from offers WHERE product_id=%1").arg(code);
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
    }
    if (query.size() <= 0)
      setError(i18n("Error serching offer id %1, Rows affected: %2", code,query.size()));
    else {
      while (query.next()) {
        int fieldId = query.record().indexOf("id");
        result = query.value(fieldId).toULongLong(); //return offer id
      }
    }
  }
  return result;
}


ProductInfo Azahar::getProductInfo(const QString &code, const bool &notConsiderDiscounts)
{
  ProductInfo info;
  info.code="0";
  info.desc="Ninguno";
  info.price=0;
  info.disc=0;
  info.cost=0;
  info.lastProviderId = 0;
  info.category=0;
  info.taxmodelid=0; //for future use! MCH DEC 28 2010
  info.units=0;
  info.points=0;
  info.row=-1;info.qtyOnList=-1;info.purchaseQty=-1;
  info.discpercentage=0;
  info.validDiscount=false;
  info.alphaCode = "-NA-";
  info.lastProviderId = 0;
  info.isAGroup = false;
  info.isARawProduct = false;
  info.groupPriceDrop = 0;
  info.hasUnlimitedStock = false;
  info.isNotDiscountable = false;
  QString rawCondition;

  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QString qry = QString("SELECT  P.code as CODE, \
    P.alphacode as ALPHACODE, \
    P.name as NAME ,\
    P.price as PRICE, \
    P.cost as COST ,\
    P.stockqty as STOCKQTY, \
    P.units as UNITS, \
    P.points as POINTS, \
    P.photo as PHOTO, \
    P.category as CATID, \
    P.lastproviderid as PROVIDERID, \
    P.taxpercentage as TAX1, \
    P.extrataxes as TAX2, \
    P.taxmodel as TAXMODELID, \
    P.isARawProduct as ISRAW, \
    P.isAGroup as ISGROUP, \
    P.groupElements as GE, \
    P.groupPriceDrop as GPRICEDROP,\
    P.soldunits as SOLDUNITS, \
    P.isNotDiscountable as NONDISCOUNT, \
    P.hasUnlimitedStock as UNLIMITEDSTOCK, \
    U.text as UNITSDESC, \
    C.text as CATEGORY, \
    T.tname as TAXNAME, \
    T.elementsid as TAXELEM \
    FROM products AS P, taxmodels as T, categories as C, measures as U \
    WHERE T.modelid=P.taxmodel \
    AND C.catid=P.category AND U.id=P.units\
    AND (CODE='%1' or ALPHACODE='%1');").arg(code);
    
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
      setError(i18n("Error getting product information for code %1, Rows affected: %2", code,query.size()));
    }
    else {
      while (query.next()) {
        int fieldTax1= query.record().indexOf("TAX1");
        int fieldTax2= query.record().indexOf("TAX2");
        int fieldGroupPD = query.record().indexOf("GPRICEDROP");
        int fieldCODE = query.record().indexOf("CODE");
        int fieldDesc = query.record().indexOf("NAME");
        int fieldPrice= query.record().indexOf("PRICE");
        int fieldPhoto= query.record().indexOf("PHOTO");
        int fieldCost= query.record().indexOf("COST");
        int fieldUnits= query.record().indexOf("UNITS");
        int fieldUnitsDESC= query.record().indexOf("UNITSDESC");
        //int fieldTaxName= query.record().indexOf("TAXNAME");
        int fieldTaxModelId= query.record().indexOf("TAXMODELID");
        //int fieldCategoryName= query.record().indexOf("CATEGORY");
        int fieldCategoryId= query.record().indexOf("CATID");
        int fieldPoints= query.record().indexOf("POINTS");
        //int fieldLastProviderName = query.record().indexOf("LASTPROVIDER");
        int fieldLastProviderId = query.record().indexOf("PROVIDERID");
        int fieldAlphaCode = query.record().indexOf("ALPHACODE");
        int fieldTaxElem = query.record().indexOf("TAXELEM");
        int fieldStock= query.record().indexOf("STOCKQTY");
        int fieldIsGroup = query.record().indexOf("ISGROUP");
        int fieldIsRaw = query.record().indexOf("ISRAW");
        int fieldGE = query.record().indexOf("GE");
        int fieldSoldU = query.record().indexOf("SOLDUNITS");
        int fieldUnlimited = query.record().indexOf("UNLIMITEDSTOCK");
        int fieldNonDiscount = query.record().indexOf("NONDISCOUNT");

        info.code     = query.value(fieldCODE).toString();
        info.alphaCode = query.value(fieldAlphaCode).toString();
        info.desc     = query.value(fieldDesc).toString();
        info.price    = query.value(fieldPrice).toDouble();
        info.photo    = query.value(fieldPhoto).toByteArray();
        info.stockqty = query.value(fieldStock).toDouble();
        info.cost     = query.value(fieldCost).toDouble();
        info.tax      = query.value(fieldTax1).toDouble(); ///TO be removed later, when taxmodel is coded.
        info.extratax = query.value(fieldTax2).toDouble(); ///TO be removed later, when taxmodel is coded.
        info.units    = query.value(fieldUnits).toInt();
        info.unitStr  = query.value(fieldUnitsDESC).toString();
        info.category = query.value(fieldCategoryId).toInt();
        info.utility  = info.price - info.cost;
        info.row      = -1;
        info.points   = query.value(fieldPoints).toInt();
        info.qtyOnList = -1;
        info.purchaseQty = -1;
        info.lastProviderId = query.value(fieldLastProviderId).toULongLong();
        info.soldUnits = query.value(fieldSoldU).toDouble();
        info.isARawProduct = query.value(fieldIsRaw).toBool();
        info.isAGroup = query.value(fieldIsGroup).toBool();
        info.groupPriceDrop = query.value(fieldGroupPD).toDouble();
        info.taxmodelid = query.value(fieldTaxModelId).toULongLong();
        info.taxElements = query.value(fieldTaxElem).toString();
        info.groupElementsStr = query.value(fieldGE).toString();
        info.hasUnlimitedStock = query.value(fieldUnlimited).toBool();
        info.isNotDiscountable = query.value(fieldNonDiscount).toBool();
      }

      if ( info.hasUnlimitedStock )
            info.stockqty = 999999; //just make sure we do not return 0 for unlimited stock items.
      
      /** @TODO: for future releases where taxmodel is included in code
      //get missing stuff - tax,offers for the requested product
      if (info.isAGroup) //If its a group, the taxmodel is ignored, the tax will be its elements taxes
        info.tax = getGroupAverageTax(info.code); ///NOTE: This may be deprecated.. see the getGroupPriceAndTax() method
      else
          info.tax = getTotalTaxPercent(info.taxElements);
      if (getConfigTaxIsIncludedInPrice()) {
          ///tax is included in price... mexico style.
          double pWOtax = info.price/(1+((info.tax)/100));
          info.totaltax = pWOtax*((info.tax)/100); // in money...
      } else {
          ///tax is not included in price... usa style.
          info.totaltax = info.price*(1+(info.tax/100)); //tax in money
      }
      **/
      
      ///NOTE FOR DISCOUNTS:  TODO: ADD THIS TO THE USER MANUAL
      //     If a group contains product with discounts THOSE ARE NOT TAKEN INTO CONSIDERATION,
      //     The only DISCOUNT for a GROUP is the DISCOUNT created for the GROUP PRODUCT -not for its contents-.
      //     The discounts for GROUPS are PERPETUAL.
      //     Another way to assign a "discount" for a group is reducing the price in the product editor
      //     this is not considered a discount but a lower price, and both can coexist so
      ///    be CAREFUL with this!
      
     //get discount info... if have one.
     QSqlQuery query2(db);
     if ( !info.isNotDiscountable ) { //get discounts if !isNotDiscountable...
        if (query2.exec(QString("Select * from offers where product_id=%1").arg(info.code) )) {
        QList<double> descuentos; descuentos.clear();
        while (query2.next()) // return the valid discount only (and the greater one if many).
            {
            int fieldDisc = query2.record().indexOf("discount");
            int fieldDateStart = query2.record().indexOf("datestart");
            int fieldDateEnd   = query2.record().indexOf("dateend");
            double descP= query2.value(fieldDisc).toDouble();
            QDate dateStart = query2.value(fieldDateStart).toDate();
            QDate dateEnd   = query2.value(fieldDateEnd).toDate();
            QDate now = QDate::currentDate();
            //See if the offer is in a valid range...
            if (info.isAGroup) { /// if produc is a group, the discount has NO DATE LIMITS!!! its valid forever.
                descuentos.append(descP);
            }
            else if ((dateStart<dateEnd) && (dateStart<=now) && (dateEnd>=now)  ) {
                //save all discounts here and decide later to return the bigger valid discount.
                descuentos.append(descP);
            }
            }
            //now which is the bigger valid discount?
            qSort(descuentos.begin(), descuentos.end(), qGreater<int>());
            //qDebug()<<"DESCUENTOS ORDENADOS DE MAYOR A MENOR:"<<descuentos;
            if (!descuentos.isEmpty()) {
            //get the first item, which is the greater one.
            info.validDiscount = true;
            info.discpercentage = descuentos.first();
            info.disc =       (info.discpercentage/100) * (info.price); //round((info.discpercentage/100) * (info.price*100))/100; //FIXME:This is not necesary VALID.
            } else {info.disc = 0; info.validDiscount =false;}
        }
     } else {info.disc = 0; info.validDiscount = false;} // if !nondiscountable
     /// If its a group, calculate the right price first.  @note: this will be removed when taxmodels are coded.
     double priceDrop = 0;
     if (info.isAGroup) {
      //get each content price and tax percentage.
      GroupInfo gInfo = getGroupPriceAndTax(info);
      info.price = gInfo.price; //it includes price drop!
      info.tax   = gInfo.tax;
      info.extratax = 0; //accumulated in tax...
      priceDrop = gInfo.priceDrop;
      //qDebug()<<"=================== GROUP Price:"<<info.price<<" Tax:"<<info.tax<<"======================";
     }
     ///tax calculation - it depends on discounts... @note: this will be removed when taxmodels are coded.
     double pWOtax = 0;
     if (getConfigTaxIsIncludedInPrice()) //added on jan 28 2010. 
       pWOtax= info.price/(1+((info.tax+info.extratax)/100));
     else
       pWOtax = info.price;
     //take into account the discount.
     if (info.validDiscount) {
       double iDisc=0;
       iDisc = (notConsiderDiscounts) ? 0 : (info.discpercentage/100)*pWOtax;
       //if (notConsiderDiscounts) qDebug()<<" ================= WARNING: NOT CONSIDERING DISCOUNT FOR TAX CALCULATION! ======================= ";
       pWOtax = pWOtax - iDisc;
     }
     //finally we have on pWOtax the price without tax and discount for 1 item
     ///NOTE: Aug 17 2011: this tax does not take into account the ocassional discounts or price changes. It may be false.
     double tax1m = (info.tax/100)*pWOtax;
     double tax2m = (info.extratax/100)*pWOtax;
     info.totaltax = tax1m + tax2m;
     //qDebug()<<"  getProductInfo() :: Total tax for product "<<info.desc<<info.tax+info.extratax<< " $:"<<info.totaltax;
     ///end of tax calculation
    }
  }
  return info;
}

// QList<ProductInfo> Azahar::getTransactionGroupsList(qulonglong tid)
// {
//   QList<ProductInfo> list;
//   QStringList groupsList = getTransactionInfo(tid).groups.split(",");
//   foreach(QString ea, groupsList) {
//     qulonglong c = ea.section('/',0,0).toULongLong();
//     double     q = ea.section('/',1,1).toDouble();
//     ProductInfo pi = getProductInfo(c);
//     pi.qtyOnList = q;
//     list.append(pi);
//   }
//   return list;
// }

QString Azahar::getProductCode(QString text)
{
  QSqlQuery query(db);
  QString code=0;
  if (query.exec(QString("select code from products where name='%1';").arg(text))) {
    while (query.next()) { 
      int fieldCode  = query.record().indexOf("code");
      code = query.value(fieldCode).toString();
    }
  }
  else {
    //qDebug()<<"ERROR: "<<query.lastError();
    setError(query.lastError().text());
  }
  return code;
}

QString Azahar::getProductCodeFromAlphacode(QString text)
{
    QSqlQuery query(db);
    QString code=0;
    if (query.exec(QString("select code from products where alphacode='%1';").arg(text))) {
        while (query.next()) {
            int fieldCode   = query.record().indexOf("code");
            code = query.value(fieldCode).toString();
        }
    }
    else {
        //qDebug()<<"ERROR: "<<query.lastError();
        setError(query.lastError().text());
    }
    return code;
}

/// UPDATED: This searches products by description and alphacode.   -Dec 28 2011-
QList<QString> Azahar::getProductsCode(QString regExpName)
{
  QList<QString> result;
  result.clear();
  QSqlQuery query(db);
  QString qry;
  if (regExpName == "*") qry = "SELECT code from products;";
  else qry = QString("select code,name,alphacode from products WHERE `name` REGEXP '%1' OR  `alphacode` REGEXP '%1'").arg(regExpName);
  if (query.exec(qry)) {
    while (query.next()) {
      int fieldCode   = query.record().indexOf("code");
      QString id = query.value(fieldCode).toString();
      result.append(id);
//       qDebug()<<"APPENDING TO PRODUCTS LIST:"<<id;
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}

QStringList Azahar::getProductsList()
{
  QStringList result; result.clear();
  QSqlQuery query(db);
  if (query.exec("select name from products;")) {
    while (query.next()) {
      int fieldText = query.record().indexOf("name");
      QString text = query.value(fieldText).toString();
      result.append(text);
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}


bool Azahar::insertProduct(ProductInfo info)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);

  //some buggy info can cause troubles.
  bool groupValueOK = false;
  bool rawValueOK = false;
  if (info.isAGroup == 0 || info.isAGroup == 1) groupValueOK=true;
  if (info.isARawProduct == 0 || info.isARawProduct == 1) rawValueOK=true;
  if (!groupValueOK) info.isAGroup = 0;
  if (!rawValueOK) info.isARawProduct = 0;

  info.taxmodelid = 1; ///FIXME: Delete this code when taxmodels are added, for now, insert default one.

  if (info.hasUnlimitedStock)
      info.stockqty = 1; //for not showing "Not Available" in the product delegate.
  
  query.prepare("INSERT INTO products (code, name, price, stockqty, cost, soldunits, datelastsold, units, taxpercentage, extrataxes, photo, category, points, alphacode, lastproviderid, isARawProduct,isAGroup, groupElements, groupPriceDrop, taxmodel, hasUnlimitedStock, isNotDiscountable ) VALUES (:code, :name, :price, :stock, :cost, :soldunits, :lastgetld, :units, :tax1, :tax2, :photo, :category, :points, :alphacode, :lastproviderid, :isARaw, :isAGroup, :groupE, :groupPriceDrop, :taxmodel, :unlimitedStock, :NonDiscountable);");
  query.bindValue(":code", info.code);
  query.bindValue(":name", info.desc);
  query.bindValue(":price", info.price);
  query.bindValue(":stock", info.stockqty);
  query.bindValue(":cost", info.cost);
  query.bindValue(":soldunits", 0);
  query.bindValue(":lastsold", "0000-00-00");
  query.bindValue(":units", info.units);
  query.bindValue(":tax1", info.tax);
  query.bindValue(":tax2", info.extratax);
  query.bindValue(":photo", info.photo);
  query.bindValue(":category", info.category);
  query.bindValue(":points", info.points);
  query.bindValue(":alphacode", info.alphaCode);
  query.bindValue(":lastproviderid", info.lastProviderId);
  query.bindValue(":isAGroup", info.isAGroup);
  query.bindValue(":isARaw", info.isARawProduct);
  query.bindValue(":groupE", info.groupElementsStr);
  query.bindValue(":groupPriceDrop", info.groupPriceDrop);
  query.bindValue(":taxmodel", info.taxmodelid); //for later use
  query.bindValue(":unlimitedStock", info.hasUnlimitedStock);
  query.bindValue(":NonDiscountable", info.isNotDiscountable);

  if (!query.exec()) setError(query.lastError().text()); else result=true;

  /** @note & TODO: Document this for the user.
   *                If the new product's code is reused, and a discount exists in the offers table, it will be deleted.
   **/
  
  QSqlQuery queryX(db);
  if (queryX.exec(QString("Select id from offers where product_id=%1").arg(info.code) )) {
    while (queryX.next()) {
      int fieldId    = queryX.record().indexOf("id");
      qulonglong oId = queryX.value(fieldId).toULongLong();
      deleteOffer(oId);
      qDebug()<<" **WARNING** Deleting Existing Offer for the new Product";
    }
  }
  
  return result;
}


bool Azahar::updateProduct(ProductInfo info, QString oldcode)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  
  //some buggy info can cause troubles.
  bool groupValueOK = false;
  bool rawValueOK = false;
  if (info.isAGroup == 0 || info.isAGroup == 1) groupValueOK=true;
  if (info.isARawProduct == 0 || info.isARawProduct == 1) rawValueOK=true;
  if (!groupValueOK) info.isAGroup = 0;
  if (!rawValueOK) info.isARawProduct = 0;

  /// TODO FIXME: Remove the next line when taxmodels are implemented
  info.taxmodelid = 1;

  if (info.hasUnlimitedStock)
      info.stockqty = 1; //for not showing "Not Available" in the product delegate.
  
  //query.prepare("UPDATE products SET code=:newcode, photo=:photo, name=:name, price=:price, stockqty=:stock, cost=:cost, units=:measure, taxpercentage=:tax1, extrataxes=:tax2, category=:category, points=:points, alphacode=:alphacode, lastproviderid=:lastproviderid , isARawProduct=:isRaw, isAGroup=:isGroup, groupElements=:ge, groupPriceDrop=:groupPriceDrop WHERE code=:id");
  ///TODO: remove the taxpercentage and extrataxes when taxmodel is implemented
  query.prepare("UPDATE products SET \
  code=:newcode, \
  name=:name, \
  price=:price, \
  cost=:cost, \
  stockqty=:stock, \
  units=:measure, \
  taxpercentage=:tax1,\
  extrataxes=:tax2,\
  taxmodel=:taxmodel, \
  photo=:photo, \
  category=:category, \
  points=:points, \
  alphacode=:alphacode, \
  lastproviderid=:lastproviderid, \
  isARawProduct=:isRaw, \
  isAGroup=:isGroup, \
  groupElements=:ge, \
  groupPriceDrop=:groupPriceDrop, \
  isNotDiscountable=:NonDiscountable, \
  hasUnlimitedStock=:unlimitedStock \
  WHERE code=:id;");
  
  query.bindValue(":newcode", info.code);
  query.bindValue(":name", info.desc);
  query.bindValue(":price", info.price);
  query.bindValue(":stock", info.stockqty);
  query.bindValue(":cost", info.cost);
  query.bindValue(":measure", info.units);
  query.bindValue(":tax1", info.tax);
  query.bindValue(":tax2", info.extratax);
  query.bindValue(":photo", info.photo);
  query.bindValue(":category", info.category);
  query.bindValue(":points", info.points);
  query.bindValue(":id", oldcode);
  query.bindValue(":alphacode", info.alphaCode);
  query.bindValue(":lastproviderid", info.lastProviderId);
  query.bindValue(":isGroup", info.isAGroup);
  query.bindValue(":isRaw", info.isARawProduct);
  query.bindValue(":ge", info.groupElementsStr);
  query.bindValue(":groupPriceDrop", info.groupPriceDrop);
  query.bindValue(":taxmodel", info.taxmodelid);
  query.bindValue(":unlimitedStock", info.hasUnlimitedStock);
  query.bindValue(":NonDiscountable", info.isNotDiscountable);

  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}

bool Azahar::decrementProductStock(QString code, double qty, QDate date)
{
  bool result = false;
  double qtys=qty;
  double nqty = 0;
  if (!db.isOpen()) db.open();

  ProductInfo p = getProductInfo( code );
  if ( p.hasUnlimitedStock )
      nqty = 0; //not let unlimitedStock products to be decremented.
  else
      nqty = qty;
  // nqty is the qty to ADD or DEC, qtys is the qty to ADD_TO_SOLD_UNITS only.
   
  QSqlQuery query(db);
  query.prepare("UPDATE products SET datelastsold=:dateSold , stockqty=stockqty-:qty , soldunits=soldunits+:qtys WHERE code=:id");
  query.bindValue(":id", code);
  query.bindValue(":qty", nqty);
  query.bindValue(":qtys", qtys);
  query.bindValue(":dateSold", date.toString("yyyy-MM-dd"));
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  //qDebug()<<"DECREMENT Product Stock  -- Rows Affected:"<<query.numRowsAffected();
  return result;
}

bool Azahar::decrementGroupStock(QString code, double qty, QDate date)
{
  bool result = true;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);

  ProductInfo info = getProductInfo(code);
  QStringList lelem = info.groupElementsStr.split(",");
  foreach(QString ea, lelem) {
    QString c = ea.section('/',0,0);
    double     q = ea.section('/',1,1).toDouble();
    //ProductInfo pi = getProductInfo(QString::number(c));
    //FOR EACH ELEMENT, DECREMENT PRODUCT STOCK
    result = result && decrementProductStock(c, q*qty, date);
  }
  
  return result;
}

/// WARNING: This method is DECREMENTING soldunits... not used in lemonview.cpp nor squeezeview.cpp !!!!!
///          Do not use when doing PURCHASES in squeeze!
bool Azahar::incrementProductStock(QString code, double qty)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  double qtys=qty;
  double nqty=0;

  ProductInfo p = getProductInfo( code );
  if ( p.hasUnlimitedStock )
      nqty = 0; //not let unlimitedStock products to be decremented.
  else
      nqty = qty;
  // nqty is the qty to ADD or DEC, qtys is the qty to ADD_TO_SOLD_UNITS only.

  query.prepare("UPDATE products SET stockqty=stockqty+:qty , soldunits=soldunits-:qtys WHERE code=:id");
  query.bindValue(":id", code);
  query.bindValue(":qty", nqty);
  query.bindValue(":qtys", qtys);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  //qDebug()<<"Increment Stock - Rows:"<<query.numRowsAffected();
  return result;
}

/// WARNING: This method is DECREMENTING soldunits... not used in lemonview.cpp nor squeezeview.cpp !!!!!
///          Do not use when doing PURCHASES in squeeze!
bool Azahar::incrementGroupStock(QString code, double qty)
{
  bool result = true;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  
  ProductInfo info = getProductInfo(code);
  QStringList lelem = info.groupElementsStr.split(",");
  foreach(QString ea, lelem) {
    QString c = ea.section('/',0,0);
    double     q = ea.section('/',1,1).toDouble();
    //ProductInfo pi = getProductInfo(c);
    //FOR EACH ELEMENT, DECREMENT PRODUCT STOCK
    result = result && incrementProductStock(c, q*qty);
  }
  
  return result;
}


bool Azahar::deleteProduct(QString code)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query = QString("DELETE FROM products WHERE code=%1").arg(code);
  if (!query.exec()) setError(query.lastError().text()); else result=true;

  /** @note & TODO: Document this for the user.
   *                If a discount exists in the offers table for the deleted product, the offer will be deleted also.
   **/
  
  QSqlQuery queryX(db);
  if (queryX.exec(QString("Select id from offers where product_id=%1").arg(code) )) {
    while (queryX.next()) {
      int fieldId    = queryX.record().indexOf("id");
      qulonglong oId = queryX.value(fieldId).toULongLong();
      deleteOffer(oId);
      qDebug()<<" **NOTE** Deleting Existing Offer for the deleted Product";
    }
  }
  
  return result;
}

double Azahar::getProductDiscount(QString code, bool isGroup)
{
  double result = 0.0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query2(db);
    
    ProductInfo p = getProductInfo( code );
    if ( p.isNotDiscountable )
        return 0.0;
            
    QString qry = QString("SELECT * FROM offers WHERE product_id=%1").arg(code);
    if (!query2.exec(qry)) { setError(query2.lastError().text()); }
    else {
      QList<double> descuentos; descuentos.clear();
      while (query2.next())
      {
        int fieldDisc = query2.record().indexOf("discount");
        int fieldDateStart = query2.record().indexOf("datestart");
        int fieldDateEnd   = query2.record().indexOf("dateend");
        double descP= query2.value(fieldDisc).toDouble();
        QDate dateStart = query2.value(fieldDateStart).toDate();
        QDate dateEnd   = query2.value(fieldDateEnd).toDate();
        QDate now = QDate::currentDate();
        //See if the offer is in a valid range...
        if (isGroup) descuentos.append(descP);
        else if ((dateStart<dateEnd) && (dateStart<=now) && (dateEnd>=now)  ) {
          //save all discounts here and decide later to return the bigger valid discount.
          descuentos.append(descP);
        }
      }
      //now which is the bigger valid discount?
      qSort(descuentos.begin(), descuentos.end(), qGreater<int>());
      if (!descuentos.isEmpty()) {
        //get the first item, which is the greater one.
        result = descuentos.first(); ///returns the discount percentage!
      } else result = 0;
    }
  } else { setError(db.lastError().text()); }
  return result;
}

QList<pieProdInfo> Azahar::getTop5SoldProducts()
{
  QList<pieProdInfo> products; products.clear();
  pieProdInfo info;
  QSqlQuery query(db);
  if (query.exec("SELECT name,soldunits,units,code FROM products WHERE (soldunits>0 AND isARawProduct=false) ORDER BY soldunits DESC LIMIT 5")) {
    while (query.next()) {
      int fieldName  = query.record().indexOf("name");
      int fieldUnits = query.record().indexOf("units");
      int fieldSoldU = query.record().indexOf("soldunits");
      int fieldCode  = query.record().indexOf("code");
      int unit       = query.value(fieldUnits).toInt();
      info.name    = query.value(fieldName).toString();
      info.count   = query.value(fieldSoldU).toDouble();
      info.unitStr = getMeasureStr(unit);
      info.code    = query.value(fieldCode).toString();
      products.append(info);
    }
  }
  else {
    setError(query.lastError().text());
  }
  return products;
}

double Azahar::getTopFiveMaximum()
{
  double result = 0;
  QSqlQuery query(db);
  if (query.exec("SELECT soldunits FROM products WHERE (soldunits>0 AND isARawProduct=false) ORDER BY soldunits DESC LIMIT 5")) {
    if (query.first()) {
      int fieldSoldU = query.record().indexOf("soldunits");
      result   = query.value(fieldSoldU).toDouble();
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}

double Azahar::getAlmostSoldOutMaximum(int max)
{
double result = 0;
  QSqlQuery query(db);
  if (query.exec(QString("SELECT stockqty FROM products WHERE (isARawProduct=false  AND isAGroup=false AND stockqty<=%1) ORDER BY stockqty DESC LIMIT 5").arg(max))) {
    if (query.first()) {
      int fieldSoldU = query.record().indexOf("stockqty");
      result   = query.value(fieldSoldU).toDouble();
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}

QList<pieProdInfo> Azahar::getAlmostSoldOutProducts(int min, int max)
{
  QList<pieProdInfo> products; products.clear();
  pieProdInfo info;
  QSqlQuery query(db);
  //NOTE: Check lower limit for the soldunits range...
  query.prepare("SELECT name,stockqty,units,code FROM products WHERE (isARawProduct=false AND isAGroup=false AND stockqty<=:maxV) ORDER BY stockqty ASC LIMIT 5");
  query.bindValue(":maxV", max);
//   query.bindValue(":minV", min);
  if (query.exec()) {
    while (query.next()) {
      int fieldName  = query.record().indexOf("name");
      int fieldUnits = query.record().indexOf("units");
      int fieldStock = query.record().indexOf("stockqty");
      int fieldCode  = query.record().indexOf("code");
      int unit       = query.value(fieldUnits).toInt();
      info.name    = query.value(fieldName).toString();
      info.count   = query.value(fieldStock).toDouble();
      info.code    = query.value(fieldCode).toString();
      info.unitStr = getMeasureStr(unit);
      products.append(info);
    }
  }
  else {
    setError(query.lastError().text());
    qDebug()<<lastError();
  }
  return products;
}

//returns soldout products only if the product is NOT a group.
QList<ProductInfo> Azahar::getSoldOutProducts()
{
  QList<ProductInfo> products; products.clear();
  ProductInfo info;
  QSqlQuery query(db);
  query.prepare("SELECT code FROM products WHERE stockqty=0 and isAgroup=false ORDER BY code ASC;");
  if (query.exec()) {
    while (query.next()) {
      int fieldCode  = query.record().indexOf("code");
      info = getProductInfo(query.value(fieldCode).toString());
      products.append(info);
    }
  }
  else {
    setError(query.lastError().text());
    qDebug()<<lastError();
  }
  return products;
}

//also discard group products. and unlimited stock ones...
QList<ProductInfo> Azahar::getLowStockProducts(double min)
{
  QList<ProductInfo> products; products.clear();
  ProductInfo info;
  QSqlQuery query(db);
  query.prepare("SELECT code FROM products WHERE (stockqty<=:minV and stockqty>1 and isAGroup=false and hasUnlimitedStock=false) ORDER BY code,stockqty ASC;");
  query.bindValue(":minV", min);
  if (query.exec()) {
    while (query.next()) {
      int fieldCode  = query.record().indexOf("code");
      info = getProductInfo(query.value(fieldCode).toString());
      products.append(info);
    }
  }
  else {
    setError(query.lastError().text());
    qDebug()<<lastError();
  }
  return products;
}

//The list will not include groups or packages
QList<ProductInfo> Azahar::getAllProducts()
{
    QList<ProductInfo> products; products.clear();
    ProductInfo info;
    QSqlQuery query(db);
    query.prepare("SELECT code,stockqty FROM products WHERE isAGroup=false ORDER BY code,stockqty ASC;");
    if (query.exec()) {
        while (query.next()) {
            int fieldCode  = query.record().indexOf("code");
            info = getProductInfo(query.value(fieldCode).toString());
            products.append(info);
        }
    }
    else {
        setError(query.lastError().text());
        qDebug()<<lastError();
    }
    return products;
}

QList<ProductInfo> Azahar::getGroupProductsList(QString id, bool notConsiderDiscounts)
{
  //qDebug()<<"getGroupProductsList...";
  QList<ProductInfo> pList;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QString ge = getProductGroupElementsStr(id); //DONOT USE getProductInfo... this will cause an infinite loop because at that method this method is called trough getGroupAverageTax
    //qDebug()<<"elements:"<<ge;
    if (ge.isEmpty()) return pList;
    QStringList pq = ge.split(",");
    foreach(QString str, pq) {
      QString c = str.section('/',0,0);
      double     q = str.section('/',1,1).toDouble();
      //get info
      ProductInfo pi = getProductInfo(c, notConsiderDiscounts);
      pi.qtyOnList = q;
      pList.append(pi);
      //qDebug()<<" code:"<<c<<" qty:"<<q;
    }
  }
  return pList;
}


//This method replaces the two above deprecated methods.
///NOTE: Aug 17 2011: this tax does not take into account the ocassional discounts or price changes. It may be false.
GroupInfo Azahar::getGroupPriceAndTax(ProductInfo pi)
{
  GroupInfo gInfo;
  gInfo.price = 0;
  gInfo.taxMoney = 0;
  gInfo.tax = 0;
  gInfo.cost = 0;
  gInfo.count = 0;
  gInfo.priceDrop = pi.groupPriceDrop;
  gInfo.name = pi.desc;
  
  if ( pi.code == "0" ) return gInfo;

  QList<ProductInfo> plist = getGroupProductsList(pi.code, true); //for getting products with taxes not including discounts.
  foreach(ProductInfo info, plist) {
    info.price      = (info.price -info.price*(pi.groupPriceDrop/100));
    info.totaltax   = info.price * info.qtyOnList *  (info.tax/100) + info.price * info.qtyOnList *  (info.extratax/100);
    
    gInfo.price    += info.price * info.qtyOnList;
    gInfo.cost     += info.cost  * info.qtyOnList;
    gInfo.taxMoney += info.totaltax*info.qtyOnList;
    gInfo.count    += info.qtyOnList;
    bool yes = false;
    if (info.stockqty >= info.qtyOnList ) yes = true;
    gInfo.isAvailable = (gInfo.isAvailable && yes );
    qDebug()<<" < GROUP - EACH ONE > Product: "<<info.desc<<" Price with p-drop: "<<info.price<<" taxMoney: "<<info.totaltax*info.qtyOnList<<" info.tax:"<<info.tax<<" info.extataxes:"<<info.extratax;
    gInfo.productsList.insert( info.code, info );
  }

  foreach(ProductInfo info, gInfo.productsList) {
    gInfo.tax += (info.totaltax*info.qtyOnList/gInfo.price)*100;
    qDebug()<<" < GROUP TAX >  qtyOnList:"<<info.qtyOnList<<" tax money for product: "<<info.totaltax<<" group price:"<<gInfo.price<<" taxMoney for group:"<<gInfo.taxMoney<<" tax % for group:"<< gInfo.tax<<" Group cost:"<<gInfo.cost;
  }

  return gInfo;
}

QString Azahar::getProductGroupElementsStr(QString code)
{
  QString result;
  if (db.isOpen()) {
    QString qry = QString("SELECT groupElements from products WHERE code=%1").arg(code);
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
    }
    if (query.size() <= 0)
      setError(i18n("Error serching product id %1, Rows affected: %2", code,query.size()));
    else {
      while (query.next()) {
        int field = query.record().indexOf("groupElements");
        result    = query.value(field).toString();
      }
    }
  }
  return result;
}

void Azahar::updateGroupPriceDrop(QString code, double pd)
{
  if (db.isOpen()) {
    QSqlQuery query(db);
    query.prepare("UPDATE products SET groupPriceDrop=:pdrop WHERE code=:code");
    query.bindValue(":code", code);
    query.bindValue(":pdrop", pd);
    if (!query.exec()) setError(query.lastError().text());
    qDebug()<<"<*> updateGroupPriceDrop  | Rows Affected:"<<query.numRowsAffected();
  }
}

void Azahar::updateGroupElements(QString code, QString elementsStr)
{
  if (db.isOpen()) {
    QSqlQuery query(db);
    query.prepare("UPDATE products SET groupElements=:elements WHERE code=:code");
    query.bindValue(":code", code);
    query.bindValue(":elements", elementsStr);
    if (!query.exec()) setError(query.lastError().text());
    qDebug()<<"<*> updateGroupElements  | Rows Affected:"<<query.numRowsAffected();
  }
}


//CATEGORIES
bool Azahar::insertCategory(QString text)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query.prepare("INSERT INTO categories (text) VALUES (:text);");
  query.bindValue(":text", text);
  if (!query.exec()) {
    setError(query.lastError().text());
  }
  else result=true;
  
  return result;
}

QHash<QString, int> Azahar::getCategoriesHash()
{
  QHash<QString, int> result;
  result.clear();
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec("select * from categories;")) {
      while (myQuery.next()) {
        int fieldId   = myQuery.record().indexOf("catid");
        int fieldText = myQuery.record().indexOf("text");
        int id = myQuery.value(fieldId).toInt();
        QString text = myQuery.value(fieldText).toString();
        result.insert(text, id);
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

QStringList Azahar::getCategoriesList()
{
    // Alphabetical order!
  QStringList result;
  result.clear();
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec("select text from categories order by text;")) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("text");
        QString text = myQuery.value(fieldText).toString();
        result.append(text);
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

qulonglong Azahar::getCategoryId(QString texto)
{
  qulonglong result=0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    QString qryStr = QString("SELECT categories.catid FROM categories WHERE text='%1';").arg(texto);
    if (myQuery.exec(qryStr) ) {
      while (myQuery.next()) {
        int fieldId   = myQuery.record().indexOf("catid");
        qulonglong id= myQuery.value(fieldId).toULongLong();
        result = id;
      }
    }
    else {
      setError(myQuery.lastError().text());
    }
  }
  return result;
}

QString Azahar::getCategoryStr(qulonglong id)
{
  QString result;
  QSqlQuery query(db);
  QString qstr = QString("select text from categories where catid=%1;").arg(id);
  if (query.exec(qstr)) {
    while (query.next()) {
      int fieldText = query.record().indexOf("text");
      result = query.value(fieldText).toString();
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}

bool Azahar::renameCategory(qulonglong id, QString newText)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  // Reassign all products in that category to newId
  query=QString("update categories set text='%1' where catid=%2").arg(newText).arg(id);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  qDebug()<<"Rename category"<<query.lastQuery()<<query.lastError();
  return result;
}

bool Azahar::deleteCategory(qulonglong id, qulonglong newId=0)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  // Reassign all products in that category to newId
  query=QString("update products set category=%1 where category=%2").arg(newId).arg(id);
  query.exec();
  qDebug()<<"Reassign products:"<<query.lastError().text()<<query.lastQuery();
  query = QString("DELETE FROM categories WHERE catid=%1").arg(id);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}


//MEASURES
bool Azahar::insertMeasure(QString text)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query.prepare("INSERT INTO measures (text) VALUES (:text);");
  query.bindValue(":text", text);
  if (!query.exec()) {
    setError(query.lastError().text());
  }
  else result=true;
  
  return result;
}

qulonglong Azahar::getMeasureId(QString texto)
{
  qulonglong result=0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    QString qryStr = QString("select measures.id from measures where text='%1';").arg(texto);
    if (myQuery.exec(qryStr) ) {
      while (myQuery.next()) {
        int fieldId   = myQuery.record().indexOf("id");
        qulonglong id = myQuery.value(fieldId).toULongLong();
        result = id;
      }
    }
    else {
      setError(myQuery.lastError().text());
    }
  }
  return result;
}

QString Azahar::getMeasureStr(qulonglong id)
{
  QString result;
  QSqlQuery query(db);
  QString qstr = QString("select text from measures where measures.id=%1;").arg(id);
  if (query.exec(qstr)) {
    while (query.next()) {
      int fieldText = query.record().indexOf("text");
      result = query.value(fieldText).toString();
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}

QStringList Azahar::getMeasuresList()
{
  QStringList result;
  result.clear();
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec("select text from measures;")) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("text");
        QString text = myQuery.value(fieldText).toString();
        result.append(text);
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

bool Azahar::deleteMeasure(qulonglong id)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query = QString("DELETE FROM measures WHERE id=%1").arg(id);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}

//OFFERS
bool Azahar::createOffer(OfferInfo info)
{
  bool result=false;
  QString qryStr;
  QSqlQuery query(db);
  if (!db.isOpen()) db.open();

  ProductInfo p = getProductInfo( info.productCode );
  if ( p.isNotDiscountable ) {
      setError(i18n("Unable to set an offer/discount for the selected produc because it is NOT DISCOUNTABLE"));
      return false;
  }
      
  
  //The product has no offer yet.
  //NOTE: Now multiple offers supported (to save offers history)
  qryStr = "INSERT INTO offers (discount, datestart, dateend, product_id) VALUES(:discount, :datestart, :dateend, :code)";
  query.prepare(qryStr);
  query.bindValue(":discount", info.discount );
  query.bindValue(":datestart", info.dateStart.toString("yyyy-MM-dd"));
  query.bindValue(":dateend", info.dateEnd.toString("yyyy-MM-dd"));
  query.bindValue(":code", info.productCode);
  if (query.exec()) result = true; else setError(query.lastError().text());

  return result;
}

bool Azahar::deleteOffer(qlonglong id)
{
  bool result=false;
  if (db.isOpen()) {
    QString qry = QString("DELETE from offers WHERE offers.id=%1").arg(id);
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
    }
    if (query.numRowsAffected() == 1) result = true;
    else setError(i18n("Error deleting offer id %1, Rows affected: %2", id,query.numRowsAffected()));
  }
  return result;
}


QString Azahar::getOffersFilterWithText(QString text)
{
  QStringList codes;
  QString result="";
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery qry(db);
    QString qryStr= QString("SELECT P.code, P.name, O.product_id FROM offers AS O, products AS P WHERE P.code = O.product_id and P.name REGEXP '%1' ").arg(text);
    if (!qry.exec(qryStr)) setError(qry.lastError().text());
    else {
      codes.clear();
      while (qry.next()) {
        int fieldCode   = qry.record().indexOf("code");
        QString c = qry.value(fieldCode).toString();
        codes.append(QString("offers.product_id=%1 ").arg(c));
      }
      result = codes.join(" OR ");
    }
  }
  return result;
}

bool Azahar::moveOffer(QString oldp, QString newp)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  QSqlQuery q(db);
  QString qs = QString("UPDATE offers SET product_id=%1 WHERE product_id=%2;").arg(newp).arg(oldp);
  if (!q.exec( qs )) setError(q.lastError().text()); else result = true;
  return result;
}


//USERS
bool Azahar::insertUser(UserInfo info)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password, salt, name, address, phone, phone_movil, role, photo) VALUES(:uname, :pass, :salt, :name, :address, :phone, :cell, :rol, :photo)");
    query.bindValue(":photo", info.photo);
    query.bindValue(":uname", info.username);
    query.bindValue(":name", info.name);
    query.bindValue(":address", info.address);
    query.bindValue(":phone", info.phone);
    query.bindValue(":cell", info.cell);
    query.bindValue(":pass", info.password);
    query.bindValue(":salt", info.salt);
    query.bindValue(":rol", info.role);
    if (!query.exec()) setError(query.lastError().text()); else result = true;
    qDebug()<<"USER insert:"<<query.lastError();
    //FIXME: We must see error types, which ones are for duplicate KEYS (codes) to advertise the user.
  }//db open
  return result;
}

QHash<QString,UserInfo> Azahar::getUsersHash()
{
  QHash<QString,UserInfo> result;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    QString qry = "SELECT * FROM users;";
    if (query.exec(qry)) {
      while (query.next()) {
        int fielduId       = query.record().indexOf("id");
        int fieldUsername  = query.record().indexOf("username");
        int fieldPassword  = query.record().indexOf("password");
        int fieldSalt      = query.record().indexOf("salt");
        int fieldName      = query.record().indexOf("name");
        int fieldRole      = query.record().indexOf("role"); // see role numbers at enums.h
        int fieldPhoto     = query.record().indexOf("photo");
        //more fields, now im not interested in that...
        UserInfo info;
        info.id       = query.value(fielduId).toInt();
        info.username = query.value(fieldUsername).toString();
        info.password = query.value(fieldPassword).toString();
        info.salt     = query.value(fieldSalt).toString();
        info.name     = query.value(fieldName).toString();
        info.photo    = query.value(fieldPhoto).toByteArray();
        info.role     = query.value(fieldRole).toInt();
        result.insert(info.username, info);
        //qDebug()<<"got user:"<<info.username;
      }
    }
    else {
      qDebug()<<"**Error** :"<<query.lastError();
    }
  }
 return result;
}

UserInfo Azahar::getUserInfo(const qulonglong &userid)
{
  UserInfo info;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    QString qry = QString("SELECT * FROM users where id=%1;").arg(userid);
    if (query.exec(qry)) {
      while (query.next()) {
        int fielduId       = query.record().indexOf("id");
        int fieldUsername  = query.record().indexOf("username");
        int fieldPassword  = query.record().indexOf("password");
        int fieldSalt      = query.record().indexOf("salt");
        int fieldName      = query.record().indexOf("name");
        int fieldRole      = query.record().indexOf("role"); // see role numbers at enums.h
        int fieldPhoto     = query.record().indexOf("photo");
        //more fields, now im not interested in that...
        info.id       = query.value(fielduId).toInt();
        info.username = query.value(fieldUsername).toString();
        info.password = query.value(fieldPassword).toString();
        info.salt     = query.value(fieldSalt).toString();
        info.name     = query.value(fieldName).toString();
        info.photo    = query.value(fieldPhoto).toByteArray();
        info.role     = query.value(fieldRole).toInt();
        //qDebug()<<"got user:"<<info.username;
      }
    }
    else {
      qDebug()<<"**Error** :"<<query.lastError();
    }
  }
  return info; 
}

bool Azahar::updateUser(UserInfo info)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query.prepare("UPDATE users SET photo=:photo, username=:uname, name=:name, address=:address, phone=:phone, phone_movil=:cell, salt=:salt, password=:pass, role=:rol  WHERE id=:code;");
  query.bindValue(":code", info.id);
  query.bindValue(":photo", info.photo);
  query.bindValue(":uname", info.username);
  query.bindValue(":name", info.name);
  query.bindValue(":address", info.address);
  query.bindValue(":phone", info.phone);
  query.bindValue(":cell", info.cell);
  query.bindValue(":pass", info.password);
  query.bindValue(":salt", info.salt);
  query.bindValue(":rol", info.role);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}

QString Azahar::getUserName(QString username)
{
  QString name = "";
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery queryUname(db);
    QString qry = QString("SELECT name FROM users WHERE username='%1'").arg(username);
    if (!queryUname.exec(qry)) { setError(queryUname.lastError().text()); }
    else {
      if (queryUname.isActive() && queryUname.isSelect()) { //qDebug()<<"queryUname select && active.";
        if (queryUname.first()) { //qDebug()<<"queryUname.first()=true";
          name = queryUname.value(0).toString();
        }
      }
    }
  } else { setError(db.lastError().text()); }
  return name;
}

QString Azahar::getUserName(qulonglong id)
{
  QString name = "";
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery queryUname(db);
    QString qry = QString("SELECT name FROM users WHERE users.id=%1").arg(id);
    if (!queryUname.exec(qry)) { setError(queryUname.lastError().text()); }
    else {
      if (queryUname.isActive() && queryUname.isSelect()) { //qDebug()<<"queryUname select && active.";
        if (queryUname.first()) { //qDebug()<<"queryUname.first()=true";
          name = queryUname.value(0).toString();
        }
      }
    }
  } else { setError(db.lastError().text()); }
  return name;
}

QStringList Azahar::getUsersList()
{
  QStringList result;
  result.clear();
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec("select name from users;")) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("name");
        QString text = myQuery.value(fieldText).toString();
        result.append(text);
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

unsigned int Azahar::getUserId(QString uname)
{
  unsigned int iD = 0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery queryId(db);
    QString qry = QString("SELECT id FROM users WHERE username='%1'").arg(uname);
    if (!queryId.exec(qry)) { setError(queryId.lastError().text()); }
    else {
      if (queryId.isActive() && queryId.isSelect()) { //qDebug()<<"queryId select && active.";
        if (queryId.first()) { //qDebug()<<"queryId.first()=true";
        iD = queryId.value(0).toUInt();
        }
      }
    }
  } else { setError(db.lastError().text()); }
  return iD;
}

unsigned int Azahar::getUserIdFromName(QString uname)
{
  unsigned int iD = 0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery queryId(db);
    QString qry = QString("SELECT id FROM users WHERE name='%1'").arg(uname);
    if (!queryId.exec(qry)) { setError(queryId.lastError().text()); }
    else {
      if (queryId.isActive() && queryId.isSelect()) { //qDebug()<<"queryId select && active.";
      if (queryId.first()) { //qDebug()<<"queryId.first()=true";
      iD = queryId.value(0).toUInt();
      }
      }
    }
  } else { setError(db.lastError().text()); }
  return iD;
}

int Azahar::getUserRole(const qulonglong &userid)
{
  int role = 0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery queryId(db);
    QString qry = QString("SELECT role FROM users WHERE id=%1").arg(userid);
    if (!queryId.exec(qry)) { setError(queryId.lastError().text()); }
    else {
      if (queryId.isActive() && queryId.isSelect()) { //qDebug()<<"queryId select && active.";
        if (queryId.first()) { //qDebug()<<"queryId.first()=true";
        role = queryId.value(0).toInt();
        }
      }
    }
  } else { setError(db.lastError().text()); }
  return role;
}

bool Azahar::deleteUser(qulonglong id)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query = QString("DELETE FROM users WHERE id=%1").arg(id);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}

bool Azahar::_bindDonor(DonorInfo &info, QSqlQuery &query)
{
    bool result=false;
    query.bindValue(":id", info.id);
    query.bindValue(":photo", info.photo);
    query.bindValue(":name", info.name);
    query.bindValue(":email", info.emailDonor);
    query.bindValue(":code", info.code);
    query.bindValue(":address", info.address);
    query.bindValue(":phone", info.phone);
    query.bindValue(":since", info.since);
    query.bindValue(":refname", info.nameRefDonor);
    query.bindValue(":refsurname", info.surnameRefDonor);
    query.bindValue(":refemail", info.emailRefDonor);
    query.bindValue(":refphone", info.phoneRefDonor);
    query.bindValue(":notes", info.notesRefDonor);
    if (!query.exec()) setError(query.lastError().text()); else result = true;
    return result;
}

//DONORS
bool Azahar::insertDonor(DonorInfo info)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    QString q="INSERT INTO donors ";
    q=q+getInsertString(donorFields)+";";
    qDebug()<<"Insert Donor query: "<<q;
    query.prepare(q);
    result=_bindDonor(info,query);
  }
  return result;
}

bool Azahar::updateDonor(DonorInfo info)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  QString q="UPDATE donors SET ";
  q=q+getUpdateString(donorFields);
  q=q+" WHERE id=:id;";
  qDebug()<<"UpdateDonor query:"<<q;
  query.prepare(q);
  result=_bindDonor(info,query);
  return result;
}

DonorInfo Azahar::getDonorInfo(qulonglong clientId)
{
  DonorInfo info;
  info=_getDonorInfo(clientId);
  return info;
}

DonorInfo Azahar::_getDonorInfo(qulonglong clientId)
{
  DonorInfo info;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
      QSqlQuery qC(db);
      if (qC.exec(QString("select * from donors where id=%1;").arg(clientId))) {
        getDonorInfoFromQuery(qC,info);
      }
      else {
        qDebug()<<"ERROR: "<<qC.lastError();
      }
    }
 return info;
}

DonorInfo Azahar::getDonorInfo(QString clientCode)
{
    DonorInfo info;
    info=_getDonorInfo(clientCode);
    return info;
}

DonorInfo Azahar::_getDonorInfo(QString clientCode)
{
    DonorInfo info;
    info.name = "";
    info.id = 0;//to recognize errors.
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery qC(db);
        if (qC.exec(QString("select * from donors WHERE code='%1';").arg(clientCode))) {
            getDonorInfoFromQuery(qC,info);
        }
        else {
            qDebug()<<"ERROR: "<<qC.lastError();
        }
    }

    return info;
}

bool Azahar::getDonorInfoFromQuery(QSqlQuery &qC, DonorInfo &info){
    info.id=0;
    info.name="";
    if (qC.next()) {
      int fieldId     = qC.record().indexOf("id");
      int fieldCode   = qC.record().indexOf("code");
      int fieldName   = qC.record().indexOf("name");
      int fieldEmailDonor  = qC.record().indexOf("email");
      int fieldPhoto  = qC.record().indexOf("photo");
      int fieldSince  = qC.record().indexOf("since");
      int fieldPhone  = qC.record().indexOf("phone");
      int fieldAdd    = qC.record().indexOf("address");
      int fieldNameRefDonor   = qC.record().indexOf("refname");
      int fieldSurnameDonor   = qC.record().indexOf("refsurname");
      int fieldPhoneDonor   = qC.record().indexOf("refphone");
      int fieldEmailRefDonor   = qC.record().indexOf("refemail");
      int fieldNotes   = qC.record().indexOf("notes");

      //Should be only one
      info.id         = qC.value(fieldId).toUInt();
      info.code       = qC.value(fieldCode).toString();
      info.name       = qC.value(fieldName).toString();
      info.emailDonor    = qC.value(qC.record().indexOf("email")).toString();
      info.photo      = qC.value(fieldPhoto).toByteArray();
      info.since      = qC.value(fieldSince).toDate();
      info.phone      = qC.value(fieldPhone).toString();
      info.address    = qC.value(fieldAdd).toString();
      info.nameRefDonor    = qC.value(qC.record().indexOf("refname")).toString();
      info.surnameRefDonor    = qC.value(qC.record().indexOf("refsurname")).toString();
      info.emailRefDonor    = qC.value(qC.record().indexOf("refemail")).toString();
      info.phoneRefDonor    = qC.value(qC.record().indexOf("refphone")).toString();
      info.notesRefDonor    =   qC.value(qC.record().indexOf("notes")).toString();
      return true;
    }
    return false;
}

bool Azahar::deleteDonor(qulonglong id)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query = QString("DELETE FROM donors WHERE id=%1").arg(id);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}

//CLIENTS

bool Azahar::_bindClient(ClientInfo &info, QSqlQuery &query)
{
    bool result=false;
    query.bindValue(":id", info.id);
    query.bindValue(":code", info.code);
    query.bindValue(":name", info.name);
    query.bindValue(":surname", info.surname);
    query.bindValue(":email", info.email);
    query.bindValue(":address", info.address);
    query.bindValue(":nation", info.nation);
    query.bindValue(":photo", info.photo);
    query.bindValue(":monthly", info.monthly);
    query.bindValue(":parent", info.parentClient);
    query.bindValue(":phone", info.phone);
    query.bindValue(":since", info.since);
    query.bindValue(":birthDate", info.since);
    query.bindValue(":expiry", info.expiry);
    query.bindValue(":beginsusp", info.beginsusp);
    query.bindValue(":endsusp", info.endsusp);
    query.bindValue(":msgsusp", info.msgsusp);
    query.bindValue(":notes", info.notes);
    qDebug()<<"Insert client lastQuery: "<<query.boundValues();
    if (!query.exec()) setError(query.lastError().text()); else result = true;
    return result;
}

QString Azahar::getUpdateString(QStringList list)
{
    QString msg="";
    for (int i = 0; i<list.count(); ++i) {
        QString e=list.at(i);
        if (i==0) {
            msg="`"+e+"`=:"+e;
        } else {
            msg+=", `"+e+"`=:"+e;
        }
    }
    return msg+" ";
}

QString Azahar::getInsertString(QStringList list)
{
    QString msg="";
    QString val="";
    for (int i = 0; i<list.count(); ++i) {
        QString e=list.at(i);
        if (i==0) {
            msg=msg+"`"+e+"`";
            val=val+":"+e;
        } else {
            msg=msg+", "+"`"+e+"`";
            val=val+", :"+e;
        }
    }
    return "("+msg+") VALUES("+val+")";
}


bool Azahar::insertClient(ClientInfo info)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    QString q="INSERT INTO clients ";
    q=q+getInsertString(clientFields)+";";
    qDebug()<<"Insert Client query: "<<q;
    query.prepare(q);
    result=_bindClient(info,query);
  }
  return result;
}

bool Azahar::updateClient(ClientInfo info)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  QString q="UPDATE clients SET ";
  q=q+getUpdateString(clientFields);
  q=q+" WHERE id=:id;";
  qDebug()<<"UpdateClient query:"<<q;
  query.prepare(q);
  result=_bindClient(info,query);
  return result;
}



bool Azahar::getBasicInfoFromQuery(QSqlQuery &qC, BasicInfo &info){
    info.id=0;
    info.name="";
    info.code="";
    info.surname="";
    if (qC.next()) {
      int fieldId     = qC.record().indexOf("id");
      int fieldCode   = qC.record().indexOf("code");
      int fieldName   = qC.record().indexOf("name");
      int fieldSurname   = qC.record().indexOf("surname");
      //Should be only one
      info.id         = qC.value(fieldId).toULongLong();
      info.code       = qC.value(fieldCode).toString();
      info.name       = qC.value(fieldName).toString();
      // Notice: products does not have a surname!
      if (fieldSurname>=0) {
        info.surname       = qC.value(fieldSurname).toString();
      }
      return true;
    }
    return false;
}

bool Azahar::getClientInfoFromQuery(QSqlQuery &qC, ClientInfo &info){
    info.id=0;
    info.parentClient="";
    info.name="";
    if (qC.next()) {
      int fieldId     = qC.record().indexOf("id");
      int fieldCode   = qC.record().indexOf("code");
      int fieldName   = qC.record().indexOf("name");
      int fieldSurname   = qC.record().indexOf("surname");
      int fieldNation   = qC.record().indexOf("nation");
      int fieldEmail   = qC.record().indexOf("email");
      int fieldPhoto  = qC.record().indexOf("photo");
      int fieldSince  = qC.record().indexOf("since");
      int fieldExpiry = qC.record().indexOf("expiry");
      int fieldBirthDate   = qC.record().indexOf("birthDate");
      int fieldPhone  = qC.record().indexOf("phone");
      int fieldAdd    = qC.record().indexOf("address");
      int fieldParent = qC.record().indexOf("parent");
      int fieldMonthly = qC.record().indexOf("monthly");
      int fieldBeginsusp = qC.record().indexOf("beginsusp");
      int fieldEndsusp = qC.record().indexOf("endsusp");
      int fieldMsgsusp = qC.record().indexOf("msgsusp");
      int fieldNotes = qC.record().indexOf("notes");

      //Should be only one
      info.id         = qC.value(fieldId).toUInt();
      info.code       = qC.value(fieldCode).toString();      
      info.name       = qC.value(fieldName).toString();
      info.surname       = qC.value(qC.record().indexOf("surname")).toString();
      info.email       = qC.value(fieldEmail).toString();
      info.nation       = qC.value(fieldNation).toString();
      info.parentClient = qC.value(fieldParent).toString();
      info.photo      = qC.value(fieldPhoto).toByteArray();
      info.since      = qC.value(fieldSince).toDate();
      info.expiry     = qC.value(fieldExpiry).toDate();
      info.birthDate      = qC.value(qC.record().indexOf("birthDate")).toDate();
      info.phone      = qC.value(fieldPhone).toString();
      info.address    = qC.value(fieldAdd).toString();
      info.monthly   = qC.value(fieldMonthly).toDouble();
      info.email       = qC.value(qC.record().indexOf("email")).toString();
      info.nation       = qC.value(qC.record().indexOf("nation")).toString();
      info.beginsusp       = qC.value(qC.record().indexOf("beginsusp")).toDate();
      info.endsusp       = qC.value(qC.record().indexOf("endsusp")).toDate();
      info.msgsusp       = qC.value(qC.record().indexOf("msgsusp")).toString();
      info.notes       = qC.value(qC.record().indexOf("notes")).toString();
//      info.       = qC.value(qC.record().indexOf("")).toString();
      return true;
    }
    return false;
}

qulonglong Azahar::checkParent(ClientInfo &info)
{
    ClientInfo parentInfo;
    if (info.parentClient.count()>0){
        parentInfo.id=0;
        parentInfo=_getClientInfo(info.parentClient);
        if (parentInfo.id > 0){
            info.monthly=parentInfo.monthly;
            return parentInfo.id;
        }
    }
    return info.id;
}

Family Azahar::getFamily(ClientInfo &info)
{
    // Retrieve the list of ClientInfo pertaining to the family of info. The parent client is the first of the list.
    Family family;
    QList<ClientInfo> result;
    if (!db.isOpen()) {db.open();}
    if (!db.isOpen()) return family;
    ClientInfo parentInfo;
    QString parentCode;
    if (info.parentClient.count()==0) {
        parentInfo=info;
        parentCode=info.code;
        result.append(info);
    } else {
        parentInfo=_getClientInfo(info.parentClient);
        parentCode=info.parentClient;
        result.append(parentInfo);
        result.append(info);
    }
    QSqlQuery query(db);
    qDebug()<<"getFamily pre-query"<<parentCode;
    query.exec(QString("select * from clients where parent='%1';").arg(parentCode));
    qDebug()<<"getFamily query"<<query.lastError()<<query.lastQuery();
    // Cycle over results, to build the limits hash
    ClientInfo ci;
    while (getClientInfoFromQuery(query,ci)) {
        qDebug()<<"GET Family"<<info.code<<ci.code<<result.count();
        if (ci.code==info.code or ci.code==parentCode) {continue;}
        ci.photo="";
        result.append(ci);
    }
    family.members=result;
    return family;
}


bool Azahar::getLimitFromQuery(QSqlQuery &query, Limit &result)
{
    int fieldId     = query.record().indexOf("id");
    int fieldClientCode     = query.record().indexOf("clientCode");
    int fieldClientTag     = query.record().indexOf("clientTag");
    int fieldProductCode    = query.record().indexOf("productCode");
    int fieldProductCat     = query.record().indexOf("productCat");
    int fieldPriority     = query.record().indexOf("priority");
    int fieldLimit     = query.record().indexOf("limit");
    int fieldCurrent     = query.record().indexOf("current");
    int fieldParent     = query.record().indexOf("parent");
    result.id=query.value(fieldId).toLongLong();
    result.clientCode=query.value(fieldClientCode).toString();
    result.clientTag=query.value(fieldClientTag).toString();
    result.productCode=query.value(fieldProductCode).toString();
    result.productCat=query.value(fieldProductCat).toInt();
    result.priority=query.value(fieldPriority).toInt();
    result.limit=query.value(fieldLimit).toDouble();
    result.current=query.value(fieldCurrent).toDouble();
    result.parent=query.value(fieldParent).toLongLong();
    return true;
}

bool Azahar::_bindLimit(Limit &info, QSqlQuery &query)
{
    bool result=false;
    if (query.lastQuery().contains(":id")) {
        query.bindValue(":id", info.id);
    }
    query.bindValue(":clientCode", info.clientCode);
    query.bindValue(":clientTag", info.clientTag);
    query.bindValue(":productCode", info.productCode);
    query.bindValue(":productCat", info.productCat);
    query.bindValue(":limit", info.limit);
    query.bindValue(":current", info.current);
    query.bindValue(":priority", info.priority);
    query.bindValue(":parent", info.parent);
    if (!query.exec()) setError(query.lastError().text()); else result = true;
    return result;
}

Limit Azahar::getLimit(qulonglong limitId) {
    Limit lim;
    if (!db.isOpen()) db.open();
    if (!db.isOpen()) {
        return lim;
    }
    QSqlQuery query(db);
    query.prepare("select * from limits where id=:id;");
    query.bindValue(":id",limitId);
    query.exec();
    query.next();
    getLimitFromQuery(query,lim);
    qDebug()<<"Getting limit"<<limitId<<lim.limit<<query.lastQuery()<<query.lastError()<<query.boundValues();
    return lim;
}

bool Azahar::deleteLimit(qlonglong &limitId)
{
    if (!db.isOpen()) db.open();
    if (!db.isOpen()) {
        return false;
    }
    QSqlQuery query(db);
    query.prepare("delete from limits where id=:id;");
    query.bindValue(":id",limitId);
    query.exec();
    return true;
}

bool Azahar::deleteLimit(Limit &lim) {
     qDebug()<<"deleting limit:"<<lim.clientCode<<lim.clientTag<<lim.productCode<<lim.productCat<<lim.limit<<lim.priority;
    return deleteLimit(lim.id);
}

bool Azahar::insertLimit(Limit &lim)
{
    qDebug()<<"inserting limit:"<<lim.clientCode<<lim.clientTag<<lim.productCode<<lim.productCat<<lim.limit<<lim.priority;
    if (!db.isOpen()) db.open();
    if (!db.isOpen()) {
        return false;
    }
    QSqlQuery query(db);
    QString q="INSERT INTO limits ";
    q=q+getInsertString(limitFields)+";";
    qDebug()<<"Insert Client query: "<<q;
    query.prepare(q);
    bool r=_bindLimit(lim, query);
    qDebug()<<"insertLimit: "<<r<<query.lastError()<<query.boundValues();
    return r;
}


bool Azahar::modifyLimit(Limit &lim)
{

    qDebug()<<"modify limit:"<<lim.id<<lim.clientCode<<lim.clientTag<<lim.productCat<<lim.productCode<<lim.limit<<lim.parent;
    if (!db.isOpen()) db.open();
    if (!db.isOpen()) {
        return false;
    }
    QString q="UPDATE limits set ";
    q+=getUpdateString(limitFields);
    q+=" where id=:id;";
    QSqlQuery query(db);
    query.prepare(q);
    bool r=_bindLimit(lim,query);
    qDebug()<<"modifyLimit: "<<r<<query.lastError()<<query.boundValues()<<query.lastQuery();
\
    // If it's general, should recursively modify all limits having same parent as lim
    if (lim.parent<=0) {
        q="select * from limits where parent=:parent";
        query.prepare(q);
        query.exec(q);
        while (query.next()) {
            // Get child limit
            Limit child;
            getLimitFromQuery(query,child);
            // Copy updatable fields from parent to child
            child.clientTag=lim.clientTag;
            child.limit=lim.limit;
            child.priority=lim.priority;
            child.productCat=lim.productCat;
            child.productCode=lim.productCode;
            // Recursively modify the child limit
            modifyLimit(child);
        }
    }
    return true;
}

QStringList Azahar::getClientLimits(ClientInfo &cInfo, ProductInfo &pInfo, QHash<QString,Limit> &currentLimits){
    // Retrieve the list of limits regarding client cInfo for product pInfo
    // Search applicable specific limits
    // Returns a list of applicable limits ids and updates the currentLimits hash

    QStringList result;
    if (!db.isOpen()) { db.open();}
    if (!db.isOpen()) {return QStringList();}
    QSqlQuery query(db);

    // TODO: creare limiti specifici se inesistenti, quindi cercare solo per i limiti specifici!
    qDebug()<<"Querying getClientLimits"<<cInfo.code<<currentLimits.count();
    qDebug()<<cInfo.tags.join("\", \"");
    QString q=QString("select * from limits where ( (clientCode in (:clientCode,'*') or (clientTag in (:clientTag))) and (productCode=:productCode or (productCode='*' and productCat=:productCat)) );");
    query.prepare(q);
    query.bindValue(":clientCode", cInfo.code);
    query.bindValue(":clientTag", cInfo.tags.join("', '"));
    query.bindValue(":productCode", pInfo.code);
    query.bindValue(":productCat", pInfo.category);
    query.exec();
    qDebug()<<"getClientLimits:"<<query.lastError()<<query.boundValues()<<query.lastQuery();
    // Cycle over results to build the limits hash
    while (query.next()) {
        Limit lim;
        getLimitFromQuery(query,lim);
        // key must contain both lim.id and clientCode, in case of same limit applying to more clients
        QString key=cInfo.code+"::"+QString::number(lim.id);
        qDebug()<<"Found key"<<key<<lim.id<<lim.parent;
        result.append(key);
        // If the limit is already present in current hash, skip
        if (currentLimits.contains(key)) {continue;}
        // If it is a general limit, specify it
        if (lim.clientCode=="*") {
            // Transform into a limit specification (just set clientCode, parentClient, and unset id)
            lim.clientCode=cInfo.code;
            lim.parent=lim.id;
            lim.id=-1; // so, it will be generated while committing
            // notice: key must remain equal to lim.id!
        }
        // add to hash
        currentLimits[key]=lim;
    }
    // Remove duplicate limits
    // Duplicates arise if a general limit is created before its specific limits are found
    QStringList rresult; // result filtered from duplicates
    QString genkey;
    for (int i; i<result.count(); ++i) {
        QString key=result.at(i);
        Limit lim=currentLimits[key];

        rresult.append(key);
        // Generic limit, do not check
        if (lim.parent<=0) { continue; }
        // Generated limit, keep
        if (lim.id<0) {continue;}
        // Build a possible generic key, deriving from the parent of lim
        genkey=cInfo.code+"::"+QString::number(lim.parent);
        // If that generic key exists
        if (currentLimits.contains(genkey)) {
            qDebug()<<"Removing duplicate key:"<<genkey<<key;
            // remove it from currentLimits and from rresult
            currentLimits.remove(genkey);
            rresult.removeAll(genkey);
        }
    }
    qDebug()<<"getClientLimits ending with"<<currentLimits.count()<<currentLimits.keys()<<rresult;
    return rresult;
}

bool Azahar::getFamilyLimits(Family &family, ProductInfo &pInfo, double qty) {
    // Iterate over all members of the family in order to find applicable limits
    // for the specified product
    qDebug()<<"getFamilyLimits"<<family.members.count();
    QStringList applicable;
    double result=0;
    for (int i=0; i<family.members.count(); ++i) {
        ClientInfo ci=family.members.at(i);
//        QHash <QString, Limit> limits =family.limits;
        qDebug()<<"Retrieved member"<<ci.code<<ci.name;
        applicable+=getClientLimits(ci, pInfo, family.limits);
    }

    family.lastProduct=pInfo;
    family.applicable=applicable;
    family.effectiveLimit=0;
    if (applicable.count()==0) {
        qDebug()<<"NO applicable limits:"<<applicable;
        return true;
    }
    for (int i=0; i<applicable.count(); ++i) {
        QString key=applicable.at(i);
        if (!family.limits.contains(key)) {
            qDebug()<<"Applicable limit NOT FOUND:"<<key;
        }
        Limit lim=family.limits.value(key);
        result+=lim.limit-lim.current;
        qDebug()<<"Applicable limit"<<key<<lim.limit<<lim.current<<result;
    }
    family.effectiveLimit=result;
    qDebug()<<"New effective limit"<<family.effectiveLimit;
    // If the product cannot be bought, return false
    if (pInfo.price*qty>result) {
        return false;
    }
    // it's ok! record applicable limits
    return true;

}

bool Azahar::changeFamilyLimits(Family &family, ProductInfo &pInfo, double qty) {
    // Increment/decrement by one product unit the current limit consumption for that product.
    if (getFamilyLimits(family,pInfo,qty)==false and qty>=1) {
        return false;
    }
    double mod=qty*pInfo.price/family.applicable.count();
    for (int i=0; i<family.applicable.count(); ++i) {
        QString key=family.applicable.at(i);
        Limit lim=family.limits.value(key);
        lim.current+=mod;
        family.limits[key]=lim;
    }
    return true;
}

void Azahar::commitLimits( QHash<QString,Limit> &currentLimits) {
    // Save to database current limit values
    QList<QString> keys=currentLimits.keys();
    for (int i; i<keys.count(); ++i) {
        Limit lim=currentLimits.value(keys.at(i));
        if (lim.id<0) {
            // new limit specification found
            insertLimit(lim);
        } else {
            // modify current limit
            modifyLimit(lim);
        }
    }
}



QStringList Azahar::getClientTags(QString clientCode)
{
    QStringList tags;
    if (clientCode.count() == 0) {
        return tags;
    }
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery qC(db);
        if (qC.exec(QString("select * from tags where clientCode='%1';").arg(clientCode))) {
            int fieldTag     = qC.record().indexOf("tag");
            while (qC.next()) {
                tags.append(qC.value(fieldTag).toString());
            }
        }
    }
    return tags;
}

void Azahar::setClientTags(ClientInfo info)
{
    qDebug()<<"setClientTags"<<info.tags;
    // Recupero vecchi tag
    QStringList old = getClientTags(info.code);
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery query(db);
        // Aggiungo tag mancanti
        for (int i = 0; i<info.tags.count(); ++i) {
            if (old.contains(info.tags.at(i))) { continue ;}
            qDebug()<<"ADDING"<<info.tags.at(i);
            query.prepare("INSERT INTO tags (clientCode, tag) VALUES (:idclient, :tag);");
            query.bindValue(":idclient",info.code);
            query.bindValue(":tag",info.tags.at(i));
            if (!query.exec()){
                qDebug()<<"ERROR ADDING"<<info.tags.at(i)<<query.lastError();
            }
        }
        // Rimuovo tag non più validi

        for (int i = 0; i<old.count(); ++i) {
            if (info.tags.contains(old.at(i))) { continue ; }
            qDebug()<<"REMOVING"<<old.at(i);
            query.prepare("DELETE FROM tags WHERE clientCode=:idclient and tag=:tag");
            query.bindValue(":idclient",info.code);
            query.bindValue(":tag",old.at(i));
            query.exec();
        }
    }
}

QStringList Azahar::getAvailableTags()
{
    qDebug()<<"getAvail";
    QStringList tags;
    if (!db.isOpen()) db.open();
    qDebug()<<"DB OPEN"<<db.isOpen();
    if (db.isOpen()) {
        QSqlQuery qC(db);
        qDebug()<<" Query";
        if (qC.exec(QString("select * from tags"))) {
            int fieldTag     = qC.record().indexOf("tag");
            qDebug()<<" Tag field"<<fieldTag;
            while (qC.next()) {
                qDebug()<<"Iterating";
                tags.append(qC.value(fieldTag).toString());
                qDebug()<<"TAG FOUND"<<tags.last();
            }
            qDebug()<<"Last query"<<qC.lastQuery();
        }
    } else {
        qDebug() <<"NOT OPENED";
    }


    tags.removeDuplicates();
    qDebug()<<"Returning"<<tags;
    return tags;
}

ClientInfo Azahar::getClientInfo(qulonglong clientId)
{
  ClientInfo info;
  info=_getClientInfo(clientId);
  checkParent(info);
  return info;
}

ClientInfo Azahar::_getClientInfo(qulonglong clientId)
{
  ClientInfo info;
//  info.name = "";
//  info.parentClient="";
//  info.id = 0;//to recognize errors.
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
      QSqlQuery qC(db);
      if (qC.exec(QString("select * from clients where id=%1;").arg(clientId))) {
        getClientInfoFromQuery(qC,info);
        info.tags=getClientTags(info.code);
      }
      else {
        qDebug()<<"ERROR: "<<qC.lastError();
      }
    }
 return info;
}

ClientInfo Azahar::getClientInfo(QString clientCode)
{
    ClientInfo info;
    info=_getClientInfo(clientCode);
    checkParent(info);
    return info;
}

ClientInfo Azahar::_getClientInfo(QString clientCode)
{
    ClientInfo info;
    info.name = "";
    info.id = 0;//to recognize errors.
    info.parentClient="";
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery qC(db);
        if (qC.exec(QString("select * from clients WHERE code='%1';").arg(clientCode))) {
            getClientInfoFromQuery(qC,info);
            info.tags=getClientTags(clientCode);
        }
        else {
            qDebug()<<"ERROR: "<<qC.lastError();
        }
    }

    return info;
}



QString Azahar::getMainClient()
{
 QString result;
 ClientInfo info;
  if (m_mainClient == "undefined") {
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
      QSqlQuery qC(db);
      if (qC.exec("select * from clients where id=1;")) {
        while (qC.next()) {
          int fieldName   = qC.record().indexOf("name");
          info.name       = qC.value(fieldName).toString();
          m_mainClient = info.name;
          result = info.name;
        }
      }
      else {
        qDebug()<<"ERROR: "<<qC.lastError();
      }
    }
  } else result = m_mainClient;
return result;
}

QHash<int, BasicInfo> Azahar::getBasicHash(QString table)
{
 QHash<int, BasicInfo> result;
 BasicInfo info;
 QString select;
 select="select * from " + table;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery qC(db);
    if (qC.exec(select)) {

      while (getBasicInfoFromQuery(qC,info)) {
        result.insert(info.id, info);
        qDebug()<<"Inserted "<<info.name<<result.count()<<qC.lastError();

      }
    }
    else {
      qDebug()<<"ERROR: "<<qC.lastError();
    }
  }
  qDebug()<<"getBasicHash count:"<<result.count();
  return result;
}

QHash<int, ClientInfo> Azahar::getClientsHash()
{
 QHash<int, ClientInfo> result;
 ClientInfo info;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery qC(db);
    if (qC.exec("select * from clients;")) {

      while (getClientInfoFromQuery(qC,info)) {
        checkParent(info);
        info.photo = "";
        result.insert(info.id, info);
        if (info.id == 1) {
            m_mainClient = info.name+" "+info.surname;
        }
      }
    }
    else {
      qDebug()<<"ERROR: "<<qC.lastError();
    }
  }
  return result;
}

QStringList Azahar::getClientsList()
{
  QStringList result;
  result.clear();
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec("select name from clients;")) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("name");
        QString text = myQuery.value(fieldText).toString();
        result.append(text);
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

// Cerco la lista di clienti che hanno parentClient come cliente pagante
void Azahar::getChildrenClientsList(QString parentClient, QStringList& codes, QStringList& names)
{
    codes.clear();
    names.clear();
  qDebug()<<"Starting getChildreClientsList"<<parentClient;
  if (!db.isOpen()) db.open();
  qDebug()<<"Is db open?"<<db.isOpen();

  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    myQuery.prepare("select code,name from clients where parent = :parent ;");
    myQuery.bindValue(":parent", parentClient);

    qDebug()<<"Last Query: "<<myQuery.lastQuery();
    if (myQuery.exec()) {
      while (myQuery.next()) {
        int fieldName = myQuery.record().indexOf("name");
        int fieldCode = myQuery.record().indexOf("code");
        QString name = myQuery.value(fieldName).toString();
        QString code = myQuery.value(fieldCode).toString();
        names.append(name);
        codes.append(code);
        qDebug()<<"Found: "<<code<<" , "<<name;
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  else {
      qDebug()<<"ERROR Database Closed: "<<db.lastError();
  }

}

unsigned int Azahar::getClientId(QString uname)
{
  unsigned int iD = 0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery queryId(db);
    QString qry = QString("SELECT clients.id FROM clients WHERE clients.name='%1'").arg(uname);
    if (!queryId.exec(qry)) { setError(queryId.lastError().text()); }
    else {
      if (queryId.isActive() && queryId.isSelect()) { //qDebug()<<"queryId select && active.";
       if (queryId.first()) { //qDebug()<<"queryId.first()=true";
        iD = queryId.value(0).toUInt();
       }
      }
    }
  } else { setError(db.lastError().text()); }
  return iD;
}

bool Azahar::deleteClient(qulonglong id)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query = QString("DELETE FROM clients WHERE id=%1").arg(id);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}


//TRANSACTIONS

TransactionInfo Azahar::getTransactionInfo(qulonglong id)
{
  TransactionInfo info;
  info.id = 0;
  QString qry = QString("SELECT * FROM transactions WHERE id=%1").arg(id);
  QSqlQuery query;
  if (!query.exec(qry)) { qDebug()<<query.lastError(); }
  else {
    while (query.next()) {
      int fieldId = query.record().indexOf("id");
      int fieldAmount = query.record().indexOf("amount");
      int fieldDate   = query.record().indexOf("date");
      int fieldTime   = query.record().indexOf("time");
      int fieldPaidWith = query.record().indexOf("paidwith");
      int fieldPayMethod = query.record().indexOf("paymethod");
      int fieldType      = query.record().indexOf("type");
      int fieldChange    = query.record().indexOf("changegiven");
      int fieldState     = query.record().indexOf("state");
      int fieldUserId    = query.record().indexOf("userid");
      int fieldClientId  = query.record().indexOf("clientid");
      int fieldCardNum   = query.record().indexOf("cardnumber");
      int fieldCardAuth  = query.record().indexOf("cardauthnumber");
      int fieldItemCount = query.record().indexOf("itemcount");
      int fieldItemsList = query.record().indexOf("itemsList");
      int fieldDiscount  = query.record().indexOf("disc");
      int fieldDiscMoney = query.record().indexOf("discmoney");
      int fieldPoints    = query.record().indexOf("points");
      int fieldUtility   = query.record().indexOf("utility");
      int fieldTerminal  = query.record().indexOf("terminalnum");
      int fieldTax       = query.record().indexOf("totalTax");
      int fieldSpecialOrders = query.record().indexOf("specialOrders");
      int fieldDonor       = query.record().indexOf("donor");
      
      info.id     = query.value(fieldId).toULongLong();
      info.amount = query.value(fieldAmount).toDouble();
      info.date   = query.value(fieldDate).toDate();
      info.time   = query.value(fieldTime).toTime();
      info.paywith= query.value(fieldPaidWith).toDouble();
      info.paymethod = query.value(fieldPayMethod).toInt();
      info.type      = query.value(fieldType).toInt();
      info.changegiven = query.value(fieldChange).toDouble();
      info.state     = query.value(fieldState).toInt();
      info.userid    = query.value(fieldUserId).toULongLong();
      info.clientid  = query.value(fieldClientId).toULongLong();
      info.cardnumber= query.value(fieldCardNum).toString();//.replace(0,15,"***************"); //FIXED: Only save last 4 digits;
      info.cardauthnum=query.value(fieldCardAuth).toString();
      info.itemcount = query.value(fieldItemCount).toInt();
      info.itemlist  = query.value(fieldItemsList).toString();
      info.disc      = query.value(fieldDiscount).toDouble();
      info.discmoney = query.value(fieldDiscMoney).toDouble();
      info.points    = query.value(fieldPoints).toULongLong();
      info.utility   = query.value(fieldUtility).toDouble();
      info.terminalnum=query.value(fieldTerminal).toInt();
      info.totalTax   = query.value(fieldTax).toDouble();
      info.specialOrders = query.value(fieldSpecialOrders).toString();
      info.donor = query.value(fieldDonor).toString();
    }
  }
  return info;
}

ProfitRange Azahar::getMonthProfitRange()
{
  QList<TransactionInfo> monthTrans = getMonthTransactionsForPie();
  ProfitRange range;
  QList<double> profitList;
  TransactionInfo info;
  for (int i = 0; i < monthTrans.size(); ++i) {
    info = monthTrans.at(i);
    profitList.append(info.utility);
  }

  if (!profitList.isEmpty()) {
   qSort(profitList.begin(),profitList.end()); //sorting in ascending order (1,2,3..)
   range.min = profitList.first();
   range.max = profitList.last();
  } else {range.min=0.0; range.max=0.0;}

  return range;
}

ProfitRange Azahar::getMonthSalesRange()
{
  QList<TransactionInfo> monthTrans = getMonthTransactionsForPie();
  ProfitRange range;
  QList<double> salesList;
  TransactionInfo info;
  for (int i = 0; i < monthTrans.size(); ++i) {
    info = monthTrans.at(i);
    salesList.append(info.amount);
  }
  
  if (!salesList.isEmpty()) {
    qSort(salesList.begin(),salesList.end()); //sorting in ascending order (1,2,3..)
    range.min = salesList.first();
    range.max = salesList.last();
  } else {range.min=0.0; range.max=0.0;}

  return range;
}

QList<TransactionInfo> Azahar::getMonthTransactionsForPie()
{
  ///just return the amount and the profit.
  QList<TransactionInfo> result;
  TransactionInfo info;
  QSqlQuery qryTrans(db);
  QDate today = QDate::currentDate();
  QDate startDate = QDate(today.year(), today.month(), 1); //get the 1st of the month.
  //NOTE: in the next query, the state and type are hardcoded (not using the enums) because problems when preparing query.
  qryTrans.prepare("SELECT date,SUM(amount),SUM(utility) from transactions where (date BETWEEN :dateSTART AND :dateEND ) AND (type=1) AND (state=2 OR state=8 OR state=9) GROUP BY date ASC;");
  qryTrans.bindValue("dateSTART", startDate.toString("yyyy-MM-dd"));
  qryTrans.bindValue("dateEND", today.toString("yyyy-MM-dd"));
  //tCompleted=2, tSell=1. With a placeholder, the value is inserted as a string, and cause the query to fail.
  if (!qryTrans.exec() ) {
    int errNum = qryTrans.lastError().number();
    QSqlError::ErrorType errType = qryTrans.lastError().type();
    QString errStr = qryTrans.lastError().text();
    QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
    setError(details);
  } else {
    while (qryTrans.next()) {
      int fieldAmount = qryTrans.record().indexOf("SUM(amount)");
      int fieldProfit = qryTrans.record().indexOf("SUM(utility)");
      int fieldDate = qryTrans.record().indexOf("date");
      info.amount = qryTrans.value(fieldAmount).toDouble();
      info.utility = qryTrans.value(fieldProfit).toDouble();
      info.date = qryTrans.value(fieldDate).toDate();
      result.append(info);
      //qDebug()<<"APPENDING:"<<info.date<< " Sales:"<<info.amount<<" Profit:"<<info.utility;
    }
    //qDebug()<<"executed query:"<<qryTrans.executedQuery();
    //qDebug()<<"Qry size:"<<qryTrans.size();
    
  }
  return result;
}

QList<TransactionInfo> Azahar::getMonthTransactions()
{
  QList<TransactionInfo> result;
  TransactionInfo info;
  QSqlQuery qryTrans(db);
  QDate today = QDate::currentDate();
  QDate startDate = QDate(today.year(), today.month(), 1); //get the 1st of the month.
  //NOTE: in the next query, the state and type are hardcoded (not using the enums) because problems when preparing query.
  qryTrans.prepare("SELECT id,date from transactions where (date BETWEEN :dateSTART AND :dateEND ) AND (type=1) AND (state=2 OR state=8 OR state=9) ORDER BY date,id ASC;");
  qryTrans.bindValue("dateSTART", startDate.toString("yyyy-MM-dd"));
  qryTrans.bindValue("dateEND", today.toString("yyyy-MM-dd"));
  //tCompleted=2, tSell=1. With a placeholder, the value is inserted as a string, and cause the query to fail.
  if (!qryTrans.exec() ) {
    int errNum = qryTrans.lastError().number();
    QSqlError::ErrorType errType = qryTrans.lastError().type();
    QString errStr = qryTrans.lastError().text();
    QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
    setError(details);
  } else {
    while (qryTrans.next()) {
      int fieldId = qryTrans.record().indexOf("id");
      info = getTransactionInfo(qryTrans.value(fieldId).toULongLong());
      result.append(info);
      //qDebug()<<"APPENDING: id:"<<info.id<<" "<<info.date;
    }
    //qDebug()<<"executed query:"<<qryTrans.executedQuery();
    //qDebug()<<"Qry size:"<<qryTrans.size();
    
  }
  return result;
}


QList<TransactionInfo> Azahar::getDayTransactions(int terminal)
{
    QList<TransactionInfo> result;
    TransactionInfo info;
    QSqlQuery qryTrans(db);
    QDate today = QDate::currentDate();
    //NOTE: in the next query, the state and type are hardcoded (not using the enums) because problems when preparing query. State 8 = OwnCreditCompletedPaymentPending
    qryTrans.prepare("SELECT id,time,paidwith,paymethod,amount,utility,totalTax from transactions where (date = :today) AND (terminalnum=:terminal) AND (type=1) AND (state=2 OR state=8 OR state=9) ORDER BY id ASC;");
    qryTrans.bindValue("today", today.toString("yyyy-MM-dd"));
    qryTrans.bindValue(":terminal", terminal);
    //tCompleted=2, tSell=1. With a placeholder, the value is inserted as a string, and cause the query to fail.
    if (!qryTrans.exec() ) {
      int errNum = qryTrans.lastError().number();
      QSqlError::ErrorType errType = qryTrans.lastError().type();
      QString errStr = qryTrans.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
      setError(details);
    } else {
      while (qryTrans.next()) {
        int fieldAmount = qryTrans.record().indexOf("amount");
        int fieldProfit = qryTrans.record().indexOf("utility");
        info.id = qryTrans.value(qryTrans.record().indexOf("id")).toULongLong();
        info.amount = qryTrans.value(fieldAmount).toDouble();
        info.utility = qryTrans.value(fieldProfit).toDouble();
        info.paymethod = qryTrans.value(qryTrans.record().indexOf("paymethod")).toInt();
        info.paywith = qryTrans.value(qryTrans.record().indexOf("paidwith")).toDouble();
        info.time = qryTrans.value(qryTrans.record().indexOf("time")).toTime();
        info.totalTax = qryTrans.value(qryTrans.record().indexOf("totalTax")).toDouble();
        result.append(info);
        //qDebug()<<"APPENDING:"<<info.id<< " Sales:"<<info.amount<<" Profit:"<<info.utility;
      }
      //qDebug()<<"executed query:"<<qryTrans.executedQuery();
      //qDebug()<<"Qry size:"<<qryTrans.size();

    }
    return result;
}

QList<TransactionInfo> Azahar::getDayTransactions()
{
  QList<TransactionInfo> result;
  TransactionInfo info;
  QSqlQuery qryTrans(db);
  QDate today = QDate::currentDate();
  //NOTE: in the next query, the state and type are hardcoded (not using the enums) because problems when preparing query.
  qryTrans.prepare("SELECT id,time,paidwith,paymethod,amount,utility,totalTax from transactions where (date = :today) AND (type=1) AND (state=2 OR state=8 OR state=9) ORDER BY id ASC;");
  qryTrans.bindValue("today", today.toString("yyyy-MM-dd"));
  //tCompleted=2, tSell=1. With a placeholder, the value is inserted as a string, and cause the query to fail.
  if (!qryTrans.exec() ) {
    int errNum = qryTrans.lastError().number();
    QSqlError::ErrorType errType = qryTrans.lastError().type();
    QString errStr = qryTrans.lastError().text();
    QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
    setError(details);
  } else {
    while (qryTrans.next()) {
      int fieldAmount = qryTrans.record().indexOf("amount");
      int fieldProfit = qryTrans.record().indexOf("utility");
      info.id = qryTrans.value(qryTrans.record().indexOf("id")).toULongLong();
      info.amount = qryTrans.value(fieldAmount).toDouble();
      info.utility = qryTrans.value(fieldProfit).toDouble();
      info.paymethod = qryTrans.value(qryTrans.record().indexOf("paymethod")).toInt();
      info.paywith = qryTrans.value(qryTrans.record().indexOf("paidwith")).toDouble();
      info.time = qryTrans.value(qryTrans.record().indexOf("time")).toTime();
      info.totalTax = qryTrans.value(qryTrans.record().indexOf("totalTax")).toDouble();
      result.append(info);
      //qDebug()<<"APPENDING:"<<info.id<< " Sales:"<<info.amount<<" Profit:"<<info.utility;
    }
    //qDebug()<<"executed query:"<<qryTrans.executedQuery();
    //qDebug()<<"Qry size:"<<qryTrans.size();
    
  }
  return result;
}

AmountAndProfitInfo  Azahar::getDaySalesAndProfit(int terminal)
{
    AmountAndProfitInfo result;
    QSqlQuery qryTrans(db);
    QDate today = QDate::currentDate();
    //NOTE: in the next query, the state and type are hardcoded (not using the enums) because problems when preparing query.
    qryTrans.prepare("SELECT SUM(amount),SUM(utility) from transactions where (date = :today) AND (terminalnum=:terminal) AND (type=1) AND (state=2 OR state=8 OR state=9) GROUP BY date ASC;");
    qryTrans.bindValue("today", today.toString("yyyy-MM-dd"));
    qryTrans.bindValue(":terminal", terminal);
    //tCompleted=2, tSell=1. With a placeholder, the value is inserted as a string, and cause the query to fail.
    if (!qryTrans.exec() ) {
      int errNum = qryTrans.lastError().number();
      QSqlError::ErrorType errType = qryTrans.lastError().type();
      QString errStr = qryTrans.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
      setError(details);
    } else {
      while (qryTrans.next()) {
        int fieldAmount = qryTrans.record().indexOf("SUM(amount)");
        int fieldProfit = qryTrans.record().indexOf("SUM(utility)");
        result.amount = qryTrans.value(fieldAmount).toDouble();
        result.profit = qryTrans.value(fieldProfit).toDouble();
        //qDebug()<<"APPENDING:"<<info.date<< " Sales:"<<info.amount<<" Profit:"<<info.utility;
      }
      //qDebug()<<"executed query:"<<qryTrans.executedQuery();
      //qDebug()<<"Qry size:"<<qryTrans.size();

    }
    return result;
}

AmountAndProfitInfo  Azahar::getDaySalesAndProfit()
{
  AmountAndProfitInfo result;
  QSqlQuery qryTrans(db);
  QDate today = QDate::currentDate();
  //NOTE: in the next query, the state and type are hardcoded (not using the enums) because problems when preparing query.
  qryTrans.prepare("SELECT SUM(amount),SUM(utility) from transactions where (date = :today) AND (type=1) AND (state=2 OR state=8 OR state=9) GROUP BY date ASC;");
  qryTrans.bindValue("today", today.toString("yyyy-MM-dd"));
  //tCompleted=2, tSell=1. With a placeholder, the value is inserted as a string, and cause the query to fail.
  if (!qryTrans.exec() ) {
    int errNum = qryTrans.lastError().number();
    QSqlError::ErrorType errType = qryTrans.lastError().type();
    QString errStr = qryTrans.lastError().text();
    QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
    setError(details);
  } else {
    while (qryTrans.next()) {
      int fieldAmount = qryTrans.record().indexOf("SUM(amount)");
      int fieldProfit = qryTrans.record().indexOf("SUM(utility)");
      result.amount = qryTrans.value(fieldAmount).toDouble();
      result.profit = qryTrans.value(fieldProfit).toDouble();
      //qDebug()<<"APPENDING:"<<info.date<< " Sales:"<<info.amount<<" Profit:"<<info.utility;
    }
    //qDebug()<<"executed query:"<<qryTrans.executedQuery();
    //qDebug()<<"Qry size:"<<qryTrans.size();
    
  }
  return result;
}
  
//this returns the sales and profit from the 1st day of the month until today
AmountAndProfitInfo  Azahar::getMonthSalesAndProfit()
{
  AmountAndProfitInfo result;
  QSqlQuery qryTrans(db);
  QDate today = QDate::currentDate();
  QDate startDate = QDate(today.year(), today.month(), 1); //get the 1st of the month.
  //NOTE: in the next query, the state and type are hardcoded (not using the enums) because problems when preparing query.
  qryTrans.prepare("SELECT date,SUM(amount),SUM(utility) from transactions where (date BETWEEN :dateSTART AND :dateEND) AND (type=1) AND (state=2 OR state=8 OR state=9) GROUP BY type ASC;"); //group by type is to get the sum of all trans
  qryTrans.bindValue("dateSTART", startDate.toString("yyyy-MM-dd"));
  qryTrans.bindValue("dateEND", today.toString("yyyy-MM-dd"));
  //tCompleted=2, tSell=1. With a placeholder, the value is inserted as a string, and cause the query to fail.
  if (!qryTrans.exec() ) {
    int errNum = qryTrans.lastError().number();
    QSqlError::ErrorType errType = qryTrans.lastError().type();
    QString errStr = qryTrans.lastError().text();
    QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
    setError(details);
  } else {
    while (qryTrans.next()) {
      int fieldAmount = qryTrans.record().indexOf("SUM(amount)");
      int fieldProfit = qryTrans.record().indexOf("SUM(utility)");
      result.amount = qryTrans.value(fieldAmount).toDouble();
      result.profit = qryTrans.value(fieldProfit).toDouble();
      //qDebug()<<"APPENDING --  Sales:"<<result.amount<<" Profit:"<<result.profit;
    }
    //qDebug()<<"executed query:"<<qryTrans.executedQuery();
    //qDebug()<<"Qry size:"<<qryTrans.size();
    
  }
  return result;
}

//TRANSACTIONS
qulonglong Azahar::insertTransaction(TransactionInfo info)
{
  qulonglong result=0;
  //NOTE:The next commented code was deprecated because it will cause to overwrite transactions
  //    When two or more terminals were getting an empty transacion at the same time, getting the same one.
  //first look for an empty transaction.
  //qulonglong availableId = getEmptyTransactionId();
  //if (availableId > 0 ) {
  //  qDebug()<<"The empty transaction # "<<availableId <<" is available to reuse.";
  //  info.id = availableId;
  //  if (updateTransaction(info)) result = availableId;
  //}
  //else {
    // insert a new one.
    QSqlQuery query2(db);
    query2.prepare("INSERT INTO transactions (clientid, type, amount, date,  time, paidwith, changegiven, paymethod, state, userid, cardnumber, itemcount, itemslist, cardauthnumber, utility, terminalnum, providerid, specialOrders, balanceId, totalTax, donor, note) VALUES (:clientid, :type, :amount, :date, :time, :paidwith, :changegiven, :paymethod, :state, :userid, :cardnumber, :itemcount, :itemslist, :cardauthnumber, :utility, :terminalnum, :providerid, :specialOrders, :balance, :tax, :donor, :note)"); //removed groups 29DIC09

    /** Remember to improve queries readability:
     * query2.prepare("INSERT INTO transactions ( \
    clientid, userid, type, amount, date,  time, \
    paidwith,  paymethod, changegiven, state,    \
    cardnumber, itemcount, itemslist, points, \
    discmoney, disc, discmoney, cardauthnumber, profit,  \
    terminalnum, providerid, specialOrders, balanceId, totalTax, donor, note) \
    VALUES ( \
    :clientid, :userid, :type, :amount, :date, :time, \
    :paidwith, :paymethod, :changegiven, :state,  \
    :cardnumber, :itemcount, :itemslist, :points, \
    :discmoney, :disc, :discm, :cardauthnumber, :utility, \
    :terminalnum, :providerid, :specialOrders, :balanceId, :totalTax, :donor, :note)");
    **/
    
    query2.bindValue(":type", info.type);
    query2.bindValue(":amount", info.amount);
    query2.bindValue(":date", info.date.toString("yyyy-MM-dd"));
    query2.bindValue(":time", info.time.toString("hh:mm"));
    query2.bindValue(":paidwith", info.paywith );
    query2.bindValue(":changegiven", info.changegiven);
    query2.bindValue(":paymethod", info.paymethod);
    query2.bindValue(":state", info.state);
    query2.bindValue(":userid", info.userid);
    query2.bindValue(":clientid", info.clientid);
    query2.bindValue(":cardnumber", info.cardnumber); //.replace(0,15,"***************")); //FIXED: Only save last 4 digits
    query2.bindValue(":itemcount", info.itemcount);
    query2.bindValue(":itemslist", info.itemlist);
    query2.bindValue(":cardauthnumber", info.cardauthnum);
    query2.bindValue(":utility", info.utility);
    query2.bindValue(":terminalnum", info.terminalnum);
    query2.bindValue(":providerid", info.providerid);
    query2.bindValue(":tax", info.totalTax);
    query2.bindValue(":specialOrders", info.specialOrders);
    query2.bindValue(":balance", info.balanceId);
    query2.bindValue(":donor", info.donor);
    query2.bindValue(":note", info.note);
    if (!query2.exec() ) {
      int errNum = query2.lastError().number();
      QSqlError::ErrorType errType = query2.lastError().type();
      QString errStr = query2.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
      setError(details);
    } else result=query2.lastInsertId().toULongLong();
  //}

  return result;
}

bool Azahar::updateTransaction(TransactionInfo info)
{
  bool result=false;
  QSqlQuery query2(db);
  query2.prepare("UPDATE transactions SET disc=:disc, discmoney=:discMoney, amount=:amount, date=:date,  time=:time, paidwith=:paidw, changegiven=:change, paymethod=:paymethod, state=:state, cardnumber=:cardnumber, itemcount=:itemcount, itemslist=:itemlist, cardauthnumber=:cardauthnumber, utility=:utility, terminalnum=:terminalnum, points=:points, clientid=:clientid, specialOrders=:sorders, balanceId=:balance, totalTax=:tax, donor=:donor WHERE id=:code");
  query2.bindValue(":disc", info.disc);
  query2.bindValue(":discMoney", info.discmoney);
  query2.bindValue(":code", info.id);
  query2.bindValue(":amount", info.amount);
  query2.bindValue(":date", info.date.toString("yyyy-MM-dd"));
  query2.bindValue(":time", info.time.toString("hh:mm"));
  query2.bindValue(":paidw", info.paywith );
  query2.bindValue(":change", info.changegiven);
  query2.bindValue(":paymethod", info.paymethod);
  query2.bindValue(":state", info.state);
  query2.bindValue(":cardnumber", info.cardnumber);//.replace(0,15,"***************")); //FIXED: Only save last 4 digits
  query2.bindValue(":itemcount", info.itemcount);
  query2.bindValue(":itemlist", info.itemlist);
  query2.bindValue(":cardauthnumber", info.cardauthnum);
  query2.bindValue(":utility", info.utility);
  query2.bindValue(":terminalnum", info.terminalnum);
  query2.bindValue(":points", info.points);
  query2.bindValue(":clientid", info.clientid);
  query2.bindValue(":tax", info.totalTax);
  query2.bindValue(":sorders", info.specialOrders);
  query2.bindValue(":balance", info.balanceId);
  query2.bindValue(":donor", info.donor);
  qDebug()<<"Transaction ID:"<<info.id;
  if (!query2.exec() ) {
    int errNum = query2.lastError().number();
    QSqlError::ErrorType errType = query2.lastError().type();
    QString errStr = query2.lastError().text();
    QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
    setError(details);
    qDebug()<<"DETALLES DEL ERROR:"<<details;
  } else result=true;
  
  return result;
}

bool Azahar::deleteTransaction(qulonglong id)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    QString qry = QString("DELETE FROM transactions WHERE id=%1").arg(id);
    if (!query.exec(qry)) {
      result = false;
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString errStr = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
      setError(details);
    } else {
      result = true;
    }
  }
  return result;
}


//NOTE: Is it convenient to reuse empty transactions or simply delete them?
bool Azahar::deleteEmptyTransactions()
{
   bool result = false;
   if (!db.isOpen()) db.open();
   if (db.isOpen()) {
     QSqlQuery query(db);
     QString qry = QString("DELETE FROM transactions WHERE itemcount<=0 and amount<=0");
     if (!query.exec(qry)) {
       int errNum = query.lastError().number();
       QSqlError::ErrorType errType = query.lastError().type();
       QString errStr = query.lastError().text();
       QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
       setError(details);
     } else {
       result = true;
     }
   }
   return result;
}

qulonglong Azahar::getEmptyTransactionId()
{
    qulonglong result = 0;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
     QSqlQuery query(db);
     QString qry = QString("SELECT id from transactions WHERE itemcount<=0 and amount<=0");
     if (!query.exec(qry)) {
       int errNum = query.lastError().number();
       QSqlError::ErrorType errType = query.lastError().type();
       QString errStr = query.lastError().text();
       QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
       setError(details);
     } else {
        while (query.next()) {
          int fieldId = query.record().indexOf("id");
          result      = query.value(fieldId).toULongLong();
          return result;
        }
       }
     }
     return result;
}

bool Azahar::cancelTransaction(qulonglong id, bool inProgress)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  bool ok = db.isOpen();
  
  TransactionInfo tinfo = getTransactionInfo(id);
  bool transCompleted = false;
  bool alreadyCancelled = false;
  bool transExists = false;
  if (tinfo.id    >  0)          transExists      = true;
  if (tinfo.state == tCompleted && transExists) transCompleted   = true;
  if (tinfo.state == tCancelled && transExists) alreadyCancelled = true;
  
  
  if (ok) {
    QSqlQuery query(db);
    QString qry;
    
    if (!inProgress && !alreadyCancelled && transExists) {
      qry = QString("UPDATE transactions SET  state=%1 WHERE id=%2")
      .arg(tCancelled)
      .arg(id);
      if (!query.exec(qry)) {
        int errNum = query.lastError().number();
        QSqlError::ErrorType errType = query.lastError().type();
        QString errStr = query.lastError().text();
        QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
        setError(details);
      } else { //Cancelled...
        result = true;
        qDebug()<<"Marked as Cancelled!";
      }
      ///not in progress, it means stockqty,points... are affected.
      if (transCompleted) {
        //TODO: when cancelling a transacion, take into account the groups sold to be returned. new feature
        QStringList soProducts;
        ///if there is any special order (product)
        if ( !tinfo.specialOrders.isEmpty() ) {
          //get each special order
          QStringList pSoList = tinfo.specialOrders.split(",");
          for (int i = 0; i < pSoList.size(); ++i) {
            QStringList l = pSoList.at(i).split("/");
            if ( l.count()==2 ) { //==2 means its complete, having product and qty
              qulonglong soid = l.at(0).toULongLong();
              //set as cancelled
              specialOrderSetStatus(soid, 4); //4 == cancelled
              //get each product of the special order to increment its stock later
              soProducts.append( getSpecialOrderProductsStr(soid) ); //are normal products (raw or not)
            } //if count
          } //for
        }//if there are special orders
        QString soProductsStr = soProducts.join(",");
        ///increment stock for each product. including special orders and groups
        QStringList plist = (tinfo.itemlist.split(",") + soProductsStr.split(","));
        qDebug()<<"[*****] plist: "<< plist;
        for (int i = 0; i < plist.size(); ++i) {
          QStringList l = plist.at(i).split("/");
          if ( l.count()==2 ) { //==2 means its complete, having product and qty
            //check if the product is a group
            //NOTE: rawProducts ? affect stock when cancelling = YES but only if affected when sold one of its parents (specialOrders) and stockqty is set. But they would not be here, if not at specialOrders List
            ProductInfo pi = getProductInfo(l.at(0));
            if ( pi.isAGroup ) 
                incrementGroupStock(l.at(0), l.at(1).toDouble()); //code at 0, qty at 1
            else //there is a normal product
                incrementProductStock(l.at(0), l.at(1).toDouble()); //code at 0, qty at 1
          }
        }//for each product
        ///save cashout for the money return
        qDebug()<<"Saving cashout-cancel";
        CashFlowInfo cinfo;
        cinfo.userid = tinfo.userid;
        cinfo.amount = tinfo.amount;
        cinfo.date   = QDate::currentDate();
        cinfo.time   = QTime::currentTime();
        cinfo.terminalNum = tinfo.terminalnum;
        cinfo.type   = ctCashOutMoneyReturnOnCancel;
        cinfo.reason = i18n("Money return on cancelling ticket #%1 ", id);
        insertCashFlow(cinfo);
      }//transCompleted
    } //not in progress
  }
  if ( alreadyCancelled ) {
    //The transaction was already canceled
    setError(i18n("Ticket #%1 was already canceled.", id));
    result = false;
    qDebug()<<"Transaction already cancelled...";
  }
  return result;
}



QList<TransactionInfo> Azahar::getLastTransactions(int pageNumber,int numItems,QDate beforeDate)
{
  QList<TransactionInfo> result;
  result.clear();
  QSqlQuery query(db);
  QString qry;
  qry = QString("SELECT * from transactions where type=1 and  date <= STR_TO_DATE('%1', '%d/%m/%Y') order by date desc, id desc LIMIT %2,%3").arg(beforeDate.toString("dd/MM/yyyy")).arg((pageNumber-1)*numItems+1).arg(numItems);
  if (query.exec(qry)) {
    while (query.next()) {
      TransactionInfo info;
      int fieldId = query.record().indexOf("id");
      int fieldAmount = query.record().indexOf("amount");
      int fieldDate   = query.record().indexOf("date");
      int fieldTime   = query.record().indexOf("time");
      int fieldPaidWith = query.record().indexOf("paidwith");
      int fieldPayMethod = query.record().indexOf("paymethod");
      int fieldType      = query.record().indexOf("type");
      int fieldChange    = query.record().indexOf("changegiven");
      int fieldState     = query.record().indexOf("state");
      int fieldUserId    = query.record().indexOf("userid");
      int fieldClientId  = query.record().indexOf("clientid");
      int fieldCardNum   = query.record().indexOf("cardnumber");
      int fieldCardAuth  = query.record().indexOf("cardauthnumber");
      int fieldItemCount = query.record().indexOf("itemcount");
      int fieldItemsList = query.record().indexOf("itemsList");
      int fieldDiscount  = query.record().indexOf("disc");
      int fieldDiscMoney = query.record().indexOf("discmoney");
      int fieldPoints    = query.record().indexOf("points");
      int fieldUtility   = query.record().indexOf("utility");
      int fieldTerminal  = query.record().indexOf("terminalnum");
      int fieldTax    = query.record().indexOf("totalTax");
      int fieldSOrd      = query.record().indexOf("specialOrders");
      int fieldBalance   = query.record().indexOf("balanceId");
      int fieldDonor   = query.record().indexOf("donor");
      info.id     = query.value(fieldId).toULongLong();
      info.amount = query.value(fieldAmount).toDouble();
      info.date   = query.value(fieldDate).toDate();
      info.time   = query.value(fieldTime).toTime();
      info.paywith= query.value(fieldPaidWith).toDouble();
      info.paymethod = query.value(fieldPayMethod).toInt();
      info.type      = query.value(fieldType).toInt();
      info.changegiven = query.value(fieldChange).toDouble();
      info.state     = query.value(fieldState).toInt();
      info.userid    = query.value(fieldUserId).toULongLong();
      info.clientid  = query.value(fieldClientId).toULongLong();
      info.cardnumber= query.value(fieldCardNum).toString();//.replace(0,15,"***************"); //FIXED: Only save last 4 digits;
      info.cardauthnum=query.value(fieldCardAuth).toString();
      info.itemcount = query.value(fieldItemCount).toInt();
      info.itemlist  = query.value(fieldItemsList).toString();
      info.disc      = query.value(fieldDiscount).toDouble();
      info.discmoney = query.value(fieldDiscMoney).toDouble();
      info.points    = query.value(fieldPoints).toULongLong();
      info.utility   = query.value(fieldUtility).toDouble();
      info.terminalnum=query.value(fieldTerminal).toInt();
      info.totalTax  = query.value(fieldTax).toDouble();
      info.specialOrders  = query.value(fieldSOrd).toString();
      info.balanceId = query.value(fieldBalance).toULongLong();
      info.donor  = query.value(fieldDonor).toString();
      result.append(info);
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}

//NOTE: The next method is not used... Also, for what pourpose? is it missing the STATE condition?
QList<TransactionInfo> Azahar::getTransactionsPerDay(int pageNumber,int numItems, QDate beforeDate)
{
  QList<TransactionInfo> result;
  result.clear();
  QSqlQuery query(db);
  QString qry;
  qry = QString("SELECT date, count(1) as transactions, sum(itemcount) as itemcount, sum(amount) as amount FROM transactions WHERE TYPE =1 AND date <= STR_TO_DATE('%1', '%d/%m/%Y') GROUP BY date(DATE) ORDER BY date DESC LIMIT %2,%3").arg(beforeDate.toString("dd/MM/yyyy")).arg((pageNumber-1)*numItems+1).arg(numItems);
  if (query.exec(qry)) {
    while (query.next()) {
      TransactionInfo info;
      int fieldTransactions = query.record().indexOf("transactions");
      int fieldAmount = query.record().indexOf("amount");
      int fieldDate   = query.record().indexOf("date");
      int fieldItemCount   = query.record().indexOf("itemcount");
      info.amount = query.value(fieldAmount).toDouble();
      info.date   = query.value(fieldDate).toDate();
      info.state     = query.value(fieldTransactions).toInt();
      info.itemcount = query.value(fieldItemCount).toInt();
      result.append(info);
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}

double Azahar::getTransactionDiscMoney(qulonglong id)
{
  double result = 0;
  QSqlQuery query(db);
  QString qry;
  qry = QString("SELECT discMoney FROM transactions WHERE id=%1").arg(id);
  if (query.exec(qry)) {
    while (query.next()) {
      int fieldAmount = query.record().indexOf("discMoney");
      result = query.value(fieldAmount).toDouble();
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}


bool Azahar::setTransactionStatus(qulonglong trId, TransactionState state)
{
    bool result = false;
    if (!db.isOpen()) db.open();
    if (db.isOpen())
    {
        QSqlQuery query(db);
        query.prepare("UPDATE transactions SET transactions.state=:state WHERE transactions.id=:trid");
        query.bindValue(":trid", trId);
        query.bindValue(":state", state);

        qDebug()<< __FUNCTION__ << "Tr Id:"<<trId<<" State:"<<state;
        
        if (!query.exec() ) {
            int errNum = query.lastError().number();
            QSqlError::ErrorType errType = query.lastError().type();
            QString errStr = query.lastError().text();
            QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
            setError(details);
        } else result = true;
    }
    return result;
}


// TRANSACTIONITEMS
bool Azahar::insertTransactionItem(TransactionItemInfo info)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO transactionitems (transaction_id, position, product_id, qty, points, unitstr, cost, price, disc, total, name, payment, completePayment, soId, isGroup, deliveryDateTime, tax) VALUES(:transactionid, :position, :productCode, :qty, :points, :unitStr, :cost, :price, :disc, :total, :name, :payment, :completeP, :soid, :isGroup, :deliveryDT, :tax)");
    query.bindValue(":transactionid", info.transactionid);
    query.bindValue(":position", info.position);
    query.bindValue(":productCode", info.productCode);
    query.bindValue(":qty", info.qty);
    query.bindValue(":points", info.points);
    query.bindValue(":unitStr", info.unitStr);
    query.bindValue(":cost", info.cost);
    query.bindValue(":price", info.price);
    query.bindValue(":disc", info.disc);
    query.bindValue(":total", info.total);
    query.bindValue(":name", info.name);
    query.bindValue(":payment", info.payment);
    query.bindValue(":completeP", info.completePayment);
    query.bindValue(":soid", info.soId);
    query.bindValue(":isGroup", info.isGroup);
    query.bindValue(":deliveryDT", info.deliveryDateTime);
    query.bindValue(":tax", info.tax);
    if (!query.exec()) {
      setError(query.lastError().text());
      qDebug()<<"Insert TransactionItems error:"<<query.lastError().text();
    } else result = true;
  }
  return result;
}

bool Azahar::deleteAllTransactionItem(qulonglong id)
{
  bool result=false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    QString qry = QString("DELETE FROM transactionitems WHERE transaction_id=%1").arg(id);
    if (!query.exec(qry)) {
      result = false;
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString errStr = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
      setError(details);
    } else {
      result = true;
    }
  }
  return result;
}

QList<TransactionItemInfo> Azahar::getTransactionItems(qulonglong id)
{
  QList<TransactionItemInfo> result;
  result.clear();
  QSqlQuery query(db);
  QString qry = QString("SELECT * FROM transactionitems WHERE transaction_id=%1 ORDER BY POSITION").arg(id);
  if (query.exec(qry)) {
    while (query.next()) {
      TransactionItemInfo info;
      int fieldPosition = query.record().indexOf("position");
      int fieldProductCode   = query.record().indexOf("product_id");
      int fieldQty     = query.record().indexOf("qty");
      int fieldPoints  = query.record().indexOf("points");
      int fieldCost    = query.record().indexOf("cost");
      int fieldPrice   = query.record().indexOf("price");
      int fieldDisc    = query.record().indexOf("disc");
      int fieldTotal   = query.record().indexOf("total");
      int fieldName    = query.record().indexOf("name");
      int fieldUStr    = query.record().indexOf("unitstr");
      int fieldPayment = query.record().indexOf("payment");
      int fieldCPayment = query.record().indexOf("completePayment");
      int fieldSoid = query.record().indexOf("soId");
      int fieldIsG = query.record().indexOf("isGroup");
      int fieldDDT = query.record().indexOf("deliveryDateTime");
      int fieldTax = query.record().indexOf("tax");
      
      info.transactionid     = id;
      info.position      = query.value(fieldPosition).toInt();
      info.productCode   = query.value(fieldProductCode).toULongLong();
      info.qty           = query.value(fieldQty).toDouble();
      info.points        = query.value(fieldPoints).toDouble();
      info.unitStr       = query.value(fieldUStr).toString();
      info.cost          = query.value(fieldCost).toDouble();
      info.price         = query.value(fieldPrice).toDouble();
      info.disc          = query.value(fieldDisc).toDouble();
      info.total         = query.value(fieldTotal).toDouble();
      info.name          = query.value(fieldName).toString();
      info.payment       = query.value(fieldPayment).toDouble();
      info.completePayment  = query.value(fieldCPayment).toBool();
      info.soId          = query.value(fieldSoid).toString();
      info.isGroup       = query.value(fieldIsG).toBool();
      info.deliveryDateTime=query.value(fieldDDT).toDateTime();
      info.tax           = query.value(fieldTax).toDouble();
      result.append(info);
    }
  }
  else {
    setError(query.lastError().text());
    qDebug()<<"Get TransactionItems error:"<<query.lastError().text();
  }
  return result;
}

//BALANCES

double Azahar::getClientCredit(ClientInfo clientInfo, double totalSum)
{
    CreditInfo credit = getCreditInfoForClient(clientInfo.id);
    double paid=clientInfo.monthly-credit.total;
   // qDebug()<<"PAID (credit.total):"<<paid;
           //<<"TOTSUM:"<<totalSum;
    double change = paid - totalSum;
    if (paid <= 0) change = 0.0;
    return change;
}

qulonglong Azahar::insertBalance(BalanceInfo info)
{
  qulonglong result =0;
  if (!db.isOpen()) db.open();
  if (db.isOpen())
  {
    QSqlQuery queryBalance(db);
    queryBalance.prepare("INSERT INTO balances (balances.datetime_start, balances.datetime_end, balances.userid, balances.usern, balances.initamount, balances.in, balances.out, balances.cash, balances.card, balances.transactions, balances.terminalnum, balances.cashflows, balances.done) VALUES (:date_start, :date_end, :userid, :user, :initA, :in, :out, :cash, :card, :transactions, :terminalNum, :cashflows, :isDone)");
    queryBalance.bindValue(":date_start", info.dateTimeStart.toString("yyyy-MM-dd hh:mm:ss"));
    queryBalance.bindValue(":date_end", info.dateTimeEnd.toString("yyyy-MM-dd hh:mm:ss"));
    queryBalance.bindValue(":userid", info.userid);
    queryBalance.bindValue(":user", info.username);
    queryBalance.bindValue(":initA", info.initamount);
    queryBalance.bindValue(":in", info.in);
    queryBalance.bindValue(":out", info.out);
    queryBalance.bindValue(":cash", info.cash);
    queryBalance.bindValue(":card", info.card);
    queryBalance.bindValue(":transactions", info.transactions);
    queryBalance.bindValue(":terminalNum", info.terminal);
    queryBalance.bindValue(":cashflows", info.cashflows);
    queryBalance.bindValue(":isDone", info.done);

    if (!queryBalance.exec() ) {
      int errNum = queryBalance.lastError().number();
      QSqlError::ErrorType errType = queryBalance.lastError().type();
      QString errStr = queryBalance.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
      setError(details);
    } else result = queryBalance.lastInsertId().toULongLong();
  }
  return result;
}

BalanceInfo Azahar::getBalanceInfo(qulonglong id)
{
  BalanceInfo info;
  info.id = 0;
  QString qry = QString("SELECT * FROM balances WHERE id=%1").arg(id);
  QSqlQuery query;
  if (!query.exec(qry)) { qDebug()<<query.lastError(); }
  else {
    while (query.next()) {
      int fieldId = query.record().indexOf("id");
      int fieldDtStart = query.record().indexOf("datetime_start");
      int fieldDtEnd   = query.record().indexOf("datetime_end");
      int fieldUserId  = query.record().indexOf("userid");
      int fieldUsername= query.record().indexOf("usern");
      int fieldInitAmount = query.record().indexOf("initamount");
      int fieldIn      = query.record().indexOf("in");
      int fieldOut     = query.record().indexOf("out");
      int fieldCash    = query.record().indexOf("cash");
      int fieldTransactions    = query.record().indexOf("transactions");
      int fieldCard    = query.record().indexOf("card");
      int fieldTerminalNum   = query.record().indexOf("terminalnum");
      int fieldCashFlows     = query.record().indexOf("cashflows");
      int fieldDone    = query.record().indexOf("done");
      info.id     = query.value(fieldId).toULongLong();
      info.dateTimeStart = query.value(fieldDtStart).toDateTime();
      info.dateTimeEnd   = query.value(fieldDtEnd).toDateTime();
      info.userid   = query.value(fieldUserId).toULongLong();
      info.username= query.value(fieldUsername).toString();
      info.initamount = query.value(fieldInitAmount).toDouble();
      info.in      = query.value(fieldIn).toDouble();
      info.out = query.value(fieldOut).toDouble();
      info.cash   = query.value(fieldCash).toDouble();
      info.card   = query.value(fieldCard).toDouble();
      info.transactions= query.value(fieldTransactions).toString();
      info.terminal = query.value(fieldTerminalNum).toInt();
      info.cashflows= query.value(fieldCashFlows).toString();
      info.done = query.value(fieldDone).toBool();
    }
  }
  return info;
}

bool Azahar::updateBalance(BalanceInfo info)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen())
  {
    QSqlQuery queryBalance(db);
    queryBalance.prepare("UPDATE balances SET balances.datetime_start=:date_start, balances.datetime_end=:date_end, balances.userid=:userid, balances.usern=:user, balances.initamount=:initA, balances.in=:in, balances.out=:out, balances.cash=:cash, balances.card=:card, balances.transactions=:transactions, balances.terminalnum=:terminalNum, cashflows=:cashflows, done=:isDone WHERE balances.id=:bid");
    queryBalance.bindValue(":date_start", info.dateTimeStart.toString("yyyy-MM-dd hh:mm:ss"));
    queryBalance.bindValue(":date_end", info.dateTimeEnd.toString("yyyy-MM-dd hh:mm:ss"));
    queryBalance.bindValue(":userid", info.userid);
    queryBalance.bindValue(":user", info.username);
    queryBalance.bindValue(":initA", info.initamount);
    queryBalance.bindValue(":in", info.in);
    queryBalance.bindValue(":out", info.out);
    queryBalance.bindValue(":cash", info.cash);
    queryBalance.bindValue(":card", info.card);
    queryBalance.bindValue(":transactions", info.transactions);
    queryBalance.bindValue(":terminalNum", info.terminal);
    queryBalance.bindValue(":cashflows", info.cashflows);
    queryBalance.bindValue(":bid", info.id);
    queryBalance.bindValue(":isDone", info.done);
    
    if (!queryBalance.exec() ) {
      int errNum = queryBalance.lastError().number();
      QSqlError::ErrorType errType = queryBalance.lastError().type();
      QString errStr = queryBalance.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
      setError(details);
    } else result = true;
  }
  return result;
}

qulonglong Azahar::insertCashFlow(CashFlowInfo info)
{
  qulonglong result =0;
  if (!db.isOpen()) db.open();
  if (db.isOpen())
  {
    QSqlQuery query(db);
    query.prepare("INSERT INTO cashflow ( cashflow.userid, cashflow.type, cashflow.reason, cashflow.amount, cashflow.date, cashflow.time, cashflow.terminalnum) VALUES (:userid, :type, :reason, :amount, :date, :time,  :terminalNum)");
    query.bindValue(":date", info.date.toString("yyyy-MM-dd"));
    query.bindValue(":time", info.time.toString("hh:mm:ss"));
    query.bindValue(":userid", info.userid);
    query.bindValue(":terminalNum", info.terminalNum);
    query.bindValue(":reason", info.reason);
    query.bindValue(":amount", info.amount);
    query.bindValue(":type", info.type);
    
    if (!query.exec() ) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString errStr = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),errStr);
      setError(details);
    } else result = query.lastInsertId().toULongLong();
  }
  return result;
}

QList<CashFlowInfo> Azahar::getCashFlowInfoList(const QList<qulonglong> &idList)
{
  QList<CashFlowInfo> result;
  result.clear();
  if (idList.count() == 0) return result;
  QSqlQuery query(db);

  foreach(qulonglong currId, idList) {
    QString qry = QString(" \
    SELECT CF.id as id, \
    CF.type as type, \
    CF.userid as userid, \
    CF.amount as amount, \
    CF.reason as reason, \
    CF.date as date, \
    CF.time as time, \
    CF.terminalNum as terminalNum, \
    CFT.text as typeStr \
    FROM cashflow AS CF, cashflowtypes AS CFT \
    WHERE id=%1 AND (CFT.typeid = CF.type) ORDER BY CF.id;").arg(currId);
    if (query.exec(qry)) {
      while (query.next()) {
        CashFlowInfo info;
        int fieldId      = query.record().indexOf("id");
        int fieldType    = query.record().indexOf("type");
        int fieldUserId  = query.record().indexOf("userid");
        int fieldAmount  = query.record().indexOf("amount");
        int fieldReason  = query.record().indexOf("reason");
        int fieldDate    = query.record().indexOf("date");
        int fieldTime    = query.record().indexOf("time");
        int fieldTermNum = query.record().indexOf("terminalNum");
        int fieldTStr    = query.record().indexOf("typeStr");

        info.id          = query.value(fieldId).toULongLong();
        info.type        = query.value(fieldType).toULongLong();
        info.userid      = query.value(fieldUserId).toULongLong();
        info.amount      = query.value(fieldAmount).toDouble();
        info.reason      = query.value(fieldReason).toString();
        info.typeStr     = query.value(fieldTStr).toString();
        info.date        = query.value(fieldDate).toDate();
        info.time        = query.value(fieldTime).toTime();
        info.terminalNum = query.value(fieldTermNum).toULongLong();
        result.append(info);
        qDebug()<<__FUNCTION__<<" Cash Flow:"<<info.id<<" type:"<<info.type<<" reason:"<<info.reason;
      }
    }
    else {
      setError(query.lastError().text());
    }
  } //foreach
  return result;
}

QList<CashFlowInfo> Azahar::getCashFlowInfoList(const QDateTime &start, const QDateTime &end)
{
  QList<CashFlowInfo> result;
  result.clear();
  QSqlQuery query(db);

  query.prepare(" \
  SELECT CF.id as id, \
  CF.type as type, \
  CF.userid as userid, \
  CF.amount as amount, \
  CF.reason as reason, \
  CF.date as date, \
  CF.time as time, \
  CF.terminalNum as terminalNum, \
  CFT.text as typeStr \
  FROM cashflow AS CF, cashflowtypes AS CFT \
  WHERE (date BETWEEN :dateSTART AND :dateEND) AND (CFT.typeid = CF.type) ORDER BY CF.id");
  query.bindValue(":dateSTART", start.date());
  query.bindValue(":dateEND", end.date());
  if (query.exec()) {
    while (query.next()) {
      QTime ttime = query.value(query.record().indexOf("time")).toTime();
      if ( (ttime >= start.time()) &&  (ttime <= end.time()) ) {
        //its inside the requested time period.
        CashFlowInfo info;
        int fieldId      = query.record().indexOf("id");
        int fieldType    = query.record().indexOf("type");
        int fieldUserId  = query.record().indexOf("userid");
        int fieldAmount  = query.record().indexOf("amount");
        int fieldReason  = query.record().indexOf("reason");
        int fieldDate    = query.record().indexOf("date");
        int fieldTime    = query.record().indexOf("time");
        int fieldTermNum = query.record().indexOf("terminalNum");
        int fieldTStr    = query.record().indexOf("typeStr");
        
        info.id          = query.value(fieldId).toULongLong();
        info.type        = query.value(fieldType).toULongLong();
        info.userid      = query.value(fieldUserId).toULongLong();
        info.amount      = query.value(fieldAmount).toDouble();
        info.reason      = query.value(fieldReason).toString();
        info.typeStr     = query.value(fieldTStr).toString();
        info.date        = query.value(fieldDate).toDate();
        info.time        = query.value(fieldTime).toTime();
        info.terminalNum = query.value(fieldTermNum).toULongLong();
        result.append(info);
      } //if time...
    } //while
  }// if query
  else {
    setError(query.lastError().text());
  }
  return result;
}

//TransactionTypes
QString  Azahar::getPayTypeStr(qulonglong type)
{
  QString result;
  QSqlQuery query(db);
  QString qstr = QString("select text from paytypes where paytypes.typeid=%1;").arg(type);
  if (query.exec(qstr)) {
    while (query.next()) {
      int fieldText = query.record().indexOf("text");
      result = query.value(fieldText).toString();
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}

qulonglong  Azahar::getPayTypeId(QString type)
{
  qulonglong result=0;
  QSqlQuery query(db);
  QString qstr = QString("select typeid from paytypes where paytypes.text='%1';").arg(type);
  if (query.exec(qstr)) {
    while (query.next()) {
      int fieldText = query.record().indexOf("typeid");
      result = query.value(fieldText).toULongLong();
    }
  }
  else {
    setError(query.lastError().text());
  }
  return result;
}


//LOGS

bool Azahar::insertLog(const qulonglong &userid, const QDate &date, const QTime &time, const QString actionStr)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO logs (userid, date, time, action) VALUES(:userid, :date, :time, :action);");
    query.bindValue(":userid", userid);
    query.bindValue(":date", date.toString("yyyy-MM-dd"));
    query.bindValue(":time", time.toString("hh:mm"));
    query.bindValue(":action", actionStr);
    if (!query.exec()) {
      setError(query.lastError().text());
      qDebug()<<"ERROR ON SAVING LOG:"<<query.lastError().text();
    } else result = true;
  }
  return result;
}

bool Azahar::getConfigFirstRun()
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select firstrun from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("firstrun");
        QString value = myQuery.value(fieldText).toString();
        qDebug()<<"firstRun VALUE="<<value;
        if (value == "yes, it is February 6 1978")
          result = true;
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

bool Azahar::getConfigTaxIsIncludedInPrice()
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select taxIsIncludedInPrice from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("taxIsIncludedInPrice");
        result = myQuery.value(fieldText).toBool();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

void Azahar::cleanConfigFirstRun()
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update config set firstrun='yes, i like the rainy days';"))) {
      qDebug()<<"Change config firstRun...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

void Azahar::setConfigTaxIsIncludedInPrice(bool option)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update config set taxIsIncludedInPrice=%1;").arg(option))) {
      qDebug()<<"Change config taxIsIncludedInPrice...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

QPixmap  Azahar::getConfigStoreLogo()
{
  QPixmap result;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select storeLogo from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("storeLogo");
        result.loadFromData(myQuery.value(fieldText).toByteArray());
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

QString  Azahar::getConfigStoreName()
{
  QString result;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select storeName from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("storeName");
        result = myQuery.value(fieldText).toString();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

QString  Azahar::getConfigStoreAddress()
{
  QString result;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select storeAddress from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("storeAddress");
        result = myQuery.value(fieldText).toString();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

QString  Azahar::getConfigStorePhone()
{
  QString result;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select storePhone from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("storePhone");
        result = myQuery.value(fieldText).toString();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

bool     Azahar::getConfigSmallPrint()
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select smallPrint from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("smallPrint");
        result = myQuery.value(fieldText).toBool();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

bool     Azahar::getConfigLogoOnTop()
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select logoOnTop from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("logoOnTop");
        result = myQuery.value(fieldText).toBool();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

bool     Azahar::getConfigUseCUPS()
{
  bool result = false;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select useCUPS from config;"))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("useCUPS");
        result = myQuery.value(fieldText).toBool();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

QString     Azahar::getConfigDbVersion()
{
    QString result = "";
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery myQuery(db);
        if (myQuery.exec(QString("select db_version from config;"))) {
            while (myQuery.next()) {
                int fieldText = myQuery.record().indexOf("db_version");
                result = myQuery.value(fieldText).toString();
            }
        }
        else {
            qDebug()<<"ERROR: "<<myQuery.lastError();
        }
    }
    return result;
}


void   Azahar::setConfigStoreLogo(const QByteArray &baPhoto)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    myQuery.prepare("update config set storeLogo=:logo;");
    myQuery.bindValue(":logo", baPhoto);
    if (myQuery.exec()) {
      qDebug()<<"Change config store logo...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

void   Azahar::setConfigStoreName(const QString &str)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update config set storeName='%1';").arg(str))) {
      qDebug()<<"Change config storeName...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

void   Azahar::setConfigStoreAddress(const QString &str)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update config set storeAddress='%1';").arg(str))) {
      qDebug()<<"Change config storeAddress...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

void   Azahar::setConfigStorePhone(const QString &str)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update config set storePhone='%1';").arg(str))) {
      qDebug()<<"Change config taxIsIncludedInPrice...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

void   Azahar::setConfigSmallPrint(bool yes)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update config set smallPrint=%1;").arg(yes))) {
      qDebug()<<"Change config SmallPrint...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

void   Azahar::setConfigUseCUPS(bool yes)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update config set useCUPS=%1;").arg(yes))) {
      qDebug()<<"Change config useCUPS...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

void   Azahar::setConfigLogoOnTop(bool yes)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update config set logoOnTop=%1;").arg(yes))) {
      qDebug()<<"Change config logoOnTop...";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}


//SPECIAL ORDERS
QList<SpecialOrderInfo> Azahar::getAllSOforSale(qulonglong saleId)
{
  QList<SpecialOrderInfo> list;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QString qry = QString("SELECT orderid,saleid from special_orders where saleid=%1").arg(saleId);
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
      setError(i18n("Error getting special Order information, id: %1, Rows affected: %2", saleId,query.size()));
    }
    else {
      while (query.next()) {
        int fieldId  = query.record().indexOf("orderid");
        qulonglong num = query.value(fieldId).toULongLong();
        SpecialOrderInfo soInfo = getSpecialOrderInfo(num);
        list.append(soInfo);
      }
    }
  }
  return list;
}

//NOTE: Here the question is, what status to take into account? pending,inprogress,ready...
//      We will return all with status < 3 .
QList<SpecialOrderInfo> Azahar::getAllReadySOforSale(qulonglong saleId)
{
  QList<SpecialOrderInfo> list;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QString qry = QString("SELECT orderid,saleid from special_orders where saleid=%1 and status<3").arg(saleId);
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
      setError(i18n("Error getting special Order information, id: %1, Rows affected: %2", saleId,query.size()));
    }
    else {
      while (query.next()) {
        int fieldId  = query.record().indexOf("orderid");
        qulonglong num = query.value(fieldId).toULongLong();
        SpecialOrderInfo soInfo = getSpecialOrderInfo(num);
        list.append(soInfo);
      }
    }
  }
  return list;
}

int  Azahar::getReadySOCountforSale(qulonglong saleId)
{
  int count=0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QString qry = QString("SELECT orderid from special_orders where saleid=%1 and status<3").arg(saleId);
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
      setError(i18n("Error getting special Order information, id: %1, Rows affected: %2", saleId,query.size()));
    }
    else {
//       while (query.next()) {
//         count++;
//       }
    count = query.size();
    }
  }
  return count;
}

void Azahar::specialOrderSetStatus(qulonglong id, int status)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update special_orders set status=%1 where orderid=%2;").arg(status).arg(id))) {
      qDebug()<<"Status Order updated";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

void Azahar::soTicketSetStatus(qulonglong ticketId, int status)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update special_orders set status=%1 where saleid=%2;").arg(status).arg(ticketId))) {
      qDebug()<<"Status Order updated";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

QString Azahar::getSpecialOrderProductsStr(qulonglong id)
{
  QString result = "";
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select groupElements from special_orders where orderid=%1;").arg(id))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("groupElements");
        result = myQuery.value(fieldText).toString();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

QList<ProductInfo> Azahar::getSpecialOrderProductsList(qulonglong id)
{
  QList<ProductInfo> pList;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QString ge = getSpecialOrderProductsStr(id);
    QStringList pq = ge.split(",");
    foreach(QString str, pq) {
      qulonglong c = str.section('/',0,0).toULongLong();
      double     q = str.section('/',1,1).toDouble();
      //get info
      ProductInfo pi = getProductInfo(QString::number(c));
      pi.qtyOnList = q;
      pList.append(pi);
    }
  }
  return pList;
}

QString  Azahar::getSONotes(qulonglong id)
{
  QString result = "";
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select notes from special_orders where orderid=%1;").arg(id))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("notes");
        result = myQuery.value(fieldText).toString();
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

qulonglong Azahar::insertSpecialOrder(SpecialOrderInfo info)
{
  qulonglong result = 0;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query.prepare("INSERT INTO special_orders (name, price, qty, cost, units, groupElements, status, saleid, notes, payment, completePayment, dateTime, deliveryDateTime,clientId,userId) VALUES (:name, :price, :qty, :cost, :units,  :groupE, :status, :saleId, :notes, :payment, :completeP, :dateTime, :deliveryDT, :client, :user);");
  query.bindValue(":name", info.name);
  query.bindValue(":price", info.price);
  query.bindValue(":qty", info.qty);
  query.bindValue(":cost", info.cost);
  query.bindValue(":status", info.status);
  query.bindValue(":units", info.units);
  query.bindValue(":groupE", info.groupElements);
  query.bindValue(":saleId", info.saleid);
  query.bindValue(":notes", info.notes);
  query.bindValue(":payment", info.payment);
  query.bindValue(":completeP", info.completePayment);
  query.bindValue(":dateTime", info.dateTime);
  query.bindValue(":deliveryDT", info.deliveryDateTime);
  query.bindValue(":user", info.userId);
  query.bindValue(":client", info.clientId);
  
  if (!query.exec()) setError(query.lastError().text()); else {
    result=query.lastInsertId().toULongLong();
  }
  
  return result;
}

//saleid, dateTime/userid/clientid are not updated.
bool Azahar::updateSpecialOrder(SpecialOrderInfo info)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query.prepare("UPDATE special_orders SET  name=:name, price=:price, qty=:qty, cost=:cost, units=:units,  groupElements=:groupE, status=:status, notes=:notes, payment=:payment, completePayment=:completeP, deliveryDateTime=:deliveryDT WHERE orderid=:code;");
  query.bindValue(":code", info.orderid);
  query.bindValue(":name", info.name);
  query.bindValue(":price", info.price);
  query.bindValue(":qty", info.qty);
  query.bindValue(":cost", info.cost);
  query.bindValue(":status", info.status);
  query.bindValue(":units", info.units);
  query.bindValue(":groupE", info.groupElements);
  query.bindValue(":notes", info.notes);
  query.bindValue(":payment", info.payment);
  query.bindValue(":completeP", info.completePayment);
  query.bindValue(":deliveryDT", info.deliveryDateTime);
  
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}

bool Azahar::decrementSOStock(qulonglong id, double qty, QDate date)
{
  bool result = true;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);

  QList<QString> lelem = getSpecialOrderProductsStr(id).split(",");
  foreach(QString ea, lelem) {
    QString c = ea.section('/',0,0);
    double     q = ea.section('/',1,1).toDouble();
    //FOR EACH ELEMENT, DECREMENT PRODUCT STOCK
    result = result && decrementProductStock(c, q*qty, date);
  }
  
  return result;
}

bool Azahar::deleteSpecialOrder(qulonglong id)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query = QString("DELETE FROM special_orders WHERE orderid=%1").arg(id);
  if (!query.exec()) setError(query.lastError().text()); else result=true;
  return result;
}

SpecialOrderInfo Azahar::getSpecialOrderInfo(qulonglong id)
{
  SpecialOrderInfo info;
  info.orderid=0;
  info.name="Ninguno";
  info.price=0;

  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QString qry = QString("SELECT * from special_orders where orderid=%1").arg(id);
    QSqlQuery query(db);
    if (!query.exec(qry)) {
      int errNum = query.lastError().number();
      QSqlError::ErrorType errType = query.lastError().type();
      QString error = query.lastError().text();
      QString details = i18n("Error #%1, Type:%2\n'%3'",QString::number(errNum), QString::number(errType),error);
      setError(i18n("Error getting special Order information, id: %1, Rows affected: %2", id,query.size()));
    }
    else {
      while (query.next()) {
        int fieldDesc = query.record().indexOf("name");
        int fieldPrice= query.record().indexOf("price");
        int fieldQty= query.record().indexOf("qty");
        int fieldCost= query.record().indexOf("cost");
        int fieldUnits= query.record().indexOf("units");
        int fieldStatus= query.record().indexOf("status");
        int fieldSaleId= query.record().indexOf("saleid");
        int fieldGroupE = query.record().indexOf("groupElements");
        int fieldPayment = query.record().indexOf("payment");
        int fieldCPayment = query.record().indexOf("completePayment");
        int fieldDateT   = query.record().indexOf("dateTime");
        int fieldDDT     = query.record().indexOf("deliveryDateTime");
        int fieldNotes   = query.record().indexOf("notes");
        int fieldClientId = query.record().indexOf("clientId");
        int fieldUserId = query.record().indexOf("userId");
        
        info.orderid=id;
        info.name      = query.value(fieldDesc).toString();
        info.price    = query.value(fieldPrice).toDouble();
        info.qty      = query.value(fieldQty).toDouble();
        info.cost     = query.value(fieldCost).toDouble();
        info.units    = query.value(fieldUnits).toInt();
        info.status   = query.value(fieldStatus).toInt();
        info.saleid   = query.value(fieldSaleId).toULongLong();
        info.groupElements = query.value(fieldGroupE).toString();
        info.payment  = query.value(fieldPayment).toDouble();
        info.completePayment  = query.value(fieldCPayment).toBool();
        info.dateTime = query.value(fieldDateT).toDateTime();
        info.deliveryDateTime = query.value(fieldDDT).toDateTime();
        info.notes = query.value(fieldNotes).toString();
        info.clientId = query.value(fieldClientId).toULongLong();
        info.userId = query.value(fieldUserId).toULongLong();
        //get specialOrder Discounts
        info.disc = getSpecialOrderAverageDiscount(info.orderid)/100; //in percentage.
      }
      //get units descriptions
      qry = QString("SELECT * from measures WHERE id=%1").arg(info.units);
      QSqlQuery query3(db);
      if (query3.exec(qry)) {
        while (query3.next()) {
          int fieldUD = query3.record().indexOf("text");
          info.unitStr=query3.value(fieldUD).toString();
        }//query3 - get descritptions
      }
    }
  }
  return info;
}

// NOTE: This returns the compound tax that effectively apply to the amount for the specialOrder,
//       regardless of the equitative tax for each product. It means that the tax returned is the
//       EFFECTIVE tax for the total amount, instead of an average tax (total/numberOfProducts)..
//       The average tax is not accurate if there are different products with very different taxes.
//       For example:
//              A SO with TWO products :
//                      product 1: Price: 10, tax:15%
//                      product 2: Price  100, tax 1%
//              The average tax is 8%, its equally for the two products but the second one has a lower tax.
//              The tax charged to the group with the average tax is $8.8
//              The tax charged to the group with each product tax is $2.15
//              This means the average tax is not accurate for all cases.
//       Thats why this 'effective' tax average is returned.
double Azahar::getSpecialOrderAverageTax(qulonglong id, AzaharRTypes returnType)
{
  double price = 0;
  double taxMoney = 0;
  double tax = 0;

  if ( id <= 0 ) return 0;
  
  QList<ProductInfo> pList = getSpecialOrderProductsList(id);
  foreach( ProductInfo info, pList) {
    double discount = 0; double pWOtax= 0;
    if (info.validDiscount) discount = info.price*info.qtyOnList*(info.discpercentage/100);

    if ( !getConfigTaxIsIncludedInPrice() )
      pWOtax = info.price;
    else
      pWOtax= info.price/(1+((info.tax+info.extratax)/100));

    price += (pWOtax*info.qtyOnList)-discount;
    taxMoney += (info.tax/100)*((info.qtyOnList*pWOtax)-discount); //info.totaltax*info.qtyOnList;
    //qDebug()<<" < EACH ONE > price: "<<(pWOtax*info.qtyOnList)-discount<<" taxMoney: "<<(info.tax/100)*((pWOtax*info.qtyOnList)-discount)
    //<<" Discount:"<<discount<<" pWOTax:"<<pWOtax;
  }

  foreach(ProductInfo info, pList) {
    tax += (info.totaltax*info.qtyOnList/price)*100;
  }

  qDebug()<<"\n <<<<<<< GetSpecialOrderAverageTax    :%"<<tax<<" $"<<taxMoney<<" Based Price:"<<price<<">>>>>>>>\n";

  if (returnType == rtMoney) 
    return taxMoney;
  else
    return tax;
}

///This gets discounts on a special order based on its raw products discount, returned in %.
double Azahar::getSpecialOrderAverageDiscount(qulonglong id)
{
  double price = 0;
  double discMoney = 0;
  double disc = 0;

  if ( id <= 0 ) return 0;
  
  QList<ProductInfo> pList = getSpecialOrderProductsList(id);
  foreach( ProductInfo info, pList) {
    price += info.price*info.qtyOnList;
    if (info.validDiscount  && !info.isNotDiscountable)
      discMoney += (info.discpercentage/100)*info.price*info.qtyOnList;
    //qDebug()<<" < EACH ONE > price: "<<info.price*info.qtyOnList<<" discMoney: "<<(info.discpercentage/100)*info.price*info.qtyOnList;
  }
  
  foreach(ProductInfo info, pList) {
      if (info.validDiscount  && !info.isNotDiscountable)
        disc += ( ((info.discpercentage/100)*info.price*info.qtyOnList)/price )*100;
    //qDebug()<<" < SO DISCOUNT >  qtyOnList:"<<info.qtyOnList<<" discMoney for product: "<<(info.discpercentage/100)*info.price*info.qtyOnList<<" SO price:"<<price<<" discMoney for group:"<<discMoney<<" DISCOUNT % for group:"<< disc;
  }

  qDebug()<<"\n <<<<<<< GetSpecialOrderAverageDiscount    :%"<<disc<<" $"<<discMoney<<" Based Price:"<<price<<">>>>>>>>\n";
  return disc;
}

int Azahar::getSpecialOrderNonDiscountables(qulonglong id)
{
    int count = 0;
    if ( id <= 0 ) return 0;
    
    QList<ProductInfo> pList = getSpecialOrderProductsList(id);
    foreach(ProductInfo info, pList) {
        if (info.isNotDiscountable)
            count++;
    }
    
    return count;
}


QStringList Azahar::getStatusList()
{
  QStringList result;
  result.clear();
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec("select text from so_status;")) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("text");
        QString text = myQuery.value(fieldText).toString();
        result.append(text);
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

QStringList Azahar::getStatusListExceptDelivered()
{
  QStringList result;
  result.clear();
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select text from so_status where id!=%1;").arg(stDelivered))) {
      while (myQuery.next()) {
        int fieldText = myQuery.record().indexOf("text");
        QString text = myQuery.value(fieldText).toString();
        result.append(text);
      }
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
  return result;
}

int Azahar::getStatusId(QString texto)
{
  qulonglong result=0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    QString qryStr = QString("SELECT so_status.id FROM so_status WHERE text='%1';").arg(texto);
    if (myQuery.exec(qryStr) ) {
      while (myQuery.next()) {
        int fieldId   = myQuery.record().indexOf("id");
        qulonglong id= myQuery.value(fieldId).toULongLong();
        result = id;
      }
    }
    else {
      setError(myQuery.lastError().text());
    }
  }
  return result;
}

QString Azahar::getRandomMessage(QList<qulonglong> &excluded, const int &season)
{
  QString result;
  QString firstMsg;
  qulonglong firstId = 0;
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    QString qryStr = QString("SELECT message,id FROM random_msgs WHERE season=%1 order by count ASC;").arg(season);
    if (myQuery.exec(qryStr) ) {
      while (myQuery.next()) {
        int fieldId      = myQuery.record().indexOf("id");
        int fieldMsg     = myQuery.record().indexOf("message");
        qulonglong id    = myQuery.value(fieldId).toULongLong();
        if ( firstMsg.isEmpty() ) {
          firstMsg = myQuery.value(fieldMsg).toString(); //get the first msg.
          firstId  = myQuery.value(fieldId).toULongLong();
        }
        qDebug()<<"Examining msg Id: "<<id;
        //check if its not on the excluded list.
        if ( !excluded.contains(id) ) {
          //ok, return the msg, increment count.
          result = myQuery.value(fieldMsg).toString();
          randomMsgIncrementCount(id);
          //modify the excluded list, insert this one.
          excluded.append(id);
          //we exit from the while loop.
          qDebug()<<" We got msg:"<<result;
          break;
        }
      }
    }
    else {
      setError(myQuery.lastError().text());
    }
  }
  if (result.isEmpty() && firstId > 0) {
    result = firstMsg;
    randomMsgIncrementCount(firstId);
    excluded << firstId;
    qDebug()<<"Returning the fist message!";
  }
  return result;
}

void Azahar::randomMsgIncrementCount(qulonglong id)
{
  if (!db.isOpen()) db.open();
  if (db.isOpen()) {
    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("update random_msgs set count=count+1 where id=%1;").arg(id))) {
      qDebug()<<"Random Messages Count updated";
    }
    else {
      qDebug()<<"ERROR: "<<myQuery.lastError();
    }
  }
}

bool Azahar::insertRandomMessage(const QString &msg, const int &season)
{
  bool result = false;
  if (!db.isOpen()) db.open();
  QSqlQuery query(db);
  query.prepare("INSERT INTO random_msgs (message, season, count) VALUES (:message, :season, :count);");
  query.bindValue(":msg", msg);
  query.bindValue(":season", season);
  query.bindValue(":count", 0);
  if (!query.exec()) {
    setError(query.lastError().text());
  }
  else result=true;
  return result;
}


CurrencyInfo Azahar::getCurrency(const qulonglong &id)
{
    CurrencyInfo result;
    result.id = 0;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery myQuery(db);
        if (myQuery.exec(QString("select * from currencies where id=%1;").arg(id))) {
            while (myQuery.next()) {
                int fieldName   = myQuery.record().indexOf("name");
                int fieldFactor = myQuery.record().indexOf("factor");
                result.name   = myQuery.value(fieldName).toString();
                result.factor = myQuery.value(fieldFactor).toDouble();
                result.id     = id;
            }
        }
        else {
            qDebug()<<"ERROR: "<<myQuery.lastError();
        }
    }
    return result;
}

CurrencyInfo Azahar::getCurrency(const QString &name)
{
    CurrencyInfo result;
    result.id = 0;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery myQuery(db);
        if (myQuery.exec(QString("select * from currencies where name='%1';").arg(name))) {
            while (myQuery.next()) {
                int fieldName   = myQuery.record().indexOf("name");
                int fieldFactor = myQuery.record().indexOf("factor");
                int fieldId     = myQuery.record().indexOf("id");
                result.name   = myQuery.value(fieldName).toString();
                result.factor = myQuery.value(fieldFactor).toDouble();
                result.id     = myQuery.value(fieldId).toULongLong();
            }
        }
        else {
            qDebug()<<"ERROR: "<<myQuery.lastError();
        }
    }
    return result;
}

QList<CurrencyInfo> Azahar::getCurrencyList()
{
    QList<CurrencyInfo> result;
    if (!db.isOpen()) db.open();
    if (!db.isOpen()) {return result;}

    QSqlQuery myQuery(db);
    if (myQuery.exec(QString("select * from currencies;"))) {
        while (myQuery.next()) {
            CurrencyInfo info;
            int fieldName   = myQuery.record().indexOf("name");
            int fieldFactor = myQuery.record().indexOf("factor");
            int fieldId     = myQuery.record().indexOf("id");
            info.name   = myQuery.value(fieldName).toString();
            info.factor = myQuery.value(fieldFactor).toDouble();
            info.id     = myQuery.value(fieldId).toULongLong();
            result.append(info);
        }
    }
    else {
        qDebug()<<"ERROR: "<<myQuery.lastError();
    }

    return result;
}

bool Azahar::insertCurrency(const QString name, const double &factor)
{
    bool result = false;
    if (!db.isOpen()) db.open();
    QSqlQuery query(db);
    query.prepare("INSERT INTO currencies (name,factor) VALUES (:name, :factor);");
    query.bindValue(":name", name);
    query.bindValue(":factor", factor);
    if (!query.exec()) {
        setError(query.lastError().text());
    }
    else result=true;
    return result;
}

bool Azahar::deleteCurrency(const qulonglong &cid)
{
    bool result = false;
    if (!db.isOpen()) db.open();
    QSqlQuery query(db);
    query = QString("DELETE FROM currencies WHERE id=%1").arg(cid);
    if (!query.exec()) setError(query.lastError().text()); else result=true;
    return result;
}

// Reservations

qulonglong Azahar::insertReservation(ReservationInfo info)
{
    qulonglong result = 0;
    
    if (!db.isOpen()) db.open();
    QSqlQuery query(db);
    query.prepare("INSERT INTO reservations (transaction_id, client_id, date, status, payment, total, totaltaxes) VALUES (:transaction, :client, :date, :status, :payment, :total, :totaltaxes);");
    query.bindValue(":transaction", info.transaction_id);
    query.bindValue(":client", info.client_id);
    query.bindValue(":payment", info.payment);
    query.bindValue(":date", info.date);
    query.bindValue(":status", info.status);
    query.bindValue(":total", info.total);
    query.bindValue(":totaltaxes", info.totalTaxes);
    
    if (!query.exec()) {
        setError(query.lastError().text());
        qDebug()<< __FUNCTION__ << query.lastError().text();
    }
    else result = query.lastInsertId().toULongLong();
    
    return result;
}

bool Azahar::setReservationStatus(qulonglong id, reservationState state)
{
    bool result = false;
    if (!db.isOpen()) db.open();
    QSqlQuery query(db);
    query.prepare("UPDATE reservations SET status=:state WHERE id=:id;");
    query.bindValue(":id", id);
    query.bindValue(":state", state);
    
    if (!query.exec()) setError(query.lastError().text()); else result=true;
    return result;
}

bool Azahar::setTransactionReservationStatus(const qulonglong &trId)
{
    bool result = false;
    if (!db.isOpen()) db.open();
    QSqlQuery query(db);
    query.prepare("UPDATE transactions SET state=:status WHERE id=:id;");
    query.bindValue(":id", trId);
    query.bindValue(":status", tReserved);
    
    if (!query.exec()) setError(query.lastError().text()); else result=true;
    return result;
}

double Azahar::getReservationTotalAmount(qulonglong id)
{
    double result=0;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery myQuery(db);
        myQuery.prepare("SELECT total FROM reservations WHERE id=:id;");
        myQuery.bindValue(":id", id);
        if (myQuery.exec() ) {
            while (myQuery.next()) {
                int fieldTotal   = myQuery.record().indexOf("total");
                result = myQuery.value(fieldTotal).toDouble();
            }
        }
        else {
            setError(myQuery.lastError().text());
        }
    }
    return result;
}

double Azahar::getReservationPayment(qulonglong id)
{
    double result=0;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery myQuery(db);
        myQuery.prepare("SELECT payment FROM reservations WHERE id=:id;");
        myQuery.bindValue(":id", id);
        if (myQuery.exec() ) {
            while (myQuery.next()) {
                int fieldTotal   = myQuery.record().indexOf("payment");
                result = myQuery.value(fieldTotal).toDouble();
            }
        }
        else {
            setError(myQuery.lastError().text());
        }
    }
    return result;
}

ReservationInfo Azahar::getReservationInfo(const qulonglong &id)
{
    ReservationInfo result;
    result.id=0;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery myQuery(db);
        myQuery.prepare("SELECT * FROM reservations WHERE id=:id;");
        myQuery.bindValue(":id", id);
        if (myQuery.exec() ) {
            while (myQuery.next()) {
                int fieldPayment = myQuery.record().indexOf("payment");
                int fieldClient  = myQuery.record().indexOf("client_id");
                int fieldTr      = myQuery.record().indexOf("transaction_id");
                int fieldDate    = myQuery.record().indexOf("date");
                int fieldStatus  = myQuery.record().indexOf("status");
                int fieldTotal   = myQuery.record().indexOf("total");
                int fieldTaxes   = myQuery.record().indexOf("totaltaxes");
                
                result.id = id;
                result.client_id = myQuery.value(fieldClient).toULongLong();
                result.transaction_id = myQuery.value(fieldTr).toULongLong();
                result.total     = myQuery.value(fieldTotal).toDouble();
                result.payment   = myQuery.value(fieldPayment).toDouble();
                result.date      = myQuery.value(fieldDate).toDate();
                result.status    = myQuery.value(fieldStatus).toInt();
                result.totalTaxes= myQuery.value(fieldTaxes).toDouble();
            }
        }
        else {
            setError(myQuery.lastError().text());
        }
    }
    return result;
}

ReservationInfo  Azahar::getReservationInfoFromTr(const qulonglong &trId)
{
    ReservationInfo result;
    result.id=0;
    result.transaction_id = 0;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery myQuery(db);
        myQuery.prepare("SELECT * FROM reservations WHERE transaction_id=:id;");
        myQuery.bindValue(":id", trId);
        if (myQuery.exec() ) {
            while (myQuery.next()) {
                int fieldPayment = myQuery.record().indexOf("payment");
                int fieldClient  = myQuery.record().indexOf("client_id");
                int fieldTr      = myQuery.record().indexOf("transaction_id");
                int fieldDate    = myQuery.record().indexOf("date");
                int fieldStatus  = myQuery.record().indexOf("status");
                int fieldTotal   = myQuery.record().indexOf("total");
                int fieldTaxes   = myQuery.record().indexOf("totaltaxes");
                
                result.id = trId;
                result.client_id = myQuery.value(fieldClient).toULongLong();
                result.transaction_id = myQuery.value(fieldTr).toULongLong();
                result.total     = myQuery.value(fieldTotal).toDouble();
                result.payment   = myQuery.value(fieldPayment).toDouble();
                result.date      = myQuery.value(fieldDate).toDate();
                result.status    = myQuery.value(fieldStatus).toInt();
                result.totalTaxes= myQuery.value(fieldTaxes).toDouble();
            }
        }
        else {
            setError(myQuery.lastError().text());
        }
    }
    return result;
}

CreditInfo Azahar::getCreditInfoForClient(const qulonglong &clientId, const bool &create)
{
    CreditInfo result;
    ClientInfo info;
    qulonglong cid;
    info= getClientInfo(clientId);
    cid=checkParent(info);
    result.id=0;
    result.clientId = 0;
    result.total = 0;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QSqlQuery myQuery(db);
        myQuery.prepare("SELECT * FROM credits WHERE customerid=:id;");
        myQuery.bindValue(":id", cid);
        if (myQuery.exec() ) {
            while (myQuery.next()) {
                int fieldId     = myQuery.record().indexOf("id");
                int fieldTotal  = myQuery.record().indexOf("total");
                result.clientId = cid;
                result.id       = myQuery.value(fieldId).toULongLong();
                result.total    = myQuery.value(fieldTotal).toDouble();
                qDebug()<<__FUNCTION__<<" ----> Got credit:"<<result.id<<" for $"<<result.total;
            }
            qDebug()<<__FUNCTION__<<" Getting CREDIT INFO FOR CLIENT ID:"<<clientId<<" .. create="<<create<<" query size:"<<myQuery.size();
            if (myQuery.size() <= 0 && create) {
                //NO RECORD FOUND, CREATE A NEW ONE.
                if ( getClientInfo(cid).id > 0 ) {
                    qDebug()<<__FUNCTION__<<" Creating new credit for client ID:"<<clientId;
                    result.clientId = cid;
                    result.total = 0;
                    qulonglong newId = insertCredit(result);
                    result.id = newId; //now we can return result with the new credit.
                } else { //NOTE: INVALID CLIENT ID. INVALID CREDIT ALSO!
                    result.clientId = 0;
                    result.total = 0;
                    qDebug()<<__FUNCTION__<<" CREDIT NOT FOUND AND NOT CREATED BECAUSE NO CLIENT FOUND WITH ID:"<<clientId;
                }

            }
        }
        else {
            setError(myQuery.lastError().text());
        }
    }
    return result;
}


qulonglong  Azahar::insertCredit(const CreditInfo &info)
{
    qulonglong result = 0;
    if (!db.isOpen()) db.open();
    QSqlQuery query(db);
    query.prepare("INSERT INTO credits (customerid, total) VALUES (:client, :total);");
    query.bindValue(":client", info.clientId);
    query.bindValue(":total", info.total);
    
    if (!query.exec()) {
        setError(query.lastError().text());
        qDebug()<< __FUNCTION__ << query.lastError().text();
    }
    else result = query.lastInsertId().toULongLong();
    return result;
}

bool Azahar::updateCredit(const CreditInfo &info)
{
    bool result = false;
    if (!db.isOpen()) db.open();
    QSqlQuery query(db);
    query.prepare("UPDATE credits SET customerid=:client, total=:total WHERE id=:id;");
    query.bindValue(":client", info.clientId);
    query.bindValue(":total", info.total);
    query.bindValue(":id", info.id);
    
    if (!query.exec()) {
        setError(query.lastError().text());
        qDebug()<< __FUNCTION__ << query.lastError().text();
    }
    else result = query.lastInsertId().toULongLong();
    return result;
}


QList<CreditHistoryInfo> Azahar:: getCreditHistoryForClient(const qulonglong &clientId, const int &lastDays)
{
    QList<CreditHistoryInfo> result;
    if (!db.isOpen()) db.open();
    if (db.isOpen()) {
        QDate today = QDate::currentDate();
        QDate limitDate;
        if (lastDays > 0) 
            limitDate = today.addDays(-lastDays);
        else
            limitDate = today.addDays(-30);//default is one month ago.
        QSqlQuery myQuery(db);
        myQuery.prepare("SELECT * FROM credit_history WHERE customerid=:clientId AND date >= :lDate ;");
        myQuery.bindValue(":clientId", clientId);
        myQuery.bindValue(":lDate", limitDate);
        if (myQuery.exec()) {
            while (myQuery.next()) {
                int fieldId     = myQuery.record().indexOf("id");
                int fieldSaleId = myQuery.record().indexOf("saleid");
                int fieldTotal  = myQuery.record().indexOf("amount");
                int fieldDate   = myQuery.record().indexOf("date");
                int fieldTime   = myQuery.record().indexOf("time");
                CreditHistoryInfo info;
                info.id = myQuery.value(fieldId).toULongLong();
                info.customerId = clientId;
                info.saleId   = myQuery.value(fieldSaleId).toULongLong();
                info.amount   = myQuery.value(fieldTotal).toDouble();
                info.date     = myQuery.value(fieldDate).toDate();
                info.time     = myQuery.value(fieldTime).toTime();
                result.append(info);
                qDebug()<<__FUNCTION__<<" Got history for "<<info.customerId<<" SaleID:"<<info.saleId<<" Amount:"<<info.amount;
            }
        }
        else {
            qDebug()<<"ERROR: "<<myQuery.lastError();
        }
    }
    return result;
}

qulonglong  Azahar::insertCreditHistory(const CreditHistoryInfo &info)
{
    qulonglong result = 0;
    if (!db.isOpen()) db.open();
    QSqlQuery query(db);
    query.prepare("INSERT INTO credit_history (customerid, saleid, amount, date, time) VALUES (:clientId, :saleId, :amount, :date, :time);");
    query.bindValue(":clientId", info.customerId);
    query.bindValue(":date", info.date);
    query.bindValue(":time", info.time);
    query.bindValue(":amount", info.amount);
    query.bindValue(":saleId", info.saleId);
    
    if (!query.exec()) {
        setError(query.lastError().text());
        qDebug()<< __FUNCTION__ << query.lastError().text();
    }
    else result = query.lastInsertId().toULongLong();
    return result;
}






#include "azahar.moc"
