#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "parser.h"
#include "error_handle.h"
#include "data_handle.h"
char *file_name = NULL; /* string to contain path filename*/
int ln_cnt = 1;         /*line counter.*/
int err;                /*error flag.*/
int DC = 0;             /*data counter.*/
int IC = 100;           /*instruction counter.*/
int main(int argc, char **argv)
{
    list_t list;        /* linked list to contain the prased data.*/
    list_t symbol_list; /* linked list to contain the the symbol table.*/
    while (*++argv)
    {
        FILE *fp;
        char *line = NULL; /*string for a line from AS file*/
        ln_cnt = 1;        /*reset line counter before new file feed.*/
        IC = 100;
        DC = 0;
        initilize_list(&list);                 /*initialize a linked list*/
        initilize_list(&symbol_list);          /*initialize a linked list*/
        file_name = path_fname_extract(*argv); /*get file name without it's extension.*/
        /*first scan*/
        if (!(fp = fopen(*argv, "r")))
            printf("\n%s file does not exists.", *argv);
        else
        {
            data_t *pdata = NULL; /*pointer to store data from given line, will be used as the data section in the linked list.*/
            while (!feof(fp))
            {
                char *temp = NULL;              /*pointer the beginning of given line. for future free.*/
                line = NULL;                    /*initialize line pointer before each iteration*/
                fget_line(&line, fp);           /*get next line from given file.*/
                temp = line;                    /*save starting address.*/
                if (is_comment(&line, &ln_cnt)) /*check if the given line is a comment.*/
                    continue;
                if ((pdata = get_data(&line))) /*if there is data, i.e the line isn't empty.*/
                {
                    /*check if the given line label is valid, if ext/ent - label is ignored. if the line has label only the line is ignored.*/
                    if (!check_line_label(&pdata, &temp, &ln_cnt))
                        continue;
                    list_enqueue(&list, (void *)pdata); /*enqueue new data into linked list.*/
                    build_symbol(&symbol_list, pdata);  /*add to symbol list if valid.*/
                }                                       /*end of symbol table creation block.*/
                free(temp);                             /*free current line string.*/
                ln_cnt++;
            }                                  /*end of input stream.*/
            symbol_table_add_IC(&symbol_list); /*update address in symbol list according to IC. add IC to all addresses where external = FALSE and not a command.*/
            printf("error = %s\n", err == TRUE ? "TRUE" : "FALSE");
            list_print(symbol_list, SYMBOL_T); /*print symbol table*/
            list_print(list, DATA_T);          /*print list*/
        }
        /*second scan*/
        if ((list.tail) && (err == FALSE))
        {
        }
        list_free(&symbol_list, SYMBOL_T); /*free symbol table.*/
        list_free(&list, DATA_T);          /*free the data list*/
        free(file_name);
        fclose(fp);
    } /*while (*argv) block.*/
    return 0;
}