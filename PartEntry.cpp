#define MYSQLPP_SSQLS_NO_STATICS
#include "main.hpp"
static FORM *partEntryForm;


static WINDOW *winFormArea;
using namespace std;

int PartEntry(WINDOW *winMsgArea, WINDOW *winFullScreen)
{

    short formCurrentField;
    bool firstTime=true,quit=false;
    int key=0, ch=0;
    bool EditingRow=false;
    bool NewRow=false;
    bool addOrder=false;
    static FIELD *partEntryFields[10];
    static FIELD*tempFptr;

PartEntryStart:

    quit=false;
    PartEntrySetupForm(partEntryFields,winFullScreen);
    post_form(partEntryForm);
    set_current_field(partEntryForm,partEntryFields[PE_PARTID]);

    wattron(winFullScreen,A_REVERSE);
    mvwprintw(winFullScreen,22, 2, "F2 Lookup");
    mvwprintw(winFullScreen,22, 12, "F3 Delete");
    mvwprintw(winFullScreen,22, 22, "F4 Save");
    mvwprintw(winFullScreen,22, 32, "F5 New");
    mvwprintw(winFullScreen,22, 42, "F7 Exit");
    wattroff(winFullScreen,A_REVERSE);
    box(winFormArea, 0, 0);
    move(2,11);
    wrefresh(winFormArea);
    wrefresh(winFullScreen);
    refresh();

    if (firstTime==true)
    {
        firstTime=false;
        PartEntryFormCleanup(partEntryFields);
        goto PartEntryStart;
    }


    while (quit!=true)
    {

        tempFptr = current_field(partEntryForm);
        formCurrentField = field_index(tempFptr);
        ch=getch();
        switch (ch)
        {

        case KEY_F(2): // lookup
            if (formCurrentField==PE_PARTID)
            {
                def_prog_mode();
                endwin();
                PartEntryDisplayLookup(partEntryFields);
                refresh();
                pos_form_cursor(partEntryForm);

            }
            break;
        case  KEY_F(3)://delete
            if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Delete Part",COLOR_PAIR(1),KEY_F(3))==true)

            {
                key=atoi(TrimWhitespaces(field_buffer(partEntryFields[PE_PARTKEY], 0)));

                DeletePart(partEntryFields,key);
                goto PartEntryStart;
            }
            pos_form_cursor(partEntryForm);
            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            break;

        case KEY_F(4):// save

            if (NewRow==true)

                InsertPart(partEntryFields);
            else
                key=atoi(TrimWhitespaces(field_buffer(partEntryFields[PE_PARTKEY], 0)));

            UpdatePart(partEntryFields,key);
            goto PartEntryStart;
            break;

        case KEY_F(5): //reset

            PartEntryFormCleanup(partEntryFields);
            goto PartEntryStart;
            break;

        case KEY_F(7): //exit
            if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Exit",COLOR_PAIR(1),KEY_F(7))==true)
            {
                quit=true;
            }

            pos_form_cursor(partEntryForm);
            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            break;
        case KEY_ENTER:
        case 9: // TAB
        case 10:
        case 13:
            if(formCurrentField==PE_PARTID)
            {

                //      sync buffer
                form_driver(partEntryForm, REQ_NEXT_FIELD);
                form_driver(partEntryForm, REQ_PREV_FIELD);
                if (PartEntryVerify(partEntryFields,winFullScreen,winMsgArea,EditingRow,NewRow)==true)
                {

                    form_driver(partEntryForm, REQ_NEXT_FIELD);
                    form_driver(partEntryForm, REQ_BEG_LINE);

                }
                else
                {

                    //set_current_field(partEntryForm,partEntryFields[PE_PARTID]);
                    pos_form_cursor(partEntryForm);

                }
            }
            else
            {

                form_driver(partEntryForm, REQ_NEXT_FIELD);
                form_driver(partEntryForm, REQ_BEG_LINE);

            }
            break;

        case KEY_UP:

            if(formCurrentField==PE_PARTID)
            {
                //      sync buffer
                form_driver(partEntryForm, REQ_NEXT_FIELD);
                form_driver(partEntryForm, REQ_PREV_FIELD);
                if (PartEntryVerify(partEntryFields,winFullScreen,winMsgArea,EditingRow,NewRow)==true)
                {
                    form_driver(partEntryForm, REQ_PREV_FIELD);
                    form_driver(partEntryForm, REQ_BEG_LINE);
                    break;
                }
                else
                {
                    set_current_field(partEntryForm,partEntryFields[PE_PARTID]);
                    break;
                }
            }
            else
            {
                form_driver(partEntryForm, REQ_PREV_FIELD);
                form_driver(partEntryForm, REQ_BEG_LINE);
            }

            break;


        case KEY_LEFT:
            form_driver(partEntryForm, REQ_PREV_CHAR);
            break;

        case KEY_RIGHT:
            form_driver(partEntryForm, REQ_NEXT_CHAR);
            break;

        // Delete the char before cursor
        case KEY_BACKSPACE:
        case 127:
            form_driver(partEntryForm, REQ_DEL_PREV);
            break;

        // Delete the char under the cursor
        case KEY_DC:
            form_driver(partEntryForm, REQ_DEL_CHAR);
            break;
        case KEY_END:
            form_driver(partEntryForm, REQ_END_FIELD);
            break;
        case KEY_HOME:
            form_driver(partEntryForm, REQ_BEG_FIELD);
            break;

        default:

            form_driver(partEntryForm, ch);
            break;
        }
        wrefresh(winFormArea);

    }
    PartEntryFormCleanup(partEntryFields);
    return 0;

}


