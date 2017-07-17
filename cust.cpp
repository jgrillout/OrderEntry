
// for customer entry
//const int CE_CUSTKEY(1);
//const int CE_CUSTID (3);
//const int CE_NAME (5);
//const int CE_ADDR(7);
//const int CE_CITY(9);
//const int CE_STATE(11);
//const int CE_ZIP(13);

int CustEntry()
{

    short formCurrentField;
    bool firstTime=true;
    int key;
    int ch;

CustEntryStart:


    CustEntrySetupForm();
    post_form(form);
    set_current_field(form,custEntryFields[CE_CUSTID]);



    wattron(stdscr,A_REVERSE);
    mvwprintw(stdscr,23, 2, "F2 Lookup");
    mvwprintw(stdscr,23, 12, "F3 Delete");
    mvwprintw(stdscr,23, 22, "F4 Save");
    mvwprintw(stdscr,23, 32, "F5 Reset");
    mvwprintw(stdscr,23, 42, "F7 Exit");
    wattroff(stdscr,A_REVERSE);
    box(winFormArea, 0, 0);
    move(2,11);


    wrefresh(winFormArea);

    wrefresh(winFullScreen);
    refresh();
    pos_form_cursor(form);

    if (firstTime==true)
    {
        firstTime=false;
        CustEntryFormCleanup();

        goto CustEntryStart;
    }
//    debugMsg("before while","",formCurrentField);

    while ((ch = getch()) != KEY_F(7)||quit==true)
    {
        tempFptr = current_field(form);
        formCurrentField = field_index(tempFptr);
        //debugMsg("formCurrentField","",formCurrentField);
        switch (ch)
        {

        case KEY_F(2): // lookup
            if (formCurrentField==CE_CUSTID)
            {
                def_prog_mode();
                endwin();
                CustEntryDisplayLookup();
                refresh();
                form_driver(form,REQ_NEXT_FIELD);
            }
            break;
        case  KEY_F(3)://delete

            if (ConfirmAction(stdscr,21,2,79,"Are you sure you want to delete this Customer ?",COLOR_PAIR(1))=="Yes")
            {
                key=atoi(TrimWhitespaces(field_buffer(custEntryFields[CE_PARTKEY], 0)));
                DeletePart(key);
                goto CustEntryStart;
            }
            pos_form_cursor(form);

            break;

        case KEY_F(4):// save
            key=atoi(TrimWhitespaces(field_buffer(custEntryFields[CE_CUSTKEY], 0)));
            if (NewRow==true)
                InsertCust();
            else

                UpdateCust(key);
            goto CustEntryStart;
            break;
            // F2 is handled in CustEntryDriver
        case KEY_F(5): //reset
                CustEntryFormCleanup();
                goto CustEntryStart;
                break;
        default:
            CustEntryDriver(ch);
            break;
        }
    }


    if (ConfirmAction(stdscr,21,2,79,"Are you sure you want to quit ?",COLOR_PAIR(1))=="Yes")
    {

        CustEntryFormCleanup();

    }
    else
    {
        CustEntryFormCleanup();
        goto CustEntryStart;
    }
    return 0;

}


