#define MYSQLPP_SSQLS_NO_STATICS
#include "main.hpp"

// for custom field types but I'm not using this because I only have one
#define FIELDCHECK_CB(func) bool func(FIELD * fld, const void * data GCC_UNUSED)
#define CHAR_CHECK_CB(func) bool func(int ch, const void *data GCC_UNUSED)


static FORM *orderEntryForm, *orderEntryEditLinesForm;
//static FIELD *orderEntryFields[19],*orderEntryEditLinesFields[11],*tempFptr;
//static WINDOW *winFullScreen,*winFormArea,*winEditLinesFullScreen,*winEditLinesFormArea;
static FIELD *orderEntryEditLinesFields[11],*tempFptr;

static WINDOW *winFormArea,*winEditLinesFullScreen,*winEditLinesFormArea;
using namespace std;


int OrderEntry(WINDOW *winMsgArea,WINDOW *winFullScreen)
{
    static FIELD *orderEntryFields[19];
    bool firstTime=true,quit=false,EditingRow=false,NewRow=true,addOrder=false,moveNext=true,movePrev=true;
    short formPreviousField,formCurrentField;
    int key=0, ch=0;
    double orderTotal=0;
    std::vector<LookUpOrderLines> LookUpOrderLinesResults;
    char NumToCharPointer[10];

OrderEntryModuleStart:

    quit=false;
    EditingRow=false;
    NewRow=false;
    addOrder=false;
    int countLines=0,lastLineKey=0;

    orderTotal=0;

    OrderEntrySetupForm(orderEntryFields,winFullScreen);

    post_form(orderEntryForm);


    set_current_field(orderEntryForm,orderEntryFields[OE_ORDERID]);

    wattron(winFullScreen,A_REVERSE);
    mvwprintw(winFullScreen,22, 2,  "F2 Lookup");
    mvwprintw(winFullScreen,22, 12, "F3 Delete");
    mvwprintw(winFullScreen,22, 22, "F4 Edit Lines");
    mvwprintw(winFullScreen,22, 36, "F5 New");
    mvwprintw(winFullScreen,22, 43, "F7 Exit");
    wattroff(winFullScreen,A_REVERSE);
    mvwprintw(winFormArea,5, 23, "Part Name");
    mvwprintw(winFormArea, 4, 71, "Ext.");
    mvwprintw(winFormArea, 5, 71, "Price");

    box(winFullScreen, 0, 0);
    mvwprintw(winFullScreen,21, 71, "%8.2f",orderTotal);

    move(2,11);

    wrefresh(winFormArea);
    wrefresh(winFullScreen);
    refresh();
    //pos_form_cursor(orderEntryForm);

    if (firstTime==true)
    {
        OrderEntryFormCleanup(orderEntryFields);
        firstTime=false;
        goto OrderEntryModuleStart;
    }



    while (quit!=true)
    {
        tempFptr = current_field(orderEntryForm);
        formCurrentField = field_index(tempFptr);
        formPreviousField = field_index(tempFptr);

        moveNext=true;
        movePrev=true;

        ch = getch();
        switch (ch)
        {
        case KEY_F(2): // lookup

            if (formCurrentField==OE_ORDERID||formCurrentField==OE_CUSTID||formCurrentField==OE_PARTID)
            {
                if (formCurrentField==OE_ORDERID)
                {
                    countLines=OE_WINFORMAREA_SCROLL_LINES;
                    ClearDisplayOrderLines(winFullScreen,countLines,orderTotal,winFormArea,false);
                }


                def_prog_mode();
                endwin();

                OrderEntryDisplayLookup(winFullScreen,orderEntryFields,countLines,orderTotal,EditingRow,NewRow);

                wrefresh(winFormArea);
                refresh();
                pos_form_cursor(orderEntryForm);
            }
            break;

        case  KEY_F(3): //delete
            if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Delete Order",COLOR_PAIR(1),KEY_F(3))==true)
            {
                key=atoi(TrimWhitespaces(field_buffer(orderEntryFields[OE_ORDERID], 0)));
                DeleteOrder(key);
                goto OrderEntryModuleStart;
            }


            pos_form_cursor(orderEntryForm);
            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);


            break;

        case KEY_F(4):// edit lines
        {

            def_prog_mode();
            endwin();

            OrderEntryEditLines(orderEntryFields,winFullScreen,winMsgArea);
            key=atoi(TrimWhitespaces(field_buffer(orderEntryFields[OE_ORDERID], 0)));
            GetOrderLines(LookUpOrderLinesResults,key,false);
            countLines=OE_WINFORMAREA_SCROLL_LINES;
            ClearDisplayOrderLines(winFullScreen,countLines,orderTotal,winFormArea,false);
            DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,lastLineKey,countLines,orderTotal,winFormArea,false);

            touchwin(winFullScreen);
            touchwin(winFormArea);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            refresh();
            pos_form_cursor(orderEntryForm);

        }

        break;

        case KEY_F(5): //reset
            OrderEntryFormCleanup(orderEntryFields);
            goto OrderEntryModuleStart;
            break;
        case KEY_F(7): //exit
            if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Exit !",COLOR_PAIR(1),KEY_F(7))==true)
            {


                quit=true;

            }

            pos_form_cursor(orderEntryForm);
            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            break;

        case KEY_END:
            form_driver(orderEntryForm, REQ_END_FIELD);
            break;
        case KEY_HOME:
            form_driver(orderEntryForm, REQ_BEG_FIELD);
            break;
        case KEY_DOWN:
        case KEY_ENTER:
        case 9: // TAB
        case 10:
        case 13:
            // sync buffer
            if (formCurrentField!=OE_SAVE) form_driver(orderEntryForm, REQ_NEXT_FIELD);
            if (formCurrentField!=OE_SAVE) form_driver(orderEntryForm, REQ_PREV_FIELD);
//
//            debugMsg("before OrderEntryVerify","NewRow",NewRow);
//            debugMsg("before OrderEntryVerify","EditingRow",EditingRow);
//            debugMsg("before OrderEntryVerify","addOrder",addOrder);
            if (OrderEntryVerify(orderEntryFields,winFullScreen,countLines,lastLineKey,orderTotal,KEY_DOWN,winMsgArea,EditingRow,NewRow,addOrder)==true)

            {

                tempFptr = current_field(orderEntryForm);
                formCurrentField = field_index(tempFptr);
//            debugMsg("after OrderEntryVerify","formCurrentField",formCurrentField);
//            debugMsg("after OrderEntryVerify","NewRow",NewRow);
//            debugMsg("after OrderEntryVerify","EditingRow",EditingRow);
//            debugMsg("after OrderEntryVerify","addOrder",addOrder);

                if (formPreviousField==OE_SAVE && formCurrentField==OE_PARTID)
                {
                    moveNext=false;
                }
                if (formCurrentField==OE_SAVE)
                {
                    moveNext=false;
                }
                if (formCurrentField==OE_ORDERID && addOrder==false && EditingRow==false)
                {

                    set_current_field(orderEntryForm,orderEntryFields[OE_ORDERID]);
                    moveNext=false;
                    move(2,11); // order id
                }
                else if (formCurrentField==OE_ORDERID && EditingRow==true)
                {

                    set_current_field(orderEntryForm,orderEntryFields[OE_CUSTID]);
                    moveNext=false;
                    move(4,11); // cust id
                }
                if (formCurrentField==OE_TRANSDATE)
                {


                    if (NewRow==true)
                    {

                        if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Save Order Header",COLOR_PAIR(1),KEY_NPAGE)==true)
                        {

                            move(7,6);// part id
                            key=InsertOrderHeader(orderEntryFields);
                            sprintf(NumToCharPointer, "%d", key);
                            set_field_buffer(orderEntryFields[OE_ORDERKEY], 0,  NumToCharPointer);
                            sprintf(NumToCharPointer, "%06d", key);

                            set_field_buffer(orderEntryFields[OE_ORDERID], 0,  NumToCharPointer);
                            NewRow=false;
                        }
                        else
                        {

                            key =atoi(TrimWhitespaces(field_buffer(orderEntryFields[OE_ORDERKEY],0)));
                            UpdateOrderHeader(orderEntryFields,key);
                        }
                        touchwin(winFullScreen);
                        touchwin(winFormArea);
                        wrefresh(winFormArea);
                        wrefresh(winFullScreen);
                        refresh();
                    }

                }

                if (moveNext==true)
                {
                    form_driver(orderEntryForm, REQ_NEXT_FIELD);
                    form_driver(orderEntryForm, REQ_BEG_LINE);
                }
            }

            break;

        case KEY_UP:


            if (formCurrentField==OE_SAVE)
            {

                form_driver(orderEntryForm, REQ_NEXT_FIELD);//goto part
                form_driver(orderEntryForm, REQ_PREV_FIELD);// goto save

            }
            else
            {
                form_driver(orderEntryForm, REQ_NEXT_FIELD);
                form_driver(orderEntryForm, REQ_PREV_FIELD);
            }

            if (OrderEntryVerify(orderEntryFields,winFullScreen,countLines,lastLineKey,orderTotal,KEY_UP,winFormArea,EditingRow,NewRow,addOrder)==true)
            {

                switch (formCurrentField)

                {
                case OE_ORDERID:
                    set_current_field(orderEntryForm,orderEntryFields[OE_ORDERID]);
                    movePrev=false;
                    move(2,11); // order id
                    break;

                case OE_CUSTID:
                    if (EditingRow==false)
                    {
                        set_current_field(orderEntryForm,orderEntryFields[OE_ORDERID]);
                        movePrev=false;
                        move(2,11); // order id
                    }
                    else
                    {
                        movePrev=false;
                        move(4,11); // cust id
                    }
                    break;
                case OE_TRANSDATE:
                    set_current_field(orderEntryForm,orderEntryFields[OE_CUSTID]);
                    movePrev=false;
                    move(4,11); // cust id
                    break;
                case OE_PARTID:

                    set_current_field(orderEntryForm,orderEntryFields[OE_PARTID]);
                    move(2,31); // part id
                    break;
                case OE_SAVE:
                    set_current_field(orderEntryForm,orderEntryFields[OE_SAVE]);
                    move(7,51); // save
                    break;
                }

                if (movePrev==true)
                {
                    form_driver(orderEntryForm, REQ_PREV_FIELD);
                    form_driver(orderEntryForm, REQ_BEG_LINE);
                }

            }
            break;

        case KEY_LEFT:
            form_driver(orderEntryForm, REQ_PREV_CHAR);
            break;

        case KEY_RIGHT:
            form_driver(orderEntryForm, REQ_NEXT_CHAR);
            break;
        // Delete the char before cursor
        case KEY_BACKSPACE:
        case 127:
            form_driver(orderEntryForm, REQ_DEL_PREV);
            break;
        // Delete the char under the cursor
        case KEY_DC:
            form_driver(orderEntryForm, REQ_DEL_CHAR);
            break;
        default:
            form_driver(orderEntryForm, ch);
            break;
        }
        wrefresh(winFormArea);

    }

    OrderEntryFormCleanup(orderEntryFields);
    return 0;
}
bool  OrderEntryVerify( FIELD *orderEntryFields[],WINDOW *winFullScreen,int &countLines, int &lastLineKey, double &orderTotal,int  keyPress,WINDOW *winMsgArea,bool &EditingRow,bool &NewRow,bool &addOrder)
{
// this is not up to date
//OE_ORDERID = 1, OE_CUSTID=3,OE_TRANSDATE=5,OE_PARTID=7,OE_QTY=9,OE_PRICE=11,OE_SAVE=13;



    std::vector<customer>::iterator custit;
    std::vector<customer> custsResults;
    std::vector<part>::iterator partit;
    std::vector<part> partsResults;


    std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit;
    std::vector<LookUpOrderHeader> LookUpOrderHeaderResults;

    std::vector<LookUpOrderLines>::iterator LookUpOrderLinesit;
    std::vector<LookUpOrderLines> LookUpOrderLinesResults;


    short formCurrentField;
    bool Result=true;
    std::string lookup;
    std::string answer;
    std::string sSpaces(30, ' ');
    int key=0;

    char NumToCharPointer[10];

    tempFptr = current_field(orderEntryForm);
    formCurrentField = field_index(tempFptr);



// Any field that has a lookup has to be verified here and in the lookup logic
    switch (formCurrentField)
    {
    case OE_ORDERID:
        if (keyPress==KEY_DOWN && EditingRow==false)
        {

            lookup=TrimWhitespaces(field_buffer(orderEntryFields[OE_ORDERID], 0));
            key = atoi(lookup.c_str());

            EditingRow=false;
            NewRow=true;
            if (key>0)//value of zero means no order id was entered
            {
                if (GetOrder(LookUpOrderHeaderResults,key)==true)
                {

                    LookUpOrderHeaderit = LookUpOrderHeaderResults.begin();// there should only be one row in result set
                    if (LookUpOrderHeaderit->idtblOrder==key)
                    {

                        sprintf(NumToCharPointer, "%d", LookUpOrderHeaderit->idtblOrder);

                        set_field_buffer(orderEntryFields[OE_ORDERKEY], 0,  NumToCharPointer);
                        sprintf(NumToCharPointer, "%d", LookUpOrderHeaderit->idtblCustomer);
                        set_field_buffer(orderEntryFields[OE_CUSTKEY], 0,  NumToCharPointer);
                        set_field_buffer(orderEntryFields[OE_CUSTID], 0,  LookUpOrderHeaderit->CustId.c_str());
                        set_field_buffer(orderEntryFields[OE_TRANSDATE], 0,  LookUpOrderHeaderit->transDate.c_str());
                        mvwprintw(winFormArea,3, 23, "%s", sSpaces.substr(1,20).c_str());
                        mvwprintw(winFormArea,3, 23, "%s", LookUpOrderHeaderit->CustName.c_str());

                        GetOrderLines(LookUpOrderLinesResults,LookUpOrderHeaderit->idtblOrder,false);

                        DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines,lastLineKey,orderTotal,winFormArea,false);

                        EditingRow=true;
                        NewRow=false;

                        set_current_field(orderEntryForm,orderEntryFields[OE_ORDERID]);

                        break;
                    }
                    else
                    {
                        //???
                    }

                }
            }
        }

        if (EditingRow==false && key>0)

        {

            errMsg(winMsgArea,2,2,"Invalid Order Id");

        }
        addOrder=false;
        if (key==0 && confirmAction(winMsgArea,winFullScreen,2,2,78,"Create new Order",COLOR_PAIR(1),KEY_NPAGE)==true)
        {

            EditingRow=false;
            NewRow=true;
            addOrder=true;
            move(4,11); // cust id
            //break;
        }



        touchwin(winFullScreen);
        wrefresh(winFormArea);
        wrefresh(winFullScreen);
        break;

    case OE_CUSTID:
        lookup=TrimWhitespaces(field_buffer(orderEntryFields[OE_CUSTID], 0));
        Result=GetCust(custsResults,lookup);
        if (Result==true)
        {

            custit = custsResults.begin();
            sprintf(NumToCharPointer, "%d", custit->idtblCustomer);
            set_field_buffer(orderEntryFields[OE_CUSTKEY], 0,  NumToCharPointer);
            mvwprintw(winFormArea,3, 23, "%s", sSpaces.substr(1,20).c_str());
            mvwprintw(winFormArea,3, 23, "%s", custit->Name.c_str());
            wrefresh(winFormArea);
        }
        else
        {
            errMsg(winMsgArea,2,2,"Invalid Customer Id");
            set_current_field(orderEntryForm,orderEntryFields[OE_CUSTID]);
            move(4,11);
            key=atoi(TrimWhitespaces(field_buffer(orderEntryFields[OE_ORDERID], 0)));
            GetOrderLines(LookUpOrderLinesResults,key,false);
            countLines=OE_WINFORMAREA_SCROLL_LINES;
            ClearDisplayOrderLines(winFullScreen,countLines,orderTotal,winFormArea,false);

            DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines,lastLineKey,orderTotal,winFormArea,false);

        }

        break;
    case OE_TRANSDATE:
        break;

    case OE_PARTID:
        lookup=TrimWhitespaces(field_buffer(orderEntryFields[OE_PARTID], 0));
        Result=GetPart(partsResults,lookup);
        if (Result==true)
        {
            partit = partsResults.begin(); // there should only be one row in result set
            sprintf(NumToCharPointer, "%d", partit->idtblPart);
            set_field_buffer(orderEntryFields[OE_PARTKEY], 0,  NumToCharPointer);
            set_field_buffer(orderEntryFields[OE_PARTNAME], 0,  partit->Name.c_str());

            sprintf(NumToCharPointer, "%4.2f", partit->Price);
            set_field_buffer(orderEntryFields[OE_PRICE], 0,  NumToCharPointer);
            sprintf(NumToCharPointer, "%4.2f", partit->Cost);
            set_field_buffer(orderEntryFields[OE_COST], 0,  NumToCharPointer);

            wrefresh(winFormArea);

        }
        else
        {
            errMsg(winMsgArea,2,2,"Invalid Part Id");
            set_current_field(orderEntryForm,orderEntryFields[OE_PARTID]);
            key=atoi(TrimWhitespaces(field_buffer(orderEntryFields[OE_ORDERID], 0)));
            GetOrderLines(LookUpOrderLinesResults,key,false);
            countLines=OE_WINFORMAREA_SCROLL_LINES;
            ClearDisplayOrderLines(winFullScreen,countLines,orderTotal,winFormArea,false);
            DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines,lastLineKey,orderTotal,winFormArea,false);

            move(7,6);
        }
        break;
    case OE_QTY:
        OrderEntryExtendCost(orderEntryFields,winFullScreen,countLines);
        break;
    case OE_PRICE:
        OrderEntryExtendCost(orderEntryFields,winFullScreen,countLines);
        break;
    case OE_SAVE: // save?
        if (keyPress==KEY_DOWN)
        {

            // next 2 lines sync buffer
            form_driver(orderEntryForm, REQ_NEXT_FIELD);//goto part
            form_driver(orderEntryForm, REQ_PREV_FIELD);// goto save



            answer=TrimWhitespaces(field_buffer(orderEntryFields[OE_SAVE], 0));
            if (answer=="Yes"||answer=="Y"||answer=="yes"||answer=="y")
            {
                if(keyPress==KEY_DOWN)
                {
                    if (lastLineKey==0)
                        lastLineKey++;
                    InsertOrderLine(orderEntryFields,lastLineKey);
                    GetOrderLines(LookUpOrderLinesResults,atoi(field_buffer(orderEntryFields[OE_ORDERID], 0)),false);
                    countLines=OE_WINFORMAREA_SCROLL_LINES;
                    ClearDisplayOrderLines(winFullScreen,countLines,orderTotal,winFormArea,false);
                    DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines,lastLineKey,orderTotal,winFormArea,false);


                    set_field_buffer(orderEntryFields[OE_PARTKEY], 0,  "");
                    set_field_buffer(orderEntryFields[OE_PARTID], 0,  "");
                    set_field_buffer(orderEntryFields[OE_PARTNAME], 0,  "");
                    set_field_buffer(orderEntryFields[OE_QTY], 0,  "");
                    set_field_buffer(orderEntryFields[OE_PRICE], 0,  "");
                    set_field_buffer(orderEntryFields[OE_SAVE], 0,  "");
                    mvwprintw(winFormArea,6, 68, "%s", sSpaces.substr(1,8).c_str());
                    wrefresh(winFormArea);

                    set_current_field(orderEntryForm,orderEntryFields[OE_PARTID]);
                    move(7,6); // part id
                }
                else
                {
                    OrderEntryExtendCost(orderEntryFields,winFullScreen,countLines);
                    set_current_field(orderEntryForm,orderEntryFields[OE_QTY]);
                }
            }
            else
                set_current_field(orderEntryForm,orderEntryFields[OE_QTY]);
        }
        break;
    default:
        break;
    }

    return Result;
}
void OrderEntryScrollLines(FIELD *orderEntryFields[],WINDOW *winFullScreen,int  &countLines,double &orderTotal)
{
    std::string answer;
    wattron(winFullScreen,COLOR_PAIR(1));


//    countLines++;
//    mvprintw(21,6, "%d of %d",countLines, OE_SCROLL_LINES);
    wrefresh(winFullScreen);
    pos_form_cursor(orderEntryForm);
    //refresh();


//      sync buffer
    form_driver(orderEntryForm, REQ_NEXT_FIELD);
    form_driver(orderEntryForm, REQ_PREV_FIELD);

    wmove(winFormArea,8,1);
    winsertln(winFormArea);
    double extPrice;
    extPrice = atoi(field_buffer(orderEntryFields[OE_QTY], 0))*atof(field_buffer(orderEntryFields[OE_PRICE], 0));
    orderTotal+=extPrice;
    wattron(winFormArea,A_REVERSE);


    mvwprintw(winFormArea, 8, 1, "%02d",countLines);
    mvwprintw(winFormArea, 8, 5, "%s",field_buffer(orderEntryFields[OE_PARTID], 0));
    mvwprintw(winFormArea, 8, 23, "%s",field_buffer(orderEntryFields[OE_PARTNAME], 0));

    mvwprintw(winFormArea, 8, 45, "%i",atoi(field_buffer(orderEntryFields[OE_QTY], 0)));
    mvwprintw(winFormArea, 8, 50, "%6.2f",atof(field_buffer(orderEntryFields[OE_PRICE], 0)));
    mvwprintw(winFormArea, 8, 68, "%8.2f",extPrice);

    wattroff(winFormArea,A_REVERSE);
    box(winFormArea, 0, 0);
    wrefresh(winFormArea);
    mvwprintw(winFullScreen,22, 69, "%8.2f",orderTotal);

    pos_form_cursor(orderEntryForm);
    wrefresh(winFullScreen);

}
void OrderEntryExtendCost( FIELD *orderEntryFields[], WINDOW *winFullScreen,int  &countLines)
{
    double extPrice;
    form_driver(orderEntryForm, REQ_NEXT_FIELD);
    form_driver(orderEntryForm, REQ_PREV_FIELD);

    extPrice = atoi(field_buffer(orderEntryFields[OE_QTY], 0))*atof(field_buffer(orderEntryFields[OE_PRICE], 0));
    mvwprintw(winFormArea, 6, 68, "%8.2f",extPrice);
    pos_form_cursor(orderEntryForm);
    wrefresh(winFormArea);
}

