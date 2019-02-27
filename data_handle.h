#ifndef DATA_HANDLE_h
#define DATA_HANDLE_h
#include "database.h"
/*print a list with given data type. the arguments are list and data type.*/
void list_print(list_t, int);
/*the following function gets a pointer to a list. the function frees it and all of its nodes. 
arg are the address of the list and the data type of its nodes.*/
void list_free(list_t *, int);
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then pushed into the start of the list within a new node.*/
void list_push(list_t *, void *);
/*the following function receives a pointer to a linked list and a generic data pointer.
the data is then enqueued into the end of the list within a new node.*/
void list_enqueue(list_t *, void *);
/*the following function creates a symbol node with the given label.
input is a label string, the function then returns to the caller the address of the dynamically allocated node.
the node is initialized to 0 in all other fields.*/
symbol_t *create_symbol_node(char *);
/*initilize linked list with NULL.
input - address of a list.*/
void initilize_list(list_t *);
/*this function receives a pointer to a list and a label to search in the given list.
if the label exists, its node address is return to the caller. otherwise NULL is returned.*/
symbol_t *search_label(list_t *, char *);
/*the following function inserts into the given symbol table the given data by its type - command, data, string or extern.
inputs  - symbol table address represented by a linked list
        - data_t node by value.
        - symbol_type as the "word" type.
this function returns none. */
void sym_table_insert(list_t *, data_t, symbol_type);
/*the following function updates data counter according the size of the data given.
DC is updated by the size of .data array, or by the length of the .string.
input : - data_t as the data extracted from the current line.
        - symbol_type enum that describes the given data type -> .data/.string*/
void update_DC(data_t, symbol_type);
/*the following function builds a symbol entry by the given data.
the data is analyezed for its type -> instruction or command line.
then checked for errors, or if exists in the table. 
if the given data is valid, sym_table_insert is used to enqueue with the corresponding parameters.
input:  - the address of the symbol list
        - the address of the data_t as extracted from the current line.
output = none*/
void build_symbol(list_t *, data_t *);
/*the following function updates the given data entries 
with the final IC count.
input   - address of the symbol list
output  - none*/
void symbol_table_add_IC(list_t *);
#endif