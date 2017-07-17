#define MYSQLPP_SSQLS_NO_STATICS
#include "main.hpp"


using namespace std;
// TRIED TO MAKE THESE NON GLOBAL BUT COULDN'T
static FORM *custEntryForm;
static WINDOW *winFormArea;

int CustEntry(WINDOW *winMsgArea,WINDOW *winFullScreen)
{

    short formCurrentField;
    bool firstTime=true,quit=false,NewRow=false,EditingRow=false;
    int key=0,ch=0,tabPlace=0;
    static FIELD *custEntryFields[14];
    static int custEntryFieldsTabOrder[14];
    static FIELD *tempFptr;

CustEntryStart:
    quit=false;

     //debugMsg("custEntryFieldsTabOrder[3]","before",custEntryFieldsTabOrder[3]);
    CustEntrySetupForm(custEntryFields,winFullScreen,custEntryFieldsTabOrder);
    //debugMsg("custEntryFieldsTabOrder[3]","after",custEntryFieldsTabOrder[3]);
    post_form(custEntryForm);
    //set_current_field(custEntryForm,custEntryFields[CE_CUSTID]);

//    tempFptr = current_field(custEntryForm);
//    formCurrentField = field_index(tempFptr);
    //debugMsg("formCurrentField","",formCurrentField);


    wattron(winFullScreen,A_REVERSE);
    mvwprintw(winFullScreen,22, 2, "F2 Lookup");
    mvwprintw(winFullScreen,22, 12, "F3 Delete");
    mvwprintw(winFullScreen,22, 22, "F4 Save");
    mvwprintw(winFullScreen,22, 32, "F5 New");
    mvwprintw(winFullScreen,22, 42, "F7 Exit");
    wattroff(winFullScreen,A_REVERSE);
    box(winFormArea, 0, 0);



    wrefresh(winFormArea);
    wrefresh(winFullScreen);
    refresh();

    pos_form_cursor(custEntryForm);

    if (firstTime==true)
    {
        firstTime=false;
        CustEntryFormCleanup(custEntryFields);
        goto CustEntryStart;
    }

    // custom tab logic
    set_current_field(custEntryForm,custEntryFields[custEntryFieldsTabOrder[tabPlace]]);
    int x=0,top=0,left=0;
    field_info(custEntryFields[custEntryFieldsTabOrder[tabPlace]],x, x,&top, &left, x, x);
    move(top+1,left+1);
    //pos_form_cursor(custEntryForm);
//    tempFptr = current_field(custEntryForm);
//    formCurrentField = field_index(tempFptr);



    while (quit!=true)
    {


        ch = getch();
        switch (ch)
        {

        case KEY_F(2): // lookup
            if (formCurrentField==CE_CUSTID)
            {
                def_prog_mode();
                endwin();

                CustEntryDisplayLookup(custEntryFields,EditingRow,NewRow);
                refresh();
                pos_form_cursor(custEntryForm);
            }
            break;
        case  KEY_F(3)://delete

            if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Delete Customer",COLOR_PAIR(1),KEY_F(3))==true)
            {

                key=atoi(TrimWhitespaces(field_buffer(custEntryFields[CE_CUSTKEY], 0)));
                DeleteCust(custEntryFields,key);
                goto CustEntryStart;
            }
            pos_form_cursor(custEntryForm);
            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);

            break;

        case KEY_F(4):// save
            //      sync buffer
            form_driver(custEntryForm, REQ_NEXT_FIELD);
            form_driver(custEntryForm, REQ_PREV_FIELD);

            key=atoi(TrimWhitespaces(field_buffer(custEntryFields[CE_CUSTKEY], 0)));
            if (NewRow==true)
            {

                InsertCust(custEntryFields);

            }
            else
            {

                UpdateCust(custEntryFields,key);

            }
            goto CustEntryStart;
            break;

        case KEY_F(5): //reset
            CustEntryFormCleanup(custEntryFields);
            goto CustEntryStart;
            break;
        case KEY_F(7): //exit
            if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Exit",COLOR_PAIR(1),KEY_F(7))==true)
            {
                quit=true;
            }

            pos_form_cursor(custEntryForm);
            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            break;

        case KEY_END:
            form_driver(custEntryForm, REQ_END_FIELD);
            break;
        case KEY_HOME:
            form_driver(custEntryForm, REQ_BEG_FIELD);
            break;
        case 9: // TAB
        case KEY_DOWN:
        case KEY_ENTER:

        case 10:
        case 13:

            if(formCurrentField==CE_CUSTID)
            {

                //      sync buffer
                form_driver(custEntryForm, REQ_NEXT_FIELD);
                form_driver(custEntryForm, REQ_PREV_FIELD);
                if (CustEntryVerify(custEntryFields,winFullScreen,winMsgArea,EditingRow,NewRow)==true)
                {

                    form_driver(custEntryForm, REQ_NEXT_FIELD);
                    form_driver(custEntryForm, REQ_BEG_LINE);



                }
                else
                {

                    set_current_field(custEntryForm,custEntryFields[CE_CUSTID]);
                    pos_form_cursor(custEntryForm);
                    break;

                }
            }
            else
            {
                // custom tab logic
                //debugMsg("tabPlace","",tabPlace);
                if (tabPlace<6)
                {
                    set_current_field(custEntryForm,custEntryFields[custEntryFieldsTabOrder[tabPlace]]);
                    tabPlace++;
                    if (tabPlace==6) tabPlace=0;

                }

            }
            break;
        case KEY_UP:

            if(formCurrentField==CE_CUSTID)
            {
                //      sync buffer
                form_driver(custEntryForm, REQ_NEXT_FIELD);
                form_driver(custEntryForm, REQ_PREV_FIELD);
                if (CustEntryVerify(custEntryFields,winFullScreen,winMsgArea,EditingRow,NewRow)==true)
                {

                    form_driver(custEntryForm, REQ_PREV_FIELD);
                    form_driver(custEntryForm, REQ_BEG_LINE);
                    break;
                }
                else
                {

                }
            }
            else
            {
                // custom tab logic
                if (tabPlace==0) tabPlace=6;
                //debugMsg("tabPlace","",tabPlace);

                if (tabPlace>0)
                {
                    tabPlace--;
                    set_current_field(custEntryForm,custEntryFields[custEntryFieldsTabOrder[tabPlace]]);

                }

            }
            break;

        case KEY_LEFT:
            form_driver(custEntryForm, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(custEntryForm, REQ_NEXT_CHAR);
            break;
        // Delete the char before cursor
        case KEY_BACKSPACE:
        case 127:
            form_driver(custEntryForm, REQ_DEL_PREV);
            break;
        // Delete the char under the cursor
        case KEY_DC:
            form_driver(custEntryForm, REQ_DEL_CHAR);
            break;
        default:
            form_driver(custEntryForm,ch);
            break;
        }// switch

        wrefresh(winFormArea);

    }// while


    CustEntryFormCleanup(custEntryFields);


    return 0;

}



