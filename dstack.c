#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "menuio.h"
#include "stdlen.h"
#include "dstructure.h"
#include "dstack.h"
#include "dwindow.h"

/* 
 * display:/Users/linzhien/Desktop/dslab/dstack.c
 *                |
 *           base |[1][2][3] <- top
 *                |
 * 
 * current stack: number
 * input:
 * ----
 * command: list number
 */

INITIAL_DTYPE_SET_UP(stack)

struct metadata* stack_menu(struct metadata *dlist){
    print_title("stack menu");
    menu_print_main_stack();
    CHECK_FIRST_TIME_GET_IN(stack, 0);
    while(CONTINUE){
        scan_list_print_format(input_buffer, "stack>");
        if(compare_and_choose(2, input_buffer, "c", "create")){
            scan_list_print_format(input_buffer, "name:");
            dstack = ditem_create(dstack, input_buffer);
            *current_node = dnode_next_search(dstack->head, input_buffer);
            stack(*current_node);
            print_title("stack menu");
            menu_print_main_stack();
        }
        else if(compare_and_choose(1, input_buffer, "detail")){
            graphic_print_dtype(dstack);
            scan_list_print_format(input_buffer, "search name:");
            *current_node = dnode_next_search(dstack->head, input_buffer);
            if(*current_node != NULL){
                graphic_print_stack(*current_node);
                stack(*current_node);
                print_title("stack menu");
                menu_print_main_stack();
            }
            else { graphic_refresh(); print_graphic(1, "not exist\n");}
        }
        else if(compare_and_choose(1, input_buffer, "delete")){
            graphic_print_dtype(dstack);
            scan_list_print_format(input_buffer, "search name:");
            dstack->head = ditem_delete(dstack->head, &trash, input_buffer);
            graphic_print_dtype(dstack);
        }
        MENU_ENDIF;
    }
    return dlist;
}

void stack(struct data_node *head){
    print_title(head->buffer);
    menu_print_stack();
    dtype_ditem_detail("stack");
    HEAD_INPUT_NAME_SET(head->buffer);
    while(CONTINUE){
        scan_list_print_format(input_buffer, "%s", input_name);
        if(compare_and_choose(1, input_buffer, "push")){
            scan_list_print_format(input_buffer, "push:");
            head = stack_push(head, input_buffer);
        }
        else if(compare_and_choose(1, input_buffer, "pop")){
            head = stack_pop(head);
        }
        else if(compare_and_choose(1, input_buffer, "detail")){
            scan_list_print_format(input_buffer, "node or trash:");
            if(compare_and_choose(2, input_buffer, "n", "node")){
                scan_list_print_format(input_buffer, "node detail:");
                detail = detail_item_search(detail, head->buffer, dnode_link_buffer_search(head->link, input_buffer), IS_NODE);
            }
            else if(compare_and_choose(2, input_buffer, "t", "trash")){
                scan_list_print_format(input_buffer, "trash detail:");
                if(recently_trash)
                    detail = detail_item_search(detail, head->buffer, dnode_link_buffer_search(recently_trash->head, input_buffer), IS_TRASH);
            }
        }
        else if(compare_and_choose(1, input_buffer, "clean")){
            trash_clean(&recently_trash, &recently_detail);
        }
        else if(MENU_LIST_CHOOSE_REFRESH){
            graphic_refresh();
        }
        else if(MENU_LIST_CHOOSE_EXIT){
            break;
        }
        graphic_print_stack(head);
        dtype_ditem_detail("stack");
    }
    graphic_refresh();
    list_refresh();
}

struct data_node* stack_push(struct data_node *head, char *buffer){
    data_node_append(head->link, buffer, ++head->number, NULL, head->link, "stack push");
    return head;
}

struct data_node* stack_pop(struct data_node *head){
    if(head->link == NULL) return head;
    struct data_node* tem = head->link;
    head->link = tem->link;
    trash = ditem_trash_add(head->buffer, tem, trash);
    head->number--;
    return head;
}

void stack_ditem_detail(struct data_node *head){
    if(recently_trash == NULL || strncmp(recently_trash->name, head->buffer, input_buffer_size) != 0){
        recently_trash = trash;
        while(recently_trash){
            if(strncmp(recently_trash->name, head->buffer, input_buffer_size) == 0)
                break;
            recently_trash = recently_trash->next;
        }
    }
    if(recently_detail == NULL || strncmp(recently_detail->name, head->buffer, input_buffer_size) != 0){
        recently_detail = detail;
        while(recently_detail){
            if(strncmp(recently_detail->name, head->buffer, input_buffer_size) == 0)
                break;
            recently_detail = recently_detail->next;
        }
    }
    list_refresh();
    print_std_detail("stack", head, recently_trash);
    print_item_detail(recently_detail);
    graphic_print_trash(recently_trash);
}
