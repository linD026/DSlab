#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "menuio.h"
#include "stdlen.h"
#include "dstructure.h"
#include "dgraph.h"
#include "dwindow.h"

/*  recently_trash
 *  recently_detail
 *  detail
 */
static short int first_time_get_in = CONTINUE;
static struct metadata *dgraph = NULL;
static struct ditem_graph **current_node = NULL;
extern char *input_buffer;

struct metadata* graph_menu(struct metadata *dlist){
    print_title("graph menu");
    GRAPH_CHECK_FIRST_TIME_GET_IN(graph, 0);
    while(CONTINUE){
        scan_list_print_format(input_buffer, "graph>");
        if(compare_and_choose(2, input_buffer, "c", "create")){
            print_graphic(2, "hello.\ntest\n");
        }
        else if(compare_and_choose(1, input_buffer, "layout")){
            
        }
        MENU_ENDIF
    }
    return dlist;
}
