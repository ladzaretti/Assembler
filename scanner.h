#ifndef SCANNER_H
#define SCANNER_H
#include "database.h"
/*the following function receives a path as string and extracts its filename. the function then returns the filename as a string.*/
char *path_fname_extract(char *);
/*the following function receives a string representing an integer number.
using strtod, the string is converted to a double. if the double has a decimal value other then zero, the convertion fails.
zero is returned. the vessel num doesnt changed.
if successful, 1 is return and num gets the whole part of the double.*/
int get_num(char *, int *);
/*the following function creates a symbol node with the given label.
input is a label string, the function then returns to the caller the address of the dynamically allocated node.
the node is initialized to 0 in all other fields.*/
symbol_t *create_symbol(char *);
/*the following function inserts into the given symbol table the given data by its type - command, data, string or extern.
inputs  - symbol table address represented by a linked list
        - data_t node by value.
        - symbol_type as the "word" type.
if successfull, new node address is return. if not null is returned.*/
symbol_t *insert_symbol(list_t *, data_t, symbol_type);
/*the following function calculates the size in memory of a .data array /.string type.
input : - data_t as the data extracted from the current line.
        - symbol_type enum that describes the given data type -> .data/.string
return values are:  - size in memory
                    - UNINITILIZED_DATA, NON_INT or UNINITILIZED_STRING on error*/
int data_size(data_t, symbol_type);
/*the following function builds a symbol entry by the given data.
the data is analyezed for its type -> instruction or command line.
then checked for errors, or if exists in the table. 
if the given data is valid, insert_symbol is used to enqueue with the corresponding parameters.
input:  - the address of the symbol list
        - the address of the data_t as extracted from the current line.
output = none*/
void build_symbol_type(list_t *, data_t *);
/*the following function updates the given data entries 
with the final IC count.
input   - address of the symbol list
output  - none*/
void symbol_table_add_IC(list_t *);
/**/
void initial_scan(list_t *symbol_list, list_t *list, FILE *fp);
#endif