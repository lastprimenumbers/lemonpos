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

#ifndef AZAHAR_H
#define AZAHAR_H

#include <QtSql>
#include <QObject>
#include <QHash>
#include "../src/structs.h"
#include "../src/enums.h"

enum AzaharRTypes {rtPercentage=1001, rtMoney=1002};

class QString;

/**
 * This class makes all database access.
 *
 * @short Database access class
 * @author Miguel Chavez Gamboa <miguel.chavez.gamboa@gmail.com>
 * @version 0.9
 */

class Azahar : public QObject
{
  Q_OBJECT  //NOTE: This also gives the extra function of self destroying when parent is destroyed ? it has no parent
  private:
    QSqlDatabase db;
    QString errorStr;
    void setError(QString err);
    QString m_mainClient;
  public:
    Azahar(QWidget * parent = 0);
    ~ Azahar();
    bool isConnected();
    QString lastError();
    void initDatabase(QString user, QString server, QString password, QString dbname);
    void setDatabase(const QSqlDatabase& database);

    // PRODUCTS
    ProductInfo  getProductInfo(const QString &code, const bool &notConsiderDiscounts = false); //the 2nd parameter is to get the taxes for the group (not considering discounts)
    qulonglong   getProductOfferCode(qulonglong code);
    qulonglong   getProductCode(QString text);
    qulonglong   getProductCodeFromAlphacode(QString text);
    QList<qulonglong> getProductsCode(QString regExpName);
    QStringList  getProductsList();
    bool         insertProduct(ProductInfo info);
    bool         updateProduct(ProductInfo info, qulonglong oldcode);
    bool         decrementProductStock(qulonglong code, double qty, QDate date);
    bool         decrementGroupStock(qulonglong code, double qty, QDate date);
    bool         incrementGroupStock(qulonglong code, double qty);
    bool         incrementProductStock(qulonglong code, double qty);
    bool         deleteProduct(qulonglong code);
    double       getProductDiscount(qulonglong code, bool isGroup=false); //returns the discount percentage!
    QList<pieProdInfo>  getTop5SoldProducts();
    double       getTopFiveMaximum();
    QList<pieProdInfo>  getAlmostSoldOutProducts(int min, int max);
    double       getAlmostSoldOutMaximum(int max);
    QList<ProductInfo>  getSoldOutProducts();
    QList<ProductInfo>  getLowStockProducts(double min);
    QList<ProductInfo>  getAllProducts();
    double       getProductStockQty(qulonglong code);
    qulonglong   getLastProviderId(qulonglong code);
    bool         updateProductLastProviderId(qulonglong code, qulonglong provId);
    QList<ProductInfo> getGroupProductsList(qulonglong id, bool notConsiderDiscounts = false);
    /* DEPRECATED double       getGroupAverageTax(qulonglong id);
       DEPRECATED double       getGroupTotalTax(qulonglong id); */
    GroupInfo    getGroupPriceAndTax(ProductInfo pi);
    QString      getProductGroupElementsStr(qulonglong id);
    void         updateGroupPriceDrop(qulonglong code, double pd);
    void         updateGroupElements(qulonglong code, QString elementsStr);

    
    //PRODUCT STOCK CORRECTION
    bool         correctStock(qulonglong pcode, double oldStockQty, double newStockQty, const QString &reason );

    //CATEGORIES
    QHash<QString, int> getCategoriesHash();
    QStringList getCategoriesList();
    qulonglong  getCategoryId(QString texto);
    QString     getCategoryStr(qulonglong id);
    bool        insertCategory(QString text);
    bool        deleteCategory(qulonglong id);

    //MEASURES
    QStringList getMeasuresList();
    qulonglong  getMeasureId(QString texto);
    QString     getMeasureStr(qulonglong id);
    bool        insertMeasure(QString text);
    bool        deleteMeasure(qulonglong id);

