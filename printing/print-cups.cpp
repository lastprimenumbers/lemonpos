/**************************************************************************
*   Copyright (C) 2009-2010 by Miguel Chavez Gamboa                       *
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

#include "print-cups.h"
#include "../src/misc.h"

#include <QString>
#include <QFont>
#include <QtGui/QPrinter>
#include <QPainter>
#include <QLocale>
#include <QDebug>


bool PrintCUPS::printSmallBalance(const PrintBalanceInfo &pbInfo, QPrinter &printer)
{
  bool result = false;
  // calculate font size
  const int headerSize = printer.width()/16; //divisor found by trying, maybe has to do with font metrics?
  const int textSize = headerSize/2; //i think the half of header size is ok
  qDebug()<<"Paper Width: "<<printer.widthMM()<<"mm"<<"; Page Width: "<<printer.width()<<"; calculated headerSize: "<<headerSize;
  
  QFont header = QFont("Impact", headerSize);
  const int Margin = printer.width()/40;
  int yPos        = 0;
  QPainter painter;
  painter.begin( &printer );
  QFontMetrics fm = painter.fontMetrics();

  //NOTE from Qt for the drawText: The y-position is used as the baseline of the font

  QFont tmpFont = QFont("Bitstream Vera Sans", textSize);
  QPen normalPen = painter.pen();
  QString text;
  QSize textWidth;
  if (pbInfo.logoOnTop) {
    // Store Logo
    painter.drawPixmap(printer.width()/2 - (pbInfo.storeLogo.width()/2), Margin, pbInfo.storeLogo);
    yPos = yPos + pbInfo.storeLogo.height();
    // Store Name
    painter.setFont(header);
    text = pbInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    yPos = yPos + Margin + textWidth.height();
    painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos, text);
    tmpFont = QFont("Bitstream Vera Sans", textSize);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    text = pbInfo.storeAddr;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    yPos = yPos + fm.lineSpacing();
    painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
    yPos = yPos + fm.lineSpacing();
  }
  else {
    // Store Logo
    painter.drawPixmap(printer.width() - pbInfo.storeLogo.width() - Margin, Margin*1.75, pbInfo.storeLogo);
    // Store Name
    painter.setFont(header);
    text = pbInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin,Margin+textWidth.height(), text);
    // Store Address
    tmpFont = QFont("Bitstream Vera Sans", textSize);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    yPos = yPos + fm.lineSpacing()+(Margin*0.75);
    text = pbInfo.storeAddr;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.setPen(Qt::darkGray);
    painter.drawText(Margin, yPos, text);
    yPos = yPos + fm.lineSpacing();
  }

  //FIXME: Check for logo printing!!!! if no logo, text is missplaced...
  if (pbInfo.storeLogo.isNull())
      yPos = yPos + Margin + fm.lineSpacing();

  // Header line
  painter.setPen(QPen(Qt::black, Margin*0.25, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);
  yPos = yPos + fm.lineSpacing()+Margin;
  
  tmpFont = QFont("Bitstream Vera Sans", textSize); //FIXME: A little more bigger than normal text? Apr 12 2011.
  tmpFont.setItalic(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  //USER and Terminal
  text = pbInfo.thTitle;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text); // +textWidth.height()+15
  yPos = yPos + 2*fm.lineSpacing();
  //change font
  tmpFont = QFont("Bitstream Vera Sans", textSize*1.25); // bigger
  tmpFont.setItalic(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  painter.setPen(normalPen);
  //Balance ID
  text = pbInfo.thBalanceId;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos , text); //+textWidth.height()
  //Date
  tmpFont = QFont("Bitstream Vera Sans", textSize);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  yPos = yPos + fm.lineSpacing()*2;
  text = pbInfo.startDate;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos , text); //+textWidth.height()
  yPos = yPos + fm.lineSpacing();
  text = pbInfo.endDate;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text); // +textWidth.height()
  yPos = yPos + 2*fm.lineSpacing();
  
  // drawer balance 
  //painter.setPen(QPen(Qt::black, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  int tmpOffset = (printer.width()/4);
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = pbInfo.thDeposit;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin, yPos, text); //+textWidth.height()
  text = pbInfo.thIn;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(tmpOffset, yPos, text); //+textWidth.height()
  text = pbInfo.thOut;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(tmpOffset*2, yPos, text);
  text = pbInfo.thInDrawer;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((tmpOffset*3)-Margin, yPos, text);
  yPos = yPos + fm.lineSpacing();
  painter.setPen(QPen(Qt::darkGray, 2, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);
  painter.setPen(normalPen);
  //The quantities
  tmpFont.setBold(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  yPos = yPos + fm.lineSpacing();
  text = pbInfo.initAmount;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin, yPos, text);
  text = pbInfo.inAmount;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(tmpOffset, yPos, text);
  text = pbInfo.outAmount;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(tmpOffset*2, yPos, text);
  text = pbInfo.cashAvailable;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((tmpOffset*3)-Margin, yPos, text);

  //TRANSACTION DETAILS

  //TODO:
  //     Instead of 'Pay With' we can use 'Tendered' to make more sense
  //     It will be:  Tr. Id   Hour   Amount   Tendered  Type
  //     and Type only include the first two letter 'CA' for Cash and 'CC' for credit cards and 'DB' for Debit Cards
  
  yPos = yPos + 3*fm.lineSpacing();
  //header
  tmpFont = QFont("Bitstream Vera Sans", textSize+2);//FIXME: A little more smaller than normal text? Apr 12 2011.
  tmpFont.setItalic(false);
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = pbInfo.thTitleDetails;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
  
  tmpFont = QFont("Bitstream Vera Sans", textSize);
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  yPos = yPos + 2*fm.lineSpacing();
  tmpOffset = printer.width()/5;
  text = pbInfo.thTrId;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin, yPos, text);
  text = pbInfo.thTrTime;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(tmpOffset, yPos, text);
  text = pbInfo.thTrAmount;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((tmpOffset*2)-Margin*2, yPos, text);
  text = pbInfo.thTrPaidW;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((tmpOffset*3)+Margin*2, yPos, text);
  //text = pbInfo.thTrPayMethod;
  //textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  //painter.drawText(tmpOffset*4, yPos, text);  //printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
  yPos = yPos + fm.lineSpacing();
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, yPos, printer.width()-Margin,yPos);

  tmpFont.setBold(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  painter.setPen(QPen(normalPen));
  yPos = yPos + fm.lineSpacing();
  bool drawNote = false;
  //Iterating each transaction
  foreach(QString trStr, pbInfo.trList) {
    QStringList data = trStr.split("|");
    //we have 5 fields in the string [ORDER] ID, HOUR, AMOUNT, PAIDWITH, PAYMETHOD
    text = data.at(2); // AMOUNT
    text = text.remove("$ ");
    double amount   = text.toDouble();
    //qDebug()<<" TEXT AMOUNT:"<<text<<" double Amount:"<<amount;
    text = data.at(3); // PAID WITH
    text = text.remove("$ ");
    double paidWith = text.toDouble();
    //qDebug()<<" TEXT PAID:"<<text<<" double PAID:"<<paidWith<<" ID:"<<data.at(0);
    QString idTxt;
    if (amount > paidWith) {
        idTxt = "*"+ data.at(0); // ID
        drawNote = true;
    } else idTxt = data.at(0);
    
    text = idTxt;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin,yPos, text);
    text = data.at(1); // HOUR
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(tmpOffset, yPos, text);
    text = data.at(2); // AMOUNT
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((tmpOffset*2)-Margin*2, yPos, text);
    text = data.at(3) + data.at(4); // PAID WITH
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((tmpOffset*3)+Margin*2, yPos, text);
    //text = data.at(4); // PAYMENT METHOD
    //textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    //painter.drawText(tmpOffset*4, yPos, text);  //printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
    yPos = yPos + fm.lineSpacing()+Margin*.5;
  }
  
  //yPos = yPos + fm.lineSpacing()+Margin;
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, yPos, printer.width()-Margin,yPos);


  //Draw the Note about Reservations ( amount > paid )
  if (drawNote) {
      yPos = yPos + fm.lineSpacing()*2;
      painter.setPen(QPen(normalPen));
      fm = painter.fontMetrics();
      text = "* " + pbInfo.reservationNote;
      // Adjust text width
      double maxLen = (printer.width())-Margin*1.5;
      QStringList tmpStr = Misc::stringToParagraph( text , fm, maxLen );
      foreach(QString str, tmpStr) {
          painter.drawText(Margin, yPos, str);
          yPos = yPos + fm.lineSpacing();
      }
      
  }

  //NOTE: Print a note for the payment type.
  yPos = yPos + fm.lineSpacing()*2;
  painter.drawText(Margin, yPos, "** "+ pbInfo.notCashNote);
  yPos = yPos + fm.lineSpacing();

  // CASH FLOW DETAILS
  //NOTE:
  //     The cashout type will be removed from ticket to save space, instead to indicate the type, the amount will be positive (cash-in)
  //     or negative (cash-out).
  tmpOffset = printer.width()/5;
  if (!pbInfo.cfList.isEmpty()) {
    yPos = yPos + 2*fm.lineSpacing(); //2 lines for the title
    tmpFont = QFont("Bitstream Vera Sans", textSize+2); //FIXME: A little more bigger than normal text? Apr 12 2011.
    tmpFont.setItalic(true);
    tmpFont.setBold(true);
    painter.setPen(QPen(normalPen));
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    text = pbInfo.thTitleCFDetails;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
    tmpFont = QFont("Bitstream Vera Sans", textSize); //FIXME: A little more smaller than normal text? Apr 12 2011.
    tmpFont.setItalic(false);
    tmpFont.setBold(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    yPos = yPos + 2*fm.lineSpacing();
    //titles
    text = pbInfo.thTrId;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text); 
    painter.drawText(Margin, yPos, text);
    text = pbInfo.thCFDate;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(tmpOffset-Margin*2, yPos, text);
    text = pbInfo.thTrAmount;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((tmpOffset*2)-Margin*2, yPos, text);
    text = pbInfo.thCFReason;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((tmpOffset*3)+Margin*3,yPos, text);
    //text = pbInfo.thCFType;
    //textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    //painter.drawText(tmpOffset*4, yPos, text); //printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
    yPos = yPos + fm.lineSpacing();
    painter.setPen(QPen(Qt::darkGray, 2, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
    painter.drawLine(Margin,yPos, printer.width()-Margin, yPos);

    tmpFont = QFont("Bitstream Vera Sans", textSize);
    tmpFont.setItalic(false);
    tmpFont.setBold(false);
    painter.setFont(tmpFont);
    painter.setPen(QPen(normalPen));
    fm = painter.fontMetrics();
    yPos =yPos + fm.lineSpacing();
    //Iterating each cashflow
    foreach(QString cfStr, pbInfo.cfList) {
      QStringList data = cfStr.split("|");
      //we have 5 fields in the string [ORDER] ID, TYPESTR, REASON, AMOUNT, DATE
      text = data.at(0); // ID
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.drawText(Margin, yPos, text);
      text = data.at(4); // HOUR
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.drawText(tmpOffset-Margin*2, yPos, text);
      text = data.at(1)+" "+data.at(3); // AMOUNT  --here we add the sign to indicate it was IN+ or OUT-.
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.drawText((tmpOffset*2)-Margin*2,yPos, text);
      text = data.at(2); // REASON
      while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= printer.width()-5 ) { text.chop(2); }
      //textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.drawText((tmpOffset*3)+Margin*3, yPos, text);
      //text = data.at(1); // TYPE
      //FIXME: Pass a parameter to indicate a cash-in or cash-out. Now we take the word 'cash in/out'
      //       The text is stored in the database, so the admin can change this text using mysql (not squeeze)...
      //fm = painter.fontMetrics();
      //while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= printer.width()-Margin*1) { text.chop(2); } //(printer.width()-fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width())
      //textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      //painter.drawText(tmpOffset*4, yPos, text); //printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
      yPos = yPos + fm.lineSpacing();
    }
  }
  result = painter.end();// this makes the print job start
  return result;
}

// Paper size for a receipt: 72mm x 200mm (the height is just a number, because it is 'endless')
// This code is meant to be used in receipt printers, which dont need page breaks.
// The problem is when this code is sent to a PDF instead of a receipt printer (i.e. for testing and debugging)
// Which i can solve by enable the printer.newPage() (uncommenting it on the products iterations where its mainly needed)
bool PrintCUPS::printSmallTicket(const PrintTicketInfo &ptInfo, QPrinter &printer)
{
  bool result = false;
  // calculate font size
  const int headerSize = printer.width()/16; //divisor found by trying, maybe has to do with font metrics?
  const int textSize = headerSize/2; //i think the half of header size is ok
  qDebug()<<"Paper Width: "<<printer.widthMM()<<"mm"<<"; Page Width: "<<printer.width()<<"; calculated headerSize: "<<headerSize;

  QFont header = QFont("Impact", headerSize);
  const int Margin = printer.width()/40; //divisor found by trying
  int yPos        = 0;
  QPainter painter;
  painter.begin( &printer );
  QFontMetrics fm = painter.fontMetrics();

  //NOTE from Qt for the drawText: The y-position is used as the baseline of the font
  
  // Header: Store Name, Store Address, Store Phone, Store Logo...
  QFont tmpFont = QFont("Bitstream Vera Sans", textSize);
  QPen normalPen = painter.pen();
  QString text;
  QSize textWidth;
  if (ptInfo.logoOnTop) {
    // Store Logo
    painter.drawPixmap(printer.width()/2 - (ptInfo.storeLogo.width()/2), Margin, ptInfo.storeLogo);
    yPos = yPos + ptInfo.storeLogo.height();
    // Store Name
    painter.setFont(header);
    text = ptInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    yPos = yPos + Margin + textWidth.height();
    painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos, text);
    // Store Address
    tmpFont = QFont("Bitstream Vera Sans", textSize);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    ///Aug 14 2011: If the user enters an '|' in the phone we will add a new line. The user is responsible for text width between new lines.
    QStringList lines = ptInfo.storePhone.split("|", QString::SkipEmptyParts);
    //remove first line to be attached to the ", Phone:".
    QString first = "";
    if (!lines.isEmpty())
        first = lines.takeFirst();
    text = ptInfo.storeAddr + ", " +ptInfo.thPhone + first; //+ ptInfo.storePhone;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    yPos = yPos + fm.lineSpacing();
    painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
    yPos = yPos + fm.lineSpacing();
    //now we print the lines if any..
    if (!lines.isEmpty()) {
        foreach(QString line, lines) {
            text = line;
            textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
            painter.drawText((printer.width()/2)-(textWidth.width()/2) , yPos, text);
            yPos = yPos + fm.lineSpacing(); //ok now we inc yPos.
        }   
    }
  }
  else {
    // Store Logo
    painter.drawPixmap(printer.width() - ptInfo.storeLogo.width() - Margin, Margin*1.75, ptInfo.storeLogo);
    // Store Name
    painter.setFont(header);
    text = ptInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin,Margin+textWidth.height(), text);
    // Store Address
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    yPos = yPos + fm.lineSpacing()+(Margin*0.75);
    text = ptInfo.storeAddr + ", " +ptInfo.thPhone + ptInfo.storePhone;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.setPen(Qt::darkGray);
    painter.drawText(Margin, Margin*2 + yPos + textWidth.height(), text);
    yPos = yPos + fm.lineSpacing()*2;
  }

  //FIXME: Check for logo printing!!!! if no logo, text is missplaced...
  if (ptInfo.storeLogo.isNull())
      yPos = yPos + Margin + fm.lineSpacing();

  // Header line
  painter.setPen(QPen(Qt::black, Margin*0.25, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);
  yPos = yPos + fm.lineSpacing()*2;

  //Date
  tmpFont = QFont("Bitstream Vera Sans", textSize);
  tmpFont.setItalic(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  painter.setPen(normalPen);
  text = ptInfo.thDate ;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin, yPos, text);

  //Ticket Number
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = ptInfo.thTicket;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(printer.width() - textWidth.width() - Margin, yPos, text);
  yPos = yPos + fm.lineSpacing();


  //Vendor, terminal number
  tmpFont.setBold(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = ptInfo.salesPerson + ", " + ptInfo.terminal;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin, yPos, text);
  yPos = yPos + 2*fm.lineSpacing();

  //Print a RESERVATION header if is a starting Reservation.
  if (ptInfo.ticketInfo.isAReservation && ptInfo.ticketInfo.reservationStarted && !ptInfo.ticketInfo.hasSpecialOrders) {
      tmpFont = QFont("Bitstream Vera Sans", textSize);
      tmpFont.setItalic(true);
      tmpFont.setBold(true);
      painter.setFont(tmpFont);
      fm = painter.fontMetrics();
      text = ptInfo.hdrReservation;
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
      yPos = yPos + fm.lineSpacing();
      text =  QString::number(ptInfo.ticketInfo.reservationId);
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos , text); //+textWidth.height()
      yPos = yPos + fm.lineSpacing()*2;
  }


  painter.setPen(Qt::black);
  tmpFont = QFont("Bitstream Vera Sans", textSize);
  tmpFont.setItalic(false);
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  
  // Texts needed to calculate the width used..
  QString textTax = ptInfo.thTax;
  QString textTotal = ptInfo.thTotal;
  QString textPrice = ptInfo.thPrice;
  QString textQty = ptInfo.thQty; //textPrice; //ptInfo.thQty + " x " + textPrice;
  QString textDisc = ptInfo.thDiscount;
  //FIXME: Possible problems with TRANSLATED TEXTS, for example, Tax in Spanish is "Impuesto" but can be used "Imp", but translators DONT KNOW THIS.

  // Products Subheader OFFSETS
  int columnDesc = Margin*2 + (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, textQty).width());
  int columnTotal= printer.width() - 5*Margin - (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, textTotal).width());
  // discount is only printed if there is any discounts on products (not global/ocassional discount)
  int columnDisc = columnTotal - Margin - (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, textDisc).width());
  if  (ptInfo.totDisc <= 0) columnDisc = columnTotal - Margin;

  //Printing Qty
  painter.drawText(Margin, Margin+yPos, textQty);
  //Printing Product
  painter.drawText(columnDesc,Margin+yPos, ptInfo.thProduct);
  //Printing TOTALS
  painter.drawText(columnTotal, Margin + yPos, textTotal);
  yPos = yPos + fm.lineSpacing();
  
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);
  painter.setPen(normalPen);
  tmpFont = QFont("Bitstream Vera Sans", textSize );
  painter.setFont(tmpFont);
  yPos = yPos + fm.lineSpacing();
  // End of Header Information.

  // Content : Each product
  QLocale localeForPrinting;
  for (int i = 0; i < ptInfo.ticketInfo.lines.size(); ++i)
  {
    TicketLineInfo tLine = ptInfo.ticketInfo.lines.at(i);
    QDateTime deliveryDT = ptInfo.ticketInfo.lines.at(i).deliveryDateTime;
    QString  idesc =  tLine.desc;
    QString iprice =  localeForPrinting.toString(tLine.price,'f',2);
    QString iqty   =  localeForPrinting.toString(tLine.qty, 'f', 2);
    QString itax = "";
    if (tLine.tax <= 0)
        itax   =  "E";
    else
        itax   =  localeForPrinting.toString(tLine.tax, 'f', 2);
    
    // We must be aware of the locale. europe uses ',' instead of '.' as the decimals separator.
    // note: QLocale has a method that does this locale aware! if the locale is set correctly
    //NOTE: CHop trailing zeroes to SAVE SPACE!!!!  TODO:Does anyone wants them back???
    if (iqty.endsWith(".00") || iqty.endsWith(",00")) { iqty.chop(3); iqty += ""; /*xqty++;*/}//we chop the trailing zeroes...
    if (itax.endsWith(".00") || itax.endsWith(",00")) { itax.chop(3); }//we chop the trailing zeroes...
    //if (iprice.endsWith(".00") || iprice.endsWith(",00")) { iprice.chop(3); }//we chop the trailing zeroes...
    if (itax != "E" ) itax += " %";
    //FIXME: Si en el ticket hay solo cantidades enteras, entonces hay espacios en blanco de los choped zeroes.
    //       Y asi toma mucho espacio... poner los espacios en blanco solo si hay uno que no termine en .00
    //       Pero no hay forma de saberlo sin hacer una iteracion extra...
    //if ( i == ptInfo.ticketInfo.lines.size()-1   && xqty == i) 
    //iqty = iqty + tLine.unitStr;
    QString idiscount =  localeForPrinting.toString(-(/*tLine.qty**/tLine.disc),'f',2);
    if (tLine.disc <= 0) idiscount = ""; // dont print 0.0
    //if (idiscount.endsWith(".00") || idiscount.endsWith(",00")) { idiscount.chop(3); }//we chop the trailing zeroes...
    QString idue =  localeForPrinting.toString(tLine.total,'f',2);
    //if (idue.endsWith(".00") || idue.endsWith(",00")) { idue.chop(3); }//we chop the trailing zeroes...
    while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, idesc).width()+columnDesc >= columnTotal/* - Margin*/) { idesc.chop(1); }
    
    //first print the qty.
    text = iqty+" x";
    painter.drawText(Margin, Margin+yPos, text);
    //product description...
    painter.drawText(columnDesc, Margin+yPos, idesc); 
    painter.drawText(columnTotal, Margin+yPos, idue);
    yPos = yPos + fm.lineSpacing();
    
    ///Second line. Aug 14 2011.  Printing unitary price, discounts, taxes
    QString discc = "";
    if (idiscount > 0) {
        discc = ", "+idiscount+" "+textDisc;
    }
