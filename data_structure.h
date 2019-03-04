#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H
#include "database.h"
/*print a list with given data type. the arguments are list and data type.*/
void list_print(list_t, int);
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
void initilize_list(list_t *);
/*this function receives a pointer to a list and a label to search in the given list.
if the label exists, its node address is return to the caller. otherwise NULL is returned.*/
void *search_label(list_t *, char *);
/*chain given lists.*/
void chain_lists(list_t *, list_t *);
#endif