int OrderEntryDisplayLookup(WINDOW *winFullScreen,FIELD *orderEntryFields[],int &countLines,double &orderTotal,bool &EditingRow,bool &NewRow)
{
    ITEM **myItems;
    int c,r;

    MENU *myLookUpMenu;
    WINDOW *myLookUpMenuWin;
    int NumChoices=0,retValue=0,lookup=0;
    std::string slookup;
    std::vector<part>::iterator partit;
    std::vector<part> partsResults;

    std::vector<customer>::iterator customerit;
    std::vector<customer> customersResults;

    std::vector<LookUpOrderHeader>::iterator LookUpOrderHeaderit;
    std::vector<LookUpOrderHeader> LookUpOrderHeaderResults;

    std::vector<LookUpOrderLines>::iterator LookUpOrderLinesit;
    std::vector<LookUpOrderLines> LookUpOrderLinesResults;


    std::vector <int*> intptrs;
    char NumToCharPointer[10];
    bool exitMenu = false;
    bool Result = false;
    int lastLineKey;
    std::string sSpaces(30, ' ');
    /* Initialize curses */
//    initscr();
//    start_color();
//    cbreak();
//    noecho();
//    keypad(stdscr, TRUE);

    short formCurrentField;
    tempFptr = current_field(orderEntryForm);
    formCurrentField = field_index(tempFptr);
    switch (formCurrentField)
    {

    case OE_ORDERID:

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

        break;

    case OE_CUSTID:

        NumChoices=GetAllCustomers(customersResults);
        NumChoices=customersResults.size();

        myItems = (ITEM **)calloc(NumChoices+1, sizeof(ITEM *));
        r=0;
        for (customerit = customersResults.begin(); customerit != customersResults.end(); ++customerit)
        {
            myItems[r] = new_item(customerit->CustId.c_str(),customerit->Name.c_str());
            intptrs.push_back(&customerit->idtblCustomer);
            set_item_userptr(myItems[r], (int *)*intptrs.back());
            ++r;
        }
        break;
    case OE_PARTID:
        NumChoices=GetAllParts(partsResults);
        NumChoices=partsResults.size();
        myItems = (ITEM **)calloc(NumChoices+1, sizeof(ITEM *));
        r=0;
        for (partit = partsResults.begin(); partit != partsResults.end(); ++partit)
        {
            myItems[r] = new_item(partit->PartId.c_str(),partit->Name.c_str());
            intptrs.push_back(&partit->idtblPart);
            set_item_userptr(myItems[r], (int *)*intptrs.back());
            ++r;
        }

        break;
    default:
        break;
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
        if (formCurrentField==OE_CUSTID)
        {
            int winSize = std::max(NumChoices,5);
            if (winSize<4) winSize=4;
            myLookUpMenuWin = newwin(winSize, 50, 5, 5);
            set_menu_win(myLookUpMenu, myLookUpMenuWin);
            set_menu_sub(myLookUpMenu, derwin(myLookUpMenuWin, (winSize-2), 48, 1, 1));
            set_menu_format(myLookUpMenu, (winSize-2), 1);
        }
        else
        {

            int winSize = std::max(NumChoices,5);
            if (winSize<4) winSize=4;
            myLookUpMenuWin = newwin(winSize, 40, 1, 4);
            set_menu_win(myLookUpMenu, myLookUpMenuWin);
            set_menu_sub(myLookUpMenu, derwin(myLookUpMenuWin, (winSize-2), 38, 1, 1));
            set_menu_format(myLookUpMenu, (winSize-2), 1);

        }

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
                EditingRow=false;
                NewRow=false;

                unpost_menu(myLookUpMenu);
                delwin(myLookUpMenuWin);
                touchwin(winFormArea);
                touchwin(winFullScreen);
                wrefresh(winFormArea);
                wrefresh(winFullScreen);
                refresh();

                unposted=true;
                break;
            case KEY_ENTER:
            case 10:
            case 13:
                ITEM *cur;
                cur = current_item(myLookUpMenu);
                exitMenu=true;
                retValue = (int)item_userptr(cur);
                switch (formCurrentField)
                {
                case OE_ORDERID:
                    EditingRow=false;
                    NewRow=true;



                    if (GetOrder(LookUpOrderHeaderResults,retValue)==true)
                    {
                        LookUpOrderHeaderit = LookUpOrderHeaderResults.begin();

                        if (LookUpOrderHeaderit->idtblOrder==retValue)
                        {
                            lookup = retValue;
                        }

                        LookUpOrderHeaderResults.clear();
                        if (GetOrder(LookUpOrderHeaderResults,lookup)==true)
                        {
                            LookUpOrderHeaderit = LookUpOrderHeaderResults.begin();
// herehere
                            if (LookUpOrderHeaderit->idtblOrder==retValue)
                            {
                                sprintf(NumToCharPointer, "%06d", LookUpOrderHeaderit->idtblOrder);
                                set_field_buffer(orderEntryFields[OE_ORDERID], 0, NumToCharPointer);
                                sprintf(NumToCharPointer, "%d", LookUpOrderHeaderit->idtblOrder);
                                set_field_buffer(orderEntryFields[OE_ORDERKEY], 0,  NumToCharPointer);
                                sprintf(NumToCharPointer, "%d", LookUpOrderHeaderit->idtblCustomer);
                                set_field_buffer(orderEntryFields[OE_CUSTKEY], 0,  NumToCharPointer);
                                set_field_buffer(orderEntryFields[OE_CUSTID], 0,  LookUpOrderHeaderit->CustId.c_str());
                                set_field_buffer(orderEntryFields[OE_TRANSDATE], 0,  LookUpOrderHeaderit->transDate.c_str());
                                mvwprintw(winFormArea,3, 23, "%s", sSpaces.substr(1,20).c_str());
                                mvwprintw(winFormArea,3, 23, "%s", LookUpOrderHeaderit->CustName.c_str());
                                GetOrderLines(LookUpOrderLinesResults,LookUpOrderHeaderit->idtblOrder,false);

                                DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines,lastLineKey,orderTotal,winFormArea,false);
                                EditingRow=true;
                                NewRow=false;
                                unpost_menu(myLookUpMenu);
                                unposted=true;
                                set_current_field(orderEntryForm,orderEntryFields[OE_CUSTID]);
                                field_opts_off(orderEntryFields[OE_ORDERID],O_EDIT|O_ACTIVE);

                                move(4,11);
                                break;

                            }
                        }
                    }
                    else
                    {
                        //???
                    }

                    break;
                case OE_PARTID:

                    set_field_buffer(orderEntryFields[OE_PARTID], 0,  item_name(cur));
                    set_field_buffer(orderEntryFields[OE_PARTNAME], 0,  item_description(cur));
                    unpost_menu(myLookUpMenu);
                    unposted=true;
                    set_current_field(orderEntryForm,orderEntryFields[OE_PARTID]);
                    move(7,6);
                    break;
                case OE_CUSTID:
                    mvwprintw(winFormArea,3, 23, "%s", sSpaces.substr(1,20).c_str());
                    mvwprintw(winFormArea,3, 23, "%s", item_description(cur));
                    set_field_buffer(orderEntryFields[OE_CUSTID], 0,  item_name(cur));
                    set_current_field(orderEntryForm,orderEntryFields[OE_CUSTID]);
                    // jrg 9/11/15
                    //lookup = orderEntryFields[OE_CUSTID];
                    //Result=GetCust(customersResults,lookup);
                    slookup = TrimWhitespaces(field_buffer(orderEntryFields[OE_CUSTID], 0));
                    Result=GetCust(customersResults,slookup);

                    if (Result==true)
                    {

                        customerit = customersResults.begin();
                        sprintf(NumToCharPointer, "%d", customerit->idtblCustomer);
                        set_field_buffer(orderEntryFields[OE_CUSTKEY], 0,  NumToCharPointer);
                        mvwprintw(winFormArea,3, 23, "%s", sSpaces.substr(1,20).c_str());
                        mvwprintw(winFormArea,3, 23, "%s", customerit->Name.c_str());
                        wrefresh(winFormArea);

                    }
                    else
                    {
                        //????
                    }

                    unpost_menu(myLookUpMenu);
                    unposted=true;
                    move(4,11);
                    break;

                } // switch
            } // switch

            if(unposted==false)
            {

                box(myLookUpMenuWin, 0, 0);
                wrefresh(winFormArea);
                wrefresh(myLookUpMenuWin);

            }

        }//while



        wrefresh(winFormArea);


        if(unposted==false)
            unpost_menu(myLookUpMenu);


        free_menu(myLookUpMenu);

        for(int i = 0; i < NumChoices; ++i)
            free_item(myItems[i]);

        endwin();

        customersResults.clear();
        partsResults.clear();
        intptrs.clear();

    }

    wrefresh(winFormArea);

    refresh();

    return retValue;
}
// define without using #define macro above
static bool MyDateFieldChecker(FIELD * fld, const void * data GCC_UNUSED)
{

    char* sdate;
    sdate=TrimWhitespaces(field_buffer(fld, 0));
    //debugMsg("sdate",sdate,0);
    return isValidDate(sdate);
    //return false;

}
static bool MyDateCharChecker(int ch, const void * data GCC_UNUSED)
{

    //debugMsg("MyDateCharChecker","",0);
    return ((isdigit(ch) || ch == '/') ? TRUE : FALSE);

}
FIELDTYPE *CustomDateType = new_fieldtype(MyDateFieldChecker, MyDateCharChecker);