//    text = "@ "+iprice+ ", "+itax+ " "+ textTax + discc;
//    painter.drawText(Margin*5, Margin + yPos, text);
//    yPos = yPos + fm.lineSpacing();
    
    ///check if there is a Special Order or group, to print contents
    if ( !tLine.geForPrint.isEmpty() ) {
      QStringList tmpList = tLine.geForPrint.split("|");
      QStringList strList;
      tmpList.removeFirst();
      //check text lenght
      double maxL = ((printer.width())-Margin*2);
      foreach(QString strTmp, tmpList) {
        fm = painter.fontMetrics();
        QString strCopy = strTmp;
        double realTrozos = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, strTmp).width() / maxL;
        int trozos   = realTrozos;
        double diff = (realTrozos - trozos);
        if (diff > 0.25 && trozos > 0) trozos += 1;
        int tamTrozo = 0;
        if (trozos > 0) {
          tamTrozo = (strTmp.length()/trozos);
        } else {
          tamTrozo = strTmp.length();
        }
        
        QStringList otherList;
        for (int x = 1; x <= trozos; x++) {
          //we repeat for each trozo
          if (x*(tamTrozo-1) < strCopy.length())
            strCopy.insert(x*(tamTrozo-1), "|  "); //create a section
          otherList = strCopy.split("|");
        }
        if (!otherList.isEmpty()) strList << otherList;
        if (trozos < 1) strList << strTmp;
        //qDebug()<<"Trozos:"<<trozos<<" tamTrozo:"<<tamTrozo<<" realTrozos:"<<QString::number(realTrozos,'f', 2)<<" maxL:"<<maxL<<" strTmp.width in pixels:"<<fm.size(Qt::TextExpandTabs | Qt::TextDontClip, strTmp).width()<<" diff:"<<diff;
      }

      foreach(QString str, strList) {
        painter.drawText(Margin, Margin+yPos, str);
        yPos = yPos + fm.lineSpacing();
      }
      //is payment complete?