bool  CustEntryVerify(FIELD *custEntryFields[],WINDOW *winFullScreen,WINDOW *winMsgArea, bool &EditingRow, bool &NewRow)
{

    std::vector<customer>::iterator custit;
    std::vector<customer> custsResults;

    short formCurrentField;
    bool Result=false;

    std::string lookup;
    static FIELD *tempFptr;

    tempFptr = current_field(custEntryForm);
    formCurrentField = field_index(tempFptr);

// This code will not be executed for any field that has a lookup unless the user enters the value without using the F2 key.
// put additional verify logic in the lookup procedure
    switch (formCurrentField)
    {

    case CE_CUSTID:


        lookup=TrimWhitespaces(field_buffer(custEntryFields[CE_CUSTID], 0));
        if (GetCust(custsResults,lookup)==true)
        {
            EditingRow=true;
            NewRow=false;
            custit = custsResults.begin();
            SetCustEntryFields(custEntryFields,custit,EditingRow);
            Result=true;

        }
        else
        {
            EditingRow=false;
            NewRow=true;
            Result=false;
            if ( askQuestion(winMsgArea,2,2,78,"Customer not on file, do you want to add it  ?",COLOR_PAIR(1)) =="Yes")
            {
                Result=true;
                NewRow=true;
                set_current_field(custEntryForm,custEntryFields[CE_CUSTID]);
            }
            else
            {
                Result=false;
                NewRow=false;
                set_current_field(custEntryForm,custEntryFields[CE_CUSTID]);
            }

            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            pos_form_cursor(custEntryForm);
            refresh();
            break;

        }
        break;
    case CE_NAME:
        break;
    case CE_ADDR:
        break;
    case CE_CITY:
        break;
    case CE_STATE:
        break;
    case CE_ZIP:
        break;

    default:
        break;
    } // end switch

    return Result;
}

