#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "menuio.h"
#include "stdlen.h"
#include "dstructure.h"
#include "dtree.h"
#include "dwindow.h"

/*
 * display:/Users/linzhien/Desktop/dslab/dtree.c
 *
 * stored type: - array to find faster
 *              - binary tree
 *
 * main manage:
 *             (dtype)   (ditem)
 *              dtree -> normal -> max heap ->...
 *                         |
 *                      dnode_1(head_1, child...)
 *                         |
 *                      dnode_2(head_2, child...)
 *
 * head->node[n]
 *
 *
 * input:
 * ----
 * command: list number
 */

INITIAL_DTYPE_SET_UP(tree)
double max_tree_level = 0;
int tree_leaf_pos_row = 0;
static int max_buff_num = 0;

int set_max_level(){
    /*
     *  compute the max level "k":
     *  because max number of node = 2^(k - 1), and we know graphic col size so we can get it.
     *  log2(graphic's col size) + 1 = k
     *  ex: log2(140/2) + 1 = k = 7
     */
    print_graphic(1, "set up max level of tree and integer buffer size.\n");
    modf(log2((double)get_graphic_col_size() / 2) + 1, &max_tree_level);
    // max node num len * max level node num > graphic col size
    while(GET_DIGIT_LEN((int)pow(2, max_tree_level) - 1) * (pow(2, max_tree_level) - 1) > get_graphic_col_size())
        max_tree_level--;
    if(max_tree_level == 0){print_graphic(1, "max level set failed\n");return 1;};
    for(int i = 0;i < GET_DIGIT_LEN((int)pow(2, max_tree_level) - 1);i++) max_buff_num+=9 * pow(10, i);
    print_graphic(1, "max level and integer buffer size set finished\n");
    return 0;
}

struct metadata* tree_menu(struct metadata *dlist){
    print_title("binary tree menu:");
    menu_print_main_tree();
    CHECK_FIRST_TIME_GET_IN(tree, set_max_level());
    while(CONTINUE){
        scan_list_print_format(input_buffer, "Binary tree>");
        if(compare_and_choose(2, input_buffer, "c", "create")){
            graphic_print_tree_type();
            scan_list_print_format(input_buffer, "select type(enter number):");
            if(tree_select_type(input_buffer)){
                dtree = ditem_create(dtree, input_buffer);
                *current_node = dnode_next_search(dtree->head, input_buffer);
                tree_get_in_type_menu((*current_node)->buffer, tree_head_setup(*current_node));
            }
            else { graphic_refresh(); print_graphic(1, "not exist\n");}
        }
        else if (compare_and_choose(1, input_buffer, "detail")){
            graphic_print_dtype_detail(dtree);
            scan_list_print_format(input_buffer, "search type:");
            if(tree_select_type(input_buffer)){
                *current_node = dnode_next_search(dtree->head, input_buffer);
                scan_list_print_format(input_buffer, "search name:");
                tree_get_in_type_menu((*current_node)->buffer, dnode_next_search((*current_node)->link, input_buffer));
                menu_print_main_tree();
            }
            else { graphic_refresh(); print_list_boardcast_format(1, "not exist.");}
        }
        else if(compare_and_choose(1, input_buffer, "delete")){
            graphic_print_dtype_detail(dtree);
            scan_list_print_format(input_buffer, "search type:");
            if(tree_select_type(input_buffer)){
                *current_node = dnode_next_search(dtree->head, input_buffer);
                scan_list_print_format(input_buffer, "search name:");
                (*current_node)->link = ditem_delete((*current_node)->link, &trash, input_buffer);
                graphic_print_dtype_detail(dtree);
            }
            else { graphic_refresh(); print_list_boardcast_format(1, "not exist.");}
        }
        MENU_ENDIF;
    }
    return dlist;
}