//       if ( Margin + yPos > printer.height() - Margin ) {
//         printer.newPage();             // no more room on this page
//         yPos = 0;                       // back to top of page
//       }
    } /// is group or specialorder ?
    //Check if space for the next text line
//     if ( Margin + yPos > printer.height() - Margin ) {
//       printer.newPage();             // no more room on this page
//       yPos = 0;                       // back to top of page
//     }
  } //for each item

    //NOTE: Is it really needed to print a line with the discount?
    //      The discount amount is printed at the end (totals).
    //Check for client discount
    //if (ptInfo.clientDiscMoney > 0) {
    //  text = ptInfo.clientDiscountStr;
    //  tmpFont.setWeight(QFont::Bold);
    //  tmpFont.setItalic(true);
    //  painter.setFont(tmpFont);
    //  painter.drawText(Margin, Margin + yPos , text);
    //  text = localeForPrinting.toString(-ptInfo.clientDiscMoney,'f',2);
    //  painter.drawText(columnDisc, Margin + yPos , text); //printer.width()/3)+columnDisc-50
     // yPos = yPos + fm.lineSpacing();
     // tmpFont.setWeight(QFont::Normal);
     // tmpFont.setItalic(false);
     // painter.setFont(tmpFont);
    //}
    //line separator..
    yPos += Margin;
    painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
    painter.drawLine(Margin, Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);
    
    //Check if space for the next text 3 lines
