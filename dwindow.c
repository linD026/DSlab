#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <curses.h>
#include <stdarg.h>
#include <wchar.h>
#include <math.h>

#include "dwindow.h"
#include "menuio.h"
#include "stdlen.h"
#include "dstructure.h"
#include "dstack.h"
#include "dtree.h"

static struct window_screen* display = NULL;
static int graphic_current_line = GRAPHIC_POS_ROW;
static int list_current_line = LIST_POS_ROW;
static int menu_current_line = MENU_POS_ROW;
extern double max_tree_level;
extern int tree_leaf_pos_row;

/*
 *  First time setup the menu UI
 */
void set_display_size(void){
    display = (struct window_screen*)malloc(sizeof(*display));
    if(!display) {printf("create screen failed.\n"); return;}
    INITIAL_DISPLAY_SIZE_DEPEND_ON_OS
    initscr();
    
    display->title_size.row = 3;
    display->title_size.col = display->size.col;
    display->title = newwin(display->title_size.row, display->title_size.col, 0, 0);
    
    display->list_size.row = 14;
    display->list_size.col = display->size.col - 40;
    display->list = newwin(display->list_size.row, display->list_size.col, display->size.row-display->list_size.row, 0);

    display->menu_size.row = 14;
    display->menu_size.col = 40;
    display->menu = newwin(display->menu_size.row, display->menu_size.col, display->size.row-display->menu_size.row, display->list_size.col);
    
    display->graphic_size.row = display->size.row-display->title_size.row-display->list_size.row;
    display->graphic_size.col = display->size.col;
    display->graphic = newwin(display->graphic_size.row, display->graphic_size.col, 3, 0);

    input_buffer_size = set_input_buffer_size();
    
    SPLIT_LINE_LIST_SET;
    SPLIT_LINE_MENU_SET;
    box(display->title, 0, 0);
    box(display->list, 0, 0);
    box(display->menu, 0, 0);
    refresh_display();
    print_title("<menu>");
}

void refresh_display(void){
    refresh();
    wrefresh(display->title);
    wrefresh(display->list);
    wrefresh(display->menu);
    wrefresh(display->graphic);
}

/*
 *  standard function
 */

/*
 * return to input_buffer_size of int, to let char *input_buffer limited size.
 */
int set_input_buffer_size(void){
    return (int)display->list_size.col/2 - 12;
}

/*
 *  return graphic's col size.
 */
int get_graphic_col_size(void){
    return display->graphic_size.col;
}


/*
 *  title window function
 */
void print_title(char *buffer){
    char tem[display->title_size.col - 2]; int i;
    for(i = 0;i < display->title_size.col - 2;i++) tem[i] = ' ';
    tem[i] = '\0';
    mvwprintw(display->title, TITLE_POS_ROW, TITLE_POS_COL, "%s", tem);
    mvwprintw(display->title, TITLE_POS_ROW, TITLE_POS_COL, "%s", buffer);
    wrefresh(display->title);
}

/*
 *  list(command) window function
 */
void list_input_print(const char *format, ...){
    va_list arg;
    va_start(arg, format);
    mvwinch(display->list, INPUT_POS_ROW, INPUT_POS_COL);
    while(*format != '\0'){
        if(*format == '%'){
            format++;
            if(*format == 't'){
                format++;
                if(*format == 's'){
                    wprintw(display->list, "%s", va_arg(arg, char *));
                }else if(*format == 'd'){
                    wprintw(display->list, "%d", va_arg(arg, int));
                }
            }
            else if(*format == 'i'){
                wrefresh(display->list);
                format++;
                if(*format == 's'){
                    char *ptr = va_arg(arg, char *);
                    wgetnstr(display->list, ptr, input_buffer_size);
                    TRANSER_SMALL_LETTET(ptr);
                }else if(*format == 'd'){
                    int *ptr = va_arg(arg, int *);
                    wscanw(display->list, "%d", ptr);
                }
            }
        }else{
            wprintw(display->list, "%c", *format);
        }
        format++;
    }
    va_end(arg);
    list_input_clean();
}