bool  PartEntryVerify(FIELD *partEntryFields[],WINDOW *winFullScreen, WINDOW *winMsgArea,bool &EditingRow, bool &NewRow)
{
//PE_PARTID = 1, PE_NAME=3,COST=5,PE_PRICE=7
    std::vector<part>::iterator partit;
    std::vector<part> partsResults;

    char NumToCharPointer[10];
    short formCurrentField;
    bool Result=false;
    static FIELD*tempFptr;
    std::string lookup;

    tempFptr = current_field(partEntryForm);
    formCurrentField = field_index(tempFptr);


    switch (formCurrentField)
    {

    // This code will not be executed for any field that has a lookup unless the user enters the value without using the F2 key.
    // put additional verify logic in the lookup procedure
    case PE_PARTID:

        lookup=TrimWhitespaces(field_buffer(partEntryFields[PE_PARTID], 0));

        if (GetPart(partsResults,lookup)==true)
        {
            EditingRow=true;
            NewRow=false;
            partit = partsResults.begin();
            SetPartEntryFields(partEntryFields,partit,EditingRow);
            Result=true;
        }
        else
        {
            EditingRow=false;
            NewRow=true;
            Result=false;

            if (askQuestion(winMsgArea,2,2,78,"Part not on file, do you want to add it ?",COLOR_PAIR(1)) =="Yes")
            {
                Result=true;
                NewRow=true;
                set_current_field(partEntryForm,partEntryFields[PE_PARTID]);

            }
            else
            {
                Result=false;
                NewRow=false;
                set_current_field(partEntryForm,partEntryFields[PE_PARTID]);
            }

            touchwin(winFullScreen);
            wrefresh(winFormArea);
            wrefresh(winFullScreen);
            pos_form_cursor(partEntryForm);
            refresh();
            break;
        }
        break;
    case PE_NAME:
        break;
    case PE_COST:
        break;

    case PE_PRICE:
        break;
    default:
        break;
    }

    return Result;
}

