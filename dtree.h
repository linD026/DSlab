//
//  dtree.h
//  DS_project
//
//  Created by lin zhien on 2021/2/5.
//

#ifndef dtree_h
#define dtree_h

#include "dstructure.h"

#define rightC link
#define leftC next
#define tree_level number

#define tree_node_left  4
#define tree_node_right 6

//                  "┴"
#define TREE_RL_OUT 0x2534
//                  "└"
#define TREE_R_OUT  0x2514
//                  "┘"
#define TREE_L_OUT  0x2518
//                  "┌"
#define TREE_R_IN   0x250c
//                  "┐"
#define TREE_L_IN   0x2510
//                  "─"
#define TREE_LINE   0x2500
//                  "star"
#define TREE_INSERT_LINE 0x2605

#define TREE_ACT_MODE_PRINT  0
#define TREE_ACT_MODE_INSERT 2

#define TREE_PRINT_MOD_NUM 1
#define TREE_PRINT_MOD_BUF 2

#define TREE_TYPE_INITIAL \
    graphic_refresh();\
    if(!head) {print_graphic_format(1, "does not found\n"); return;}\
    HEAD_INPUT_NAME_SET(head->buffer);

/*
 * window function
 */

/*
 * window graphic function
 */
void graphic_print_tree_type(void);
void graphic_print_tree(struct data_node *head, int print_mod, int func_mod,
                        void print_func(int print_mod, const int base_width, int node_pos_row, int node_pos_col, int current_line_len, struct data_node *node,  int node_num));
void graphic_print_tree_node(int print_mod, const int base_width, int node_pos_row, int node_pos_col, int current_line_len, struct data_node *node, int node_num);
void graphic_print_tree_insert_node(int print_mod, const int base_width, int node_pos_row, int node_pos_col, int current_line_len, struct data_node *node, int node_num);
void graphic_print_tree_line(int line, int start_row, int start_col, struct data_node *parent, int sight);

/*
 * window list function
 */

/*
 * window menu function
 */
void menu_print_main_tree(void);
void menu_print_normal_tree(void);
void menu_print_max_heap(void);


/*
 * tree function
 */
struct metadata* tree_menu(struct metadata *dlist);
int set_max_level(void);
int tree_select_type(char *buffer);
int tree_type_word_to_num(char *buffer);
void tree_get_in_type_menu(char *type, struct data_node *head);
struct data_node* tree_head_setup(struct data_node *ditem);
void tree_detail(struct data_node *head);

/*
 * tree normal_tree function
 */
void tree_normal(struct data_node *head);
void tree_normal_insert(struct data_node *head);
void tree_normal_replace(struct data_node *head);
void tree_normal_delete(struct data_node *head);

/*
 * tree max heap function
 */
void tree_max_heap(struct data_node *head);
void tree_max_heap_push(struct data_node *head);
void tree_max_heap_pop(struct data_node *head);


#endif /* dtree_h */
