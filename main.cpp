#define MYSQLPP_SSQLS_NO_STATICS
#include "main.hpp"


// after building this code blocks project
// run executable with this command
// code/ncurses/OrderEntry/bin/Debug/OrderEntry

/************************************Order Entry Application*********************************
By John Grillot
start 04/29/2015
complete ??

This application was written in C/C++ on Ubuntu 14.04.1
 gcc --version
gcc (Ubuntu 4.8.2-19ubuntu1) 4.8.2

This application requires the following

NCURSES forms and menus library see
    http://invisible-island.net/ncurses/ncurses-intro.html or
    http://www.gnu.org/software/ncurses/
MySQL database and mysql++ see
    http://tangentsoft.net/mysql++/
    http://tangentsoft.net/mysql++/doc/html/userman/index.html#history

the test database scripts are provided in this package
************************************************************************************************/


//static FORM *custEntryForm,*partEntryForm,*orderEntryForm, *orderEntryEditLinesForm;
//static FIELD *orderEntryFields[19],*partEntryFields[10],*custEntryFields[14],*orderEntryEditLinesFields[11],*tempFptr,*partFptr;

static WINDOW *winFullScreen,*winMsgArea;//,*winEditLinesFullScreen,*winEditLinesFormArea,*winFormArea;


using namespace std;

int main()
{
//system("Pause");
    char *choicenames[] =
    {
        "Customer Maintenance",
        "Parts Maintenance",
        "Enter Orders",
        "Print Invoice",
        "Exit",
        (char *)NULL,
    };

    char *choicenumbers[] =
    {
        "1",
        "2",
        "3",
        "4",
        (char *)NULL,
    };

start:

    int MenuNumber,NumChoices,c,i;
    ITEM *cur;
    ITEM **myMenuItems;
    MENU *myMainMenu;
    WINDOW *myMainMenuWin;



    /* Initialize curses */
    initscr();
    start_color();
    init_pair(1,COLOR_GREEN,COLOR_BLACK);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    wattron(stdscr,COLOR_PAIR(1));
    winFullScreen = newwin(24, 80, 0, 0);

    assert(winFullScreen != NULL);
    wattron(winFullScreen,COLOR_PAIR(1));
    wrefresh(winFullScreen);

    winMsgArea = newwin(4, 78, 19, 1);

    assert(winMsgArea != NULL);
    wattron(winMsgArea,COLOR_PAIR(1));

    wrefresh(winMsgArea);
    refresh();

    /* Create menu items */
    NumChoices = ARRAY_SIZE(choicenames);
    myMenuItems = (ITEM **)calloc(NumChoices+1, sizeof(ITEM *));
    for(i = 0; i < NumChoices; ++i)
        myMenuItems[i] = new_item(choicenumbers[i], choicenames[i]);

    myMainMenu = new_menu((ITEM **)myMenuItems);
    set_menu_fore(myMainMenu, COLOR_PAIR(1) | A_REVERSE);
    set_menu_back(myMainMenu, COLOR_PAIR(1));

    /* Create the window to be associated with the menu */
    myMainMenuWin = newwin(10, 40, 6, 20);
    wattron(myMainMenuWin,COLOR_PAIR(1)); //| A_REVERSE);
    keypad(myMainMenuWin, TRUE);


    /* Set main window and sub window */
    set_menu_win(myMainMenu, myMainMenuWin);
    set_menu_sub(myMainMenu, derwin(myMainMenuWin,6, 30, 3, 1));


    set_menu_mark(myMainMenu, " * ");
    /* Print a border around the main window and print a title */
    box(myMainMenuWin, 0, 0);
    PrintInMiddle(myMainMenuWin, 1, 0, 40, "Order Application", COLOR_PAIR(1));
    wattron(myMainMenuWin,COLOR_PAIR(1)); //| A_REVERSE);
    mvwaddch(myMainMenuWin, 2, 0, ACS_LTEE);
    mvwhline(myMainMenuWin, 2, 1, ACS_HLINE, 38);
    mvwaddch(myMainMenuWin, 2, 39, ACS_RTEE);

    refresh();

    post_menu(myMainMenu);
    wrefresh(myMainMenuWin);

    while((c = wgetch(myMainMenuWin)) != KEY_F(7))
    {
        switch(c)
        {

        case KEY_DOWN:
            menu_driver(myMainMenu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(myMainMenu, REQ_UP_ITEM);
            break;

        case KEY_ENTER:
        case 10:
        case 13:
            cur = current_item(myMainMenu);
            MenuNumber = atoi(item_name(cur));


            switch (MenuNumber)
            {
            case 1: // "Customer Maintenance":
                CustEntry(winMsgArea,winFullScreen);
                goto start;
                break;
            case 2: //"Parts Maintenance":
                PartEntry(winMsgArea,winFullScreen);
                goto start;
                break;
            case 3: //"Enter Orders":
                CleanupMenu(myMainMenu,myMenuItems,NumChoices);
                OrderEntry(winMsgArea,winFullScreen);
                goto start;
                break;
            case 4: //"PrintInvoice":
                PrintInvoice(winMsgArea,winFullScreen);
                goto start;


            case 5: //"Exit":
            default:
                break;
            }


        }
    }
    if (confirmAction(winMsgArea,winFullScreen,2,2,78,"Exit",COLOR_PAIR(1),KEY_F(7))==true)
        goto done;
    else
        goto start;
done:
    CleanupMenu(myMainMenu,myMenuItems,NumChoices);
    endwin();
    return 0;

}

void CleanupMenu(MENU *myMainMenu, ITEM **myMenuItems, int NumChoices)
{
    unpost_menu(myMainMenu);
    free_menu(myMainMenu);
    for(int i = 0; i < NumChoices; ++i)
        free_item(myMenuItems[i]);
    delwin(winFullScreen);

}