    //OFFERS
    bool         createOffer(OfferInfo info);
    bool         deleteOffer(qlonglong id);
    QString      getOffersFilterWithText(QString text); //get all products with desc=text as a regexp that has discounts.
    bool         moveOffer(qulonglong oldp, qulonglong newp);

    //USERS
    bool         updateUser(UserInfo info);
    QString      getUserName(QString username); //gets the user name from username
    QString      getUserName(qulonglong id);
    unsigned int getUserId(QString uname);
    unsigned int getUserIdFromName(QString uname);
    QHash<QString,UserInfo> getUsersHash();
    QStringList  getUsersList();
    bool         insertUser(UserInfo info);
    int          getUserRole(const qulonglong &userid);
    UserInfo     getUserInfo(const qulonglong &userid);
    bool         deleteUser(qulonglong id);

    //CLIENTS
    bool         insertClient(ClientInfo info);
    bool         updateClient(ClientInfo info);
    bool         incrementClientPoints(qulonglong id, qulonglong points);
    bool         decrementClientPoints(qulonglong id, qulonglong points);
    ClientInfo   getClientInfo(qulonglong clientId);
    ClientInfo   getClientInfo(QString clientCode);
    QHash<QString, ClientInfo> getClientsHash();
    QStringList  getClientsList();
    void getChildrenClientsList(QString parentClient, QStringList &codes, QStringList &names);
    QString      getMainClient();
    unsigned int getClientId(QString uname);
    bool         deleteClient(qulonglong id);

    //TRANSACTIONS
    TransactionInfo getTransactionInfo(qulonglong id);
    qulonglong  insertTransaction(TransactionInfo info);
    //QList<ProductInfo>     getTransactionGroupsList(qulonglong tid);
    QList<TransactionInfo> getDayTransactions(int terminal);
    QList<TransactionInfo> getDayTransactions();
    AmountAndProfitInfo    getDaySalesAndProfit(int terminal);
    AmountAndProfitInfo    getDaySalesAndProfit();
    QList<TransactionInfo> getMonthTransactionsForPie();
    QList<TransactionInfo> getMonthTransactions();
    AmountAndProfitInfo    getMonthSalesAndProfit();
    ProfitRange getMonthProfitRange();
    ProfitRange getMonthSalesRange();
    bool        updateTransaction(TransactionInfo info);
    bool        cancelTransaction(qulonglong id, bool inProgress);
    bool        deleteTransaction(qulonglong id);
    bool        deleteEmptyTransactions();
    QList<TransactionInfo> getLastTransactions(int pageNumber=1,int numItems=20,QDate beforeDate=QDate::currentDate ());
    QList<TransactionInfo> getTransactionsPerDay(int pageNumber=1,int numItems=20,QDate beforeDate=QDate::currentDate ());
    qulonglong  getEmptyTransactionId();
    double      getTransactionDiscMoney(qulonglong id);
    bool        setTransactionStatus(qulonglong trId, TransactionState state);
    
    // TRANSACTIONITEMS
    bool        insertTransactionItem(TransactionItemInfo info);
    bool        deleteAllTransactionItem(qulonglong id);
    QList<TransactionItemInfo> getTransactionItems(qulonglong id);

    
    //BALANCES
    qulonglong  insertBalance(BalanceInfo info);
    bool        updateBalance(BalanceInfo info);
    BalanceInfo getBalanceInfo(qulonglong id);

    //CASHOUTS
    qulonglong insertCashFlow(CashFlowInfo info);
    QList<CashFlowInfo> getCashFlowInfoList(const QList<qulonglong> &idList);
    QList<CashFlowInfo> getCashFlowInfoList(const QDateTime &start, const QDateTime &end);

    //PayTypes
    QString     getPayTypeStr(qulonglong type);
    qulonglong  getPayTypeId(QString type);

    //LOGS
    bool        insertLog(const qulonglong &userid, const QDate &date, const QTime &time, const QString actionStr);