//     if ( (Margin + yPos +fm.lineSpacing()*3) > printer.height() - Margin ) {
//       printer.newPage();             // no more room on this page
//       yPos = 0;                       // back to top of page
//     }

    //total Articles...
    painter.setPen(normalPen);
    tmpFont.setWeight(QFont::Bold);
    painter.setFont(tmpFont);
    yPos = yPos + fm.lineSpacing();
    text = ptInfo.thArticles;
    painter.drawText(Margin, Margin + yPos , text);
    yPos = yPos + fm.lineSpacing();

    //RESERVATION STUFF
    bool isAReservation = ptInfo.ticketInfo.isAReservation;
    double resPayment   = ptInfo.ticketInfo.reservationPayment;
    if (isAReservation  && !ptInfo.ticketInfo.hasSpecialOrders) {
        QLocale locale;
        QString sp; QString spQty;
        tmpFont = QFont("Bitstream Vera Sans", textSize);
        tmpFont.setWeight(QFont::Bold);
        painter.setFont(tmpFont);
        fm = painter.fontMetrics();
        yPos = yPos + Margin; //just a small space..
        // PURCHASE AMOUNT
        sp     = ptInfo.resTotalAmountStr;
        spQty  = locale.toString(ptInfo.ticketInfo.purchaseTotal, 'f',2);
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, sp);
        painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, sp);
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, spQty);
        painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, spQty);
        yPos = yPos + fm.lineSpacing();
        // PRE PAYMENT.
        sp     = ptInfo.paymentStrPrePayment;
        spQty  = locale.toString(resPayment,'f',2); // THE AMOUNT PRE PAID
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, sp);
        painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, sp);
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, spQty);
        painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, spQty);
        yPos = yPos + fm.lineSpacing()*3;
    }
    
    // NOW SPECIAL ORDERS STUFF
    //BEGIN The PRE-PAYMENT/NEXT-PAYMENT DATA.
    bool isSO = ptInfo.ticketInfo.hasSpecialOrders;
    bool isCompletingSO = ptInfo.ticketInfo.completingSpecialOrder;
    QLocale locale;
    if (isSO) {
      QString sp; QString sp2; QString spQty; QString sp2Qty;
      if (isCompletingSO) {
        sp     = ptInfo.paymentStrComplete;
        spQty  = locale.toString(ptInfo.ticketInfo.total,'f',2); // THE DEBT of the SO.
        sp2    = ptInfo.lastPaymentStr;
        sp2Qty = locale.toString(ptInfo.ticketInfo.soTotal - ptInfo.ticketInfo.total , 'f',2); // the prepayment
      }
      else {
        // SO is in pre payment
        sp = ptInfo.paymentStrPrePayment;
        spQty  = locale.toString(ptInfo.ticketInfo.total, 'f',2); // prepayment
        sp2    = ptInfo.nextPaymentStr;
        sp2Qty = locale.toString(ptInfo.ticketInfo.soTotal-ptInfo.ticketInfo.total, 'f',2); // Next payment
      }
      if (!isSO) sp  = ""; ///hack!
      //change font for the PAYMENT MSG
      tmpFont = QFont("Bitstream Vera Sans", textSize );
      tmpFont.setWeight(QFont::Bold);
      painter.setFont(tmpFont);
      fm = painter.fontMetrics();
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, sp);
      painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, sp);
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, spQty);
      painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, spQty);
      yPos = yPos + fm.lineSpacing();
      //print the negative qty for the payment.
      // The negative qty is one of the next:
      //     * When completing the SO: the amount of the pre-payment.
      //     * When starting the SO:   the remaining amount to pay.
      sp2 = (ptInfo.ticketInfo.soTotal-ptInfo.ticketInfo.total >0) ? sp2 : "";
      if (!isSO) sp2  = ""; ///hack!
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, sp2);
      painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, sp2);
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, sp2Qty);
      painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, sp2Qty);
      yPos = yPos + fm.lineSpacing();
      ///Check for delivery date and if its a SO
      if (isSO) {
        sp = ptInfo.deliveryDateStr; // + deliveryDT.toString("ddd d MMMM, h:m ap"); //TODO:hey i18n stuff!!!
        tmpFont = QFont("Bitstream Vera Sans", textSize );
        tmpFont.setWeight(QFont::Bold);
        painter.setFont(tmpFont);
        fm = painter.fontMetrics();
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, sp);
        painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, sp);
        sp = ptInfo.ticketInfo.deliveryDT.toString("ddd d MMM");
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, sp);
        painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, sp);
        sp = ptInfo.ticketInfo.deliveryDT.toString("h:m ap");
        yPos = yPos + fm.lineSpacing();
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, sp);
        painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, sp);
        yPos = yPos + fm.lineSpacing();
      }
      yPos = yPos + fm.lineSpacing()*2;
      //END The PRE-PAYMENT/NEXT-PAYMENT DATA.
    }
    
    tmpFont = QFont("Bitstream Vera Sans", textSize );
    tmpFont.setWeight(QFont::Normal);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();

    //NOTE:
    //When starting a reservation, the amount paid is only an advance payment, not the whole.
    //When complelting the reservation (payment) the taxes are added to the remaining amount to pay.
    //So, when starting the reservation, we do not print details about taxes (subtotal taxes and total)
    //because those amounts will not check.
    //When completing reservation, the details WILL be printed.
    //ALSO SEE NOTE on the balance printing when are reservations in the transactions.
    bool printDetails = true;
    if (ptInfo.ticketInfo.isAReservation && ptInfo.ticketInfo.reservationStarted)
        printDetails = false;
    
    if ( printDetails ) {
        //subtotals
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thSubtotal);
        painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thSubtotal);
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.subtotal);
        painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, ptInfo.subtotal);
        yPos = yPos + fm.lineSpacing();
        if (ptInfo.totDisc >0) {
            textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thDiscount);
            painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thDiscount);
            textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.tDisc);
            painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, ptInfo.tDisc);
            yPos = yPos + fm.lineSpacing();
        }
        //taxes
   //   textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thTax);
   //     painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thTax);
//        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.taxes);
//        painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, ptInfo.taxes);
//        yPos = yPos + fm.lineSpacing();
       //draw a total line
        painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        painter.drawLine((printer.width() - (printer.width()/3)) , Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);
        yPos = yPos + fm.lineSpacing();
        painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
        //grand total
        tmpFont = QFont("Bitstream Vera Sans", textSize );
        tmpFont.setWeight(QFont::Bold);
        painter.setFont(tmpFont);
        fm = painter.fontMetrics();
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thTotal);
        painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thTotal);
        textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thTotals);
        painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, ptInfo.thTotals);
    }
    
    yPos = yPos + fm.lineSpacing()*2;
    // Tendered with and change
    tmpFont = QFont("Bitstream Vera Sans", textSize );
    tmpFont.setWeight(QFont::Normal);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
//    //TENDERED
//    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thTendered);
//    painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thTendered);
//    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thPaid);
//    painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, ptInfo.thPaid);
//    yPos = yPos + fm.lineSpacing();
    //CHANGE
  //  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thChangeStr);
    //   painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thChangeStr);
     //  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thChange);
     //  painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, ptInfo.thChange);
     //  yPos = yPos + fm.lineSpacing();
    //TODO: Verify THIS!!!
//    if (ptInfo.ticketInfo.paidWithCard) {
//      painter.setFont(tmpFont);
//      text = ptInfo.thCard;
//      painter.drawText(Margin, Margin + yPos , text);
//      yPos = yPos + fm.lineSpacing();
//      painter.setFont(tmpFont);
//      text = ptInfo.thCardAuth;
//      painter.drawText(Margin, Margin + yPos , text);
//      yPos = yPos + fm.lineSpacing();
//    }
    
    //Points if is not the default user TODO: configure this to allow or not to print this info in case the store does not use points
//    if (ptInfo.ticketInfo.clientid > 1) { //no default user
//      yPos = yPos + fm.lineSpacing();
//      QStringList strPoints = ptInfo.thPoints.split("|");
//      foreach(QString strTmp, strPoints) {
//        painter.drawText(Margin, Margin+yPos, strTmp);
//        yPos = yPos + fm.lineSpacing();
//      }
//    }
//     if ( (Margin + yPos +fm.lineSpacing()*2) > printer.height() - Margin ) {
//       printer.newPage();             // no more room on this page
//       yPos = 0;                       // back to top of page
//     }
//Balance
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thBalanceStr);
    painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thBalanceStr);
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thBalance);
    painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos,ptInfo.thBalance);
    yPos = yPos + fm.lineSpacing();
//Monthly
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thMonthlyStr);
    painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thMonthlyStr);
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip,ptInfo.thMonthly);
    painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos,ptInfo.thMonthly);
    yPos = yPos + 2*fm.lineSpacing();

 //Expiry
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thExpiryStr);
    painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thExpiryStr);
    yPos = yPos +  fm.lineSpacing();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip,ptInfo.thExpiry);
    painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, ptInfo.thExpiry);
    yPos = yPos +  fm.lineSpacing();

    //Reset
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.thNextResetStr);
    painter.drawText(printer.width()-(printer.width()/3)-textWidth.width(), Margin+yPos, ptInfo.thNextResetStr);
    yPos = yPos +  fm.lineSpacing();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip,ptInfo.thNextReset);
    painter.drawText((printer.width() - textWidth.width() - Margin), Margin+yPos, ptInfo.thNextReset);
    yPos = yPos +  fm.lineSpacing();

    //Random Message
    double maxL = ((printer.width())-Margin*2);
    QStringList strList;
    QString strTmp = ptInfo.randomMsg;
    if (!ptInfo.randomMsg.isEmpty()) {
      tmpFont = QFont("Bitstream Vera Sans", textSize);
      tmpFont.setItalic(true);
      painter.setFont(tmpFont);
      fm = painter.fontMetrics();
      yPos = yPos + fm.lineSpacing()*2;
        //fixing message lenght
        QString strCopy = strTmp;
        double realTrozos = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, strTmp).width() / maxL;
        int trozos   = realTrozos;
        double diff = (realTrozos - trozos);
        if (diff > 0.25 && trozos > 0) trozos += 1;
        int tamTrozo = 0;
        if (trozos > 0) {
          tamTrozo = (strTmp.length()/trozos);
        } else {
          tamTrozo = strTmp.length();
        }

        QStringList otherList;
        for (int x = 1; x <= trozos; x++) {
          //we repeat for each trozo
          if (x*(tamTrozo-1) < strCopy.length())
            strCopy.insert(x*(tamTrozo-1), "|  "); //create a section
          otherList = strCopy.split("|");
        }
        if (!otherList.isEmpty()) strList << otherList;
        if (trozos < 1) strList << strTmp;
        //qDebug()<<"rm : Trozos:"<<trozos<<" tamTrozo:"<<tamTrozo<<" realTrozos:"<<QString::number(realTrozos,'f', 2)<<" maxL:"<<maxL<<" strTmp.width in pixels:"<<fm.size(Qt::TextExpandTabs | Qt::TextDontClip, strTmp).width()<<" diff:"<<diff;
        //end if fixing the lenght of the message.
      foreach(QString str, strList) {
        painter.drawText(Margin, Margin+yPos, str);
        yPos = yPos + fm.lineSpacing();
      }
    }

    // The ticket message (tanks...)
    tmpFont = QFont("Bitstream Vera Sans", textSize);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    yPos = yPos + fm.lineSpacing()*2;
    painter.drawText((printer.width()/2)-(fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.ticketMsg).width()/2)-Margin, Margin+yPos, ptInfo.ticketMsg);

    result = painter.end();// this makes the print job start
      
  return result;
}


