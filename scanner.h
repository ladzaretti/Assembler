#ifndef SCANNER_H
#define SCANNER_H
#include "database.h"
void initial_scan(list_t *symbol_list, list_t *list, FILE *fp);
/*preform the final scan on the given parsed data.
input:  - linked list containing the parsed user input
        - a symbol list containing labels
the function creates the requierd output files to the current directory*/
void final_scan(list_t, list_t);
#endif