    //new config way - for cross binary access.
    bool        getConfigFirstRun();
    bool        getConfigTaxIsIncludedInPrice();
    void        cleanConfigFirstRun();
    void        setConfigTaxIsIncludedInPrice(bool option);
    QPixmap     getConfigStoreLogo();
    QString     getConfigStoreName();
    QString     getConfigStoreAddress();
    QString     getConfigStorePhone();
    bool        getConfigSmallPrint();
    bool        getConfigLogoOnTop();
    bool        getConfigUseCUPS();
    QString     getConfigDbVersion(); //NEW: Aug 31 2011.
    void        setConfigStoreLogo(const QByteArray &baPhoto);
    void        setConfigStoreName(const QString &str);
    void        setConfigStoreAddress(const QString &str);
    void        setConfigStorePhone(const QString &str);
    void        setConfigSmallPrint(bool yes);
    void        setConfigUseCUPS(bool yes);
    void        setConfigLogoOnTop(bool yes);

    //Special Orders
    void                    specialOrderSetStatus(qulonglong id, int status);
    void                    soTicketSetStatus(qulonglong ticketId, int status);
    qulonglong              insertSpecialOrder(SpecialOrderInfo info);
    bool                    updateSpecialOrder(SpecialOrderInfo info);
    bool                    decrementSOStock(qulonglong id, double qty, QDate date);
    bool                    deleteSpecialOrder(qulonglong id);
    SpecialOrderInfo        getSpecialOrderInfo(qulonglong id);
    QList<ProductInfo>      getSpecialOrderProductsList(qulonglong id);
    QString                 getSpecialOrderProductsStr(qulonglong id);
    QList<SpecialOrderInfo> getAllSOforSale(qulonglong saleId);
    QList<SpecialOrderInfo> getAllReadySOforSale(qulonglong saleId);
    int                     getReadySOCountforSale(qulonglong saleId);
    QString                 getSONotes(qulonglong id);
    QStringList             getStatusList();
    QStringList             getStatusListExceptDelivered();
    int                     getStatusId(QString texto);
    double                  getSpecialOrderAverageTax(qulonglong id, AzaharRTypes returnType= rtPercentage);
    double                  getSpecialOrderAverageDiscount(qulonglong id);
    int                     getSpecialOrderNonDiscountables(qulonglong id);

    //Reservations
    qulonglong              insertReservation(ReservationInfo info);
    bool                    setReservationStatus(qulonglong id, reservationState state);
    double                  getReservationTotalAmount(qulonglong id);
    double                  getReservationPayment(qulonglong id);
    bool                    setTransactionReservationStatus(const qulonglong &trId);
    ReservationInfo         getReservationInfo(const qulonglong &id);
    ReservationInfo         getReservationInfoFromTr(const qulonglong &trId);

    //Random Msgs
    QString   getRandomMessage(QList<qulonglong> &excluded, const int &season);
              //NOTE:We will modify excluded list.. SO do not make const the List.
    void      randomMsgIncrementCount(qulonglong id);
    bool      insertRandomMessage(const QString &msg, const int &season);

    //Currencies
    CurrencyInfo getCurrency(const qulonglong &id);
    CurrencyInfo getCurrency(const QString &name);
    QList<CurrencyInfo> getCurrencyList();
    bool     insertCurrency(const QString name, const double &factor);
    bool     deleteCurrency(const qulonglong &cid);

    //Credits and its history
    CreditInfo        getCreditInfo(const qulonglong &id);
    CreditInfo        getCreditInfoForClient(const qulonglong &clientId, const bool &create=true); //by default it creates a new credit record if no one found for the customer.
    QList<CreditHistoryInfo> getCreditHistoryForClient(const qulonglong &clientId, const int &lastDays=0);
    
    qulonglong        insertCredit(const CreditInfo &info);
    bool              updateCredit(const CreditInfo &info);
    qulonglong        insertCreditHistory(const CreditHistoryInfo &info);

};

#endif
