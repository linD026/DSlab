/*
 *  Last update 2021/1/31
 *  included at stdlen.h, dwindow.h
 *              menuio.c, dwindow.c, data_dtype_list.c, dstack.c, dtree.c
 *
 */

#ifndef dstructure_h
#define dstructure_h

#include "stdlen.h"

#define HEAD_INPUT_NAME_SET(HEAD_BUFFER) \
int input_name_len = (int)strlen(HEAD_BUFFER)+1 + 2;\
char input_name[input_name_len];\
for(int i = 0;i < input_name_len;i++) input_name[i] = '\0';\
strncat(input_name, head->buffer, input_buffer_size);\
strncat(input_name, ">", 2);

/*
 *  varible set:
 *  1. first_time_get_in      : check that the first help is printed
 *  2. d##DTYPE               : dlist -> dtype
 *  3. current_node           : dtype -> head (which we want to get) NOTE: it's PTP
 *  4. trash                  : maintain the all the trash of dtype (dlist like)
 *  5. recently_##DTYPE_trash : let code more efficiency to not search again
 *  6. input_buffer           : all the scan will put it (stack is only use on display->list)
 */
#define INITIAL_DTYPE_SET_UP(DTYPE)\
static short int first_time_get_in = CONTINUE;\
static struct metadata *d##DTYPE = NULL;\
static struct data_node **current_node = NULL;\
static struct dnode_trash *trash = NULL;\
static struct dnode_trash *recently_trash = NULL;\
static struct dnode_detail *recently_detail = NULL;\
extern char *input_buffer;\
static struct dnode_detail *detail = NULL;\