bool PrintCUPS::printBigTicket(const PrintTicketInfo &ptInfo, QPrinter &printer)
{
  bool result = false;
  QFont header = QFont("Impact", 30);
  QFont sectionHeader = QFont("Bitstream Vera Sans", 14);
  const int Margin = 30;
  
  
  QPainter painter;
  painter.begin( &printer );
  
  
  int yPos        = 0;
  QFontMetrics fm = painter.fontMetrics();
  
  // Header: Store Name, Store Address, Store Phone, Store Logo...
  painter.setFont(header);
  QString text = ptInfo.storeName;
  QSize textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin,Margin*2+textWidth.height(), text);
  yPos = yPos + fm.lineSpacing();
  // Store Address
  QFont tmpFont = QFont("Bitstream Vera Sans", 10);
  tmpFont.setItalic(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  QPen normalPen = painter.pen();
  painter.setPen(Qt::darkGray);
  painter.drawText(Margin, Margin*2 + yPos + textWidth.height() -5, printer.width(), fm.lineSpacing(), Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.storeAddr + ", " +ptInfo.thPhone + ptInfo.storePhone);
  yPos = yPos + fm.lineSpacing();
  // Store Logo
  painter.drawPixmap(printer.width() - ptInfo.storeLogo.width() - Margin, Margin, ptInfo.storeLogo);
  // Header line
  painter.setPen(QPen(Qt::gray, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, 105, printer.width()-Margin, 105);
  yPos = yPos + 3 * fm.lineSpacing(); // 3times the height of the line
  // Ticket Number, Date
  painter.setPen(normalPen);
  text = ptInfo.thDate;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(printer.width()-Margin-textWidth.width(), Margin + yPos +textWidth.height(), text);
  yPos = yPos + fm.lineSpacing();
  // Vendor name, terminal number
  text = ptInfo.salesPerson + ", " + ptInfo.terminal;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(printer.width()-Margin-textWidth.width(), Margin + yPos +textWidth.height(), text);
  yPos = yPos + 3*fm.lineSpacing();
  // Products Subheader
  int columnQty  = 0;
  if (ptInfo.totDisc <= 0) {
    columnQty  = 90;
  }
  int columnPrice= columnQty+100;
  int columnDisc = columnPrice+100;
  int columnTotal = 0;
  if ( ptInfo.totDisc >0 )
    columnTotal = columnDisc+100;
  else
    columnTotal = columnPrice+100;
  
  painter.setPen(Qt::darkBlue);
  tmpFont = QFont("Bitstream Vera Sans", 10 );
  tmpFont.setWeight(QFont::Bold);
  painter.setFont(tmpFont);
  painter.drawText(Margin,Margin+yPos, ptInfo.thProduct);
  text = ptInfo.thQty;
  painter.drawText(printer.width()/2+columnQty, Margin + yPos, text);
  text = ptInfo.thPrice;
  painter.drawText(printer.width()/2+columnPrice, Margin + yPos, text);
  text = ptInfo.thDiscount;
  if (ptInfo.totDisc > 0)
    painter.drawText(printer.width()/2+columnDisc, Margin + yPos, text);
  text = ptInfo.thTotal;
  painter.drawText(printer.width()/2+columnTotal, Margin + yPos, text);
  yPos = yPos + fm.lineSpacing();
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);
  painter.setPen(normalPen);
  painter.setFont(QFont("Bitstream Vera Sans", 10 ));
  yPos = yPos + fm.lineSpacing();
  // End of Header Information.
  
  // Content : Each product
  QLocale localeForPrinting;
  for (int i = 0; i < ptInfo.ticketInfo.lines.size(); ++i)
  {
    //Check if space for the next text line
    if ( Margin + yPos > printer.height() - Margin ) {
      printer.newPage();             // no more room on this page
      yPos = 0;                       // back to top of page
    }
    TicketLineInfo tLine = ptInfo.ticketInfo.lines.at(i);
    QString  idesc =  tLine.desc;
    QString iprice =  localeForPrinting.toString(tLine.price,'f',2);
    QString iqty   =  localeForPrinting.toString(tLine.qty, 'f', 2);
    // We must be aware of the locale. europe uses ',' instead of '.' as the decimals separator.
    if (iqty.endsWith(".00") || iqty.endsWith(",00")) { iqty.chop(3); iqty += "   ";}//we chop the trailing zeroes...
    iqty = iqty+" "+tLine.unitStr;
    QString idiscount =  localeForPrinting.toString(-(tLine.qty*tLine.disc),'f',2);
    if (tLine.disc <= 0) idiscount = ""; // dont print 0.0
    QString idue =  localeForPrinting.toString(tLine.total,'f',2);
    if ( ptInfo.totDisc > 0 )
      while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, idesc).width() >= ((printer.width()/2)-Margin-40)) { idesc.chop(2); }
    else
      while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, idesc).width() >= ((printer.width()/2)-Margin-40+90)) { idesc.chop(2); }
    painter.drawText(Margin, Margin+yPos, idesc); //first product description...
    text = iqty;
    painter.drawText(printer.width()/2+columnQty, Margin+yPos, text);
    text = iprice;
    painter.drawText(printer.width()/2+columnPrice, Margin+yPos, text);
    if ( ptInfo.totDisc > 0 ) {
      text = idiscount;
      if (text == "0.0") text = ""; //dont print a 0.0 !!!
      painter.drawText(printer.width()/2+columnDisc, Margin+yPos, text);
    }
    text = idue;
    painter.drawText(printer.width()/2+columnTotal, Margin+yPos, text);
    yPos = yPos + fm.lineSpacing();
  } //for each item
  
  //now the totals...
  //Check if space for the next text 3 lines
  if ( (Margin + yPos +fm.lineSpacing()*3) > printer.height() - Margin ) {
    printer.newPage();             // no more room on this page
    yPos = 0;                       // back to top of page
  }
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);
  painter.setPen(normalPen);
  painter.setFont(tmpFont);
  yPos = yPos + fm.lineSpacing();
  if ( ptInfo.totDisc >0 )
    painter.drawText((printer.width()/2)+columnDisc, Margin + yPos , ptInfo.tDisc);
  painter.drawText((printer.width()/2)+columnQty, Margin + yPos , ptInfo.thArticles);
  painter.drawText((printer.width()/2)+columnTotal, Margin+yPos, ptInfo.thTotals);
  yPos = yPos + fm.lineSpacing();
  // NOTE: I think its redundant to say again the savings.
  //if (tDisc > 0) {
 //    painter.drawText(Margin, Margin + yPos , line = i18n("You saved %1", KGlobal::locale()->formatMoney(tDisc, QString(), 2)););
 //    yPos = yPos + fm.lineSpacing();
 //}
 
 //if space, the ticket message.
 if ( (Margin + yPos +fm.lineSpacing()*4) <= printer.height() - Margin ) {
   tmpFont = QFont("Bitstream Vera Sans", 12);
   tmpFont.setItalic(true);
   painter.setPen(Qt::darkGreen);
   painter.setFont(tmpFont);
   yPos = yPos + fm.lineSpacing()*4;
   painter.drawText((printer.width()/2)-(fm.size(Qt::TextExpandTabs | Qt::TextDontClip, ptInfo.ticketMsg).width()/2)-Margin, Margin+yPos, ptInfo.ticketMsg);
 }
 
 result = painter.end();// this makes the print job start
 
 return result;
 }