void scan_list_format(const char *title, char *buffer, ...){
    va_list arg;
    va_start(arg, buffer);
    mvwinch(display->list, INPUT_POS_ROW, INPUT_POS_COL);
    wprintw(display->list, title);
    wrefresh(display->list);
    vw_scanw(display->list, buffer, arg);
    va_end(arg);
    list_input_clean();
}

void scan_list_print_format(char *buffer, const char *format, ...){
    va_list arg;
    va_start(arg, format);
    mvwinch(display->list, INPUT_POS_ROW, INPUT_POS_COL);
    vw_printw(display->list, format, arg);
    box(display->list, 0, 0);
    wrefresh(display->list);
    wgetnstr(display->list, buffer, input_buffer_size);
    va_end(arg);
    TRANSER_SMALL_LETTET(buffer);
    list_input_clean();
}

void list_input_clean(void){
    char buffer[display->list_size.col - 2]; int i;
    for(i = 0;i < display->list_size.col - 2;i++) buffer[i] = ' ';
    buffer[i] = '\0';
    mvwprintw(display->list, INPUT_POS_ROW, INPUT_POS_COL, "%s", buffer);
    box(display->list, 0, 0);
    wrefresh(display->list);
}

void print_list(int lines, const char *format){
    if(list_current_line + lines >= display->list_size.row)
        list_refresh();
    mvwprintw(display->list, list_current_line, LIST_POS_COL, "%s", format);
    list_current_line+=lines;
    box(display->list, 0, 0);
    wrefresh(display->list);
}

void list_refresh(void){
    char buffer[display->list_size.col - 2]; int i;
    for(i = 0;i < display->list_size.col - 2;i++) buffer[i] = ' ';
    buffer[i] = '\0';
    for(i = LIST_POS_ROW;i < display->list_size.row;i++)
        mvwprintw(display->list, i, LIST_POS_COL, "%s", buffer);
    list_current_line = LIST_POS_ROW;
    SPLIT_LINE_LIST_SET;
    box(display->list, 0, 0);
    wrefresh(display->list);
}

void print_list_format(int lines, const char *format, ...){
    if(list_current_line + lines >= display->list_size.row)
        list_refresh_format();
    va_list arg;
    va_start(arg, format);
    mvwinch(display->list, list_current_line, LIST_POS_COL);
    vw_printw(display->list, format, arg);
    va_end(arg);
    list_current_line+=lines;
    box(display->list, 0, 0);
    wrefresh(display->list);
}

void list_refresh_format(void){
    char buffer[display->list_size.col - 2]; int i;
    for(i = 0;i < display->list_size.col - 2;i++) buffer[i] = ' ';
    buffer[i] = '\0';
    for(i = LIST_POS_ROW;i < display->list_size.row;i++)
        mvwprintw(display->list, i, LIST_POS_COL, "%s", buffer);
    list_current_line = LIST_POS_ROW;
    SPLIT_LINE_LIST_SET;
    box(display->list, 0, 0);
    mvwinch(display->list, LIST_POS_ROW, LIST_POS_COL);
    wrefresh(display->list);
}

void print_list_boardcast_format(int lines, const char *format, ...){
    list_refresh_format();
    va_list arg;
    va_start(arg, format);
    vw_printw(display->list, format, arg);
    va_end(arg);
    box(display->list, 0, 0);
    wrefresh(display->list);
    wgetch(display->list);
    list_refresh_format();
}

/*
 *  menu(command) window function
 */
void print_menu_format(int lines, const char *format, ...){
    if(menu_current_line + lines >= display->menu_size.row)
        menu_refresh_format();
    va_list arg;
    va_start(arg, format);
    mvwinch(display->menu, menu_current_line, MENU_POS_COL);
    vw_printw(display->menu, format, arg);
    va_end(arg);
    menu_current_line+=lines;
    box(display->menu, 0, 0);
    wrefresh(display->menu);
}

void menu_refresh_format(void){
    char buffer[display->menu_size.col - 2]; int i;
    for(i = 0;i < display->menu_size.col - 2;i++) buffer[i] = ' ';
    buffer[i] = '\0';
    for(i = MENU_POS_ROW;i < display->menu_size.row;i++)
        mvwprintw(display->menu, i, MENU_POS_COL, "%s", buffer);
    menu_current_line = MENU_POS_ROW;
    SPLIT_LINE_MENU_SET;
    box(display->menu, 0, 0);
    mvwinch(display->menu, MENU_POS_ROW, MENU_POS_COL);
    wrefresh(display->menu);
}