int tree_type_word_to_num(char *buffer){
    if(compare_and_choose(2, buffer, "n", "normal")) return 1;
    else if (compare_and_choose(2, buffer, "max", "max heap")) return 2;
    else if (compare_and_choose(2, buffer, "min", "max heap")) return 3;
    else if (compare_and_choose(2, buffer, "BST", "binary search tree")) return 4;
    else return TERMINATED;
}

int tree_select_type(char *buffer){
    if(isdigit(buffer[0])){
        switch (buffer[0] - '0') {
            case 1:
                strncpy(buffer, "normal", input_buffer_size);
                return CONTINUE;
            case 2:
                strncpy(buffer, "max heap", input_buffer_size);
                return CONTINUE;
            case 3:
                strncpy(buffer, "min heap", input_buffer_size);
                return CONTINUE;
            case 4:
                strncpy(buffer, "BST", input_buffer_size);
                return CONTINUE;
        }
    }
    print_list_boardcast_format(1, "doesn't has this type.");
    return TERMINATED;
}

struct data_node* tree_head_setup(struct data_node *ditem){
    int size_of_node = 0;
    int level;
    while(CONTINUE){
        list_input_print("set max number of level(must below %td):%id", (int)max_tree_level, &level);
        if(level <= (int)max_tree_level){
            size_of_node += (int)pow(2, level) - 1;
            break;
        }
        else{
            print_list_boardcast_format(1, "set max number of level error.");
        }
    }
    scan_list_print_format(input_buffer, "name:");
    ditem = dnode_head_create(ditem, input_buffer, size_of_node);
    ditem = dnode_next_search(ditem->link, input_buffer);       //pass head address to ditem.
    data_node_append(ditem->node[0], "head", level, NULL, NULL, "ditem->node[0]");
    tree_leaf_pos_row = ditem->node[0]->tree_level * 2 - 1;
    return ditem;
}

/*
 * head is dtree -> normal ->...
 *                    ^this one
 */
void tree_get_in_type_menu(char *type, struct data_node *ditem){
    if(compare_and_choose(1, type, "normal")){
        tree_normal(ditem);
    }
    else if (compare_and_choose(1, type, "max heap")){
        tree_max_heap(ditem);
    }
    else if (compare_and_choose(1, type, "min heap")){
        tree_min_heap(ditem);
    }
    else if (compare_and_choose(1, type, "BST")){
    }
}

void tree_detail(struct data_node *head){
    int target;
    scan_list_print_format(input_buffer, "detail for node or trash:");
    if(compare_and_choose(2, input_buffer, "n", "node")){
        while(CONTINUE){
            scan_list_format("input the node number for detail(0 to exit):", "%d", &target);
            if(target == 0)return;
            else if(target <= head->number && head->node[target]) break;
        }
        detail = detail_item_search(detail, head->buffer, head->node[target], IS_NODE);
    }
    else if(compare_and_choose(2, input_buffer, "t", "trash")){
        scan_list_format("input the number buffer of trash for detail:", "%d", &target);
        if(recently_trash)
            detail = detail_item_search(detail, head->buffer, dnode_link_number_search(recently_trash->head, target), IS_TRASH);
    }
}

void tree_normal(struct data_node *head){
    TREE_TYPE_INITIAL
    static int print_mod = TREE_PRINT_MOD_NUM;
    print_title("normal tree");
    menu_print_normal_tree();
    while(CONTINUE){
        graphic_print_tree(head, print_mod, TREE_INSERT_LINE, graphic_print_tree_node);
        dtype_ditem_detail("normal tree");
        scan_list_print_format(input_buffer, "%s", input_name);
        if(compare_and_choose(4, input_buffer, "m", "p", "print", "mode")){
            do{scan_list_format("number(1) or integer buffer(2):", "%d", &print_mod);
            }while(print_mod != TREE_PRINT_MOD_BUF && print_mod != TREE_PRINT_MOD_NUM);
        }
        else if(compare_and_choose(2, input_buffer, "i", "insert")){
            graphic_print_tree(head, print_mod, TREE_ACT_MODE_INSERT, graphic_print_tree_insert_node);
            tree_normal_insert(head);
        }
        else if(compare_and_choose(2, input_buffer, "r", "replace")){
            graphic_print_tree(head, TREE_PRINT_MOD_NUM, TREE_ACT_MODE_INSERT, graphic_print_tree_node);
            tree_normal_replace(head);
        }
        else if(compare_and_choose(1, input_buffer, "delete")){
            tree_normal_delete(head);
        }
        else if(compare_and_choose(1, input_buffer, "clean")){
            trash_clean(&recently_trash, &recently_detail);
        }
        else if(compare_and_choose(1, input_buffer, "detail")){
            tree_detail(head);
        }
        MENU_ENDIF
    }
    graphic_refresh();
    list_refresh();
}