#define CHECK_FIRST_TIME_GET_IN(DTYPE, ADD_FUNC)\
    if(first_time_get_in){\
        int det = 0;\
        print_graphic_format(2, "get in %s\nsearching %s node in dlist\n", #DTYPE, #DTYPE);\
        d##DTYPE = metadata_search(dlist, #DTYPE);\
        if(d##DTYPE == NULL) {print_graphic(1, "search set failed\n");det++;}\
        print_graphic(1, "search finished\n");\
        print_graphic(1, "setting detail\n");\
        current_node = (struct data_node**)malloc(sizeof(**current_node));\
        if(current_node == NULL){ print_graphic(1, "detail set failed\n");det++;}\
        print_graphic(1, "set detail finished\n");\
        det = det + ADD_FUNC;\
        if(det == 0){first_time_get_in = TERMINATED;\
            print_graphic_format(1, "welcome to %s menu page\n", #DTYPE);\
        }\
        else{\
            free(d##DTYPE); d##DTYPE == NULL; free(current_node); current_node = NULL;\
        }\
    }

#define GRAPH_CHECK_FIRST_TIME_GET_IN(DTYPE, ADD_FUNC)\
    if(first_time_get_in){\
        int det = 0;\
        print_graphic_format(2, "get in %s\nsearching %s node in dlist\n", #DTYPE, #DTYPE);\
        d##DTYPE = metadata_search(dlist, #DTYPE);\
        if(d##DTYPE == NULL) {print_graphic(1, "search set failed\n");det++;}\
        print_graphic(1, "search finished\n");\
        print_graphic(1, "setting detail\n");\
        current_node = (struct ditem_graph**)malloc(sizeof(**current_node));\
        if(current_node == NULL){ print_graphic(1, "detail set failed\n");det++;}\
        print_graphic(1, "set detail finished\n");\
        det = det + ADD_FUNC;\
        if(det == 0){first_time_get_in = TERMINATED;\
            print_graphic_format(1, "welcome to %s menu page\n", #DTYPE);\
        }\
        else{\
            free(d##DTYPE); d##DTYPE == NULL; free(current_node); current_node = NULL;\
        }\
    }

#define dtype_ditem_detail(DTYPE) do{\
    if(recently_trash == NULL || strncmp(recently_trash->name, head->buffer, input_buffer_size) != 0){\
        recently_trash = trash;\
        while(recently_trash){\
            if(strncmp(recently_trash->name, head->buffer, input_buffer_size) == 0)\
                break;\
            recently_trash = recently_trash->next;\
        }\
    }\
    if(recently_detail == NULL || strncmp(recently_detail->name, head->buffer, input_buffer_size) != 0){\
        recently_detail = detail;\
        while(recently_detail){\
            if(strncmp(recently_detail->name, head->buffer, input_buffer_size) == 0)\
                break;\
            recently_detail = recently_detail->next;\
        }\
    }\
    list_refresh();\
    print_std_detail( DTYPE, head, recently_trash);\
    print_item_detail(recently_detail);\
    graphic_print_trash(recently_trash);}while(0)\

#define data_node_append(PRE_NODE, BUFFER, NUMBER, NEXT, LINK, ACT) do{\
    struct data_node *tem = (struct data_node*)malloc(sizeof(*tem));\
    if(!tem) print_graphic_format(1, "malloc node (%s) failed.\n", ACT);\
    tem->buffer = (char*)malloc(sizeof(char)*input_buffer_size);\
    if(!tem->buffer) print_graphic_format(1, "malloc node's buffer (%s) failed.\n", ACT);\
    strncpy(tem->buffer, BUFFER, input_buffer_size);\
    tem->number = NUMBER;\
    tem->next = NEXT;\
    tem->link = LINK;\
    PRE_NODE = tem;}while(0)


#define IS_NODE 1
#define IS_TRASH 0

struct data_node{
    char *buffer;
    int number;
    struct data_node *link;
    struct data_node *next;
    struct data_node* node[];
};

struct metadata{
    char *data_type;
    int data_type_num;
    struct data_node *head;
    struct ditem_graph *graph_head;
    struct metadata *next;
};

struct dnode_trash{
    int trash_num;
    char *name;
    struct data_node *head;
    struct dnode_trash *next;
};

struct dnode_detail{
    char *name;
    struct data_node *node_item;
    struct data_node *trash_item;
    struct dnode_detail *next;
};

/*
 * dlist create the dtype
 */
struct metadata* metadata_create(struct metadata *dlist, char *data_type);

/*
 * dtype create the ditem
 */
struct metadata* ditem_create(struct metadata *dtype, char *name);
struct metadata* ditem_graph_create(struct metadata *dtype, char *name);

/*
 * ditem create dnode(head) connected noraml(ditem) with link, both child dnode connected with next, if already has it do nothing
 * using at tree:
 *             (dtype)   (ditem)
 *              dtree -> normal -> max heap ->...
 *                         |
 *                      dnode_1 -> head_0 -> child...       dnode_#->number = size_of_node(2^level equal to all level node sum)
 *                         |                                head_0->number  = tree_level
 *                      dnode_2 -> head_0 -> child...
 *
 * data_node's initial at data_node *ptr[16] when size_of_node > 16.
 */
struct data_node* dnode_head_create(struct data_node *ditem, char *name, int size_of_node);
/*
 * dlist search the dtype
 */
struct metadata* metadata_search(struct metadata *dlist, char *data_type);

/*
 * dtype create the ditem, pass dtype's first ditem. with next by next.
 */
struct data_node* dnode_next_search(struct data_node *ditem, char *name);

/*
 * ditem search the dnode. with link by link.
 */
struct data_node* dnode_link_buffer_search(struct data_node *head, char *buffer);
struct data_node* dnode_link_number_search(struct data_node *head, int number);
/*
 * ditem is we want delete, trash will in this function to find out wherther it have
 */
struct data_node* ditem_delete(struct data_node *ditem, struct dnode_trash **trash, char *name);

/*
 * ditem's self trash create at dnode_trash list.
 * does not search will create.
 */
struct dnode_trash* ditem_trash_add(char *name, struct data_node *ditem, struct dnode_trash *trash);

/*
 * ditem's trash clean up with checking detail have trash's thing or not.
 * pass recently_dtype_trash and recently_dtpe_detail address.
 */
void trash_clean(struct dnode_trash **dtype, struct dnode_detail **detail);

/*
 * search ditem's detail at detail list.
 * tagret can be in ditem or trash, det is check which one is it.
 * for example in dstask.c function stack:
 *  ditem :
 *      detail = detail_item_search(detail, head->buffer, dnode_link_search(head->link, input_buffer), IS_NODE);
 *  trash :
 *      detail = detail_item_search(detail, head->buffer, dnode_link_search(recently_trash->head, input_buffer), IS_TRASH);
 * does not find out the ditem detail's node then will create.
 */
struct dnode_detail* detail_item_search(struct dnode_detail *detail, char *buffer, struct data_node *target, int det);

#endif /* dstructure_h */
