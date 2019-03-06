#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H
#include "database.h"
/*print generic list to stream.
the type of the node is passed as an enum entry.
types supported:    - DATA_T = 0
                    - SYMBOL_T = 1
                    - INT_BIN_T = 2
                    - EXTERNAL_T = 3*/
void fprint_list(FILE *, list_t, int);
/*the following function gets a pointer to a list. the function frees it and all of its nodes. 
arg are the address of the list and the data type of its nodes.*/
void list_free(list_t *, int);
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then pushed into the start of the list within a new node.
if successful, the address of the node is returned, else NULL is returned.*/
node_t *list_push(list_t *, void *);
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then enqueued into the end of the list within a new node.
if successful, the address of the node is returned, else NULL is returned.*/
node_t *list_enqueue(list_t *, void *);
/*initilize linked list with NULL.
input - address of a list.*/
void initilize_list(list_t *, unsigned int);
/*this function receives a pointer to a list and a label to search in the given list.
if the label exists, its node address is return to the caller. otherwise NULL is returned.*/
void *search_label(list_t *, char *);
/*chain given lists.*/
void chain_lists(list_t *, list_t *);
/*output given list into file with the given extention.
input:  - linked list
        - type of the list
        - desired extension*/
void list_to_file(list_t, int, char *);
#endif