void tree_normal_insert(struct data_node *head){
    int number;
    int buff_num;
    scan_list_format("insert at:", "%d", &number);
    int odd_or_even = number % 2;
    if(number <= head->number && (head->node[(number - odd_or_even)/2]) && (head->node[number] == NULL)){
        while(CONTINUE){
            buff_num = max_buff_num + 1;
            list_input_print("integer buffer (must below %td):%id", max_buff_num, &buff_num);
            if(buff_num <= max_buff_num) break;
            print_list_boardcast_format(1, "integer buffer input error.");
        }
        scan_list_print_format(input_buffer, "buffer:");
        if((number * 2)< head->number){
            if((number * 2) + 1  < head->number)
                 data_node_append(head->node[number], input_buffer, buff_num, head->node[number*2], head->node[(number*2)+1], "tree node insert");
            else data_node_append(head->node[number], input_buffer, buff_num, head->node[number*2], NULL, "tree node insert");
        }else{
            data_node_append(head->node[number], input_buffer, number, NULL, NULL, "tree node insert");
        }
        if(odd_or_even == 0) head->node[number/2]->leftC = head->node[number];
        else head->node[(number-1)/2]->rightC = head->node[number];
        if(number == 1) head->link = head->node[1];
    }
    else print_list_boardcast_format(1, "you cannot insert the postion that doesn't has parent.");
}

void tree_normal_replace(struct data_node *head){
    int number;
    int buff_num;
    scan_list_format("replace at:", "%d", &number);
    if(number <= head->number && head->node[number]){
        while(CONTINUE){
            buff_num = max_buff_num + 1;
            list_input_print("integer buffer (must below %td):%id", max_buff_num, &buff_num);
            if(buff_num <= max_buff_num) break;
            print_list_boardcast_format(1, "integer buffer input error.");
        }
        scan_list_print_format(input_buffer, "buffer:");
        head->node[number]->number = buff_num;
        strncpy(head->node[number]->buffer, input_buffer, input_buffer_size);
    }
    else print_list_boardcast_format(1, "you cannot replace the postion that doesn't has node.");
}

void tree_normal_delete(struct data_node *head){
    int target;
    while(CONTINUE){
        scan_list_format("delete the node number(0 to exit):", "%d", &target);
        if(target == 0)return;
        else if(target <= head->number && head->node[target] && head->node[target]->leftC == NULL && head->node[target]->rightC == NULL) break;
        print_list_boardcast_format(1, "The number you want delete doesn't exist.\n Please try again or input 0 to quit.");
    }
    trash = ditem_trash_add(head->buffer, head->node[target], trash);
    head->node[target] = NULL;
    if(target == 1)
        head->link = NULL;
    else if(target % 2 == 0)
        head->node[target/2]->leftC = NULL;
    else
        head->node[(target-1)/2]->rightC = NULL;
}