/*
 *  graphic window function
 */
void scan_graphic(char *buffer){
    mvwgetnstr(display->graphic, GRAPHIC_POS_ROW, GRAPHIC_POS_COL, buffer, display->graphic_size.col);
}

void print_graphic(int lines, const char *format){
    if(graphic_current_line + lines > display->graphic_size.row)
        graphic_refresh();
    mvwprintw(display->graphic, graphic_current_line, GRAPHIC_POS_COL, "%s", format);
    graphic_current_line += lines;
    wrefresh(display->graphic);
}

void print_graphic_format(int lines, const char *format, ...){
    if(graphic_current_line + lines > display->graphic_size.row)
        graphic_refresh_format();
    va_list arg;
    va_start(arg, format);
    mvwinch(display->graphic, graphic_current_line, GRAPHIC_POS_COL);
    vw_printw(display->graphic, format, arg);
    va_end(arg);
    graphic_current_line += lines;
    wrefresh(display->graphic);
}

void print_graphic_format_pos(int row, int col, const char *format, ...){
    va_list arg;
    va_start(arg, format);
    mvwinch(display->graphic, row, col);
    vw_printw(display->graphic, format, arg);
    va_end(arg);
    wrefresh(display->graphic);
}

void graphic_refresh(){
    char buffer[display->graphic_size.col]; int i;
    for(i = 0;i < display->graphic_size.col;i++) buffer[i] = ' ';
    buffer[i] = '\0';
    for(i = GRAPHIC_POS_ROW;i < display->graphic_size.row;i++)
        mvwprintw(display->graphic, i, GRAPHIC_POS_COL, "%s", buffer);
    graphic_current_line = GRAPHIC_POS_ROW;
    wrefresh(display->graphic);
}

void graphic_refresh_format(){
    char buffer[display->graphic_size.col]; int i;
    for(i = 0;i < display->graphic_size.col;i++) buffer[i] = ' ';
    buffer[i] = '\0';
    for(i = GRAPHIC_POS_ROW;i < display->graphic_size.row;i++)
        mvwprintw(display->graphic, i, GRAPHIC_POS_COL, "%s", buffer);
    graphic_current_line = GRAPHIC_POS_ROW;
    mvwinch(display->graphic, GRAPHIC_POS_ROW, GRAPHIC_POS_COL);
    wrefresh(display->graphic);
}

void graphic_print_trash(struct dnode_trash *trash){
    struct data_node *node = NULL;
    if(trash){
        node = trash->head;
    }
    int col = GRAPHIC_TRASH_POS_COL;
    mvwprintw(display->graphic, GRAPHIC_TRASH_POS_ROW, 0, "[");
    if(node){
        for(;node && col + strlen(node->buffer) + GET_DIGIT_LEN(node->number) + 1 < display->graphic_size.col - 1;col+=strlen(node->buffer)+GET_DIGIT_LEN(node->number)+3, node=node->link){
            mvwprintw(display->graphic, GRAPHIC_TRASH_POS_ROW, col, "%d %s, ", node->number, node->buffer);
        }
        if(node && col < display->graphic_size.col-1 && col + 3 < display->graphic_size.col-1) mvwprintw(display->graphic, GRAPHIC_TRASH_POS_ROW, col, "...");
    }
    mvwprintw(display->graphic, GRAPHIC_TRASH_POS_ROW, display->graphic_size.col-1, "]");
    wrefresh(display->graphic);
}

/*
 *
 *  common print out
 */

/*
 *  list(command) window function
 */