int CustEntryDisplayLookup(FIELD *custEntryFields[],bool &EditingRow,bool &NewRow)
{
    ITEM **myItems;
    int c,r;

    MENU *myLookUpMenu;
    WINDOW *myLookUpMenuWin;
    int NumChoices=0;
    int retValue=0;
    std::vector<customer> custsResults;
    std::vector<customer>::iterator custit;
    std::vector <int*> intptrs;
    bool exitMenu = false;
    std::string lookup;
    static FIELD *tempFptr;

//    /* Initialize curses */
//    initscr();
//    start_color();
//    cbreak();
//    noecho();
//    keypad(stdscr, TRUE);

    short formCurrentField;
    tempFptr = current_field(custEntryForm);
    formCurrentField = field_index(tempFptr);

    switch (formCurrentField)
    {
    case CE_CUSTID:
        NumChoices=GetAllCustomers(custsResults);
        NumChoices=custsResults.size();
        myItems = (ITEM **)calloc(NumChoices+1, sizeof(ITEM *));
        r=0;

        for (custit = custsResults.begin(); custit != custsResults.end(); ++custit)
        {

            myItems[r] = new_item(custit->CustId.c_str(),custit->Name.c_str());

            intptrs.push_back(&custit->idtblCustomer);


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

        int winSize = max(NumChoices,5);
        if (winSize<4) winSize=4;
        myLookUpMenuWin = newwin(winSize, 40, 2, 4);
        set_menu_win(myLookUpMenu, myLookUpMenuWin);
        set_menu_sub(myLookUpMenu, derwin(myLookUpMenuWin, (winSize-2), 38, 1, 1));
        set_menu_format(myLookUpMenu, (winSize-2), 1);

        keypad(myLookUpMenuWin, TRUE);
        set_menu_mark(myLookUpMenu, " > ");
        wattron(myLookUpMenuWin,COLOR_PAIR(1)); // color of box lines
        box(myLookUpMenuWin, 0, 0);
        refresh();
        post_menu(myLookUpMenu);
        wrefresh(myLookUpMenuWin);

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
                SetCustEntryFields(custEntryFields,custit,EditingRow); // custit is not  used have to pass something in this case
                break; // test exit without choice
            case KEY_ENTER:
            case 10:
            case 13:
                ITEM *cur;
                cur = current_item(myLookUpMenu);
                exitMenu=true;
                retValue = (int)item_userptr(cur);


                if (formCurrentField==CE_CUSTID)
                {
                    EditingRow=false;
                    NewRow=true;
                    for (custit = custsResults.begin(); custit != custsResults.end(); ++custit)
                    {

                        if (custit->idtblCustomer==retValue)
                        {
                            lookup = custit->CustId.c_str();
                        }
                    }

                    custsResults.clear();
                    if (GetCust(custsResults,lookup)==true)
                    {
                        custit = custsResults.begin();

                        if (custit->idtblCustomer==retValue)
                        {
                            EditingRow=true;
                            NewRow=false;
                            SetCustEntryFields(custEntryFields,custit,EditingRow);

                        }
                        else
                        {
                            //??
                        }



                    }
                }


            } // end switch

            //break;
        } // end while

        box(myLookUpMenuWin, 0, 0);
        wrefresh(winFormArea);
        wrefresh(myLookUpMenuWin);

    } // end if (NumChoices!=0)


    unpost_menu(myLookUpMenu);
    free_menu(myLookUpMenu);
    for(int i = 0; i < NumChoices; ++i)
        free_item(myItems[i]);
    endwin();

    custsResults.clear();
    intptrs.clear();

    wrefresh(winFormArea);

    return retValue;

}