void OrderEntrySetupForm(FIELD *orderEntryFields[],WINDOW *winFullScreen)
{
    start_color();

    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    wattron(stdscr,COLOR_PAIR(1));

//    initscr();
//    noecho();
//    cbreak();
//    keypad(stdscr, TRUE);

    delwin(winFullScreen); // not sure why I had to do this in this module when I didn't in the others


    winFullScreen = newwin(24, 80, 0, 0);
    assert(winFullScreen != NULL);
    wattron(winFullScreen,COLOR_PAIR(1));
    wrefresh(winFullScreen);

    mvwprintw(winFullScreen,21, 62, "Total");


    winFormArea = derwin(winFullScreen, 20, 78, 1, 1);
    assert(winFormArea != NULL);
    wattron(winFormArea,COLOR_PAIR(1));
    wrefresh(winFormArea);

//              new_field(h,w,  tp,left, offscreen, nbuffers);
// not sure if these are up to date
// Order key field hidden ROW=0,COL=2
// Cust key field hidden ROW=0,COL=10
// Part key field hidden ROW=0,COL=18

//Order # label ROW=1,COL=2
// Order # field ROW=1,COL=10

// Cust Id label ROW=3,COL=2
// Cust Id field ROW=3,COL=10

// trans date Label ROW=1,COL=20
// trans date field ROW=1,COL=30


// part id label ROW=5,COL=5
// part id field ROW=6,COL=5
// part name ROW=6,COL=22

// qty label ROW=5,COL=45
// qty field ROW=6,COL=45

// price label ROW=5,COL=50
// price field ROW=6,COL=50
// save label ROW=5,COL=60
// save field ROW=6,COL=60
// cost hidden field ROW=6,COL=64



    orderEntryFields[OE_ORDERKEY] =  new_field(1,  6,  0, 2,0,0);//Order key field hidden
    orderEntryFields[OE_CUSTKEY] =   new_field(1,  6,  0,10,0,0);// Cust key field hidden
    orderEntryFields[OE_PARTKEY] =   new_field(1,  6,  0,18,0,0);// Part key field hidden

    orderEntryFields[3] =  new_field(1, 10,  1, 2,0, 0);// Order # label
    orderEntryFields[OE_ORDERID] =  new_field(1,  6,  1, 10, 0,0);// Order # field

    orderEntryFields[5] =          new_field(1, 10,  3, 2,0, 0);// Cust Id label
    orderEntryFields[OE_CUSTID] =  new_field(1,  6,  3,10,0,0);// Cust Id field

    orderEntryFields[7] =  new_field(1, 10,  1,20,0,0); // trans date Label
    orderEntryFields[OE_TRANSDATE] =  new_field(1, 10,  1,30,0,0);// trans date field

    orderEntryFields[9] =            new_field(1, 10,  5, 5,0,0);// part id label
    orderEntryFields[OE_PARTID] =    new_field(1,  6,  6, 5,0,0);// part id field
    orderEntryFields[OE_PARTNAME] =  new_field(1, 20,  6, 22,0,0);// part name field

    orderEntryFields[12] =       new_field(1, 10, 5,45,0,0);// qty label
    orderEntryFields[OE_QTY] =   new_field(1,  4, 6,45,0,0);// qty field

    orderEntryFields[14] =       new_field(1,10,  5,50,0,0); // price Label
    orderEntryFields[OE_PRICE] = new_field(1, 9,  6,50,0,0);// price field

    orderEntryFields[16] =       new_field(1,10,  5,60,0,0); // save Label
    orderEntryFields[OE_SAVE] =  new_field(1, 3,  6,60,0,0);// save field

    orderEntryFields[OE_COST] =  new_field(1, 3,  6,64,0,0);// cost hidden field

    orderEntryFields[19] = NULL;


    assert( orderEntryFields[3] != NULL
            && orderEntryFields[4] != NULL && orderEntryFields[5] != NULL
            && orderEntryFields[6] != NULL && orderEntryFields[7] != NULL
            && orderEntryFields[8] != NULL && orderEntryFields[9] != NULL
            && orderEntryFields[10] != NULL && orderEntryFields[11] != NULL
            && orderEntryFields[12] != NULL && orderEntryFields[13] != NULL
            && orderEntryFields[14] != NULL && orderEntryFields[15] != NULL
            && orderEntryFields[16] != NULL && orderEntryFields[18] != NULL);

    set_field_buffer(orderEntryFields[OE_ORDERKEY], 0, ""); //Order Key
    set_field_buffer(orderEntryFields[OE_CUSTKEY], 0, ""); // Cust Key
    set_field_buffer(orderEntryFields[OE_PARTKEY], 0, ""); // Part Key

    set_field_buffer(orderEntryFields[3], 0, "Order #");
    set_field_buffer(orderEntryFields[OE_ORDERID], 0, ""); //Order#


    set_field_buffer(orderEntryFields[5], 0, "Cust Id");
    set_field_buffer(orderEntryFields[OE_CUSTID], 0, ""); // Cust id

    set_field_buffer(orderEntryFields[7], 0, "Tran Date");
    set_field_buffer(orderEntryFields[OE_TRANSDATE], 0, "");// Trans date

    set_field_buffer(orderEntryFields[9], 0, "Part Id");
    set_field_buffer(orderEntryFields[OE_PARTID], 0, ""); //part
    set_field_buffer(orderEntryFields[OE_PARTNAME], 0, ""); //part name

    set_field_buffer(orderEntryFields[12], 0, "Qty");
    set_field_buffer(orderEntryFields[OE_QTY], 0, ""); // qty

    set_field_buffer(orderEntryFields[14], 0, "Price");
    set_field_buffer(orderEntryFields[OE_PRICE], 0, "");// price

    set_field_buffer(orderEntryFields[16], 0, "Save?");
    set_field_buffer(orderEntryFields[OE_SAVE], 0, "");// save
    set_field_buffer(orderEntryFields[OE_COST], 0, "");// cost hidden

    set_field_type(orderEntryFields[OE_ORDERKEY], TYPE_INTEGER,6,1,999999);
    set_field_type(orderEntryFields[OE_CUSTKEY], TYPE_INTEGER,6,1,999999);
    set_field_type(orderEntryFields[OE_PARTKEY], TYPE_INTEGER,6,1,999999);
    set_field_type(orderEntryFields[OE_ORDERID], TYPE_INTEGER,6,1,999999);
    set_field_type(orderEntryFields[OE_CUSTID], TYPE_ALNUM,0,1, 999999);

//     FIELDTYPE *CustomDateType = new_fieldtype(MyDateFieldChecker, MyDateCharChecker);
    set_field_type(orderEntryFields[OE_TRANSDATE], CustomDateType);// only numbers and slashes allowed

    set_field_type(orderEntryFields[OE_PARTID], TYPE_ALNUM,0);
    set_field_type(orderEntryFields[OE_PARTNAME], TYPE_ALNUM,0);
    set_field_type(orderEntryFields[OE_QTY], TYPE_INTEGER,0,1, 99999);
    set_field_type(orderEntryFields[OE_PRICE], TYPE_NUMERIC,2,1, 99999);
    set_field_type(orderEntryFields[OE_COST], TYPE_NUMERIC,2,1, 99999);

    char * yesno[] = { "yes", "no", (char *)0 };

    set_field_type(orderEntryFields[OE_SAVE],TYPE_ENUM, yesno, false,false);

    set_field_back(orderEntryFields[OE_ORDERKEY], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryFields[OE_CUSTKEY], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryFields[OE_PARTKEY], A_UNDERLINE|COLOR_PAIR(1));

    set_field_back(orderEntryFields[OE_ORDERID], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryFields[OE_CUSTID], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryFields[OE_TRANSDATE], A_UNDERLINE|COLOR_PAIR(1));

    set_field_back(orderEntryFields[OE_PARTID], A_UNDERLINE|COLOR_PAIR(1));

    set_field_back(orderEntryFields[OE_QTY], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryFields[OE_PRICE], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryFields[OE_SAVE], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryFields[OE_COST], A_UNDERLINE|COLOR_PAIR(1));
    for (int i=0; i<=19; ++i)
        set_field_fore(orderEntryFields[i], COLOR_PAIR(1));



    set_field_opts(orderEntryFields[OE_ORDERKEY], O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[OE_CUSTKEY], O_PUBLIC | O_AUTOSKIP);

    set_field_opts(orderEntryFields[OE_PARTKEY], O_PUBLIC | O_AUTOSKIP);

    set_field_opts(orderEntryFields[3], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[OE_ORDERID], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);

    set_field_opts(orderEntryFields[5], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[OE_CUSTID], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(orderEntryFields[7],O_VISIBLE| O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[OE_TRANSDATE], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(orderEntryFields[9], O_VISIBLE| O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[OE_PARTID], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(orderEntryFields[OE_PARTNAME], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[12], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[OE_QTY], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_RIGHT|O_NULLOK);
    set_field_opts(orderEntryFields[14], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[OE_PRICE], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_RIGHT|O_NULLOK);
    set_field_opts(orderEntryFields[16], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryFields[OE_SAVE], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_RIGHT|O_NULLOK);
    set_field_opts(orderEntryFields[OE_COST],O_PUBLIC |O_AUTOSKIP);


    orderEntryForm = new_form(orderEntryFields);
    assert(orderEntryForm != NULL);
    set_form_win(orderEntryForm,winFullScreen);
    set_form_sub(orderEntryForm,winFormArea);
}
void OrderEntryFormCleanup(FIELD *orderEntryFields[])
{

    unpost_form(orderEntryForm);
    free_form(orderEntryForm);

    for (int i=0; i<19; ++i)
    {
        free_field(orderEntryFields[i]);
    }

    //free_fieldtype(CustomDateType);
    delwin(winFormArea);
    //delwin(winFullScreen);


    endwin();

}
void OrderEntryEditLines(FIELD *orderEntryFields[],WINDOW *winFullScreen,WINDOW *winMsgArea)
{
OrderEntryEditLinesStart:


    bool quit=false,reset=false;
    int countLines=0,OrderKey=0,LineKey=0,lastLineKey=0,ch=0;
    std::string lookup;
    std::string prompt;

    double orderTotal=0;
    char input[3];
    char temp[25];


    std::vector<LookUpOrderLines> LookUpOrderLinesResults;


    winEditLinesFullScreen = newwin(24, 80, 0, 0);
    assert(winEditLinesFullScreen != NULL);
    wattron(winEditLinesFullScreen,COLOR_PAIR(1));

    wrefresh(winEditLinesFullScreen);


    wattron(winEditLinesFullScreen,A_REVERSE);
    //mvwprintw(winEditLinesFullScreen,22, 2,  "F2 Lookup");
    mvwprintw(winEditLinesFullScreen,22, 10, "F3 Delete Line");
    mvwprintw(winEditLinesFullScreen,22, 26, "F4 Edit Line");
    mvwprintw(winEditLinesFullScreen,22, 39, "F5 Refresh");
    mvwprintw(winEditLinesFullScreen,22, 50, "F7 Exit");
    wattroff(winEditLinesFullScreen,A_REVERSE);


    wmove(winEditLinesFullScreen,21,1);
    whline(winEditLinesFullScreen,0,79);
    box(winEditLinesFullScreen, 0, 0);
    wrefresh(winEditLinesFullScreen);
    refresh();

    lookup=TrimWhitespaces(field_buffer(orderEntryFields[OE_ORDERID], 0));
    OrderKey = atoi(lookup.c_str());


    if (OrderKey>0) //value of zero means no order id was entered
    {
        GetOrderLines(LookUpOrderLinesResults,OrderKey,true);
        DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines,lastLineKey,orderTotal,winEditLinesFullScreen,true);
    }
    else
    {
        debugMsg("no order id passed to OrderEntryEditLines","",OrderKey);
    }

    while (quit!=true)
    {

        ch = getch();
        switch (ch)
        {


        case  KEY_F(3): //delete

startdelete:
            getNumericInput(winMsgArea,winEditLinesFullScreen,2,2,2,78,"Enter Line to Delete ",input,COLOR_PAIR(1));

            if (input[0]!='*')
            {
                LineKey=atoi(input);
                LookUpOrderLinesResults.resize(0);

                if (GetOrderLine(LookUpOrderLinesResults,OrderKey,LineKey)>0)

                {

                    prompt="";
                    sprintf(temp, "Delete Order Line %02d", atoi(input));
                    prompt.append(temp);



                    if (confirmAction(winMsgArea,winEditLinesFullScreen,2,2,78,prompt,COLOR_PAIR(1),KEY_F(3))==true)
                    {

                        LineKey=atoi(input);
                        DeleteOrderLine(OrderKey,LineKey);
                        countLines=OE_WINEDITLINES;
                        ClearDisplayOrderLines(winFullScreen,countLines,orderTotal,winEditLinesFullScreen,true);
                        GetOrderLines(LookUpOrderLinesResults,OrderKey,true);
                        DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines, lastLineKey,orderTotal,winEditLinesFullScreen,true);


                    }
                }

                else
                {
                    errMsg(winMsgArea,2,2,"Invalid Entry order line does not exist");
                    if (confirmAction(winMsgArea,winEditLinesFullScreen,2,2,78,"Exit",COLOR_PAIR(1),KEY_F(7))==true)
                    {

                        OrderEntryEditLinesFormCleanup(winFullScreen,-1);


                    }
                    else
                    {
                        goto startdelete;
                        //goto OrderEntryEditLinesStart;

                    }
                }

            }


            touchwin(winEditLinesFullScreen);
            wrefresh(winEditLinesFormArea);
            wrefresh(winEditLinesFullScreen);
            refresh();
            break;

        case KEY_F(4):// edit lines
        {
//                def_prog_mode();
//                endwin();

            OrderEntryEditLinesPopUp(winFullScreen,winMsgArea,OrderKey);


            refresh();


        }

        break;

        case KEY_F(5): //reset
            orderTotal=0;
            GetOrderLines(LookUpOrderLinesResults,OrderKey,true);
            DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines,lastLineKey,orderTotal,winEditLinesFullScreen,true);
            break;

        case KEY_F(7): //exit
            if (confirmAction(winMsgArea,winEditLinesFullScreen,2,2,78,"Exit*",COLOR_PAIR(1),KEY_F(7))==true)
            {
                quit=true;
            }
            break;

        }
    }



//endwin();

    touchwin(winEditLinesFullScreen);
    wrefresh(winEditLinesFullScreen);



}


