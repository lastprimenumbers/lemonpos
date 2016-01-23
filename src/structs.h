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
**************************************************************************/

#ifndef MYSTRUCTS_H
#define MYSTRUCTS_H

#include <QString>
#include <QByteArray>
#include <QPixmap>
#include <QDateTime>
#include <QDate>
#include <QHash>

struct BasicInfo {
    qulonglong id;
    QString code;
    QString name;
    QString surname;
};


struct ProductInfo {
  QString code;
  QString desc;
  double price;
  double disc;
  double discpercentage;
  bool   validDiscount;
  double cost;
  double tax;
  double extratax;
  double totaltax;//in money...
  QByteArray photo;
  double stockqty;
  int units;
  QString unitStr;
  int category;
  double utility;
  int row; // New: Sept 30, 2007: To store the row where the product is located in the listView.
  qulonglong points; //New: Dec 15 2007: To store the points the product gives.
  double qtyOnList;
  double purchaseQty; //New: Jan 13 2007: For purchase editor...
  qulonglong lastProviderId;
  QString alphaCode;
  double soldUnits; // mch 21Nov09 FOR PRINTED REPORT - LOW STOCK
  // for grouped products and special orders
  bool isAGroup;
  bool isARawProduct;
  QString groupElementsStr;
  double groupPriceDrop;
  //for later use:
  qulonglong taxmodelid;
  QString taxElements;
  //aug 11 2011, for 0.9.4.rc9
  bool hasUnlimitedStock;
  bool isNotDiscountable;
  double quantity;
  int qunit;
};

struct UserInfo
{
  unsigned int id;
  QString username;
  QString password;
  QString salt;
  QString name;
  QString address;
  QString cell;
  QString phone;
  QByteArray photo;
  int role;
};

struct Limit
{
    qlonglong id;
    QString clientCode;
    QString clientTag;
    QString productCode;
    int productCat;
    double limit;
    double current;
    int priority;
    qlonglong parent;
    QString label;
};

struct ClientInfo
{
  unsigned int id;
  QString      code;
  QString      name;
  QString      surname;
  QString      email;
  QString      parentClient;
  QString      address;
  QString      nation;
  QString      phone;
  double       monthly;
  QByteArray   photo;
  QDate        since;
  QDate        expiry;
  QDate        birthDate;
  QDate        beginsusp;
  QDate        endsusp;
  QString      msgsusp;
  QString      notes;
  QStringList   tags;
  QDate        lastCreditReset;
  // Effective limits for the current transaction
  QHash<qulonglong, Limit>         limits;
};


struct OfferInfo
{
  QString productCode;
  double     discount;
  QDate      dateStart;
  QDate      dateEnd;
};

struct TransactionInfo
{
  qulonglong id;
  int        type;
  double     amount;
  QDate      date;
  QTime      time;
  double     paywith;
  double     changegiven;
  int        paymethod;
  int        state;
  qulonglong userid;
  qulonglong clientid;
  QString    cardnumber;
  QString    cardauthnum;
  int        itemcount;
  QString    itemlist;
  double     disc;
  double     discmoney;
  qulonglong points;
  double     utility;
  int        terminalnum;
  //QString    groups;  //DEPRECATED
  QString    specialOrders;
  qulonglong providerid;
  qulonglong balanceId; //to store balance where it was sold. For SESSIONS.
  double     totalTax; //in money.
  QString    donor;
  QString   note;
};

struct BalanceInfo
{
  qulonglong id;
  QDateTime  dateTimeStart;
  QDateTime  dateTimeEnd;
  qulonglong userid;
  QString    username;
  double     initamount;
  double     in;
  double     out;
  double     cash;
  double     card;
  QString    transactions;
  int        terminal;
  QString    cashflows;//28DIC09
  bool       done;
};