void tree_max_heap(struct data_node *head){
    TREE_TYPE_INITIAL
    static int print_mod = TREE_PRINT_MOD_BUF;
    print_title("max heap");
    menu_print_max_heap();
    while(CONTINUE){
        graphic_print_tree(head, print_mod, TREE_INSERT_LINE, graphic_print_tree_node);
        dtype_ditem_detail("max heap");
        scan_list_print_format(input_buffer, "%s", input_name);
        if(compare_and_choose(4, input_buffer, "m", "p", "print", "mode")){
            do{scan_list_format("number(1) or integer buffer(2):", "%d", &print_mod);
            }while(print_mod != TREE_PRINT_MOD_BUF && print_mod != TREE_PRINT_MOD_NUM);
        }
        else if(compare_and_choose(1, input_buffer, "push")){
            tree_max_heap_push(head);
        }
        else if(compare_and_choose(1, input_buffer, "pop")){
            tree_max_heap_pop(head);
        }
        else if(compare_and_choose(1, input_buffer, "detail")){
            tree_detail(head);
        }
        else if(compare_and_choose(1, input_buffer, "clean")){
            trash_clean(&recently_trash, &recently_detail);
        }
        MENU_ENDIF
    }
    graphic_refresh();
    list_refresh();
}

void tree_max_heap_push(struct data_node *head){
    int buff_num;
    if(head->node[head->number]){
        print_list_boardcast_format(1, "The heap is full.");
        return;
    }
    int num;
    for(num = 1;num <= head->number;num++) if(head->node[num] == NULL) break;
    while(CONTINUE){
        buff_num = max_buff_num + 1;
        list_input_print("integer buffer (must below %td):%id", max_buff_num, &buff_num);
        if(buff_num <= max_buff_num) break;
        print_list_boardcast_format(1, "integer buffer input error.");
    }
    scan_list_print_format(input_buffer, "buffer:");
    struct data_node* input = NULL;
    data_node_append(input, input_buffer, buff_num, NULL, NULL, "max heap push malloc failed.");
    while((num != 1) && (buff_num > head->node[num/2]->number)){
        head->node[num] = head->node[num/2];
        if(num * 2 < head->number){
            head->node[num]->leftC = head->node[num*2];
            head->node[num]->rightC = head->node[num*2+1];
        }else{
            head->node[num]->leftC = NULL;
            head->node[num]->rightC = NULL;
        }
        num/=2;
    }
    head->node[num] = input;
    if(num * 2 < head->number){
        head->node[num]->leftC = head->node[num*2];
        head->node[num]->rightC = head->node[num*2+1];
    }else{
        head->node[num]->leftC = NULL;
        head->node[num]->rightC = NULL;
    }
    if(num == 1)
        head->link = head->node[num];
    else if(num % 2 == 0)
        head->node[num/2]->leftC = head->node[num];
    else
        head->node[(num-1)/2]->rightC = head->node[num];
}

void tree_max_heap_pop(struct data_node *head){
    if(head->node[1] == NULL){
        print_list_boardcast_format(1, "heap is null");
        return;
    }
    trash = ditem_trash_add(head->buffer, head->node[1], trash);
    int num;
    for(num = 1;num <= head->number;num++) if(head->node[num] == NULL) break; num--;
    if(num == 1){
        head->link = NULL;
        head->node[1] = NULL;
        return;
    }
    struct data_node *tem = head->node[num];
    head->node[num--] = NULL;
    int parent = 1;
    int child = 2;
    while(child <= num){
        if(child < num && head->node[child]->number < head->node[child+1]->number) child++;
        if(tem->number > head->node[child]->number) break;
        head->node[parent] = head->node[child];
        parent = child;
        child*=2;
    }
    head->node[parent] = tem;
    int i;
    for(i = 1;i < pow(2,head->node[0]->tree_level - 1);i++){
        if(head->node[i]){
            head->node[i]->leftC = head->node[i*2];
            head->node[i]->rightC = head->node[i*2+1];
        }
    }
    for(;i < head->number;i++){
        if(head->node[i]){
            head->node[i]->leftC = NULL;
            head->node[i]->rightC = NULL;
        }
    }
    head->link = head->node[1];
}