static void CustEntryDriver(int ch)
{
    short formCurrentField;
    tempFptr = current_field(form);
    formCurrentField = field_index(tempFptr);

//    string key;
//
//
    switch (ch)
    {



    case KEY_END:
        form_driver(form, REQ_END_FIELD);
        break;
    case KEY_HOME:
        form_driver(form, REQ_BEG_FIELD);
        break;
    case KEY_DOWN:
    case KEY_ENTER:
    case 9: // TAB
    case 10:// MAY BE SAME AS ENTER ON SOME TERMINALS),
    case 13:// MAY BE SAME AS ENTER ON SOME TERMINALS

        if(formCurrentField==CE_CUSTID)
        {

            //      must do next 2 lines or the current field buffer won't be sync with what is displayed
            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_PREV_FIELD);
            if (CustEntryVerify()==true)
            {
                form_driver(form, REQ_NEXT_FIELD);
                form_driver(form, REQ_BEG_LINE);

            }
            else
            {

                pos_form_cursor(form);

            }
        }
        else
        {

            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_BEG_LINE);

        }
        break;
    case KEY_UP:

        if(formCurrentField==CE_CUSTID)
        {
            //      must do next 2 lines or the current field buffer won't be sync with what is displayed
            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_PREV_FIELD);
            if (CustEntryVerify()==true)
            {
                form_driver(form, REQ_PREV_FIELD);
                form_driver(form, REQ_BEG_LINE);
                break;
            }
            else
            {
                set_current_field(form,custEntryFields[CE_CUSTID]);
                break;
            }
        }
        else
        {
            form_driver(form, REQ_PREV_FIELD);
            form_driver(form, REQ_BEG_LINE);
        }

        break;


    case KEY_LEFT:
        form_driver(form, REQ_PREV_CHAR);
        break;

    case KEY_RIGHT:
        form_driver(form, REQ_NEXT_CHAR);
        break;

    // Delete the char before cursor
    case KEY_BACKSPACE:
    case 127:
        form_driver(form, REQ_DEL_PREV);
        break;

    // Delete the char under the cursor
    case KEY_DC:
        form_driver(form, REQ_DEL_CHAR);
        break;

    default:

        form_driver(form, ch);
        break;
    }


    wrefresh(winFormArea);
}