void OrderEntryEditLinesPopUp(WINDOW *winFullScreen, WINDOW *winMsgArea,int OrderKey)

{

    short formCurrentField;
    bool firstTime=true;
    bool quit=false;
    bool prompt=true;
    bool moveNext=true;
    bool movePrev=true;
    int ch=0,LineKey=0,lastLineKey=0;


    std::string lookup;

    std::vector<LookUpOrderLines> LookUpOrderLinesResults;
    std::vector<LookUpOrderLines>::iterator LookUpOrderLinesit;
    char input[3];
    char NumToCharPointer[20];

OrderEntryEditLinesPopUpStart:
    quit=false;
    int countLines=0;

    OrderEntryEditLinesSetupForm();
    touchwin(winEditLinesFullScreen);
    wrefresh(winEditLinesFullScreen);
    refresh();

    post_form(orderEntryEditLinesForm);
    set_current_field(orderEntryEditLinesForm,orderEntryEditLinesFields[OE_WINEDITLINES_PARTID]);


    if (firstTime==true)
    {


        OrderEntryEditLinesFormCleanup(winFullScreen,OrderKey);
        firstTime=false;

        goto OrderEntryEditLinesPopUpStart;
    }


    if (prompt==true)
    {
        getNumericInput(winMsgArea,winEditLinesFullScreen,2,2,2,78,"Enter Line to Edit ",input,COLOR_PAIR(1));
        prompt=false;

    }

    if (input[0]!='*')
    {

        LineKey=atoi(input);
        if (GetOrderLine(LookUpOrderLinesResults,OrderKey,LineKey)>0)
        {

            LookUpOrderLinesit = LookUpOrderLinesResults.begin();

            sprintf(NumToCharPointer, "%d",LookUpOrderLinesit->idtblPart);
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTKEY], 0, NumToCharPointer); // Part Key
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTID], 0, LookUpOrderLinesit->PartId.c_str()); //part
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTNAME], 0, LookUpOrderLinesit->Name.c_str()); //part name
            sprintf(NumToCharPointer, "%d",LookUpOrderLinesit->Qty);
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_QTY], 0, NumToCharPointer); // qty
            sprintf(NumToCharPointer, "%4.2f",LookUpOrderLinesit->Price);
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PRICE], 0,NumToCharPointer);// price
            sprintf(NumToCharPointer, "%4.2f", LookUpOrderLinesit->Cost);
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_COST], 0, NumToCharPointer);// cost hidden
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_SAVE], 0, "");// save

            //    post_form(orderEntryEditLinesForm);
            box(winEditLinesFormArea, 0, 0);
            pos_form_cursor(orderEntryEditLinesForm);
            wrefresh(winEditLinesFormArea);
        }
        else
        {
            errMsg(winMsgArea,2,2,"Invalid Entry order line does not exist");
            firstTime=true;
            prompt=true;

            if (confirmAction(winMsgArea,winEditLinesFullScreen,2,2,78,"Exit",COLOR_PAIR(1),KEY_F(7))==true)
            {


                OrderEntryEditLinesFormCleanup(winFullScreen,OrderKey);
                quit=true;
            }
            else
            {
                firstTime=true;
                prompt=true;
                goto OrderEntryEditLinesPopUpStart;

            }
        }
    }
    else
    {
        OrderEntryEditLinesFormCleanup(winFullScreen,OrderKey);
        quit=true;

    }


    while (quit!=true)

    {

        tempFptr = current_field(orderEntryEditLinesForm);
        formCurrentField = field_index(tempFptr);
        ch = getch();
        switch (ch)
        {
        case KEY_F(2): // lookup

            if (formCurrentField==OE_WINEDITLINES_PARTID)
            {
                def_prog_mode();
                endwin();

                OrderEntryEditLinesDisplayLookup(countLines);
                wrefresh(winEditLinesFullScreen);
                refresh();
                pos_form_cursor(orderEntryEditLinesForm);


            }
            break;
        case KEY_F(7): //exit
            if (confirmAction(winMsgArea,winEditLinesFullScreen,2,2,78,"Exit # ",COLOR_PAIR(1),KEY_F(7))==true)
            {
                OrderEntryEditLinesFormCleanup(winFullScreen,OrderKey);
                quit=true;

            }
            break;


        case KEY_ENTER:
        case 9: // TAB
        case 10:
        case 13:


            moveNext=true;
            // sync buffer
            if (formCurrentField!=OE_WINEDITLINES_SAVE) form_driver(orderEntryEditLinesForm, REQ_NEXT_FIELD);
            if (formCurrentField!=OE_WINEDITLINES_SAVE) form_driver(orderEntryEditLinesForm, REQ_PREV_FIELD);

            if (OrderEntryEditLinesVerify(winFullScreen,countLines,lastLineKey,KEY_DOWN,OrderKey,LineKey,winMsgArea)==true)

            {


                if (formCurrentField==OE_WINEDITLINES_SAVE)
                {

                    //goto OrderEntryEditLinesPopUpStart;
                    quit=true;
                    //moveNext=false;
                }
                if (moveNext==true)
                {


                    form_driver(orderEntryEditLinesForm, REQ_NEXT_FIELD);
                    form_driver(orderEntryEditLinesForm, REQ_BEG_LINE);

                }



            }
            else
            {


                if (formCurrentField==OE_WINEDITLINES_SAVE)
                {

                    set_current_field(orderEntryEditLinesForm,orderEntryEditLinesFields[OE_PARTID]);
                    //quit=true;
                }
            }

            break;

        case KEY_UP:

            if (formCurrentField==OE_WINEDITLINES_SAVE)
            {


                form_driver(orderEntryEditLinesForm, REQ_NEXT_FIELD);//goto part
                form_driver(orderEntryEditLinesForm, REQ_PREV_FIELD);// goto save


            }
            else
            {
                form_driver(orderEntryEditLinesForm, REQ_NEXT_FIELD);
                form_driver(orderEntryEditLinesForm, REQ_PREV_FIELD);
            }

            if (OrderEntryEditLinesVerify(winFullScreen,countLines,lastLineKey,KEY_UP,OrderKey,LineKey,winFormArea)==true)
            {


                switch (formCurrentField)
                {

                case OE_WINEDITLINES_PARTID:
                    set_current_field(orderEntryEditLinesForm,orderEntryEditLinesFields[OE_WINEDITLINES_PARTID]);
                    wmove(winEditLinesFormArea,4,6); // part id
                    break;
                case OE_WINEDITLINES_SAVE:
                    set_current_field(orderEntryEditLinesForm,orderEntryEditLinesFields[OE_WINEDITLINES_SAVE]);
                    wmove(winEditLinesFormArea,4,61);
                    break;
                }



                if (movePrev==true)
                {

                    form_driver(orderEntryEditLinesForm, REQ_PREV_FIELD);

                    form_driver(orderEntryEditLinesForm, REQ_BEG_LINE);

                }

            }
            else
            {
                if (formCurrentField==OE_WINEDITLINES_SAVE)
                {

                    set_current_field(orderEntryEditLinesForm,orderEntryEditLinesFields[OE_WINEDITLINES_SAVE]);
                    quit=true;
                }

            }

            break;

        case KEY_END:
            form_driver(orderEntryEditLinesForm, REQ_END_FIELD);
            break;
        case KEY_HOME:
            form_driver(orderEntryEditLinesForm, REQ_BEG_FIELD);
            break;

        case KEY_LEFT:
            form_driver(orderEntryEditLinesForm, REQ_PREV_CHAR);
            break;

        case KEY_RIGHT:
            form_driver(orderEntryEditLinesForm, REQ_NEXT_CHAR);
            break;
        // Delete the char before cursor
        case KEY_BACKSPACE:
        case 127:
            form_driver(orderEntryEditLinesForm, REQ_DEL_PREV);
            break;
        // Delete the char under the cursor
        case KEY_DC:
            form_driver(orderEntryEditLinesForm, REQ_DEL_CHAR);
            break;
        default:

            form_driver(orderEntryEditLinesForm, ch);
            break;
        }

        wrefresh(winEditLinesFormArea);

    }
    //jrg 9/11
    //delwin(winEditLinesFormArea);
    delwin(winEditLinesFullScreen);


}


