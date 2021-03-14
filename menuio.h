/*
 *  Last update 2021/1/31
 *  included at main.c, menuio.c, dwindow.c, data_dtype_list.c, dstack.c, dtree.c
 *
 */


#ifndef menuio_h
#define menuio_h

#include <stdarg.h>
#include <string.h>

#define TERMINATED 0
#define CONTINUE 1

#define FIRST_PRINT do{print_graphic(3, "Welcome to data Structure dynamic display project.\nPlease input \"help\" to look up how to do, \"exit\" or \"quit\" to exit.\nNOTE: DO NOT change terminal window size when using, thank you.\n");refresh_display();}while(0)

#define SET_INPUT_BUFFER(BUFFER, NUM)\
    do{NUM = set_input_buffer_size();\
    BUFFER = (char*)malloc(sizeof(char)*NUM);\
    if(!BUFFER) print_graphic(1, "malloc input buffer failed.\n");\
    for(int i = 0;i < NUM;i++) BUFFER[i] = '\0';}while(0)

#define TRANSER_SMALL_LETTET(BUFFER)\
for(int i = 0;i < strlen(BUFFER);i++){\
    if(isupper(BUFFER[i])) BUFFER[i] = tolower(BUFFER[i]);\
}

#define MENU_LIST_CHOOSE_EXIT compare_and_choose(3, input_buffer, "exit", "q", "quit")
#define MENU_LIST_CHOOSE_REFRESH compare_and_choose(2, input_buffer, "r", "refresh")

#define MENU_ENDIF \
    else if(MENU_LIST_CHOOSE_REFRESH){\
        refresh_display();\
    }\
    else if(MENU_LIST_CHOOSE_EXIT){\
        break;\
    }\
    else{\
        print_graphic(2, "ERROR\nPlease enter again.\n");\
    }

int welcome(void);
int compare_and_choose(const int num, const char *input_buffer, ...);

#endif /* menuio_h */
