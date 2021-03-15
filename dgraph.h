//
//  dgraph.h
//  dslab
//
//  Created by lin zhien on 2021/2/17.
//

#ifndef dgraph_h
#define dgraph_h

#include <stdio.h>
#include <stdlib.h>

#include "dstructure.h"
#include "dwindow.h"

#define max_number_node         25
#define maximal_temperature     256
#define thresh_tempertature     0
#define minimal_enegry          0
#define initial_enegry          0

#define dot_product(x1,y1,x2,y2) (x1 * x2 + y1 * y2)
#define vector_distance_x(node_v, node_w) (node_w->vector_x - node_v->vector_x)
#define vector_distance_y(node_v, node_w) (node_w->vector_y - node_v->vector_y)
#define vector_length(x,y) (sqrt(pow(x,2) - pow(y,2)))

#define Euclidean_distance(v,w) (sqrt(pow((w->vector_x - v->vector_x),2) + pow((w->vector_y - v->vector_y),2)))

#define Magnetic_field_parallel_vector_x 0
#define Magnetic_field_parallel_vector_y 1

#define angle_rotation_right 45 ~ -45
#define angle_rotation_left  135 ~ 225

/*  parameter:
 *  lambda
 *
 */
#define lambda_repulsive    0
#define lambda_attractive   0
#define lambda_gravity      0
#define lambda_magnetic     0

struct graph_force_repulsive{
    int vector_x;
    int vector_y;
};

struct graph_force_attractive{
    int vector_x;
    int vector_y;
};

#define barycenter_calculate(head, out_x, out_y) do{\
    int tem_x = 0; int tem_y = 0;\
    for(int i = 0;i < head->num;i++){\
        tem_x += head->vector[i]->vector_x;\
        tem_y += head->vector[i]->vector_y;\
    }\
    out_x = tem_x / head->num;\
    out_y = tem-y / head->num;\
    }while(0)

struct graph_force_gravity{
    int     vector_x;
    int     vector_y;
    int   barycenter;
};

/*v -> w (v,w)
 *                    dot_product(v, w)
 *  angle = arccose( ------------------- )
 *                        |v|*|w|
 */
#define Magnetic_field_parallel_angle(node_v, node_w, angle) do{\
        acos( dot_product(vector_distance_x(node_v, node_w), vector_distance_y(node_v, node_w), 0, 1) / (vector_length(0,1) * vector_length(vector_distance_x(node_v, node_w), vector_distance_y(node_v, node_w))))\
    }while(0)

#define Magnetic_parameter_c
struct graph_force_magnetic{
    int     vector_x;
    int     vector_y;
    int        angle;
};

struct graph_forces{
    struct graph_force_repulsive   repulsive;
    struct graph_force_attractive attractive;
    struct graph_force_gravity       gravity;
    struct graph_force_magnetic     magnetic;
    int vector_x;
    int vector_y;
};

struct graph_node_link{
    int                   number;
    struct graph_node_link* link;
};


#define degree(NODE) (NODE->predecessprs_num + NODE->successors_num)

struct graph_node{
    int                     integer_buffer;
    int                      string_buffer;
    int                           vector_x;
    int                           vector_y;
    int                   self_temperature;
    int                       last_impulse;
    int                         skew_gauge;
    struct graph_forces              force;
    int                   predecessors_num;
    struct graph_node_link*   predecessors;
    int                     successors_num;
    struct graph_node_link*     successors;
};

struct ditem_graph{
    struct ditem_graph*    next;
    char*                  name;
    int                 max_num;
    int                     num;
    int      global_temperature;
    int      temperature_thresh;
    int                  energy;
    int            overlappings;
    int                crossing;
    struct graph_node* vector[];
};


/*  ditem_graph_trash_add
 *  graph_trash_clean
 *  struct dnode_detail* detail_item_search(struct dnode_detail *detail, char *buffer, struct data_node *target, int det);
 */
struct dnode_graph_trash{
    int trash_num;
    char *name;
    struct graph_node *head;
    struct dnode_graph_trash *next;
};
struct dnode_graph_detail{
    char *name;
    struct graph_node *node_item;
    struct graph_node *trash_item;
    struct dnode_graph_detail *next;
};

/*
 * graph function
 */
struct metadata* graph_menu(struct metadata *dlist);
//struct metadata* metadata_graph_create(struct metadata *dlist, char *data_type);
//struct data_node* ditem_delete(struct data_node *ditem, struct dnode_trash **trash, char *name); NOT DONE YET
void graph_input(struct ditem_graph *head);
void graph_initial(struct ditem_graph *head);
void graph_layout(struct ditem_graph *head);
int graph_calculate_force(struct graph_node *node);
int graph_movement(struct ditem_graph *head, struct graph_node *node);
void graph_calculate_enegry(struct ditem_graph *head);

#endif /* dgraph_h */