bool OrderEntryEditLinesVerify(WINDOW *winFullScreen,int &countLines,int &lastLineKey, int keyPress,int OrderKey, int LineKey,WINDOW *winMsgArea)
{
//OE_WINEDITLINES_PARTKEY(0), OE_WINEDITLINES_PARTID(2), OE_WINEDITLINES_PARTNAME(3), OE_WINEDITLINES_QTY(5)
//OE_WINEDITLINES_PRICE(7), OE_WINEDITLINES_SAVE(9), OE_WINEDITLINES_COST(10)

    std::vector<part>::iterator partit;
    std::vector<part> partsResults;



    std::vector<LookUpOrderLines>::iterator LookUpOrderLinesit;
    std::vector<LookUpOrderLines> LookUpOrderLinesResults;


    short formCurrentField;
    bool Result=true;
    std::string lookup;
    std::string answer;
    char NumToCharPointer[10];

    tempFptr = current_field(orderEntryEditLinesForm);
    formCurrentField = field_index(tempFptr);

// Any field that has a lookup has to be verified here and in the lookup logic

    switch (formCurrentField)
    {


    case OE_WINEDITLINES_PARTID:


        lookup=TrimWhitespaces(field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTID], 0));
        Result=GetPart(partsResults,lookup);
        if (Result==true)
        {
            partit=partsResults.begin();//there should only be one row in result set

            sprintf(NumToCharPointer, "%d", partit->idtblPart);
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTKEY], 0,  NumToCharPointer);
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTNAME], 0,  partit->Name.c_str());

            sprintf(NumToCharPointer, "%4.2f", partit->Price);
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PRICE], 0,  NumToCharPointer);
            sprintf(NumToCharPointer, "%4.2f", partit->Cost);
            set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_COST], 0,  NumToCharPointer);
            //wmove(winEditLinesFormArea,4,46); //part id

        }

        else
        {
            errMsg(winMsgArea,2,2,"Invalid Part Id");
            set_current_field(orderEntryEditLinesForm,orderEntryEditLinesFields[OE_WINEDITLINES_PARTID]);
            Result=false;

            //wmove(winEditLinesFormArea,4,46); //part id
        }
        break;
    case OE_WINEDITLINES_QTY:
        //OrderEntryEditLinesUpdateTotals(countLines);
        break;
    case OE_WINEDITLINES_PRICE:
        //OrderEntryEditLinesUpdateTotals(countLines);
        break;
    case OE_WINEDITLINES_SAVE: // save?
        if (keyPress==KEY_DOWN)
        {


            // next 2 lines sync buffer
            form_driver(orderEntryEditLinesForm, REQ_NEXT_FIELD);//goto part
            form_driver(orderEntryEditLinesForm, REQ_PREV_FIELD);// goto save

            answer=TrimWhitespaces(field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_SAVE], 0));
            if (answer=="Yes"||answer=="Y"||answer=="yes"||answer=="y")
            {

                if(keyPress==KEY_DOWN)
                {
                    UpdateOrderLine(OrderKey,LineKey);
                    OrderEntryEditLinesFormCleanup(winFullScreen,OrderKey);
                    Result=true;
                }


            }
            else
            {
                if (answer==""||answer=="No"||answer=="N"||answer=="no"||answer=="n")

                {

                    set_current_field(orderEntryEditLinesForm,orderEntryEditLinesFields[OE_WINEDITLINES_PARTID]);
                    Result=false;
                }
                else
                {

                    Result=false;
                }
            }

        }
        break;
    default:
        break;

        box(winEditLinesFullScreen,0,0);
        // caused problem with edit lines form appearing after F7
        //touchwin(winEditLinesFormArea);
        //touchwin(winEditLinesFullScreen);
        wrefresh(winEditLinesFormArea);
        // if this is uncommented because it causes function key labels to apprear and I didn't like that
        //wrefresh(winEditLinesFullScreen);
        refresh();


    }

    return Result;
}