bool  CustEntryVerify()
{

    std::vector<customer>::iterator custit;
    std::vector<customer> custsResults;
    char NumToCharPointer[10];

    short formCurrentField;
    bool Result=false;
    std::string lookup;

    tempFptr = current_field(form);
    formCurrentField = field_index(tempFptr);

// This code will not be executed for any field that has a lookup unless the user enters the value without using the F2 key.
// put additional verify logic in the lookup procedure
    switch (formCurrentField)
    {

    case CE_CUSTID:
        //debugMsg("CE_CUSTID","",0);
        lookup=TrimWhitespaces(field_buffer(custEntryFields[CE_CUSTID], 0));
        //debugMsg("CE_CUSTID",(char *)lookup.c_str(),1);
        if (GetCust(custsResults,lookup)==true)
        {
            for (custit = custsResults.begin(); custit != custsResults.end(); ++custit)
            {

                set_field_buffer(custEntryFields[CE_CUSTID], 0,  custit->CustId.c_str());
                set_field_buffer(custEntryFields[CE_NAME], 0,  custit->Name.c_str());
                set_field_buffer(custEntryFields[CE_ADDR], 0,  custit->Address.c_str());
                set_field_buffer(custEntryFields[CE_CITY], 0,  custit->City.c_str());
                set_field_buffer(custEntryFields[CE_STATE], 0,  custit->State.c_str());
                set_field_buffer(custEntryFields[CE_ZIP], 0,  custit->Zip.c_str());

                sprintf(NumToCharPointer, "%d", custit->idtblPart);
                set_field_buffer(custEntryFields[CE_CUSTKEY], 0, NumToCharPointer);
                EditingRow=true;
                NewRow=false;
            }
            Result=true;
        }
        else
        {
            EditingRow=false;
            NewRow=true;
            Result=false;
            if ( ConfirmAction(stdscr,21,2,79,"Customer not on file, do you want to add it ?",COLOR_PAIR(1)) =="Yes")
            {
                NewRow=true;
                set_current_field(form,custEntryFields[CE_NAME]);

            }
            else
            {
                NewRow=false;
                set_current_field(form,custEntryFields[CE_CUSTID]);
            }
            //move(2,11);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            refresh();
            //pos_form_cursor(form);

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
    }

    return Result;



}


int CustEntryDisplayLookup()
{
    ITEM **myItems;
    int c,r;

    MENU *myLookUpMenu;
    WINDOW *myLookUpMenu_win;
    int NumChoices=0;
    int retValue=0;
    std::vector<customer> custsResults;
    std::vector<customer>::iterator custit;
    std::vector <int*> ptrs;
    char NumToCharPointer[10];


    bool exitMenu = false;
    /* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);


    short formCurrentField;
    tempFptr = current_field(form);
    formCurrentField = field_index(tempFptr);

    switch (formCurrentField)
    {



    case CE_CUSTID:
        //debugMsg("CE_CUSTID","a",0);
        NumChoices=GetAllCustomers(custsResults);


        NumChoices=custsResults.size();


        myItems = (ITEM **)calloc(NumChoices+1, sizeof(ITEM *));

        r=0;
        for (custit = custsResults.begin(); custit != custsResults.end(); ++custit)
        {

            myItems[r] = new_item(custit->CustId.c_str(),custit->Name.c_str());

            ptrs.push_back(&custit->idtblPart);


            set_item_userptr(myItems[r], (int *)*ptrs.back());
            ++r;


        }

        break;


    default:
        break;
    }

    if (NumChoices!=0)
    {

        //debugMsg("menu","a",r);

        /* Create menu */
        myLookUpMenu = new_menu((ITEM **)myItems);
        //debugMsg("menu","b",0);

        set_menu_fore(myLookUpMenu, COLOR_PAIR(1) | A_REVERSE);
        //debugMsg("menu","c",0);
        set_menu_back(myLookUpMenu, COLOR_PAIR(1));
        //debugMsg("menu","d",0);

        /* Create the window to be associated with the menu */
        //newwin(rows,cols,y_org,x_org)
        //derwin(WINDOW *orig, int nlines, int ncols,int begin_y, int begin_x);

        int winSize = max(NumChoices,5);
        if (winSize<4) winSize=4;
        //debugMsg("winSize","",winSize);

        myLookUpMenu_win = newwin(winSize, 40, 2, 4);
        //debugMsg("menu","e",0);
        set_menu_win(myLookUpMenu, myLookUpMenu_win);
        //debugMsg("menu","f",0);
        set_menu_sub(myLookUpMenu, derwin(myLookUpMenu_win, (winSize-2), 38, 1, 1));
        //debugMsg("menu","g",0);
        set_menu_format(myLookUpMenu, (winSize-2), 1);
        //debugMsg("menu","h",0);


        keypad(myLookUpMenu_win, TRUE);



        //set_menu_format(myLookUpMenu, 10, 1);
        set_menu_mark(myLookUpMenu, " > ");

        wattron(myLookUpMenu_win,COLOR_PAIR(1)); // color of box lines
        box(myLookUpMenu_win, 0, 0);

        refresh();
        //debugMsg("menu","j",0);
        post_menu(myLookUpMenu);
        //debugMsg("menu","k",0);

        wrefresh(myLookUpMenu_win);
        //debugMsg("menu","l",0);

        int result=0;

        while(exitMenu==false)
        {
            c = wgetch(myLookUpMenu_win);
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

                retValue = (long)item_userptr(cur);


                if (formCurrentField==CE_CUSTID)
                {

                    EditingRow=false;
                    NewRow=true;


                    for (custit = custsResults.begin(); custit != custsResults.end(); ++custit)
                    {
                        if (custit->idtblCustomer==retValue)
                        {
                            set_field_buffer(custEntryFields[CE_CUSTID], 0,  custit->CustId.c_str());
                            set_field_buffer(custEntryFields[CE_NAME], 0,  custit->Name.c_str());
                            set_field_buffer(custEntryFields[CE_ADDR], 0,  custit->Address.c_str());
                            set_field_buffer(custEntryFields[CE_CITY], 0,  custit->City.c_str());
                            set_field_buffer(custEntryFields[CE_STATE], 0,  custit->State.c_str());
                            set_field_buffer(custEntryFields[CE_ZIP], 0,  custit->Zip.c_str());

                            EditingRow=true;
                            NewRow=false;
                            break;

                        }

                    }


                    break;

                }

                box(myLookUpMenu_win, 0, 0);
                wrefresh(winFormArea);

                wrefresh(myLookUpMenu_win);


            } // switch
        }// while



        /* Unpost and free all the memory taken up */
        unpost_menu(myLookUpMenu);
        free_menu(myLookUpMenu);
        for(int i = 0; i < NumChoices; ++i)
            free_item(myItems[i]);
        endwin();

        custsResults.clear();
        ptrs.clear();

    } // if (NumChoices!=0)

    return retValue;

}