int PartEntryDisplayLookup(FIELD *partEntryFields[])
{
    ITEM **myItems;
    int c,r;

    MENU *myLookUpMenu;
    WINDOW *myLookUpMenuWin;
    int NumChoices=0,retValue=0;
    std::vector<part> partsResults;
    std::vector<part>::iterator partit;
    std::vector <int*> intptrs;
    char NumToCharPointer[10];
    bool exitMenu = false;
    bool EditingRow= false;
    bool NewRow= false;
    static FIELD*tempFptr;
//    /* Initialize curses */
//    initscr();
//    start_color();
//    cbreak();
//    noecho();
//    keypad(stdscr, TRUE);


    short formCurrentField;
    tempFptr = current_field(partEntryForm);
    formCurrentField = field_index(tempFptr);

    NumChoices=GetAllParts(partsResults);

    myItems = (ITEM **)calloc(NumChoices+1, sizeof(ITEM *));

    r=0;
    for (partit = partsResults.begin(); partit != partsResults.end(); ++partit)
    {

        myItems[r] = new_item(partit->PartId.c_str(),partit->Name.c_str());

        intptrs.push_back(&partit->idtblPart);

        set_item_userptr(myItems[r], (int *)*intptrs.back());
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
        int winSize = max(NumChoices,5);
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

        int result=0;

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
                SetPartEntryFields(partEntryFields,partit,EditingRow); // partit is not  used have to pass something in this case
                break; // test exit without choice
            case KEY_ENTER:
            case 10:
            case 13:

                ITEM *cur;
                cur = current_item(myLookUpMenu);
                exitMenu=true;
                retValue = (int)item_userptr(cur);
                EditingRow=false;
                NewRow=true;

                for (partit = partsResults.begin(); partit != partsResults.end(); ++partit)
                {
                    if (partit->idtblPart==retValue)
                    {
                        EditingRow=true;
                        NewRow=false;
                        SetPartEntryFields(partEntryFields,partit,EditingRow);
                        break;

                    }

                }
                break;
            } //switch
            box(myLookUpMenuWin, 0, 0);
            wrefresh(winFormArea);
            wrefresh(myLookUpMenuWin);

        } // while

    }// if (NumChoices!=0)


    unpost_menu(myLookUpMenu);
    free_menu(myLookUpMenu);
    for(int i = 0; i < NumChoices; ++i)
        free_item(myItems[i]);
    endwin();

    partsResults.clear();
    intptrs.clear();



    return retValue;// not used yet

}


void  SetPartEntryFields(FIELD *partEntryFields[],std::vector<part>::iterator partit,bool EditingRow)
{
    char NumToCharPointer[10];
    //PE_PARTID = 1, PE_NAME=3,PE_COST=5,PE_PRICE=7
    if (EditingRow==true)
    {

        set_field_buffer(partEntryFields[PE_PARTID], 0,  partit->PartId.c_str());
        set_field_buffer(partEntryFields[PE_NAME], 0,  partit->Name.c_str());
        sprintf(NumToCharPointer, "%4.2f", partit->Cost);
        set_field_buffer(partEntryFields[PE_COST], 0, NumToCharPointer);
        sprintf(NumToCharPointer, "%4.2f", partit->Price);
        set_field_buffer(partEntryFields[PE_PRICE], 0, NumToCharPointer);
        sprintf(NumToCharPointer, "%d", partit->idtblPart);
        set_field_buffer(partEntryFields[PE_PARTKEY], 0, NumToCharPointer);
    }
    else
    {
        set_field_buffer(partEntryFields[PE_PARTID], 0,  "");
        set_field_buffer(partEntryFields[PE_NAME], 0,"");
        sprintf(NumToCharPointer, "%4.2f", 0);
        set_field_buffer(partEntryFields[PE_COST], 0, NumToCharPointer);
        sprintf(NumToCharPointer, "%4.2f", 0);
        set_field_buffer(partEntryFields[PE_PRICE], 0, NumToCharPointer);
        sprintf(NumToCharPointer, "%d", 0);
        set_field_buffer(partEntryFields[PE_PARTKEY], 0, NumToCharPointer);

    }

}