int OrderEntryEditLinesDisplayLookup(int &countLines)
{
    ITEM **myItems;
    MENU *myLookUpMenu;
    WINDOW *myLookUpMenuWin;


    std::vector<part>::iterator partit;
    std::vector<part> partsResults;
    std::vector <int*> intptrs;

    //char NumToCharPointer[10]; jrg 9/10/15

    int NumChoices=0,retValue=0,c=0,r=0;//lookup=0,

    bool exitMenu = false;
    //bool Result = false;
    /* Initialize curses */
//    initscr();
//    start_color();
//    cbreak();
//    noecho();
//    keypad(stdscr, TRUE);

    short formCurrentField;
    tempFptr = current_field(orderEntryEditLinesForm);
    formCurrentField = field_index(tempFptr);
    switch (formCurrentField)
    {


    case OE_WINEDITLINES_PARTID:
        NumChoices=GetAllParts(partsResults);
        NumChoices=partsResults.size();
        myItems = (ITEM **)calloc(NumChoices+1, sizeof(ITEM *));
        r=0;
        for (partit = partsResults.begin(); partit != partsResults.end(); ++partit)
        {
            myItems[r] = new_item(partit->PartId.c_str(),partit->Name.c_str());
            intptrs.push_back(&partit->idtblPart);
            set_item_userptr(myItems[r], (int *)*intptrs.back());
            ++r;
        }

        break;
    default:
        break;
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


        myLookUpMenuWin = newwin(winSize, 40, 2, 4);

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
        //int result=0;
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

            case KEY_ENTER:
            case 10:
            case 13:
                ITEM *cur;
                cur = current_item(myLookUpMenu);
                exitMenu=true;
                retValue = (int)item_userptr(cur);


                switch (formCurrentField)
                {


                case OE_WINEDITLINES_PARTID:



                    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTID], 0,  item_name(cur));
                    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTNAME], 0,  item_description(cur));
                    unpost_menu(myLookUpMenu);
                    unposted=true;
                    set_current_field(orderEntryEditLinesForm,orderEntryEditLinesFields[OE_WINEDITLINES_PARTID]);

                    move(8,6);



                default:

                    break;

                } // switch


            }// switch

            if(unposted==false)
            {
                box(myLookUpMenuWin, 0, 0);
                wrefresh(winEditLinesFormArea);
                wrefresh(myLookUpMenuWin);
            }

            box(winEditLinesFormArea,0,0);
        }// while


        wrefresh(winEditLinesFormArea);

        if(unposted==false)
            unpost_menu(myLookUpMenu);
        free_menu(myLookUpMenu);
        for(int i = 0; i < NumChoices; ++i)
            free_item(myItems[i]);
        endwin();


        partsResults.clear();
        intptrs.clear();

    } //if (NumChoices!=0)

    touchwin(winEditLinesFormArea);
    touchwin(winEditLinesFullScreen);
    wrefresh(winEditLinesFormArea);
    wrefresh(winEditLinesFullScreen);
    refresh();

    return retValue;

}
void OrderEntryEditLinesSetupForm(void)
{
// jrg 9/11
//    double orderTotal;
//    orderTotal=0;
    winEditLinesFormArea = derwin(winEditLinesFullScreen, 6, 78, 5, 1);
    assert(winEditLinesFormArea != NULL);
    wattron(winEditLinesFormArea,COLOR_PAIR(1));
    wrefresh(winEditLinesFormArea);

    orderEntryEditLinesFields[OE_WINEDITLINES_PARTKEY] =   new_field(1,  6,  0,18,0,0);// Part key field hidden
    orderEntryEditLinesFields[1] =                         new_field(1, 10,  2, 5,0,0);// part id label
    orderEntryEditLinesFields[OE_WINEDITLINES_PARTID] =    new_field(1,  6,  3, 5,0,0);// part id field
    orderEntryEditLinesFields[OE_WINEDITLINES_PARTNAME] =  new_field(1, 20,  3, 22,0,0);// part name field
    orderEntryEditLinesFields[4] =                         new_field(1, 10, 2,45,0,0);// qty label
    orderEntryEditLinesFields[OE_WINEDITLINES_QTY] =       new_field(1,  4, 3,45,0,0);// qty field
    orderEntryEditLinesFields[6] =                         new_field(1,10,  2,50,0,0); // price Label
    orderEntryEditLinesFields[OE_WINEDITLINES_PRICE] =     new_field(1, 9,  3,50,0,0);// price field
    orderEntryEditLinesFields[8] =                         new_field(1,10,  2,60,0,0); // save Label
    orderEntryEditLinesFields[OE_WINEDITLINES_SAVE] =      new_field(1, 3,  3,60,0,0);// save field
    orderEntryEditLinesFields[OE_WINEDITLINES_COST] =      new_field(1, 3,  4,64,0,0);// cost hidden field

    orderEntryEditLinesFields[11] = NULL;

    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTKEY], 0, ""); // Part Key


    set_field_buffer(orderEntryEditLinesFields[1], 0, "Part Id");
    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTID], 0, ""); //part
    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTNAME], 0, ""); //part name

    set_field_buffer(orderEntryEditLinesFields[4], 0, "Qty");
    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_QTY], 0, ""); // qty

    set_field_buffer(orderEntryEditLinesFields[6], 0, "Price");
    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PRICE], 0, "");// price

    set_field_buffer(orderEntryEditLinesFields[8], 0, "Save?");
    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_SAVE], 0, "");// save
    set_field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_COST], 0, "");// cost hidden


    set_field_type(orderEntryEditLinesFields[OE_WINEDITLINES_PARTKEY], TYPE_INTEGER,6,1,999999);
    set_field_type(orderEntryEditLinesFields[OE_WINEDITLINES_PARTID], TYPE_ALNUM,0);
    set_field_type(orderEntryEditLinesFields[OE_WINEDITLINES_PARTNAME], TYPE_ALNUM,0);
    set_field_type(orderEntryEditLinesFields[OE_WINEDITLINES_QTY], TYPE_INTEGER,0,1, 99999);
    set_field_type(orderEntryEditLinesFields[OE_WINEDITLINES_PRICE], TYPE_NUMERIC,2,1, 99999);
    set_field_type(orderEntryEditLinesFields[OE_WINEDITLINES_COST], TYPE_NUMERIC,2,1, 99999);

    char * yesno[] = { "yes", "no", (char *)0 };

    set_field_type(orderEntryEditLinesFields[OE_WINEDITLINES_SAVE],TYPE_ENUM, yesno, false,false);


    set_field_back(orderEntryEditLinesFields[OE_WINEDITLINES_PARTKEY], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryEditLinesFields[OE_WINEDITLINES_PARTID], A_UNDERLINE|COLOR_PAIR(1));

    set_field_back(orderEntryEditLinesFields[OE_WINEDITLINES_QTY], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryEditLinesFields[OE_WINEDITLINES_PRICE], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryEditLinesFields[OE_WINEDITLINES_SAVE], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(orderEntryEditLinesFields[OE_WINEDITLINES_COST], A_UNDERLINE|COLOR_PAIR(1));
    for (int i=0; i<=11; ++i)
        set_field_fore(orderEntryEditLinesFields[i], COLOR_PAIR(1));



    set_field_opts(orderEntryEditLinesFields[OE_WINEDITLINES_PARTKEY], O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryEditLinesFields[1], O_VISIBLE| O_PUBLIC | O_AUTOSKIP);

    set_field_opts(orderEntryEditLinesFields[OE_WINEDITLINES_PARTID], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(orderEntryEditLinesFields[OE_WINEDITLINES_PARTNAME], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryEditLinesFields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryEditLinesFields[OE_WINEDITLINES_QTY], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_RIGHT|O_NULLOK);
    set_field_opts(orderEntryEditLinesFields[6], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryEditLinesFields[OE_WINEDITLINES_PRICE], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_RIGHT|O_NULLOK);
    set_field_opts(orderEntryEditLinesFields[8], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(orderEntryEditLinesFields[OE_WINEDITLINES_SAVE], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_RIGHT|O_NULLOK);
    set_field_opts(orderEntryEditLinesFields[OE_WINEDITLINES_COST],O_PUBLIC |O_AUTOSKIP);


    orderEntryEditLinesForm = new_form(orderEntryEditLinesFields);
    assert(orderEntryEditLinesForm != NULL);
    set_form_win(orderEntryEditLinesForm,winEditLinesFullScreen);
    set_form_sub(orderEntryEditLinesForm,winEditLinesFormArea);

}
void OrderEntryEditLinesFormCleanup(WINDOW *winFullScreen,int OrderKey)
{
    std::vector<LookUpOrderLines> LookUpOrderLinesResults;
    int countLines=0,lastLineKey=0;
    double orderTotal=0;


    unpost_form(orderEntryEditLinesForm);
    free_form(orderEntryEditLinesForm);


    for (int i=0; i<11; ++i)
    {
        free_field(orderEntryEditLinesFields[i]);
    }



    endwin();



    if(OrderKey>0)
    {
        GetOrderLines(LookUpOrderLinesResults,OrderKey,true);
        countLines=OE_WINEDITLINES;
        ClearDisplayOrderLines(winFullScreen,countLines,orderTotal,winEditLinesFullScreen,true);
        DisplayOrderLines(winFullScreen,LookUpOrderLinesResults,countLines,lastLineKey,orderTotal,winEditLinesFullScreen,true);
    }
}
void  DisplayOrderLines(WINDOW *winFullScreen,std::vector<LookUpOrderLines> &LookUpOrderLinesResults,int &countLines, int &lastLineKey,double &orderTotal,WINDOW *win, bool EditLinesMode)
{

    std::vector<LookUpOrderLines>::iterator LookUpOrderLinesit;
    int counter=0,startRow=0;
    double extPrice=0;

    counter=0;
    extPrice=0;

    if (EditLinesMode==true)
    {
        startRow=1;
        wmove(win,startRow,1);
        mvwprintw(win, 1,5, "Part Id");
        mvwprintw(win, 1,23,"Part Name");
        mvwprintw(win, 1,45,"Qty");
        mvwprintw(win, 1,51,"Price");
        mvwprintw(win, 1, 69, "Ext. Price");

    }
    else
    {
        startRow=8;
        wmove(winFormArea,startRow,1);

    }

    for (LookUpOrderLinesit = LookUpOrderLinesResults.begin(); LookUpOrderLinesit != LookUpOrderLinesResults.end(); ++LookUpOrderLinesit)
    {
        counter++;
        if (lastLineKey<LookUpOrderLinesit->idtblOrderLine)
            lastLineKey=LookUpOrderLinesit->idtblOrderLine;



        extPrice = LookUpOrderLinesit->Qty*LookUpOrderLinesit->Price;
        orderTotal+=extPrice;

        if (EditLinesMode==true)
        {
            if (startRow+counter>OE_WINEDITLINES)
            {
// TODO set full screen flag and???

            }
            else
            {
                mvwprintw(win, startRow+counter, 1, "%02d",LookUpOrderLinesit->idtblOrderLine);
                mvwprintw(win, startRow+counter,5, "%s",LookUpOrderLinesit->PartId.c_str());
                mvwprintw(win, startRow+counter,23,"%s",LookUpOrderLinesit->Name.c_str());
                mvwprintw(win, startRow+counter,45,"%i",LookUpOrderLinesit->Qty);
                mvwprintw(win, startRow+counter, 50, "%6.2f",LookUpOrderLinesit->Price);
                mvwprintw(win, startRow+counter, 70, "%8.2f",extPrice);
            }

        }
        else
        {

            winsertln(win);
            wattron(win,A_REVERSE);
            mvwprintw(win,startRow+counter, 1, "%02d",LookUpOrderLinesit->idtblOrderLine);
            mvwprintw(win,startRow+counter, 5, "%s",LookUpOrderLinesit->PartId.c_str());
            mvwprintw(win,startRow+counter, 23, "%s",LookUpOrderLinesit->Name.c_str());
            mvwprintw(win,startRow+counter , 45, "%i",LookUpOrderLinesit->Qty);
            mvwprintw(win, startRow+counter, 50, "%6.2f",LookUpOrderLinesit->Price);
            mvwprintw(win, startRow+counter, 70, "%8.2f",extPrice);
            wattroff(win,A_REVERSE);
        }

    }



    if (EditLinesMode==true)
    {
        mvwprintw(win,22, 70, "%8.2f",orderTotal);
        countLines=counter;
        //mvwprintw(win,21,2, "%d Order Lines",countLines);
        box(win, 0, 0);
        wrefresh(win);
    }
    else
    {
        mvwprintw(winFullScreen,21, 71, "%8.2f",orderTotal);
        countLines=counter;
        mvwprintw(winFullScreen,21,2, "%d Order Lines",countLines);
        //box(win, 0, 0);
        wrefresh(winFullScreen);
    }

    wrefresh(win);
    refresh();
    countLines++;
    lastLineKey++;




}
void ClearDisplayOrderLines(WINDOW *winFullScreen,int &countLines, double &orderTotal,WINDOW *win,bool EditLinesMode)
{
    int startRow=0;

    if (EditLinesMode==true)
    {
        startRow=1;
        wmove(win,startRow,1);

    }
    else
    {
        startRow=8;
        wmove(winFormArea,startRow,1);

    }

    for(int i=0; i<countLines; ++i)
    {


        wmove(win,startRow+i,1);
        wclrtoeol(win);

    }
    countLines=0;
    orderTotal=0;
    if (EditLinesMode==true)
    {
        mvwprintw(winEditLinesFullScreen,22, 70, "%8.2f",orderTotal);
        //box(winEditLinesFormArea, 0, 0);
    }
    else
    {
        mvwprintw(winFullScreen,21, 70, "%8.2f",orderTotal);
//    mvprintw(21,6, "%d of %d",countLines, OE_SCROLL_LINES);
//    box(winFormArea, 0, 0);
        wrefresh(win);
        refresh();
    }

}
int GetAllOrderHeaders(std::vector<LookUpOrderHeader> &LookUpOrderHeaderResults)
{
    int rows=0;
    mysqlpp::Connection con;
    string sql;

    try
    {


        con.connect("mydb","localHost", "root", "moonpie");

        sql = "SELECT h.idtblOrder,h.charidtblOrder,h.idtblCustomer,h.transDate, c.CustId,c.Name AS CustName ";
        sql.append("FROM mydb.tblOrderHeader h ");
        sql.append("INNER JOIN  tblCustomer c ");
        sql.append("ON h.idtblCustomer = c.idtblCustomer ");
        sql.append("ORDER BY idtblOrder ");

        mysqlpp::Query query = con.query(sql);

        LookUpOrderHeaderResults.clear();
        query.storein(LookUpOrderHeaderResults);
        rows = LookUpOrderHeaderResults.size();

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return -1;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions; e.g. type mismatch populating 'stock'
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return -1;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return -1;
    }

    return rows;
}

bool GetOrder(std::vector<LookUpOrderHeader> &LookUpOrderHeaderResults,int key)
{
    //int row=0;jrg 9/10/15
    mysqlpp::Connection con;
    string sql;
    char NumToCharPointer[10];

    sprintf(NumToCharPointer, "%d", key);

    try
    {
        con.connect("mydb","localHost", "root", "moonpie");
        sql = "SELECT h.idtblOrder,h.charidtblOrder,h.idtblCustomer,h.transDate,c.Name AS CustName, c.CustId ";
        sql.append("FROM mydb.tblOrderHeader h ");
        sql.append("INNER JOIN  tblCustomer c ");
        sql.append("ON h.idtblCustomer = c.idtblCustomer ");
        sql.append("WHERE h.idtblOrder = " );
        sql.append(NumToCharPointer);
        sql.append(" ORDER BY idtblOrder ");
        //debugMsg(sql.c_str(),"",0);

        mysqlpp::Query query = con.query(sql);
        LookUpOrderHeaderResults.clear();
        query.storein(LookUpOrderHeaderResults);

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return false;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions; e.g. type mismatch populating 'stock'
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return false;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return false;
    }

    if ( LookUpOrderHeaderResults.empty() == true)
        return false;
    else
        return true;
}


int GetOrderLines(std::vector<LookUpOrderLines> &LookUpOrderLinesResults,int key,bool EditLinesMode)
{

    int rows=0;

    mysqlpp::Connection con;

    string sql;
    char NumToCharPointer[10];
    sprintf(NumToCharPointer, "%d", key);

    try
    {

        con.connect("mydb","localHost", "root", "moonpie");

        sql = "SELECT l.idtblOrder,l.idtblOrderLine,l.idtblPart,l.Qty,l.Price,l.Cost,p.PartId,p.Name ";
        sql.append("FROM mydb.tblOrderLines l ");
        sql.append("INNER JOIN  tblPart p ");
        sql.append("ON l.idtblPart = p.idtblPart ");
        sql.append("WHERE l.idtblOrder = " );
        sql.append(NumToCharPointer);

        if(EditLinesMode==true)
            sql.append(" ORDER BY idtblOrder,idtblOrderLine");
        else
            sql.append(" ORDER BY idtblOrder,idtblOrderLine desc");
        //debugMsg("Inside GetOrderLines sql =",sql.c_str(),0);

        mysqlpp::Query query = con.query(sql);

        LookUpOrderLinesResults.clear();

        query.storein(LookUpOrderLinesResults);

        rows = LookUpOrderLinesResults.size();

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return -1;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions; e.g. type mismatch populating 'stock'
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return -1;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return -1;
    }


    return rows;
}

int GetOrderLine(std::vector<LookUpOrderLines> &LookUpOrderLinesResults,int Orderkey,int LineKey)
{
    int rows=0;
    mysqlpp::Connection con;
    string sql;
    char NumToCharPointer[10];

    try
    {
        sql="";
        con.connect("mydb","localHost", "root", "moonpie");
        sql = "SELECT l.idtblOrder,l.idtblOrderLine,l.idtblPart,l.Qty,l.Price,l.Cost,p.PartId,p.Name ";
        //debugMsg("Inside GetOrderLine sql =",sql.c_str(),0);

        sql.append("FROM mydb.tblOrderLines l ");
        sql.append("INNER JOIN  tblPart p ");
        sql.append("ON l.idtblPart = p.idtblPart ");
        sql.append("WHERE l.idtblOrder = " );
        sprintf(NumToCharPointer, "%d", Orderkey);
        sql.append(NumToCharPointer);
        sql.append(" AND idtblOrderLine = ");
        sprintf(NumToCharPointer, "%d", LineKey);
        sql.append(NumToCharPointer);
        //debugMsg("GetOrderLine",sql.c_str(),0);
        mysqlpp::Query query = con.query(sql);

        LookUpOrderLinesResults.resize(0);

        query.storein(LookUpOrderLinesResults);

        rows = LookUpOrderLinesResults.size();
    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return -1;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions; e.g. type mismatch populating 'stock'
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return -1;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return -1;
    }
    //debugMsg("GetOrderLine","rows",rows);

    return rows;
}


int GetNextOrderId()
{

    string sql;
    std::vector<nextOrderId>::iterator nextOrderIdit;
    std::vector<nextOrderId> NextOrderIdResults;


    mysqlpp::Connection con;
    mysqlpp::sql_int identity;

    try
    {

        con.connect("mydb","localHost", "root", "moonpie");
        sql = "SELECT max(idtblOrder) as MaxOrderId FROM mydb.tblOrderHeader";
//        sql = "SELECT idtblOrder FROM mydb.tblOrderHeader where idtblOrder = 2";
        mysqlpp::Query query = con.query(sql);
        //debugMsg("sql.c_str()",sql.c_str(),0);
        NextOrderIdResults.clear();
        query.storein(NextOrderIdResults);
        nextOrderIdit = NextOrderIdResults.begin();
        identity=nextOrderIdit->MaxOrderId;
        //debugMsg("MaxOrderId","",identity);

        return ++identity;

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return -1;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return -1;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return -1;
    }


}

int InsertOrderHeader(FIELD *orderEntryFields[])
{

    string sql;
    char NumToCharPointer[10];

    mysqlpp::Connection con;
    mysqlpp::sql_int identity;
    try
    {

        con.connect("mydb","localHost", "root", "moonpie");

        mysqlpp::Query query = con.query();

        sql = "INSERT INTO mydb.tblOrderHeader ";
        sql.append ("(idtblCustomer,transDate) VALUES ( \"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_CUSTKEY], 0)));
        sql.append("\""",\"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_TRANSDATE], 0)));
        sql.append("\"");

        sql.append(")");
        //debugMsg("sql.c_str()",sql.c_str(),0);

        query << sql;
        query.execute();

        identity=query.insert_id();
        //debugMsg("Auto-increment value:","",identity);
        sprintf(NumToCharPointer, "%06d", identity);
        sql = "UPDATE mydb.tblOrderHeader SET ";
        sql.append ("charidtblOrder = \"");
        sql.append (NumToCharPointer);
        sql.append("\"");
        sprintf(NumToCharPointer, "%0d", identity);
        sql.append(" WHERE idtblOrder = ");
        sql.append (NumToCharPointer);
        //debugMsg("sql.c_str()",sql.c_str(),0);

        query << sql;
        query.execute();

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return -1;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return -1;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return -1;
    }

    return identity;
}
int UpdateOrderHeader(FIELD *orderEntryFields[],int key)
{

    string sql;
    mysqlpp::Connection con;
    // jrg 9/10/15 mysqlpp::sql_int identity;
    char NumToCharPointer[10];

    try
    {

        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();

        sql = "UPDATE mydb.tblOrderHeader SET ";
        sql.append ("idtblCustomer= \"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_CUSTKEY], 0)));
        sql.append("\""" , transDate= \"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_TRANSDATE], 0)));
        sql.append("\""" WHERE idtblOrder = ");
        sprintf(NumToCharPointer, "%d", key);
        sql.append(NumToCharPointer);
        //debugMsg("sql.c_str()",sql.c_str(),0);
        query << sql;
        query.execute();

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return -1;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return -1;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return -1;
    }

    return 0;
}
int DeleteOrder(int key)
{
    string sql;
    char NumToCharPointer[10];
    sprintf(NumToCharPointer, "%d", key);

    mysqlpp::Connection con;
    try
    {
        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();


        sql = "DELETE FROM mydb.tblOrderLines WHERE idtblOrder = ";
        sql.append(NumToCharPointer);
        //debugMsg("sql.c_str()",sql.c_str(),0);

        query << sql;
        query.execute();

        sql = "DELETE FROM mydb.tblOrderHeader WHERE idtblOrder = ";
        sql.append(NumToCharPointer);
        //debugMsg("sql.c_str()",sql.c_str(),0);

        query << sql;
        query.execute();

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        return -1;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        return -1;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        return -1;
    }

    return 0;
}
bool DeleteOrderLine(int Orderkey,int LineKey)
{
    string sql;
    char NumToCharPointer[10];
    bool Result = true;


    mysqlpp::Connection con;
    try
    {
        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();

        sprintf(NumToCharPointer, "%d", Orderkey);
        sql = "DELETE FROM mydb.tblOrderLines WHERE idtblOrder = ";
        sql.append(NumToCharPointer);
        sql.append(" AND idtblOrderLine = ");
        sprintf(NumToCharPointer, "%d", LineKey);
        sql.append(NumToCharPointer);
        //debugMsg("sql.c_str()",sql.c_str(),0);



        query << sql;
        query.execute();

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        Result=false;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        Result=false;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        Result=false;
    }

    return Result;

}
bool UpdateOrderLine(int Orderkey,int LineKey)
{

    string sql;
    mysqlpp::Connection con;
    //mysqlpp::sql_int identity;
    char NumToCharPointer[10];
    bool Result=true;

    try
    {

        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();

        // sprintf(NumToCharPointer, "%d", linenum);
        sql="UPDATE mydb.tblOrderLines";
        sql.append(" SET idtblPart = ");
        sql.append(TrimWhitespaces(field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PARTKEY], 0)));
        //sql.append("\""",\"");
        sql.append(", Qty = ");
        sql.append (TrimWhitespaces(field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_QTY], 0)));
        //sql.append("\""",\"");
        sql.append(", Price = ");
        sql.append (TrimWhitespaces(field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_PRICE], 0)));
        //sql.append("\""",\"");
        sql.append(", Cost = ");
        sql.append (TrimWhitespaces(field_buffer(orderEntryEditLinesFields[OE_WINEDITLINES_COST], 0)));
        //sql.append("\""",\"");

        //sql.append("\"");

        sql.append(" WHERE idtblOrder= ");
        sprintf(NumToCharPointer, "%d", Orderkey);
        sql.append(NumToCharPointer);
        sql.append(" AND idtblOrderLine= ");
        sprintf(NumToCharPointer, "%d", LineKey);
        sql.append(NumToCharPointer);

        //debugMsg("sql.c_str()",sql.c_str(),0);


        query << sql;
        query.execute();

    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        cerr << "Query error: " << er.what() << endl;
        Result=false;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        Result=false;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        cerr << "Error: " << er.what() << endl;
        Result=false;
    }

    return Result;
}