void  SetCustEntryFields(FIELD *custEntryFields[],std::vector<customer>::iterator custit,bool EditingRow)
{
    char NumToCharPointer[10];
    if (EditingRow==true)
    {
        set_field_buffer(custEntryFields[CE_CUSTID], 0,  custit->CustId.c_str());
        set_field_buffer(custEntryFields[CE_NAME], 0,  custit->Name.c_str());
        set_field_buffer(custEntryFields[CE_ADDR], 0,  custit->Address.c_str());
        set_field_buffer(custEntryFields[CE_CITY], 0,  custit->City.c_str());
        set_field_buffer(custEntryFields[CE_STATE], 0,  custit->State.c_str());
        set_field_buffer(custEntryFields[CE_ZIP], 0,  custit->Zip.c_str());

        sprintf(NumToCharPointer, "%d", custit->idtblCustomer);
        set_field_buffer(custEntryFields[CE_CUSTKEY], 0, NumToCharPointer);
    }
    else
    {

        set_field_buffer(custEntryFields[CE_CUSTID], 0, "");

        set_field_buffer(custEntryFields[CE_NAME], 0,  "");
        set_field_buffer(custEntryFields[CE_ADDR], 0,  "");
        set_field_buffer(custEntryFields[CE_CITY], 0,  "");
        set_field_buffer(custEntryFields[CE_STATE], 0,  "");
        set_field_buffer(custEntryFields[CE_ZIP], 0,  "");
        sprintf(NumToCharPointer, "%d", 0);
        set_field_buffer(custEntryFields[CE_CUSTKEY], 0, NumToCharPointer);
    }

}

