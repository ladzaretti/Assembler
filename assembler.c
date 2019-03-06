#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "error.h"
#include "scanner.h"
#include "data_structure.h"
char *file_name = NULL; /*string to contain path filename*/
int ln_cnt = 1;         /*line counter.*/
int DC = 0;             /*data counter.*/
int IC = STR_ADDRESS;   /*instruction counter.*/
int main(int argc, char **argv)
{
    list_t parsed_list; /* linked list to contain the prased data.*/
    list_t symbol_list; /* linked list to contain the the symbol table.*/
    while (*++argv)
    {
        FILE *fp;
        file_name = NULL;
        reset_error();                            /*reset error flag before scanning new input file*/
        if (!(fp = dyn_fopen(*argv, &file_name))) /*dynamicaly open input file*/
            printf("file <%s> does not exist.\n", file_name);
        else
        {
            /*first scan*/
            initial_scan(&symbol_list, &parsed_list, fp);
            if ((parsed_list.head) && (error() == FALSE)) /*check if data exists and encountered no errors*/
                final_scan(parsed_list, symbol_list);     /*second scan*/
            /*free lists*/
            list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
            list_free(&parsed_list, DATA_T);   /*free the data list*/
            free(file_name);
        } /*scanning block*/
    }     /*while (*argv) block.*/
    return 0;
}