struct PrintBalanceInfo
{
  QString     thBalanceId;
  QString     storeName;
  QString     storeAddr;
  QString     thTitle;
  QString     thDeposit;
  QString     thIn;
  QString     thOut;
  QString     thInDrawer;
  QString     thTitleDetails;
  QString     thTitleCFDetails;
  QString     thTrId;
  QString     thTrTime;
  QString     thTrAmount;
  QString     thTrPaidW;
  QString     thTrPayMethod;
  QPixmap     storeLogo;
  QString     startDate;
  QString     endDate;
  // qtys as string - local aware / translated
  QString     initAmount;
  QString     inAmount;
  QString     outAmount;
  QString     cashAvailable;
  bool        logoOnTop;
  QString     thCFType;
  QString     thCFReason;
  QString     thCFDate;
  QStringList trList;
  QStringList cfList;
  QString     reservationNote;
  QString     notCashNote;
};

struct pieProdInfo
{
  double count;
  QString name;
  QString unitStr;
  QString code;
};

struct ProfitRange
{
  double min;
  double max;
};

struct TicketLineInfo {
  double qty;
  QString unitStr;
  QString desc;
  double price;
  double disc;
  double partialDisc;
  double total;
  double gtotal;
  QString geForPrint;
  bool   completePayment;
  bool   isGroup;
  double payment;
  QDateTime  deliveryDateTime;
  double tax; //total tax in Percentage.
};

struct TicketInfo {
  qulonglong number;
  double total; //this is the total amount of THIS TICKET.
  double change;
  double paidwith;
  int itemcount;
  QString cardnum;
  QString cardAuthNum;
  bool paidWithCard;
  double clientDisc;
  double clientDiscMoney;
  qulonglong buyPoints;
  qulonglong clientPoints;
  qulonglong clientid; // 14 Abril 08
  QList<TicketLineInfo> lines;
  QDateTime datetime;
  bool hasSpecialOrders;
  bool completingSpecialOrder;
  double totalTax;
  QDateTime deliveryDT;
  double soTotal; //this is the total for the SO (nextpayment + prepayment)
  QString subTotal;
  QString terminal;
  //for reservations:
  bool isAReservation;
  bool reservationStarted;
  double reservationPayment;
  double purchaseTotal;
  qulonglong reservationId;
  //Portobello
  double     balance;
  double    monthly;
  QDate expiry;
  QDate nextReset;
};

struct PrintTicketInfo {
  QString    storeName;
  QString    storeAddr;
  QString    storePhone;
  QString    ticketMsg;
  QPixmap    storeLogo;
  QString    salesPerson;
  QString    terminal;
  QString    thPhone;
  QString    thDate;
  QString    thProduct;
  QString    thQty;
  QString    thPrice;
  QString    thTotal;
  QString    thTotals;
  QString    thDiscount;
  QString    thArticles;
  QString    thPoints;
  QString    thTicket;
  QString    thPaid;
  QString    thChange;
  QString    tDisc;
  QString    thCard;
  QString    thCardAuth;
  double     totDisc;
  TicketInfo ticketInfo;
  bool       logoOnTop;
  QString    paymentStrComplete;
  QString    paymentStrPrePayment;
  QString    nextPaymentStr;
  QString    lastPaymentStr;
  QString    deliveryDateStr;
  QString    clientDiscountStr;
  double     clientDiscMoney;
  QString    randomMsg;
  QString    thChangeStr;
  QString    taxes;
  QString    thTax;
  QString    thTendered;
  QString    thSubtotal;
  QString    subtotal;
  QString    resTotalAmountStr;
  QString    hdrReservation;
  QString    thExpiry;
  QString    thExpiryStr;
  QString    thBalance;
  QString    thBalanceStr;
  QString    thMonthly;
  QString    thMonthlyStr;
  QString    thNextReset;
  QString    thNextResetStr;
  QString    clientName;
  QString    clientSurname;
  QString    clientCode;
};

//TODO: add grouped products and special orders
//      is it convenient? in case a pack of 6 water botles is not convenient, but a combo "My burger package"
//      consisting of one burger, one soda and one fried potatoes.
struct TransactionItemInfo
{
  qulonglong transactionid;
  int        position;
  QString productCode;
  double     qty;
  double     points;
  QString    unitStr;
  double     cost;
  double     price;
  double     disc;
  double     total;
  QString    name;
  //for special orders, also productCode will be 0 for special orders
  QString    soId;
  double     payment;
  bool       completePayment;
  bool       isGroup;
  QDateTime  deliveryDateTime;
  double     tax; // total tax in percentage.
  // Optional: compiled during joint queries
  QDate      date;
  double    quantity;
};

