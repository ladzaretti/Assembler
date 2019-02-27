#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "parser.h"
#include "error_handle.h"
#include "data_handle.h"
int linec = 1; /*line counter.*/
int err;       /*error flag.*/
int DC = 0;    /*data counter.*/
int IC = 100;  /*instruction counter.*/
int main(int argc, char **argv)
{
    char *path_name = NULL; /*string to contain path filename*/
    list_t list;            /*initialize a linked list to contain the raw data after the parsing process.*/
    list_t symbol_list;     /*initialize a linked list to contain the the symbol table.*/
    while (*++argv)
    {
        FILE *fp;
        char *line = NULL; /*string for a line from AS file*/
        initilize_list(&list);
        initilize_list(&symbol_list);
        path_name = path_fname_extract(*argv);
        linec = 1; /*reset line counter before new file.*/
        /*first file scan*/
        if (!(fp = fopen(*argv, "r")))
            printf("\n%s file does not exists.", *argv);
        else
        {
            data_t *pdata = NULL; /*pointer to store data from given line, will be used as the data section in the linked list.*/
            while (!feof(fp))
            {
                char *temp = NULL;             /*pointer the beginning of given line. for future free.*/
                line = NULL;                   /*initialize line pointer before each iteration*/
                fget_line(&line, fp);          /*get next line from given file.*/
                temp = line;                   /*save starting address.*/
                if (is_comment(&line, &linec)) /*check if the given line is a comment.*/
                    continue;
                if ((pdata = get_data(&line))) /*if there is data, i.e the line isn't empty.*/
                {
                    /*check if the given line label is valid, 
                    if ext/ent - label is ignored. if the line has label only the line is ignored.*/
                    if (!check_line_label(&pdata, &temp, &linec))
                        continue;
                    list_enqueue(&list, (void *)pdata); /*enqueue new data into linked list.*/
                    err_invalid_cms((pdata->narg));     /*check arguments list for comma specific errors.*/
                                                        /*check if line is an instruction or a command.*/
                                                        /*the following switch cases will be executed only if there is a valid cmd/ins line. 
                        otherwise defualt case will be executed.*/
                    build_symbol(&symbol_list, pdata);
                }           /*end of symbol table creation block.*/
                free(temp); /*free current line string.*/
                linec++;
            } /*end of input stream.*/
              /*update address in symbol list according to IC. add IC to all addresses where external = FALSE and not a command.*/
            symbol_table_add_IC(&symbol_list);
            printf("error = %s\n", err == TRUE ? "TRUE" : "FALSE");
            list_print(symbol_list, SYMBOL_T); /*print symbol table*/
            list_print(list, DATA_T);          /*print list*/
            list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
            list_free(&list, DATA_T);          /*free the data list*/
            fclose(fp);
        }
        free(path_name);
    } /*while (*argv) block.*/
    return 0;
}