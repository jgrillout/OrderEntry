#ifndef MAIN_HPP_INCLUDED
#define MAIN_HPP_INCLUDED



#include "customer.h"
#include "part.h"
#include "orderHeader.h"
#include "orderLines.h"
#include "LookUpOrderHeader.h"
#include "LookUpOrderLines.h"
#include "nextOrderId.h"


#include <ncurses.h>
#include <form.h>
#include <menu.h>
#include <malloc.h>
#include <string>

#include <stdlib.h>
#include <assert.h>
#include <mysql/mysql.h>
#include <assert.h>

/*Typically <strings.h> just adds some useful but non-standard additional string functions to the standard header <string.h>.
For maximum portability you should only use <string.h> but if you need the functions in <strings.h> more than you need portability
then you can use <strings.h> instead of <string.h>. */

#include <string.h>
#include <strings.h>


#include <stdio.h>


#include <vector>
#include <algorithm>
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// just found this bit of code to test if a field option is set or not
// if (field_opts(fields[i]) & O_ACTIVE)

const int OE_WINFORMAREA_SCROLL_LINES(10); // number of lines in winFormArea scroll region
const int OE_WINEDITLINES(17); // number of lines to display enties in winEditLinesFullScreen screen

//for ncurses
const int LEFT(0);
const int CENTER(1);
const int RIGHT(2);

// for part entry
const int PE_PARTKEY(1);
const int PE_PARTID (3);
const int PE_NAME (5);
const int PE_COST(7);
const int PE_PRICE(9);

// for customer entry
const int CE_CUSTKEY(1);
const int CE_CUSTID (3);
const int CE_NAME (5);
const int CE_ADDR(7);
const int CE_CITY(9);
const int CE_STATE(11);
const int CE_ZIP(13);

//for order entry
const int OE_ORDERKEY(0);
const int OE_CUSTKEY (1);
const int OE_PARTKEY (2);
const int OE_ORDERID (4);

const int OE_CUSTID (6);
const int OE_TRANSDATE(8);

const int OE_PARTID(10);
const int OE_PARTNAME(11);
const int OE_QTY(13);
const int OE_PRICE(15);

const int OE_SAVE (17);
const int OE_COST(18);

// for Order Entry Edit Lines
const int OE_WINEDITLINES_PARTKEY(0);
const int OE_WINEDITLINES_PARTID(2);
const int OE_WINEDITLINES_PARTNAME(3);
const int OE_WINEDITLINES_QTY(5);
const int OE_WINEDITLINES_PRICE(7);
const int OE_WINEDITLINES_SAVE(9);
const int OE_WINEDITLINES_COST(10);

// for PrintInvoice
const int PI_ORDERID(1);
const int PI_DESTINATION(3);


// forward declarations --------------------------------------------------------------------------------------------

void CleanupMenu(MENU *myMainMenu, ITEM **myMenuItems, int NumChoices);


// part entry functions
int PartEntry(WINDOW *winMsgArea, WINDOW *winFullScreen);
bool  PartEntryVerify(FIELD *partEntryFields[],WINDOW *winFullScreen, WINDOW *winMsgArea,bool &EditingRow, bool &NewRow);
int PartEntryDisplayLookup(FIELD *partEntryFields[]);
void  SetPartEntryFields(FIELD *partEntryFields[],std::vector<part>::iterator partit,bool EditingRow);
void PartEntrySetupForm(FIELD *partEntryFields[],WINDOW *winFullScreen);
void  PartEntryFormCleanup(FIELD *partEntryFields[]);
int InsertPart(FIELD *partEntryFields[]);
int UpdatePart(FIELD *partEntryFields[],int key);
int DeletePart(FIELD *partEntryFields[],int key);


// cust entry functions
int CustEntry(WINDOW *winMsgArea, WINDOW *winFullScreen);
bool  CustEntryVerify(FIELD *custEntryFields[],WINDOW *winFullScreen,WINDOW *winMsgArea, bool &EditingRow, bool &NewRow);
int CustEntryDisplayLookup(FIELD *custEntryFields[],bool &EditingRow,bool &NewRow);
void  SetCustEntryFields(FIELD *custEntryFields[],std::vector<customer>::iterator custit,bool EditingRow);
void  CustEntrySetupForm(FIELD *custEntryFields[],WINDOW *winFullScreen,int custEntryFieldsTabOrder[]);
void  CustEntryFormCleanup(FIELD *custEntryFields[]);
int InsertCust(FIELD *custEntryFields[]);
int UpdateCust(FIELD *custEntryFields[],int key);
int DeleteCust(FIELD *custEntryFields[],int key);