void tree_min_heap(struct data_node *head){
    TREE_TYPE_INITIAL
    static int print_mod = TREE_PRINT_MOD_BUF;
    print_title("min heap");
    menu_print_max_heap();
    while(CONTINUE){
        graphic_print_tree(head, print_mod, TREE_INSERT_LINE, graphic_print_tree_node);
        dtype_ditem_detail("min heap");
        scan_list_print_format(input_buffer, "%s", input_name);
        if(compare_and_choose(4, input_buffer, "m", "p", "print", "mode")){
            do{scan_list_format("number(1) or integer buffer(2):", "%d", &print_mod);
            }while(print_mod != TREE_PRINT_MOD_BUF && print_mod != TREE_PRINT_MOD_NUM);
        }
        else if(compare_and_choose(1, input_buffer, "push")){
            tree_min_heap_push(head);
        }
        else if(compare_and_choose(1, input_buffer, "pop")){
            tree_min_heap_pop(head);
        }
        else if(compare_and_choose(1, input_buffer, "detail")){
            tree_detail(head);
        }
        else if(compare_and_choose(1, input_buffer, "clean")){
            trash_clean(&recently_trash, &recently_detail);
        }
        MENU_ENDIF
    }
    graphic_refresh();
    list_refresh();
}

void tree_min_heap_push(struct data_node *head){
    int buff_num;
    if(head->node[head->number]){
        print_list_boardcast_format(1, "The heap is full.");
        return;
    }
    int num;
    for(num = 1;num <= head->number;num++) if(head->node[num] == NULL) break;
    while(CONTINUE){
        buff_num = max_buff_num + 1;
        list_input_print("integer buffer (must below %td):%id", max_buff_num, &buff_num);
        if(buff_num <= max_buff_num) break;
        print_list_boardcast_format(1, "integer buffer input error.");
    }
    scan_list_print_format(input_buffer, "buffer:");
    struct data_node* input = NULL;
    data_node_append(input, input_buffer, buff_num, NULL, NULL, "min heap push malloc failed.");
    while((num != 1) && (buff_num < head->node[num/2]->number)){
        head->node[num] = head->node[num/2];
        if(num * 2 < head->number){
            head->node[num]->leftC = head->node[num*2];
            head->node[num]->rightC = head->node[num*2+1];
        }else{
            head->node[num]->leftC = NULL;
            head->node[num]->rightC = NULL;
        }
        num/=2;
    }
    head->node[num] = input;
    if(num * 2 < head->number){
        head->node[num]->leftC = head->node[num*2];
        head->node[num]->rightC = head->node[num*2+1];
    }else{
        head->node[num]->leftC = NULL;
        head->node[num]->rightC = NULL;
    }
    if(num == 1)
        head->link = head->node[num];
    else if(num % 2 == 0)
        head->node[num/2]->leftC = head->node[num];
    else
        head->node[(num-1)/2]->rightC = head->node[num];
}

void tree_min_heap_pop(struct data_node *head){
    if(head->node[1] == NULL){
        print_list_boardcast_format(1, "heap is null");
        return;
    }
    trash = ditem_trash_add(head->buffer, head->node[1], trash);
    int num;
    for(num = 1;num <= head->number;num++) if(head->node[num] == NULL) break; num--;
    if(num == 1){
        head->link = NULL;
        head->node[1] = NULL;
        return;
    }
    struct data_node *tem = head->node[num];
    head->node[num--] = NULL;
    int parent = 1;
    int child = 2;
    while(child <= num){
        if(child < num && head->node[child]->number > head->node[child+1]->number) child++;
        if(tem->number < head->node[child]->number) break;
        head->node[parent] = head->node[child];
        parent = child;
        child*=2;
    }
    head->node[parent] = tem;
    int i;
    for(i = 1;i < pow(2,head->node[0]->tree_level - 1);i++){
        if(head->node[i]){
            head->node[i]->leftC = head->node[i*2];
            head->node[i]->rightC = head->node[i*2+1];
        }
    }
    for(;i < head->number;i++){
        if(head->node[i]){
            head->node[i]->leftC = NULL;
            head->node[i]->rightC = NULL;
        }
    }
    head->link = head->node[1];
}