struct Statistics
{
    // Owners code
    QStringList code;
    // Owners id
    QList<unsigned int> id;
    // Transaction types ids
    QList<int> type;
    // Start date for the statistic
    QDate start;
    // End date for the statistic
    QDate end;
    // List of items in the statistic
    QHash<QString,TransactionItemInfo> items;
    // Mapping between product code and point expense
    QHash<QString,double> products;
    // Mapping between product code and market value
    QHash<QString,double> val_products;
    // Mapping between product id and physical quantity
    QHash<QString,double> qty_products;
    // Mapping between category id and expense
    QHash<int,double> categories;
    // Mapping between category id and expense
    QHash<int,double> val_categories;
    // Mapping between category id and physical quantity
    QHash<int,double> qty_categories;
    // Total expense
    double total;
};

// Holds the statistics about the consumes of a family in a time range.
struct Family
{
    // COMPOSITION
    QList<ClientInfo> members;
    Statistics stats;
};


struct DonorInfo
{
  unsigned int id;
  QString      code;
  QString      name;
  QString      surname; //dummy
  QString      address;
  QString      phone;
  QString      emailDonor;
  QString      nameRefDonor;
  QString      surnameRefDonor;
  QString      emailRefDonor;
  QString      phoneRefDonor;
  QByteArray   photo;
  QDate        since;
  QString      notesRefDonor;
  Statistics   stats;
};

struct CashFlowInfo
{
  qulonglong id;
  int type;
  qulonglong userid;
  double amount;
  QString reason;
  QDate date;
  QTime time;
  qulonglong terminalNum;
  //next are for cashflow into balance printing
  QString typeStr;
};

struct AmountAndProfitInfo
{
    double amount;
    double profit;
};

struct PrintEndOfDayInfo
{
  QString    storeName;
  QString    storeAddr;
  QPixmap    storeLogo;
  QString    salesPerson;
  QString    terminal;
  QString    thTitle;
  QString    thDate;
  QString    thTime;
  QString    thTicket;
  QString    thAmount;
  QString    thProfit;
  QString    thPayMethod;
  QString    thTotalSales;
  QString    thTotalProfit;
  QString    thTotalTaxes;
  QStringList trLines;
  bool       logoOnTop;
};

struct PrintLowStockInfo
{
  QString    storeName;
  QString    storeAddr;
  QPixmap    storeLogo;
  QString    hTitle;
  QString    hDate; //and time
  QString    hDesc;
  QString    hCode;
  QString    hQty;
  QString    hUnitStr;
  QString    hSoldU;
  QStringList pLines;
  bool       logoOnTop;
};

struct SpecialOrderInfo
{
  qulonglong orderid;
  qulonglong saleid;
  QString    name;
  double     qty;
  double     price;
  double     cost;
  int        status;
  int        units;
  QString    unitStr;
  QString    groupElements;
  QString    notes;
  int        insertedAtRow;
  QString    geForPrint;
  double     payment; //anticipos
  bool       completePayment;
  QDateTime  dateTime;
  qulonglong completedOnTrNum;
  QDateTime  deliveryDateTime;
  qulonglong userId;
  qulonglong clientId;
  double     averageTax;
  double     disc;
};

struct GroupInfo
{
  QString name;
  double  cost;
  double  price;
  double  taxMoney;
  double  tax;
  double  priceDrop;
  double  count; // total of items in the group
  bool    isAvailable; //based on stockqty for each product (and its qtys).
  QHash<QString, ProductInfo> productsList;
};

struct CurrencyInfo
{
    qulonglong id;
    QString name;
    double  factor;
};


struct CreditInfo
{
    qulonglong id;
    qulonglong clientId;
    double     total;
    QDate       lastCreditReset;
};

struct CreditHistoryInfo
{
    qulonglong id;
    qulonglong customerId;
    qulonglong saleId;
    QDate      date;
    QTime      time;
    double     amount;
};

struct RoundingInfo
{
    QString strResult;
    double  doubleResult;
    int     intDecPart;
    int     intIntPart;
};

#endif
