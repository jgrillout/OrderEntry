#define MYSQLPP_SSQLS_NO_STATICS
#include "main.hpp"
#include <stdio.h>

#include <ctime>

//#include <iostream>
//#include <iomanip>


using namespace std;

static FORM *PrintInvoiceForm;
static FIELD *PrintInvoiceFields[4],*tempFptr;
//static WINDOW *winFullScreen;
static WINDOW *winFormArea;


int  PrintInvoice(WINDOW *winMsgArea,WINDOW *winFullScreen)
{

    short formCurrentField;
    bool firstTime=true,quit=false;
    int key=0,ch=0;


PrintInvoiceStart:
    quit=false;

    PrintInvoiceSetupForm(winMsgArea,winFullScreen);
    post_form(PrintInvoiceForm);
    set_current_field(PrintInvoiceForm,PrintInvoiceFields[PI_ORDERID]);


    wattron(winFullScreen,A_REVERSE);
    mvwprintw(winFullScreen,22, 2, "F2 Lookup");
//    mvwprintw(winFullScreen,22, 12, "F3 Delete");
    mvwprintw(winFullScreen,22, 22, "F4 Print");
    mvwprintw(winFullScreen,22, 32, "F5 New");
    mvwprintw(winFullScreen,22, 42, "F7 Exit");
    wattroff(winFullScreen,A_REVERSE);
    box(winFormArea, 0, 0);
    move(9,39);


    wrefresh(winFormArea);
    PrintInMiddle(winFullScreen, 1, 0, 80, "Print Invoice", COLOR_PAIR(1));
    wrefresh(winFullScreen);
    refresh();

    pos_form_cursor(PrintInvoiceForm);

    if (firstTime==true)
    {
        firstTime=false;
        PrintInvoiceFormCleanup();
        goto PrintInvoiceStart;
    }


    while (quit!=true)
    {

        tempFptr = current_field(PrintInvoiceForm);
        formCurrentField = field_index(tempFptr);

        ch = getch();
        switch (ch)
        {

        case KEY_F(2): // lookup

            def_prog_mode();
            endwin();

            PrintInvoiceDisplayLookup(winFullScreen);
            refresh();
            pos_form_cursor(PrintInvoiceForm);

            break;


        case KEY_F(4):// print
            //      sync buffer
            form_driver(PrintInvoiceForm, REQ_NEXT_FIELD);
            form_driver(PrintInvoiceForm, REQ_PREV_FIELD);

            key=atoi(TrimWhitespaces(field_buffer(PrintInvoiceFields[PI_ORDERID], 0)));
            if (InvoicePrint(winFullScreen,winMsgArea,key)==false)
            {
                goto PrintInvoiceStart;
            }
            else
                quit=true;
            break;

        case KEY_F(5): //reset
            PrintInvoiceFormCleanup();
            goto PrintInvoiceStart;
            break;
        case KEY_F(7): //exit
            if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Exit",COLOR_PAIR(1),KEY_F(7))==true)
            {
                quit=true;
            }

            pos_form_cursor(PrintInvoiceForm);
            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            break;

        case KEY_END:
            form_driver(PrintInvoiceForm, REQ_END_FIELD);
            break;
        case KEY_HOME:
            form_driver(PrintInvoiceForm, REQ_BEG_FIELD);
            break;
        case 9: // TAB
        case KEY_DOWN:
        case KEY_ENTER:

        case 10:
        case 13:


            form_driver(PrintInvoiceForm, REQ_NEXT_FIELD);
            form_driver(PrintInvoiceForm, REQ_BEG_LINE);
            break;
        case KEY_UP:

            form_driver(PrintInvoiceForm, REQ_PREV_FIELD);
            form_driver(PrintInvoiceForm, REQ_BEG_LINE);
            break;
        case KEY_LEFT:
            form_driver(PrintInvoiceForm, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(PrintInvoiceForm, REQ_NEXT_CHAR);
            break;
        // Delete the char before cursor
        case KEY_BACKSPACE:
        case 127:
            form_driver(PrintInvoiceForm, REQ_DEL_PREV);
            break;
        // Delete the char under the cursor
        case KEY_DC:
            form_driver(PrintInvoiceForm, REQ_DEL_CHAR);
            break;
        default:
            form_driver(PrintInvoiceForm, ch);
            break;
        }// switch

        wrefresh(winFormArea);

    }// while


    PrintInvoiceFormCleanup();


    return 0;

}
int PrintInvoiceDisplayLookup(WINDOW *winFullScreen)
{
    ITEM **myItems;
    int c,r;

    MENU *myLookUpMenu;
    WINDOW *myLookUpMenuWin;
    int NumChoices=0,retValue=0;//,lookup=0;
    short formCurrentField;
//    std::string slookup;
//    std::vector<part>::iterator partit;
//    std::vector<part> partsResults;
//
//    std::vector<customer>::iterator customerit;
//    std::vector<customer> customersResults;

    std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit;
    std::vector<LookUpOrderHeader> LookUpOrderHeaderResults;

//    std::vector<LookUpOrderLines>::iterator LookUpOrderLinesit;
//    std::vector<LookUpOrderLines> LookUpOrderLinesResults;


    std::vector <int*> intptrs;
    char NumToCharPointer[10];
    bool exitMenu = false;
    //bool Result = false;
    bool unposted =false;
//    int lastLineKey;
    std::string sSpaces(30, ' ');
    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

//    short formCurrentField;
    tempFptr = current_field(PrintInvoiceForm);
    formCurrentField = field_index(tempFptr);


    NumChoices=GetAllOrderHeaders(LookUpOrderHeaderResults);
    NumChoices=LookUpOrderHeaderResults.size();

    myItems = (ITEM **)calloc(NumChoices+1, sizeof(ITEM *));
    r=0;
    for (LookUpOrderHeaderit = LookUpOrderHeaderResults.begin(); LookUpOrderHeaderit != LookUpOrderHeaderResults.end(); ++LookUpOrderHeaderit)
    {
        // jrg changed 9/10/15 @ 8:45pm
        //intptrs.push_back((int)&LookUpOrderHeaderit->idtblOrder);
        intptrs.push_back((int*)&LookUpOrderHeaderit->idtblOrder);
        set_item_userptr(myItems[r], (int *)*intptrs.back());
        myItems[r] = new_item(LookUpOrderHeaderit->charidtblOrder.c_str(),LookUpOrderHeaderit->CustName.c_str());
        ++r;
    }

    if (NumChoices!=0)
    {
        /* Create menu */
        myLookUpMenu = new_menu((ITEM **)myItems);
        set_menu_fore(myLookUpMenu, COLOR_PAIR(1) | A_REVERSE);
        set_menu_back(myLookUpMenu, COLOR_PAIR(1));

        /* Create the window to be associated with the menu */
        //newwin(rows,cols,y_org,x_org)
        //derwin(WINDOW *orig, int nlines, int ncols,int begin_y, int begin_x);


        int winSize = std::max(NumChoices,5);
        if (winSize<4) winSize=4;
        myLookUpMenuWin = newwin(winSize, 40, 1, 4);
        set_menu_win(myLookUpMenu, myLookUpMenuWin);
        set_menu_sub(myLookUpMenu, derwin(myLookUpMenuWin, (winSize-2), 38, 1, 1));
        set_menu_format(myLookUpMenu, (winSize-2), 1);



        keypad(myLookUpMenuWin, TRUE);
        //set_menu_format(myLookUpMenu, 10, 1);
        set_menu_mark(myLookUpMenu, " > ");

        wattron(myLookUpMenuWin,COLOR_PAIR(1)); // color of box lines
        box(myLookUpMenuWin, 0, 0);
        refresh();
        post_menu(myLookUpMenu);
        wrefresh(myLookUpMenuWin);

        bool unposted=false;

        while(exitMenu==false)
        {
            c = wgetch(myLookUpMenuWin);
            switch(c)
            {
            case KEY_DOWN:
                menu_driver(myLookUpMenu,REQ_DOWN_ITEM);
                break;

            case KEY_UP:
                menu_driver(myLookUpMenu, REQ_UP_ITEM);
                break;
            case KEY_NPAGE:
                menu_driver(myLookUpMenu, REQ_SCR_DPAGE);
                break;
            case KEY_PPAGE:
                menu_driver(myLookUpMenu, REQ_SCR_UPAGE);
                break;
            case 9: // TAB
                exitMenu=true;
                unpost_menu(myLookUpMenu);
                delwin(myLookUpMenuWin);
                touchwin(winFormArea);
                touchwin(winFullScreen);
                wrefresh(winFormArea);
                wrefresh(winFullScreen);
                refresh();
                retValue=-1;
                unposted=true;
                break;
            case KEY_ENTER:
            case 10:
            case 13:
                ITEM *cur;
                cur = current_item(myLookUpMenu);
                exitMenu=true;
                retValue = (int)item_userptr(cur);


                if (GetOrder(LookUpOrderHeaderResults,retValue)==true)
                {
                    LookUpOrderHeaderit = LookUpOrderHeaderResults.begin();

//                if (LookUpOrderHeaderit->idtblOrder==retValue)
//                {
//                    lookup = retValue;
//                }

                    LookUpOrderHeaderResults.clear();
                    sprintf(NumToCharPointer, "%06d", LookUpOrderHeaderit->idtblOrder);
                    set_field_buffer(PrintInvoiceFields[PI_ORDERID], 0, NumToCharPointer);


                    unpost_menu(myLookUpMenu);
                    unposted=true;
                    move(2,11);
                    break;


                } // if

                if(unposted==false)
                {

                    box(myLookUpMenuWin, 0, 0);
                    wrefresh(winFormArea);
                    wrefresh(myLookUpMenuWin);

                }

            }//switch



            wrefresh(winFormArea);

        }//while

        if(unposted==false)
            unpost_menu(myLookUpMenu);


        free_menu(myLookUpMenu);

        for(int i = 0; i < NumChoices; ++i)
            free_item(myItems[i]);

        endwin();


        intptrs.clear();
        LookUpOrderHeaderResults.clear();


        touchwin(winFormArea);
        wrefresh(winFormArea);
    }// if (NumChoices!=0)
    return retValue;
}


void PrintInvoiceSetupForm(WINDOW *winMsgArea,WINDOW *winFullScreen)
{
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    wattron(stdscr,COLOR_PAIR(1));

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    winFullScreen = newwin(23, 80, 0, 0);
    assert(winFullScreen != NULL);
    wattron(winFullScreen,COLOR_PAIR(1));
    wrefresh(winFullScreen);
    winFormArea = derwin(winFullScreen, 20, 78, 1, 1);
    assert(winFormArea != NULL);
    wattron(winFormArea,COLOR_PAIR(1));
    wrefresh(winFormArea);

//              new_field(h,w,  tp,left, offscreen, nbuffers);

    PrintInvoiceFields[0] =  new_field(1, 20,  8, 20,0, 0);// Order # label
    PrintInvoiceFields[PI_ORDERID] =  new_field(1,  6,  8, 38, 0,0);// Order # field
    PrintInvoiceFields[2] =  new_field(1, 20,  10, 20,0, 0);// Dest label
    PrintInvoiceFields[PI_DESTINATION] =  new_field(1,  7,  10, 38, 0,0);// Dest field
    PrintInvoiceFields[4] = NULL;

    assert(PrintInvoiceFields[0] != NULL && PrintInvoiceFields[1] != NULL
           && PrintInvoiceFields[2] != NULL && PrintInvoiceFields[3] != NULL);

    set_field_buffer(PrintInvoiceFields[0], 0, "Order #");
    set_field_buffer(PrintInvoiceFields[PI_ORDERID], 0, ""); //Order#
    set_field_buffer(PrintInvoiceFields[2], 0, "Screen or Printer");//Dest
    set_field_buffer(PrintInvoiceFields[PI_DESTINATION], 0, "");

    set_field_type(PrintInvoiceFields[PI_ORDERID], TYPE_INTEGER,6,1,999999);
    set_field_back(PrintInvoiceFields[PI_ORDERID], A_UNDERLINE|COLOR_PAIR(1));

    char * dest[] = { "Printer", "Screen", (char *)0 };

    set_field_type(PrintInvoiceFields[PI_DESTINATION],TYPE_ENUM, dest, false,false);
    set_field_back(PrintInvoiceFields[PI_DESTINATION], A_UNDERLINE|COLOR_PAIR(1));




    for (int i=0; i<=4; ++i)
        set_field_fore(PrintInvoiceFields[i], COLOR_PAIR(1));

    set_field_opts(PrintInvoiceFields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(PrintInvoiceFields[PI_ORDERID], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(PrintInvoiceFields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(PrintInvoiceFields[PI_DESTINATION], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);

    PrintInvoiceForm = new_form(PrintInvoiceFields);
    assert(PrintInvoiceForm != NULL);
    set_form_win(PrintInvoiceForm,winFullScreen);
    set_form_sub(PrintInvoiceForm,winFormArea);
}
void PrintInvoiceFormCleanup()
{
    unpost_form(PrintInvoiceForm);
    free_form(PrintInvoiceForm);
    for (int i=0; i<4; i++)
    {
        free_field(PrintInvoiceFields[i]);
    }
    delwin(winFormArea);
    //delwin(winFullScreen);
    endwin();
    endwin();
}
bool InvoicePrint(WINDOW *winFullScreen,WINDOW *winMsgArea,int key)

{
    std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit;
    std::vector<LookUpOrderHeader> LookUpOrderHeaderResults;

    std::vector<LookUpOrderLines>::iterator LookUpOrderLinesit;
    std::vector<LookUpOrderLines> LookUpOrderLinesResults;
    bool quit=false;
    FILE * pFile;
    char NumToCharPointer[10];
    std::string sDate;
    sDate.append("System Date ");
    // current date/time based on current system
    time_t now = time(0);
    tm *ltm = localtime(&now);

    sprintf(NumToCharPointer, "%02d", 1 + ltm->tm_mon);// month
    sDate.append(NumToCharPointer);
    sDate.append("/");
    sprintf(NumToCharPointer, "%02d", ltm->tm_mday); // day
    sDate.append(NumToCharPointer);
    sDate.append("/");
    sprintf(NumToCharPointer, "%04d", 1900 + ltm->tm_year); //year
    sDate.append(NumToCharPointer);
    //debugMsg("Date is ",sDate.c_str(),0);


//   // print various components of tm structure.
//   cout << "Year: "<< 1900 + ltm->tm_year << endl;
//   cout << "Month: "<< 1 + ltm->tm_mon<< endl;
//   cout << "Day: "<<  ltm->tm_mday << endl;
//   cout << "Time: "<< 1 + ltm->tm_hour << ":";
//   cout << 1 + ltm->tm_min << ":";
//   cout << 1lpr invoice.txt + ltm->tm_sec << endl;

    pFile = fopen ("invoice.txt","w");

    if (GetOrder(LookUpOrderHeaderResults,key)==true)
    {

        LookUpOrderHeaderit = LookUpOrderHeaderResults.begin();// there should only be one row in result set
        if (LookUpOrderHeaderit->idtblOrder==key)
        {
            printInvoiceLines(pFile,LookUpOrderHeaderit,key,sDate);


        }
        else
        {
            //???
        }
    }
    else
    {
        //???
    }

    fclose (pFile);

    char* test;
    test="Printer";
    std::string dest;



    dest=TrimWhitespaces(field_buffer(PrintInvoiceFields[PI_DESTINATION],0));
    if (dest==test)
    {
        char command[75];
        snprintf(command, 75, "lpr invoice.txt");
        system(command);

        if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Exit",COLOR_PAIR(1),KEY_F(7))==true)
        {
            quit=true;
        }
        touchwin(winFullScreen);
        wrefresh(winFullScreen);
    }
    else
    {

        displayInvoice(winMsgArea,winFullScreen);
        quit=true;
    }

    return quit;
}


void printInvoiceHeader(FILE *pFile,std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit, int page,std::string sDate)
{

    std::string sLine(80, '_');
    std::string sSpace(20, ' ');
    std::string sLabel1;
    sLabel1="Ln#";
    std::string sLabel2;
    sLabel2="Part Id";
    std::string sLabel3;
    sLabel3="Part Name";
    std::string sLabel4;
    sLabel4="Price";

    fprintf(pFile,"Page %02d%72s\n", page,sDate.c_str());

    fprintf (pFile,"Order # %06d\t", LookUpOrderHeaderit->idtblOrder);
    fprintf (pFile,"%42sOrder Date %s\n\n",sSpace.c_str(),LookUpOrderHeaderit->transDate.c_str());

    fprintf (pFile, "  Customer Id %s\n",LookUpOrderHeaderit->CustId.c_str());
    fprintf (pFile,"              %s\n", LookUpOrderHeaderit->CustName.c_str());
    fprintf (pFile,"              %s\n", LookUpOrderHeaderit->Address.c_str());
    fprintf (pFile,"              %s" , LookUpOrderHeaderit->City.c_str());
    fprintf (pFile,",%s", LookUpOrderHeaderit->State.c_str());
    fprintf (pFile,"  %s\n\n", LookUpOrderHeaderit->Zip.c_str());


    fprintf (pFile, "%2s\t",sLabel1.c_str());
    fprintf (pFile,"%10s\t",sLabel2.c_str());
    fprintf (pFile,"%20s\t", sLabel3.c_str());
    fprintf (pFile, "%6s\n",sLabel4.c_str());

    fprintf (pFile,"%s\n",sLine.c_str());

}

void printInvoiceLines(FILE *pFile,std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit, int key,std::string sDate)
{

    int counter,linesPerPage,page;
    double extPrice,orderTotal;

    counter=0;
    linesPerPage=66;
    page=1;
    extPrice=0;
    orderTotal=0;
    printInvoiceHeader(pFile,LookUpOrderHeaderit,page,sDate);
    //std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit;
    std::vector<LookUpOrderHeader> LookUpOrderHeaderResults;

    std::vector<LookUpOrderLines>::iterator LookUpOrderLinesit;
    std::vector<LookUpOrderLines> LookUpOrderLinesResults;
    GetOrderLines(LookUpOrderLinesResults, key, true);

    for (LookUpOrderLinesit = LookUpOrderLinesResults.begin(); LookUpOrderLinesit != LookUpOrderLinesResults.end(); ++LookUpOrderLinesit)
    {
        counter++;
        if (counter>linesPerPage)
        {
            page++;
            printInvoiceHeader(pFile,LookUpOrderHeaderit,page,sDate);
            counter=0;
        }
        //sql = "SELECT l.idtblOrder,l.idtblOrderLine,l.idtblPart,l.Qty,l.Price,l.Cost,p.PartId,p.Name ";
        fprintf (pFile, "%02d\t",LookUpOrderLinesit->idtblOrderLine);
        fprintf (pFile,"%10s\t",LookUpOrderLinesit->PartId.c_str());
        fprintf (pFile,"%20s\t", LookUpOrderLinesit->Name.c_str());
        fprintf (pFile, "%6.2f\n",LookUpOrderLinesit->Price);

        extPrice = LookUpOrderLinesit->Qty*LookUpOrderLinesit->Price;
        orderTotal+=extPrice;
    }




    printInvoiceFooter(pFile,orderTotal);



}
void printInvoiceFooter(FILE *pFile,double orderTotal)
{

    std::string sLine(80, '_');
    std::string sSpace(20, ' ');
    std::string sLabel1;
    sLabel1="Order Total";


    fprintf (pFile,"%s\n",sLine.c_str());

    fprintf (pFile, "%2s\t",sSpace.substr(1,2).c_str());
    fprintf (pFile,"%10s\t",sSpace.substr(1,10).c_str());
    fprintf (pFile,"%20s\t", sLabel1.c_str());
    fprintf (pFile, "%6.2f\n",orderTotal);


}
void displayInvoice(WINDOW *winMsgArea,WINDOW *winFullScreen)
{

    WINDOW *winPrintScreen;
    FILE * pFile;
    int ch;
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    wattron(stdscr,COLOR_PAIR(1));

//    initscr();
//    noecho();
//    cbreak();
//    keypad(stdscr, TRUE);

    winPrintScreen = newwin(23, 80, 0, 0);
    assert(winPrintScreen != NULL);
    wattron(winPrintScreen,COLOR_PAIR(1));
    pFile = fopen ("invoice.txt","r");

    /* display the file's contents */
    do
    {
        ch = fgetc(pFile);
        waddch(winPrintScreen,ch);
//			wrefresh(winPrintScreen);
        //getch();
//            waddch(stdscr,ch);
//			wrefresh(stdscr);
//			refresh();
    }
    while (ch != EOF);
    fclose(pFile);
    wrefresh(winPrintScreen);
    getch();
    delwin(winPrintScreen);
    touchwin(winFullScreen);

    wrefresh(winFullScreen);
    refresh();


    endwin();




}