void print_std_detail(char *type, struct data_node *head, struct dnode_trash *trash){
    if(trash) mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW, LIST_ITEM_DETAIL_POS_COL, "STDINFOR> Type:%s  Name:%s  Have:%d  Trash:%d\n",  type, head->buffer, head->number, trash->trash_num);
    else mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW, LIST_ITEM_DETAIL_POS_COL, "STDINFOR> Type:%s  Name:%s  Have:%d  Trash:0\n",  type, head->buffer, head->number);
    if(head->link){
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+1, LIST_ITEM_DETAIL_POS_COL, "STDNODEINFOR:");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+2, LIST_ITEM_DETAIL_POS_COL, "NUMBER : %d", head->link->number);
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+3, LIST_ITEM_DETAIL_POS_COL, "BUFFER : %s", head->link->buffer);
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+4, LIST_ITEM_DETAIL_POS_COL, "ADDRESS: %p", head->link);
    }else{
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+1, LIST_ITEM_DETAIL_POS_COL, "STDNODEINFOR:");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+2, LIST_ITEM_DETAIL_POS_COL, "ID     : 0");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+3, LIST_ITEM_DETAIL_POS_COL, "BUFFER : NULL");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+4, LIST_ITEM_DETAIL_POS_COL, "ADDRESS: NULL");
    }
    
    if(trash && trash->head){
    mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+6, LIST_ITEM_DETAIL_POS_COL, "STDTRASHINFOR:");
    mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+7, LIST_ITEM_DETAIL_POS_COL, "NUMBER : %d", trash->head->number);
    mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+8, LIST_ITEM_DETAIL_POS_COL, "BUFFER : %s", trash->head->buffer);
    mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+9, LIST_ITEM_DETAIL_POS_COL, "ADDRESS: %p", trash->head);
    }else{
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+6, LIST_ITEM_DETAIL_POS_COL, "STDTRASHINFOR:");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+7, LIST_ITEM_DETAIL_POS_COL, "NUMBER : 0");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+8, LIST_ITEM_DETAIL_POS_COL, "BUFFER : NULL");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+9, LIST_ITEM_DETAIL_POS_COL, "ADDRESS: NULL");
    }
    wrefresh(display->list);
}

void print_item_detail(struct dnode_detail *detail){
    struct data_node *item = NULL;
    struct data_node *item_t = NULL;
    if(detail){
        item = detail->node_item;
        item_t = detail->trash_item;
    }
    int split_space_len = display->list_size.col/2;
    if(item){
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+1, LIST_ITEM_DETAIL_POS_COL + split_space_len, "NODEINFOR:");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+2, LIST_ITEM_DETAIL_POS_COL + split_space_len, "NUMBER : %d", item->number);
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+3, LIST_ITEM_DETAIL_POS_COL + split_space_len, "BUFFER : %s", item->buffer);
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+4, LIST_ITEM_DETAIL_POS_COL + split_space_len, "ADDRESS: %p", *item);
    }else{
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+1, LIST_ITEM_DETAIL_POS_COL + split_space_len, "NODEINFOR:");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+2, LIST_ITEM_DETAIL_POS_COL + split_space_len, "NUMBER : 0");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+3, LIST_ITEM_DETAIL_POS_COL + split_space_len, "BUFFER : NULL");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+4, LIST_ITEM_DETAIL_POS_COL + split_space_len, "ADDRESS: NULL");
    }
    if(item_t){
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+6, LIST_ITEM_DETAIL_POS_COL + split_space_len, "TRASHINFOR:");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+7, LIST_ITEM_DETAIL_POS_COL + split_space_len, "NUMBER : %d", item_t->number);
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+8, LIST_ITEM_DETAIL_POS_COL + split_space_len, "BUFFER : %s", item_t->buffer);
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+9, LIST_ITEM_DETAIL_POS_COL + split_space_len, "ADDRESS: %p", *item_t);
        wrefresh(display->list);
    }else{
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+6, LIST_ITEM_DETAIL_POS_COL + split_space_len, "TRASHINFOR:");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+7, LIST_ITEM_DETAIL_POS_COL + split_space_len, "NUMBER : 0");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+8, LIST_ITEM_DETAIL_POS_COL + split_space_len, "BUFFER : NULL");
        mvwprintw(display->list, LIST_ITEM_DETAIL_POS_ROW+9, LIST_ITEM_DETAIL_POS_COL + split_space_len, "ADDRESS: NULL");
    }
}