void PartEntrySetupForm(FIELD *partEntryFields[],WINDOW *winFullScreen)
{
//    start_color();
//    init_pair(1,COLOR_GREEN,COLOR_BLACK);
//    wattron(stdscr,COLOR_PAIR(1));
//    initscr();
//    noecho();
//    cbreak();
//    keypad(stdscr, TRUE);

//    winFullScreen = newwin(24, 80, 0, 0);
//    assert(winFullScreen != NULL);
//    wattron(winFullScreen,COLOR_PAIR(1));
//    wrefresh(winFullScreen);

    winFormArea = derwin(winFullScreen, 20, 78, 1, 1);
    assert(winFormArea != NULL);
    wattron(winFormArea,COLOR_PAIR(1));
    wrefresh(winFormArea);

//              new_field(h,w,  tp,left, offscreen, nbuffers);

    partEntryFields[0]        = new_field(1, 10,  0, 2,0,0);// Part key label
    partEntryFields[PE_PARTKEY]=new_field(1,  6,  0, 10,0,0);//Part key field
    partEntryFields[2]         =new_field(1, 10,  1, 2,0, 0);// Part Id label
    partEntryFields[PE_PARTID] =new_field(1,  6,  1,10, 0,0);// Part Id field
    partEntryFields[4] =        new_field(1, 10,  3, 2,0, 0);// Part Name label
    partEntryFields[PE_NAME] =  new_field(1, 20,  3,10,0,0);// Part Name field
    partEntryFields[6] =        new_field(1, 10,  5,2,0,0); // Part Cost Label
    partEntryFields[PE_COST] =  new_field(1, 6,   5,10,0,0);// Part Cost field
    partEntryFields[8] =        new_field(1, 10,  7, 2,0,0);// Part Price label
    partEntryFields[PE_PRICE] = new_field(1,  6,  7, 10,0,0);//Part Price field
    partEntryFields[10] = NULL;


    assert(partEntryFields[0] != NULL && partEntryFields[1] != NULL
           && partEntryFields[2] != NULL && partEntryFields[3] != NULL
           && partEntryFields[4] != NULL && partEntryFields[5] != NULL
           && partEntryFields[6] != NULL && partEntryFields[7] != NULL
           && partEntryFields[8] != NULL && partEntryFields[9] != NULL);


    set_field_buffer(partEntryFields[0], 0, "Key");
    set_field_buffer(partEntryFields[PE_PARTKEY], 0, ""); //Key
    set_field_buffer(partEntryFields[2], 0, "Part Id");
    set_field_buffer(partEntryFields[PE_PARTID], 0, ""); //Part Id
    set_field_buffer(partEntryFields[4], 0, "Name");
    set_field_buffer(partEntryFields[PE_NAME], 0, ""); // Name
    set_field_buffer(partEntryFields[6], 0, "Cost");
    set_field_buffer(partEntryFields[PE_COST], 0, "");// Cost
    set_field_buffer(partEntryFields[8], 0, "Price");
    set_field_buffer(partEntryFields[PE_PRICE], 0, ""); //Price


    set_field_type(partEntryFields[PE_PARTKEY], TYPE_NUMERIC,0,1, 999999);// key
    set_field_type(partEntryFields[PE_PARTID], TYPE_ALNUM,0);// part id
    // documenation states by default a field will except any data input by the user so
    // dont do a set_field_type if you need to have a text field with spaces.
    //set_field_type(partEntryFields[PE_NAME], TYPE_ALNUM,0); // name
    set_field_type(partEntryFields[PE_COST], TYPE_NUMERIC,2,1, 99999);// cost
    set_field_type(partEntryFields[PE_PRICE], TYPE_NUMERIC,2,1, 99999);// price

    set_field_back(partEntryFields[PE_PARTKEY], A_UNDERLINE|COLOR_PAIR(1));//KEY
    set_field_back(partEntryFields[PE_PARTID], A_UNDERLINE|COLOR_PAIR(1));//part id
    set_field_back(partEntryFields[PE_NAME], A_UNDERLINE|COLOR_PAIR(1));//name
    set_field_back(partEntryFields[PE_COST], A_UNDERLINE|COLOR_PAIR(1));// cost
    set_field_back(partEntryFields[PE_PRICE], A_UNDERLINE|COLOR_PAIR(1));//price

    for (int i=0; i<=9; ++i)
        set_field_fore(partEntryFields[i], COLOR_PAIR(1));

    set_field_opts(partEntryFields[0], O_PUBLIC | O_AUTOSKIP);
    set_field_opts(partEntryFields[PE_PARTKEY],  O_PUBLIC |O_AUTOSKIP); //key
    set_field_opts(partEntryFields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(partEntryFields[PE_PARTID], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK);// part id
    set_field_opts(partEntryFields[4], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(partEntryFields[PE_NAME], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_RIGHT|O_NULLOK);// name
    set_field_opts(partEntryFields[6], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(partEntryFields[PE_COST], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_RIGHT|O_NULLOK); //cost
    set_field_opts(partEntryFields[8], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
    set_field_opts(partEntryFields[PE_PRICE], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE|O_STATIC|O_BLANK|JUSTIFY_LEFT|O_NULLOK); //price


    partEntryForm = new_form(partEntryFields);
    assert(partEntryForm != NULL);
    set_form_win(partEntryForm,winFullScreen);
    set_form_sub(partEntryForm,winFormArea);
}


void  PartEntryFormCleanup(FIELD *partEntryFields[])
{
    unpost_form(partEntryForm);
    free_form(partEntryForm);
    for (int i=0; i<9; ++i)
    {
        free_field(partEntryFields[i]);
    }
    delwin(winFormArea);

    endwin();
}

int InsertPart(FIELD *partEntryFields[])
{
    string sql;
    char NumToCharPointer[10];

    mysqlpp::Connection con;
    mysqlpp::sql_int identity;

    try
    {

        con.connect("mydb","localHost", "root", "moonpie");
        part row;

        row.PartId = TrimWhitespaces(field_buffer(partEntryFields[PE_PARTID], 0));
        row.Name =TrimWhitespaces(field_buffer(partEntryFields[PE_NAME], 0));
        row.Price = atof(TrimWhitespaces(field_buffer(partEntryFields[PE_PRICE], 0)));
        row.Cost = atof(TrimWhitespaces(field_buffer(partEntryFields[PE_COST], 0)));

        mysqlpp::Query query = con.query();

        sql = "INSERT INTO mydb.tblPart ";
        sql.append ("(PartId, Name, Price, Cost) VALUES ( \"");
        sql.append (row.PartId);
        sql.append("\""",\"");
        sql.append (row.Name);
        sql.append("\""",\"");
        sprintf(NumToCharPointer, "%4.2f", row.Price);
        sql.append(NumToCharPointer);
        sql.append("\""",\"");
        sprintf(NumToCharPointer, "%4.2f", row.Cost);
        sql.append(NumToCharPointer);
        sql.append("\""")");


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

int UpdatePart(FIELD *partEntryFields[],int key)
{
    string sql;
    char NumToCharPointer[10];

    mysqlpp::Connection con;
    mysqlpp::sql_int identity;

    try
    {

        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();


        sql = "UPDATE mydb.tblPart SET ";
        sql.append ("PartId= \"");
        sql.append (TrimWhitespaces(field_buffer(partEntryFields[PE_PARTID], 0)));

        sql.append("\""" , Name= \"");
        sql.append (TrimWhitespaces(field_buffer(partEntryFields[PE_NAME], 0)));
        sql.append("\""" , Price= \"");
        //sql.append (sPrice);
        sql.append(TrimWhitespaces(field_buffer(partEntryFields[PE_PRICE], 0)));
        sql.append("\""" , Cost= \"");
        //sql.append (sCost);
        sql.append(TrimWhitespaces(field_buffer(partEntryFields[PE_COST], 0)));
        sql.append("\""" WHERE idtblPart = ");
        sprintf(NumToCharPointer, "%d", key);
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

int DeletePart(FIELD *partEntryFields[],int key)
{
    string sql;
    char NumToCharPointer[10];
    mysqlpp::Connection con;
    sprintf(NumToCharPointer, "%d", key);


    try
    {

        con.connect("mydb","localHost", "root", "moonpie");
        mysqlpp::Query query = con.query();

        sql = "DELETE FROM tblPart WHERE idtblPart= ";
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