void CustEntrySetupForm()
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

    custEntryFields[0]        = new_field(1, 10,  0, 2,0,0);// Cust key label
    custEntryFields[CE_PARTKEY]=new_field(1,  6,  0, 10,0,0);//Cust key field
    custEntryFields[2]         =new_field(1, 10,  1, 2,0, 0);// Cust Id label
    custEntryFields[CE_PARTID] =new_field(1,  6,  1,10, 0,0);// Cust Id field
    custEntryFields[4] =        new_field(1, 10,  3, 2,0, 0);// Cust Name label
    custEntryFields[CE_NAME] =  new_field(1, 20,  3,10,0,0);// Cust Name field
    custEntryFields[6] =        new_field(1, 10,  5,2,0,0); // Cust Cost Label
    custEntryFields[CE_COST] =  new_field(1, 6,   5,10,0,0);// Cust Cost field
    custEntryFields[8] =        new_field(1, 10,  7, 2,0,0);// Cust Price label
    custEntryFields[CE_PRICE] = new_field(1,  6,  7, 10,0,0);//Cust Price field


    custEntryFields[10] = NULL;


    assert(custEntryFields[0] != NULL && custEntryFields[1] != NULL
           && custEntryFields[2] != NULL && custEntryFields[3] != NULL
           && custEntryFields[4] != NULL && custEntryFields[5] != NULL
           && custEntryFields[6] != NULL && custEntryFields[7] != NULL
           && custEntryFields[8] != NULL && custEntryFields[9] != NULL);



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



    form = new_form(custEntryFields);
    assert(form != NULL);
    set_form_win(form,winFullScreen);
    set_form_sub(form,winFormArea);


}


void CustEntryFormCleanup()
{
    unpost_form(form);
    free_form(form);
    for (int i=0;i<13;i++)
    {
    free_field(custEntryFields[i]);
    }
    delwin(winFormArea);
    delwin(winFullScreen);

    endwin();

}

int InsertCust()
{
    string sql;
    //debugMsg("inside InsertCust","",0);

    char NumToCharPointer[10];
    mysqlpp::Connection con;
    mysqlpp::sql_int identity;

    try
    {
        // Establish the connection to the database server.
        con.connect("mydb","localHost", "root", "moonpie");
        //debugMsg("inside InsertCust","after  con.connect",0);


        //debugMsg("before set row","",0);


        cust row;



        row.CustId = TrimWhitespaces(field_buffer(custEntryFields[CE_CUSTID], 0));
        row.Name =TrimWhitespaces(field_buffer(custEntryFields[CE_NAME], 0));
        row.Address = TrimWhitespaces(field_buffer(custEntryFields[CE_ADDR], 0));
        row.City = TrimWhitespaces(field_buffer(custEntryFields[CE_CITY], 0));
        row.State = TrimWhitespaces(field_buffer(custEntryFields[CE_STATE], 0));
        row.Zip =TrimWhitespaces(field_buffer(custEntryFields[CE_ZIP], 0));
        //debugMsg("after set row","",0);



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

        sql.append("\""")");\
        //debugMsg((char*)sql.c_str(),"",0);

        //debugMsg("before insert","",0);


        query << sql;
        query.execute();

        identity=query.insert_id();
        //debugMsg("Auto-increment value:","",identity);

        //debugMsg("after insert","",0);

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
int UpdateCust(int key)
{
string sql;
    //debugMsg("inside UpdateCust","",0);


    mysqlpp::Connection con;
    mysqlpp::sql_int identity;
    char NumToCharPointer[10];


    try
    {
        // Establish the connection to the database server.
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
        ql.append("\""" , Zip= \"");
        sql.append(TrimWhitespaces(field_buffer(custEntryFields[CE_ZIP], 0)));
        sql.append("\""" WHERE idtblCustomer = ");
        sprintf(NumToCharPointer, "%d", key);
        sql.append(NumToCharPointer);
        debugMsg((char*)sql.c_str(),"",0);
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
int DeletePart(int key)
{
    string sql;
    char NumToCharPointer[10];
    sprintf(NumToCharPointer, "%d", key);

    //debugMsg("inside InsertCust","",0);

    //ostringstream sKey;

    //sKey << key;

    mysqlpp::Connection con;


    try
    {
        // Establish the connection to the database server.
        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();

        sql = "DELETE FROM tblCustomer WHERE idtblCust= ";
        sql.append(NumToCharPointer);

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
