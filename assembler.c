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
int IC = STR_ADDRESS;   /*instruction counter.*/
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
            /*first scan*/
            initial_scan(&symbol_list, &parsed_list, fp);
            /*second scan*/
            /*printf("DC=%d. IC=%d: %d\n", DC, IC, ln_cnt);
            printf("error = %s\n", error() == TRUE ? "TRUE" : "FALSE");*/
            if ((parsed_list.head) && (error() == FALSE)) /*check if data exists and encountered no errors*/
            {
                instruction_list = bin_translate(parsed_list, symbol_list, &external_list);
                if (error() == FALSE)
                {
                    char *file_with_ext;
                    FILE *fp = NULL;
                    list_t *entry_list = NULL;
                    if (instruction_list->head)
                    {
                        file_with_ext = strcat_new(file_name, ".ob");
                        if (file_with_ext)
                        {
                            fp = fopen(file_with_ext, "w");
                            fprintf(fp, "%d %d\n", IC - STR_ADDRESS, DC);
                            fprint_list(fp, *instruction_list, BASE64_P);
                            fclose(fp);
                            free(file_with_ext);
                            fp = NULL;
                            file_with_ext = NULL;
                        }
                    }
                    if (external_list->head) /*if external list isnt empty, create .ext file*/
                    {
                        file_with_ext = strcat_new(file_name, ".ext");
                        if (file_with_ext)
                        {
                            fp = fopen(file_with_ext, "w");
                            fprint_list(fp, *external_list, EXTERNAL_T);
                            fclose(fp);
                            free(file_with_ext);
                            fp = NULL;
                            file_with_ext = NULL;
                        }
                    }
                    if ((entry_list = create_entry_list(symbol_list))) /*if entry declared, create .ent file*/
                    {
                        file_with_ext = strcat_new(file_name, ".ent");
                        if (file_with_ext)
                        {
                            fp = fopen(file_with_ext, "w");
                            fprint_list(fp, *entry_list, ENTRT_P);
                            fclose(fp);
                            list_free(entry_list, SYMBOL_T); /*free symbol table.*/
                            free(entry_list);
                            free(file_with_ext);
                            fp = NULL;
                            file_with_ext = NULL;
                        }
                    }
                }
                list_free(instruction_list, INT_BIN_T);
                free(instruction_list);
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