//
//  dstack.h
//  DS_project
//
//  Created by lin zhien on 2021/2/5.
//

#ifndef dstack_h
#define dstack_h

#include "dstructure.h"

/*
 * window function
 */
void menu_print_main_stack(void);
void menu_print_stack(void);
void graphic_print_stack(struct data_node *head);

/*
 * stack function
 */
struct metadata* stack_menu(struct metadata *dlist);
void stack(struct data_node *head);
struct data_node* stack_push(struct data_node *head, char *buffer);
struct data_node* stack_pop(struct data_node *head);

/*
 * set up recently_trash and recently_stack_detail
 * print out all the detail (STD and normal) and trash.
 * 2021/2/3 new marco dtype_ditem_detail("stack");
 */
void stack_ditem_detail(struct data_node *head);

#endif /* dstack_h */
