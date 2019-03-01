#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "error.h"
#include "scanner.h"
#include "data_structure.h"
char *file_name = NULL; /*string to contain path filename*/
int ln_cnt = 1;         /*line counter.*/
int DC = 0;             /*data counter.*/
int IC = 100;           /*instruction counter.*/
int main(int argc, char **argv)
{
    list_t list;        /* linked list to contain the prased data.*/
    list_t symbol_list; /* linked list to contain the the symbol table.*/
    while (*++argv)
    {
        FILE *fp;
        if (!(fp = fopen(*argv, "r")))
            printf("file <%s> does not exist.\n", *argv);
        else
        {
            file_name = path_fname_extract(*argv);
            puts(file_name);
            /*first scan*/
            initial_scan(&symbol_list, &list, fp);
            /*second scan*/
            printf("DC=%d. IC=%d\n", DC, IC);
            printf("error = %s\n", error() == TRUE ? "TRUE" : "FALSE");
            if ((list.tail) && (error() == FALSE))
            {
            }
            list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
            list_free(&list, DATA_T);          /*free the data list*/
            free(file_name);
            fclose(fp);
        } /*scanning block*/
    }     /*while (*argv) block.*/
    return 0;
}