bool PrintCUPS::printBigEndOfDay(const PrintEndOfDayInfo &pdInfo, QPrinter &printer)
{
  bool result = false;
  QFont header = QFont("Impact", 25);
  const int Margin = 20;
  int yPos        = 0;
  QPainter painter;
  painter.begin( &printer );
  QFontMetrics fm = painter.fontMetrics();
  
  //NOTE from Qt for the drawText: The y-position is used as the baseline of the font
  
  QFont tmpFont = QFont("Bitstream Vera Sans", 14);
  QPen normalPen = painter.pen();
  QString text;
  QSize textWidth;
  if (pdInfo.logoOnTop) {
    // Store Logo
    painter.drawPixmap(printer.width()/2 - (pdInfo.storeLogo.width()/2), Margin, pdInfo.storeLogo);
    yPos = yPos + pdInfo.storeLogo.height();
    // Store Name
    painter.setFont(header);
    text = pdInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos+Margin+textWidth.height(), text);
    yPos = yPos + textWidth.height()+5;
    tmpFont = QFont("Bitstream Vera Sans", 14);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    painter.setPen(Qt::darkGray);
    text = pdInfo.storeAddr;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos+Margin+textWidth.height(), text);
    yPos = yPos + textWidth.height()+20;
  }
  else {
    // Store Logo
    painter.drawPixmap(printer.width() - pdInfo.storeLogo.width() - Margin, Margin+20, pdInfo.storeLogo);
    // Store Name
    painter.setFont(header);
    text = pdInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin,Margin+textWidth.height(), text);
    yPos = yPos + textWidth.height()+5;
    tmpFont = QFont("Bitstream Vera Sans", 14);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    painter.setPen(Qt::darkGray);
    text = pdInfo.storeAddr;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin,yPos+Margin+textWidth.height(), text);
    yPos = yPos + fm.lineSpacing()+20;
  }
  
  // Header line
  painter.setPen(QPen(Qt::gray, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin+yPos, printer.width()-Margin, Margin+yPos);
  tmpFont = QFont("Bitstream Vera Sans", 18);
  tmpFont.setItalic(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  //title
  text = pdInfo.thTitle;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), Margin + yPos  +textWidth.height()+15, text);
  yPos = yPos + 2*fm.lineSpacing();
  //Date
  tmpFont = QFont("Bitstream Vera Sans", 13);
  painter.setFont(tmpFont);
  painter.setPen(normalPen);
  fm = painter.fontMetrics();
  yPos = yPos + fm.lineSpacing();
  text = pdInfo.thDate;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), Margin + yPos +textWidth.height(), text);
  yPos = yPos + fm.lineSpacing();
  text = pdInfo.salesPerson +" "+ pdInfo.terminal;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), Margin + yPos +textWidth.height(), text);
  yPos = yPos + 2*fm.lineSpacing();
  
  // Transaction Headers
  tmpFont = QFont("Bitstream Vera Sans", 10);
  painter.setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = pdInfo.thTicket;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin+30, Margin + yPos +textWidth.height(), text);
  text = pdInfo.thTime;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin+130, Margin + yPos +textWidth.height(), text);
  text = pdInfo.thAmount;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin+220, Margin + yPos +textWidth.height(), text);
  text = pdInfo.thProfit;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin+450, Margin + yPos +textWidth.height(), text);
  text = pdInfo.thPayMethod;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
  yPos = yPos + 2*fm.lineSpacing();
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);
  painter.setPen(normalPen);
  
  //TRANSACTION DETAILS
  
  tmpFont.setBold(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  painter.setPen(QPen(normalPen));
  yPos = yPos + fm.lineSpacing();
  //Iterating each transaction
  foreach(QString trStr, pdInfo.trLines) {
    QStringList data = trStr.split("|");
    if ( (Margin + yPos +fm.lineSpacing()) > printer.height() - Margin ) {
      printer.newPage();              // no more room on this page
      yPos = 0;                       // back to top of page
    }
    //we have 5 fields in the string [ORDER] ID, HOUR, AMOUNT, PROFIT, PAYMETHOD
    text = data.at(0); // ID
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin+10, Margin + yPos +textWidth.height(), text);
    text = data.at(1); // HOUR
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin+130, Margin + yPos +textWidth.height(), text);
    text = data.at(2); // AMOUNT
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin+220, Margin + yPos +textWidth.height(), text);
    text = data.at(3); // PROFIT
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin+450, Margin + yPos +textWidth.height(), text);
    text = data.at(4); // PAYMENT METHOD
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
    yPos = yPos + fm.lineSpacing();
  }
  
  yPos = yPos + fm.lineSpacing();
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);

  if ( (Margin + yPos +fm.lineSpacing()) > printer.height() - Margin ) {
    printer.newPage();             // no more room on this page
    yPos = 0;                       // back to top of page
  }
  
  //TOTALS
  tmpFont.setBold(true);
  tmpFont.setItalic(true);
  painter.setFont(tmpFont);
  painter.setPen(QPen(Qt::black, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  yPos = yPos + fm.lineSpacing();
  text = QString::number(pdInfo.trLines.count()); //transaction count
  painter.drawText(Margin+10, Margin + yPos , text);
  text = pdInfo.thTotalSales; // sales
  painter.drawText(Margin+210, Margin + yPos , text);
  text = pdInfo.thTotalProfit; // profit
  painter.drawText(Margin+440, Margin + yPos , text);

  result = painter.end();
  return result;
}

bool PrintCUPS::printSmallEndOfDay(const PrintEndOfDayInfo &pdInfo, QPrinter &printer)
{
  bool result = false;
  // calculate font size
  const int headerSize = printer.width()/16; //divisor found by trying, maybe has to do with font metrics?
  const int textSize = headerSize/2; //i think the half of header size is ok
  qDebug()<<"Paper Width: "<<printer.widthMM()<<"mm"<<"; Page Width: "<<printer.width()<<"; calculated headerSize: "<<headerSize;
  
  QFont header = QFont("Impact", headerSize);
  const int Margin = printer.width()/40;
  int yPos        = 0;
  QPainter painter;
  painter.begin( &printer );
  QFontMetrics fm = painter.fontMetrics();
  
  //NOTE from Qt for the drawText: The y-position is used as the baseline of the font
  
  QFont tmpFont = QFont("Bitstream Vera Sans", textSize);
  QPen normalPen = painter.pen();
  QString text;
  QSize textWidth;

  if (pdInfo.logoOnTop) {
    // Store Logo
    painter.drawPixmap(printer.width()/2 - (pdInfo.storeLogo.width()/2), Margin, pdInfo.storeLogo);
    yPos = yPos + pdInfo.storeLogo.height();
    // Store Name
    painter.setFont(header);
    text = pdInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    yPos = yPos + Margin + textWidth.height();
    painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos, text);
    tmpFont = QFont("Bitstream Vera Sans", textSize);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    text = pdInfo.storeAddr;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    yPos = yPos + fm.lineSpacing();
    painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
    yPos = yPos + fm.lineSpacing()*2;
  }
  else {
    // Store Logo
    painter.drawPixmap(printer.width() - pdInfo.storeLogo.width() - Margin, Margin*1.75, pdInfo.storeLogo);
    // Store Name
    painter.setFont(header);
    text = pdInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin,Margin+textWidth.height(), text);
    // Store Address
    tmpFont = QFont("Bitstream Vera Sans", textSize);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    yPos = yPos + fm.lineSpacing()+(Margin*0.75);
    text = pdInfo.storeAddr;
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.setPen(Qt::darkGray);
    painter.drawText(Margin, yPos, text);
    yPos = yPos + fm.lineSpacing()*2;
  }

  //FIXME: Check for logo printing!!!! if no logo, text is missplaced...
  if (pdInfo.storeLogo.isNull())
      yPos = yPos + Margin + fm.lineSpacing();
  
  painter.setFont(header);
  fm = painter.fontMetrics();
  //title
  text = pdInfo.thTitle;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
  //Date
  painter.setFont(tmpFont);
  painter.setPen(normalPen);
  fm = painter.fontMetrics();
  yPos = yPos + 2*fm.lineSpacing();
  text = pdInfo.thDate;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
  yPos = yPos + fm.lineSpacing();
  text = pdInfo.salesPerson +" "+ pdInfo.terminal;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
  yPos = yPos + fm.lineSpacing()*2;

  int tmpOffset = (printer.width()/5);
  // Transaction Headers
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = pdInfo.thTicket;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin, yPos, text);
  text = pdInfo.thTime;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(tmpOffset, yPos, text);
  text = pdInfo.thAmount;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((tmpOffset*2)-Margin*2, yPos, text);
  text = pdInfo.thProfit;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((tmpOffset*3), yPos, text);
  //text = pdInfo.thPayMethod;
  //textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  //painter.drawText(tmpOffset*4, yPos, text);//printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
  yPos = yPos + fm.lineSpacing();
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);
  painter.setPen(normalPen);
  
  //TRANSACTION DETAILS
  
  tmpFont.setBold(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  painter.setPen(QPen(normalPen));
  yPos = yPos + fm.lineSpacing();
  //Iterating each transaction
  foreach(QString trStr, pdInfo.trLines) {
    QStringList data = trStr.split("|");
    //we have 5 fields in the string [ORDER] ID, HOUR, AMOUNT, PROFIT, PAYMETHOD
    text = data.at(0); // ID
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin, yPos, text);
    text = data.at(1); // HOUR
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(tmpOffset, yPos, text);
    text = data.at(2); // AMOUNT
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((tmpOffset*2)-Margin*2, yPos, text);
    text = data.at(3); // PROFIT
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(tmpOffset*3, yPos, text);
    text = data.at(4); // PAYMENT METHOD
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(printer.width()-Margin-textWidth.width(), yPos, text);
    yPos = yPos + fm.lineSpacing();
  }
  
  yPos = yPos + fm.lineSpacing();
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);


  //TOTALS
  tmpFont.setBold(true);
  tmpFont.setItalic(true);
  painter.setFont(tmpFont);
  yPos = yPos + fm.lineSpacing();
  text = QString::number(pdInfo.trLines.count()); //transaction count
  painter.drawText(Margin, yPos , text);
  text = pdInfo.thTotalSales; // sales
  painter.drawText((tmpOffset*2)-Margin*2, yPos , text);
  text = pdInfo.thTotalProfit; // profit
  painter.drawText(tmpOffset*3, yPos , text);

  //print taxes for all sales
  yPos = yPos + fm.lineSpacing()*2;
  QStringList tL = pdInfo.thTotalTaxes.split(':');
  text = tL.first();
  painter.drawText(Margin, yPos , text);
  yPos = yPos + fm.lineSpacing();
  text = tL.last();
  painter.drawText(Margin, yPos , text);

  result = painter.end();
  return result;
}