/*
 *  menu(commad) window function
 */
void menu_print_main_menu(void){
    menu_refresh_format();
    mvwprintw(display->menu, 1, 1, "command\n");
    print_menu_format(1, " > help\n");
    print_menu_format(1, " > detail\n");
    print_menu_format(1, " > stack\n");
    print_menu_format(1, " > queue\n");
    print_menu_format(1, " > tree\n");
    print_menu_format(1, " > graph\n");
    print_menu_format(1, " > exit(quit)\n");
    wrefresh(display->menu);
}

void menu_print_main_stack(void){
    menu_refresh_format();
    print_menu_format(1, " > create\n");
    print_menu_format(1, " > detail\n");
    print_menu_format(1, " > multiple\n");
    print_menu_format(1, " > delete\n");
    wrefresh(display->menu);
}

void menu_print_stack(void){
    menu_refresh_format();
    print_menu_format(1, " > push\n");
    print_menu_format(1, " > pop\n");
    print_menu_format(1, " > detail\n");
    print_menu_format(1, "    - node\n");
    print_menu_format(1, "    - trash\n");
    print_menu_format(1, " > clean\n");
    wrefresh(display->menu);
}

void menu_print_main_tree(void){
    menu_refresh_format();
    print_menu_format(1, " > create\n");
    print_menu_format(1, "    - normal\n");
    print_menu_format(1, "    - max/min heap\n");
    print_menu_format(1, "    - binary search tree(BST)\n");
    print_menu_format(1, " > detail\n");
    print_menu_format(1, " > delete\n");
    wrefresh(display->menu);
}
void menu_print_normal_tree(void){
    menu_refresh_format();
    print_menu_format(1, " > print symbol mode\n");
    print_menu_format(1, "    - number or integer buffer\n");
    print_menu_format(1, " > insert\n");
    print_menu_format(1, " > replace\n");
    print_menu_format(1, " > delete\n");
    print_menu_format(1, " > detail\n");
    print_menu_format(1, "    - node\n");
    print_menu_format(1, "    - trash\n");
    print_menu_format(1, " > clean\n");
    wrefresh(display->menu);
}

void menu_print_max_heap(void){
    menu_refresh_format();
    print_menu_format(1, " > print symbol mode\n");
    print_menu_format(1, "    - number or integer buffer\n");
    print_menu_format(1, " > push\n");
    print_menu_format(1, " > pop\n");
    print_menu_format(1, " > detail\n");
    print_menu_format(1, "    - node\n");
    print_menu_format(1, "    - trash\n");
    print_menu_format(1, " > clean\n");
    wrefresh(display->menu);
}

/*
 *  graphic window function
 */

/*
 *  graphic window stack function
 */
void graphic_print_dtype(struct metadata *dtype){
    graphic_refresh();
    struct data_node* ptr = dtype->head;
    int row = 0;
    int col = 0;
    mvwprintw(display->graphic, row++, col, "%s", dtype->data_type);
    for(;ptr;ptr=ptr->next){
        mvwprintw(display->graphic, row++, col, "%s", ptr->buffer);
    }
    wrefresh(display->graphic);
}

void graphic_print_stack(struct data_node *head){
    graphic_refresh();
    struct data_node* ptr = head;
    int row = 4;
    int col = 0;
    for(;ptr;ptr=ptr->link){
        mvwprintw(display->graphic, 2, 0, "row:%d", row);
        mvwprintw(display->graphic, 3, 0, "col:%d", col);
        mvwprintw(display->graphic, row, col, "[%s]", ptr->buffer);
        col+=strlen(ptr->buffer)+2;
    }
    wrefresh(display->graphic);
}

/*
 *  graphic window tree function
 */

//can be common function, now using at tree.
void graphic_print_dtype_detail(struct metadata *dtype){
    graphic_refresh();
    struct data_node* ditem = dtype->head;
    struct data_node* head = NULL;
    int row = 0;
    int col = 0;
    mvwprintw(display->graphic, row++, col, "%s", dtype->data_type);
    for(;ditem;ditem = ditem->next, row++, col = 0){
        print_graphic_format_pos(row, col, "%d. %-10s:", tree_type_word_to_num(ditem->buffer),ditem->buffer);
        //col += (int)strlen(ditem->buffer) + 5;
        col += 15;
        for(head = ditem->link;head;head = head->next){
            mvwprintw(display->graphic, row, col, "'%s', ", head->buffer);
            col += (int)strlen(head->buffer) + 4;
        }
    }
    wrefresh(display->graphic);
}

