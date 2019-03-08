#ifndef SCANNER_H
#define SCANNER_H
#include "database.h"
/*parse given stream in to raw list, create symbol table from the parsed list.
input:  -   addresses of two empty linked lists. one for the parsed data and the other 
            for the symbol table. 
        -   input stream.*/
void initial_scan(list_t *symbol_list, list_t *list, FILE *fp);
/*preform the final scan on the given parsed data.
input:  - linked list containing the parsed user input
        - a symbol list containing labels
the function creates the required output files to the current directory*/
void final_scan(list_t, list_t);
#endif