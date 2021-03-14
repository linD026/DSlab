#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlen.h"
#include "menuio.h"
#include "dstructure.h"
#include "dstack.h"
#include "dtree.h"
#include "dwindow.h"

/*
 *  search dlist, if it not any data_type that you want then create, otherwise do noting.
 *  using in menuio.c
 */
struct metadata* metadata_create(struct metadata *dlist, char *data_type){
    struct metadata **indirect = &dlist;
    for(;*indirect;indirect = &(*indirect)->next){
        if(strncmp((*indirect)->data_type, data_type, METADATA_BUFFER_SIZE) == 0) return dlist;
    }
    struct metadata *tem;
    tem = (struct metadata*)malloc(sizeof(*tem));
    if(tem == NULL) {print_graphic(1, " malloc dtype failed.\n"); return NULL;}
    tem->data_type = (char*)malloc(sizeof(char) * METADATA_BUFFER_SIZE);
    if(!tem->data_type) {print_graphic(1, " malloc dtype's data_type (dtype's name) failed.\n"); return dlist;}
    strncpy(tem->data_type, data_type, METADATA_BUFFER_SIZE);
    tem->data_type_num = 0;
    tem->head = NULL;
    tem->next = NULL;
    *indirect = tem;
    return dlist;
}

struct metadata* metadata_search(struct metadata *dlist, char *data_type){
    struct metadata **indirect = &dlist;
    for(;*indirect;indirect = &(*indirect)->next){
        if(strncmp((*indirect)->data_type, data_type, METADATA_BUFFER_SIZE) == 0) return (*indirect);
    }
    return NULL;
}

struct metadata* ditem_create(struct metadata *dtype, char *name){
    struct data_node **indirect = &dtype->head;
    for(;*indirect;indirect = &(*indirect)->next){
        if(strncmp((*indirect)->buffer, name, METADATA_BUFFER_SIZE) == 0) return dtype;
    }
    struct data_node *tem;
    tem = (struct data_node*)malloc(sizeof(*tem));
    if(!tem){print_graphic_format(1, "malloc ditem %s failed.\n", name); return dtype;}
    tem->buffer = (char*)malloc(sizeof(char)*input_buffer_size);
    if(!tem->buffer){print_graphic_format(1, "malloc ditem's dnode %s's buffer (name) failed.\n", name); return dtype;}
    strncpy(tem->buffer, name, input_buffer_size);
    tem->number = 0;
    tem->link = NULL;
    tem->next = NULL;
    *indirect = tem;
    dtype->data_type_num++;
    return dtype;
}

struct data_node* dnode_next_search(struct data_node* ditem, char *name){
    struct data_node **indirect = &ditem;
    for(;*indirect;indirect = &(*indirect)->next){
        if(strncmp((*indirect)->buffer, name, input_buffer_size) == 0) return (*indirect);
    }
    return NULL;
}

struct data_node* ditem_delete(struct data_node *ditem, struct dnode_trash **trash, char *name){
    if(ditem){
        struct data_node **indirect = &ditem;
        while(!compare_and_choose(1, (*indirect)->buffer, name)){
            indirect = &(*indirect)->next;
        }
        struct data_node *target = *indirect;
        *indirect = (*indirect)->next;
        free(target->buffer);
        target->buffer = NULL;
        target->number = 0;
        target->next = NULL;
        struct data_node *ptr = target->link;
        struct data_node *next = NULL;
        for(;ptr;ptr = next){
            free(ptr->buffer);
            ptr->buffer = NULL;
            ptr->number = 0;
            next = ptr->link;
            free(ptr);
        }
    }
    if(*trash){
        struct dnode_trash **indirect_t = &(*trash);
        while(!compare_and_choose(1, (*indirect_t)->name, name)){
            indirect_t = &(*indirect_t)->next;
        }
        trash_clean(indirect_t, NULL);
        free((*indirect_t)->name);
        (*indirect_t)->name = NULL;
        (*indirect_t)->trash_num = 0;
        *indirect_t = (*indirect_t)->next;
    }
    return ditem;
}