bool PrintCUPS::printSmallLowStockReport(const PrintLowStockInfo &plInfo, QPrinter &printer)
{
  bool result = false;
  // calculate font size
  const int headerSize = printer.width()/16; //divisor found by trying, maybe has to do with font metrics?
  const int textSize = headerSize/2; //i think the half of header size is ok
  qDebug()<<"Paper Size: "<<printer.widthMM()<<"mm x "<<printer.heightMM()<<" -- calculated headerSize: "<<headerSize;
  
  QFont header = QFont("Impact", headerSize);
  const int Margin = printer.width()/40;
  int yPos        = 0;
  QPainter painter;
  painter.begin( &printer );
  QFontMetrics fm = painter.fontMetrics();
  
  //NOTE from Qt for the drawText: The y-position is used as the baseline of the font
  
  QFont tmpFont = QFont("Bitstream Vera Sans", textSize);
  QPen normalPen = painter.pen();
  QString text;
  QSize textWidth;
  
  if (plInfo.logoOnTop) {
      // Store Logo
      painter.drawPixmap(printer.width()/2 - (plInfo.storeLogo.width()/2), Margin, plInfo.storeLogo);
      yPos = yPos + plInfo.storeLogo.height();
      // Store Name
      painter.setFont(header);
      text = plInfo.storeName;
      fm = painter.fontMetrics();
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      yPos = yPos + Margin + textWidth.height();
      painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos, text);
      tmpFont.setItalic(true);
      painter.setFont(tmpFont);
      fm = painter.fontMetrics();
      text = plInfo.storeAddr;
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      yPos = yPos + fm.lineSpacing();
      painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
      yPos = yPos + fm.lineSpacing()*2;
  }
  else {
      // Store Logo
      painter.drawPixmap(printer.width() - plInfo.storeLogo.width() - Margin, Margin*1.75, plInfo.storeLogo);
      // Store Name
      painter.setFont(header);
      text = plInfo.storeName;
      fm = painter.fontMetrics();
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.drawText(Margin,Margin+textWidth.height(), text);
      // Store Address
      tmpFont.setItalic(true);
      painter.setFont(tmpFont);
      fm = painter.fontMetrics();
      yPos = yPos + fm.lineSpacing()+(Margin*0.75);
      text = plInfo.storeAddr;
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.setPen(Qt::darkGray);
      painter.drawText(Margin, yPos, text);
      yPos = yPos + fm.lineSpacing()*2;
  }
  
  //FIXME: Check for logo printing!!!! if no logo, text is missplaced...
  if (plInfo.storeLogo.isNull())
      yPos = yPos + Margin + fm.lineSpacing();
  
  // Header line
  //painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);
  tmpFont = QFont("Bitstream Vera Sans", textSize+2);
  tmpFont.setItalic(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  //title
  text = plInfo.hTitle;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
  yPos = yPos + 2*fm.lineSpacing();
  //Date
  tmpFont = QFont("Bitstream Vera Sans", textSize);
  painter.setFont(tmpFont);
  painter.setPen(normalPen);
  fm = painter.fontMetrics();
  yPos = yPos + fm.lineSpacing();
  text = plInfo.hDate;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), yPos, text);
  yPos = yPos + 2*fm.lineSpacing();

  int tmpOffset = printer.width()/3.6;
  // Content Headers
  tmpFont = QFont("Bitstream Vera Sans", textSize-2);//NOTE: Making smaller font, its much text on each field to print.
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  //code
  text = plInfo.hCode;
  painter.drawText(Margin, yPos, text);
  //Description
  text = plInfo.hDesc;
  while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= tmpOffset+(tmpOffset*2)+Margin*6) { text.chop(2); }
  painter.drawText(tmpOffset, yPos, text);
  //Qty
  text = plInfo.hQty;
  //while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= (tmpOffset*2)+(tmpOffset*3)+Margin*2) { text.chop(2); }
  painter.drawText((tmpOffset*2)+Margin*7, yPos, text);
  //SOLD UNITS
  text = plInfo.hSoldU;
  //while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= ((tmpOffset*3)+Margin*2)+tmpOffset*4) { text.chop(2); }
  painter.drawText((tmpOffset*3)+Margin*2, yPos, text);//printer.width()-Margin-textWidth.width(), yPos, text);
  yPos = yPos + 2*fm.lineSpacing();
  //line
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);

  
  //PRODUCTS DETAILS
  
  tmpFont.setBold(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  yPos = yPos + fm.lineSpacing();
  //Iterating each product
  //TODO: Maybe, paint a background color in odd rows to make it more readable.
  foreach(QString trStr, plInfo.pLines) {
    QStringList data = trStr.split("|");
    //we have 5 fields in the string  CODE, DESC, STOCK QTY, UNITSTR, SOLDUNITS
    text = data.at(0); // CODE
    painter.drawText(Margin, yPos, text);
    text = data.at(1); // DESCRIPTION
    while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= (tmpOffset*2)-Margin*6) { text.chop(2); }
    painter.drawText(tmpOffset, yPos, text);
    text = data.at(2); // STOCK QTY
    // We must be aware of the locale. europe uses ',' instead of '.' as the decimals separator.
    if (text.endsWith(".00") || text.endsWith(",00")) { text.chop(3); text += "   ";}//we chop the trailing zeroes...
    text = text; //+" "+ data.at(3); // 10 pieces
    painter.drawText((tmpOffset*2)+Margin*7, yPos, text);
    text = data.at(4); // SOLD UNITS
    // We must be aware of the locale. europe uses ',' instead of '.' as the decimals separator.
    if (text.endsWith(".00") || text.endsWith(",00")) { text.chop(3); text += "   ";}//we chop the trailing
    painter.drawText((tmpOffset*3)+Margin*2, yPos, text);//printer.width()-textWidth.width()-Margin, yPos, text);
    yPos = yPos + fm.lineSpacing();
  }
  
  yPos = yPos + fm.lineSpacing();
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);

  result = painter.end();
  return result;
}

bool PrintCUPS::printBigLowStockReport(const PrintLowStockInfo &plInfo, QPrinter &printer)
{
  bool result = false;
  QFont header = QFont("Impact", 25);
  const int Margin = 20;
  int yPos        = 0;
  QPainter painter;
  painter.begin( &printer );
  QFontMetrics fm = painter.fontMetrics();
  
  //NOTE from Qt for the drawText: The y-position is used as the baseline of the font
  
  QFont tmpFont = QFont("Bitstream Vera Sans", 14);
  QPen normalPen = painter.pen();
  QString text;
  QSize textWidth;
  if (plInfo.logoOnTop) {
    // Store Logo
    painter.drawPixmap(printer.width()/2 - (plInfo.storeLogo.width()/2), Margin, plInfo.storeLogo);
    yPos = yPos + plInfo.storeLogo.height();
    // Store Name
    painter.setFont(header);
    text = plInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos+Margin+textWidth.height(), text);
    yPos = yPos + textWidth.height()+5;
    tmpFont = QFont("Bitstream Vera Sans", 14);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    painter.setPen(Qt::darkGray);
    text = plInfo.storeAddr;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos+Margin+textWidth.height(), text);
    yPos = yPos + textWidth.height()+20;
  }
  else {
    // Store Logo
    painter.drawPixmap(printer.width() - plInfo.storeLogo.width() - Margin, Margin+20, plInfo.storeLogo);
    // Store Name
    painter.setFont(header);
    text = plInfo.storeName;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin,Margin+textWidth.height(), text);
    yPos = yPos + textWidth.height()+5;
    tmpFont = QFont("Bitstream Vera Sans", 14);
    tmpFont.setItalic(true);
    painter.setFont(tmpFont);
    painter.setPen(Qt::darkGray);
    text = plInfo.storeAddr;
    fm = painter.fontMetrics();
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin,yPos+Margin+textWidth.height(), text);
    yPos = yPos + fm.lineSpacing()+20;
  }
  
  // Header line
  painter.setPen(QPen(Qt::gray, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin+yPos, printer.width()-Margin, Margin+yPos);
  tmpFont = QFont("Bitstream Vera Sans", 18);
  tmpFont.setItalic(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  //title
  text = plInfo.hTitle;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), Margin + yPos  +textWidth.height()+15, text);
  yPos = yPos + 2*fm.lineSpacing();
  //Date
  tmpFont = QFont("Bitstream Vera Sans", 13);
  painter.setFont(tmpFont);
  painter.setPen(normalPen);
  fm = painter.fontMetrics();
  yPos = yPos + fm.lineSpacing();
  text = plInfo.hDate;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2), Margin + yPos +textWidth.height(), text);
  yPos = yPos + 2*fm.lineSpacing();
  
  // Content Headers
  tmpFont = QFont("Bitstream Vera Sans", 10);
  painter.setPen(QPen(Qt::blue, 5, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = plInfo.hCode;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin+30, Margin + yPos +textWidth.height(), text);
  text = plInfo.hDesc;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin+160, Margin + yPos +textWidth.height(), text);
  text = plInfo.hQty;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin+450, Margin + yPos +textWidth.height(), text);
  //text = plInfo.hUnitStr;
  //textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  //painter.drawText(printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
  text = plInfo.hSoldU;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(printer.width()-Margin-textWidth.width(), Margin + yPos +textWidth.height(), text);
  yPos = yPos + 2*fm.lineSpacing();
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);
  painter.setPen(normalPen);
  
  //PRODUCTS DETAILS
  
  tmpFont.setBold(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  painter.setPen(QPen(normalPen));
  yPos = yPos + fm.lineSpacing();
  //Iterating each product
  foreach(QString trStr, plInfo.pLines) {
    if ( Margin + yPos > printer.height() - Margin ) {
      printer.newPage();                        // no more room on this page
      yPos = 0;                                 // back to top of page
    }
    QStringList data = trStr.split("|");
    //we have 5 fields in the string  CODE, DESC, STOCK QTY, UNITSTR, SOLDUNITS
    text = data.at(0); // CODE
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin+10, Margin + yPos +textWidth.height(), text);
    text = data.at(1); // DESCRIPTION
    while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= 270) { text.chop(2); }
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin+160, Margin + yPos +textWidth.height(), text);
    text = data.at(2); // STOCK QTY
    // We must be aware of the locale. europe uses ',' instead of '.' as the decimals separator.
    if (text.endsWith(".00") || text.endsWith(",00")) { text.chop(3); text += "   ";}//we chop the trailing zeroes...
    text = text +" "+ data.at(3); // 10 pieces
    textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
    painter.drawText(Margin+450, Margin + yPos +textWidth.height(), text);
    text = data.at(4); // SOLD UNITS
    // We must be aware of the locale. europe uses ',' instead of '.' as the decimals separator.
    if (text.endsWith(".00") || text.endsWith(",00")) { text.chop(3); text += "   ";}//we chop the trailing
      textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
      painter.drawText(printer.width()-textWidth.width()-Margin, Margin + yPos +textWidth.height(), text);
      yPos = yPos + fm.lineSpacing();
  }
        
  yPos = yPos + fm.lineSpacing();
  painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
  painter.drawLine(Margin, Margin + yPos - 8, printer.width()-Margin, Margin + yPos - 8);
  
  result = painter.end();
  return result;
}

