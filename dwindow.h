/*
 *  Last update 2021/1/31
 *  included at main.c, menuio.c, dwindow.c, data_dtype_list.c, dstack.c, dtree.c
 *
 */

#ifndef window_h
#define window_h

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <windows.h>
    #define INITIAL_DISPLAY_SIZE_DEPEND_ON_OS \
        CONSOLE_SCREEN_BUFFER_INFO csbi;\
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);\
        display->size.col = csbi.srWindow.Right - csbi.srWindow.Left + 1;\
        display->size.row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
    #include <sys/ioctl.h>
    #define INITIAL_DISPLAY_SIZE_DEPEND_ON_OS \
        struct winsize tem;\
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &(tem));\
        display->size.row = tem.ws_row;\
        display->size.col = tem.ws_col;
    #endif
#elif __linux__
    #include <sys/ioctl.h>
    #define INITIAL_DISPLAY_SIZE_DEPEND_ON_OS \
        struct winsize tem;\
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &(tem));\
        display->size.row = tem.ws_row;\
        display->size.col = tem.ws_col;
#endif

#include <stdio.h>
#include <unistd.h>
#include <curses.h>

#include "dstructure.h"

#define TITLE_POS_ROW 1
#define TITLE_POS_COL 1

#define GRAPHIC_POS_ROW 0
#define GRAPHIC_POS_COL 0

#define GRAPHIC_TRASH_POS_ROW display->graphic_size.row-1
#define GRAPHIC_TRASH_POS_COL 1

#define LIST_POS_ROW 3
#define LIST_POS_COL 1

#define INPUT_POS_ROW 1
#define INPUT_POS_COL 1

#define SPLIT_LINE '-'
#define SPLIT_POS_ROW 2
#define SPLIT_POS_COL 1
#define SPLIT_LINE_LIST_SET do{for(int i = SPLIT_POS_COL;i < display->list_size.col-1;i++)mvwprintw(display->list, SPLIT_POS_ROW, i, "%c", SPLIT_LINE);}while(0)
#define SPLIT_LINE_MENU_SET do{for(int i = SPLIT_POS_COL;i < display->menu_size.col-1;i++)mvwprintw(display->menu, SPLIT_POS_ROW, i, "%c", SPLIT_LINE);}while(0)

#define LIST_ITEM_DETAIL_POS_ROW 3
#define LIST_ITEM_DETAIL_POS_COL 1

#define MENU_POS_ROW 3
#define MENU_POS_COL 1

#define GRAPHIC_POS_MID_COL (display->graphic_size.col/2)

struct screen_detail{
    int row;
    int col;
};

struct window_screen{
    WINDOW* title;
    WINDOW* graphic;
    WINDOW* list;
    WINDOW* menu;
    struct screen_detail size;
    struct screen_detail title_size;
    struct screen_detail graphic_size;
    struct screen_detail list_size;
    struct screen_detail menu_size;
};

//standard win_size is 140 x 48
void set_display_size(void);
void refresh_display(void);
int set_input_buffer_size(void);
int get_graphic_col_size(void);
/*
 *  title window function
 */
void print_title(char *buffer);

/*
 *  list(command) window function
 */
//only can use %ts %td %is %id
void list_input_print(const char *format, ...);
void scan_list_format(const char *title, char *buffer, ...);
void scan_list_print_format(char *buffer, const char *format, ...);
void list_input_clean(void);
void print_list(int lines, const char *format);
void list_refresh(void);
void print_list_format(int lines, const char *format, ...);
void print_list_boardcast_format(int lines, const char *format, ...);
void list_refresh_format(void);

/*
 *  menu(command) window function
 */
void print_menu_format(int lines, const char *format, ...);
void menu_refresh_format(void);

/*
 *  graphic window function
 */
void scan_graphic(char *buffer);
void print_graphic(int lines, const char *format);
void graphic_refresh(void);
void print_graphic_format(int lines, const char *format, ...);
void print_graphic_format_pos(int row, int col, const char *format, ...);
void graphic_refresh_format(void);
void graphic_print_trash(struct dnode_trash *trash);


/*
 *  common print out
 */
void print_std_detail(char *type, struct data_node *head, struct dnode_trash *trash);
void print_item_detail(struct dnode_detail *detail);
void menu_print_main_menu(void);
void graphic_print_dtype(struct metadata *dtype);
void graphic_print_dtype_detail(struct metadata *dtype);
#endif /* window_h */
