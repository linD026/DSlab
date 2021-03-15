/*
 *  Last update 2021/1/31
 *  included at dstructure.h,
 *              dwindow.c, data_type_list.c, dstack.c, dtree.c
 *
 */

#ifndef stdlen_h
#define stdlen_h

#define STATIC_DISPLAY_SIZE 50

#define METADATA_BUFFER_SIZE 24

#define GET_DIGIT_LEN(DIGIT) (floor(log10(abs(DIGIT))) + 1)

//set at menu.c
extern int input_buffer_size;

#endif /* stdlen_h */