void  CustEntrySetupForm(FIELD *custEntryFields[],WINDOW *winFullScreen,int custEntryFieldsTabOrder[])
{


    winFullScreen = newwin(24, 80, 0, 0);
    assert(winFullScreen != NULL);
    wattron(winFullScreen,COLOR_PAIR(1));
    wrefresh(winFullScreen);
    winFormArea = derwin(winFullScreen, 20, 78, 1, 1);
    assert(winFormArea != NULL);
    wattron(winFormArea,COLOR_PAIR(1));
    wrefresh(winFormArea);

//              new_field(h,w,  tp,left, offscreen, nbuffers);
//custEntryFieldsTabOrder[0]=CE_NAME; //5
//custEntryFieldsTabOrder[1]=CE_ZIP;//13
//custEntryFieldsTabOrder[2]=CE_CITY;//9
//custEntryFieldsTabOrder[3]=CE_STATE;//11
//custEntryFieldsTabOrder[4]=CE_ADDR;//7
//custEntryFieldsTabOrder[5]=CE_CUSTID;//3

custEntryFieldsTabOrder[0]=CE_CUSTID; //3
custEntryFieldsTabOrder[1]=CE_NAME;//5
custEntryFieldsTabOrder[2]=CE_ADDR;//7
custEntryFieldsTabOrder[3]=CE_CITY;//9
custEntryFieldsTabOrder[4]=CE_STATE;//11
custEntryFieldsTabOrder[5]=CE_ZIP;//13



    custEntryFields[0]        = new_field(1, 10,  0, 2,0,0);// Cust key label
    custEntryFields[CE_CUSTKEY]=new_field(1,  6,  0, 10,0,0);//Cust key field
    custEntryFields[2]         =new_field(1, 10,  1, 2,0, 0);// Cust Id label
    custEntryFields[CE_CUSTID] =new_field(1,  6,  1,10, 0,0);// Cust Id field
    custEntryFields[4] =        new_field(1, 10,  3,2,0, 0);// Cust Name label
    custEntryFields[CE_NAME] =  new_field(1, 20,  3,10,0,0);// Cust Name field
    custEntryFields[6] =        new_field(1, 10,  5, 2,0,0); // Cust Address Label
    custEntryFields[CE_ADDR] =  new_field(1, 30,  5,10,0,0);// Cust Address field
    custEntryFields[8] =        new_field(1, 10,  7, 2,0,0);// Cust City label
    custEntryFields[CE_CITY] =  new_field(1, 30,  7,10,0,0);//Cust City field
    custEntryFields[10] =       new_field(1, 10,   7,42,0,0); // Cust State Label
    custEntryFields[CE_STATE] = new_field(1, 2,   7,49,0,0);// Cust State field
    custEntryFields[12] =       new_field(1, 10,   7,55,0,0);// Cust Zip label
    custEntryFields[CE_ZIP] =   new_field(1, 5,   7,59,0,0);//Cust Zip field
    custEntryFields[14] = NULL;

    assert(custEntryFields[0] != NULL && custEntryFields[1] != NULL
           && custEntryFields[2] != NULL && custEntryFields[3] != NULL
           && custEntryFields[4] != NULL && custEntryFields[5] != NULL
           && custEntryFields[6] != NULL && custEntryFields[7] != NULL
           && custEntryFields[8] != NULL && custEntryFields[9] != NULL
           && custEntryFields[10] != NULL && custEntryFields[11] != NULL
           && custEntryFields[12] != NULL && custEntryFields[13] != NULL);

    set_field_buffer(custEntryFields[0], 0, "Key");
    set_field_buffer(custEntryFields[CE_CUSTKEY], 0, ""); //Key
    set_field_buffer(custEntryFields[2], 0, "Cust Id");
    set_field_buffer(custEntryFields[CE_CUSTID], 0, ""); //Cust Id
    set_field_buffer(custEntryFields[4], 0, "Name");
    set_field_buffer(custEntryFields[CE_NAME], 0, ""); // Name
    set_field_buffer(custEntryFields[6], 0, "Address");
    set_field_buffer(custEntryFields[CE_ADDR], 0, "");// Address
    set_field_buffer(custEntryFields[8], 0, "City");
    set_field_buffer(custEntryFields[CE_CITY], 0, ""); //City
    set_field_buffer(custEntryFields[10], 0, "State");
    set_field_buffer(custEntryFields[CE_STATE], 0, ""); //state
    set_field_buffer(custEntryFields[12], 0, "Zip");
    set_field_buffer(custEntryFields[CE_ZIP], 0, ""); //Zip

    set_field_type(custEntryFields[CE_CUSTKEY], TYPE_NUMERIC,0,1, 999999);// key
    set_field_type(custEntryFields[CE_CUSTID], TYPE_ALNUM,0);// cust id
    // documenation states by default a field will except any data input by the user so
    // dont do a set_field_type if you need to have a text field with spaces.
    set_field_back(custEntryFields[CE_CUSTKEY], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(custEntryFields[CE_CUSTID], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(custEntryFields[CE_NAME], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(custEntryFields[CE_ADDR], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(custEntryFields[CE_CITY], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(custEntryFields[CE_STATE], A_UNDERLINE|COLOR_PAIR(1));
    set_field_back(custEntryFields[CE_ZIP], A_UNDERLINE|COLOR_PAIR(1));

    for (int i=0; i<=13; ++i)
        set_field_fore(custEntryFields[i], COLOR_PAIR(1));

    set_field_opts(custEntryFields[0], O_PUBLIC | O_AUTOSKIP);
    set_field_opts(custEntryFields[CE_CUSTKEY],  O_PUBLIC |O_AUTOSKIP);
    set_field_opts(custEntryFields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(custEntryFields[CE_CUSTID], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(custEntryFields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(custEntryFields[CE_NAME], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(custEntryFields[6], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(custEntryFields[CE_ADDR], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(custEntryFields[8], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(custEntryFields[CE_CITY], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(custEntryFields[10], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(custEntryFields[CE_CITY], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);
    set_field_opts(custEntryFields[12], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(custEntryFields[CE_CITY], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);

    custEntryForm = new_form(custEntryFields);
    assert(custEntryForm != NULL);
    set_form_win(custEntryForm,winFullScreen);
    set_form_sub(custEntryForm,winFormArea);
}

void  CustEntryFormCleanup(FIELD *custEntryFields[])
{
    unpost_form(custEntryForm);
    free_form(custEntryForm);
    for (int i=0; i<13; i++)
    {
        free_field(custEntryFields[i]);
    }
    delwin(winFormArea);

    endwin();
}

int InsertCust(FIELD *custEntryFields[])
{
    string sql;

    mysqlpp::Connection con;
    mysqlpp::sql_int identity;

    try
    {
        con.connect("mydb","localHost", "root", "moonpie");

        customer row;
        row.CustId = TrimWhitespaces(field_buffer(custEntryFields[CE_CUSTID], 0));
        row.Name =TrimWhitespaces(field_buffer(custEntryFields[CE_NAME], 0));
        row.Address = TrimWhitespaces(field_buffer(custEntryFields[CE_ADDR], 0));
        row.City = TrimWhitespaces(field_buffer(custEntryFields[CE_CITY], 0));
        row.State = TrimWhitespaces(field_buffer(custEntryFields[CE_STATE], 0));
        row.Zip =TrimWhitespaces(field_buffer(custEntryFields[CE_ZIP], 0));

        mysqlpp::Query query = con.query();

        sql = "INSERT INTO mydb.tblPart ";
        sql.append ("(CustId, Name, Address, City, State, Zip) VALUES ( \"");
        sql.append (row.CustId);
        sql.append("\""",\"");
        sql.append (row.Name);
        sql.append("\""",\"");
        sql.append (row.Address);
        sql.append("\""",\"");
        sql.append (row.City);
        sql.append("\""",\"");
        sql.append (row.State);
        sql.append("\""",\"");
        sql.append (row.Zip);

        sql.append("\""")");

        //debugMsg((char*)sql.c_str(),"",0);

        query << sql;
        query.execute();

        identity=query.insert_id();
        //debugMsg("Auto-increment value:","",identity);

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

int UpdateCust(FIELD *custEntryFields[],int key)
{
    string sql;
    mysqlpp::Connection con;

    char NumToCharPointer[10];

    try
    {

        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();

        sql = "UPDATE mydb.tblCustomer SET ";
        sql.append ("CustId= \"");
        sql.append (TrimWhitespaces(field_buffer(custEntryFields[CE_CUSTID], 0)));
        sql.append("\""" , Name= \"");
        sql.append (TrimWhitespaces(field_buffer(custEntryFields[CE_NAME], 0)));
        sql.append("\""" , Address= \"");
        sql.append(TrimWhitespaces(field_buffer(custEntryFields[CE_ADDR], 0)));
        sql.append("\""" , City= \"");
        sql.append(TrimWhitespaces(field_buffer(custEntryFields[CE_CITY], 0)));
        sql.append("\""" , State= \"");
        sql.append(TrimWhitespaces(field_buffer(custEntryFields[CE_STATE], 0)));
        sql.append("\""" , Zip= \"");
        sql.append(TrimWhitespaces(field_buffer(custEntryFields[CE_ZIP], 0)));
        sql.append("\""" WHERE idtblCustomer = ");
        sprintf(NumToCharPointer, "%d", key);
        sql.append(NumToCharPointer);
        //debugMsg((char*)sql.c_str(),"",0);
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

int DeleteCust(FIELD *custEntryFields[],int key)
{
    string sql;
    char NumToCharPointer[10];
    mysqlpp::Connection con;
    sprintf(NumToCharPointer, "%d", key);

    try
    {

        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();

        sql = "DELETE FROM tblCustomer WHERE idtblCustomer= ";
        sql.append(NumToCharPointer);
        //debugMsg("inside DeleteCust",sql.c_str(),0);

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