struct dnode_trash* ditem_trash_add(char *name, struct data_node *item, struct dnode_trash *trash){
    struct dnode_trash **ptr = &trash;
    item->link = NULL;
    for(;*ptr;ptr=&(*ptr)->next){
        if(strncmp(name, (*ptr)->name, input_buffer_size) == 0){
            (*ptr)->trash_num++;
            item->link = (*ptr)->head;
            (*ptr)->head = item;
            return trash;
        }
    }
    (*ptr) = (struct dnode_trash*)malloc(sizeof(struct dnode_trash));
    if(!(*ptr)){print_graphic_format(1, "malloc trash dnode %s failed.\n", name);return trash;}
    (*ptr)->name = (char*)malloc(sizeof(char)*input_buffer_size);
    if(!(*ptr)->name){print_graphic_format(1, "malloc trash dnode %s's name failed.\n", name);return trash;}
    strncpy((*ptr)->name, name, input_buffer_size);
    (*ptr)->trash_num = 1;
    (*ptr)->head = item;
    (*ptr)->next = NULL;
    return trash;
}

void trash_clean(struct dnode_trash **dtype, struct dnode_detail **detail){
    if(!(*dtype)) return;
    struct data_node *ptr = (*dtype)->head;
    (*dtype)->head = NULL;
    struct data_node *next = NULL;
    for(;ptr;ptr = next){
        if(*detail){
            if((*detail)->node_item == ptr) (*detail)->node_item = NULL;
            if((*detail)->trash_item == ptr) (*detail)->trash_item = NULL;
        }
        free(ptr->buffer);
        next = ptr->link;
        ptr->buffer = NULL;
        ptr->link = NULL;
        free(ptr);
    }
}

struct data_node* dnode_head_create(struct data_node *ditem, char *name, int size_of_node){
    struct data_node **indirect = &ditem->link;
    for(;*indirect;indirect = &(*indirect)->next){
        if(strncmp((*indirect)->buffer, name, input_buffer_size) == 0) return ditem;
    }
    struct data_node *tem;
    tem = (struct data_node*)malloc(sizeof(*tem) + sizeof(struct data_node*) * size_of_node);
    if(!tem){print_graphic_format(1, "malloc ditem %s failed.\n", name); return ditem;}
    tem->buffer = (char*)malloc(sizeof(char)*input_buffer_size);
    if(!tem->buffer){print_graphic_format(1, "malloc ditem's dnode %s's buffer (name) failed.\n", name); return ditem;}
    strncpy(tem->buffer, name, input_buffer_size);
    tem->number = size_of_node;
    tem->link = NULL;
    tem->next = NULL;
    for(int i = 0;i <= size_of_node;i++) tem->node[i] = NULL;
    *indirect = tem;
    ditem->number++;
    return ditem;
}

struct data_node* dnode_link_buffer_search(struct data_node *head, char *buffer){
    struct data_node **indirect = &head;
    for(;*indirect;indirect = &(*indirect)->link){
        if(strncmp((*indirect)->buffer, buffer, input_buffer_size) == 0) return (*indirect);
    }
    return NULL;
}

struct data_node* dnode_link_number_search(struct data_node *head, int number){
    struct data_node **indirect = &head;
    for(;*indirect;indirect = &(*indirect)->link){
        if((*indirect)->number == number) return (*indirect);
    }
    return NULL;
}

struct dnode_detail* detail_item_search(struct dnode_detail *detail, char *buffer, struct data_node *target, int det){
    if(target == NULL) return detail;
    struct dnode_detail **indirect = &detail;
    for(;(*indirect);indirect=&(*indirect)->next){
        if(compare_and_choose(1, buffer, (*indirect)->name)){
            if(det == IS_NODE){
                (*indirect)->node_item = target;
                return detail;
            }
            else{
                (*indirect)->trash_item = target;
                return detail;
            }
        }
    }
    (*indirect) = (struct dnode_detail*)malloc(sizeof(struct dnode_detail));
    if(!(*indirect)) { print_graphic(1, "malloc struct detail failed.\n");return detail;}
    (*indirect)->node_item = NULL;
    (*indirect)->trash_item = NULL;
    (*indirect)->next = NULL;
    (*indirect)->name = (char*)malloc(sizeof(char)*input_buffer_size);
    if(!(*indirect)->name) {print_graphic(1, "malloc struct detail's name failed.\n");return detail;}
    strncpy((*indirect)->name, buffer, input_buffer_size);
    if(det == IS_NODE){
        (*indirect)->node_item = target;
        return detail;
    }
    else{
        (*indirect)->trash_item = target;
        return detail;
    }
}