// order entry functions
int OrderEntry(WINDOW *winMsgArea, WINDOW *winFullScreen);
bool  OrderEntryVerify( FIELD *orderEntryFields[],WINDOW *winFullScreen,int &countLines, int &lastLineKey, double &orderTotal,int  keyPress,WINDOW *winMsgArea,bool &EditingRow,bool &NewRow,bool &addOrder);
void OrderEntryScrollLines(FIELD *orderEntryFields[],WINDOW *winFullScreen,int  &countLines,double &orderTotal);
void OrderEntryExtendCost( FIELD *orderEntryFields[],WINDOW *winFullScreen, int  &countLines);
int OrderEntryDisplayLookup(WINDOW *winFullScreen,FIELD *orderEntryFields[],int &countLines,double &orderTotal,bool &EditingRow,bool &NewRow);
void OrderEntrySetupForm(FIELD *orderEntryFields[],WINDOW *winFullScreen);
void OrderEntryFormCleanup(FIELD *orderEntryFields[]);
void OrderEntryEditLines(FIELD *orderEntryFields[],WINDOW *winFullScreen,WINDOW *winMsgArea);
void OrderEntryEditLinesPopUp(WINDOW *winFullScreen, WINDOW *winMsgArea,int OrderKey);
bool OrderEntryEditLinesVerify(WINDOW *winFullScreen,int &countLines,int &lastLineKey, int keyPress,int OrderKey, int LineKey,WINDOW *winMsgArea);
int OrderEntryEditLinesDisplayLookup(int &countLines);
void OrderEntryEditLinesSetupForm(void);
void OrderEntryEditLinesFormCleanup(WINDOW *winFullScreen,int OrderKey);
int GetNextOrderId();
int InsertOrderHeader(FIELD *orderEntryFields[]);
int DeleteOrder(int key);
int UpdateOrderHeader(FIELD *orderEntryFields[],int key);
bool InsertOrderLine(FIELD *orderEntryFields[], int linenum);
bool DeleteOrderLine(int Orderkey,int LineKey);
bool UpdateOrderLine(int Orderkey,int LineKey);
void  DisplayOrderLines(WINDOW *winFullScreen,std::vector<LookUpOrderLines> &LookUpOrderLinesResults,int &countLines, int &lastLineKey,double &orderTotal,WINDOW *win, bool EditLinesMode);
void ClearDisplayOrderLines(WINDOW *winFullScreen,int &countLines, double &orderTotal,WINDOW *win,bool EditLinesMode);
int GetOrderLine(std::vector<LookUpOrderLines> &LookUpOrderLinesResults,int Orderkey,int LineKey);


// print invoice functions
int  PrintInvoice(WINDOW *winMsgArea,WINDOW *winFullScreen);
int PrintInvoiceDisplayLookup(WINDOW *winFullScreen);
void PrintInvoiceSetupForm(WINDOW *winMsgArea,WINDOW *winFullScreen);
void PrintInvoiceFormCleanup();

bool InvoicePrint(WINDOW *winFullScreen,WINDOW *winMsgArea,int key);
void printInvoiceHeader(FILE *pFile,std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit,int page,std::string sDate);

void printInvoiceLines(FILE *pFile,std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit, int key,std::string sDate);
void printInvoiceFooter(FILE *pFile,double orderTotal);

void displayInvoice(WINDOW *winMsgArea,WINDOW *winFullScreen);

// shared functions
int GetAllCustomers(std::vector<customer> &customersResults);
bool GetCust(std::vector<customer> &custResults,std::string lookupId);
int GetAllParts(std::vector<part> &partsResults);
bool GetPart(std::vector<part> &partsResults,std::string lookupId);
int GetAllOrderHeaders(std::vector<LookUpOrderHeader> &LookUpOrderHeaderResults);
bool GetOrder(std::vector<LookUpOrderHeader> &LookUpOrderHeaderResults,int key);
int GetOrderLines(std::vector<LookUpOrderLines> &LookUpOrderLinesResults,int key,bool EditLinesMode);

//static char* TrimWhitespaces(char *str);
char* TrimWhitespaces(char *str);
bool isValidDate(char* s);
void debugMsg(char *string1,char *string2,int val);

void errMsg(WINDOW *win,int row,int col,char *string1);
std::string askQuestion( WINDOW *win, int startrow, int startcol, int width, char *prompt, chtype color);
bool  confirmAction( WINDOW *msgwin, WINDOW *fullwin, int startrow, int startcol, int width, std::string prompt, chtype color, int keyToPress);
void getNumericInput( WINDOW *msgwin, WINDOW *fullwin, int startrow, int startcol, int len,int width, char *prompt, char *input, chtype color);
void PrintInMiddle(WINDOW *win, int startrow, int startcol, int width, char *string, chtype color);
void FindMiddle(WINDOW *win, int startrow, int startcol, int &outrow, int &outcol,int width, char *msg);
// end of forward declarations --------------------------------------------------------------------------------------------
#endif // MAIN_HPP_INCLUDED