bool PrintCUPS::printSmallSOTicket(const PrintTicketInfo &ptInfo, QPrinter &printer)
{
  bool result = false;
  // calculate font size
  const int headerSize = printer.width()/16; //divisor found by trying, maybe has to do with font metrics?
  const int textSize = headerSize/2; //i think the half of header size is ok
  qDebug()<<"Paper Width: "<<printer.widthMM()<<"mm"<<"; Page Width: "<<printer.width()<<"; calculated headerSize: "<<headerSize;
  
  QFont header = QFont("Impact", headerSize);
  const int Margin = printer.width()/40;
  int yPos = 0;
  QPainter painter;
  painter.begin( &printer );
  QFontMetrics fm = painter.fontMetrics();
  
  //NOTE from Qt for the drawText: The y-position is used as the baseline of the font
  // Header
  QFont tmpFont = QFont("Bitstream Vera Sans", textSize);
  QPen normalPen = painter.pen();
  QString text;
  QSize textWidth;
  // Store Name
  painter.setFont(header);
  yPos = Margin + painter.fontMetrics().height();

  painter.setFont(header);
  fm = painter.fontMetrics();
  text = ptInfo.storeName;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText((printer.width()/2)-(textWidth.width()/2),yPos, text);

  tmpFont.setItalic(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  yPos = yPos + fm.lineSpacing()*2;
  //Date, Ticket number
  painter.setPen(normalPen);
  text = ptInfo.thDate ;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin, yPos, text);
  //change font for ticket number... bigger. Check for really big numbers to fit the page.
  tmpFont = QFont("Bitstream Vera Sans", textSize+3);
  tmpFont.setItalic(false);
  tmpFont.setBold(true);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = ptInfo.thTicket;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(printer.width() - textWidth.width() - Margin, yPos, text);
  //change font again
  tmpFont = QFont("Bitstream Vera Sans", textSize);
  tmpFont.setItalic(false);
  tmpFont.setBold(false);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  yPos = yPos + fm.lineSpacing();
  //Vendor, terminal number
  text = ptInfo.salesPerson + ", " + ptInfo.terminal;
  textWidth = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text);
  painter.drawText(Margin, yPos, text);
  yPos = yPos + 3*fm.lineSpacing();
  // Products Subheader

  int offsetQty = printer.width()/5;
  tmpFont.setWeight(QFont::Bold);
  painter.setFont(tmpFont);
  fm = painter.fontMetrics();
  text = ptInfo.thProduct;
  while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= offsetQty*4 ) { text.chop(2); }
  painter.drawText(Margin,yPos, text);
  text = ptInfo.thQty;
  painter.drawText(offsetQty*4, yPos, text);
  yPos = yPos + fm.lineSpacing();
  
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);
  painter.setPen(normalPen);
  tmpFont = QFont("Bitstream Vera Sans", textSize );
  painter.setFont(tmpFont);
  yPos = yPos + fm.lineSpacing();
  // End of Header Information.

  // Content : Each product
  QLocale localeForPrinting;
  for (int i = 0; i < ptInfo.ticketInfo.lines.size(); ++i)
  {
    TicketLineInfo tLine = ptInfo.ticketInfo.lines.at(i);
    QString  idesc =  tLine.desc;
    QString iqty   =  localeForPrinting.toString(tLine.qty, 'f', 2);
    bool isGroup = ptInfo.ticketInfo.lines.at(i).isGroup;
    QString sp;
    QDateTime deliveryDT = ptInfo.ticketInfo.lines.at(i).deliveryDateTime;
    // We must be aware of the locale. europe uses ',' instead of '.' as the decimals separator.
    // note: QLocale has a method that does this locale aware! if the locale is set correctly
    if (iqty.endsWith(".00") || iqty.endsWith(",00")) { iqty.chop(3); iqty += "   ";}//we chop the trailing zeroes...
    iqty = iqty;
    tmpFont.setBold(true);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    text = idesc;
    while (fm.size(Qt::TextExpandTabs | Qt::TextDontClip, text).width() >= offsetQty*2 ) { text.chop(2); }
    painter.drawText(Margin, yPos, text); //first product description...
    text = iqty;
    tmpFont.setBold(false);
    painter.setFont(tmpFont);
    fm = painter.fontMetrics();
    painter.drawText(offsetQty*4, yPos, text);
    yPos = yPos + fm.lineSpacing();
      //check if there is a Special Order, to print contents
      if ( !tLine.geForPrint.isEmpty() ) {
        //QStringList strList = tLine.geForPrint.split("|");
        //strList.removeFirst();
        QStringList tmpList = tLine.geForPrint.split("|");
        QStringList strList;
        tmpList.removeFirst();
        //check text lenght
        double maxL = ((printer.width())-Margin*2);
        foreach(QString strTmp, tmpList) {
          fm = painter.fontMetrics();
          QString strCopy = strTmp;
          double realTrozos = fm.size(Qt::TextExpandTabs | Qt::TextDontClip, strTmp).width() / maxL;
          int trozos   = realTrozos;
          double diff = (realTrozos - trozos);
          if (diff > 0.25 && trozos > 0) trozos += 1;
          int tamTrozo = 0;
          if (trozos > 0) {
            tamTrozo = (strTmp.length()/trozos);
          } else {
            tamTrozo = strTmp.length();
          }
          
          QStringList otherList;
          for (int x = 1; x <= trozos; x++) {
            //we repeat for each trozo
            if (x*(tamTrozo-1) < strCopy.length())
              strCopy.insert(x*(tamTrozo-1), "|  "); //create a section
              otherList = strCopy.split("|");
          }
          if (!otherList.isEmpty()) strList << otherList;
          if (trozos < 1) strList << strTmp;
          //qDebug()<<"Trozos:"<<trozos<<" tamTrozo:"<<tamTrozo<<" realTrozos:"<<QString::number(realTrozos,'f', 2)<<" maxL:"<<maxL<<" strTmp.width in pixels:"<<fm.size(Qt::TextExpandTabs | Qt::TextDontClip, strTmp).width()<<" diff:"<<diff;
        }


        foreach(QString str/*Tmp*/, strList) {
          painter.drawText(Margin, yPos, str/*Tmp*/);
          yPos = yPos + fm.lineSpacing();
        }
        ///Check for delivery date and if its a SO
        if (!isGroup && tLine.payment>0 ) {
          sp = ptInfo.deliveryDateStr + deliveryDT.toString("  ddd d MMMM, h:m ap"); //TODO FIXME:hey i18n stuff!!!
          tmpFont.setBold(true);
          painter.setFont(tmpFont);
          fm = painter.fontMetrics();
          painter.drawText(Margin, yPos, sp);
          yPos = yPos + fm.lineSpacing();
          tmpFont.setBold(false);
          painter.setFont(tmpFont);
          fm = painter.fontMetrics();
          yPos = yPos + fm.lineSpacing();
        }
        yPos = yPos + fm.lineSpacing();
      }
  } //for each item
  
  painter.drawLine(Margin, yPos, printer.width()-Margin, yPos);
  
  result = painter.end();// this makes the print job start
  
  return result;
}

