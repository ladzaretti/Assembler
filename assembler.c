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
    list_t parsed_list;              /* linked list to contain the prased data.*/
    list_t symbol_list;              /* linked list to contain the the symbol table.*/
    list_t *instruction_list = NULL; /* linked list to store the converted data.*/
    list_t *external_list = NULL;    /* linked list for storing the external variables references.*/
    while (*++argv)
    {
        FILE *fp;
        file_name = NULL;
        reset_error();
        if (!(fp = dy_fopen(*argv, &file_name)))
            printf("file <%s> does not exist.\n", file_name);
        else
        {
            puts(file_name);
            /*first scan*/
            initial_scan(&symbol_list, &parsed_list, fp);
            /*second scan*/
            printf("DC=%d. IC=%d\n", DC, IC);
            printf("error = %s\n", error() == TRUE ? "TRUE" : "FALSE");
            if ((parsed_list.head) && (error() == FALSE)) /*check if data exists and encountered no errors*/
            {
                instruction_list = bin_translate(parsed_list, symbol_list, &external_list);
                list_print(*instruction_list, BINARY_T);
                list_free(instruction_list, BINARY_T);
                free(instruction_list);
                list_print(*external_list, EXTERNAL_T);
                list_free(external_list, EXTERNAL_T);
                free(external_list);
            }
            list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
            list_free(&parsed_list, DATA_T);   /*free the data list*/
            free(file_name);
            fclose(fp);
        } /*scanning block*/
    }     /*while (*argv) block.*/
    return 0;
}