#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <curses.h>

#include "menuio.h"
#include "dwindow.h"
#include "dstructure.h"
#include "dstack.h"
#include "dtree.h"
#include "dgraph.h"

int input_buffer_size = 0;
char *input_buffer = NULL;
struct metadata *dlist = NULL;

int welcome(void){
    SET_INPUT_BUFFER(input_buffer, input_buffer_size);
    menu_print_main_menu();
    while(CONTINUE){
        scan_list_print_format(input_buffer, "menu>");
        if(compare_and_choose(2, input_buffer, "h", "help")){
            print_graphic(1, "get in help menu\n");
        }
        else if (compare_and_choose(1, input_buffer, "detail")){
            print_graphic(1, "search type.\n");
        }
        else if(compare_and_choose(2, input_buffer,  "s", "stack")){
            graphic_refresh();
            dlist = metadata_create(dlist, "stack");
            dlist = stack_menu(dlist);
            graphic_refresh();
            print_title("menu");
            menu_print_main_menu();
        }
        else if (compare_and_choose(1, input_buffer, "queue")){
            print_graphic(1, "get in queue menu\n");
        }
        else if (compare_and_choose(2, input_buffer, "t", "tree")){
            print_graphic(1, "get in tree menu\n");
            dlist = metadata_create(dlist, "tree");
            dlist = tree_menu(dlist);
            graphic_refresh();
            print_title("menu");
            menu_print_main_menu();
        }
        else if (compare_and_choose(2, input_buffer, "g", "graph")){
            print_graphic(1, "get in graph menu\n");
            dlist = metadata_create(dlist, "graph");
            dlist = graph_menu(dlist);
            graphic_refresh();
            print_title("menu");
            menu_print_main_menu();
        }
        else if(MENU_LIST_CHOOSE_EXIT){
            endwin();
            printf("exit\n");
            return TERMINATED;
        }
        else if(compare_and_choose(2, input_buffer, "t", "test")){
            char *ptr = "test";
            print_list_format(1, "%s\n", ptr);
        }
        else{
            refresh_display();
            print_graphic(2, "ERROR\nPlease enter again.\n");
        }
    }
    return CONTINUE;
}

int compare_and_choose(const int num, const char *input_buffer, ...){
    char *ptr = NULL;
    va_list arg;
    va_start(arg, input_buffer);
    for(int i = 0;i < num;i++){
        ptr = va_arg(arg, char *);
        if(strncmp(ptr, input_buffer, input_buffer_size) == 0){
            return CONTINUE;
        }
    }
    va_end(arg);
    return TERMINATED;
}