void graphic_print_tree_type(void){
    graphic_refresh();
    print_graphic_format(1, "tree\n");
    print_graphic_format(1, "1. normal\n");
    print_graphic_format(1, "2. max heap\n");
    print_graphic_format(1, "3. min heap\n");
    print_graphic_format(1, "4. BST\n");
    wrefresh(display->graphic);
}

void graphic_print_tree(struct data_node *head, int print_mod, int func_mod,
                        void print_func(int print_mod, const int base_width,int node_pos_row, int node_pos_col, int current_line_len, struct data_node *node,  int node_num)){
    graphic_refresh();
    if(head->node[1] == NULL){
        print_graphic_format_pos(GRAPHIC_POS_ROW, GRAPHIC_POS_MID_COL, "*1*");
        return;
    }
    int base_width = 0;
    int i = 1;
    /*
     * using print_mod and act_mod to seperate.
     * find out max number len.
     */
    if(print_mod == TREE_PRINT_MOD_BUF){
        for(/*i = 1*/;i < head->number;i++) if(head->node[i] && base_width < GET_DIGIT_LEN((int)head->node[i]->number)) base_width = GET_DIGIT_LEN((int)head->node[i]->number);
        if(func_mod == TREE_ACT_MODE_INSERT){
            int tem;
            tem = i * 2 + 1;
            if(tem <= head->number && GET_DIGIT_LEN(tem) > base_width)
                base_width = GET_DIGIT_LEN(tem);
        }
    }else {
        for(/*i = 1*/;i <= head->number;i++) if(head->node[i]) base_width = i;
        if(func_mod == TREE_ACT_MODE_INSERT){
            int tem;
            tem = base_width * 2 + 1;
            if(tem <= head->number)
                base_width = GET_DIGIT_LEN(tem);
            else base_width = GET_DIGIT_LEN(base_width);
        }
        else base_width = GET_DIGIT_LEN(base_width);
    }
    //the length of line that connect the parent and child.
    int current_line_len = (head->number/4 - 3/4) * base_width;
    int node_pos_row = 0;
    int node_pos_col = GRAPHIC_POS_MID_COL;
    print_func(print_mod, base_width, node_pos_row, node_pos_col, current_line_len, head->node[1], 1);
    wrefresh(display->graphic);
}

void graphic_print_tree_node(int print_mod, const int base_width, int node_pos_row, int node_pos_col, int current_line_len, struct data_node *node, int node_num){
    if(node){
        if(print_mod == TREE_PRINT_MOD_BUF){
            mvwinch(display->graphic, node_pos_row, node_pos_col - GET_DIGIT_LEN((int)node->number) + 1);
            for(int i = 0;i < base_width - GET_DIGIT_LEN((int)node->number);i++) wprintw(display->graphic, "%c", ' ');
            wprintw(display->graphic, "%d", node->number);
        }else{
            mvwinch(display->graphic, node_pos_row, node_pos_col - GET_DIGIT_LEN((int)node_num) + 1);
            for(int i = 0;i < base_width - GET_DIGIT_LEN((int)node_num);i++) wprintw(display->graphic, "%c", ' ');
            wprintw(display->graphic, "%d", node_num);
        }
        if(node->leftC){
            graphic_print_tree_line(current_line_len, node_pos_row + 1, node_pos_col, node, tree_node_left);
            graphic_print_tree_node(print_mod, base_width, node_pos_row+2, node_pos_col - current_line_len - base_width, (current_line_len - base_width)/2, node->leftC, node_num * 2);
        }
        if(node->rightC){
            graphic_print_tree_line(current_line_len, node_pos_row + 1, node_pos_col, node, tree_node_right);
            graphic_print_tree_node(print_mod, base_width, node_pos_row+2, node_pos_col + current_line_len + base_width, (current_line_len - base_width)/2, node->rightC, node_num * 2 + 1);
        }
    }
}