bool InsertOrderLine(FIELD *orderEntryFields[], int linenum)
{

    string sql;
    char NumToCharPointer[10];
    bool Result=true;

    mysqlpp::Connection con;
    //mysqlpp::sql_int identity;
    try
    {

        con.connect("mydb","localHost", "root", "moonpie");

        mysqlpp::Query query = con.query();




        sql = "INSERT INTO mydb.tblOrderLines ";
        sql.append ("(idtblOrder,idtblOrderLine,idtblPart,Qty,Price,Cost) VALUES ( \"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_ORDERKEY], 0)));
        sql.append("\""",\"");
        sprintf(NumToCharPointer, "%d", linenum);
        sql.append (NumToCharPointer);
        sql.append("\""",\"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_PARTKEY], 0)));
        sql.append("\""",\"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_QTY], 0)));
        sql.append("\""",\"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_PRICE], 0)));
        sql.append("\""",\"");
        sql.append (TrimWhitespaces(field_buffer(orderEntryFields[OE_COST], 0)));
        sql.append("\"");

        sql.append(")");
        //debugMsg("sql.c_str()",sql.c_str(),0);


        query << sql;
        query.execute();


    }
    catch (const mysqlpp::BadQuery& er)
    {
        // Handle any query errors
        move(22,2); // so error will show at bottom of screen
        cerr << "Query error: " << er.what() << endl;
        Result=false;
    }
    catch (const mysqlpp::BadConversion& er)
    {
        // Handle bad conversions
        move(22,2); // so error will show at bottom of screen
        cerr << "Conversion error: " << er.what() << endl <<
             "\tretrieved data size: " << er.retrieved <<
             ", actual size: " << er.actual_size << endl;
        Result=false;
    }
    catch (const mysqlpp::Exception& er)
    {
        // Catch-all for any other MySQL++ exceptions
        move(22,2); // so error will show at bottom of screen
        cerr << "Error: " << er.what() << endl;
        Result=false;
    }

    return Result;
}