void graphic_print_tree_insert_node(int print_mod, const int base_width, int node_pos_row, int node_pos_col, int current_line_len, struct data_node *node, int node_num){
    if(node){
        if(print_mod == TREE_PRINT_MOD_BUF){
            mvwinch(display->graphic, node_pos_row, node_pos_col - GET_DIGIT_LEN((int)node->number) + 1);
            for(int i = 0;i < base_width - GET_DIGIT_LEN((int)node->number);i++) wprintw(display->graphic, "%c", ' ');
            wprintw(display->graphic, "%d", node->number);
        }else{
            mvwinch(display->graphic, node_pos_row, node_pos_col - GET_DIGIT_LEN((int)node_num) + 1);
            for(int i = 0;i < base_width - GET_DIGIT_LEN((int)node_num);i++) wprintw(display->graphic, "%c", ' ');
            wprintw(display->graphic, "%d", node_num);
        }
        if(node->leftC){
            graphic_print_tree_line(current_line_len, node_pos_row + 1, node_pos_col, node, tree_node_left);
            graphic_print_tree_insert_node(print_mod, base_width, node_pos_row+2, node_pos_col - current_line_len - base_width, (current_line_len - base_width)/2, node->leftC, node_num * 2);
        }else if (node_pos_row < tree_leaf_pos_row - 1){
            graphic_print_tree_line(current_line_len, node_pos_row + 1, node_pos_col, node, tree_node_left);
            mvwprintw(display->graphic, node_pos_row + 2, node_pos_col - current_line_len - base_width, "%lc", TREE_INSERT_LINE);
            mvwinch(display->graphic, node_pos_row + 3,  node_pos_col - current_line_len - base_width);
            for(int i = 0;i < base_width - GET_DIGIT_LEN((int)node_num * 2);i++) wprintw(display->graphic, "%c", ' ');
            wprintw(display->graphic, "%d", node_num * 2);
        }
        if(node->rightC){
            graphic_print_tree_line(current_line_len, node_pos_row + 1, node_pos_col, node, tree_node_right);
            graphic_print_tree_insert_node(print_mod, base_width, node_pos_row+2, node_pos_col + current_line_len + base_width, (current_line_len - base_width)/2, node->rightC, node_num * 2 + 1);
        }else if (node_pos_row < tree_leaf_pos_row - 1){
            graphic_print_tree_line(current_line_len, node_pos_row + 1, node_pos_col, node, tree_node_right);
            mvwprintw(display->graphic, node_pos_row + 2, node_pos_col + current_line_len + base_width, "%lc", TREE_INSERT_LINE);
            mvwinch(display->graphic, node_pos_row + 3,  node_pos_col + current_line_len + base_width);
            for(int i = 0;i < base_width - GET_DIGIT_LEN((int)node_num * 2 + 1);i++) wprintw(display->graphic, "%c", ' ');
            wprintw(display->graphic, "%d", node_num * 2 + 1);
        }
    }
}

void graphic_print_tree_line(int line, int start_row, int start_col, struct data_node *parent, int sight){
    int col;
    if(sight == tree_node_left){
        if(parent->rightC)
            mvwprintw(display->graphic, start_row, start_col, "%lc", TREE_RL_OUT);
        else
            mvwprintw(display->graphic, start_row, start_col, "%lc", TREE_L_OUT);
        for(col = start_col - 1;col >= start_col - line;col--)
            mvwprintw(display->graphic, start_row, col, "%lc", TREE_LINE);
        mvwprintw(display->graphic, start_row, col, "%lc", TREE_R_IN);
    }
    else{
        if(parent->leftC)
            mvwprintw(display->graphic, start_row, start_col, "%lc", TREE_RL_OUT);
        else
            mvwprintw(display->graphic, start_row, start_col, "%lc", TREE_R_OUT);
        for(col = start_col + 1;col <= start_col + line;col++)
            mvwprintw(display->graphic, start_row, col, "%lc", TREE_LINE);
        mvwprintw(display->graphic, start_row, col, "%lc", TREE_L_IN);
    }
}
